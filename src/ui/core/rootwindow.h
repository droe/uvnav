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

#ifndef ROOTWINDOW_H
#define ROOTWINDOW_H
#if 0
#include "ui/core/window.h"

class UVConf;
class UVVideo;
//class UVKeyboard;

class UVRootWindow : public UVWindow
{
	public:
		UVRootWindow(UVWidget*, SDL_Surface*);
		virtual ~UVRootWindow();

		virtual void resize();
		virtual void draw();
		virtual void handle_click(int, int);

		virtual void set_surface(SDL_Surface*);

		void run();

	protected:
		void handle_event();

	private:
		bool running;
		UVConf* conf;
		UVVideo* video;
//		UVKeyboard* keyboard;
};
#endif
#endif // ROOTWINDOW_H

