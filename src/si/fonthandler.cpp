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

#include "fonthandler.h"

#include "../lib/exceptions.h"

/*
 * UVFontHandler - TTF-Schriftarten laden und UVFont-Objekte verwalten.
 */

typedef ext::hash_map<string, UVFont*>::iterator font_iterator;

/*
 * Konstruktor.
 */
UVFontHandler::UVFontHandler()
{
	if(TTF_Init() < 0)
		throw EXCEPTION("TTF_Init: " + to_string(TTF_GetError()));
	else
		atexit(TTF_Quit);
}


/*
 * Destruktor.
 */
UVFontHandler::~UVFontHandler()
{
	for(font_iterator iter = fonts.begin(); iter != fonts.end(); iter++)
	{
		delete (*iter).second;
	}
}


/*
 * Gibt Pointer auf UVFont-Objekt zurueck, das mit den angeforderten
 * Parametern initialisiert wurde.
 */
UVFont* UVFontHandler::get_font(const string& file, int size)
{
	string key = file + "@" + to_string(size);

	if(fonts.count(key) == 0)
	{
		fonts[key] = new UVFont(file, size);
	}

	return fonts[key];
}

