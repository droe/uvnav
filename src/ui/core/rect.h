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

#ifndef RECT_H
#define RECT_H

#include "lib/sdl.h"
#include "ui/core/size.h"

class UVRect : public UVSize
{
	public:
		UVRect(int = 0, int = 0, int = 0, int = 0);
		~UVRect();

		bool contains(int, int) const;
		SDL_Rect* to_sdl_rect() const;

		int x;
		int y;
};

#endif // RECT_H

