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

#ifndef VIDEO_H
#define VIDEO_H

#include "util/sdl.h"
#include "util/singleton.h"

class UVConf;

class UVVideo : public Singleton<UVVideo>
{
	friend class Singleton<UVVideo>;

	public:
		void init();
		SDL_Surface* get_screen();
		SDL_Surface* create_surface(Uint32 flags, int width, int height);
		void dispose();

	protected:
		UVVideo();
		virtual ~UVVideo();

	private:
		SDL_Surface* screen;
		UVConf* conf;
};

#endif // VIDEO_H
