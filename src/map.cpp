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
	screen_size.w = s->w;
	screen_size.h = s->h;

	offset_x = conf->l_get("map-offset-x", true);
	offset_y = conf->l_get("map-offset-y", true);
	zoom = conf->f_get("map-zoom", true);
	dim = conf->l_get("map-dim", true);

	// Sinnvolle Dimension waehlen, falls keine Daten in dim.
	if(welt->get_dim(dim) == "")
	{
		for(dim_iterator iter = welt->first_dim(); iter != welt->last_dim(); iter++)
		{
			if((*iter).second != "")
			{
				dim = (*iter).first;
			}
		}
		if(welt->get_dim(dim) == "")
		{
			throw EXCEPTION("Keine Dimension hat Daten!");
		}
		else
		{
			conf->l_set("map-dim", dim, true);
		}
	}

	// *** Sinnvollen Zoom und Auschnitt waehlen falls keine Daten sichtbar
	// *** gleichzeitig muss aber der Default in conf.cpp geaendert werden

	drw = new UVDraw(conf);
	overlay_font = new UVFont(conf, FNT_SANS, screen->h / 48);
	grid_font = new UVFont(conf, FNT_SANS, screen->h / 64);
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

	delete drw;
	delete overlay_font;
	delete grid_font;
}


/*
 * Setzt die Dimension.
 */
void UVMap::set_dim(long d)
{
	dim = d;
	SDL_Rect rect = { 0, 0, screen->w, screen->h };
	draw(&rect);
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
	draw(&rect);
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

	offset_x -= long(rint(1.0 * screen->w * zoom * (f - 1.0) / 2.0));
	offset_y -= long(rint(1.0 * screen->h * zoom * (f - 1.0) / 2.0));
	zoom *= f;

	SDL_Rect rect = { 0, 0, screen->w, screen->h };
	draw(&rect);
}


/*
 * Fuehrt einen Map Resize durch.
 */
void UVMap::resize(SDL_Surface* s)
{
	screen = s;

//	cout << "resize from w=" << screen_size.w << " h=" << screen_size.h << endl;
//	cout << "resize to w=" << screen->w << " h=" << screen->h << endl;
	double wf = 1.0 * screen_size.w / screen->w;
//	double hf = 1.0 * screen_size.h / screen->h;
//	cout << "factors wf=" << wf << " hf=" << hf << endl;

	screen_size.w = screen->w;
//	screen_size.h = screen->h;

	zoom *= wf;

	delete overlay_font;
	delete grid_font;
	overlay_font = new UVFont(conf, FNT_SANS, screen->h / 48);
	grid_font = new UVFont(conf, FNT_SANS, screen->h / 64);

	SDL_Rect rect = { 0, 0, screen->w, screen->h };
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

	SDL_FillRect(screen, phys, SDL_MapRGB(screen->format, 0, 0, 0));

	// Grid zeichnen
	draw_grid();

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
	dst.x = screen->w - status->w;
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
 * Zeichnet das Koordinatennetz.
 *
 * Berechnet eine sinnvolle Maschengroesse als Vielfaches
 * von ganzen Lichtjahren (1000 Karteneinheiten).
 *
 * TODO: 3D-Windrose (x, y, dim), Massstab.
 *
 * +-------------------------------------------+
 * | ж123     ж124     ж125     ж126     ж127  |
 * | ж        ж        ж        ж        ж     |
 * |-+--------+--------+--------+--------+-----|
 * |13        ж        ж        ж        ж   13|
 * | ж        ж        ж        ж        ж     |
 * | ж        ж        ж        ж        ж     |
 * |-+--------+--------+--------+--------+-----|
 * |14        ж        ж        ж        ж   14|
 * | ж        ж        ж        ж        ж     |
 * | ж        ж        ж        ж        ж     |
 * |-+--------+--------+--------+--------+-----|
 * |15        ж        ж        ж        ж   15|
 * | ж        ж        ж        ж        ж     |
 * | ж        ж        ж        ж        ж     |
 * |-+--------+--------+--------+--------+-----|
 * |16123     ж124     ж125     ж126     ж127  |
 * +-------------------------------------------+
 *
 */
void UVMap::draw_grid()
{
	// Maschengroesse = groesste Zweierpotenz in Lichtjahren kleiner als 100px
	long d = long(rint(zoom / 10.0));
	long i = 1;
	while(i < d) { i *= 2; }
	d = 1000 * i;

//	cout << "gridsize=" << d << endl;

	SDL_Rect dst = { 0, 0, 0, 0 };

	// Abszissen-Linien (senkrechte Linien)
	for(long x = long(rint(floor(double(offset_x) / d) * d)); x < offset_x + long(rint(screen->w * zoom)); x += d)
	{
		long p_x = long(rint(double(x - offset_x) / zoom));
		drw->line(screen, p_x, 0, p_x, screen->h - 1, 0x22, 0x22, 0x22);

		// Beschriftung
		SDL_Surface* abszisse = grid_font->get_surface(str_stream() << x, 0x44, 0x44, 0x44);
		dst.x = p_x + 2; dst.y = 0;
		SDL_BlitSurface(abszisse, 0, screen, &dst);
		dst.x = p_x + 2; dst.y = screen->h - 1 - abszisse->h;
		SDL_BlitSurface(abszisse, 0, screen, &dst);
		SDL_FreeSurface(abszisse);
	}

	// Ordinaten-Linien (waagrechte Linien)
	for(long y = long(rint(floor(double(offset_y) / d) * d)); y < offset_y + long(rint(screen->h * zoom)); y += d)
	{
		long p_y = long(rint(double(y - offset_y) / zoom));
		drw->line(screen, 0, p_y, screen->w - 1, p_y, 0x22, 0x22, 0x22);

		// Beschriftung
		SDL_Surface* ordinate = grid_font->get_surface(str_stream() << y, 0x44, 0x44, 0x44);
		dst.x = 0; dst.y = p_y + 2;
		SDL_BlitSurface(ordinate, 0, screen, &dst);
		dst.x = screen->w - 1 - ordinate->w; dst.y = p_y + 2;
		SDL_BlitSurface(ordinate, 0, screen, &dst);
		SDL_FreeSurface(ordinate);
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
	static const long size = 250;

	if(((virt_x - size < planet->x) && (virt_x + virt_w + size >= planet->x))
	&& ((virt_y - size < planet->y) && (virt_y + virt_h + size >= planet->y)))
	{

		// *** konfigurierbar: groesse des planeten beruecksichtigen
		// *** benoetigt aber besseres caching sonst ist performance im arsch
		long h = long(rint(1.0 * size / zoom));
		h = (h < 5) ? 5 : h;
		SDL_Surface* surface = images->get_surface(planet->image, 0, h);

		SDL_Rect dst;
		double center_x = 1.0 * (planet->x - offset_x) / zoom;
		double center_y = 1.0 * (planet->y - offset_y) / zoom;
		dst.x = long(rint(center_x - h / 2));
		dst.y = long(rint(center_y - h / 2));

//		cout << "draw Planet (" << center_x << "/" << center_y << ")" << endl;

		SDL_BlitSurface(surface, 0, screen, &dst);

		if(h > 5)
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

			drw->circle(screen, long(rint(center_x)), long(rint(center_y)),
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
	static const long size = 100;

	if(((virt_x - size - schiff->v * 100 < schiff->x) && (virt_x + virt_w + size + schiff->v * 100 >= schiff->x))
	&& ((virt_y - size - schiff->v * 100 < schiff->y) && (virt_y + virt_h + size + schiff->v * 100 >= schiff->y)))
	{
		long h = long(rint(1.0 * size / zoom));
		h = (h < 10) ? 10 : h;

		double center_x = 1.0 * (schiff->x - offset_x) / zoom;
		double center_y = 1.0 * (schiff->y - offset_y) / zoom;

		/*
		 *         dx
		 *      +------.
		 *      ж     /
		 *      ж    /
		 *   dy ж   /
		 *      ж__/ v
		 *      жw/
		 *      ж/
		 *      o
		 *
		 *       w(rad) = w(░) / 180 * PI
		 *       dy = v * cos( w - 270 )
		 *       dx = v * sin( w - 270 )
		 */

		double target_x = center_x + sin(PI * (schiff->w) / 180.0) * schiff->v * 100.0 / zoom;
		double target_y = center_y - cos(PI * (schiff->w) / 180.0) * schiff->v * 100.0 / zoom;

//		cout << "draw Schiff (" << center_x << "/" << center_y << ")" << endl;

		drw->circle(screen, long(rint(center_x)), long(rint(center_y)), h/2,
		                     0xFF, 0xFF, 0xFF);
		drw->line(screen, long(rint(center_x)), long(rint(center_y)),
		                   long(rint(target_x)), long(rint(target_y)),
		                   0xFF, 0xFF, 0xFF);
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
		h = (h < 2) ? 2 : h;

		double center_x = 1.0 * (container->x - offset_x) / zoom;
		double center_y = 1.0 * (container->y - offset_y) / zoom;

//		cout << "draw Container (" << center_x << "/" << center_y << ")" << endl;

		drw->box(screen, long(rint(center_x)) - h/2, long(rint(center_y)) - h/2,
		                  long(rint(center_x)) + h/2, long(rint(center_y)) + h/2,
			              0xFF, 0xFF, 0xFF);
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
		double center_x = 1.0 * (anomalie->x - offset_x) / zoom;
		double center_y = 1.0 * (anomalie->y - offset_y) / zoom;

		if(anomalie->radius)
		{
			// Kreis
			long h = long(rint(1.0 * size / zoom));
			h = (h < 10) ? 10 : h;
			drw->circle(screen, long(rint(center_x)), long(rint(center_y)),
			                    h / 2, 0xFF, 0x00, 0x00, 0xFF);
		}

		// Zentrum
		long mitte_r = long(rint(1.0 * mitte_size / zoom));
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


