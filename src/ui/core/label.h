/*
 * UV Navigator - Auswertungsvisualisierung fuer Universum V
 * Copyright (C) 2004-2006 Daniel Roethlisberger <roe@chronator.ch>
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

#ifndef LABEL_H
#define LABEL_H

#include "util/sdl.h"
#include "ui/core/widget.h"
#include "util/stl_string.h"

class UVFont;

class UVLabel : public UVWidget
{
	public:
		UVLabel(std::string = " ", int = 1, SDL_Surface* = NULL);
		virtual ~UVLabel();

		virtual void draw();

		virtual void set_text(std::string);

	protected:
		UVFont *font;
		SDL_Surface *text;
};

#endif // LABEL_H

