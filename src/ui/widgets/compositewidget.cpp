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

#include "compositewidget.h"

#include "lib/minmax.h"
#include "si/draw.h"

/*
 * Konstruktor.
 */
CompositeWidget::CompositeWidget(unsigned int w, CWOrientation o)
: Widget(w), modified(false), orientation(o), weight_total(0)
{
}

/*
 * Destruktor.
 */
CompositeWidget::~CompositeWidget()
{
	int count = widgets.size();
	for(int i = 0; i < count; i++)
	{
		delete widgets[i];
	}
}


/*
 * Fuegt ein Widget hinzu.  Reihenfolge ist signifikant.
 */
void CompositeWidget::add_widget(Widget* w)
{
	w->set_surface(surface);

	// min/max aktualisieren
	const SDL_Rect* wmin = w->get_min_rect();
	const SDL_Rect* wmax = w->get_max_rect();
	if(orientation == CWOHorizontal)
	{
		min_rect.h = max(min_rect.h, wmin->h);
		max_rect.h = max(max_rect.h, wmax->h);
		min_rect.w += wmin->w;
		max_rect.w += wmax->w;
	}
	else // CWOVertical
	{
		min_rect.h += wmin->h;
		max_rect.h += wmax->h;
		min_rect.w = max(min_rect.w, wmin->w);
		max_rect.w = max(max_rect.w, wmax->w);
	}
	weight_total += w->get_weight();

	widgets.push_back(w);
	modified = true;
}


/*
 * Berechnet das Layout des CompositeWidget und aller enthaltenen Widgets
 * und aktualisiert min_rect / max_rect.
 */
void CompositeWidget::resize()
{
/*
cerr << "===> CompositeWidget::resize()" << endl;
cerr << "eff_rect.x=" << eff_rect.x << endl;
cerr << "eff_rect.y=" << eff_rect.y << endl;
cerr << "eff_rect.w=" << eff_rect.w << endl;
cerr << "eff_rect.h=" << eff_rect.h << endl;
cerr << "min_rect.x=" << min_rect.x << endl;
cerr << "min_rect.y=" << min_rect.y << endl;
cerr << "min_rect.w=" << min_rect.w << endl;
cerr << "min_rect.h=" << min_rect.h << endl;
cerr << "max_rect.x=" << max_rect.x << endl;
cerr << "max_rect.y=" << max_rect.y << endl;
cerr << "max_rect.w=" << max_rect.w << endl;
cerr << "max_rect.h=" << max_rect.h << endl;
*/
	int count = widgets.size();

	/*
	 * Layout manager code.
	 */

	int dyn = 0;
	if(orientation == CWOHorizontal) { dyn = eff_rect.w - min_rect.w; }
	else /* CWOVertical */           { dyn = eff_rect.h - min_rect.h; }
	dyn = max(0, dyn);
/*
cerr << "dyn=" << dyn << endl;
*/
	int last_x = eff_rect.x;
	int last_y = eff_rect.y;

	// loop all contained widgets
	for(int i = 0; i < count; i++)
	{
/*
cerr << "===> widget[" << i << "]" << endl;

cerr << "last_x=" << last_x << endl;
cerr << "last_y=" << last_y << endl;
*/
		SDL_Rect* r = widgets[i]->get_eff_rect();

		if(orientation == CWOHorizontal)
		{
			r->x = last_x;
			r->y = last_y;
			r->w = min(dyn * widgets[i]->get_weight() / weight_total
			           + widgets[i]->get_min_rect()->w,
			           widgets[i]->get_max_rect()->w);
			r->h = min(eff_rect.h, widgets[i]->get_max_rect()->h);
			last_x += r->w;
		}
		else // CWOVertical
		{
			r->x = last_x;
			r->y = last_y;
			r->h = min(dyn * widgets[i]->get_weight() / weight_total
			           + widgets[i]->get_min_rect()->h,
			           widgets[i]->get_max_rect()->h);
			r->w = min(eff_rect.w, widgets[i]->get_max_rect()->w);
			last_y += r->h;
		}
/*
cerr << "widgets[" << i << "]->eff_rect.x=" << r->x << endl;
cerr << "widgets[" << i << "]->eff_rect.y=" << r->y << endl;
cerr << "widgets[" << i << "]->eff_rect.w=" << r->w << endl;
cerr << "widgets[" << i << "]->eff_rect.h=" << r->h << endl;
*/
		widgets[i]->resize();
	}

	// clear modified flag
	modified = false;
}


/*
 * Zeichnet das CompositeWidget und alle enthaltenen Widgets.
 */
void CompositeWidget::draw()
{
	static UVDraw* drw = UVDraw::get_instance();

	if(modified)
		resize();

	// *** FIXME
	drw->box(surface, eff_rect.x, eff_rect.y,
		eff_rect.x + eff_rect.w, eff_rect.y + eff_rect.h,
		0x00, 0xFF, 0x00, 0x7F);

	int count = widgets.size();
	for(int i = 0; i < count; i++)
	{
		widgets[i]->draw();
	}
}


/*
 * Mausklick-Event wird aufgerufen wenn der Benutzer auf die Flaeche des
 * CompositeWidgets klickt.  Der Event muss verarbeitet und ans richtige
 * Widget weitergegeben werden.
 */
void CompositeWidget::handle_click(SDL_Rect* pos)
{
	int count = widgets.size();
	for(int i = 0; i < count; i++)
	{
		if(widgets[i]->contains(pos))
		{
			widgets[i]->handle_click(pos);
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
void CompositeWidget::set_surface(SDL_Surface* s)
{
	surface = s;

	int count = widgets.size();
	for(int i = 0; i < count; i++)
	{
		widgets[i]->set_surface(s);
	}
}


