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
 * Kreis zeichnen.
 */
void UVDraw::circle(SDL_Surface* surface, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a) const
{
	if(antialiasing)
	{
		aacircleRGBA(surface, x, y, rad, r, g, b, a);
	}
	else
	{
		circleRGBA(surface, x, y, rad, r, g, b, a);
	}
}


