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

#ifndef PARSER_TXT_H
#define PARSER_TXT_H

#include "../util/global.h"

#include "../infra/conf.h"
#include "../model/welt.h"
#include "../gui/progress.h"
#include "../util/regexp.h"

class UVParserTXT
{
	public:
		UVParserTXT(UVConf* c, int = 0, UVWelt* = NULL);
		~UVParserTXT();

		void parse(const string& file, UVProgress* pro = NULL);
		UVWelt* get_welt() const;

	private:
		UVConf* conf;
		UVWelt* welt;
		UVProgress* progress;

		ifstream stream;
		string cur;
		unsigned long line;
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

#endif // PARSER_TXT_H

