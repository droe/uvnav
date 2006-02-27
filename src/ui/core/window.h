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

#ifndef WINDOW_H
#define WINDOW_H

#include "ui/core/rect.h"
#include "ui/core/widget.h"
#include "ui/core/halign.h"
#include "ui/core/valign.h"
#include "util/sdl.h"

class UVWindow : public UVRect
{
public:
	UVWindow(UVWidget*, int = 0, int = 0, int = 0, int = 0, UVHAlign = UVHALeft, UVVAlign = UVVATop, bool = false);
	virtual ~UVWindow();

	virtual void draw(SDL_Surface*);
	virtual void resize();

protected:
	UVWidget* widget;
	SDL_Surface* canvas;
	UVHAlign halign;
	UVVAlign valign;
	bool autosize;
	int X, Y;
};

#endif // WINDOW_H

