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

#ifndef DEBUG_H
#define DEBUG_H

/*
 * Debugging und Exceptions.
 *
 * Dieser Header definiert diverse Dinge im Zusammenhang mit
 * Debugging und Fehlerhandling.
 *
 * DEBUG wird von config.h definiert, falls Debug-Code benutzt
 * werden soll.
 *
 * Dieser Header muss nach config.h und stl.h eingebunden werden.
 */

// *** DEBUG_PRINT

//#define EXCEPTION(x) string(str_stream() << x << " in " << __FUNCTION__ << "() at " << __FILE__ << ":" << __LINE__)
#define EXCEPTION(x) to_string(x) + " in " + to_string(__FUNCTION__) + "() at " + to_string(__FILE__) + ":" + to_string(__LINE__)

#endif // DEBUG_H