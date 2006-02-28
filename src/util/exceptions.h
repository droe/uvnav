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

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

/*
 * Exceptions.
 *
 * std::exception
 *   std::logic_error
 *     std::domain_error
 *     std::invalid_argument
 *     std::length_error
 *     std::out_of_range
 *   std::runtime_error
 *     std::range_error
 *     std::overflow_error
 *     std::underflow_error
 *     quit_application
 *     key_not_found_error
 */

// TODO: eigene Exceptions von exception ableiten.  Benötigt Implemenation.

#include <exception>
#include <stdexcept>
#include <string>

class quit_application : public std::runtime_error {
public:
	explicit quit_application(const std::string & emsg = "")
	: std::runtime_error(emsg) { }
};

class key_not_found_error : public std::runtime_error {
public:
	explicit key_not_found_error(const std::string & emsg = "")
	: std::runtime_error(emsg) { }
};

#define EXCEPTION(x) to_string(x) + " in " + to_string(__FUNCTION__) + "() at " + to_string(__FILE__) + ":" + to_string(__LINE__)

#endif // EXCEPTIONS_H
