/*
 * UV Navigator - Auswertungsvisualisierung fuer Universum V
 * Copyright (C) 2004 Daniel Roethlisberger <roe@chronator.ch>
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
 *    ж
 * -x ж (0/0)     virtuelle Koordinaten des Universums in Karteneinheiten +x
 * <--+-------------------------------------------------------------------->
 *    ж
 *    ж                  (offset_x/offset_y)
 *    ж                +---------------------------+
 *    ж                ж (0/0)                     ж
 *    ж                ж                           ж
 *    ж                ж    physikalische          ж
 *    ж                ж    Bildschirm-            ж
 *    ж                ж    Koordinaten            ж
 *    ж                ж    (Pixel)                ж
 *    ж                ж                           ж
 *    ж                ж       (screen_w/screen_h) ж
 *    ж                +---------------------------+
 *    ж                     (offset_x+screen_w*zoom/offset_y+screen_h*zoom)
 *    ж
 *    V +y
 *
 * UVMap Zeichnet UVWelt. UVWelt ist die Datenstruktur, welche von UVMap
 * iteriert und gezeichnet wird. UVWelt wird zur Ablage von Surface Caches
 * benutzt, hat aber lediglich eine passive Rolle in der
 * Bildschirmdarstellung. Das mag zwar von der Objektorientierung her nicht
 * so schoen sein, ist aber X mal schneller und viel weniger umstaendlich
 * zu implementieren. Unter dem Strich viel schoenerer Code, auch wenn sich
 * die Objekte nicht selber zeichnen koennen.
 */


/*
 * Konstruktor.
 *
 * Waehlt sinnvollen Kartenausschnitt und Dimension falls alte Werte sinnlos,
 * d.h. wenn keine Daten sichtbar waeren.
 */
UVMap::UVMap(UVConf* c, UVImages* i, UVWelt* w, SDL_Surface* s)
: conf(c), images(i), welt(w), screen(s)
, phys(NULL), virt_x(0), virt_y(0), virt_w(0), virt_h(0)
{
	offset_x = conf->l_get("map-offset-x", true);
	offset_y = conf->l_get("map-offset-y", true);
	zoom = conf->f_get("map-zoom", true);
	dim = conf->l_get("map-dim", true);

	// Sinnvolle Dimension waehlen, falls keine Daten in dim.
	while((welt->get_dim(dim) == "") && (dim < 5))
	{
		dim++;
	}
	if(welt->get_dim(dim) == "")
	{
		throw EXCEPTION("Keine Dimension hat Daten!");
	}

	// *** Sinnvollen Zoom und Auschnitt waehlen falls keine Daten sichtbar
	// *** gleichzeitig muss aber der Default in conf.cpp geaendert werden

	draw = new UVDraw(conf);
	overlay_font = new UVFont(conf, FNT_SANS, screen->h / 64);
}


/*
 * Destruktor.
 */
UVMap::~UVMap()
{
	conf->l_set("map-offset-x", offset_x, true);
	conf->l_set("map-offset-y", offset_y, true);
	conf->f_set("map-zoom", zoom, true);
	conf->l_set("map-dim", dim, true);

	delete draw;
	delete overlay_font;
}


/*
 * Setzt die Dimension.
 */
void UVMap::set_dim(long d)
{
	dim = d;
	SDL_Rect rect = { 0, 0, screen->w, screen->h };
	draw_welt(&rect);
}
long UVMap::get_dim()
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

	SDL_Rect rect = { 0, 0, screen->w, screen->h };
	draw_welt(&rect);
}


/*
 * Zoomt die Karte.
 */
void UVMap::zoom_out()
{
	static const double step = 0.2;

	offset_x -= long(rint(screen->w * zoom * step / 2));
	offset_y -= long(rint(screen->h * zoom * step / 2));
	zoom *= 1.0 + step;

	SDL_Rect rect = { 0, 0, screen->w, screen->h };
	draw_welt(&rect);
}
void UVMap::zoom_in()
{
	static const double step = 0.2;

	// Extreme Zoomraten arten aus, bringen miserable
	// Performance, und bringen nix. Alles was Skala 1:1
	// uebersteigt wird hier abgefangen.
	if(zoom >= 1.0 + step)
	{
		zoom /= 1.0 + step;
		offset_x += long(rint(screen->w * zoom * step / 2));
		offset_y += long(rint(screen->h * zoom * step / 2));

		SDL_Rect rect = { 0, 0, screen->w, screen->h };
		draw_welt(&rect);
	}
}


/*
 * Zeichnet die Welt.
 */
void UVMap::draw_welt(SDL_Rect* rect)
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

	SDL_FillRect(screen, phys, SDL_MapRGB(screen->format, 0, 0, 0));

	// Ueber alle zeichenbaren Objekte loopen.
	for(planeten_iterator iter = welt->first_planet(); iter != welt->last_planet(); iter++)
	{
		if((*iter).second->dim == dim)
		{
			draw_planet((*iter).second);
		}
	}
	for(anomalien_iterator iter = welt->first_anomalie(); iter != welt->last_anomalie(); iter++)
	{
		if((*iter)->dim == dim)
		{
			draw_anomalie(*iter);
		}
	}
	for(sensorsonden_iterator iter = welt->first_sensorsonde(); iter != welt->last_sensorsonde(); iter++)
	{
		if((*iter).second->dim == dim)
		{
			draw_sensorsonde((*iter).second);
		}
	}
	for(infosonden_iterator iter = welt->first_infosonde(); iter != welt->last_infosonde(); iter++)
	{
		if((*iter).second->dim == dim)
		{
			draw_infosonde((*iter).second);
		}
	}
	for(container_iterator iter = welt->first_container(); iter != welt->last_container(); iter++)
	{
		if((*iter)->dim == dim)
		{
			draw_container(*iter);
		}
	}
	for(schiffe_iterator iter = welt->first_schiff(); iter != welt->last_schiff(); iter++)
	{
		if((*iter).second->dim == dim)
		{
			draw_schiff((*iter).second);
		}
	}

	// Overlay, GUI, etc
	SDL_Surface* status = overlay_font->get_surface(
		str_stream() << "zoom=" << zoom
		             << " offx=" << offset_x
		             << " offy=" << offset_y
		             << " dim="  << dim);

	SDL_Rect dst;
	dst.x = screen->h - status->h;
	dst.y = 0;
	// evtl noch rechteck davor-blitten
	SDL_BlitSurface(status, 0, screen, &dst);

	if((screen->flags & SDL_DOUBLEBUF) == SDL_DOUBLEBUF)
	{
		SDL_Flip(screen);
	}
	else
	{
		SDL_UpdateRect(screen, phys->x, phys->y, phys->w, phys->h);
	}

	if(SDL_MUSTLOCK(screen))
	{
		SDL_UnlockSurface(screen);
	}

	SDL_FreeSurface(status);
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
	static const long size = 250;

	if(((virt_x - size < planet->x) && (virt_x + virt_w + size >= planet->x))
	&& ((virt_y - size < planet->y) && (virt_y + virt_h + size >= planet->y)))
	{

		// *** konfigurierbar: groesse des planeten beruecksichtigen
		// *** benoetigt aber besseres caching sonst ist performance im arsch
		long h = long(rint(1.0 * size / zoom));
		h = (h < 3) ? 3 : h;
		SDL_Surface* surface = images->get_surface(planet->image, 0, h);

		SDL_Rect dst;
		double center_x = 1.0 * (planet->x - offset_x) / zoom;
		double center_y = 1.0 * (planet->y - offset_y) / zoom;
		dst.x = long(rint(center_x - h / 2));
		dst.y = long(rint(center_y - h / 2));

//		cout << "draw Planet (" << center_x << "/" << center_y << ")" << endl;

		SDL_BlitSurface(surface, 0, screen, &dst);

		if(h > 3)
		{
			// *** Alternative: statt kreis ein rechteck zeichnen, in farbe, mit zahl.

			// 10:  00 ff 00
			// 7-9: 00 00 ff
			// 4-6: ff ff 00
			// 1-3: ff 00 00
			long tl = planet->techlevel;
			short r = (tl > 6) ? 0x00 : 0xFF;
			short g = ((tl == 10) || ((tl >= 4) && (tl <= 6))) ? 0xFF : 0x00;
			short b = ((tl >= 7) && (tl <= 9)) ? 0xFF : 0x00;

			draw->circle(screen, long(rint(center_x)), long(rint(center_y)),
				              h / 2 + 2, r, g, b, 0xFF);
		}
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
 *     иии
 *
 */
void UVMap::draw_schiff(UVSchiff* schiff)
{
	static const long size = 250;

	if(((virt_x - size < schiff->x) && (virt_x + virt_w + size >= schiff->x))
	&& ((virt_y - size < schiff->y) && (virt_y + virt_h + size >= schiff->y)))
	{
		long h = long(rint(1.0 * size / zoom));
		h = (h < 30) ? 30 : h;

		// *** anstatt kreis: kleiner kreis mit geschwindigkeitsvektor

		double center_x = 1.0 * (schiff->x - offset_x) / zoom;
		double center_y = 1.0 * (schiff->y - offset_y) / zoom;

//		cout << "draw Schiff (" << center_x << "/" << center_y << ")" << endl;

		draw->circle(screen, long(rint(center_x)), long(rint(center_y)),
			              h / 2, 0xFF, 0xFF, 0xFF, 0xFF);
	}
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
		long h = long(rint(1.0 * size / zoom));
		h = (h < 10) ? 10 : h;

		// *** anstatt kreis: kleines rechteck oder png

		double center_x = 1.0 * (container->x - offset_x) / zoom;
		double center_y = 1.0 * (container->y - offset_y) / zoom;

//		cout << "draw Container (" << center_x << "/" << center_y << ")" << endl;

		draw->circle(screen, long(rint(center_x)), long(rint(center_y)),
			              h / 2, 0xFF, 0xFF, 0xFF, 0xFF);
	}
}


/*
 * Zeichnet eine Anomalie.
 * 
 *   ( x )
 *
 */
void UVMap::draw_anomalie(UVAnomalie* anomalie)
{
	long size = anomalie->radius;

	if(((virt_x - size < anomalie->x) && (virt_x + virt_w + size >= anomalie->x))
	&& ((virt_y - size < anomalie->y) && (virt_y + virt_h + size >= anomalie->y)))
	{
		long h = long(rint(1.0 * size / zoom));
		h = (h < 10) ? 10 : h;

		// evtl kreis fuellen? schraffieren? grafik?

		double center_x = 1.0 * (anomalie->x - offset_x) / zoom;
		double center_y = 1.0 * (anomalie->y - offset_y) / zoom;

//		cout << "draw Anomalie (" << center_x << "/" << center_y << ")" << endl;

		draw->circle(screen, long(rint(center_x)), long(rint(center_y)),
			              h / 2, 0xFF, 0x00, 0x00, 0xFF);
	}
}


/*
 * Zeichnet eine Sensorsonde.
 * 
 *   ( x )
 *
 */
void UVMap::draw_sensorsonde(UVSensorsonde* sensorsonde)
{
	long size = sensorsonde->lebensdauer * 2000;

	if(((virt_x - size < sensorsonde->x) && (virt_x + virt_w + size >= sensorsonde->x))
	&& ((virt_y - size < sensorsonde->y) && (virt_y + virt_h + size >= sensorsonde->y)))
	{
		long h = long(rint(1.0 * size / zoom));
		h = (h < 10) ? 10 : h;

		// evtl kreis fuellen? schraffieren? grafik?

		double center_x = 1.0 * (sensorsonde->x - offset_x) / zoom;
		double center_y = 1.0 * (sensorsonde->y - offset_y) / zoom;

//		cout << "draw Sensorsonde (" << center_x << "/" << center_y << ")" << endl;

		draw->circle(screen, long(rint(center_x)), long(rint(center_y)),
			              h / 2, 0x00, 0xFF, 0x00, 0xFF);
	}
}


/*
 * Zeichnet eine Infosonde.
 * 
 *   ( x )
 *
 */
void UVMap::draw_infosonde(UVInfosonde* infosonde)
{
	long size = infosonde->lebensdauer * 2000;

	if(((virt_x - size < infosonde->x) && (virt_x + virt_w + size >= infosonde->x))
	&& ((virt_y - size < infosonde->y) && (virt_y + virt_h + size >= infosonde->y)))
	{
		long h = long(rint(1.0 * size / zoom));
		h = (h < 10) ? 10 : h;

		// evtl kreis fuellen? schraffieren? grafik?

		double center_x = 1.0 * (infosonde->x - offset_x) / zoom;
		double center_y = 1.0 * (infosonde->y - offset_y) / zoom;

//		cout << "draw Infosonde (" << center_x << "/" << center_y << ")" << endl;

		draw->circle(screen, long(rint(center_x)), long(rint(center_y)),
			              h / 2, 0x00, 0x00, 0xFF, 0xFF);
	}
}


