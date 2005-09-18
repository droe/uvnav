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

#ifndef SCHIFF_H
#define SCHIFF_H

#include "util/stl_hash_map.h"
#include "util/stl_string.h"

class UVKomponente;
class UVLadung;

typedef std::vector<UVKomponente*>::iterator komponenten_iterator;
typedef std::vector<UVLadung*>::iterator ladung_iterator;

class UVSchiff
{
	public:
		UVSchiff(std::string, std::string, long);
		virtual ~UVSchiff();

		std::string name;			// key in UVWelt

		std::string besitzer;
		long groesse;			// in BRT

		long x;
		long y;
		long dim;
		long planet;			// key: UVPlanet in UVWelt
		bool zonenstatus;		// true = verteidigt allierte Zonen
		long werft;				// key: UVWerft in UVPlanet
		std::string beschreibung;
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
		std::vector<UVKomponente*> energiekanonen;
		std::vector<UVKomponente*> raketen;
		std::vector<UVKomponente*> triebwerke;
		std::vector<UVLadung*> ladung;
};

#endif // SCHIFF_H

