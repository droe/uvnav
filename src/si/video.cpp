/*
 * UV Navigator - Auswertungsvisualisierung fuer Universum V
 * Copyright (C) 2004-2005 Daniel Roethlisberger <roe@chronator.ch>
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

#include "lib/sysdep.h"
#include "lib/exceptions.h"
#include "si/conf.h"

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
	init();
}


/*
 * Destruktor.
 */
/*
UVVideo::~UVVideo()
{
}
*/


/*
 * Video Mode initialisieren.
 *
 * Initialisiert den SDL-Video-Mode aufgrund der aktuellen
 * Einstellungen im globalen Konfigurationsobjekt conf neu.
 */
void UVVideo::init()
{
	int flags = SDL_SWSURFACE;

	cout << "Angeforderte Bildschirmoptionen:";
	SDL_Rect r = { 0, 0, 0, 0 };
	if(conf->b_get("screen-fullscreen"))
	{
		sysdep_screensize(&r);
		cout << " Fullscreen(" << r.w << "x" << r.h << "x?)";
		flags |= SDL_FULLSCREEN;
	}
	else
	{
		r.w = conf->l_get("screen-width");
		r.h = conf->l_get("screen-height");
		cout << " Windowed(" << r.w << "x" << r.h << "x?)";
	}
	if(conf->b_get("screen-resizable"))
	{
		cout << " Resizable";
		flags |= SDL_RESIZABLE;
	}
	cout << "." << endl;

	screen = SDL_SetVideoMode(r.w, r.h, 0, flags);
	if(screen == NULL)
	{
		throw EXCEPTION(string("SDL: ") + SDL_GetError());
	}

	cout << "Effektiv erhaltene Optionen:";
	if((screen->flags & SDL_FULLSCREEN) == SDL_FULLSCREEN)
	{
		cout << " Fullscreen(" << screen->w << "x" << screen->h << "x" << (screen->format->BytesPerPixel * 8) << ")";
	}
	else
	{
		cout << " Windowed(" << screen->w << "x" << screen->h << "x" << (screen->format->BytesPerPixel * 8) << ")";
	}
	if((screen->flags & SDL_RESIZABLE) == SDL_RESIZABLE)
	{
		cout << " Resizable";
	}
	cout << "." << endl;
	cout << "------------------------------------------------------------------------------" << endl;
}


/*
 * Gibt SDL Screen zurueck.
 */
SDL_Surface* UVVideo::get_screen()
{
	return screen;
}


