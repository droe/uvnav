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

#include "widget.h"

#include "si/draw.h"

/*
 * Konstruktor.
 */
Widget::Widget(unsigned int w)
: weight(w), surface(NULL)
{
	min_rect.x = 0;
	min_rect.y = 0;
	min_rect.w = 0;
	min_rect.h = 0;
	max_rect.x = 0;
	max_rect.y = 0;
	max_rect.w = 10000;
	max_rect.h = 10000;
	eff_rect.x = 0;
	eff_rect.y = 0;
	eff_rect.w = 0;
	eff_rect.h = 0;
}

/*
 * Destruktor.
 */
Widget::~Widget()
{
}


/*
 * Signalisiert dem Widget, dass seine effektive Groesse geaendert hat.
 * Wird vom umgebenden CompositeWidget aufgerufen.
 */
void Widget::resize()
{
	// ignore
}


/*
 * Zeichnet das Widget auf die SDL_Surface surface.
 */
void Widget::draw()
{
	static UVDraw* drw = UVDraw::get_instance();

	// *** FIXME
	drw->box(surface, eff_rect.x + 2, eff_rect.y + 2,
		eff_rect.x + eff_rect.w - 2, eff_rect.y + eff_rect.h - 2,
		0xFF, 0x00, 0x00, 0x7F);
}


/*
 * Einen Mausklick auf das Widget behandeln.
 */
void Widget::handle_click(SDL_Rect*)
{
	// ignore
}


/*
 * Setzt / golt das relative Gewicht des Widgets.
 */
void Widget::set_weight(unsigned int w)
{
	weight = w;
}
unsigned int Widget::get_weight() const
{
	return weight;
}


/*
 * Setzt / holt die Mindest- und Maximalgroesse des Widgets.
 * Wird in der Regel vom Widget selber veraendert.
 * Bei einigen Widgets aber kann dies vom Benutzer aufgerufen werden.
 */
void Widget::set_min_rect(const SDL_Rect* r)
{
	min_rect.w = r->w;
	min_rect.h = r->h;
}
const SDL_Rect* Widget::get_min_rect() const
{
	return &min_rect;
}
void Widget::set_max_rect(const SDL_Rect* r)
{
	max_rect.w = r->w;
	max_rect.h = r->h;
}
const SDL_Rect* Widget::get_max_rect() const
{
	return &max_rect;
}


/*
 * Setzt / holt die effektiv erhaltene Groesse des Widgets.
 * Wird vom umgebenden CompositeWidget aufgerufen.
 */
void Widget::set_eff_rect(const SDL_Rect* r)
{
	eff_rect.x = r->x;
	eff_rect.y = r->y;
	eff_rect.w = r->w;
	eff_rect.h = r->h;
}
SDL_Rect* Widget::get_eff_rect()
{
	return &eff_rect;
}


/*
 * Setzt / holt die Zeichenflaeche, auf welche gezeichnet werden soll.
 * Alle Koordinaten beziehen sich auf diese Surface.
 */
void Widget::set_surface(SDL_Surface* s)
{
	surface = s;
}
SDL_Surface* Widget::get_surface() const
{
	return surface;
}


/*
 * Enthaelt dieses Widget den Punkt (r->x,r->y)?
 */
bool Widget::contains(SDL_Rect* r) const
{
	return (r->x >= eff_rect.x)
	    && (r->x <  eff_rect.x + eff_rect.w)
	    && (r->y >= eff_rect.y)
	    && (r->y <  eff_rect.y + eff_rect.h);
}

