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

#include "rootwindow.h"

/*
 * Konstruktor.
 */
UVRootWindow::UVRootWindow(UVWidget* wi, SDL_Surface* s)
: UVWindow(wi, s)
{
	conf = UVConf::get_instance();
	video = UVVideo::get_instance();
//	keyboard = UVKeyboard::get_instance();
	x = 0;
	y = 0;
	resize();
}

/*
 * Destruktor.
 */
UVRootWindow::~UVRootWindow()
{
}


/*
 * Berechnet das Layout des UVWindow neu.
 */
void UVRootWindow::resize()
{
	w = surface->w;
	h = surface->h;
	widget->w = w;
	widget->h = h;
	widget->resize();
}


/*
 * Zeichnet das UVRootWindow und alle enthaltenen Widgets.
 */
void UVRootWindow::draw()
{
	widget->draw();
}


/*
 * Mausklick-Event wird aufgerufen wenn der Benutzer auf die Flaeche des
 * Window klickt.  Der Event muss verarbeitet und ans richtige
 * Widget weitergegeben werden.
 */
void UVRootWindow::handle_click(int posx, int posy)
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
void UVRootWindow::set_surface(SDL_Surface* s)
{
	surface = s;

	widget->set_surface(s);

	resize();
}


/*
 * SDL Event Loop
 */
void UVRootWindow::run()
{
	running = true;
	while(running)
	{
		handle_event();
	}
}


/*
 * SDL Event Multiplexer.
 *
 * In einer eigenen Funktion, damit es auch aus update() heraus
 * aufgerufen werden kann waehrend einer zeitraubenden Operation.
 * Wird z.B. beim Parsen via Importer benoetigt.
 */
void UVRootWindow::handle_event()
{
	SDL_Event event;
	SDL_WaitEvent(&event);
	switch(event.type)
	{
		case SDL_VIDEORESIZE:
			conf->l_set("screen-width", event.resize.w);
			conf->l_set("screen-height", event.resize.h);
			video->init();
			set_surface(video->get_screen());
			break;
		case SDL_KEYDOWN:
//			keyboard->handle_keydown(event.key.keysym);
			break;
// *** Mouse: handle_click(x, y);
		case SDL_QUIT:
			running = false;
	}
}


