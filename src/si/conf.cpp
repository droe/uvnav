/*
 * UV Navigator - Auswertungsvisualisierung fuer Universum V
 * Copyright (C) 2004-2006 Daniel Roethlisberger <roe@chronator.ch>
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

#include "conf.h"

#include "util/version.h"
#include "util/sysdep.h"
#include "util/exceptions.h"
#include "util/regexp.h"

#include <iostream>
#include <fstream>
using namespace std;

/*
 * UVConf - Verwaltet die persistente Konfiguration.
 *
 * UVConf unterstuetzt Globale und auswertungsspezifische Schluessel:
 *
 * Wenn *_set|*_get mit optionalem Parameter aw = true aufgerufen werden,
 * so sind die abgefragten oder gesetzten Schluessel auswertungsspezifisch.
 * Das heisst, dass die Schluessel sowohl als globale Default-Werte, als auch
 * pro Auswertung gespeichert werden. So kann eine Option fuer jede
 * Auswertung separat gespeichert werden. Zoomfaktor/Offset pro Auswertung.
 *
 * Damit das funktioniert, muss UVConf mit Hilfe von set_auswertung()
 * der Name der Spielfigur und die Sternzeit mitgeteilt werden.
 * Aufruf von *_get|*_set ohne vorherigen Aufruf von set_auswertung()
 * fuehrt zu einer Exception.
 *
 * *** Option: bool aw -> string aw; mit "aw" =^= true
 *     und (bes x sz x string) sonst
 */


/*
 * Konstruktor.
 *
 * Laedt automatisch die Konfiguration aus Konfigurationsdatei,
 * oder initialisiert die Konfiguration mit den Standardwerten,
 * falls keine Konfigurationsdatei existiert oder die betreffende
 * Einstellung nicht konfiguriert ist.
 */
UVConf::UVConf()
: aw_besitzer(""), aw_sternzeit(0)
{
	// Version von UVNav, mit welcher die Konfiguration generiert wurde
	s_set("version", to_string(PACKAGE_VERSION) + " (" + to_string(revision) + ")");

	// Bildschirm- & Grafikoptionen
	l_set("screen-width", 1024);
	l_set("screen-height", 768);
	b_set("screen-fullscreen", false);
	b_set("screen-resizable", true);
	b_set("screen-quality", true);

	// Optionen der Kartenanzeige
	l_set("map-sichtradien", 2);
	b_set("map-kaufradien", true);
	b_set("map-schiffe", true);
	l_set("map-container", 2);
	b_set("map-verbindungen", true);
	l_set("map-zonen", 2);
	l_set("map-offset-x", 0);
	l_set("map-offset-y", 0);
	f_set("map-zoom", 200.0);
	l_set("map-dim", 1);
	l_set("map-debug-font-size", 14);
	l_set("map-grid-font-size", 10);
	l_set("map-label-font-size", 10);
	l_set("map-zonen-font-size", 9);

	// Optionen der Widgets
	l_set("label-font-size", 14);

	// Konfigurationsdatei drueberladen
	load();
}


/*
 * Destruktor.
 */
UVConf::~UVConf()
{
	dispose(); // geht nur solange niemand von aussen dispose() aufruft...
}


/*
 * Befreit alle Ressourcen.
 */
void UVConf::dispose()
{
	save();
}


/*
 * Setzt aktuelle Auswertung.
 *
 * UVConf speichert auswertungsspezifische Schluesselwerte pro Tupel
 * (Spielername x Sternzeit) ab.
 *
 * Muss aufgerufen werden, bevor pro-Auswertung Konfigurationswerte
 * gesetzt oder gelesen werden.
 */
void UVConf::set_auswertung(string besitzer, long sternzeit)
{
	aw_besitzer = besitzer;
	aw_sternzeit = sternzeit;

	for(unsigned int i = 0; i < aw_besitzer.length(); i++)
	{
		if((aw_besitzer[i] == ' ') || (aw_besitzer[i] == '\t'))
		{
			aw_besitzer[i] = '_';
		}
	}
}


/*
 * Konfiguration auf aktuellen Stand bringen.
 *
 * Bringt die geladene Konfiguration auf aktuellen Stand.
 * Muss direkt nach dem Laden der Konfiguration aufgerufen werden.
 */
void UVConf::convert()
{
	long version = CONF_VERSION;

	ifstream stream_in;
	stream_in.open((sysdep_confdir() + "version").c_str());
	if(stream_in.good())
	{
		stream_in >> version;
	}
	stream_in.close();

	bool old_bool = false;

	// Fallthrough: Schrittweise Aktualisierung auf aktuelle Version.
	switch(version)
	{
		case 0:
			l_del("autoconfig");
		case 1:
			b_del("screen-double-buf");
			b_del("screen-software");
		case 2:
			try {
				old_bool = b_get("map-sichtradien");
				b_del("map-sichtradien");
			} catch(key_not_found_error e) {
				old_bool = true;
			}
			l_set("map-sichtradien", old_bool ? 2 : 0);
		case 3:
			try {
				old_bool = b_get("map-zonen");
				b_del("map-zonen");
			} catch(key_not_found_error e) {
				old_bool = true;
			}
			l_set("map-zonen", old_bool ? 2 : 0);
		case 4:
			try {
				old_bool = b_get("map-container");
				b_del("map-container");
			} catch(key_not_found_error e) {
				old_bool = true;
			}
			l_set("map-container", old_bool ? 2 : 0);
		case CONF_VERSION:
		default:
			break;
	}
}


/*
 * Konfiguration laden.
 *
 * Laedt die Konfigurationsdatei, falls sie vorhanden ist.
 */
#define GETLINE(x,y) { \
	::getline(x, y, '\n'); \
	while(y[0] == ' ' || y[0] == '\t') \
	{ \
		y = y.substr(1, y.length() - 1); \
	} \
}
void UVConf::load()
{
	ifstream stream;
	stream.open((sysdep_confdir() + "config").c_str());

	string type;
	string key;
	string s;
	long l;
	double f;
	bool b;

	while(stream.good())
	{
		stream >> type;
		if(stream.good())
		{
			if(type == "s")
			{
				stream >> key;
				GETLINE(stream, s);
				s_set(key, s);
			}
			else if(type == "l")
			{
				stream >> key >> l;
				l_set(key, l);
			}
			else if(type == "f")
			{
				stream >> key >> f;
				f_set(key, f);
			}
			else if(type == "b")
			{
				stream >> key >> b;
				b_set(key, b);
			}
			else if(type == "#")
			{
				GETLINE(stream, s);
			}
			else
			{
				GETLINE(stream, s);
				cerr << "Warnung: Fehler in Konfigurationsfile ignoriert, Zeile entfernt:" << endl;
				cerr << "\t" << type << " " << s << endl;
			}
		}
	}

	stream.close();

	// Geladene Konfiguration aktualisieren
	convert();
}
#undef GETLINE


/*
 * Konfiguration speichern.
 *
 * Achtung: Wird von Destruktor aufgerufen -- Singletons bereits destroyed!
 */
void UVConf::save() const
{
	sysdep_mkdir(sysdep_confdir(), 0750);

	// Konfiguration nach .uvnav/config
	ofstream stream;
	stream.open((sysdep_confdir() + "config").c_str());
	stream << "# Konfigurationsdatei fuer " << PACKAGE_STRING << endl;
	stream << "# DIESE DATEI WIRD BEI PROGRAMMENDE UEBERSCHRIEBEN!" << endl;
	stream << "# Aenderungen koennen gemacht werden, aber Kommentare gehen verloren." << endl;

	// string
	for(s_conf_iter s_iter = s_conf.begin(); s_iter != s_conf.end(); s_iter++)
	{
		stream << "s " << (*s_iter).first << "\t" << (*s_iter).second << endl;
	}

	// long
	for(l_conf_iter l_iter = l_conf.begin(); l_iter != l_conf.end(); l_iter++)
	{
		stream << "l " << (*l_iter).first << "\t" << (*l_iter).second << endl;
	}

	// float
	for(f_conf_iter f_iter = f_conf.begin(); f_iter != f_conf.end(); f_iter++)
	{
		stream << "f " << (*f_iter).first << "\t" << (*f_iter).second << endl;
	}

	// bool
	for(b_conf_iter b_iter = b_conf.begin(); b_iter != b_conf.end(); b_iter++)
	{
		stream << "b " << (*b_iter).first << "\t" << (*b_iter).second << endl;
	}

	stream.clear();
	stream.close();

	// Version in .uvnav/version
	ofstream stream_ver;
	stream_ver.open((sysdep_confdir() + "version").c_str());
	stream_ver << CONF_VERSION << endl;
	stream_ver.clear();
	stream_ver.close();
}


/*
 * String der Konfiguration getten/setten.
 *
 * Wenn aw = true wird der Schluessel auswertungsspezifisch behandelt.
 */
string UVConf::s_get(const string& key, bool aw) const
{
	if(s_conf.count(key) == 0)
	{
		throw key_not_found_error(key);
	}

	if(aw)
	{
		if((aw_besitzer == "") || (aw_sternzeit == 0))
		{
			throw EXCEPTION("Interner Fehler: set_auswertung() nie aufgerufen!");
		}
		string k = aw_besitzer + "@" + to_string(aw_sternzeit) + ":" + key;
		if(s_conf.count(k) == 0)
		{
			s_conf[k] = s_conf[key];
		}
		return s_conf[k];
	}
	else
	{
		return s_conf[key];
	}
}
void UVConf::s_set(const string& key, string value, bool aw)
{
	if(aw)
	{
		if((aw_besitzer == "") || (aw_sternzeit == 0))
		{
			throw EXCEPTION("Interner Fehler: set_auswertung() nie aufgerufen!");
		}
		string k = aw_besitzer + "@" + to_string(aw_sternzeit) + ":" + key;
		s_conf[k] = value;
	}
	s_conf[key] = value;
}
void UVConf::s_del(const string& key)
{
	UVRegExp* re = new UVRegExp(key + "$");
	for(s_conf_iter iter = s_conf.begin(); iter != s_conf.end(); iter++)
	{
		if(re->match(iter->first))
		{
			s_conf.erase(iter);
		}
	}
}


/*
 * Long der Konfiguration getten/setten.
 *
 * Wenn aw = true wird der Schluessel auswertungsspezifisch behandelt.
 */
long UVConf::l_get(const string& key, bool aw) const
{
	if(l_conf.count(key) == 0)
	{
		throw key_not_found_error(key);
	}

	if(aw)
	{
		if((aw_besitzer == "") || (aw_sternzeit == 0))
		{
			throw EXCEPTION("Interner Fehler: set_auswertung() nie aufgerufen!");
		}
		string k = aw_besitzer + "@" + to_string(aw_sternzeit) + ":" + key;
		if(l_conf.count(k) == 0)
		{
			l_conf[k] = l_conf[key];
		}
		return l_conf[k];
	}
	else
	{
		return l_conf[key];
	}
}
void UVConf::l_set(const string& key, long value, bool aw)
{
	if(aw)
	{
		if((aw_besitzer == "") || (aw_sternzeit == 0))
		{
			throw EXCEPTION("Interner Fehler: set_auswertung() nie aufgerufen!");
		}
		string k = aw_besitzer + "@" + to_string(aw_sternzeit) + ":" + key;
		l_conf[k] = value;
	}
	l_conf[key] = value;
}
void UVConf::l_del(const string& key)
{
	UVRegExp* re = new UVRegExp(key + "$");
	for(l_conf_iter iter = l_conf.begin(); iter != l_conf.end(); iter++)
	{
		if(re->match(iter->first))
		{
			l_conf.erase(iter);
		}
	}
}


/*
 * Double der Konfiguration getten/setten.
 *
 * Wenn aw = true wird der Schluessel auswertungsspezifisch behandelt.
 */
double UVConf::f_get(const string& key, bool aw) const
{
	if(f_conf.count(key) == 0)
	{
		throw key_not_found_error(key);
	}

	if(aw)
	{
		if((aw_besitzer == "") || (aw_sternzeit == 0))
		{
			throw EXCEPTION("Interner Fehler: set_auswertung() nie aufgerufen!");
		}
		string k = aw_besitzer + "@" + to_string(aw_sternzeit) + ":" + key;
		if(f_conf.count(k) == 0)
		{
			f_conf[k] = f_conf[key];
		}
		return f_conf[k];
	}
	else
	{
		return f_conf[key];
	}
}
void UVConf::f_set(const string& key, double value, bool aw)
{
	if(aw)
	{
		if((aw_besitzer == "") || (aw_sternzeit == 0))
		{
			throw EXCEPTION("Interner Fehler: set_auswertung() nie aufgerufen!");
		}
		string k = aw_besitzer + "@" + to_string(aw_sternzeit) + ":" + key;
		f_conf[k] = value;
	}
	f_conf[key] = value;
}
void UVConf::f_del(const string& key)
{
	UVRegExp* re = new UVRegExp(key + "$");
	for(f_conf_iter iter = f_conf.begin(); iter != f_conf.end(); iter++)
	{
		if(re->match(iter->first))
		{
			f_conf.erase(iter);
		}
	}
}


/*
 * Bool der Konfiguration getten/setten.
 *
 * Wenn aw = true wird der Schluessel auswertungsspezifisch behandelt.
 */
bool UVConf::b_get(const string& key, bool aw) const
{
	if(b_conf.count(key) == 0)
	{
		throw key_not_found_error(key);
	}

	if(aw)
	{
		if((aw_besitzer == "") || (aw_sternzeit == 0))
		{
			throw EXCEPTION("Interner Fehler: set_auswertung() nie aufgerufen!");
		}
		string k = aw_besitzer + "@" + to_string(aw_sternzeit) + ":" + key;
		if(b_conf.count(k) == 0)
		{
			b_conf[k] = b_conf[key];
		}
		return b_conf[k];
	}
	else
	{
		return b_conf[key];
	}
}
void UVConf::b_set(const string& key, bool value, bool aw)
{
	if(aw)
	{
		if((aw_besitzer == "") || (aw_sternzeit == 0))
		{
			throw EXCEPTION("Interner Fehler: set_auswertung() nie aufgerufen!");
		}
		string k = aw_besitzer + "@" + to_string(aw_sternzeit) + ":" + key;
		b_conf[k] = value;
	}
	b_conf[key] = value;
}
void UVConf::b_del(const string& key)
{
	UVRegExp* re = new UVRegExp(key + "$");
	for(b_conf_iter iter = b_conf.begin(); iter != b_conf.end(); iter++)
	{
		if(re->match(iter->first))
		{
			b_conf.erase(iter);
		}
	}
}


/*
 * Sind auswertungsspezifische Daten vorhanden?
 */
bool UVConf::have_data() const
{
	string key = "map-zoom";
	if(f_conf.count(key) == 0)
	{
		return false;
	}

	if((aw_besitzer == "") || (aw_sternzeit == 0))
	{
		throw EXCEPTION("Interner Fehler: set_auswertung() nie aufgerufen!");
	}
	string k = aw_besitzer + "@" + to_string(aw_sternzeit) + ":" + key;
	if(f_conf.count(k) == 0)
	{
		return false;
	}
	return true;
}


