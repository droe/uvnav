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

#ifndef UNIVERSUM_H
#define UNIVERSUM_H

#include "../lib/stl.h"
#include "spieler.h"
#include "anomalie.h"
#include "container.h"
#include "infosonde.h"
#include "sensorsonde.h"
#include "planet.h"
#include "schiff.h"

typedef ext::hash_map<long, string>::iterator dim_iterator;
typedef ext::hash_map<string, UVSchiff*>::iterator schiffe_iterator;
typedef ext::hash_map<long, UVPlanet*>::iterator planeten_iterator;
typedef ext::hash_map<string, UVHandelsstation*>::iterator handelsstationen_iterator;
typedef ext::vector<UVAnomalie*>::iterator anomalien_iterator;
typedef ext::vector<UVContainer*>::iterator container_iterator;
typedef ext::hash_map<long, UVSensorsonde*>::iterator sensorsonden_iterator;
typedef ext::hash_map<long, UVInfosonde*>::iterator infosonden_iterator;

class UVUniversum
{
	public:
		UVUniversum();
		~UVUniversum();

		string partie;
		string copyright;
		string galaxie;
		string motu;
		long sternzeit;

		void   set_dim(const long, const string&);
		string get_dim(const long) const;
		dim_iterator first_dim();
		dim_iterator last_dim();

		void set_spieler(UVSpieler*);
		UVSpieler* get_spieler() const;

		void set_schiff(UVSchiff* s, UVPlanet* p = NULL);
		UVSchiff* get_schiff(const string&) const;
		schiffe_iterator first_schiff();
		schiffe_iterator last_schiff();

		void set_planet(UVPlanet*);
		UVPlanet* get_planet(const long) const;
		planeten_iterator first_planet();
		planeten_iterator last_planet();

		void set_handelsstation(UVHandelsstation*, UVPlanet*);
		UVHandelsstation* get_handelsstation(const string&) const;
		handelsstationen_iterator first_handelsstation();
		handelsstationen_iterator last_handelsstation();

		void set_anomalie(UVAnomalie*);
		UVAnomalie* get_anomalie(const long) const;
		anomalien_iterator first_anomalie();
		anomalien_iterator last_anomalie();

		void set_container(UVContainer*);
		UVContainer* get_container(const long) const;
		container_iterator first_container();
		container_iterator last_container();

		void set_sensorsonde(UVSensorsonde*);
		UVSensorsonde* get_sensorsonde(const long) const;
		sensorsonden_iterator first_sensorsonde();
		sensorsonden_iterator last_sensorsonde();

		void set_infosonde(UVInfosonde*);
		UVInfosonde* get_infosonde(const long) const;
		infosonden_iterator first_infosonde();
		infosonden_iterator last_infosonde();

	private:
		mutable ext::hash_map<long, string> dim;
		UVSpieler* spieler;
		mutable ext::hash_map<string, UVSchiff*> schiffe;
		mutable ext::hash_map<long, UVPlanet*> planeten;
		mutable ext::hash_map<string, UVHandelsstation*> handelsstationen;
		vector<UVAnomalie*> anomalien;
		vector<UVContainer*> container;
		mutable ext::hash_map<long, UVSensorsonde*> sensorsonden;
		mutable ext::hash_map<long, UVInfosonde*> infosonden;
};

#endif // UNIVERSUM_H

