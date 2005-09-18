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

#ifndef CONF_H
#define CONF_H

#include "util/singleton.h"
#include "util/stl_hash_map.h"
#include "util/stl_string.h"

/*
 * Version der Konfigurationsdatenbank.
 *
 * CONF_VERSION wird inkrementiert, wenn Keys geloescht oder
 * umbenannt werden, aber nicht, wenn Keys hinzugefuegt werden.
 */
#define CONF_VERSION 3

class UVConf : public Singleton<UVConf>
{
	friend class Singleton<UVConf>;

	private:
		UVConf();
		virtual ~UVConf();

	public:
		void set_auswertung(std::string, long);

		std::string s_get(const std::string&, bool = false) const;
		void        s_set(const std::string&, std::string, bool = false);
		void        s_del(const std::string&);
		long        l_get(const std::string&, bool = false) const;
		void        l_set(const std::string&, long, bool = false);
		void        l_del(const std::string&);
		double      f_get(const std::string&, bool = false) const;
		void        f_set(const std::string&, double, bool = false);
		void        f_del(const std::string&);
		bool        b_get(const std::string&, bool = false) const;
		void        b_set(const std::string&, bool, bool = false);
		void        b_del(const std::string&);

		bool have_data() const;

		void load();
		void save() const;

		void dispose();

	private:
		std::string aw_besitzer;
		long aw_sternzeit;

		mutable ext::hash_map<std::string, std::string> s_conf;
		mutable ext::hash_map<std::string, long>        l_conf;
		mutable ext::hash_map<std::string, double>      f_conf;
		mutable ext::hash_map<std::string, bool>        b_conf;

		void convert();
};
typedef ext::hash_map<std::string, std::string>::iterator s_conf_iter;
typedef ext::hash_map<std::string, long>::iterator        l_conf_iter;
typedef ext::hash_map<std::string, double>::iterator      f_conf_iter;
typedef ext::hash_map<std::string, bool>::iterator        b_conf_iter;

#endif // CONF_H

