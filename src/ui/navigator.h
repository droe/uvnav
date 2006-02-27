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

#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "ui/core/window.h"
#include "util/sdl.h"
#include "util/stl_string.h"
#include <vector>

class UVConf;
class UVFont;
class UVMap;
class UVUniversum;
class UVSpieler;

class UVNavigator
{
	public:
		UVNavigator();
		virtual ~UVNavigator();

		void splash();
		void load(const std::string&, int = 0);
		void wait();
		void run();

	private:
		UVUniversum *universum;
		UVSpieler *spieler;
		// *** GUI
		UVConf *conf;
		UVFont *font_splash;

		SDL_Surface *screen;

		long status_y;
		void splash_status(const std::string&);

		void init_video();

		void vid_reinit(UVMap*, SDL_Surface*&, SDL_Rect&);
		void vid_redraw(SDL_Surface*&, std::vector<UVWindow*>&, SDL_Rect&);

		std::string title_string(int);
};

#endif // NAVIGATOR_H

