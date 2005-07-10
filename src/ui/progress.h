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

#ifndef PROGRESS_H
#define PROGRESS_H

#include "util/observer.h"
#include "util/sdl.h"
#include "util/stlstring.h"
#include "si/font.h"

class UVProgress : public Observer
{
	public:
		UVProgress(SDL_Surface*, SDL_Rect*);
//		~UVProgress();

		void init(unsigned long);
		void update(Subject*);

	private:
		UVFont* font;
		SDL_Surface* screen;
		SDL_Rect rect;
		string message;
		unsigned long total;
		long ticks;
};

#endif // PROGRESS_H

