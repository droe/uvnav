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

#include "universum.h"

/*
 * UVUniversum - Repraesentation eines Universum Spielstandes.
 */


/*
 * Konstruktor.
 */
UVUniversum::UVUniversum()
: partie(""), copyright(""), galaxie(""), motu(""), sternzeit(0)
{
	spieler = new UVSpieler();
}


/*
 * Destruktor.
 */
UVUniversum::~UVUniversum()
{
	delete spieler;
}


/*
 * Name einer Dimension setzen/holen.
 */
void UVUniversum::set_dim(const long d, const string& s)
{
	dim[d] = s;
}
string UVUniversum::get_dim(long d) const
{
	return dim[d];
}
dim_iterator UVUniversum::first_dim()
{
	return dim.begin();
}
dim_iterator UVUniversum::last_dim()
{
	return dim.end();
}


/*
 * Spielerobjekt setzen/holen.
 */
UVSpieler* UVUniversum::get_spieler() const
{
	return spieler;
}
void UVUniversum::set_spieler(UVSpieler* s)
{
	spieler = s;
}


/*
 * Schiffsobjekt setzen/holen.
 *
 * Planet p ist Kontext. Falls p == NULL handelt es sich um ein
 * frei fliegendes Schiff, falls p != NULL wird das Schiff beim
 * betreffenden Planeten registriert.
 */
void UVUniversum::set_schiff(UVSchiff* s, UVPlanet* p)
{
	schiffe[s->name] = s;
	if(p != NULL)
	{
		s->x = p->x;
		s->y = p->y;
		s->dim = p->dim;
		s->planet = p->nummer;

		// bei Planet registrieren
		p->schiffe.push_back(s->name);
	}
}
UVSchiff* UVUniversum::get_schiff(const string& n) const
{
	return schiffe[n];
}
schiffe_iterator UVUniversum::first_schiff()
{
	return schiffe.begin();
}
schiffe_iterator UVUniversum::last_schiff()
{
	return schiffe.end();
}


/*
 * Planetenobjekt setzen/holen.
 */
void UVUniversum::set_planet(UVPlanet* p)
{
	planeten[p->nummer] = p;
}
UVPlanet* UVUniversum::get_planet(const long n) const
{
	if(planeten.count(n) > 0)
	{
		return planeten[n];
	}
	else
	{
		return NULL;
	}
}
planeten_iterator UVUniversum::first_planet()
{
	return planeten.begin();
}
planeten_iterator UVUniversum::last_planet()
{
	return planeten.end();
}


/*
 * Handelsstationsobjekt setzen/holen.
 */
void UVUniversum::set_handelsstation(UVHandelsstation* h, UVPlanet* p)
{
	handelsstationen[h->name] = h;
	// bei Planet registrieren
	p->handelsstation = h->name;
}
UVHandelsstation* UVUniversum::get_handelsstation(const string& h) const
{
	return handelsstationen[h];
}
handelsstationen_iterator UVUniversum::first_handelsstation()
{
	return handelsstationen.begin();
}
handelsstationen_iterator UVUniversum::last_handelsstation()
{
	return handelsstationen.end();
}


/*
 * Anomalieobjekt setzen/holen.
 */
void UVUniversum::set_anomalie(UVAnomalie* a)
{
	anomalien.push_back(a);
}
UVAnomalie* UVUniversum::get_anomalie(const long i) const
{
	if(i <= static_cast<long>(anomalien.size()))
	{
		return anomalien.at(i - 1);
	}
	return NULL;
}
anomalien_iterator UVUniversum::first_anomalie()
{
	return anomalien.begin();
}
anomalien_iterator UVUniversum::last_anomalie()
{
	return anomalien.end();
}


/*
 * Containerobjekt setzen/holen.
 */
void UVUniversum::set_container(UVContainer* c)
{
	container.push_back(c);
}
UVContainer* UVUniversum::get_container(const long i) const
{
	if(i <= static_cast<long>(container.size()))
	{
		return container.at(i - 1);
	}
	return NULL;
}
container_iterator UVUniversum::first_container()
{
	return container.begin();
}
container_iterator UVUniversum::last_container()
{
	return container.end();
}


/*
 * Sensorsondenobjekt setzen/holen.
 */
void UVUniversum::set_sensorsonde(UVSensorsonde* s)
{
	sensorsonden[s->nummer] = s;
}
UVSensorsonde* UVUniversum::get_sensorsonde(const long n) const
{
	return sensorsonden[n];
}
sensorsonden_iterator UVUniversum::first_sensorsonde()
{
	return sensorsonden.begin();
}
sensorsonden_iterator UVUniversum::last_sensorsonde()
{
	return sensorsonden.end();
}


/*
 * Infosondenobjekt setzen/holen.
 */
void UVUniversum::set_infosonde(UVInfosonde* i)
{
	infosonden[i->nummer] = i;
}
UVInfosonde* UVUniversum::get_infosonde(const long n) const
{
	return infosonden[n];
}
infosonden_iterator UVUniversum::first_infosonde()
{
	return infosonden.begin();
}
infosonden_iterator UVUniversum::last_infosonde()
{
	return infosonden.end();
}


