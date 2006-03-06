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

#include "widget.h"

#include "si/draw.h"

/*
 * Konstruktor.
 */
UVWidget::UVWidget(int we, SDL_Surface* s)
: weight(we), surface(s)
{
	min.w = 0;
	min.h = 0;
	max.w = 10000;
	max.h = 10000;
}

/*
 * Destruktor.
 */
UVWidget::~UVWidget()
{
}


/*
 * Signalisiert dem UVWidget, dass seine effektive Groesse geaendert hat.
 * Wird vom umgebenden UVCompositeWidget aufgerufen.
 */
void UVWidget::resize()
{
	// ignore
}


/*
 * Zeichnet das UVWidget auf die SDL_Surface surface.
 */
void UVWidget::draw()
{
//	static UVDraw* drw = UVDraw::get_instance();
	// *** FIXME
//	drw->box(surface, x+2, y+2, x+w-5, y+h-5, 0xFF, 0x00, 0x00, 0xFF);
}


/*
 * Einen Mausklick auf das UVWidget behandeln.
 */
void UVWidget::handle_click(int, int)
{
	// ignore
}


/*
 * Setzt / holt die Zeichenflaeche, auf welche gezeichnet werden soll.
 * Alle Koordinaten beziehen sich auf diese Surface.
 */
void UVWidget::set_surface(SDL_Surface* s)
{
	surface = s;
}
SDL_Surface* UVWidget::get_surface() const
{
	return surface;
}


