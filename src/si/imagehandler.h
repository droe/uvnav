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

#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H

#include "lib/singleton.h"
#include "lib/sdl.h"

#define IMG_UNIVERSUM	 0
#define IMG_SDL_POWERED	 1
#define IMG_PLANET_01	 2
#define IMG_PLANET_02	 3
#define IMG_PLANET_03	 4
#define IMG_PLANET_04	 5
#define IMG_PLANET_05	 6
#define IMG_PLANET_06	 7
#define IMG_PLANET_07	 8
#define IMG_PLANET_08	 9
#define IMG_PLANET_09	10
#define IMG_PLANET_10	11
#define NUM_IMG			12

struct uv_img
{
	SDL_Surface* original;
	SDL_Surface* resultat;
	double faktor;
};

class UVImageHandler : public Singleton<UVImageHandler>
{
	friend class Singleton<UVImageHandler>;

	public:
		SDL_Surface* get_surface(const long id, const double f = 1.0);
		SDL_Surface* get_surface(const long id, const int w, const int h);

	protected:
		UVImageHandler();
		~UVImageHandler();

	private:
		uv_img images[NUM_IMG];

		int smoothing;
};

#endif // IMAGEHANDLER_H

