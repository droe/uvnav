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

#ifndef MAP_H
#define MAP_H

#include "util/sdl.h"

class UVImageHandler;
class UVFont;
class UVDraw;
class UVUniversum;
class UVSpieler;
class UVPlanet;
class UVSchiff;
class UVContainer;
class UVAnomalie;
class UVSensorsonde;
class UVInfosonde;

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
		UVMap(UVUniversum*, SDL_Surface*);
		virtual ~UVMap();

		void set_dim(long);
		long get_dim() const;

		void scroll(long, long);

		void zoom_in();
		void zoom_out();
		void zoom_by(double);

		void resize(SDL_Surface*);

		void jump_init();
		void jump_alle();
		void jump_eigene();

		long get_opt_sichtradien() const;
		void set_opt_sichtradien(const long);
		void toggle_opt_sichtradien();

		bool get_opt_kaufradien() const;
		void set_opt_kaufradien(const bool);
		void toggle_opt_kaufradien();

		bool get_opt_schiffe() const;
		void set_opt_schiffe(const bool);
		void toggle_opt_schiffe();

		bool get_opt_container() const;
		void set_opt_container(const bool);
		void toggle_opt_container();

		bool get_opt_verbindungen() const;
		void set_opt_verbindungen(const bool);
		void toggle_opt_verbindungen();

		long get_opt_zonen() const;
		void set_opt_zonen(const long);
		void toggle_opt_zonen();

		void redraw();
		void draw(SDL_Rect*);

		//long v2phys_x(long) const;
		//long v2phys_y(long) const;
		long p2virt_x(long) const;
		long p2virt_y(long) const;

	private:
		UVImageHandler* imagehandler;
		UVUniversum* universum;
		UVSpieler* spieler;
		SDL_Surface* canvas;
		SDL_Rect old_canvas_size;

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
		UVFont* zonen_font;

		void draw_grid();

		void draw_planet(UVPlanet*);
		void draw_schiff(UVSchiff*);
		void draw_container(UVContainer*);
		void draw_anomalie(UVAnomalie*);
		void draw_sensorsonde(UVSensorsonde*);
		void draw_infosonde(UVInfosonde*);

		long opt_sichtradien;
		long opt_zonen;
		bool opt_kaufradien;
		bool opt_schiffe;
		bool opt_container;
		bool opt_verbindungen;
};

#endif // MAP_H

