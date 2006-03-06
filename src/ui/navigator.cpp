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
#include "ui/core/window.h"
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
#include "si/draw.h"
#include "util/version.h"
#include "util/exceptions.h"
#include "util/sysdep.h"
#include "util/minmax.h"

#include <cmath>
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
, metering(false), meter_d(0), meter_x(0), meter_y(0)
, moving(false), move_x(0), move_y(0)
, mouse_x(0), mouse_y(0)
{
	conf = UVConf::get_instance();
	screen = UVVideo::get_instance()->get_screen();
	drw = UVDraw::get_instance();
	font_splash = UVFontHandler::get_instance()->get_font(FNT_SANS,
		screen->h / 32);
	font_meter = UVFontHandler::get_instance()->get_font(FNT_SANS,
		conf->l_get("map-label-font-size"));
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
 * Die Map und die Fenster neu zeichnen.
 * FIXME: Die Fenster duerfen nicht ueberlappen!
 */
void UVNavigator::vid_redraw(UVMap *&map, vector<UVWindow*> &windows)
{
	int nrects = 0;
	SDL_Rect *rects = new SDL_Rect[windows.size() + 1];

	LOCK(screen);
	if(map->dirty) {
		map->draw(screen);
		if(metering) {
			SDL_Rect dst = { mouse_x - (mouse_x - meter_x) / 2, mouse_y - (mouse_y - meter_y) / 2, 0, 0 };
			drw->line(screen, meter_x, meter_y, mouse_x, mouse_y, 0xFF, 0, 0, 0xFF);
			SDL_Surface *label = font_meter->get_surface(to_string(meter_d) + " KE", 0xFF, 0, 0);
			dst.x = max(label->w / 2, min(screen->w - label->w / 2, dst.x));
			dst.y = max(label->h / 2, min(screen->h - label->h / 2, dst.y));
			dst.x -= label->w / 2;
			dst.y -= label->h / 2;
			drw->box(screen, dst.x - label->h / 4, dst.y, dst.x + label->w + label->h / 4, dst.y + label->h, 0, 0, 0, 0xAA);
			SDL_BlitSurface(label, 0, screen, &dst);
		}
		for(vector<UVWindow*>::iterator i = windows.begin(); i != windows.end(); i++) {
			SDL_Rect *rect = &rects[nrects++];
			rect->x = 0;
			rect->y = 0;
			rect->w = screen->w;
			rect->h = screen->h;
			(*i)->dirty = true;
			(*i)->draw(screen);
		}
	} else {
		for(vector<UVWindow*>::iterator i = windows.begin(); i != windows.end(); i++) {
			if((*i)->dirty) {
				SDL_Rect *rect = &rects[nrects++];
				*rect = *(*i)->to_sdl_rect();
				map->draw(screen, rect);
				(*i)->draw(screen);
			}
		}
	}

	UNLOCK(screen);

	SDL_UpdateRects(screen, nrects, rects);
	delete [] rects;
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
 * Berechnet die Distanz zwischen zwei Koordinatenpunkten.
 * TODO: In Helper-Klasse verschieben.
 */
long UVNavigator::distance(long x1, long y1, long x2, long y2) const
{
	return long(sqrt(pow(double(x1 - x2), 2) + pow(double(y1 - y2), 2)));
}


/*
 * SDL Message Loop.
 */
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
Uint32 timer_callback(Uint32 interval)
{
	SDL_Event user_event;

	if(0 >= SDL_PeepEvents(&user_event, 1, SDL_PEEKEVENT, SDL_EVENTMASK(SDL_USEREVENT))) {
		user_event.type = SDL_USEREVENT;
		user_event.user.code = 2;
		user_event.user.data1 = NULL;
		user_event.user.data2 = NULL;
		SDL_PushEvent(&user_event);
	}

	return interval;
}

void UVNavigator::run()
{
	UVVideo *vid = UVVideo::get_instance();

	UVMap *map = new UVMap(universum);

	vector<UVWindow*> windows;

	UVLabel *title_label = new UVLabel(
		title_string(conf->l_get("map-dim", true)));
	UVWindow *title_overlay = new UVWindow(
		title_label, 40, 20, 0, 0, UVHARight, UVVATop, true);
	windows.push_back(title_overlay);

	UVLabel *status_label = new UVLabel("(?,?)");
	UVWindow *status_overlay = new UVWindow(
		status_label, 40, 20, 0, 0, UVHARight, UVVABottom, true);
	windows.push_back(status_overlay);

	vid_redraw(map, windows);

	SDL_Event event;
	bool running = true;
	long ticks = 0;
	metering = false;
	moving = false;
	long meter_origin_x = 0, meter_origin_y = 0;
	long move_origin_x = 0, move_origin_y = 0;
	SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
	SDL_SetTimer(20, timer_callback);
	while(running)
	{
		SDL_WaitEvent(&event);
		ticks = SDL_GetTicks();
		switch(event.type)
		{
		case SDL_VIDEORESIZE:
			conf->l_set("screen-width", event.resize.w);
			conf->l_set("screen-height", event.resize.h);
			vid->init();
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
				vid->init();
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
				meter_x -= -screen->w / 16;
				break;
			case SDLK_RIGHT:
				map->scroll(screen->w / 16, 0);
				meter_x -= screen->w / 16;
				break;
			case SDLK_UP:
				map->scroll(0, -screen->w / 16);
				meter_y -= -screen->w / 16;
				break;
			case SDLK_DOWN:
				map->scroll(0, screen->w / 16);
				meter_y -= screen->w / 16;
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
				break;
			case SDLK_2:
				map->set_dim(2);
				title_label->set_text(title_string(2));
				break;
			case SDLK_3:
				map->set_dim(3);
				title_label->set_text(title_string(3));
				break;
			case SDLK_4:
				map->set_dim(4);
				title_label->set_text(title_string(4));
				break;
			case SDLK_5:
				map->set_dim(5);
				title_label->set_text(title_string(5));
				break;

			default:
#ifdef DEBUG
				cerr << "Unhandled SDL_KEYDOWN: "
				     << SDL_GetKeyName(event.key.keysym.sym) << endl;
#endif
				break;
			}
			break;
		case SDL_QUIT:
			running = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch(event.button.button)
			{
			case 2:
				meter_x = event.button.x;
				meter_y = event.button.y;
				meter_origin_x = meter_x;
				meter_origin_y = meter_y;
				metering = true;
				break;
			case 1:
				// TODO: select object
			case 3:
				move_x = event.button.x;
				move_y = event.button.y;
				move_origin_x = move_x;
				move_origin_y = move_y;
				moving = true;
				break;
#ifdef DEBUG
			default:
				cerr << "Unhandled SDL_MOUSEBUTTONDOWN: " << long(event.button.button) << endl;
				break;
#endif
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch(event.button.button)
			{
			case 2:
				metering = false;
				if(meter_origin_x == event.button.x && meter_origin_y == event.button.y)
					map->dirty = true;
				break;
			case 1:
				// TODO: select object
			case 3:
				moving = false;
				if(move_origin_x == event.button.x && move_origin_y == event.button.y)
					map->scroll(event.button.x - screen->w/2, event.button.y - screen->h/2);
				break;
#ifdef DEBUG
			default:
				cerr << "Unhandled SDL_MOUSEBUTTONUP: " << long(event.button.button) << endl;
				break;
#endif
			}
			break;
		case SDL_USEREVENT:
			SDL_GetMouseState(&mouse_x, &mouse_y);
			{
				long mouse_vx = map->p2virt_x(mouse_x);
				long mouse_vy = map->p2virt_y(mouse_y);
				if(metering) {
					long meter_vx = map->p2virt_x(meter_x);
					long meter_vy = map->p2virt_y(meter_y);
					meter_d = distance(meter_vx, meter_vy, mouse_vx, mouse_vy);
					map->dirty = true;
				}
				if(moving) {
					map->scroll(move_x - mouse_x, move_y - mouse_y);
					meter_x -= move_x - mouse_x;
					meter_y -= move_y - mouse_y;
					move_x = mouse_x;
					move_y = mouse_y;
				}
				status_label->set_text("(" + to_string(mouse_vx) + "," + to_string(mouse_vy) + ")");
			}
			break;
		default:
			break;
		}

		vid_redraw(map, windows);
	}
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

