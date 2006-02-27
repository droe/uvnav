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

#include "navigator.h"

#include "ui/core/compositewidget.h"
#include "ui/core/label.h"
#include "ui/progress.h"
#include "ui/map.h"
#include "dm/abstractimporter.h"
#include "dm/importhandler.h"
#include "pd/universum.h"
#include "pd/spieler.h"
#include "si/imagehandler.h"
#include "si/fonthandler.h"
#include "si/video.h"
#include "si/conf.h"
#include "si/font.h"
#include "util/version.h"
#include "util/exceptions.h"
#include "util/sysdep.h"

#include <iostream>

using namespace std;

/*
 * UVNavigator - Hauptklasse von UVNav
 */


/*
 * Konstruktor.
 */
UVNavigator::UVNavigator()
: universum(NULL), spieler(NULL)
{
	conf = UVConf::get_instance();
	screen = UVVideo::get_instance()->get_screen();
	font_splash = UVFontHandler::get_instance()->get_font(FNT_SANS, screen->h / 32);
}


/*
 * Destruktor.
 */
UVNavigator::~UVNavigator()
{
	UVVideo::get_instance()->dispose();
}


/*
 * Splash-Screen anzeigen.
 */
void UVNavigator::splash()
{
	SDL_Surface* img;
	SDL_Rect dst = { 0, 0, 0, 0 };

	UVImageHandler* imagehandler = UVImageHandler::get_instance();

	LOCK(screen);

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

	UNLOCK(screen);
}


/*
 * Splash-Screen-Status anzeigen.
 */
void UVNavigator::splash_status(const string& text)
{
	SDL_Surface* surface = font_splash->get_surface(text);
	SDL_Rect bounds = { 0, status_y, screen->w, surface->h };

	LOCK(screen);

	SDL_FillRect(screen, &bounds, SDL_MapRGB(screen->format, 0, 0, 0));
	bounds.x = screen->w / 2 - surface->w / 2;
	SDL_BlitSurface(surface, 0, screen, &bounds);

	SDL_UpdateRect(screen, 0, 0, 0, 0);
	//SDL_UpdateRects(screen, nupdates, updates);
	// with SDL_Rect* updates

	UNLOCK(screen);

	SDL_FreeSurface(surface);
}


/*
 * Auswertung laden.
 *
 * FIXME: Momentan nur eine Auswertung aufs Mal unterstuetzt...
 */
void UVNavigator::load(const string& file, int v)
{
	if(universum == NULL)
	{
		splash_status("Lade Auswertung: " + file);

		UVAbstractImporter* importer = UVImportHandler::get_instance()->get_importer(file);
		importer->set_verbosity(v);

		SDL_Rect dest;
		dest.x = screen->w / 16; dest.w = screen->w * 7 / 8;
		dest.y = screen->h * 5 / 8;  dest.h = screen->h / 16;
		UVProgress* progress = new UVProgress(screen, &dest);
		importer->attach(progress);
		universum = importer->import(file);
		spieler = universum->get_spieler();
		importer->detach(progress);
		delete progress;
		conf->set_auswertung(universum->get_spieler()->name, universum->sternzeit);
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
				if(event.key.keysym.sym == SDLK_q)
					throw quit_application();
			case SDL_MOUSEBUTTONDOWN:
				waiting = false;
				break;
			case SDL_QUIT:
				throw quit_application();
		}
	}
}


/*
 * Reinitialisiert den Screen und die Map aufgrund geänderten Einstellungen.
 */
void UVNavigator::vid_reinit(UVMap *map, SDL_Surface *&mapsurface, SDL_Rect &rect)
{
	UVVideo *vid = UVVideo::get_instance();
	vid->init();
	screen = vid->get_screen();
	SDL_FreeSurface(mapsurface);
	mapsurface = vid->create_surface(
		SDL_SWSURFACE, screen->w, screen->h);
	map->resize(mapsurface);
	rect.w = screen->w;
	rect.h = screen->h;
}


/*
 * Die Map und die Fenster neu zeichnen.
 */
void UVNavigator::vid_redraw(SDL_Surface *&mapsurface, vector<UVWindow*> &windows, SDL_Rect &rect)
{
	LOCK(screen);
	SDL_BlitSurface(mapsurface, &rect, screen, &rect);
	for(vector<UVWindow*>::iterator i = windows.begin(); i != windows.end(); i++)
	{
		(*i)->draw(screen);
	}
	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
	UNLOCK(screen);
}


/*
 * Gibt den Text des Title-Overlay zurueck.
 */
string UVNavigator::title_string(int dim)
{
	return to_string(dim) + " - " + universum->get_dim(dim) + ", "
		+ spieler->name
		+ ((spieler->talent != "") ? " der " + spieler->talent : "")
		+ ", Sternzeit " + to_string(universum->sternzeit);
}


/*
 * SDL Message Loop.
 */
void UVNavigator::run()
{
	UVVideo *vid = UVVideo::get_instance();
	SDL_Rect dirty_rect = { 0, 0, screen->w, screen->h };
	vector<UVWindow*> windows;

	SDL_Surface *mapsurface = vid->create_surface(
		SDL_SWSURFACE, screen->w, screen->h);
	UVMap *map = new UVMap(universum, mapsurface);
	map->redraw();

	UVLabel *title_label = new UVLabel(
		title_string(conf->l_get("map-dim", true)));
	UVWindow *title_overlay = new UVWindow(
		title_label, 40, 20, 0, 0, UVHARight, UVVATop, true);
	windows.push_back(title_overlay);

	UVLabel *status_label = new UVLabel("(?,?)");
	UVWindow *status_overlay = new UVWindow(
		status_label, 40, 20, 0, 0, UVHARight, UVVABottom, true);
	windows.push_back(status_overlay);

/*
	UVCompositeWidget* cw1 = new UVCompositeWidget();
	UVCompositeWidget* cw2 = new UVCompositeWidget(2, UVOVertical);
	UVWidget* w1 = new UVWidget();
	UVWidget* w2 = new UVWidget();
	UVWidget* w3 = new UVWidget();
	UVWidget* w4 = new UVWidget();
	cw2->add_widget(w2);
	cw2->add_widget(w3);
	cw1->add_widget(w1);
	cw1->add_widget(cw2);
	cw1->add_widget(w4);
	UVWindow* win = new UVWindow(cw1, 40, 30, 400, 300);
	windows.push_back(win);
*/

	vid_redraw(mapsurface, windows, dirty_rect);

	SDL_Event event;
	bool running = true;
	bool dirty = false;
	long meter_ticks = 0;
	long ticks = 0;
	while(running)
	{
		dirty = true;
		dirty_rect.x = 0;
		dirty_rect.y = 0;
		dirty_rect.w = screen->w;
		dirty_rect.h = screen->h;

		SDL_WaitEvent(&event);
		ticks = SDL_GetTicks();
		switch(event.type)
		{
		case SDL_VIDEORESIZE:
			conf->l_set("screen-width", event.resize.w);
			conf->l_set("screen-height", event.resize.h);
			vid_reinit(map, mapsurface, dirty_rect);
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
				vid_reinit(map, mapsurface, dirty_rect);
				break;

			case SDLK_s:
				map->toggle_opt_sichtradien();
				break;
			case SDLK_k:
				map->toggle_opt_kaufradien();
				break;
			case SDLK_c:
				map->toggle_opt_container();
				break;
			case SDLK_x:
				map->toggle_opt_schiffe();
				break;
			case SDLK_v:
				map->toggle_opt_verbindungen();
				break;
			case SDLK_z:
				map->toggle_opt_zonen();
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
				title_label->set_text(title_string(1));
				title_overlay->resize();
				break;
			case SDLK_2:
				map->set_dim(2);
				title_label->set_text(title_string(2));
				title_overlay->resize();
				break;
			case SDLK_3:
				map->set_dim(3);
				title_label->set_text(title_string(3));
				title_overlay->resize();
				break;
			case SDLK_4:
				map->set_dim(4);
				title_label->set_text(title_string(4));
				title_overlay->resize();
				break;
			case SDLK_5:
				map->set_dim(5);
				title_label->set_text(title_string(5));
				title_overlay->resize();
				break;

			default:
#ifdef DEBUG
				cout << "SDL_KEYDOWN: "
				     << SDL_GetKeyName(event.key.keysym.sym) << endl;
#endif
				break;
			}
			break;
		case SDL_QUIT:
			running = false;
			break;
		case SDL_MOUSEMOTION:
			status_label->set_text("(" + to_string(map->p2virt_x(event.motion.x)) + "," + to_string(map->p2virt_y(event.motion.y)) + ")");
			status_overlay->resize();
			if(!meter_ticks)
				dirty_rect = *status_overlay->to_sdl_rect();
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch(event.button.button)
			{
			case 1:
				// TODO: select object
				map->scroll(event.button.x - screen->w/2, event.button.y - screen->h/2);
				break;
			case 2:
				// TODO: meter tool
				//meter_x = event.button.x;
				//meter_y = event.button.y;
				meter_ticks = ticks;
				dirty = false;
				break;
			case 3:
				map->scroll(event.button.x - screen->w/2, event.button.y - screen->h/2);
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch(event.button.button)
			{
			case 1:
				break;
			case 2:
				meter_ticks = 0;
				break;
			case 3:
				break;
			}
			dirty = false;
			break;
		default:
			dirty = false;
			break;
		}

		if(dirty)
			vid_redraw(mapsurface, windows, dirty_rect);
	}
	SDL_FreeSurface(mapsurface);
	delete map;
	delete title_overlay;
	delete status_overlay;
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

