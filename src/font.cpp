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

#include "font.h"

/*
 * UVFont - Eine TTF-Schriftart laden und Strings rendern.
 *
 * Die eigentlichen Surfaces werden beim Benutzer gecached, weil
 * es generell viel mehr Sinn macht, den Namensstring eines Planeten
 * direkt im UVPlanet selber zu cachen.
 *
 * Eventuell sollte mal ein UVFontCache implementiert werden, welcher
 * Surfaces mit UVFont rendert und die Surfaces in einer
 * hash_map<string, UVSurface*> ablegt.
 */


/*
 * Konstruktor.
 */
UVFont::UVFont(const UVConf* conf, const string& file, int size)
{
	string ff = sysdep_fontfile(file);
	if(!sysdep_file_exists(ff))
	{
		throw EXCEPTION("Kann Font-Datei " + file + " nicht finden!");
	}

	antialiasing = conf->b_get("screen-quality");

	TTF_Init();

	font = TTF_OpenFont(ff.c_str(), size);
	if(font == NULL)
	{
		throw EXCEPTION(string("TTF: ") + TTF_GetError());
	}
}


/*
 * Destruktor.
 */
UVFont::~UVFont()
{
	TTF_CloseFont(font);
	TTF_Quit();
}


/*
 * Surface holen.
 */
SDL_Surface* UVFont::get_surface(const string& text, const SDL_Color& color) const
{
	if(antialiasing)
	{
		return TTF_RenderText_Blended(font, text.c_str(), color);
// *** faster to blit, but blocky and non-alpha:
//		surface = TTF_RenderText_Shaded(font, text.c_str(), color);
	}
	else
	{
		return TTF_RenderText_Solid(font, text.c_str(), color);
	}
}
SDL_Surface* UVFont::get_surface(const string& text, int r, int g, int b, int a) const
{
	SDL_Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.unused = a;
	return get_surface(text, color);
}
SDL_Surface* UVFont::get_surface(const string& text) const
{
	static const SDL_Color color = {255, 255, 255, 0};
	return get_surface(text, color);
}


