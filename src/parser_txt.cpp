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

#include "parser_txt.h"

#include "images.h"
#include "sysdep.h"

/*
 * UVParserTXT - Parser fuer Text-Auswertungen
 *
 * Parst Textauswertungen und erstellt eine entsprechende UVWelt,
 * welche die Daten der Auswertung repraesentiert.
 *
 * Es muss ::parse() aufgerufen werden, um eine Auswertung zu parsen.
 *
 * *** TODO: PCRE-Benutzung optimieren, study() verwenden,
 *           und Regexp-Objekte jeweils nur einmal kompilieren
 *           (entweder function static, oder globale Tabelle, oder Members)
 *           Da dies die Uebersichtlichkeit des Parsers
 *           verringert, sollte das erst gemacht werden, wenn
 *           der Parser zufriedenstellend funktioniert.
 *
 * *** FIXME: Nachrichten parsen!
 * *** FIXME: Imperatoren-Toys parsen!
 */


using namespace pcrepp;

#ifdef DEBUG
#define debug(x) parse_debug(x)
#else
#define debug(x) 
#endif

#undef EXCEPTION
#define EXCEPTION(x) get_exception(x, __FILE__, __LINE__, __FUNCTION__)

/*
 * Konstruktor.
 */
UVParserTXT::UVParserTXT(UVConf* c, UVWelt* w)
: conf(c), welt(w), progress(NULL), re(NULL), verbose(false)
{
	if(welt == NULL)
	{
		welt = new UVWelt();
	}
}


/*
 * Destruktor.
 */
UVParserTXT::~UVParserTXT()
{
}


/*
 * Resultat des Parsens als UVWelt* zurueckgeben.
 */
UVWelt* UVParserTXT::get_welt() const
{
	return welt;
}


/*
 * Parser in Debug-Mode versetzen.
 */
void UVParserTXT::set_verbose()
{
	verbose = true;
}


/*
 * Koennen noch Daten aus Auswertung gelesen werden?
 */
bool UVParserTXT::good() const
{
	return stream.good();
}


/*
 * Eine Zeile aus Auswertung lesen.
 */
string UVParserTXT::getline()
{
	if(!stream.good())
	{
		throw EXCEPTION("Unerwartetes Ende der Auswertung!");
	}
	line++;
	::getline(stream, cur, '\n');
	bytecount += cur.length() + 1;
	if(cur[cur.length() - 1] == '\r')
	{
		cur = cur.substr(0, cur.length() - 1);
	}
	if(verbose)
	{
		cerr << line << ": " << cur << endl;
	}
	if((line % 500 == 0) && (progress))
	{
		progress->update(bytecount);
	}
	return cur;
}


/*
 * Regexp setzen.
 */
void UVParserTXT::set_re(const string& expression)
{
	if(re != NULL)
	{
		delete re;
		re = NULL;
	}
	re = new Pcre(expression);
}


/*
 * Regexp matchen.
 */
bool UVParserTXT::match() const
{
	return re->search(cur);
}


/*
 * Regexp setzen und matchen.
 */
bool UVParserTXT::match(const string& expression)
{
	set_re(expression);
	return match();
}


/*
 * Gematchten Bereich aus der aktuellen Zeile entfernen.
 * Falls ganze Zeile gematcht hat, neue Zeile einlesen.
 */
void UVParserTXT::shiftline()
{
	cur = cur.substr(re->get_match_end() + 1, cur.length() - re->get_match_end() - 1);
	if(cur == "")
	{
		getline();
	}
}


/*
 * Leerschlaege an Anfang und Ende eines Strings entfernen.
 */
string UVParserTXT::strip(string& s) const
{
	while(s[0] == ' ')
	{
		s = s.substr(1, s.length() - 1);
	}
	while(s[s.length() - 1] == ' ')
	{
		s = s.substr(0, s.length() - 1);
	}
	return s;
}


/*
 * Laedt eine Datei und jagt sie durch den Parser.
 */
void UVParserTXT::parse(const string& file, UVProgress* pro)
{
	cout << "Lade Auswertung: " << file << endl;
	progress = pro;
	if(progress)
	{
		progress->init(sysdep_filesize(file));
	}

	stream.open(file.c_str());
	line = 0;
	bytecount = 0;

	try
	{
		parse_auswertung();
	}
	catch(Pcre::exception e)
	{
		throw EXCEPTION(string("Regexp: ") + e.what());
	}

	stream.clear();
	stream.close();
	cout << "Auswertung erfolgreich geladen." << endl;
}


/*
 * Eine ganze Auswertung parsen.
 */
void UVParserTXT::parse_auswertung()
{
	parse_header();
	parse_spielerinfos();
	parse_spielstand();

	if(match("^Spionageabwehr:"))
	{
		parse_imperatorinfos();
	}

	parse_allianzen();
	parse_schiffe();
	parse_planeten();

	if(cur == "Sensorenreport:")
	{
		parse_sensorenreport();
	}

	if(match("^Lagerbest.nde auf fremden Schiffen:"))
	{
		parse_fremde_lager();
	}

	if(cur == "Besondere Nachrichten:")
	{
		parse_nachrichten();
	}
}


/*
 * Die Kopfzeile suchen und parsen.
 */
void UVParserTXT::parse_header()
{
	//   --- Universum V --- a PBM (c) 1994-2004 by Black Bird Software
	set_re("--- (.*?) --- (.*)$");
	while(!match())
	{
		getline();
	}
	welt->set_partie((*re)[0]);
	welt->copyright = (*re)[1];
	getline();

	parse_leerzeile();
}


/*
 * Die Spielerinfos parsen.
 */
void UVParserTXT::parse_spielerinfos()
{
	UVSpieler* s = new UVSpieler();

	// Name: Doctor Who
	if(!match("^Name: +(.*)$"))
	{
		throw EXCEPTION("Name: fehlt!");
	}
	s->name = (*re)[0];
	getline();

	// Spieler: Daniel Röthlisberger
	// Spieler: NSC
	if(!match("^Spieler: +(.*)$"))
	{
		throw EXCEPTION("Spieler: fehlt!");
	}
	s->spieler = (*re)[0];
	getline();

	// Status: Freischaffender
	// Status: Imperator
	if(!match("^Status: +(.*)$"))
	{
		throw EXCEPTION("Status: fehlt!");
	}
	s->status = (*re)[0];
	getline();

	// Gesellschaft: Keine
	if(!match("^Gesellschaft: +(.*)$"))
	{
		throw EXCEPTION("Gesellschaft: fehlt!");
	}
	s->gesellschaft = (*re)[0];
	getline();

	parse_leerzeile();

	// Legalitätsstatus:  5
	if(!match("^Legalit.tsstatus: +(.*)$"))
	{
		throw EXCEPTION("Legalitätsstatus: fehlt!");
	}
	s->legal = atol((*re)[0].c_str());
	getline();

	// Punkte: 886
	if(!match("^Punkte: +(.*)$"))
	{
		throw EXCEPTION("Punkte: fehlt!");
	}
	s->legal = atoll((*re)[0].c_str());
	getline();

	//?Erzertrag: 120 t Erz
	if(match("^Erzertrag: +(.*) t Erz$"))
	{
		s->erzertrag = atol((*re)[0].c_str());
		getline();
	}

	// Konto: 214461 Credits
	if(!match("^Konto: +(.*) Credits$"))
	{
		throw EXCEPTION("Konto: fehlt!");
	}
	s->konto = atoll((*re)[0].c_str());
	getline();

	welt->set_spieler(s);

	parse_leerzeile();
}


/*
 * Die Spielstand-Infos parsen.
 */
void UVParserTXT::parse_spielstand()
{
	// MotU: Roman Meng
	if(!match("^MotU: +(.*)$"))
	{
		throw EXCEPTION("MotU: fehlt!");
	}
	welt->motu = (*re)[0];
	getline();

	// Sternzeit: 214
	if(!match("^Sternzeit: +(.*)$"))
	{
		throw EXCEPTION("Sternzeit: fehlt!");
	}
	welt->sternzeit = atol((*re)[0].c_str());
	getline();

	parse_leerzeile();
}


/*
 * Die Imperatoren-Infos parsen.
 */
void UVParserTXT::parse_imperatorinfos()
{
	// Spionageabwehr: 0% Erzertrag
	if(!match("^Spionageabwehr: +(.*)% Erzertrag$"))
	{
		throw EXCEPTION("Spionageabwehr: fehlt!");
	}
	welt->get_spieler()->spionageabwehr = atol((*re)[0].c_str());
	getline();

	parse_leerzeile();
}


/*
 * Die Allianzen und Kriege parsen.
 */
void UVParserTXT::parse_allianzen()
{
	// *** TODO: Parsing der Allianzpartner vollenden

	//?Mitgesellschafter: Plinius, Rumba Joe, Gigabyte Goeffrey
	if(match("^Mitgesellschafter: +(.*)$"))
	{
//		cerr << "IGNORED: [" << (*re)[0] << "]" << endl;
		getline();
	}

	// Allianz mit Spieler: Zsakash (2), Xantes hocar (1), Kynar (2)
	if(!match("^Allianz mit Spieler: +(.*)$"))
	{
		throw EXCEPTION("Allianz mit Spieler: fehlt!");
	}
//	cerr << "IGNORED: [" << (*re)[0] << "]" << endl;
	getline();

	// Sie haben diesen Spielern den Krieg erklärt: 
	if(!match("^Sie haben diesen Spielern den Krieg erkl.rt: +(.*)$"))
	{
		throw EXCEPTION("Sie haben diesen Spielern den Krieg erklärt: fehlt!");
	}
//	cerr << "IGNORED: [" << (*re)[0] << "]" << endl;
	getline();

	// Diese Spieler haben Ihnen den Krieg erklärt: 
	if(!match("^Diese Spieler haben Ihnen den Krieg erkl.rt: +(.*)$"))
	{
		throw EXCEPTION("Diese Spieler haben Ihnen den Krieg erklärt: fehlt!");
	}
//	cerr << "IGNORED: [" << (*re)[0] << "]" << endl;
	getline();

	// Sie haben diesen Gesellschaften den Krieg erklärt: 
	if(!match("^Sie haben diesen Gesellschaften den Krieg erkl.rt: +(.*)$"))
	{
		throw EXCEPTION("Sie haben diesen Gesellschaften den Krieg erklärt: fehlt!");
	}
//	cerr << "IGNORED: [" << (*re)[0] << "]" << endl;
	getline();

	// Diese Spieler haben Ihrer Gesellschaft den Krieg erklärt: 
	if(!match("^Diese Spieler haben Ihrer Gesellschaft den Krieg erkl.rt: +(.*)$"))
	{
		throw EXCEPTION("Diese Spieler haben Ihrer Gesellschaft den Krieg erklärt: fehlt!");
	}
//	cerr << "IGNORED: [" << (*re)[0] << "]" << endl;
	getline();

	// Diese Spieler haben allen den Krieg erklärt: Clingons Wadish, _
	//     Clingons Emen, Pirat Fjodr, Pirat Knork, Pirat Killerjoe, _
	//     Clingons Ishmani, Pirat Langfinger-Ede
	if(!match("^Diese Spieler haben allen den Krieg erkl.rt: +(.*)$"))
	{
		throw EXCEPTION("Diese Spieler haben allen den Krieg erklärt: fehlt!");
	}
//	cerr << "IGNORED: [" << (*re)[0] << "]" << endl;
	getline();

	parse_leerzeile();
}


/*
 * Ein Block Schiffe parsen.
 *
 * Planet p ist der Kontext. Wenn p == NULL handelt es sich um die
 * globale Schiffssektion am Auswertungsanfang, ansonsten werden die
 * Schiffe Planet p zugeordnet.
 */
void UVParserTXT::parse_schiffe(UVPlanet* p)
{
	// * Schiff Cristina (Doctor Who) 100000 BRT  (118314,15922,4 - Vaeroch Agei)
	//   [...]
	Pcre pcre("^\\* Schiff ");
	while(pcre.search(cur))
	{
		parse_schiff(p);
	}

	if(p == NULL)
	{
		parse_leerzeile();
	}
}


/*
 * Die Planeten parsen.
 */
void UVParserTXT::parse_planeten()
{
	// Coventina (5201) (Niemand) (120588,16267,4 - Vaeroch Agei) (5498,5527)
	//  (5152) (Niemand) (123048,21142,4 - Vaeroch Agei)
	Pcre pcre("^.*? \\([0-9]+\\) \\(.*?\\) \\(-?[0-9]+,-?[0-9]+,[0-9]+ - .+?\\)(?: \\([0-9,]+\\))?$");
	while(pcre.search(cur))
	{
		parse_planet();
	}

	parse_leerzeile();
}


/*
 * Den Sensorenreport parsen.
 */
void UVParserTXT::parse_sensorenreport()
{
	// Sensorenreport:
	if(cur != "Sensorenreport:")
	{
		throw EXCEPTION("Fehler in Sensorenreport!");
	}
	getline();
	
	parse_leerzeile();

	Pcre pcre("^  (Schiff|Container|Kosmische|SensorSonde|InfoSonde)");
	while(pcre.search(cur))
	{
		if(pcre[0] == "Schiff")
		{
			parse_report_schiff();
		}
		else if(pcre[0] == "Container")
		{
			parse_report_container();
		}
		else if(pcre[0] == "Kosmische")
		{
			parse_report_anomalie();
		}
		else if(pcre[0] == "SensorSonde")
		{
			parse_report_sensorsonde();
		}
		else if(pcre[0] == "InfoSonde")
		{
			parse_report_infosonde();
		}
		else
		{
			throw EXCEPTION("Fehler im Sensorenreport!");
		}
	}

	parse_leerzeile();
	parse_leerzeile();
}


/*
 * Lagerbestaende auf fremden Schiffen parsen.
 */
void UVParserTXT::parse_fremde_lager()
{
	if(!match("^Lagerbest.nde auf fremden Schiffen:$"))
	{
		throw EXCEPTION("Fehler in Lagerbestaende auf fremden Schiffen!");
	}
	getline();

	parse_leerzeile();

	// *** Schiffe und Lager parsen
	while(match("^  "))
	{
		getline();
	}
	parse_leerzeile();
	parse_leerzeile();
}


/*
 * Nachrichten parsen.
 */
void UVParserTXT::parse_nachrichten()
{
	if(!match("^Besondere Nachrichten:$"))
	{
		throw EXCEPTION("Fehler in besonderen Nachrichten!");
	}
	getline();

	parse_leerzeile();

	// *** Besondere Nachrichten (selektiv) parsen
	while(match("^- "))
	{
		getline();
	}
}


/*
 * Ein Schiff parsen.
 *
 * Planet p ist der Kontext. Wenn p == NULL handelt es sich um ein Schiff
 * in der globalen Schiffssektion am Auswertungsanfang, ansonsten wird das
 * Schiff dem Planet p zugeordnet.
 */
void UVParserTXT::parse_schiff(UVPlanet* p)
{
	// * Schiff Cristina (Doctor Who) 100000 BRT 
	// * Schiff Cristina (Doctor Who) 100000 BRT in der Werft Hubertus (1) 
	// * Schiff Cristina (Doctor Who) 100000 BRT verteidigt auch allierte Zonen
	// * Schiff Cristina (Doctor Who) 100000 BRT  (118314,15922,4 - Vaeroch Agei)
	if(!match("^\\* Schiff (.*?) \\((.*?)\\) (\\d+) BRT((?: in der Werft .*? \\([0-9]+\\))?) ((?:verteidigt auch allierte Zonen)?)(?: \\((-?[0-9]+),(-?[0-9]+),([0-9]+) - (.*?)\\))?$"))
	{
		throw EXCEPTION("Fehler in Schiff!");
	}
	debug("schiff");
	UVSchiff* s = new UVSchiff(
		(*re)[0],
		(*re)[1],
		atol((*re)[2].c_str()));
	if((*re)[3] != "")
	{
		Pcre pcre("^ in der Werft .*? \\(([0-9]+)\\)$");
		pcre.search((*re)[3]);
		s->werft = atol(pcre[0].c_str());
	}
	else
	{
		s->werft = 0;
	}
	s->zonenstatus = ((*re)[4] != "");
	if(re->matches() == 5)
	{
		if(p == NULL)
		{
			throw EXCEPTION("Schiff in Orbit ohne Planet!");
		}
	}
	else
	{
		s->x = atol((*re)[5].c_str());
		s->y = atol((*re)[6].c_str());
		s->dim = atol((*re)[7].c_str());
		welt->set_dim(s->dim, (*re)[8]);
		s->planet = 0;
	}
	getline();

	//?  Gemaechlich tuckert die Cristina vor sich hin. [...]
	if((cur != "") && !match("^(?:[^ ] |  (?:Geschwindigkeit|Waffenstatus): )"))
	{
		debug("schiff-beschreibung");
		s->beschreibung = cur.substr(2, cur.length() - 2);
		getline();
	}
	else
	{
		s->beschreibung = "";
	}

	//?  Geschwindigkeit: 17.81 KpZ / Ausrichtung: 252°
	if(match("^  Geschwindigkeit: ([0-9.]+) KpZ / Ausrichtung: ([0-9]+)°$"))
	{
		debug("schiff-geschwindigkeit");
		s->v = atof((*re)[0].c_str());
		s->w = atol((*re)[1].c_str());
		getline();
	}
	else
	{
		s->v = 0;
		s->w = 0;
	}

	//?  Waffenstatus: 3 / Offensivbereich: 0 / Fluchtwert: 296/367 HP
	if(match("^  Waffenstatus: ([0-9]) / Offensivbereich: ([0-9]+) / Fluchtwert: ([0-9]+)/([0-9]+) HP$"))
	{
		debug("schiff-waffenstatus");
		s->waffenstatus = atol((*re)[0].c_str());
		s->offensivbereich = atol((*re)[1].c_str());
		s->fluchtwert = atol((*re)[2].c_str());
		s->hitpoints = atol((*re)[3].c_str());
		getline();
	}

	//?  Traktorstrahl: An / Treibstofftanks: 14058.008/20000 BRT 
	if(match("^  Traktorstrahl: (An|Aus) / Treibstofftanks: ([0-9.]+)/([0-9]+) BRT $"))
	{
		debug("schiff-traktorstrahl");
		s->traktorstrahl = ((*re)[0] == "An");
		s->treibstoff = atof((*re)[1].c_str());
		s->treibstofftanks = atol((*re)[2].c_str());
		getline();
	}

	//?  =------------------------------------------------------------=
	if(match("^  =-+=$"))
	{
		getline();
	}

	//?  Pray CX (40/40) 4000 BRT, Argongenerator (40/40) 4000 BRT, SensoStar II (20/20) 3000 BRT
	//?  Kein Bordcomputer, Kein Energiegenerator
	if(match("^  (?!<[0-9]+\\. )(?:(.*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT|(Kein) (B)(o)(r)dcomputer), (?:(.*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT|(Kein) (E)(n)(e)rgiegenerator)(?:, (.*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT)?$"))
	{
		debug("schiff-bordcomputer");
		if((*re)[0] != "Kein")
		{
			s->bordcomputer = new UVKomponente((*re)[0],
							atol((*re)[1].c_str()),
							atol((*re)[2].c_str()),
							atol((*re)[3].c_str()));
		}
		if((*re)[4] != "Kein")
		{
			s->energiegenerator = new UVKomponente((*re)[4],
							atol((*re)[5].c_str()),
							atol((*re)[6].c_str()),
							atol((*re)[7].c_str()));
		}
		if(re->matches() > 8)
		{
			s->sensoren = new UVKomponente((*re)[8],
							atol((*re)[9].c_str()),
							atol((*re)[10].c_str()),
							atol((*re)[11].c_str()));
		}
		getline();
	}

	//?  Brücke, Standardkabinen (40/40) 2000 BRT
	if(match("^  (?!<[0-9]+\\. )(Br.cke.*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT$"))
	{
		debug("schiff-mannschaftsraum");
		s->mannschaftsraum = new UVKomponente((*re)[0],
						atol((*re)[1].c_str()),
						atol((*re)[2].c_str()),
						atol((*re)[3].c_str()));
		getline();
	}

	//?  Energieschild I (50/50) 1000 BRT
	//?  Energieschild I (50/50) 1000 BRT, Metallpanzer (200/200) 10000 BRT
	if(match("^  (?!<[0-9]+\\. )(.*?(?:[Ss]child|Atomd.mpfer|schirm).*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT(?:, )?"))
	{
		debug("schiff-energieschild");
		s->schild = new UVKomponente((*re)[0],
						atol((*re)[1].c_str()),
						atol((*re)[2].c_str()),
						atol((*re)[3].c_str()));
		shiftline();
	}

	//?  Metallpanzer (200/200) 10000 BRT
	if(match("^(?:  )?(?!<[0-9]+\\. )(.*?(?:panzer|beschichtung|Zellneuralnetz|Dunkelmaterie).*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT$"))
	{
		debug("schiff-metallpanzer");
		s->panzerung = new UVKomponente((*re)[0],
						atol((*re)[1].c_str()),
						atol((*re)[2].c_str()),
						atol((*re)[3].c_str()));
		getline();
	}

	//?  1. Impulslaser (30/30) 10000 BRT, 2. Impulslaser (30/30) 10000 BRT
	set_re("^(?:  )?([0-9]+)\\. (.*?(?:laser|beschleuniger|Desintegrations-Strahler|Nukleoidenwerfer|kanone).*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT(?:, )?");
	while(match())
	{
		debug("schiff-energiekanone");
		s->set_energiekanone(new UVKomponente(
				(*re)[1],
				atol((*re)[2].c_str()),
				atol((*re)[3].c_str()),
				atol((*re)[4].c_str()),
				atol((*re)[0].c_str())));
		shiftline();
	}

	//?  1. Raketenwerfer (30/30) 10000 BRT, 2. Raketenwerfer (30/30) 10000 BRT
	set_re("^(?:  )?([0-9]+)\\. (.*?(?:[Rr]akete|[Tt]orpedo).*?) \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT(?:, )?");
	while(match())
	{
		debug("schiff-raketenwerfer");
		s->set_rakete(new UVKomponente(
				(*re)[1],
				atol((*re)[2].c_str()),
				atol((*re)[3].c_str()),
				atol((*re)[4].c_str()),
				atol((*re)[0].c_str())));
		shiftline();
	}

	//?  1. Erweitertes Schubtriebwerk (89.097% Leistung) (37/40) 25000 BRT, 2. Erweitertes Schubtriebwerk (89.097% Leistung) (37/40) 25000 BRT
	set_re("^(?:  )?([0-9]+)\\. (.*?)(?: \\(([0-9.]+)% Leistung\\))? \\(([0-9]+)/([0-9]+)\\) ([0-9]+) BRT(?:, )?");
	while(match())
	{
		debug("schiff-schubtriebwerk");
		s->set_triebwerk(new UVKomponente(
				(*re)[1],
				atol((*re)[3].c_str()),
				atol((*re)[4].c_str()),
				atol((*re)[5].c_str()),
				atol((*re)[0].c_str()),
				atof((*re)[2].c_str())));
		shiftline();
	}

	//?  =------------------------------------------------------------=
	if(match("^  =-+=$"))
	{
		getline();
	}

	//?  Lagerraum: 10000 BRT gesamt, 5259 BRT frei
	if(match("^  Lagerraum: ([0-9]+) BRT gesamt, ([0-9]+) BRT frei$"))
	{
		debug("schiff-lagerraum");
		s->lagerraum = atol((*re)[0].c_str());
		s->lagerraum_frei = atol((*re)[1].c_str());
		getline();
	}

	//*   1. Hopfen (4741 BRT, Doctor Who)
	//*   2. Militaerlaser Klasse X (25000 BRT, Doctor Who)
	//*   3. Boosterpack (10 BRT, Doctor Who)
	//*   4. Forschungsdokument WAB (110 BRT, Doctor Who)
	set_re("^   ([0-9]+)\\. (.*?) \\(([0-9]+) BRT, (.*?)\\)$");
	while(match())
	{
		debug("schiff-lagerraum-ladung");
		s->set_ladung(new UVLadung(
				atol((*re)[0].c_str()),
				(*re)[1],
				atol((*re)[2].c_str()),
				(*re)[3]));
		getline();
	}

	welt->set_schiff(s, p);
}


/*
 * Ein Planet parsen.
 */
void UVParserTXT::parse_planet()
{
	// Coventina (5201) (Niemand) (120588,16267,4 - Vaeroch Agei) (5498,5527)
	if(!match("^(.*?) \\(([0-9]+)\\) \\((.*?)\\) \\((-?[0-9]+),(-?[0-9]+),([0-9]+) - (.*?)\\)(?: \\(([0-9,]+)\\))?$"))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	debug("planet");
	UVPlanet* p = new UVPlanet(
		atol((*re)[1].c_str()),
		(*re)[0],
		(*re)[2],
		atol((*re)[3].c_str()),
		atol((*re)[4].c_str()),
		atol((*re)[5].c_str()));
	welt->set_dim(p->dim, (*re)[6]);
	if(re->matches() > 7)
	{
		string links_str = (*re)[7];
		set_re(",");
		vector<string> links_v = re->split(links_str);
		for(unsigned long i = 0; i < links_v.size(); i++)
		{
			p->nachbarn.push_back(atol(links_v[i].c_str()));
		}
	}
	getline();

	// Ausserlich einer unscheinbaren Murmel gleich, ...
	if(!match("^Bev.lkerung: "))
	{
		debug("planet-beschreibung");
		p->beschreibung = cur;
		getline();
	}

	// Bevölkerung: 1485.2 Millionen (Ruhig)
	// Bevölkerung: 623.8 Millionen (REVOLUTION!)
	if(!match("^Bev.lkerung: ([0-9.]+) Millionen \\((.+?)\\)$"))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	p->bevoelkerung = atof((*re)[0].c_str());
	p->zustand = (*re)[1];
	debug("planet-bevoelkerung");
	getline();

	// Minen: 7/65 Fabriken: 5/43
	if(!match("^Minen: ([0-9]+)/([0-9]+) Fabriken: ([0-9]+)/([0-9]+)$"))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	debug("planet-minenfabriken");
	p->minen = atol((*re)[0].c_str());
	p->minen_max = atol((*re)[1].c_str());
	p->fabriken = atol((*re)[2].c_str());
	p->fabriken_max = atol((*re)[3].c_str());
	getline();

	//?Produktion: 0% für Lager, 100% für Bevölkerung
	if(match("^Produktion: ([0-9]+)% f.r Lager, [0-9]+% f.r Bev.lkerung$"))
	{
		debug("planet-produktion");
		p->produktion = atol((*re)[0].c_str());
		getline();
	}

	// Tropische Vegetation, sehr fruchtbar, 29710 km Diameter
	if(!match("^(.*), ([0-9]+) km Diameter$"))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	debug("planet-klima");
	p->klima = (*re)[0];
	p->image = get_image_planet(p->klima);
	p->diameter = atol((*re)[1].c_str());
	getline();

	// Techlevel: 1
	// Techlevel: 1 Energiegenerator: 5 Tribut: 1234 t Erz
	if(!match("^Techlevel: ([0-9]+)(?: Energiegenerator: ([0-9]+) Tribut: ([0-9]+) t Erz)?$"))
	{
		throw EXCEPTION("Fehler in Planet!");
	}
	debug("planet-techlevel");
	p->techlevel = atol((*re)[0].c_str());
	if(re->matches() == 3)
	{
		p->energiegenerator = atol((*re)[1].c_str());
		p->tribut = atol((*re)[2].c_str());
	}
	getline();

	// X-Batts: 1 Y-Batts: 1 Z-Batts: 1
	if(match("^X-Batts: ([0-9]+) Y-Batts: ([0-9]+) Z-Batts: ([0-9]+) $"))
	{
		debug("planet-batts");
		p->xbatts = atol((*re)[0].c_str());
		p->ybatts = atol((*re)[1].c_str());
		p->zbatts = atol((*re)[2].c_str());
		getline();
	}

	welt->set_planet(p);

	// Zone  (3) (Niemand) (278 FUs)
	Pcre pcre("^Zone ");
	while(pcre.search(cur))
	{
		p->set_zone(parse_zone(p));
	}

	// § Handelsstation 'Red Blood'
	if(match("^§ Handelsstation "))
	{
		parse_handelsstation(p);
	}

	// *** IMP Basen
	while(match("^\\+ "))
	{
		debug("planet-impbase-skip");
		getline();
		while(match("^  "))
		{
			debug("planet-impbase-skip-whitespace");
			getline();
		}
	}

	parse_schiffe(p);

	// *** IMP Toys
	while(match("^\\+ "))
	{
		debug("planet-imptoys-skip");
		getline();
		while(match("^  "))
		{
			debug("planet-imptoys-skip-whitespace");
			getline();
		}
	}

	parse_leerzeile();
}


/*
 * Eine Zone parsen.
 */
UVZone* UVParserTXT::parse_zone(UVPlanet* p)
{
	UVZone* z;

	// Zone  (4) (Doctor Who) (193 FUs)
	if(!match("^Zone (.*?) \\(([0-9]+)\\) \\((.+?)\\) \\(([0-9]+) FUs\\)$"))
	{
		throw EXCEPTION("Fehler in Zone!");
	}
	debug("zone");
	z = new UVZone(atol((*re)[1].c_str()));
	z->name = (*re)[0];
	z->besitzer = (*re)[2];
	z->groesse = atol((*re)[3].c_str());
	getline();

	// Beschreibung
	if(!match("^(?:[ -][0-9.]+ \\t){12}T:[ -][0-9.]+ °C$"))
	{
		debug("zone-beschreibung");
		z->beschreibung = cur;
		getline();
	}

	int i = 0;

	//  5.8 	 5.7 	 6.3 	 7.7 	 9.7 	 11.3 	 12.5 	 10.6 	 9.3 	 7 	 5.7 	 5.8 	T: 8.1 °C
	// -3.3 	-3.5 	-1.4 	 3.8 	 11.2 	 16.9 	 21.3 	 14.5 	 9.6 	 1.1 	-3.5 	-3.3 	T: 5.2 °C
	// -25 	-27.5 	-25 	-20.1 	-15.1 	-10.1 	-5.2 	-10.1 	-15.1 	-20.1 	-23.8 	-25 	T:-18.6 °C
	if(!match("^([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\t([ -][0-9.]+) \\tT:([ -][0-9.]+) °C$"))
	{
		throw EXCEPTION("Fehler in Klimadaten!");
	}
	debug("klima-temperaturen");
	for(i = 0; i < 12; i++)
	{
		z->temperatur[i] = atof((*re)[i].c_str());
	}
	z->T = atof((*re)[i].c_str());
	getline();

	//  138 	 124 	 111 	 69 	 76 	 41 	 21 	 14 	 32 	 111 	 124 	 138 	N: 99.9 cm
	//  127 	 164 	 146 	 91 	 73 	 84 	 100 	 120 	 91 	 146 	 164 	 127 	N: 143.3 cm
	//  17 	 13 	 9 	 6 	 8 	 13 	 6 	 7 	 8 	 4 	 9 	 13 	N: 11.3 cm
	if(!match("^ ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\t ([0-9]+) \\tN: ([0-9.]+) cm$"))
	{
		throw EXCEPTION("Fehler in Klima!");
	}
	debug("klima-niederschlag");
	for(i = 0; i < 12; i++)
	{
		z->niederschlag[i] = atof((*re)[i].c_str());
	}
	z->N = atof((*re)[i].c_str());
	getline();

	if(z->N != z->get_N())
	{
		cerr << "Warnung: Klimadaten inkonsistent auf Zeile " << line << " (N:" << z->N << "!=" << z->get_N() << ")" << endl;
	}
	if(abs(z->T - z->get_T()) > 0.11)
	{
		cerr << "Warnung: Klimadaten inkonsistent auf Zeile " << line << " (T:" << z->T << "!=" << z->get_T() << ")" << endl;
	}

	// # Agrarfeld 1, 10 FUs
	// # Agrarfeld 1, 10 FUs: Hirse angebaut.200 BRT Ertrag.
	set_re("^# Agrarfeld ([0-9]+), ([0-9]+) FUs(?:: (.*?) angebaut\\.([0-9]+) BRT Ertrag\\.)?$");
	while(match())
	{
		debug("zone-agrarfeld");
		if(re->matches() == 2)
		{
			p->set_agrarfeld(new UVAgrarfeld(
				atol((*re)[0].c_str()),
				z,
				atol((*re)[1].c_str())), z);
		}
		else
		{
			p->set_agrarfeld(new UVAgrarfeld(
				atol((*re)[0].c_str()),
				z,
				atol((*re)[1].c_str()),
				(*re)[2],
				atol((*re)[3].c_str())), z);
		}
		getline();
	}

	// # SpeicherFeld 1, 10 FUs
	// # SpeicherFeld 1, 10 FUs: 1000 BRT Speicherplatz. Lager leer.
	// # SpeicherFeld 1, 10 FUs: 1000 BRT Speicherplatz. Gelagert werden 1000 BRT Hirse.
	set_re("^# SpeicherFeld ([0-9]+), ([0-9]+) FUs(?:: ([0-9]+) BRT Speicherplatz\\. (?:Lager leer\\.|Gelagert werden ([0-9]+) BRT (.*)\\.)?)?$");
	while(match())
	{
		debug("zone-speicherfeld");
		if(re->matches() == 2)
		{
			p->set_speicherfeld(new UVSpeicherfeld(
				atol((*re)[0].c_str()),
				z,
				atol((*re)[1].c_str())), z);
		}
		else if(re->matches() == 3)
		{
			p->set_speicherfeld(new UVSpeicherfeld(
				atol((*re)[0].c_str()),
				z,
				atol((*re)[1].c_str()),
				atol((*re)[2].c_str())), z);
		}
		else
		{
			p->set_speicherfeld(new UVSpeicherfeld(
				atol((*re)[0].c_str()),
				z,
				atol((*re)[1].c_str()),
				atol((*re)[2].c_str()),
				atol((*re)[3].c_str()),
				(*re)[4]), z);
		}
		getline();
	}

	// # Mine 1, 30 FUs
	// # Mine 1, 30 FUs: Nichts gefunden.
	// # Mine 1, 30 FUs: Phospor entdeckt.1234 BRT Ertrag.
	set_re("^# Mine ([0-9]+), ([0-9]+) FUs(?:: )?(?:(Nichts) gefunden\\.)?(?:(.*?) entdeckt\\.([0-9]+) BRT Ertrag\\.)?$");
	while(match())
	{
		debug("zone-mine");
		if(re->matches() == 2)
		{
			p->set_minenfeld(new UVMinenfeld(
				atol((*re)[0].c_str()),
				z,
				atol((*re)[1].c_str())), z);
		}
		else if(re->matches() == 3)
		{
			p->set_minenfeld(new UVMinenfeld(
				atol((*re)[0].c_str()),
				z,
				atol((*re)[1].c_str()),
				(*re)[2],
				0), z);
		}
		else
		{
			p->set_minenfeld(new UVMinenfeld(
				atol((*re)[0].c_str()),
				z,
				atol((*re)[1].c_str()),
				(*re)[3],
				atol((*re)[4].c_str())), z);
		}
		getline();
	}

	while(match("^# Werft "))
	{
		p->set_werft(parse_werft(z), z);
	}

	while(match("^# Forschungsstation "))
	{
		p->set_forschungsstation(parse_forschungsstation(z), z);
	}

	while(match("^# Stadt "))
	{
		p->set_stadt(parse_stadt(z), z);
	}

	return z;
}


/*
 * Eine Werft parsen.
 */
UVWerft* UVParserTXT::parse_werft(UVZone* z)
{
	UVWerft* w;

	// # Werft Blah (1), 200 FUs
	// # Werft Blah (1), 200 FUs: Formel: 110000001, Erzlager: 21234
	if(!match("^# Werft (.*) \\(([0-9]+)\\), ([0-9]+) FUs(?:: Formel: ([0-9A-F]{9}), Erzlager: ([0-9]+))?$"))
	{
		throw EXCEPTION("Fehler in Werft!");
	}
	debug("werft");
	w = new UVWerft(atol((*re)[1].c_str()), z);
	w->name = (*re)[0];
	w->groesse = atol((*re)[2].c_str());
	if(re->matches() != 3)
	{
		w->formel = (*re)[3];
		w->erzlager = atol((*re)[4].c_str());
	}
	getline();

	//?  Beschreibung
	if(match("^  ") && !match("^  [0-9]+\\. [^ ]"))
	{
		debug("werft-beschreibung");
		w->beschreibung = strip(cur);
		getline();
	}
	//   1. Standardgenerator, 1000 BRT, 20 HP, 15000 Cr
	//      Energiegenerator, Energie 100, Regeneration 10
	//   13. Pray I, 1000 BRT, 20 HP, 10000 Cr
	//       Bordcomputer
	//   97. Normales Schubtriebwerk, 20000 BRT, 20 HP, 5000 Cr
	//       Triebwerk, Verbrauch 0.0005, Geschwindigkeit 10 KpZ
	while(match("^  "))
	{
		debug("werft-katalog-skip");
		// *** katalog 2x
		// *** w->set_bauteil(...)
		// *** welt->set_bauteil(...)
		getline();
		getline();
	}
	return w;
}


/*
 * Eine Forschungsstation parsen.
 */
UVForschungsstation* UVParserTXT::parse_forschungsstation(UVZone* z)
{
	UVForschungsstation* f;

	// # Forschungsstation Blah (1), 200 FUs
	// # Forschungsstation Blah (1), 200 FUs: 123 Wissenspunkte
	if(!match("^# Forschungsstation (.*) \\(([0-9]+)\\), ([0-9]+) FUs(?:: ([0-9]+) Wissenspunkte)?$"))
	{
		throw EXCEPTION("Fehler in Forschungsstation!");
	}
	debug("forschungsstation");
	f = new UVForschungsstation(atol((*re)[1].c_str()), z);
	f->name = (*re)[0];
	f->groesse = atol((*re)[2].c_str());
	if(re->matches() != 3)
	{
		f->wissenspunkte = atol((*re)[3].c_str());
	}
	getline();

	//?  Beschreibung
	if(match("^  (.*)$"))
	{
		debug("forschungsstation-beschreibung");
		f->beschreibung = (*re)[0];
		getline();
	}

	return f;
}


/*
 * Eine Stadt parsen.
 */
UVStadt* UVParserTXT::parse_stadt(UVZone* z)
{
	UVStadt* s;

	// # Stadt Belleville (1), 74 FUs
	// # Stadt Belleville (1), 74 FUs: 1397599 Seelen
	if(!match("^# Stadt (.*) \\(([0-9]+)\\), ([0-9]+) FUs(?:: ([0-9]+) Seelen)?$"))
	{
		throw EXCEPTION("Fehler in Stadt!");
	}
	debug("stadt");
	s = new UVStadt(atol((*re)[1].c_str()), z);
	s->name = (*re)[0];
	s->groesse = atol((*re)[2].c_str());
	if(re->matches() != 3)
	{
		s->einwohner = atol((*re)[3].c_str());
	}
	getline();

	//?  Beschreibung
	if(match("^  (.*)$"))
	{
		debug("stadt-beschreibung");
		s->beschreibung = (*re)[0];
		getline();
	}

	return s;
}


/*
 * Eine Handelsstation parsen.
 */
void UVParserTXT::parse_handelsstation(UVPlanet* p)
{
	UVHandelsstation* h;

	// § Handelsstation 'Red Blood'
	if(!match("^§ Handelsstation '(.*)'$"))
	{
		throw EXCEPTION("Fehler in Handelsstation!");
	}
	debug("handelsstation");
	h = new UVHandelsstation((*re)[0], p);
	getline();

	//   Beschreibung
	if(match("^  (.*)$"))
	{
		debug("handelsstation-beschreibung");
		h->beschreibung = (*re)[0];
		getline();
	}

	welt->set_handelsstation(h, p);
}


/*
 * Ein Schiff im Sensorreport parsen.
 */
void UVParserTXT::parse_report_schiff()
{

	//   Schiff Foo (Doctor Who) 20000 BRT (-12345,12345,4)
	if(!match("^  Schiff (.*) \\((.*)\\) ([0-9]+) BRT \\((-?[0-9]+),(-?[0-9]+),([0-9]+)\\)$"))
	{
		throw EXCEPTION("Fehler in Schiff!");
	}
	debug("report-schiff");
	UVSchiff* s = new UVSchiff(
		(*re)[0],
		(*re)[1],
		atol((*re)[2].c_str()));
	s->x = atol((*re)[3].c_str());
	s->y = atol((*re)[4].c_str());
	s->dim = atol((*re)[5].c_str());
	getline();

	//     Geschwindigkeit: 54.62 KpZ / Ausrichtung: 123°
	if(match("^    Geschwindigkeit: ([0-9.]+) KpZ / Ausrichtung: ([0-9]+)°$"))
	{
		debug("report-schiff-geschwindigkeit");
		s->v = atof((*re)[0].c_str());
		s->w = atol((*re)[1].c_str());
		getline();
	}

	welt->set_schiff(s);
}


/*
 * Ein Container im Sensorreport parsen.
 */
void UVParserTXT::parse_report_container()
{
	UVContainer* c = new UVContainer();

	//   Container 10 BRT  (-45433,74445,4)
	if(!match("^  Container ([0-9]+) BRT  \\((-?[0-9]+),(-?[0-9]+),([0-9]+)\\)$"))
	{
		throw EXCEPTION("Fehler in Container!");
	}
	debug("report-container");
	c->groesse = atol((*re)[0].c_str());
	c->x = atol((*re)[1].c_str());
	c->y = atol((*re)[2].c_str());
	c->dim = atol((*re)[3].c_str());
	getline();

	welt->set_container(c);
}


/*
 * Eine Anomalie im Sensorreport parsen.
 */
void UVParserTXT::parse_report_anomalie()
{
	UVAnomalie* a = new UVAnomalie();

	//   Kosmische Anomalie mit 6 Lichtjahren Durchmesser (-1316,-26761,1)
	if(!match("^  Kosmische Anomalie mit ([0-9]+) Lichtjahren Durchmesser \\((-?[0-9]+),(-?[0-9]+),([0-9]+)\\)$"))
	{
		throw EXCEPTION("Fehler in Anomalie!");
	}
	debug("report-anomalie");
	a->radius = atol((*re)[0].c_str()) / 2;
	a->x = atol((*re)[1].c_str());
	a->y = atol((*re)[2].c_str());
	a->dim = atol((*re)[3].c_str());
	getline();

	welt->set_anomalie(a);
}


/*
 * Eine Sensorsonde aus dem Sensorenreport parsen.
 */
void UVParserTXT::parse_report_sensorsonde()
{
	UVSensorsonde* s;

	//   SensorSonde 123 (33339,3333,4)
	//   SensorSonde 123 - Lebensdauer: 12 (33339,3333,4)
	if(!match("^  SensorSonde ([0-9]+) (?:- Lebensdauer: ([0-9]+) )?\\((-?[0-9]+),(-?[0-9]+),([0-9]+)\\)$"))
	{
		throw EXCEPTION("Fehler in Sensorsonde!");
	}
	debug("report-sensorsonde");
	s = new UVSensorsonde(atol((*re)[0].c_str()));
	if(re->matches() == 4)
	{
		s->x = atol((*re)[1].c_str());
		s->y = atol((*re)[2].c_str());
		s->dim = atol((*re)[3].c_str());
	}
	else
	{
		s->lebensdauer = atol((*re)[1].c_str());
		s->x = atol((*re)[2].c_str());
		s->y = atol((*re)[3].c_str());
		s->dim = atol((*re)[4].c_str());
	}
	getline();

	welt->set_sensorsonde(s);
}


/*
 * Eine Infosonde aus dem Sensorenreport parsen.
 */
void UVParserTXT::parse_report_infosonde()
{
	UVInfosonde* i;

	//   InfoSonde 123 (33339,3333,4)
	//   InfoSonde 123 - Lebensdauer: 12 (33339,3333,4)
	if(!match("^  InfoSonde ([0-9]+) (?:- Lebensdauer: ([0-9]+) )?\\((-?[0-9]+),(-?[0-9]+),([0-9]+)\\)$"))
	{
		throw EXCEPTION("Fehler in Infosonde!");
	}
	debug("report-infosonde");
	i = new UVInfosonde(atol((*re)[0].c_str()));
	if(re->matches() == 4)
	{
		i->x = atol((*re)[1].c_str());
		i->y = atol((*re)[2].c_str());
		i->dim = atol((*re)[3].c_str());
	}
	else
	{
		i->lebensdauer = atol((*re)[1].c_str());
		i->x = atol((*re)[2].c_str());
		i->y = atol((*re)[3].c_str());
		i->dim = atol((*re)[4].c_str());
	}
	getline();

	welt->set_infosonde(i);
}


/*
 * Eine Leerzeile parsen.
 */
void UVParserTXT::parse_leerzeile()
{
	// 
	if(cur != "")
	{
		throw EXCEPTION("Leerzeile erwartet!");
	}
	getline();
}


/*
 * Findet die Bild-ID zu einer textuellen Klimabeschreibung.
 *
 * Basiert auf setKlima() in Planet.java 1.9 von ToVU (CVS)
 * Copyright (C) 1998-2004, Daniel Straessle <d@niel.ch>
 */
long UVParserTXT::get_image_planet(const string& s) const
{
	if(s.find("Starke St") != string::npos)
	{
		return IMG_PLANET_01;
	}
	else if(s.find("Toxische Atmosph") != string::npos)
	{
		return IMG_PLANET_02;
	}
	else if(s.find("Kraterlandschaft") != string::npos)
	{
		return IMG_PLANET_03;
	}
	else if(s.find("Vulkanisches Klima") != string::npos)
	{
		return IMG_PLANET_04;
	}
	else if(s.find("Arktisches Klima") != string::npos)
	{
		return IMG_PLANET_05;
	}
	else if(s.find("stenklima") != string::npos)
	{
		return IMG_PLANET_06;
	}
	else if(s.find("Wasseroberfl") != string::npos)
	{
		return IMG_PLANET_07;
	}
	else if(s.find("Gebirge vorherrschend") != string::npos)
	{
		return IMG_PLANET_08;
	}
	else if(s.find("ssigtes Klima") != string::npos)
	{
		return IMG_PLANET_09;
	}
	else if(s.find("Tropische Vegetation") != string::npos)
	{
		return IMG_PLANET_10;
	}
	else
	{
		return IMG_PLANET_00;
	}
}


/*
 * Den letztem Regexp-Match zum Debuggen ausgeben.
 */
#ifdef DEBUG
void UVParserTXT::parse_debug(const string& s) const
{
	if(verbose)
	{
		cerr << ">>> " << s;
		for(long i = 0; i < re->matches(); i++)
		{
			cerr << " $" << i << "=" << (*re)[i];
		}
		cerr << endl;
	}
}
#endif


/*
 * Fehlermeldung aufbereiten.
 */
string UVParserTXT::get_exception(const string& text, const string& src_file, const int src_line, const string& src_func) const
{
	return str_stream() << text << " in " << src_func << "() at " << src_file << ":" << src_line << " while processing:\n"
	                    << line << ":" << cur;
}

