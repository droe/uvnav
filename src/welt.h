/*
 * UV Navigator - Auswertungsvisualisierung fuer Universum V
 * Copyright (C) 2004 Daniel Roethlisberger <roe@chronator.ch>
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

#ifndef WELT_H
#define WELT_H

#include "global.h"

class UVZone;
class UVPlanet;

// ===========================================================================

class UVSpieler
{
	public:
		UVSpieler();
//		~UVSpieler();

		string name;			// key in UVWelt

		string spieler;
		string status;
		string gesellschaft;
		string talent;
		long long legal;
		long long punkte;
		long long konto;		// in Credits
		long erzertrag;			// in t Erz
		long spionageabwehr;	// in % Erzertrag
		// *** Information UV-DB
};

// ===========================================================================

class UVAnomalie
{
	public:
		UVAnomalie();
//		~UVAnomalie();

		long radius;
		long x;
		long y;
		long dim;
};

class UVContainer
{
	public:
		UVContainer();
//		~UVContainer();

		long groesse;			// in BRT
		long x;
		long y;
		long dim;
};

class UVInfosonde
{
	public:
		UVInfosonde(long);
//		~UVInfosonde();

		long nummer;			// key in UVWelt

		long lebensdauer;		// in Runden
		long x;
		long y;
		long dim;
		// *** Nachricht
};

class UVSensorsonde
{
	public:
		UVSensorsonde(long);
//		~UVSensorsonde();

		long nummer;			// key in UVWelt

		long lebensdauer;		// in Runden
		long x;
		long y;
		long dim;
};

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
		long image;				// #defined in images.h
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

	private:
		vector<UVZone*> zonen;
		vector<UVAgrarfeld*> agrarfelder;
		vector<UVSpeicherfeld*> speicherfelder;
		vector<UVMinenfeld*> minenfelder;
		vector<UVWerft*> werften;
		vector<UVForschungsstation*> forschungsstationen;
		vector<UVStadt*> staedte;
};

// ===========================================================================

class UVLadung
{
	public:
		UVLadung(long, string, long, string);
//		~UVLadung();

		long nummer;			// key in UVSchiff

		string name;
		long groesse;			// in BRT
		string besitzer;
};

class UVKomponente
{
	public:
		UVKomponente(string s, long zs, long hp, long gr, long n = 1, double l = 0.0);
//		~UVKomponente();

		long nummer;			// key in UVSchiff

		string name;
		long zustand;
		long hitpoints;
		long groesse;			// in BRT

		double leistung;
};

typedef vector<UVKomponente*>::iterator komponenten_iterator;
typedef vector<UVLadung*>::iterator ladung_iterator;

class UVSchiff
{
	public:
		UVSchiff(string, string, long);
//		~UVSchiff();

		string name;			// key in UVWelt

		string besitzer;
		long groesse;			// in BRT

		long x;
		long y;
		long dim;
		long planet;			// key: UVPlanet in UVWelt
		bool zonenstatus;		// true = verteidigt allierte Zonen
		long werft;				// key: UVWerft in UVPlanet
		string beschreibung;
		double v;				// in KpZ
		long w;					// in °
		long waffenstatus;		// 1..3
		long offensivbereich;
		long fluchtwert;
		long hitpoints;
		bool traktorstrahl;
		double treibstoff;
		long treibstofftanks;
		long lagerraum;
		long lagerraum_frei;

		long sichtweite;

		UVKomponente* bordcomputer;
		UVKomponente* energiegenerator;
		UVKomponente* sensoren;
		UVKomponente* mannschaftsraum;
		UVKomponente* schild;
		UVKomponente* panzerung;

		void set_energiekanone(UVKomponente*);
		UVKomponente* get_energiekanone(const long) const;
		komponenten_iterator first_energiekanone();
		komponenten_iterator last_energiekanone();

		void set_rakete(UVKomponente*);
		UVKomponente* get_rakete(const long) const;
		komponenten_iterator first_rakete();
		komponenten_iterator last_rakete();

		void set_triebwerk(UVKomponente*);
		UVKomponente* get_triebwerk(const long) const;
		komponenten_iterator first_triebwerk();
		komponenten_iterator last_triebwerk();

		void set_ladung(UVLadung*);
		UVLadung* get_ladung(const long) const;
		ladung_iterator first_ladung();
		ladung_iterator last_ladung();

	private:
		vector<UVKomponente*> energiekanonen;
		vector<UVKomponente*> raketen;
		vector<UVKomponente*> triebwerke;
		vector<UVLadung*> ladung;
};

// ===========================================================================

typedef ext::hash_map<long, string>::iterator dim_iterator;
typedef ext::hash_map<string, UVSchiff*>::iterator schiffe_iterator;
typedef ext::hash_map<long, UVPlanet*>::iterator planeten_iterator;
typedef ext::hash_map<string, UVHandelsstation*>::iterator handelsstationen_iterator;
typedef ext::vector<UVAnomalie*>::iterator anomalien_iterator;
typedef ext::vector<UVContainer*>::iterator container_iterator;
typedef ext::hash_map<long, UVSensorsonde*>::iterator sensorsonden_iterator;
typedef ext::hash_map<long, UVInfosonde*>::iterator infosonden_iterator;

class UVWelt
{
	public:
		UVWelt();
		~UVWelt();

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

#endif // WELT_H

