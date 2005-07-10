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

#include "planet.h"

#include "pd/zone.h"
#include "pd/handelsstation.h"
#include "pd/agrarfeld.h"
#include "pd/speicherfeld.h"
#include "pd/minenfeld.h"
#include "pd/werft.h"
#include "pd/forschungsstation.h"
#include "pd/stadt.h"

/*
 * UVPlanet - Stellt einen Planeten dar.
 */


/*
 * Konstruktor.
 */
UVPlanet::UVPlanet(long n, string na, string be, long x_, long y_, long d)
: nummer(n), name(na), besitzer(be), x(x_), y(y_), dim(d), drawflag(false)
, beschreibung(""), bevoelkerung(0.0), zustand(""), minen(0), minen_max(0)
, produktion(0), klima(""), image(0), diameter(0), techlevel(0)
, energiegenerator(0), tribut(0), xbatts(0), ybatts(0), zbatts(0)
, handelsstation("")
{
}


/*
 * Destruktor.
 */
/*
UVPlanet::~UVPlanet()
{
}
*/


/*
 * Zonenobjekt setzen/holen.
 */
void UVPlanet::set_zone(UVZone* z)
{
	while(static_cast<long>(zonen.size()) < z->nummer)
	{
		zonen.push_back(NULL);
	}
	if(zonen.at(z->nummer - 1) != NULL && zonen.at(z->nummer - 1) != z) // TODO: port fix to other structures
	{
		delete zonen.at(z->nummer - 1);
	}
	zonen.at(z->nummer - 1) = z;
}
UVZone* UVPlanet::get_zone(const long i) const
{
	if(i <= static_cast<long>(zonen.size()))
	{
		return zonen.at(i - 1);
	}
	return NULL;
}
long UVPlanet::max_zone() const
{
	return static_cast<long>(zonen.size());
}


/*
 * Agrarfeldobjekt setzen/holen
 */
void UVPlanet::set_agrarfeld(UVAgrarfeld* a, UVZone* z)
{
	while(static_cast<long>(agrarfelder.size()) < a->nummer)
	{
		agrarfelder.push_back(NULL);
	}
	if(agrarfelder.at(a->nummer - 1) != NULL)
	{
		delete agrarfelder.at(a->nummer - 1);
	}
	agrarfelder.at(a->nummer - 1) = a;

	// bei Zone registrieren
	z->agrarfelder.push_back(a->nummer);
}
UVAgrarfeld* UVPlanet::get_agrarfeld(const long i) const
{
	if(i <= static_cast<long>(agrarfelder.size()))
	{
		return agrarfelder.at(i - 1);
	}
	return NULL;
}
long UVPlanet::max_agrarfeld() const
{
	return static_cast<long>(agrarfelder.size());
}


/*
 * Speicherfeldobjekt setzen/holen.
 */
void UVPlanet::set_speicherfeld(UVSpeicherfeld* s, UVZone* z)
{
	while(static_cast<long>(speicherfelder.size()) < s->nummer)
	{
		speicherfelder.push_back(NULL);
	}
	if(speicherfelder.at(s->nummer - 1) != NULL)
	{
		delete speicherfelder.at(s->nummer - 1);
	}
	speicherfelder.at(s->nummer - 1) = s;

	// bei Zone registrieren
	z->speicherfelder.push_back(s->nummer);
}
UVSpeicherfeld* UVPlanet::get_speicherfeld(const long i) const
{
	if(i <= static_cast<long>(speicherfelder.size()))
	{
		return speicherfelder.at(i - 1);
	}
	return NULL;
}
long UVPlanet::max_speicherfeld() const
{
	return static_cast<long>(speicherfelder.size());
}


/*
 * Minenfeldobjet setzen/holen.
 */
void UVPlanet::set_minenfeld(UVMinenfeld* m, UVZone* z)
{
	while(static_cast<long>(minenfelder.size()) < m->nummer)
	{
		minenfelder.push_back(NULL);
	}
	if(minenfelder.at(m->nummer - 1) != NULL)
	{
		delete minenfelder.at(m->nummer - 1);
	}
	minenfelder.at(m->nummer - 1) = m;

	// bei Zone registrieren
	z->minenfelder.push_back(m->nummer);
}
UVMinenfeld* UVPlanet::get_minenfeld(const long i) const
{
	if(i <= static_cast<long>(minenfelder.size()))
	{
		return minenfelder.at(i - 1);
	}
	return NULL;
}
long UVPlanet::max_minenfeld() const
{
	return static_cast<long>(minenfelder.size());
}


/*
 * Werftobjekt setzen/holen.
 */
void UVPlanet::set_werft(UVWerft* w, UVZone* z)
{
	while(static_cast<long>(werften.size()) < w->nummer)
	{
		werften.push_back(NULL);
	}
	if(werften.at(w->nummer - 1) != NULL)
	{
		delete werften.at(w->nummer - 1);
	}
	werften.at(w->nummer - 1) = w;

	// bei Zone registrieren
	z->werften.push_back(w->nummer);
}
UVWerft* UVPlanet::get_werft(const long i) const
{
	if(i <= static_cast<long>(werften.size()))
	{
		return werften.at(i - 1);
	}
	return NULL;
}
long UVPlanet::max_werft() const
{
	return static_cast<long>(werften.size());
}


/*
 * Forschungsstationsobjekt setzen/holen.
 */
void UVPlanet::set_forschungsstation(UVForschungsstation* f, UVZone* z)
{
	while(static_cast<long>(forschungsstationen.size()) < f->nummer)
	{
		forschungsstationen.push_back(NULL);
	}
	if(forschungsstationen.at(f->nummer - 1) != NULL)
	{
		delete forschungsstationen.at(f->nummer - 1);
	}
	forschungsstationen.at(f->nummer - 1) = f;

	// bei Zone registrieren
	z->forschungsstationen.push_back(f->nummer);
}
UVForschungsstation* UVPlanet::get_forschungsstation(const long i) const
{
	if(i <= static_cast<long>(forschungsstationen.size()))
	{
		return forschungsstationen.at(i - 1);
	}
	return NULL;
}
long UVPlanet::max_forschungsstation() const
{
	return static_cast<long>(forschungsstationen.size());
}


/*
 * Stadtobjekt setzen/holen.
 */
void UVPlanet::set_stadt(UVStadt* s, UVZone* z)
{
	while(static_cast<long>(staedte.size()) < s->nummer)
	{
		staedte.push_back(NULL);
	}
	if(staedte.at(s->nummer - 1) != NULL)
	{
		delete staedte.at(s->nummer - 1);
	}
	staedte.at(s->nummer - 1) = s;

	// bei Zone registrieren
	z->staedte.push_back(s->nummer);
}
UVStadt* UVPlanet::get_stadt(const long i) const
{
	if(i <= static_cast<long>(staedte.size()))
	{
		return staedte.at(i - 1);
	}
	return NULL;
}
long UVPlanet::max_stadt() const
{
	return static_cast<long>(staedte.size());
}


/*
 * Planet als Einzeiler ausgeben.
 */
string UVPlanet::to_string_terse() const
{
	return "Planet " + name + " ("
	               + to_string(nummer) + ") ("
	               + besitzer + ") ("
	               + to_string(x) + ","
	               + to_string(y) + ","
	               + to_string(dim) + ")";
}


