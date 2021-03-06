/*
 * UV Navigator - Auswertungsvisualisierung fuer Universum V
 * Copyright (C) 2004-2006 Daniel Roethlisberger <roe@chronator.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see http://www.gnu.org/copyleft/
 *
 * $Id$
 */

#include "video.h"

#include "si/conf.h"
#include "si/imagehandler.h"
#include "si/fonthandler.h"
#include "si/draw.h"
#include "si/events.h"
#include "util/version.h"
#include "util/sysdep.h"
#include "util/exceptions.h"

#include <iostream>
using namespace std;

/*
 * UVVideo - Verwaltet den SDL Bildschirm.
 */


/*
 * Konstruktor.
 */
UVVideo::UVVideo()
: screen(NULL)
{
	conf = UVConf::get_instance();

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		throw EXCEPTION(string("SDL Error: ") + SDL_GetError());
	}

	SDL_WM_SetCaption(TITLE, PACKAGE_NAME);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_EnableUNICODE(1);

	// TODO: rework into general config print facility
	if(conf->b_get("screen-quality")) {
		cout << "Hohe Bildqualitaet aktiviert, verwende Anti-Aliasing und Interpolation." << endl;
		cout << "Dies wird auf lahmen Maschinen zu merklich langsamerer Grafik-Ausgabe fuehren." << endl;
	} else {
		cout << "Optimale Geschwindigkeit aktiviert, kein Anti-Aliasing und Interpolation." << endl;
		cout << "Gefahr: UV Navigator kann in diesem Modus zu sofortiger Erblindung fuehren." << endl;
	}
	cout << "------------------------------------------------------------------------------" << endl;

	init();
}


/*
 * Destruktor.
 */
UVVideo::~UVVideo()
{
}


/*
 * Beendet alle SDL-Aktivitaeten und befreit entsprechende Ressourcen.
 */
void UVVideo::dispose()
{
	UVImageHandler::get_instance()->dispose();
	UVFontHandler::get_instance()->dispose();
	UVDraw::get_instance()->dispose();
	UVEvents::get_instance()->dispose();
	SDL_Quit();
}


/*
 * Video Mode initialisieren.
 *
 * Initialisiert den SDL-Video-Mode aufgrund der aktuellen
 * Einstellungen im globalen Konfigurationsobjekt conf neu.
 */
void UVVideo::init()
{
#ifdef DEBUG
	cout << "Angeforderte Bildschirmoptionen:";
#endif

	int flags = SDL_SWSURFACE;
	SDL_Rect r = { 0, 0, 0, 0 };

	if(conf->b_get("screen-fullscreen")) {
		sysdep_screensize(&r);
#ifdef DEBUG
		cout << " Fullscreen(" << r.w << "x" << r.h << "x?)";
#endif
		flags |= SDL_FULLSCREEN;
	} else {
		r.w = conf->l_get("screen-width");
		r.h = conf->l_get("screen-height");
#ifdef DEBUG
		cout << " Windowed(" << r.w << "x" << r.h << "x?)";
#endif
	}
	if(conf->b_get("screen-resizable")) {
#ifdef DEBUG
		cout << " Resizable";
#endif
		flags |= SDL_RESIZABLE;
	}
#ifdef DEBUG
	cout << "." << endl;
#endif

	screen = SDL_SetVideoMode(r.w, r.h, 0, flags);
	if(screen == NULL) {
		throw EXCEPTION(string("SDL: ") + SDL_GetError());
	}

#ifdef DEBUG
	cout << "Effektiv erhaltene Optionen:";
	if((screen->flags & SDL_FULLSCREEN) == SDL_FULLSCREEN) {
		cout << " Fullscreen(" << screen->w << "x" << screen->h << "x" << (screen->format->BytesPerPixel * 8) << ")";
	} else {
		cout << " Windowed(" << screen->w << "x" << screen->h << "x" << (screen->format->BytesPerPixel * 8) << ")";
	}
	if((screen->flags & SDL_RESIZABLE) == SDL_RESIZABLE) {
		cout << " Resizable";
	}
	cout << "." << endl;
	cout << "------------------------------------------------------------------------------" << endl;
#endif
	notify();
}


/*
 * Gibt SDL Screen zurueck.
 */
SDL_Surface* UVVideo::get_screen()
{
	return screen;
}


/*
 * Erzeugt eine neue SDL_Surface.
 */
SDL_Surface* UVVideo::create_surface(Uint32 flags, int width, int height)
{
	Uint32 rmask, gmask, bmask, amask;
	Uint16 bpp;
	if(flags & SDL_SRCALPHA) {
		bpp = 32;
		rmask = 0xFF000000;
		gmask = 0x00FF0000;
		bmask = 0x0000FF00;
		amask = 0x000000FF;
	} else {
		bpp = 24;
		rmask = 0x00FF0000;
		gmask = 0x0000FF00;
		bmask = 0x000000FF;
		amask = 0x00000000;
	}
	SDL_Surface *tmpsfc = SDL_CreateRGBSurface(flags, width, height, bpp,
		rmask, gmask, bmask, amask);
	SDL_Surface *sfc = SDL_DisplayFormatAlpha(tmpsfc);
	SDL_FreeSurface(tmpsfc);
	return sfc;
}


