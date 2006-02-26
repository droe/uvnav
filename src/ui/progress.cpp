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

#include "progress.h"

#include "dm/abstractimporter.h"
#include "si/fonthandler.h"
#include "si/font.h"
#include "util/exceptions.h"

/*
 * UVProgress - Implementiert einen lightweight Progress-Bar.
 *
 * Zeichnet einen Progress Bar mit Prozentanzeige in ein SDL_Rect
 * auf einer SDL_Surface.
 */


/*
 * Konstruktor.
 */
UVProgress::UVProgress(SDL_Surface* s, SDL_Rect* r)
: screen(s), rect(*r), ticks(0)
{
	font = UVFontHandler::get_instance()->get_font(FNT_SANS, screen->h / 32);
}


/*
 * Destruktor.
 */
UVProgress::~UVProgress()
{
}


/*
 * Progress-Bar initialisieren.
 */
void UVProgress::init(unsigned long newtot)
{
	total = newtot;
	ticks = SDL_GetTicks();
//*** DEBUG
//	cerr << "progress init total=" << total << endl;
}


/*
 * Fortschritt quantifizieren und neu zeichnen.
 *
 * Zeichnet Fortschrittsbalen neu, updated die Surface, und
 * klappert SDL Events ab, damit SDL benötigte Refreshs machen
 * kann, und wir das Laden der Auswertung beenden können.
 *
 * +---------------------------------------------------------+
 * ¦ (rect.x/rect.y                                          ¦
 * ¦ +----------------------------------------+ - - - - - -+ ¦
 * ¦ ¦ (rect.x+2/rect.y+2)                    ¦              ¦
 * ¦ ¦                                        ¦            ¦ ¦
 * ¦ ¦                     +--------+         ¦              ¦
 * ¦ ¦                     ¦        ¦         ¦  ---->     ¦ ¦
 * ¦ ¦                     ¦        ¦         ¦              ¦
 * ¦ ¦                     +--------+         ¦            ¦ ¦
 * ¦ ¦                                        ¦              ¦
 * ¦ ¦                                        ¦            ¦ ¦
 * ¦ +----------------------------------------+ - - - - - -+ ¦
 * ¦                            rect.x+rect.w/rect.y+rect.h) ¦
 * +---------------------------------------------------------+
 */
void UVProgress::update(Subject* s)
{
	UVAbstractImporter* importer = static_cast<UVAbstractImporter*>(s);

//cerr << "s=" << s << " p=" << p << endl;

	unsigned long current = importer->get_bytecount();
	if(current == 0)
		init(importer->get_filesize());

	// Events abklopfen - aber nur bei Programmabbruch handeln.
	// Alle andern Events werden einfach ignoriert.
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				if(event.key.keysym.sym != SDLK_ESCAPE)
				{
					break;
				}
			case SDL_QUIT:
				throw EXCEPTION("Abgebrochen.");
		}
	}

	double progress = double(current) / double(total);

//*** DEBUG
//	cerr << "progress update current=" << current << " total=" << total << " progress=" << progress << endl;

	LOCK(screen);

	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));

	SDL_Rect inner;
	inner.x = rect.x + 2;	inner.w = long(rint(progress * (rect.w - 4)));
	inner.y = rect.y + 2;	inner.h = rect.h - 4;
	SDL_FillRect(screen, &inner, SDL_MapRGB(screen->format, 0, 0, 0xFF));

	SDL_Surface* percent = font->get_surface(to_string(long(progress * 100)) + "% (" + to_string(long(double(current) / (double(SDL_GetTicks() - ticks + 1) / 1000.0) / 1024.0)) + " kb/s)");
	inner.x = rect.x + 2 + rect.w / 2 - percent->w / 2;	inner.w = percent->w;
	inner.y = rect.y + 2 + rect.h / 2 - percent->h / 2;	inner.h = percent->h;
	SDL_BlitSurface(percent, 0, screen, &inner);

	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);

	UNLOCK(screen);
}


