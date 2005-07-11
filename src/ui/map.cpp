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

#include "map.h"

#include "ui/core/widget.h"
#include "ui/core/compositewidget.h"
#include "ui/core/window.h"
#include "pd/universum.h"
#include "pd/spieler.h"
#include "pd/planet.h"
#include "pd/schiff.h"
#include "pd/anomalie.h"
#include "pd/container.h"
#include "pd/sensorsonde.h"
#include "pd/infosonde.h"
#include "pd/zone.h"
#include "si/font.h"
#include "si/draw.h"
#include "si/conf.h"
#include "si/fonthandler.h"
#include "si/imagehandler.h"
#include "util/version.h"
#include "util/exceptions.h"
#include "util/minmax.h"

using namespace std;

#define PI 3.14159265358979323846

//============================================================================

/*
 * UVMapSektor - Basissektor fuer effiziente Organisation der Objekte
 *
 * - Quadratischer Sektor fixer Groesse
 * - x, y (w, h fix vorgegeben)
 * - Verwaltet Planeten, Schiffe und Container im Bereich (x,y)-(x+w,y+h)
 *   in Vektoren
 */


/*
 * Konstruktor.
 */
/*
UVMapSektor::UVMapSektor()
{
}
*/


/*
 * Destruktor.
 */
/*
UVMapSektor::~UVMapSektor()
{
}
*/


//============================================================================

/*
 * UVMapQuadrant - Implementiert zweidimensionalen Flaechensuchbaum
 *
 * - x, y, ebene
 * - je 4 Subquadranten
 * - jeder Node zaehlt # Planeten, # Schiffe, # Container in Bereich
 */


/*
 * Konstruktor.
 */
/*
UVMapQuadrant::UVMapQuadrant()
{
}
*/


/*
 * Destruktor.
 */
/*
UVMapQuadrant::~UVMapQuadrant()
{
}
*/


//============================================================================

/*
 * UVMap - Darstellung der Universums-Karte.
 *
 * Zeichnet eine Dimension aufs Mal.
 *
 * Verwaltet das virtuelle Koordinatensystem.
 * Virtuelle Koordinaten sind in UV Karteneinheiten.
 * Zoomfaktor 10.0 == 10 virtuelle Karteneinheiten pro physikalischem Pixel.
 * Offset ist die Verschiebung des sichtbaren Fensters im virtuellen
 * Koordinatensystem, in Karteneinheiten. Oder anders gesagt die
 * Koordinaten der linken oberen Fensterecke in UV Karteneinheiten.
 *
 *    A -y
 *    �
 * -x � (0/0)     virtuelle Koordinaten des Universums in Karteneinheiten +x
 * <--+-------------------------------------------------------------------->
 *    �
 *    �                  (offset_x/offset_y)
 *    �                +---------------------------+
 *    �                � (0/0)                     �
 *    �                �                           �
 *    �                �    physikalische          �
 *    �                �    Bildschirm-            �
 *    �                �    Koordinaten            �
 *    �                �    (Pixel)                �
 *    �                �                           �
 *    �                �       (screen_w/screen_h) �
 *    �                +---------------------------+
 *    �                     (offset_x+screen_w*zoom/offset_y+screen_h*zoom)
 *    �
 *    V +y
 *
 * UVMap zeichnet UVUniversum, die Datenstruktur, welche von UVMap
 * iteriert und gezeichnet wird. UVUniversum hat lediglich eine passive Rolle
 * in der Bildschirmdarstellung.
 */


#define LABEL_RGB 0xDD,0xDD,0xDD


/*
 * Konstruktor.
 *
 * Waehlt sinnvollen Kartenausschnitt und Dimension falls alte Werte sinnlos,
 * d.h. wenn keine Daten sichtbar waeren.
 */
UVMap::UVMap(UVUniversum* u, SDL_Surface* s)
: universum(u), spieler(NULL), screen(s), phys(NULL)
, virt_x(0), virt_y(0), virt_w(0), virt_h(0)
, alle_x1(0), alle_y1(0), alle_x2(0), alle_y2(0)
, eigene_x1(0), eigene_y1(0), eigene_x2(0), eigene_y2(0)
{
	imagehandler = UVImageHandler::get_instance();

	screen_size.w = s->w;
	screen_size.h = s->h;

	spieler = universum->get_spieler();

	UVConf* conf = UVConf::get_instance();

	opt_sichtradien = conf->b_get("map-sichtradien", true);
	opt_kaufradien = conf->b_get("map-kaufradien", true);

	bool have_conf_data = conf->have_data();
	offset_x = conf->l_get("map-offset-x", true);
	offset_y = conf->l_get("map-offset-y", true);
	zoom = conf->f_get("map-zoom", true);
	dim = conf->l_get("map-dim", true);

	// Sinnvolle Dimension waehlen, falls keine Daten in dim.
	if(universum->get_dim(dim) == "")
	{
		for(dim_iterator iter = universum->first_dim(); iter != universum->last_dim(); iter++)
		{
			if((*iter).second != "")
			{
				dim = (*iter).first;
			}
		}
		if(universum->get_dim(dim) == "")
		{
			throw EXCEPTION("Keine Dimension hat Daten!");
		}
		else
		{
			conf->l_set("map-dim", dim, true);
		}
	}

	jump_init();
	if(!have_conf_data)
	{
		zoom = max(double(alle_x2 - alle_x1) / double(screen_size.w),
		           double(alle_y2 - alle_y1) / double(screen_size.h));
		offset_x = alle_x1 - (long(rint(double(screen_size.w) * zoom))
		                      - (alle_x2 - alle_x1)) / 2;
		offset_y = alle_y1 - (long(rint(double(screen_size.h) * zoom))
		                      - (alle_y2 - alle_y1)) / 2;
	}

	drw = UVDraw::get_instance();
	debug_font = UVFontHandler::get_instance()->get_font(FNT_SANS,
			conf->l_get("map-debug-font-size"));
	grid_font  = UVFontHandler::get_instance()->get_font(FNT_SANS,
			conf->l_get("map-grid-font-size"));
	label_font = UVFontHandler::get_instance()->get_font(FNT_SANS,
			conf->l_get("map-label-font-size"));
}


/*
 * Destruktor.
 */
UVMap::~UVMap()
{
	UVConf* conf = UVConf::get_instance();
	conf->l_set("map-offset-x", offset_x, true);
	conf->l_set("map-offset-y", offset_y, true);
	conf->f_set("map-zoom", zoom, true);
	conf->l_set("map-dim", dim, true);
	conf->b_set("map-sichtradien", opt_sichtradien, true);
	conf->b_set("map-kaufradien", opt_kaufradien, true);
}


/*
 * Benutzte Bereiche eigener/aller Objekte finden.
 */
void UVMap::jump_init()
{
	bool first_alle = true;
	bool first_eigene = true;

	// Ueber alle Objekte loopen.
	for(planeten_iterator iter = universum->first_planet(); iter != universum->last_planet(); iter++)
	{
		UVPlanet* p = (*iter).second;
		if(p->dim == dim)
		{
			bool ist_eigener = false;
			if(p->besitzer == spieler->name)
			{
				ist_eigener = true;
			}
			else
			{
				for(int i = 1; (i < p->max_zone()) && (!ist_eigener); i++)
				{
					if(p->get_zone(i) && (p->get_zone(i)->besitzer == spieler->name))
					{
						ist_eigener = true;
					}
				}
			}
			if(ist_eigener)
			{
				if(first_eigene)
				{
					eigene_x1 = p->x;
					eigene_y1 = p->y;
					eigene_x2 = p->x;
					eigene_y2 = p->y;
					first_eigene = false;
				}
				else
				{
					eigene_x1 = min(p->x, eigene_x1);
					eigene_y1 = min(p->y, eigene_y1);
					eigene_x2 = max(p->x, eigene_x2);
					eigene_y2 = max(p->y, eigene_y2);
				}
			}
			if(first_alle)
			{
				alle_x1 = p->x;
				alle_y1 = p->y;
				alle_x2 = p->x;
				alle_y2 = p->y;
				first_alle = false;
			}
			else
			{
				alle_x1 = min(p->x, alle_x1);
				alle_y1 = min(p->y, alle_y1);
				alle_x2 = max(p->x, alle_x2);
				alle_y2 = max(p->y, alle_y2);
			}
		}
	}
	for(schiffe_iterator iter = universum->first_schiff(); iter != universum->last_schiff(); iter++)
	{
		UVSchiff* s = (*iter).second;
		if(s->dim == dim)
		{
			if(s->besitzer == spieler->name)
			{
				if(first_eigene)
				{
					eigene_x1 = s->x;
					eigene_y1 = s->y;
					eigene_x2 = s->x;
					eigene_y2 = s->y;
					first_eigene = false;
				}
				else
				{
					eigene_x1 = min(s->x, eigene_x1);
					eigene_y1 = min(s->y, eigene_y1);
					eigene_x2 = max(s->x, eigene_x2);
					eigene_y2 = max(s->y, eigene_y2);
				}
			}
			if(first_alle)
			{
				alle_x1 = s->x;
				alle_y1 = s->y;
				alle_x2 = s->x;
				alle_y2 = s->y;
				first_alle = false;
			}
			else
			{
				alle_x1 = min(s->x, alle_x1);
				alle_y1 = min(s->y, alle_y1);
				alle_x2 = max(s->x, alle_x2);
				alle_y2 = max(s->y, alle_y2);
			}
		}
	}
	for(container_iterator iter = universum->first_container(); iter != universum->last_container(); iter++)
	{
		UVContainer* c = (*iter);
		if(c->dim == dim)
		{
			if(first_alle)
			{
				alle_x1 = c->x;
				alle_y1 = c->y;
				alle_x2 = c->x;
				alle_y2 = c->y;
				first_alle = false;
			}
			else
			{
				alle_x1 = min(c->x, alle_x1);
				alle_y1 = min(c->y, alle_y1);
				alle_x2 = max(c->x, alle_x2);
				alle_y2 = max(c->y, alle_y2);
			}
		}
	}
	for(anomalien_iterator iter = universum->first_anomalie(); iter != universum->last_anomalie(); iter++)
	{
		UVAnomalie* a = (*iter);
		if(a->dim == dim)
		{
			if(first_alle)
			{
				alle_x1 = a->x - a->radius;
				alle_y1 = a->y - a->radius;
				alle_x2 = a->x + a->radius;
				alle_y2 = a->y + a->radius;
				first_alle = false;
			}
			else
			{
				alle_x1 = min(a->x - a->radius, alle_x1);
				alle_y1 = min(a->y - a->radius, alle_y1);
				alle_x2 = max(a->x + a->radius, alle_x2);
				alle_y2 = max(a->y + a->radius, alle_y2);
			}
		}
	}
	for(sensorsonden_iterator iter = universum->first_sensorsonde(); iter != universum->last_sensorsonde(); iter++)
	{
		UVSensorsonde* s = (*iter).second;
		if(s->dim == dim)
		{
			if(first_alle)
			{
				alle_x1 = s->x - s->lebensdauer * 1000;
				alle_y1 = s->y - s->lebensdauer * 1000;
				alle_x2 = s->x + s->lebensdauer * 1000;
				alle_y2 = s->y + s->lebensdauer * 1000;
				first_alle = false;
			}
			else
			{
				alle_x1 = min(s->x - s->lebensdauer * 1000, alle_x1);
				alle_y1 = min(s->y - s->lebensdauer * 1000, alle_y1);
				alle_x2 = max(s->x + s->lebensdauer * 1000, alle_x2);
				alle_y2 = max(s->y + s->lebensdauer * 1000, alle_y2);
			}
		}
	}
	for(infosonden_iterator iter = universum->first_infosonde(); iter != universum->last_infosonde(); iter++)
	{
		UVInfosonde* s = (*iter).second;
		if(s->dim == dim)
		{
			if(first_alle)
			{
				alle_x1 = s->x - s->lebensdauer * 1000;
				alle_y1 = s->y - s->lebensdauer * 1000;
				alle_x2 = s->x + s->lebensdauer * 1000;
				alle_y2 = s->y + s->lebensdauer * 1000;
				first_alle = false;
			}
			else
			{
				alle_x1 = min(s->x - s->lebensdauer * 1000, alle_x1);
				alle_y1 = min(s->y - s->lebensdauer * 1000, alle_y1);
				alle_x2 = max(s->x + s->lebensdauer * 1000, alle_x2);
				alle_y2 = max(s->y + s->lebensdauer * 1000, alle_y2);
			}
		}
	}

	// 1 ly Rand drumrum
	alle_x1 -= 1000;
	alle_y1 -= 1000;
	alle_x2 += 1000;
	alle_y2 += 1000;
	eigene_x1 -= 1000;
	eigene_y1 -= 1000;
	eigene_x2 += 1000;
	eigene_y2 += 1000;
}


/*
 * Springt zu Ausschnitt mit allen Objekten sichtbar.
 */
void UVMap::jump_alle()
{
	zoom = max(double(alle_x2 - alle_x1) / double(screen_size.w),
	           double(alle_y2 - alle_y1) / double(screen_size.h));
	offset_x = alle_x1 - (long(rint(double(screen_size.w) * zoom))
	                      - (alle_x2 - alle_x1)) / 2;
	offset_y = alle_y1 - (long(rint(double(screen_size.h) * zoom))
	                      - (alle_y2 - alle_y1)) / 2;

	redraw();
}


/*
 * Springt zu Ausschnitt mit allen eigenen Objekten sichtbar.
 */
void UVMap::jump_eigene()
{
	zoom = max(double(eigene_x2 - eigene_x1) / double(screen_size.w),
	           double(eigene_y2 - eigene_y1) / double(screen_size.h));
	offset_x = eigene_x1 - (long(rint(double(screen_size.w) * zoom))
	                        - (eigene_x2 - eigene_x1)) / 2;
	offset_y = eigene_y1 - (long(rint(double(screen_size.h) * zoom))
	                        - (eigene_y2 - eigene_y1)) / 2;

	redraw();
}


/*
 * Setzt die Dimension.
 */
void UVMap::set_dim(long d)
{
	dim = d;
	jump_init();
	jump_alle();
}
long UVMap::get_dim() const
{
	return dim;
}


/*
 * Scrollt die Karte.
 *
 * dx/dy geben die Anzahl physikalischer Pixel an, um welche gescrollt wird.
 */
void UVMap::scroll(long dx, long dy)
{
	offset_x += long(rint(dx * zoom));
	offset_y += long(rint(dy * zoom));

	redraw();
}


/*
 * Zoomt die Karte.
 *
 * Minimalzoom ist 1.0 (1 Karteneinheit pro physikalischem Pixel).
 * Maximalzoom gibts keinen, herauszoomen darf man solange man will.
 *
 * FIXME: Dies sollte irgendwann mal abhaengig der tatsaechlich vorhandenen
 *        Objekte gemacht werden. (zoom out)
 */
void UVMap::zoom_out()
{
	zoom_by(1.2);
}
void UVMap::zoom_in()
{
	zoom_by(1.0/1.2);
}
void UVMap::zoom_by(double f)
{
	// Maximalzoom (langsames Bitmap Scaling)
	if(f * zoom < 1.0)
	{
		f = 1.0 / zoom;
	}

	// Mindestzoom (long int Range Overflow)
	if(f * zoom > 1000000.0)
	{
		f = 1000000.0 / zoom;
	}

	offset_x -= long(rint(double(screen->w) * zoom * (f - 1.0) / 2.0));
	offset_y -= long(rint(double(screen->h) * zoom * (f - 1.0) / 2.0));
	zoom *= f;

	redraw();
}


/*
 * Fuehrt einen Map Resize durch.
 */
void UVMap::resize(SDL_Surface* s)
{
	screen = s;

//	cout << "resize from w=" << screen_size.w << " h=" << screen_size.h << endl;
//	cout << "resize to w=" << screen->w << " h=" << screen->h << endl;
	double wf = double(screen_size.w) / double(screen->w);
//	double hf = double(screen_size.h) / double(screen->h);
//	cout << "factors wf=" << wf << " hf=" << hf << endl;

	screen_size.w = screen->w;
	screen_size.h = screen->h;

	zoom *= wf;

	redraw();
}


/*
 * Sichtradien set/get/toggle.
 */
bool UVMap::get_sichtradien() const
{
	return opt_sichtradien;
}
void UVMap::set_sichtradien(const bool s)
{
	opt_sichtradien = s;
	redraw();
}
void UVMap::toggle_sichtradien()
{
	opt_sichtradien = !opt_sichtradien;
	redraw();
}


/*
 * Kaufradien set/get/toggle.
 */
bool UVMap::get_kaufradien() const
{
	return opt_kaufradien;
}
void UVMap::set_kaufradien(const bool k)
{
	opt_kaufradien = k;
	redraw();
}
void UVMap::toggle_kaufradien()
{
	opt_kaufradien = !opt_kaufradien;
	redraw();
}


/*
 * Zeichnet alles neu.
 */
void UVMap::redraw()
{
	SDL_Rect rect = { 0, 0, screen_size.w, screen_size.h };
	draw(&rect);
}


/*
 * Zeichnet die Welt.
 */
void UVMap::draw(SDL_Rect* rect)
{
	phys = rect;

	virt_x = long(rint(phys->x * zoom + offset_x));
	virt_y = long(rint(phys->y * zoom + offset_y));
	virt_w = long(rint(phys->w * zoom));
	virt_h = long(rint(phys->h * zoom));


	if(SDL_MUSTLOCK(screen))
	{
		if(SDL_LockSurface(screen) < 0)
		{
			throw EXCEPTION("Kann Bildschirm-Surface nicht reservieren!");
		}
	}

	long ticks = SDL_GetTicks();

	SDL_FillRect(screen, phys, SDL_MapRGB(screen->format, 0, 0, 0));

	// Grid zeichnen
	draw_grid();

	// Ueber alle zeichenbaren Objekte loopen.
	for(planeten_iterator iter = universum->first_planet(); iter != universum->last_planet(); iter++)
	{
		if((*iter).second->dim == dim)
		{
			draw_planet((*iter).second);
		}
	}
	for(anomalien_iterator iter = universum->first_anomalie(); iter != universum->last_anomalie(); iter++)
	{
		if((*iter)->dim == dim)
		{
			draw_anomalie(*iter);
		}
	}
	for(sensorsonden_iterator iter = universum->first_sensorsonde(); iter != universum->last_sensorsonde(); iter++)
	{
		if((*iter).second->dim == dim)
		{
			draw_sensorsonde((*iter).second);
		}
	}
	for(infosonden_iterator iter = universum->first_infosonde(); iter != universum->last_infosonde(); iter++)
	{
		if((*iter).second->dim == dim)
		{
			draw_infosonde((*iter).second);
		}
	}
	for(container_iterator iter = universum->first_container(); iter != universum->last_container(); iter++)
	{
		if((*iter)->dim == dim)
		{
			draw_container(*iter);
		}
	}
	for(schiffe_iterator iter = universum->first_schiff(); iter != universum->last_schiff(); iter++)
	{
		if((*iter).second->dim == dim)
		{
			draw_schiff((*iter).second);
		}
	}


	// Overlays...
	SDL_Rect dst;

	// Status-Overlay
	SDL_Surface* status = debug_font->get_surface(
		             to_string(dim) + " - " + universum->get_dim(dim) + ", "
		             + spieler->name
		             + ((spieler->talent != "") ? " der " + spieler->talent : "")
		             + ", Sternzeit " + to_string(universum->sternzeit), LABEL_RGB);
	dst.x = screen->w - status->w - status->h * 2;
	dst.y = status->h;
	drw->box(screen, dst.x - status->h / 4, dst.y, dst.x + status->w + status->h / 4, dst.y + status->h, 0, 0, 0, 0x88);
	SDL_BlitSurface(status, 0, screen, &dst);
	SDL_FreeSurface(status);

#ifdef DEBUG
	// Debug-Overlay
	long dticks = SDL_GetTicks() - ticks;
	SDL_Surface* debug = debug_font->get_surface(
		             + "debug  /  zoom=" + to_string(zoom)
		             + " offset_x=" + to_string(offset_x)
		             + " offset_y=" + to_string(offset_y)
		             + "  /  " + to_string(dticks) + " ms = "
		             + to_string(1000 / dticks) + " fps  /  uvnav-"
	                 + PACKAGE_VERSION + " (" + to_string(revision) + ")"
		, LABEL_RGB);
	dst.x = screen->w - debug->w - debug->h * 2;
	dst.y = screen->h - debug->h * 2;
	drw->box(screen, dst.x - debug->h / 4, dst.y, dst.x + debug->w + debug->h / 4, dst.y + debug->h, 0, 0, 0, 0x88);
	SDL_BlitSurface(debug, 0, screen, &dst);
	SDL_FreeSurface(debug);
#endif

	/*
	 * DEBUG Code fuer Widgets
	 */
/*	UVCompositeWidget* cw1 = new UVCompositeWidget();
	UVCompositeWidget* cw2 = new UVCompositeWidget(2, UVOVertical);
	UVWidget* w1 = new UVWidget();
	UVWidget* w2 = new UVWidget();
	UVWidget* w3 = new UVWidget();
	UVWidget* w4 = new UVWidget();
	cw1->set_surface(screen);
	cw2->add_widget(w2);
	cw2->add_widget(w3);
	cw1->add_widget(w1);
	cw1->add_widget(cw2);
	cw1->add_widget(w4);
	UVWindow* win = new UVWindow(cw1, screen);
	win->x = dst.x;
	win->y = dst.y;
	win->w = dst.w;
	win->h = dst.h;
	win->resize();
	win->draw();
*/	/*
	 * END OF DEBUG
	 */

	SDL_UpdateRect(screen, phys->x, phys->y, phys->w, phys->h);

	if(SDL_MUSTLOCK(screen))
	{
		SDL_UnlockSurface(screen);
	}
}


/*
 * Zeichnet das Koordinatennetz.
 *
 * Berechnet eine sinnvolle Maschengroesse als Vielfaches
 * von ganzen Lichtjahren (1000 Karteneinheiten).
 *
 * +-------------------------------------------+
 * | �123     �124     �125     �126     �127  |
 * | �        �        �        �        �     |
 * |-+--------+--------+--------+--------+-----|
 * |13 +-->x  �        �        �        �   13|
 * | � �      �        �        �        �     |
 * | � Vy     �        �        �        �     |
 * |-+--------+--------+--------+--------+-----|
 * |14        �        �        �        �   14|
 * | �        �        �        �        �     |
 * | �        �        �        �        �     |
 * |-+--------+--------+--------+--------+-----|
 * |15        �        �        �        �   15|
 * | �        �        �        �        �     |
 * | � 1000 KE�        �        �        �     |
 * |-+========+--------+--------+--------+-----|
 * |16123 1:45�124     �125     �126     �12716|
 * +-------------------------------------------+
 *
 */
void UVMap::draw_grid()
{
	static const long tick = UVConf::get_instance()->l_get("map-grid-font-size") / 2;

	// Maschengroesse = groesste Zweierpotenz von 125 KE kleiner als 100px
	long d = long(rint(zoom * 125.0 / 100.0));
	long i = 1;
	while(i < d) { i *= 2; }
	d = 125 * i;

	SDL_Rect dst = { 0, 0, 0, 0 };

	// Abszissen-Linien (senkrechte Linien)
	long first_x = long(rint(ceil(double(offset_x) / d) * d));
	long last_x = first_x;
	for(long x = first_x; x < offset_x + long(rint(screen->w * zoom)); x += d)
	{
		long p_x = long(rint(double(x - offset_x) / zoom));
		drw->line(screen, p_x, 0, p_x, screen->h - 1, 0x22, 0x22, 0x22);

		// Beschriftung
		SDL_Surface* abszisse = grid_font->get_surface(to_string(x), 0x44, 0x44, 0x44);
		dst.x = p_x + 2; dst.y = 0;
		SDL_BlitSurface(abszisse, 0, screen, &dst);
		dst.x = p_x + 2; dst.y = screen->h - 1 - abszisse->h;
		SDL_BlitSurface(abszisse, 0, screen, &dst);
		SDL_FreeSurface(abszisse);

		last_x = x;
	}

	// Ordinaten-Linien (waagrechte Linien)
	long first_y = long(rint(ceil(double(offset_y) / d) * d));
	long last_y = first_y;
	for(long y = first_y; y < offset_y + long(rint(screen->h * zoom)); y += d)
	{
		long p_y = long(rint(double(y - offset_y) / zoom));
		drw->line(screen, 0, p_y, screen->w - 1, p_y, 0x22, 0x22, 0x22);

		// Beschriftung
		SDL_Surface* ordinate = grid_font->get_surface(to_string(y), 0x44, 0x44, 0x44);
		dst.x = 0; dst.y = p_y + 2;
		SDL_BlitSurface(ordinate, 0, screen, &dst);
		dst.x = screen->w - 1 - ordinate->w; dst.y = p_y + 2;
		SDL_BlitSurface(ordinate, 0, screen, &dst);
		SDL_FreeSurface(ordinate);

		last_y = y;
	}

	// Massstabsbalken mit Laengenangabe d und Massstab 1:zoom
	SDL_Surface* distanz = grid_font->get_surface(to_string(d) + " KE", 0x88, 0x88, 0x88);
	SDL_Surface* massstab = grid_font->get_surface("1:" + to_string(long(rint(zoom))), 0x88, 0x88, 0x88);
	long m_y = long(rint(double(last_y - offset_y) / zoom));
	long m_x1 = long(rint(double(first_x - offset_x) / zoom));
	long m_x2 = long(rint(double(first_x + d - offset_x) / zoom));
	long m_d = long(rint(double(d) / zoom));
	if(screen->h - m_y < massstab->h + 2)
	{
		m_y = long(rint(double(last_y - d - offset_y) / zoom));
	}
	drw->line(screen, m_x1, m_y, m_x2, m_y, 0x66, 0x66, 0x66);
	drw->line(screen, m_x1, m_y - tick, m_x1, m_y + tick, 0x66, 0x66, 0x66);
	drw->line(screen, m_x2, m_y - tick, m_x2, m_y + tick, 0x66, 0x66, 0x66);
	dst.x = m_x1 + m_d / 2 - distanz->w / 2;
	dst.y = m_y - 2 - distanz->h;
	SDL_BlitSurface(distanz, 0, screen, &dst);
	SDL_FreeSurface(distanz);
	dst.x = m_x1 + m_d / 2 - massstab->w / 2;
	dst.y = m_y + 2;
	SDL_BlitSurface(massstab, 0, screen, &dst);
	SDL_FreeSurface(massstab);

	// System-Pfeile mit Dimensionsangabe
	long s_x1 = m_x1;
	long s_x2 = long(rint(double(first_x + d/2 - offset_x) / zoom));
	long s_y1 = long(rint(double(first_y - offset_y) / zoom));
	long s_y2 = long(rint(double(first_y + d/2 - offset_y) / zoom));
	// nur zeichnen, falls Pfeilsystem oberhalb des Massstabs zu liegen kommt
	if(s_y1 < m_y)
	{
		drw->line(screen, s_x1, s_y1, s_x2, s_y1, 0x66, 0x66, 0x66);
		drw->line(screen, s_x1, s_y1, s_x1, s_y2, 0x66, 0x66, 0x66);
		drw->line(screen, s_x2, s_y1, s_x2 - tick, s_y1 - tick, 0x66, 0x66, 0x66);
		drw->line(screen, s_x2, s_y1, s_x2 - tick, s_y1 + tick, 0x66, 0x66, 0x66);
		drw->line(screen, s_x1, s_y2, s_x1 - tick, s_y2 - tick, 0x66, 0x66, 0x66);
		drw->line(screen, s_x1, s_y2, s_x1 + tick, s_y2 - tick, 0x66, 0x66, 0x66);
		SDL_Surface* x_label = grid_font->get_surface("X", 0x88, 0x88, 0x88);
		dst.x = s_x2 + tick;
		dst.y = s_y1 - x_label->h / 2;
		SDL_BlitSurface(x_label, 0, screen, &dst);
		SDL_FreeSurface(x_label);
		SDL_Surface* y_label = grid_font->get_surface("Y", 0x88, 0x88, 0x88);
		dst.x = s_x1 - y_label->w / 2;
		dst.y = s_y2;
		SDL_BlitSurface(y_label, 0, screen, &dst);
		SDL_FreeSurface(y_label);
	}
}


/*
 * Zeichnet einen Planeten.
 *
 *
 *    ,--.
 *   (    )
 *   (    )
 *    `--'
 *
 * - wohin mit Name, Nummer?
 * - wohin mit (+) Overlay?
 * - wohin mit TL?
 * - wohin mit Zonen-Overview?
 *   - Cleveres System austuefteln
 *     Sofort erkennbar, wieviel "Wert" die Zone ist (obj. Wichtigkeit)
 *     Sofort erkennbar, wieviel Leute dort sind
 * - Verschiedene Modi
 *   - Besitzverhaeltnisse-Modus (es wimmelt von Namens-Labels)
 *   - Einnahmen-Modus (es wimmelt von Credits und Rohstoffen)
 *   - Navigations-Modus (es wimmelt von Raumschiffen)
 *
 */
void UVMap::draw_planet(UVPlanet* planet)
{
	static const long size = 125;

	planet->drawflag = !planet->drawflag;

	long h = long(rint(1.0 * size / zoom));
	h = (h < 5) ? 5 : h;

	double center_x = double(planet->x - offset_x) / zoom;
	double center_y = double(planet->y - offset_y) / zoom;

	// Verbindungen
	// *** optional
	for(vector<long>::iterator iter = planet->nachbarn.begin(); iter != planet->nachbarn.end(); iter++)
	{
		UVPlanet* p = universum->get_planet((*iter));
		// *** Verbindungen zu unbekannten Planeten auch zeichnen!
		if((p != NULL) && (p->drawflag != planet->drawflag))
		{
			double target_x = double(p->x - offset_x) / zoom;
			double target_y = double(p->y - offset_y) / zoom;
			drw->line(screen, long(rint(center_x)), long(rint(center_y)),
			                  long(rint(target_x)), long(rint(target_y)),
			                  0x88, 0x88, 0x88);
		}
	}

	// *** details zeichnen falls selektiert
	static const long dist = size/2;
	if(((virt_x - dist < planet->x) && (virt_x + virt_w + dist >= planet->x))
	&& ((virt_y - dist < planet->y) && (virt_y + virt_h + dist >= planet->y)))
	{
		// Planetenbild
		// *** konfigurierbar: groesse des planeten beruecksichtigen
		// *** benoetigt aber besseres caching sonst ist performance im arsch
		SDL_Surface* surface = imagehandler->get_surface(planet->image, 0, h);
		SDL_Rect dst;
		dst.x = long(rint(center_x - h / 2));
		dst.y = long(rint(center_y - h / 2));
		SDL_BlitSurface(surface, 0, screen, &dst);
		// Kein SDL_FreeSurface!

		long tl = planet->techlevel;

		if((zoom < 100.0) && (planet->handelsstation != ""))
		{
			// Handelsstation
			long dr = (tl > 0) ? 4 : 2;
			drw->circle(screen, long(rint(center_x)), long(rint(center_y)),
			                    (h / 2) + dr, 0xFF, 0x00, 0xFF, 0xFF);
			drw->circle(screen, long(rint(center_x)), long(rint(center_y)),
			                    (h / 2) + ++dr, 0xFF, 0x00, 0xFF, 0xFF);
		}

		if((h > 5) && (tl > 0))
		{
			// *** provisorisch
			// *** Alternative: statt kreis ein rechteck zeichnen, in farbe, mit zahl.

			// 10:  00 ff 00
			// 7-9: ff ff 00
			// 1-6: ff 00 00
			short r = (tl >= 10) ? 0x00 : 0xFF;
			short g = (tl >= 7) ? 0xFF : 0x00;
			short b = 0x00;

			drw->circle(screen, long(rint(center_x)), long(rint(center_y)),
			                    h / 2 + 2, r, g, b, 0xFF);
		}

		if(zoom < 40.0)
		{
			// Beschriftung
			// *** provisorisch
			string label_text = "";
			if(zoom < 10.0)
			{
				label_text = to_string(planet->nummer) + " " + planet->name;
			}
			else if(zoom < 20.0)
			{
				label_text = to_string(planet->nummer) + " " + planet->name.substr(0,3);
			}
			else
			{
				label_text = to_string(planet->nummer);
			}
			SDL_Surface* label = label_font->get_surface(label_text, LABEL_RGB);
			dst.x = long(rint(center_x + h / 2)) + 4;
			dst.y = long(rint(center_y - label->h / 2));
			drw->box(screen, dst.x - label->h / 4, dst.y, dst.x + label->w + label->h / 4, dst.y + label->h, 0, 0, 0, 0x88);
			SDL_BlitSurface(label, 0, screen, &dst);
			SDL_FreeSurface(label);
		}

//		cout << "draw Planet (" << center_x << "/" << center_y << ")" << endl;
	}
}


/*
 * Zeichnet ein Schiff.
 *
 *      .
 *      |
 *      A
 *  .  /|\  .
 *  |  |||  |
 *  o-( | )-o
 *     ���
 *
 */
void UVMap::draw_schiff(UVSchiff* schiff)
{
	static const long size = 75;

	double center_x = double(schiff->x - offset_x) / zoom;
	double center_y = double(schiff->y - offset_y) / zoom;

	// *** Vektor nur fuer selektiertes Schiff zeichnen
	long dist = max(size/2, long(rint(schiff->v * 100.0)));
	if(((virt_x - dist < schiff->x) && (virt_x + virt_w + dist >= schiff->x))
	&& ((virt_y - dist < schiff->y) && (virt_y + virt_h + dist >= schiff->y)))
	{
		long h = long(rint(double(size) / zoom));
		h = (h < 10) ? 10 : h;

		/*
		 *         dx
		 *      +------.
		 *      �     /
		 *      �    /
		 *   dy �   /
		 *      �__/ v
		 *      �w/
		 *      �/
		 *      o
		 *
		 *       w(rad) = w(�) / 180 * PI
		 *       dy = v * cos( w - 270 )
		 *       dx = v * sin( w - 270 )
		 */

		double target_x = center_x + sin(PI * (schiff->w) / 180.0) * schiff->v * 100.0 / zoom;
		double target_y = center_y - cos(PI * (schiff->w) / 180.0) * schiff->v * 100.0 / zoom;

		// *** provisorisch
		drw->circle(screen, long(rint(center_x)), long(rint(center_y)), h/2,
		                    0xFF, 0xFF, 0xFF);
		drw->line(screen, long(rint(center_x)), long(rint(center_y)),
		                  long(rint(target_x)), long(rint(target_y)),
		                  0xFF, 0xFF, 0xFF);
		if(zoom < 30.0)
		{
			// Beschriftung
			// *** provisorisch
			string label_text = "";
			if(zoom < 20.0)
			{
				label_text = schiff->name + " (" + schiff->besitzer + ")";
			}
			else
			{
				label_text = schiff->name;
			}
			SDL_Rect dst = { 0, 0, 0, 0};
			SDL_Surface* label = label_font->get_surface(label_text, LABEL_RGB);
			dst.x = long(rint(center_x + h / 2)) + 4;
			dst.y = long(rint(center_y - label->h / 2));
			drw->box(screen, dst.x - label->h / 4, dst.y, dst.x + label->w + label->h / 4, dst.y + label->h, 0, 0, 0, 0x88);
			SDL_BlitSurface(label, 0, screen, &dst);
			SDL_FreeSurface(label);
		}
	}

	// Reichweiten fuer eigene Schiffe
	if(schiff->besitzer == spieler->name)
	{
		// Sensoren
		if(opt_sichtradien)
		{
			long sensor_rad = schiff->sichtweite;
			if(((virt_x - sensor_rad < schiff->x) && (virt_x + virt_w + sensor_rad >= schiff->x))
			&& ((virt_y - sensor_rad < schiff->y) && (virt_y + virt_h + sensor_rad >= schiff->y)))
			{
				// Kreis
				long r = long(rint(double(sensor_rad) / zoom));
				drw->circle(screen, long(rint(center_x)), long(rint(center_y)),
				                    r, 0x00, 0xFF, 0x00, 0xFF);
			}
		}

		// Kaufradius
		if(opt_kaufradien)
		{
			static const long kauf_rad = 5000;
			if(((virt_x - kauf_rad < schiff->x) && (virt_x + virt_w + kauf_rad >= schiff->x))
			&& ((virt_y - kauf_rad < schiff->y) && (virt_y + virt_h + kauf_rad >= schiff->y)))
			{
				// Kreis
				long r = long(rint(double(kauf_rad) / zoom));
				drw->circle(screen, long(rint(center_x)), long(rint(center_y)),
				                    r, 0xFF, 0xFF, 0x00, 0xFF);
			}
		}
	}

//	cout << "draw Schiff (" << center_x << "/" << center_y << ")" << endl;
}


/*
 * Zeichnet einen Container.
 *     _______
 *    ////////
 *   +------+/
 *   ||||||||/
 *   +------+
 *
 */
void UVMap::draw_container(UVContainer* container)
{
	static const long size = 50;

	if(((virt_x - size < container->x) && (virt_x + virt_w + size >= container->x))
	&& ((virt_y - size < container->y) && (virt_y + virt_h + size >= container->y)))
	{
		long h = long(rint(double(size) / zoom));
		h = (h < 2) ? 2 : h;

		double center_x = double(container->x - offset_x) / zoom;
		double center_y = double(container->y - offset_y) / zoom;

		drw->box(screen, long(rint(center_x)) - h/2, long(rint(center_y)) - h/2,
		                 long(rint(center_x)) + h/2, long(rint(center_y)) + h/2,
		                 0xFF, 0xFF, 0xFF);

//		cout << "draw Container (" << center_x << "/" << center_y << ")" << endl;
	}
}


/*
 * Zeichnet eine Anomalie.
 * 
 *   ( + )
 *
 */
void UVMap::draw_anomalie(UVAnomalie* anomalie)
{
	static const long mitte_size = 100;
	long size = anomalie->radius * 1000;

	if(((virt_x - size < anomalie->x) && (virt_x + virt_w + size >= anomalie->x))
	&& ((virt_y - size < anomalie->y) && (virt_y + virt_h + size >= anomalie->y)))
	{
		double center_x = double(anomalie->x - offset_x) / zoom;
		double center_y = double(anomalie->y - offset_y) / zoom;

		if(anomalie->radius)
		{
			// Kreis
			long h = long(rint(double(size) / zoom));
			h = (h < 10) ? 10 : h;
			drw->circle(screen, long(rint(center_x)), long(rint(center_y)),
			                    h / 2, 0xFF, 0x00, 0x00, 0xFF);
		}

		// Zentrum
		long mitte_r = long(rint(double(mitte_size) / zoom));
		mitte_r = (mitte_r < 2) ? 2 : mitte_r;
		drw->line(screen, long(rint(center_x)) - mitte_r, long(rint(center_y)),
		                  long(rint(center_x)) + mitte_r, long(rint(center_y)),
		                  0xFF, 0x00, 0x00, 0xFF);
		drw->line(screen, long(rint(center_x)), long(rint(center_y)) - mitte_r,
		                  long(rint(center_x)), long(rint(center_y)) + mitte_r,
		                  0xFF, 0x00, 0x00, 0xFF);

//		cout << "draw Anomalie (" << center_x << "/" << center_y << ")" << endl;
	}
}


/*
 * Zeichnet eine Sensorsonde.
 * 
 *   ( + )
 *
 */
void UVMap::draw_sensorsonde(UVSensorsonde* sensorsonde)
{
	static const long mitte_size = 100;
	long size = sensorsonde->lebensdauer * 2000;

	if(((virt_x - size < sensorsonde->x) && (virt_x + virt_w + size >= sensorsonde->x))
	&& ((virt_y - size < sensorsonde->y) && (virt_y + virt_h + size >= sensorsonde->y)))
	{
		double center_x = 1.0 * (sensorsonde->x - offset_x) / zoom;
		double center_y = 1.0 * (sensorsonde->y - offset_y) / zoom;

		if(sensorsonde->lebensdauer)
		{
			// Kreis
			long h = long(rint(1.0 * size / zoom));
			h = (h < 10) ? 10 : h;
			drw->circle(screen, long(rint(center_x)), long(rint(center_y)),
			                    h / 2, 0x00, 0xFF, 0x00, 0xFF);
		}

		// Zentrum
		long mitte_r = long(rint(1.0 * mitte_size / zoom));
		mitte_r = (mitte_r < 2) ? 2 : mitte_r;
		drw->line(screen, long(rint(center_x)) - mitte_r, long(rint(center_y)),
		                  long(rint(center_x)) + mitte_r, long(rint(center_y)),
		                  0x00, 0xFF, 0x00, 0xFF);
		drw->line(screen, long(rint(center_x)), long(rint(center_y)) - mitte_r,
		                  long(rint(center_x)), long(rint(center_y)) + mitte_r,
		                  0x00, 0xFF, 0x00, 0xFF);

//		cout << "draw Sensorsonde (" << center_x << "/" << center_y << ")" << endl;
	}
}


/*
 * Zeichnet eine Infosonde.
 * 
 *   ( + )
 *
 */
void UVMap::draw_infosonde(UVInfosonde* infosonde)
{
	static const long mitte_size = 100;
	long size = infosonde->lebensdauer * 2000;

	if(((virt_x - size < infosonde->x) && (virt_x + virt_w + size >= infosonde->x))
	&& ((virt_y - size < infosonde->y) && (virt_y + virt_h + size >= infosonde->y)))
	{
		double center_x = 1.0 * (infosonde->x - offset_x) / zoom;
		double center_y = 1.0 * (infosonde->y - offset_y) / zoom;

		if(infosonde->lebensdauer)
		{
			// Kreis
			long h = long(rint(1.0 * size / zoom));
			h = (h < 10) ? 10 : h;
			drw->circle(screen, long(rint(center_x)), long(rint(center_y)),
			                    h / 2, 0x00, 0x00, 0xFF, 0xFF);
		}

		// Zentrum
		long mitte_r = long(rint(1.0 * mitte_size / zoom));
		mitte_r = (mitte_r < 2) ? 2 : mitte_r;
		drw->line(screen, long(rint(center_x)) - mitte_r, long(rint(center_y)),
		                  long(rint(center_x)) + mitte_r, long(rint(center_y)),
		                  0x00, 0x00, 0xFF, 0xFF);
		drw->line(screen, long(rint(center_x)), long(rint(center_y)) - mitte_r,
		                  long(rint(center_x)), long(rint(center_y)) + mitte_r,
		                  0x00, 0x00, 0xFF, 0xFF);

//		cout << "draw Infosonde (" << center_x << "/" << center_y << ")" << endl;
	}
}


