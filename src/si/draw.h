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

#ifndef DRAW_H
#define DRAW_H

#include "lib/sdl.h"
#include "lib/singleton.h"

class UVDraw : public Singleton<UVDraw>
{
	friend class Singleton<UVDraw>;

	public:
		void line(SDL_Surface* surface, long, long, long, long, Uint8, Uint8, Uint8, Uint8 = 0xFF) const;
		void box(SDL_Surface* surface, Sint16, Sint16, Sint16, Sint16, Uint8, Uint8, Uint8, Uint8 = 0xFF) const;
		void circle(SDL_Surface* surface, Sint16, Sint16, Sint16, Uint8, Uint8, Uint8, Uint8 = 0xFF) const;

	protected:
		UVDraw();
//		~UVDraw();

	private:
		bool antialiasing;
};

#endif // DRAW_H
