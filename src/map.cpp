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
 * UVMap Zeichnet UVWelt. UVWelt ist die Datenstruktur, welche von UVMap
 * iteriert und gezeichnet wird. UVWelt hat lediglich eine passive Rolle
 * in der Bildschirmdarstellung.
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
	debug_font = new UVFont(conf, FNT_SANS, conf->l_get("map-debug-font-size"));
	grid_font = new UVFont(conf, FNT_SANS, conf->l_get("map-grid-font-size"));
	label_font = new UVFont(conf, FNT_SANS, conf->l_get("map-label-font-size"));
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
	delete debug_font;
	delete grid_font;
	delete label_font;
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

	offset_x -= long(rint(double(screen->w) * zoom * (f - 1.0) / 2.0));
	offset_y -= long(rint(double(screen->h) * zoom * (f - 1.0) / 2.0));
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
	double wf = double(screen_size.w) / double(screen->w);
//	double hf = double(screen_size.h) / double(screen->h);
//	cout << "factors wf=" << wf << " hf=" << hf << endl;

	screen_size.w = screen->w;
//	screen_size.h = screen->h;

	zoom *= wf;

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

	long ticks = SDL_GetTicks();

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


	// Overlays...
	SDL_Rect dst;

	// Status-Overlay
	SDL_Surface* status = debug_font->get_surface(
		str_stream() << dim << " - " << welt->get_dim(dim) << ", "
		             << welt->get_spieler()->name
		             << ", Sternzeit " << welt->sternzeit, 0x88, 0x88, 0x88);
	dst.x = screen->w - status->w - status->h * 2;
	dst.y = status->h;
	drw->box(screen, dst.x - status->h / 4, dst.y, dst.x + status->w + status->h / 4, dst.y + status->h, 0, 0, 0, 0x88);
	SDL_BlitSurface(status, 0, screen, &dst);

	// Debug-Overlay
	long dticks = SDL_GetTicks() - ticks;
	SDL_Surface* debug = debug_font->get_surface(
		str_stream() << "debug  /  zoom=" << zoom
		             << " offset_x=" << offset_x
		             << " offset_y=" << offset_y
		             << "  /  " << dticks << " ms = "
		             << 1000 / dticks << " fps  /  uvnav-" << PACKAGE_VERSION
		             << " (" << revision << ")"
		, 0x88, 0x88, 0x88);
	dst.x = screen->w - debug->w - debug->h * 2;
	dst.y = screen->h - debug->h * 2;
	drw->box(screen, dst.x - debug->h / 4, dst.y, dst.x + debug->w + debug->h / 4, dst.y + debug->h, 0, 0, 0, 0x88);
	SDL_BlitSurface(debug, 0, screen, &dst);

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
	static const long tick = conf->l_get("map-grid-font-size") / 2;

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
		SDL_Surface* abszisse = grid_font->get_surface(str_stream() << x, 0x44, 0x44, 0x44);
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
		SDL_Surface* ordinate = grid_font->get_surface(str_stream() << y, 0x44, 0x44, 0x44);
		dst.x = 0; dst.y = p_y + 2;
		SDL_BlitSurface(ordinate, 0, screen, &dst);
		dst.x = screen->w - 1 - ordinate->w; dst.y = p_y + 2;
		SDL_BlitSurface(ordinate, 0, screen, &dst);
		SDL_FreeSurface(ordinate);

		last_y = y;
	}

	// Massstabsbalken mit Laengenangabe d und Massstab 1:zoom
	SDL_Surface* distanz = grid_font->get_surface(str_stream() << d << " KE", 0x88, 0x88, 0x88);
	SDL_Surface* massstab = grid_font->get_surface(str_stream() << "1:" << long(rint(zoom)), 0x88, 0x88, 0x88);
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
		SDL_Surface* x_label = grid_font->get_surface(str_stream() << "X", 0x88, 0x88, 0x88);
		dst.x = s_x2 + tick;
		dst.y = s_y1 - x_label->h / 2;
		SDL_BlitSurface(x_label, 0, screen, &dst);
		SDL_FreeSurface(x_label);
		SDL_Surface* y_label = grid_font->get_surface(str_stream() << "Y", 0x88, 0x88, 0x88);
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
		UVPlanet* p = welt->get_planet((*iter));
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
	if(((virt_x - size < planet->x) && (virt_x + virt_w + size >= planet->x))
	&& ((virt_y - size < planet->y) && (virt_y + virt_h + size >= planet->y)))
	{
		// Planetenbild
		// *** konfigurierbar: groesse des planeten beruecksichtigen
		// *** benoetigt aber besseres caching sonst ist performance im arsch
		SDL_Surface* surface = images->get_surface(planet->image, 0, h);
		SDL_Rect dst;
		dst.x = long(rint(center_x - h / 2));
		dst.y = long(rint(center_y - h / 2));
		SDL_BlitSurface(surface, 0, screen, &dst);

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
			// 7-9: 00 00 ff
			// 4-6: ff ff 00
			// 1-3: ff 00 00
			short r = (tl > 6) ? 0x00 : 0xFF;
			short g = ((tl == 10) || ((tl >= 4) && (tl <= 6))) ? 0xFF : 0x00;
			short b = ((tl >= 7) && (tl <= 9)) ? 0xFF : 0x00;

			drw->circle(screen, long(rint(center_x)), long(rint(center_y)),
			                    h / 2 + 2, r, g, b, 0xFF);
		}

		if(zoom < 40.0)
		{
			// Beschriftung
			// *** provisorisch
			string label_text = "";
			if(welt->galaxie != "")
			{
				label_text = str_stream() << planet->name << " (" << planet->nummer << ")";
			}
			else if(zoom < 10.0)
			{
				label_text = str_stream() << planet->nummer << " " << planet->name;
			}
			else if(zoom < 20.0)
			{
				label_text = str_stream() << planet->nummer << " " << planet->name.substr(0,3);
			}
			else
			{
				label_text = str_stream() << planet->nummer;
			}
			SDL_Surface* label = label_font->get_surface(label_text, 0x88, 0x88, 0x88);
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

	// *** Vektor nur fuer selektiertes Schiff zeichnen
	if(((virt_x - size - schiff->v * 100 < schiff->x) && (virt_x + virt_w + size + schiff->v * 100 >= schiff->x))
	&& ((virt_y - size - schiff->v * 100 < schiff->y) && (virt_y + virt_h + size + schiff->v * 100 >= schiff->y)))
	{
		long h = long(rint(double(size) / zoom));
		h = (h < 10) ? 10 : h;

		double center_x = double(schiff->x - offset_x) / zoom;
		double center_y = double(schiff->y - offset_y) / zoom;

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
				label_text = str_stream() << schiff->name << " (" << schiff->besitzer << ")";
			}
			else
			{
				label_text = str_stream() << schiff->name;
			}
			SDL_Rect dst = { 0, 0, 0, 0};
			SDL_Surface* label = label_font->get_surface(label_text, 0x88, 0x88, 0x88);
			dst.x = long(rint(center_x + h / 2)) + 4;
			dst.y = long(rint(center_y - label->h / 2));
			drw->box(screen, dst.x - label->h / 4, dst.y, dst.x + label->w + label->h / 4, dst.y + label->h, 0, 0, 0, 0x88);
			SDL_BlitSurface(label, 0, screen, &dst);
			SDL_FreeSurface(label);
		}

//		cout << "draw Schiff (" << center_x << "/" << center_y << ")" << endl;
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


