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

#include "imagehandler.h"

#include "lib/exceptions.h"
#include "lib/sysdep.h"
#include "si/conf.h"

/*
 * UVImageHandler - Bilder effizient laden, skalieren und verwalten.
 *
 * Laedt alle Bilder als Surface in Originalgroesse.
 * Stellt die Bilder in beliebiger Groesse zu Verfuegung.
 * Betreibt einen Cache der skalierten Bilder; vorerst mal
 * nur der zuletzt gewuenschten Groesse, spaeter eventuell
 * intelligenteres Caching, falls sinnvoll.
 *
 * TODO: Sollte mal auf SDL_scale gewechselt werden, am besten
 * konfigurierbar. Dann Performance vergleichen.
 */


/*
 * Statischer Array der Image-Files.
 */
const string img_files[NUM_IMG] = {
	"universum.png",
	"sdl_powered.png",
	"planet01.png",
	"planet02.png",
	"planet03.png",
	"planet04.png",
	"planet05.png",
	"planet06.png",
	"planet07.png",
	"planet08.png",
	"planet09.png",
	"planet10.png",
};


/*
 * Konstruktor.
 *
 * Laedt die Bilder als Surface in Originalgroesse.
 * Erledigt auch die Konversion ins passende Bildschirmformat.
 */
UVImageHandler::UVImageHandler()
{
	smoothing = UVConf::get_instance()->b_get("screen-quality")
		? SMOOTHING_ON
		: SMOOTHING_OFF;

	for(int i = 0; i < NUM_IMG; i++)
	{
		SDL_Surface* surface;
		string ff = sysdep_imagefile(img_files[i]);

		// Existiert die Datei?
		if(!sysdep_file_exists(ff))
		{
			throw EXCEPTION("Image-Datei nicht gefunden: " + ff);
		}

		// Bild laden
		surface = IMG_Load(ff.c_str());
		if(!surface)
		{
			throw EXCEPTION(string("SDL_image Error: ") + IMG_GetError());
		}

		// Surface in Bildschirmformat konvertieren
		images[i].original = SDL_DisplayFormatAlpha(surface);
		SDL_FreeSurface(surface);
		if(!images[i].original)
		{
			throw EXCEPTION(string("SDL_DisplayFormatAlpha Error: ") + SDL_GetError());
		}

		images[i].resultat = NULL;
	}
}


/*
 * Destruktor.
 *
 * Entfernt alle Surfaces im Cache.
 */
UVImageHandler::~UVImageHandler()
{
	for(int i = 0; i < NUM_IMG; i++)
	{
		if(images[i].original)
		{
			delete images[i].original;
			images[i].original = NULL;
		}
		if(images[i].resultat)
		{
			delete images[i].resultat;
			images[i].resultat = NULL;
		}
	}
}


/*
 * Surface holen.
 *
 * Gibt eine richtig skalierte Surface zurueck.
 * Entweder nach Zoomfaktor skaliert, oder direkt auf gewuenschte Groesse.
 * Wenn in der zweiten Variante w UND h gegeben sind, wird w verwendet.
 */
SDL_Surface* UVImageHandler::get_surface(const long id, const double f)
{
	if(id >= NUM_IMG)
	{
		throw EXCEPTION("Fehler: Ungueltige Image-ID");
	}

	if(f == 1.0)
	{
		return images[id].original;
	}
	else
	{
		if(f == images[id].faktor)
		{
			// cache hit
			return images[id].resultat;
		}
		else
		{
			// *** DEBUG CODE
//			cerr << "Cache miss: zoomSurface(" << img_files[id] << ": " << images[id].faktor << " -> " << f << ")" << endl;
			// cache miss
			if(images[id].resultat)
			{
				SDL_FreeSurface(images[id].resultat);
			}
			images[id].resultat = zoomSurface(images[id].original, f, f, smoothing);
			if(!images[id].resultat)
			{
				throw EXCEPTION("Fehler: zoomSurface() schlug fehl");
			}
			images[id].faktor = f;
			return images[id].resultat;
		}
	}
}
SDL_Surface* UVImageHandler::get_surface(const long id, const int w, const int h)
{
	double f;

	if(w)
	{
		f = 1.0 * w / images[id].original->w;
	}
	else
	{
		f = 1.0 * h / images[id].original->h;
	}

	return get_surface(id, f);
}

