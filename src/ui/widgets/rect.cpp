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

#include "rect.h"

/*
 * Konstruktor.
 */
UVRect::UVRect(int _x, int _y, int _w, int _h)
: UVSize(_w, _h), x(_x), y(_y)
{
}

/*
 * Destruktor.
 */
UVRect::~UVRect()
{
}


/*
 * Enthaelt dieses Rect den Punkt (x, y)?
 */
bool UVRect::contains(int px, int py) const
{
	return (px >= x)
	    && (px <  x + w)
	    && (py >= y)
	    && (py <  y + h);
}


/*
 * Gibt ein SDL_Rect zurueck, das dem UVRect entspricht.
 */
SDL_Rect* UVRect::to_sdl_rect() const
{
	static SDL_Rect r;
	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;
	return &r;
}


