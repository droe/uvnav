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

#include "window.h"

#include "si/draw.h"
#include "si/video.h"
#include "util/minmax.h"

/*
 * Konstruktor.
 */
UVWindow::UVWindow(UVWidget* wi, int x_, int y_, int w_, int h_, UVHAlign ha, UVVAlign va, bool as)
: UVRect(x_, y_, w_, h_), widget(wi), canvas(NULL), halign(ha), valign(va), autosize(as), X(x_), Y(y_)
{
	resize();
}


/*
 * Destruktor.
 */
UVWindow::~UVWindow()
{
	if(canvas)
		SDL_FreeSurface(canvas);
	delete widget;
}


/*
 * Berechnet das Layout des UVWindow neu.
 */
void UVWindow::resize()
{
	if(autosize) {
//		widget->autosize();
		h = widget->min.h + 4;
		w = widget->min.w + 4;
	}

	if(canvas)
		SDL_FreeSurface(canvas);
	canvas = UVVideo::get_instance()->create_surface(SDL_SRCALPHA, w, h);
	widget->set_surface(canvas);

	if(!autosize) {
		widget->x = 2;
		widget->y = 2;
		widget->h = h - 4;
		widget->w = w - 4;
		widget->resize();
	}
}


/*
 * Zeichnet das UVWindow und alle enthaltenen Widgets.
 */
void UVWindow::draw(SDL_Surface* surface)
{
	// TODO: separate draw and blit
	SDL_FillRect(canvas, NULL, SDL_MapRGBA(canvas->format, 0, 0, 0, 0xAF));
	widget->draw();

	switch(halign) {
	case UVHALeft:
		// ignore
		break;
	case UVHACenter:
		x = max(0, X - ((w - 1) / 2));
		break;
	case UVHARight:
		x = max(0, (surface->w - 1) - X - (w - 1));
		break;
	}
	switch(valign) {
	case UVVATop:
		// ignore
		break;
	case UVVAMiddle:
		y = max(0, Y - ((h - 1) / 2));
		break;
	case UVVABottom:
		y = max(0, (surface->h - 1) - Y - (h - 1));
		break;
	}

	SDL_Rect rect = { x, y, x + w - 1, y + h - 1};
	SDL_BlitSurface(canvas, NULL, surface, &rect);
}


/*
 * Mausklick-Event wird aufgerufen wenn der Benutzer auf die Flaeche des
 * Window klickt.  Der Event muss verarbeitet und ans richtige
 * Widget weitergegeben werden.
 */
/*
void UVWindow::handle_click(int posx, int posy)
{
	if(widget->contains(posx, posy))
	{
		widget->handle_click(posx, posy);
	}
}
*/


