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

#ifndef IMPORT_TXT_H
#define IMPORT_TXT_H

#include "dm/abstractimporter.h"

class UVUniversum;
class UVZone;
class UVWerft;
class UVForschungsstation;
class UVStadt;
class UVPlanet;
class UVRegExp;

class UVImportTXT : public UVAbstractImporter
{
	public:
		UVImportTXT();
//		~UVImportTXT();

		void set_verbosity(int v);
		UVUniversum* import(const string& file);
		unsigned long get_filesize() const;
		unsigned long get_bytecount() const;

	private:
		UVUniversum* universum;

		ifstream stream;
		string cur;
		unsigned long line;
		unsigned long filesize;
		unsigned long bytecount;
		int verbosity;

		long stats_schiffe;
		long stats_planeten;

		bool good() const;
		string getline();

		void shiftline(UVRegExp*);

		string strip(string&) const;

		string get_exception(const string&, const string&, const int, const string&) const;
#ifdef DEBUG
		void parse_debug(const string&, UVRegExp*) const;
#endif

		void parse_auswertung();

		void parse_header();
		void parse_oldschool_header();
		void parse_spielerinfos();
		void parse_spielstand();
		void parse_imperatorinfos();
		void parse_allianzen();
		void parse_schiffe(UVPlanet* p = NULL);
		void parse_planeten();
		void parse_oldschool_planeten();
		void parse_sensorenreport();
		void parse_fremde_lager();
		void parse_nachrichten();
		void parse_kampfreport();

		void parse_schiff(UVPlanet* p = NULL);
		void parse_planet();
		void parse_oldschool_planet();
		void parse_handelsstation(UVPlanet*);
		void parse_report_schiff();
		void parse_report_container();
		void parse_report_anomalie();
		void parse_report_sensorsonde();
		void parse_report_infosonde();

		UVZone* parse_zone(UVPlanet* p);
		UVWerft* parse_werft(UVZone* z);
		UVForschungsstation* parse_forschungsstation(UVZone* z);
		UVStadt* parse_stadt(UVZone* z);

		void parse_leerzeile();

		long get_image_planet(const string&) const;
		long get_sichtweite(const string&) const;
};

#endif // IMPORT_TXT_H

