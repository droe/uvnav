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

#include "welt.h"

/*
 * Diese Klassen stellen die im Spiel vorhandenen Dinge dar.
 * Die Klassen sollen als von aussen manipulierbare Datenobjekte
 * verstanden werden. Komplexe Funktionalität wird zwar durch
 * Getter und Setter abstrahiert, ist aber trotzdem fuer die
 * Manipulation von aussen gedacht. Also eher wie traditionell
 * Structs benutzt werden. Aus Konsistenzgruenden wurden hier
 * trotzdem Klassen und nicht Structs verwendet.
 */


//============================================================================

/*
 * UVSpieler - Stellt einen Spieler dar.
 */


/*
 * Konstruktor.
 */
UVSpieler::UVSpieler()
: name(""), spieler(""), gesellschaft(""), legal(0), punkte(0), konto(0)
{
}


/*
 * Destruktor.
 */
/*
UVSpieler::~UVSpieler()
{
}
*/


//============================================================================
//============================================================================

/*
 * UVAnomalie - Stellt eine Anomalie dar.
 */


/*
 * Konstruktor.
 */
UVAnomalie::UVAnomalie() : radius(0), x(0), y(0), dim(0)
{
}


/*
 * Destruktor.
 */
/*
UVAnomalie::~UVAnomalie()
{
}
*/


//============================================================================

/*
 * UVContainer - Stellt einen Container dar.
 */

/*
 * Konstruktor.
 */
UVContainer::UVContainer() : groesse(0), x(0), y(0), dim(0)
{
}


/*
 * Destruktor.
 */
/*
UVContainer::~UVContainer()
{
}
*/


//============================================================================

/*
 * UVInfosonde - Stellt eine Infosonde dar.
 */


/*
 * Konstruktor.
 */
UVInfosonde::UVInfosonde(long n) : lebensdauer(0), x(0), y(0), dim(0)
{
	nummer = n;
}


/*
 * Destruktor.
 */
/*
UVInfosonde::~UVInfosonde()
{
}
*/


//============================================================================

/*
 * UVSensorsonde - Stellt eine Sensorsonde dar.
 */


/*
 * Konstruktor.
 */
UVSensorsonde::UVSensorsonde(long n) : lebensdauer(0), x(0), y(0), dim(0)
{
	nummer = n;
}


/*
 * Destruktor.
 */
/*
UVSensorsonde::~UVSensorsonde()
{
}
*/


//============================================================================
//============================================================================

/*
 * UVAgrarfeld - Stellt ein Agrarfeld dar.
 */


/*
 * Konstruktor.
 *
 * Nummer n ist Key, Zone z ist Kontext.
 */
UVAgrarfeld::UVAgrarfeld(long n, UVZone* z, long gr, string pr, long er)
: nummer(n), zone(z->nummer), groesse(gr), produkt(pr), ertrag(er)
{
}


/*
 * Destruktor.
 */
/*
UVAgrarfeld::~UVAgrarfeld()
{
}
*/


//============================================================================

/*
 * UVSpeicherfeld - Stellt ein Speicherfeld dar.
 */


/*
 * Konstruktor.
 */
UVSpeicherfeld::UVSpeicherfeld(long n, UVZone* z, long gr, long sp, long me, string wa)
: nummer(n), zone(z->nummer), groesse(gr), speicherplatz(sp), menge(me), ware(wa)
{
}


/*
 * Destruktor.
 */
/*
UVSpeicherfeld::~UVSpeicherfeld()
{
}
*/


//============================================================================

/*
 * UVMinenfeld - Stellt ein Minenfeld dar.
 */


/*
 * Konstruktor.
 *
 * Nummer n ist Key, Zone z ist Kontext.
 */
UVMinenfeld::UVMinenfeld(long n, UVZone* z, long gr, string ro, long er)
: nummer(n), zone(z->nummer), groesse(gr), rohstoff(ro), ertrag(er)
{
}


/*
 * Destruktor.
 */
/*
UVMinenfeld::~UVMinenfeld()
{
}
*/


//============================================================================

/*
 * UVWerft - Stellt eine Werft dar.
 */


/*
 * Konstruktor.
 */
UVWerft::UVWerft(long n, UVZone* z)
: nummer(n), zone(z->nummer), name(""), groesse(0), formel(""), erzlager(0), beschreibung("")
{
}


/*
 * Destruktor.
 */
/*
UVWerft::~UVWerft()
{
}
*/


//============================================================================

/*
 * UVForschungsstation - Stellt eine Forschungsstation dar.
 */


/*
 * Konstruktor.
 */
UVForschungsstation::UVForschungsstation(long n, UVZone* z)
: nummer(n), zone(z->nummer), name(""), groesse(0), wissenspunkte(0), beschreibung("")
{
}


/*
 * Destruktor.
 */
/*
UVForschungsstation::~UVForschungsstation()
{
}
*/


//============================================================================

/*
 * UVStadt - Stellt eine Stadt dar.
 */


/*
 * Konstruktor.
 */
UVStadt::UVStadt(long n, UVZone* z)
: nummer(n), zone(z->nummer), name(""), groesse(0), einwohner(0), beschreibung("")
{
}


/*
 * Destruktor.
 */
/*
UVStadt::~UVStadt()
{
}
*/


//============================================================================
//============================================================================

/*
 * UVZone - Stellt eine Zone dar.
 */


/*
 * Konstruktor.
 */
UVZone::UVZone(long n)
: nummer(n), name(""), besitzer(""), groesse(0), beschreibung(""), T(0.0), N(0.0)
{
}


/*
 * Destruktor.
 */
/*
UVZone::~UVZone()
{
}
*/


/*
 * Klimazone aus den Klimadaten berechnen.
 */
#define WINTER(x) ((x < 3) || (x >= 9))
#define SOMMER(x) ((x >= 3) || (x < 9))
string UVZone::get_klimazone() const
{
	int i;

	int t_min = 0;	// kaeltester Monat
	int t_max = 0;	// waermster Monat
	int n_min = 0;	// regenaermster Monat
	int n_max = 0;	// regenreichster Monat
	for(i = 1; i < 12; i++)
	{
		if(temperatur[i] < temperatur[t_min]) { t_min = i; }
		if(temperatur[i] > temperatur[t_max]) { t_max = i; }
		if(niederschlag[i] < niederschlag[n_min]) { n_min = i; }
		if(niederschlag[i] > niederschlag[n_max]) { n_max = i; }
	}

	int wn_max = 0;	// feuchtester Wintermonat
	int sn_max = 3;	// feuchtester Sommermonat
	double T_winter = 0;	// mittlere Wintertemperatur
	double T_sommer = 0;	// mittlere Sommertemperatur
	double N_winter = 0;		// total Sommerregen
	double N_sommer = 0;		// total Winterregen
	for(i = 0; i < 12; i++)
	{
		if(WINTER(i))
		{
			if(niederschlag[i] > niederschlag[wn_max]) { wn_max = i; }
			T_winter += temperatur[i];
			N_winter += niederschlag[i];
		}
		else
		{
			if(niederschlag[i] > niederschlag[sn_max]) { sn_max = i; }
			T_sommer += temperatur[i];
			N_sommer += niederschlag[i];
		}
	}
	T_winter /= 6;
	T_winter /= 6;

	// 2: Sommerregen
	// 1: ohne Regenperiode
	// 0: Winterregen
	double toleranz = 0.2;
	double faktor = 1;
	if(N_sommer > N_winter)
	{
		if((N_sommer - N_winter) > (N_winter * toleranz))
		{
			faktor = 2;
		}
	}
	else if(N_winter > N_sommer)
	{
		if((N_winter - N_sommer) + (N_sommer * toleranz))
		{
			faktor = 0;
		}
	}

	// Algorithmus Seite 11 uv292.pdf
	if(temperatur[t_max] >= 10.0)
	{
		if((N < ((2.0 * T) + (faktor * 14.0))) && (N >= (T + (faktor * 7.0))))
		{
			return "BS";
		}
		else if(N < (T + (faktor * 7.0)))
		{
			return "BW";
		}
		else
		{
			if(temperatur[t_min] > 18.0)
			{
				if(niederschlag[n_min] >= 60.0)
				{
					return "Af";
				}
				else
				{
					return "Aw";
				}
			}
			else
			{
				// regenaermster Monat in waermerer Jahreszeit
				if(WINTER(n_min) != (T_sommer > T_winter))
				{
					if(niederschlag[n_min] < niederschlag[wn_max]/3)
					{
						if(temperatur[t_min] < -3.0)
						{
							return "Ds";
						}
						else
						{
							return "Cs";
						}
					}
					else
					{
						if(temperatur[t_min] < -3.0)
						{
							return "Df";
						}
						else
						{
							return "Cf";
						}
					}
				}
				else
				{
					if(niederschlag[n_min] < niederschlag[sn_max]/10)
					{
						if(temperatur[t_min] < -3.0)
						{
							return "Dw";
						}
						else
						{
							return "Cw";
						}
					}
					else
					{
						if(temperatur[t_min] < -3.0)
						{
							return "Df";
						}
						else
						{
							return "Cf";
						}
					}
				}
			}
		}
	}
	else
	{
		if(temperatur[t_max] >= 0.0)
		{
			return "ET";
		}
		else
		{
			return "EF";
		}
	}
}
#undef WINTER
#undef SOMMER


/*
 * Jahresmitteltemperatur berechnen.
 */
double UVZone::get_T() const
{
	double t = 0;
	for(int i = 0; i < 12; i++)
	{
		t += temperatur[i];
	}
	return rint(10 * t / 12) / 10;
}


/*
 * Jahresniederschlag berechnen.
 */
double UVZone::get_N() const
{
	double n = 0;
	for(int i = 0; i < 12; i++)
	{
		n += niederschlag[i];
	}
	return n / 10;
}


//============================================================================


/*
 * UVHandelsstation - Stellt eine Handelsstation dar.
 */


/*
 * Konstruktor.
 */
UVHandelsstation::UVHandelsstation(string n, UVPlanet* p)
: name(n), planet(p->nummer), beschreibung("")
{
}


/*
 * Destruktor.
 */
/*
UVHandelsstation::~UVHandelsstation()
{
}
*/


//============================================================================

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
	if(zonen.at(z->nummer - 1) != NULL)
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

//============================================================================
//============================================================================

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
, lagerraum(0), lagerraum_frei(0)
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


//============================================================================
//============================================================================

/*
 * UVWelt - Repraesentation eines Universum Spielstandes.
 */


/*
 * Konstruktor.
 */
UVWelt::UVWelt()
: partie(""), copyright(""), galaxie(""), motu(""), sternzeit(0)
{
	spieler = new UVSpieler();
}


/*
 * Destruktor.
 */
UVWelt::~UVWelt()
{
	delete spieler;
}


/*
 * Partie setzen, und ueberpruefen, ob bestehende Daten zur selben Partie
 * gehoeren.
 */
void UVWelt::set_partie(const string& s)
{
	if((partie != "") && (partie != s))
	{
		throw EXCEPTION("Auswertung gehoert zu einer anderen Partie!");
	}
	partie = s;
}


/*
 * Name einer Dimension setzen/holen.
 */
void UVWelt::set_dim(const long d, const string& s)
{
	dim[d] = s;
}
string UVWelt::get_dim(long d) const
{
	return dim[d];
}
dim_iterator UVWelt::first_dim()
{
	return dim.begin();
}
dim_iterator UVWelt::last_dim()
{
	return dim.end();
}


/*
 * Spielerobjekt setzen/holen.
 */
UVSpieler* UVWelt::get_spieler() const
{
	return spieler;
}
void UVWelt::set_spieler(UVSpieler* s)
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
void UVWelt::set_schiff(UVSchiff* s, UVPlanet* p)
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
UVSchiff* UVWelt::get_schiff(const string& n) const
{
	return schiffe[n];
}
schiffe_iterator UVWelt::first_schiff()
{
	return schiffe.begin();
}
schiffe_iterator UVWelt::last_schiff()
{
	return schiffe.end();
}


/*
 * Planetenobjekt setzen/holen.
 */
void UVWelt::set_planet(UVPlanet* p)
{
	planeten[p->nummer] = p;
}
UVPlanet* UVWelt::get_planet(const long n) const
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
planeten_iterator UVWelt::first_planet()
{
	return planeten.begin();
}
planeten_iterator UVWelt::last_planet()
{
	return planeten.end();
}


/*
 * Handelsstationsobjekt setzen/holen.
 */
void UVWelt::set_handelsstation(UVHandelsstation* h, UVPlanet* p)
{
	handelsstationen[h->name] = h;
	// bei Planet registrieren
	p->handelsstation = h->name;
}
UVHandelsstation* UVWelt::get_handelsstation(const string& h) const
{
	return handelsstationen[h];
}
handelsstationen_iterator UVWelt::first_handelsstation()
{
	return handelsstationen.begin();
}
handelsstationen_iterator UVWelt::last_handelsstation()
{
	return handelsstationen.end();
}


/*
 * Anomalieobjekt setzen/holen.
 */
void UVWelt::set_anomalie(UVAnomalie* a)
{
	anomalien.push_back(a);
}
UVAnomalie* UVWelt::get_anomalie(const long i) const
{
	if(i <= static_cast<long>(anomalien.size()))
	{
		return anomalien.at(i - 1);
	}
	return NULL;
}
anomalien_iterator UVWelt::first_anomalie()
{
	return anomalien.begin();
}
anomalien_iterator UVWelt::last_anomalie()
{
	return anomalien.end();
}


/*
 * Containerobjekt setzen/holen.
 */
void UVWelt::set_container(UVContainer* c)
{
	container.push_back(c);
}
UVContainer* UVWelt::get_container(const long i) const
{
	if(i <= static_cast<long>(container.size()))
	{
		return container.at(i - 1);
	}
	return NULL;
}
container_iterator UVWelt::first_container()
{
	return container.begin();
}
container_iterator UVWelt::last_container()
{
	return container.end();
}


/*
 * Sensorsondenobjekt setzen/holen.
 */
void UVWelt::set_sensorsonde(UVSensorsonde* s)
{
	sensorsonden[s->nummer] = s;
}
UVSensorsonde* UVWelt::get_sensorsonde(const long n) const
{
	return sensorsonden[n];
}
sensorsonden_iterator UVWelt::first_sensorsonde()
{
	return sensorsonden.begin();
}
sensorsonden_iterator UVWelt::last_sensorsonde()
{
	return sensorsonden.end();
}


/*
 * Infosondenobjekt setzen/holen.
 */
void UVWelt::set_infosonde(UVInfosonde* i)
{
	infosonden[i->nummer] = i;
}
UVInfosonde* UVWelt::get_infosonde(const long n) const
{
	return infosonden[n];
}
infosonden_iterator UVWelt::first_infosonde()
{
	return infosonden.begin();
}
infosonden_iterator UVWelt::last_infosonde()
{
	return infosonden.end();
}


