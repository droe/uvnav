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

#include "draw.h"

/*
 * UVDraw - Grafische Formen zeichnen.
 *
 * Zeichnet direkt in die Surface.
 */


/*
 * Konstruktor.
 */
UVDraw::UVDraw(const UVConf* conf)
{
	antialiasing = conf->b_get("screen-quality");
}


/*
 * Destruktor.
 */
/*
UVDraw::~UVDraw()
{
}
*/


/*
 * Linie zeichnen.
 */
#define MAX_PHYS 32000
#define MIN_PHYS -MAX_PHYS
void UVDraw::line(SDL_Surface* surface, long x1, long y1, long x2, long y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) const
{
// WORKAROUND bis UVMap sich besser an die Regeln h�lt und Linien selber
// runterrechnen kann.

//cout << "DEBUG1 line (" << x1 << "," << y1 << ")-(" << x2 << "," << y2 << ")" << endl;

	double m = (double(y2) - double(y1)) / (double(x2) - double(x1));
	if(x1 > MAX_PHYS)
	{
		y1 = long(rint(double(MAX_PHYS) * m));
		x1 = MAX_PHYS;
	}
	if(x1 < MIN_PHYS)
	{
		y1 = long(rint(double(MIN_PHYS) * m));
		x1 = MIN_PHYS;
	}
	if(y1 > MAX_PHYS)
	{
		x1 = long(rint(double(MAX_PHYS) / m));
		y1 = MAX_PHYS;
	}
	if(y1 < MIN_PHYS)
	{
		x1 = long(rint(double(MIN_PHYS) / m));
		y1 = MIN_PHYS;
	}
	if(x2 > MAX_PHYS)
	{
		y2 = long(rint(double(MAX_PHYS) * m));
		x2 = MAX_PHYS;
	}
	if(x2 < MIN_PHYS)
	{
		y2 = long(rint(double(MIN_PHYS) * m));
		x2 = MIN_PHYS;
	}
	if(y2 > MAX_PHYS)
	{
		x2 = long(rint(double(MAX_PHYS) / m));
		y2 = MAX_PHYS;
	}
	if(y2 < MIN_PHYS)
	{
		x2 = long(rint(double(MIN_PHYS) / m));
		y2 = MIN_PHYS;
	}

//cout << "DEBUG2 line (" << x1 << "," << y1 << ")-(" << x2 << "," << y2 << ")" << endl;

	if(antialiasing)
	{
		aalineRGBA(surface, x1, y1, x2, y2, r, g, b, a);
	}
	else
	{
		lineRGBA(surface, x1, y1, x2, y2, r, g, b, a);
	}
}


/*
 * Box zeichnen.
 */
void UVDraw::box(SDL_Surface* surface, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) const
{
	boxRGBA(surface, x1, y1, x2, y2, r, g, b, a);
}


/*
 * Kreis zeichnen.
 */
void UVDraw::circle(SDL_Surface* surface, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a) const
{
	// cout << "DEBUG: circle x=" << long(x) << " y=" << long(y) << " r=" << long(rad) << endl;
	if(antialiasing)
	{
		if(rad > 1000)
		{
			cerr << "Interne Warnung in UVDraw: Kreis zu gross fuer aacircle(), verwende circle()!" << endl;
			circleRGBA(surface, x, y, rad, r, g, b, a);
		}
		else
		{
			aacircleRGBA(surface, x, y, rad, r, g, b, a);
		}
	}
	else
	{
		circleRGBA(surface, x, y, rad, r, g, b, a);
	}
}


