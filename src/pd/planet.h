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

#include "../lib/stl.h"

class UVZone; // FIXME
class UVPlanet;

// ===========================================================================

class UVAgrarfeld
{
	public:
		UVAgrarfeld(long n, UVZone* z, long gr = 0, string pr = "", long er = 0);
//		~UVAgrarfeld();

		long nummer;			// key in UVPlanet

		long zone;				// key: UVZone in UVPlanet

		long groesse;			// in FUs
		string produkt;
		long ertrag;			// in BRT
};

class UVSpeicherfeld
{
	public:
		UVSpeicherfeld(long n, UVZone* z, long gr = 0, long sp = 0, long me = 0, string wa = "");
//		~UVSpeicherfeld();

		long nummer;			// key in UVPlanet

		long zone;				// key: UVZone in UVPlanet

		long groesse;			// in FUs
		long speicherplatz;		// in BRT
		long menge;				// in BRT
		string ware;
};

class UVMinenfeld
{
	public:
		UVMinenfeld(long n, UVZone* z, long gr = 0, string ro = "", long er = 0);
//		~UVMinenfeld();

		long nummer;			// key in UVPlanet

		long zone;				// key: UVZone in UVPlanet

		long groesse;			// in FUs
		string rohstoff;
		long ertrag;			// in BRT
};

class UVWerft
{
	public:
		UVWerft(long, UVZone*);
//		~UVWerft();

		long nummer;			// key in UVPlanet

		long zone;				// key: UVZone in UVPlanet

		string name;
		long groesse;			// in FUs
		string formel;
		long erzlager;			// in BRT
		string beschreibung;
};

class UVForschungsstation
{
	public:
		UVForschungsstation(long, UVZone*);
//		~UVForschungsstation();

		long nummer;			// key in UVPlanet

		long zone;				// key: UVZone in UVPlanet

		string name;
		long groesse;			// in FUs
		long wissenspunkte;
		string beschreibung;
};

class UVStadt
{
	public:
		UVStadt(long, UVZone*);
//		~UVStadt();

		long nummer;			// key in UVPlanet

		long zone;				// key: UVZone in UVPlanet

		string name;
		long groesse;			// in FUs
		long einwohner;
		string beschreibung;
};

// ===========================================================================

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

class UVHandelsstation
{
	public:
		UVHandelsstation(string, UVPlanet*);
//		~UVHandelsstation();

		string name;			// key in UVWelt

		long planet;			// key: UVPlanet in UVWelt

		string beschreibung;
};

class UVPlanet
{
	public:
		UVPlanet(long, string, string, long, long, long);
//		~UVPlanet();

		long nummer;			// key in UVWelt

		string name;
		string besitzer;
		long x;
		long y;
		long dim;

		vector<long> nachbarn;	// key: UVPlanet in UVWelt
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

