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

#include "conf.h"

#include "../lib/version.h"
#include "../lib/sysdep.h"
#include "../lib/exceptions.h"

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
	b_set("map-kaufradien", false);
	b_set("map-sichtradien", false);
	l_set("map-offset-x", 0);
	l_set("map-offset-y", 0);
	f_set("map-zoom", 200.0);
	l_set("map-dim", 1);
	l_set("map-debug-font-size", 14);
	l_set("map-grid-font-size", 10);
	l_set("map-label-font-size", 10);

	// Konfigurationsdatei drueberladen
	load();
}


/*
 * Destruktor.
 */
UVConf::~UVConf()
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
	long version = 0;

	ifstream stream_in;
	stream_in.open((sysdep_confdir() + "version").c_str());
	if(stream_in.good())
	{
		stream_in >> version;
	}
	stream_in.close();

	// Fallthrough: Schrittweise Aktualisierung auf aktuelle Version.
	switch(version)
	{
		case 0:
			l_del("autoconfig");
		case 1:
			b_del("screen-double-buf");
			b_del("screen-software");
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
	ext::hash_map<string, string>::const_iterator s_iter;
	for(s_iter = s_conf.begin(); s_iter != s_conf.end(); s_iter++)
	{
		stream << "s " << (*s_iter).first << "\t" << (*s_iter).second << endl;
	}

	// long
	ext::hash_map<string, long>::const_iterator l_iter;
	for(l_iter = l_conf.begin(); l_iter != l_conf.end(); l_iter++)
	{
		stream << "l " << (*l_iter).first << "\t" << (*l_iter).second << endl;
	}

	// float
	ext::hash_map<string, double>::const_iterator f_iter;
	for(f_iter = f_conf.begin(); f_iter != f_conf.end(); f_iter++)
	{
		stream << "f " << (*f_iter).first << "\t" << (*f_iter).second << endl;
	}

	// bool
	ext::hash_map<string, bool>::const_iterator b_iter;
	for(b_iter = b_conf.begin(); b_iter != b_conf.end(); b_iter++)
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
		throw EXCEPTION("Unbekannter Konfigurationsschluessel: " + key);
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
void UVConf::s_del(const string& key, bool aw)
{
	if(aw)
	{
		if((aw_besitzer == "") || (aw_sternzeit == 0))
		{
			throw EXCEPTION("Interner Fehler: set_auswertung() nie aufgerufen!");
		}
		string k = aw_besitzer + "@" + to_string(aw_sternzeit) + ":" + key;
		s_conf.erase(k);
	}
	s_conf.erase(key);
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
		throw EXCEPTION("Unbekannter Konfigurationsschluessel: " + key);
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
void UVConf::l_del(const string& key, bool aw)
{
	if(aw)
	{
		if((aw_besitzer == "") || (aw_sternzeit == 0))
		{
			throw EXCEPTION("Interner Fehler: set_auswertung() nie aufgerufen!");
		}
		string k = aw_besitzer + "@" + to_string(aw_sternzeit) + ":" + key;
		l_conf.erase(k);
	}
	l_conf.erase(key);
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
		throw EXCEPTION("Unbekannter Konfigurationsschluessel: " + key);
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
void UVConf::f_del(const string& key, bool aw)
{
	if(aw)
	{
		if((aw_besitzer == "") || (aw_sternzeit == 0))
		{
			throw EXCEPTION("Interner Fehler: set_auswertung() nie aufgerufen!");
		}
		string k = aw_besitzer + "@" + to_string(aw_sternzeit) + ":" + key;
		f_conf.erase(k);
	}
	f_conf.erase(key);
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
		throw EXCEPTION("Unbekannter Konfigurationsschluessel: " + key);
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
void UVConf::b_del(const string& key, bool aw)
{
	if(aw)
	{
		if((aw_besitzer == "") || (aw_sternzeit == 0))
		{
			throw EXCEPTION("Interner Fehler: set_auswertung() nie aufgerufen!");
		}
		string k = aw_besitzer + "@" + to_string(aw_sternzeit) + ":" + key;
		b_conf.erase(k);
	}
	b_conf.erase(key);
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


