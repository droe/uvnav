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

#include "navigator.h"

#include "../lib/version.h"
#include "../lib/exceptions.h"
#include "../lib/sysdep.h"
#include "../dm/importhandler.h"
#include "../si/imagehandler.h"
#include "../si/fonthandler.h"
#include "progress.h"

/*
 * UVNavigator - Hauptklasse von UVNav
 *
 * - Initialisiert SDL
 * - Koordination SDL <-> Objekte
 * - SDL Beispielcode zu Flags, DblBuf etc: testsprite.c
 * - Chef des physikalischen Koordinatensystems
 * - Keyboard-Handling
 *
 * TODO:
 * - Maus-Handling
 * - Fenstermanagment
 *   vector<UVFenster> in Anzeigereihenfolge
 *   von hinten durchackern
 */


/*
 * Konstruktor.
 *
 * Initialisiert automatisch die SDL Surface des Bildschirms.
 */
UVNavigator::UVNavigator()
: welt(NULL), map(NULL)
{
	conf = UVConf::get_instance();

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		throw EXCEPTION(string("SDL Error: ") + SDL_GetError());
	}

	init_video();

	SDL_WM_SetCaption(TITLE, PACKAGE_NAME);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_EnableUNICODE(1);

	if(conf->b_get("screen-quality"))
	{
		cout << "Hohe Bildqualitaet aktiviert, verwende Anti-Aliasing und Interpolation." << endl;
		cout << "Dies wird auf lahmen Maschinen zu merklich langsamerer Grafik-Ausgabe fuehren." << endl;
	}
	else
	{
		cout << "Optimale Geschwindigkeit aktiviert, kein Anti-Aliasing und Interpolation." << endl;
		cout << "Gefahr: UV Navigator kann in diesem Modus zu sofortiger Erblindung fuehren." << endl;
	}
	cout << "------------------------------------------------------------------------------" << endl;

	font_splash = UVFontHandler::get_instance()->get_font(FNT_SANS, screen->h / 32);
}


/*
 * Destruktor.
 *
 * Raeumt SDL mit allen Subsystemen auf.
 */
UVNavigator::~UVNavigator()
{
	if(TTF_WasInit())
		TTF_Quit();
	SDL_Quit();
}


/*
 * Video Mode initialisieren.
 *
 * Initialisiert den SDL-Video-Mode aufgrund der aktuellen
 * Einstellungen im globalen Konfigurationsobjekt conf neu.
 * Ueberschreibt die globale SDL_Surface.
 */
void UVNavigator::init_video()
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
 * Splash-Screen anzeigen.
 */
void UVNavigator::splash()
{
	SDL_Surface* img;
	SDL_Rect dst = { 0, 0, 0, 0 };

	UVImageHandler* imagehandler = UVImageHandler::get_instance();

	if(SDL_MUSTLOCK(screen))
	{
		if(SDL_LockSurface(screen) < 0)
		{
			throw EXCEPTION("Kann Bildschirm-Surface nicht reservieren!");
		}
	}

	// Bildschirm loeschen.
	dst.w = screen->w;
	dst.h = screen->h;
	SDL_FillRect(screen, &dst, SDL_MapRGB(screen->format, 0, 0, 0));

	dst.w = 0;
	dst.h = 0;
	// Universum V mitte oben, h / 12 unter rand
	img = imagehandler->get_surface(IMG_UNIVERSUM, 0, screen->h / 6);
	dst.x = screen->w / 2 - img->w / 2;
	dst.y = screen->h / 12;
	SDL_BlitSurface(img, 0, screen, &dst);

	// SDL powered mitte unten, h / 12 ueber rand
	img = imagehandler->get_surface(IMG_SDL_POWERED, 0, screen->h / 8);
	dst.x = screen->w / 2 - img->w / 2;
	dst.y = screen->h - screen->h / 12 - img->h;
	SDL_BlitSurface(img, 0, screen, &dst);

	// Titel
	SDL_Surface* line1 = font_splash->get_surface(TITLE " " PACKAGE_VERSION " (" + to_string(revision) + ")");
	dst.x = screen->w / 2 - line1->w / 2;
	dst.y = screen->h / 2 - line1->h / 2 * 3;
	SDL_BlitSurface(line1, 0, screen, &dst);

	// Copyright
	SDL_Surface* line2 = font_splash->get_surface(COPYRIGHT);
	dst.x = screen->w / 2 - line2->w / 2;
	dst.y = screen->h / 2 - line1->h / 2;
	SDL_BlitSurface(line2, 0, screen, &dst);

	status_y = screen->h / 2 + line1->h / 2 * 3;

	SDL_FreeSurface(line1);
	SDL_FreeSurface(line2);

	SDL_UpdateRect(screen, 0, 0, 0, 0);
	//SDL_UpdateRects(screen, nupdates, updates);
	// with SDL_Rect* updates

	if(SDL_MUSTLOCK(screen))
	{
		SDL_UnlockSurface(screen);
	}

}


/*
 * Splash-Screen-Status anzeigen.
 */
void UVNavigator::splash_status(const string& text)
{
	SDL_Surface* surface = font_splash->get_surface(text);
	SDL_Rect bounds = { 0, status_y, screen->w, surface->h };

	if(SDL_MUSTLOCK(screen))
	{
		if(SDL_LockSurface(screen) < 0)
		{
			throw EXCEPTION("Kann Bildschirm-Surface nicht reservieren!");
		}
	}

	SDL_FillRect(screen, &bounds, SDL_MapRGB(screen->format, 0, 0, 0));
	bounds.x = screen->w / 2 - surface->w / 2;
	SDL_BlitSurface(surface, 0, screen, &bounds);

	SDL_UpdateRect(screen, 0, 0, 0, 0);
	//SDL_UpdateRects(screen, nupdates, updates);
	// with SDL_Rect* updates

	if(SDL_MUSTLOCK(screen))
	{
		SDL_UnlockSurface(screen);
	}

	SDL_FreeSurface(surface);
}


/*
 * Auswertung laden.
 *
 * FIXME: Momentan nur eine Auswertung aufs Mal unterstuetzt...
 */
void UVNavigator::load(const string& file, int v)
{
	if(welt == NULL)
	{
		splash_status("Lade Auswertung: " + file);

		UVAbstractImporter* importer = UVImportHandler::get_instance()->get_importer(file);
		importer->set_verbosity(v);

		SDL_Rect dest;
		dest.x = screen->w / 16; dest.w = screen->w * 7 / 8;
		dest.y = screen->h * 5 / 8;  dest.h = screen->h / 16;
		UVProgress* progress = new UVProgress(screen, &dest);
		importer->attach(progress);
		welt = importer->import(file);
		importer->detach(progress);
		delete progress;
		conf->set_auswertung(welt->get_spieler()->name, welt->sternzeit);
	}
	else
	{
		throw EXCEPTION("Mehrere Auswertungen nicht implementiert!");
	}
}


/*
 * Warten auf Godot.
 *
 * Wartet bis der Benutzer eine Taste drueckt.
 */
void UVNavigator::wait()
{
	splash_status("Weiter mit beliebiger Taste!");

	SDL_Event event;
	bool waiting = true;
	while(waiting && SDL_WaitEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				waiting = false;
				break;
			case SDL_QUIT:
				throw EXCEPTION("Abgebrochen.");
		}
	}
}


/*
 * SDL Message Loop.
 */
void UVNavigator::run()
{
	SDL_Rect rect = { 0, 0, screen->w, screen->h };

	map = new UVMap(welt, screen);
	map->draw(&rect);

	SDL_Event event;
	bool running = true;
	while(running)
	{
		SDL_WaitEvent(&event);
		switch(event.type)
		{
			case SDL_VIDEORESIZE:
				conf->l_set("screen-width", event.resize.w);
				conf->l_set("screen-height", event.resize.h);
				init_video();
				map->resize(screen);
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
					case SDLK_q:
						running = false;
						break;

					case SDLK_f:
						conf->b_set("screen-fullscreen", !conf->b_get("screen-fullscreen"));
						init_video();
						map->resize(screen);
						break;

					case SDLK_s:
						map->toggle_sichtradien();
						break;
					case SDLK_k:
						map->toggle_kaufradien();
						break;

					case SDLK_LEFT:
						map->scroll(-screen->w / 16, 0);
						break;
					case SDLK_RIGHT:
						map->scroll(screen->w / 16, 0);
						break;
					case SDLK_UP:
						map->scroll(0, -screen->w / 16);
						break;
					case SDLK_DOWN:
						map->scroll(0, screen->w / 16);
						break;

					case SDLK_HOME:
						map->jump_eigene();
						break;
					case SDLK_END:
						map->jump_alle();
						break;

					case SDLK_PAGEUP:
						map->zoom_in();
						break;
					case SDLK_PAGEDOWN:
						map->zoom_out();
						break;

					case SDLK_1:
						map->set_dim(1);
						break;
					case SDLK_2:
						map->set_dim(2);
						break;
					case SDLK_3:
						map->set_dim(3);
						break;
					case SDLK_4:
						map->set_dim(4);
						break;
					case SDLK_5:
						map->set_dim(5);
						break;

					default:
						cout << "SDL_KEYDOWN: "
							 << SDL_GetKeyName(event.key.keysym.sym) << endl;
				}
				break;
			case SDL_QUIT:
				running = false;
		}
	}
	delete map;
}


/*
static void print_modifiers(void)
{
	int mod;
	printf(" modifiers:");
	mod = SDL_GetModState();
	if(!mod)
	{
		printf(" (none)");
		return;
	}
	if(mod & KMOD_LSHIFT)
		printf(" LSHIFT");
	if(mod & KMOD_RSHIFT)
		printf(" RSHIFT");
	if(mod & KMOD_LCTRL)
		printf(" LCTRL");
	if(mod & KMOD_RCTRL)
		printf(" RCTRL");
	if(mod & KMOD_LALT)
		printf(" LALT");
	if(mod & KMOD_RALT)
		printf(" RALT");
	if(mod & KMOD_LMETA)
		printf(" LMETA");
	if(mod & KMOD_RMETA)
		printf(" RMETA");
	if(mod & KMOD_NUM)
		printf(" NUM");
	if(mod & KMOD_CAPS)
		printf(" CAPS");
	if(mod & KMOD_MODE)
		printf(" MODE");
}

static void PrintKey(SDL_keysym *sym, int pressed)
{
	// Print the keycode, name and state
	if ( sym->sym )
	{
		printf("Key %s: %d-%s ", pressed ? "pressed" : "released", sym->sym, SDL_GetKeyName(sym->sym));
	} else {
		printf("Unknown Key (scancode = %d) %s ", sym->scancode, pressed ? "pressed" : "released");
	}

	// Print the translated character, if one exists
	if ( sym->unicode )
	{
		// Is it a control-character?
		if ( sym->unicode < ' ' )
		{
			printf(" (^%c)", sym->unicode+'@');
		} else {
#ifdef UNICODE
			printf(" (%c)", sym->unicode);
#else
			// This is a Latin-1 program, so only show 8-bits
			if ( !(sym->unicode & 0xFF00) )
				printf(" (%c)", sym->unicode);
#endif
		}
	}
	print_modifiers();
	printf("\n");
}
*/

