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

#ifndef WINDOW_H
#define WINDOW_H

#include "ui/core/widget.h"

class UVWindow : public UVWidget
{
	public:
		UVWindow(UVWidget*, SDL_Surface*);
		~UVWindow();

		virtual void resize();
		virtual void draw();
		virtual void handle_click(int, int);

		virtual void set_surface(SDL_Surface*);

	protected:
		UVWidget* widget;
		SDL_Surface* my_surface;
};

#endif // WINDOW_H

