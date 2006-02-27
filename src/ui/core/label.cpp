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

#include "label.h"

#include "si/fonthandler.h"
#include "si/font.h"
#include "si/conf.h"

using namespace std;

/*
 * Konstruktor.
 */
UVLabel::UVLabel(string txt, int we, SDL_Surface* s)
: UVWidget(we, s), text(NULL)
{
	font = UVFontHandler::get_instance()->get_font(FNT_SANS,
			UVConf::get_instance()->l_get("label-font-size"));
	set_text(txt);
}

/*
 * Destruktor.
 */
UVLabel::~UVLabel()
{
	if(text)
		SDL_FreeSurface(text);
}


/*
 * Zeichnet das UVWidget auf die SDL_Surface surface.
 */
void UVLabel::draw()
{
	SDL_Rect *rect = to_sdl_rect();
	rect->x += text->h / 4;
	SDL_BlitSurface(text, 0, surface, rect);
}


/*
 * Setzt den Text des Labels.
 */
void UVLabel::set_text(string s)
{
	if(text)
		SDL_FreeSurface(text);
	text = font->get_surface(s);
	min.w = text->w + text->h / 4;
	min.h = text->h;
}


