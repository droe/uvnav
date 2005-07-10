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

#ifndef PLANET_H
#define PLANET_H

#include "util/stl.h"
#include "pd/zone.h"
#include "pd/handelsstation.h"

class UVPlanet
{
	public:
		UVPlanet(long, string, string, long, long, long);
//		~UVPlanet();

		long nummer;			// key in UVUniversum

		string name;
		string besitzer;
		long x;
		long y;
		long dim;

		vector<long> nachbarn;	// key: UVPlanet in UVUniversum
		bool drawflag;			// fuer UVMap::draw_planet

		string beschreibung;
		double bevoelkerung;	// in Millionen
		string zustand;
		long minen;
		long minen_max;
		long fabriken;
		long fabriken_max;
		long produktion;		// in %
		string klima;
		long image;				// #defined in si/imagehandler.h
		long diameter;			// in km
		long techlevel;			// 0..10
		long energiegenerator;	// 0..100
		long tribut;			// in t
		long xbatts;
		long ybatts;
		long zbatts;

		string handelsstation;	// key: UVHandelsstation in UVWelt
		vector<string> schiffe;	// key: UVSchiff in UVWelt

		void set_zone(UVZone*);
		UVZone* get_zone(const long) const;
		long max_zone() const;

		void set_agrarfeld(UVAgrarfeld*, UVZone*);
		UVAgrarfeld* get_agrarfeld(const long) const;
		long max_agrarfeld() const;

		void set_speicherfeld(UVSpeicherfeld*, UVZone*);
		UVSpeicherfeld* get_speicherfeld(const long) const;
		long max_speicherfeld() const;

		void set_minenfeld(UVMinenfeld*, UVZone*);
		UVMinenfeld* get_minenfeld(const long) const;
		long max_minenfeld() const;

		void set_werft(UVWerft*, UVZone*);
		UVWerft* get_werft(const long) const;
		long max_werft() const;

		void set_forschungsstation(UVForschungsstation*, UVZone*);
		UVForschungsstation* get_forschungsstation(const long) const;
		long max_forschungsstation() const;

		void set_stadt(UVStadt*, UVZone*);
		UVStadt* get_stadt(const long) const;
		long max_stadt() const;

		string to_string_terse() const;

	private:
		vector<UVZone*> zonen;
		vector<UVAgrarfeld*> agrarfelder;
		vector<UVSpeicherfeld*> speicherfelder;
		vector<UVMinenfeld*> minenfelder;
		vector<UVWerft*> werften;
		vector<UVForschungsstation*> forschungsstationen;
		vector<UVStadt*> staedte;
};

#endif // PLANET_H

