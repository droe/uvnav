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

#ifndef REGEXP_H
#define REGEXP_H

#include "../lib/stl.h"

#include <pcre.h>

class UVRegExp
{
	public:
		UVRegExp(const string&);
		~UVRegExp();

		bool match(const string&);
		int subs();
		string sub(int = 0);
		long subtol(int = 0);
		long long subtoll(int = 0);
		double subtof(int = 0);
		int get_match_end();

	private:
		pcre* re;
		int sub_count;
		int* sub_vector;
		const char** sub_list;
		const char* last_subject;

		inline const char* sub_c_str(int = 0);
		inline void free_sub_list();
};

#endif // REGEXP_H

