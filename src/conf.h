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

#ifndef CONF_H
#define CONF_H

#include "global.h"

class UVConf
{
	public:
		UVConf();
		~UVConf();

		void set_auswertung(string, long);

		string s_get(const string&, bool = false) const;
		void   s_set(const string&, string, bool = false);
		long   l_get(const string&, bool = false) const;
		void   l_set(const string&, long, bool = false);
		double f_get(const string&, bool = false) const;
		void   f_set(const string&, double, bool = false);
		bool   b_get(const string&, bool = false) const;
		void   b_set(const string&, bool, bool = false);

	private:
		string aw_besitzer;
		long aw_sternzeit;

		mutable ext::hash_map<string, string> s_conf;
		mutable ext::hash_map<string, long>   l_conf;
		mutable ext::hash_map<string, double> f_conf;
		mutable ext::hash_map<string, bool>   b_conf;

		void load();
		void save() const;
};

#endif // CONF_H

