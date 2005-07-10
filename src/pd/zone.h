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

#include "util/stl.h"

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

		string name;
		string besitzer;
		long groesse;			// in FUs
		string beschreibung;

		double T;					// Durchschnittstemperatur in °C
		double N;					// Jahresniederschlag in cm
		double temperatur[12];		// Monatstemperatur in °C
		double niederschlag[12];	// Monstsniederschlag in mm

		vector<long> agrarfelder;		// key: UVAgrarfeld in UVPlanet
		vector<long> speicherfelder;	// key: UVSpeicherfeld in UVPlanet
		vector<long> minenfelder;		// key: UVMine in UVPlanet
		vector<long> werften;			// key: UVWerft in UVPlanet
		vector<long> forschungsstationen;	// key: UVForschungsstationen in UVPlanet
		vector<long> staedte;			// key: UVStadt in UVPlanet

		string get_klimazone() const;
		double get_T() const;
		double get_N() const;

		string to_string_terse() const;
};

#endif // ZONE_H

