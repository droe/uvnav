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

#ifndef STLSTRING_H
#define STLSTRING_H

/*
 * STL-Strings.
 */

#include <string>
#include <sstream>

using namespace std;

/*
 * Einfache Stream-maessige Konversion aller Datentypen nach string.
 *
 * string blah = str_stream() << "n=" << 123;
 */
/*
struct str_stream
{
	std::stringstream& get_stream() const
	{
		return s;
	}
	operator std::string() const
	{
		return s.str();
	}
	private:
		mutable std::stringstream s;
};

template<class type>
const str_stream& operator<< (const str_stream& out, const type& value)
{
    out.get_stream() << value;
    return out;
}
*/

template<class type>
inline std::string to_string(const type & value)
{
    std::ostringstream streamOut;
    streamOut << value;
    return streamOut.str();
}

#endif // STLSTRING_H
