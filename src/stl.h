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

#ifndef STL_H
#define STL_H

/*
 * STL-Kompatibilitaet und Helfer.
 *
 * Dieser Header sorgt dafuer, dass wir die STL auf allen Plattformen
 * auf die gleiche Art und Weise benutzen koennen, und definiert einige
 * praktische Helfer.
 *
 * Dieser Header muss nach "config.h" eingebunden werden.
 */

#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <exception>
#include <stdexcept>

#if !defined(HAVE_NAMESPACES)
  #error Require namespaces. Sorry.
#endif


/*
 * C++ STL Extensions von SGI
 */
#if defined(HAVE_STD_EXT_HASH_MAP)
	#include <ext/hash_map>
	namespace ext = std;
	namespace std
	{
		template<> struct hash< std::string >
		{
			size_t operator()( const std::string& x ) const
			{
				return hash< const char* >()( x.c_str() );
			}
		};
	}
#elif defined(HAVE_GNU_EXT_HASH_MAP)
	#include <ext/hash_map>
	namespace ext = ::__gnu_cxx;
	namespace __gnu_cxx
	{
		template<> struct hash< std::string >
		{
			size_t operator()( const std::string& x ) const
			{
				return hash< const char* >()( x.c_str() );
			}
		};
	}
#elif defined(HAVE_GLOBAL_HASH_MAP)
	#include <hash_map>
	namespace ext = std;
	namespace std
	{
		template<> struct hash< std::string >
		{
			size_t operator()( const std::string& x ) const
			{
				return hash< const char* >()( x.c_str() );
			}
		};
	}
#elif defined(HAVE_HASH_MAP_H)
	#include <hash_map.h>
	namespace ext = { using ::hash_map };
	namespace :: // ungetestet
	{
		template<> struct hash< std::string >
		{
			size_t operator()( const std::string& x ) const
			{
				return hash< const char* >()( x.c_str() );
			}
		};
	}
#else
	#error Require hash_map. Sorry.
#endif

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

#endif // STL_H
