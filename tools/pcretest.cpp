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

#include <pcre.h>

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <exception>
#include <stdexcept>

using namespace std;


/*
 * Einfache Stream-maessige Konversion aller Datentypen nach string.
 *
 * string blah = str_stream() << "n=" << 123;
 */
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


#define EXCEPTION(x) string(str_stream() << x << " in " << __FUNCTION__ << "() at " << __FILE__ << ":" << __LINE__)


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


/*
 * UVRegExp - Regulaere Ausdruecke.
 *
 * Einfacher OO-Wrapper um PCRE.  Beispiel:
 *
 *   UVRegExp* re = new UVRegExp("^Gugus (.*) Dada");
 *   re->match("Gugus juhudihui Dada Ante Portas");
 *   // re->subs() == 1
 *   // re->sub(-1) == "Gugus juhudihui Dada Ante Portas"
 *   // re->sub(0) == "Gugus juhudihui Dada"
 *   // re->sub(1) == "juhudihui"
 *   // re->sub(2) == EXCEPTION
 *   // re->get_match_end() == 20
 */


/*
 * Konstruktor.
 *
 * Der Parameter pattern ist die Regular Expression.
 */
UVRegExp::UVRegExp(const string& pattern)
: re(NULL), sub_count(0), sub_vector(NULL), sub_list(NULL), last_subject(NULL)
{
	const char* err;
	int err_off;

	re = pcre_compile(pattern.c_str(), 0, &err, &err_off, NULL);
	if(re == NULL)
	{
		throw EXCEPTION("PCRE: " << err << " at offset " << err_off);
	}

	int rc = pcre_fullinfo(re, NULL, PCRE_INFO_CAPTURECOUNT, &sub_count);
	if(rc < 0)
	{
		throw EXCEPTION("PCRE: pcre_fullinfo returned " << rc);
	}
	sub_vector = new int[3 * (sub_count + 1)];
}


/*
 * Destruktor.
 */
UVRegExp::~UVRegExp()
{
	free_sub_list();
	delete sub_vector;
	pcre_free(re);
}


/*
 * Einen String mit der RegExp matchen.
 */
bool UVRegExp::match(const string& subject)
{
	last_subject = subject.c_str();
	free_sub_list();
	return (0 < pcre_exec(re, NULL, last_subject, subject.length(),
	                      0, 0, sub_vector, 3 * (sub_count + 1)));
}


/*
 * Einen gecaptureten Substring zurueckgeben.
 *
 * index = 1...n: $1...$n
 * index = 0: ganzer Match
 *
 * Wahlweise kann der Substring gleich in einen Datentyp nach
 * Wahl konvertiert werden:
 *
 * sub(): als string, as-is
 * subtol(): als long (atol)
 * subtoll(): als long long (atoll)
 * subtof(): als double (atof)
 */
inline const char* UVRegExp::sub_c_str(int index)
{
	if(index < 0)
	{
		return last_subject;
	}
	if(sub_list == NULL)
	{
		pcre_get_substring_list(last_subject, sub_vector, sub_count + 1, &sub_list);
	}
	if(sub_list[index] == NULL)
	{
		throw EXCEPTION("Substring nicht vorhanden: " << index);
	}
	return sub_list[index];
}
string UVRegExp::sub(int index)
{
	return string(sub_c_str(index));
}
long UVRegExp::subtol(int index)
{
	return atol(sub_c_str(index));
}
long long UVRegExp::subtoll(int index)
{
	return atoll(sub_c_str(index));
}
double UVRegExp::subtof(int index)
{
	return atof(sub_c_str(index));
}


/*
 * Anzahl der Substrings.
 */
int UVRegExp::subs()
{
	return sub_count;
}


/*
 * Ende des gesamten Matches.
 *
 * Gibt den Offset des ersten Buchstabens an, welcher nicht
 * mehr im Match drin ist. (!)
 * Kann deshalb 1:1 in einem Ausdruck der Form
 *
 *   s.substr(re.get_match_end(), s.len - re.get_match_end())
 *
 * verwendet werden.
 */
int UVRegExp::get_match_end()
{
	return sub_vector[1];
}


/*
 * Loescht die Substring-Liste.
 */
inline void UVRegExp::free_sub_list()
{
	if(sub_list != NULL)
	{
		pcre_free_substring_list(sub_list);
		sub_list = NULL;
	}
}


int main()
{
  UVRegExp* re = new UVRegExp("^Gugus (.*) Dada");
  re->match("Gugus juhudihui Dada Ante Portas");
  cout << "subs: " << re->subs() << " (==1?)" << endl;
}
