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

#ifndef ZONE_H
#define ZONE_H

#include "util/stl_hash_map.h"
#include "util/stl_string.h"

class UVAgrarfeld;
class UVSpeicherfeld;
class UVMinenfeld;
class UVWerft;
class UVForschungsstation;
class UVStadt;

class UVZone
{
	public:
		UVZone(long);
//		~UVZone();

		long nummer;			// key in UVPlanet

		std::string name;
		std::string besitzer;
		long groesse;			// in FUs
		std::string beschreibung;

		double T;					// Durchschnittstemperatur in °C
		double N;					// Jahresniederschlag in cm
		double temperatur[12];		// Monatstemperatur in °C
		double niederschlag[12];	// Monstsniederschlag in mm

		std::vector<long> agrarfelder;		// key: UVAgrarfeld in UVPlanet
		std::vector<long> speicherfelder;	// key: UVSpeicherfeld in UVPlanet
		std::vector<long> minenfelder;		// key: UVMine in UVPlanet
		std::vector<long> werften;			// key: UVWerft in UVPlanet
		std::vector<long> forschungsstationen;	// key: UVForschungsstationen in UVPlanet
		std::vector<long> staedte;			// key: UVStadt in UVPlanet

		std::string get_klimazone() const;
		double get_T() const;
		double get_N() const;

		std::string to_string_terse() const;
};

#endif // ZONE_H

