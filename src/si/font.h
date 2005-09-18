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

#ifndef FONT_H
#define FONT_H

#include "util/stl_string.h"
#include "util/sdl.h"

class UVFont
{
	friend class UVFontHandler;

	public:
		SDL_Surface* get_surface(const std::string& text, const SDL_Color& color) const;
		SDL_Surface* get_surface(const std::string& text, int r, int g, int b, int a = 0xFF) const;
		SDL_Surface* get_surface(const std::string& text) const;

	protected:
		UVFont(const std::string& file, int size);
		virtual ~UVFont();

	private:
		TTF_Font* font;

		bool antialiasing;
};

#endif // FONT_H

