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

#include "window.h"

#include "si/draw.h"

/*
 * Konstruktor.
 */
GUIWindow::GUIWindow(GUIWidget* wi, SDL_Surface* s)
: GUIWidget(1, s), widget(wi)
{
	widget->set_surface(s);
}

/*
 * Destruktor.
 */
GUIWindow::~GUIWindow()
{
	delete widget;
}


/*
 * Berechnet das Layout des GUIWindow neu.
 */
void GUIWindow::resize()
{
	widget->x = x + 2;
	widget->y = y + 2;
	widget->w = w - 4;
	widget->h = h - 4;
	widget->resize();
}


/*
 * Zeichnet das GUIWindow und alle enthaltenen Widgets.
 */
void GUIWindow::draw()
{
	static UVDraw* drw = UVDraw::get_instance();

	drw->box(surface, x, y, x + w, y + h, 0x00, 0x00, 0xFF, 0x7F);
	widget->draw();
}


/*
 * Mausklick-Event wird aufgerufen wenn der Benutzer auf die Flaeche des
 * Window klickt.  Der Event muss verarbeitet und ans richtige
 * Widget weitergegeben werden.
 */
void GUIWindow::handle_click(int posx, int posy)
{
	if(widget->contains(posx, posy))
	{
		widget->handle_click(posx, posy);
	}
}


/*
 * Setzt die Zeichenflaeche, auf welche gezeichnet werden soll.
 * Alle Koordinaten beziehen sich auf diese Surface.
 *
 * Muss ueberschrieben werden, damit allen enthaltenen Widgets ebenfalls
 * die Surface gesetzt wird.
 */
void GUIWindow::set_surface(SDL_Surface* s)
{
	surface = s;

	widget->set_surface(s);
}


