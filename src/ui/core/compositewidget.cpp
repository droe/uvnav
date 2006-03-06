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

#include "compositewidget.h"

#include "si/draw.h"
#include "util/minmax.h"

/*
 * Konstruktor.
 */
UVCompositeWidget::UVCompositeWidget(int we, UVOrientation o, SDL_Surface* s)
: UVWidget(we, s), modified(false), orientation(o), weight_total(0)
{
}

/*
 * Destruktor.
 */
UVCompositeWidget::~UVCompositeWidget()
{
	int count = widgets.size();
	for(int i = 0; i < count; i++)
	{
		delete widgets[i];
	}
}


/*
 * Wird aufgerufen, wenn enthaltene Widgets aendern.
 */
void UVCompositeWidget::update(Subject *subject)
{
	notify();
}


/*
 * Fuegt ein Widget hinzu.  Reihenfolge ist signifikant.
 */
void UVCompositeWidget::add_widget(UVWidget* wi)
{
	wi->set_surface(surface);
	wi->attach(this);

	// min/max aktualisieren
	if(orientation == UVOHorizontal)
	{
		min.h = max(min.h, wi->min.h);
		max.h = max(max.h, wi->max.h);
		min.w += wi->min.w;
		max.w += wi->max.w;
	}
	else // UVOVertical
	{
		min.h += wi->min.h;
		max.h += wi->max.h;
		min.w = max(min.w, wi->min.w);
		max.w = max(max.w, wi->max.w);
	}
	weight_total += wi->weight;

	widgets.push_back(wi);
	modified = true;
}


/*
 * Berechnet das Layout des CompositeWidget und aller enthaltenen Widgets
 * und aktualisiert min/max.
 */
void UVCompositeWidget::resize()
{
/*
cerr << "===> UVCompositeWidget::resize()" << endl;
cerr << "x=" << x << endl;
cerr << "y=" << y << endl;
cerr << "w=" << w << endl;
cerr << "h=" << h << endl;
cerr << "min.x=" << min.x << endl;
cerr << "min.y=" << min.y << endl;
cerr << "min.w=" << min.w << endl;
cerr << "min.h=" << min.h << endl;
cerr << "max.x=" << max.x << endl;
cerr << "max.y=" << max.y << endl;
cerr << "max.w=" << max.w << endl;
cerr << "max.h=" << max.h << endl;
*/
	int count = widgets.size();

	/*
	 * Layout manager code.
	 */

	int dyn = 0;
	if(orientation == UVOHorizontal) { dyn = w - min.w; }
	else /* UVOVertical */           { dyn = h - min.h; }
	dyn = max(0, dyn);
/*
cerr << "dyn=" << dyn << endl;
*/
	int last_x = x;
	int last_y = y;

	// loop all contained widgets
	for(int i = 0; i < count; i++)
	{
/*
cerr << "===> widget[" << i << "]" << endl;

cerr << "last_x=" << last_x << endl;
cerr << "last_y=" << last_y << endl;
*/
		UVWidget* widget = widgets[i];

		if(orientation == UVOHorizontal)
		{
			widget->x = last_x;
			widget->y = last_y;
			widget->w = min(dyn * widget->weight / weight_total + widget->min.w,
			                widget->max.w);
			widget->h = min(h, widget->max.h);
			last_x += widget->w;
		}
		else // UVOVertical
		{
			widget->x = last_x;
			widget->y = last_y;
			widget->h = min(dyn * widget->weight / weight_total + widget->min.h,
			                widget->max.h);
			widget->w = min(w, widget->max.w);
			last_y += widget->h;
		}
/*
cerr << "widgets[" << i << "]->x=" << widget->x << endl;
cerr << "widgets[" << i << "]->y=" << widget->y << endl;
cerr << "widgets[" << i << "]->w=" << widget->w << endl;
cerr << "widgets[" << i << "]->h=" << widget->h << endl;
*/
		widget->resize();
	}

	// clear modified flag
	modified = false;
}


/*
 * Zeichnet das UVCompositeWidget und alle enthaltenen Widgets.
 */
void UVCompositeWidget::draw()
{
//	static UVDraw* drw = UVDraw::get_instance();

	if(modified)
		resize();

	// *** FIXME
//	drw->box(surface, x, y, x + w - 1, y + h - 1, 0x00, 0xFF, 0x00, 0x7F);

	int count = widgets.size();
	for(int i = 0; i < count; i++)
	{
		widgets[i]->draw();
	}
}


/*
 * Mausklick-Event wird aufgerufen wenn der Benutzer auf die Flaeche des
 * UVCompositeWidgets klickt.  Der Event muss verarbeitet und ans richtige
 * UVWidget weitergegeben werden.
 */
void UVCompositeWidget::handle_click(int posx, int posy)
{
	int count = widgets.size();
	for(int i = 0; i < count; i++)
	{
		if(widgets[i]->contains(posx, posy))
		{
			widgets[i]->handle_click(posx, posy);
			break;
		}
	}
}


/*
 * Setzt die Zeichenflaeche, auf welche gezeichnet werden soll.
 * Alle Koordinaten beziehen sich auf diese Surface.
 *
 * Muss ueberschrieben werden, damit allen enthaltenen Widgets ebenfalls
 * die Surface gesetzt wird.
 */
void UVCompositeWidget::set_surface(SDL_Surface* s)
{
	surface = s;

	int count = widgets.size();
	for(int i = 0; i < count; i++)
	{
		widgets[i]->set_surface(s);
	}
}


