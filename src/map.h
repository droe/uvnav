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

#ifndef MAP_H
#define MAP_H

#include "global.h"

#include "conf.h"
#include "images.h"
#include "font.h"
#include "draw.h"
#include "welt.h"

/*
class UVMapSektor
{
	public:
		UVMapSektor();
//		~UVMapSektor();

	private:
};
*/
/*
class UVMapQuadrant
{
	public:
		UVMapQuadrant();
//		~UVMapQuadrant();

	private:
};
*/
class UVMap
{
	public:
		UVMap(UVConf*, UVImages*, UVWelt*, SDL_Surface*);
		~UVMap();

		void set_dim(long);
		long get_dim();
		void scroll(long, long);
		void zoom_in();
		void zoom_out();
		void zoom_by(double);
		void resize(SDL_Surface*);
		void jump_init();
		void jump_alle();
		void jump_eigene();

		void draw(SDL_Rect*);

	private:
		UVConf* conf;
		UVImages* images;
		UVWelt* welt;
		SDL_Surface* screen;
		SDL_Rect screen_size;

		SDL_Rect* phys;
		long virt_x, virt_y, virt_w, virt_h;
		long offset_x, offset_y;
		double zoom;
		long dim;

		long alle_x1, alle_y1, alle_x2, alle_y2;
		long eigene_x1, eigene_y1, eigene_x2, eigene_y2;

		UVDraw* drw;
		UVFont* debug_font;
		UVFont* grid_font;
		UVFont* label_font;

		void draw_grid();

		void draw_planet(UVPlanet*);
		void draw_schiff(UVSchiff*);
		void draw_container(UVContainer*);
		void draw_anomalie(UVAnomalie*);
		void draw_sensorsonde(UVSensorsonde*);
		void draw_infosonde(UVInfosonde*);
};

#endif // MAP_H

