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

#include "rev.h"

/*
 * Subversion Revision Hack.
 *
 * rev.h wird von make (Code in src/Makefile.am) erzeugt / aktualisiert.
 * Damit eine neue Revision keine langen Kompilierungsvorgaenge ausloest,
 * wird die aktuelle Revision hier ueber eine globale Variable allen
 * anderen Dateien zu Verfuegung gestellt.  Aendert die Revision, muss
 * nur rev.cpp neu kompiliert, und anschliessend gelinkt werden.
 * Um diesen Vorgang zu beschleunigen, werden hier keinerlei Header
 * eingebunden.
 */

const char* revision = REVISION;

