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

#ifndef COMPOSITEWIDGET_H
#define COMPOSITEWIDGET_H

#include "lib/stl.h"
#include "ui/widgets/orientation.h"
#include "ui/widgets/widget.h"

class GUICompositeWidget : public GUIWidget
{
	public:
		GUICompositeWidget(int = 1, GUIOrientation = GUIOHorizontal, SDL_Surface* = NULL);
		~GUICompositeWidget();

		virtual void add_widget(GUIWidget*);

		virtual void resize();
		virtual void draw();
		virtual void handle_click(int, int);

		virtual void set_surface(SDL_Surface*);

	protected:
		vector<GUIWidget*> widgets;
		bool modified;
		GUIOrientation orientation;
		int weight_total;
};

#endif // COMPOSITEWIDGET_H

