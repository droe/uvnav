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

#include "schiff.h"

//#include <math.h>

/*
 * UVLadung - Stellt ein Posten geladener Gueter dar.
 */


/*
 * Konstruktor.
 */
UVLadung::UVLadung(long n, string s, long gr, string be)
: nummer(n), name(s), groesse(gr), besitzer(be)
{
}


/*
 * Destruktor.
 */
/*
UVLadung::~UVLadung()
{
}
*/


//============================================================================

/*
 * UVKomponente - Stellt eine Schiffs-Komponente dar.
 */


/*
 * Konstruktor.
 */
UVKomponente::UVKomponente(string s, long zs, long hp, long gr, long n, double l)
: nummer(n), name(s), zustand(zs), hitpoints(hp), groesse(gr), leistung(l)
{
}


/*
 * Destruktor.
 */
/*
UVKomponente::~UVKomponente()
{
}
*/


//============================================================================

/*
 * UVSchiff - Stellt ein Schiff dar.
 */


/*
 * Konstruktor.
 */
UVSchiff::UVSchiff(string na, string be, long gr)
: name(na), besitzer(be), groesse(gr)
, x(0), y(0), dim(0), planet(0), zonenstatus(false), werft(0)
, beschreibung(""), v(0.0), w(0)
, waffenstatus(0), offensivbereich(0), fluchtwert(0), hitpoints(0)
, traktorstrahl(false), treibstoff(0.0), treibstofftanks(0)
, lagerraum(0), lagerraum_frei(0), sichtweite(0)
, bordcomputer(NULL), energiegenerator(NULL), sensoren(NULL)
, mannschaftsraum(NULL), schild(NULL), panzerung(NULL)
{
}


/*
 * Destruktor.
 */
/*
UVSchiff::~UVSchiff()
{
}
*/


/*
 * Enegiekanonenobjekt setzen/holen.
 */
void UVSchiff::set_energiekanone(UVKomponente* c)
{
	while(static_cast<long>(energiekanonen.size()) < c->nummer)
	{
		energiekanonen.push_back(NULL);
	}
	if(energiekanonen.at(c->nummer - 1) != NULL)
	{
		delete energiekanonen.at(c->nummer -1);
	}
	energiekanonen.at(c->nummer - 1) = c;
}
UVKomponente* UVSchiff::get_energiekanone(const long i) const
{
	if(i <= static_cast<long>(energiekanonen.size()))
	{
		return energiekanonen.at(i - 1);
	}
	return NULL;
}
komponenten_iterator UVSchiff::first_energiekanone()
{
	return energiekanonen.begin();
}
komponenten_iterator UVSchiff::last_energiekanone()
{
	return energiekanonen.end();
}


/*
 * Raketenobjekt setzen/holen.
 */
void UVSchiff::set_rakete(UVKomponente* c)
{
	while(static_cast<long>(raketen.size()) < c->nummer)
	{
		raketen.push_back(NULL);
	}
	if(raketen.at(c->nummer - 1) != NULL)
	{
		delete raketen.at(c->nummer - 1);
	}
	raketen.at(c->nummer - 1) = c;
}
UVKomponente* UVSchiff::get_rakete(const long i) const
{
	if(i <= static_cast<long>(raketen.size()))
	{
		return raketen.at(i - 1);
	}
	return NULL;
}
komponenten_iterator UVSchiff::first_rakete()
{
	return raketen.begin();
}
komponenten_iterator UVSchiff::last_rakete()
{
	return raketen.end();
}


/*
 * Triebwerkobjekt setzen/holen.
 */
void UVSchiff::set_triebwerk(UVKomponente* c)
{
	while(static_cast<long>(triebwerke.size()) < c->nummer)
	{
		triebwerke.push_back(NULL);
	}
	if(triebwerke.at(c->nummer - 1) != NULL)
	{
		delete triebwerke.at(c->nummer - 1);
	}
	triebwerke.at(c->nummer - 1) = c;
}
UVKomponente* UVSchiff::get_triebwerk(const long i) const
{
	if(i <= static_cast<long>(triebwerke.size()))
	{
		return triebwerke.at(i - 1);
	}
	return NULL;
}
komponenten_iterator UVSchiff::first_triebwerk()
{
	return triebwerke.begin();
}
komponenten_iterator UVSchiff::last_triebwerk()
{
	return triebwerke.end();
}


/*
 * Ladungsobjekt setzen/holen.
 */
void UVSchiff::set_ladung(UVLadung* c)
{
	while(static_cast<long>(ladung.size()) < c->nummer)
	{
		ladung.push_back(NULL);
	}
	if(ladung.at(c->nummer - 1) != NULL)
	{
		delete ladung.at(c->nummer - 1);
	}
	ladung.at(c->nummer - 1) = c;
}
UVLadung* UVSchiff::get_ladung(const long i) const
{
	if(i <= static_cast<long>(ladung.size()))
	{
		return ladung.at(i - 1);
	}
	return NULL;
}
ladung_iterator UVSchiff::first_ladung()
{
	return ladung.begin();
}
ladung_iterator UVSchiff::last_ladung()
{
	return ladung.end();
}


