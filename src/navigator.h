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

#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "global.h"

#include "conf.h"
#include "welt.h"
#include "font.h"
#include "images.h"
#include "map.h"

class UVNavigator
{
	public:
		UVNavigator(UVConf*);
		~UVNavigator();

		void splash();
		void load(const string&, int = 0);
		void wait();
		void run();

	private:
		UVWelt* welt;
		UVMap* map;
		// *** GUI
		UVConf* conf;
		UVImages* images;
		UVFont* font_splash;

		SDL_Surface* screen;

		long status_y;
		void splash_status(const string&);

		void init_video();
};

#endif // NAVIGATOR_H

