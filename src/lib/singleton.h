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

#ifndef SINGLETON_H
#define SINGLETON_H

/*
 * Singleton-Template.  Stellt sicher, dass es von der betreffenden
 * Klasse nur eine Instanz gibt.
 *
 * Verwendung:
 * class FooBar : public Singleton<FooBar>
 * {
 *     friend Singleton<FooBar>;
 *     private:
 *         FooBar();
 *     // ...
 * }
 */

template <class T>
class Singleton
{
	public:
		static T* get_instance();

	protected:
		Singleton() {}

	private:
		Singleton( const Singleton& );
		Singleton& operator=(Singleton const&);
};

template <class T>
T* Singleton<T>::get_instance()
{
	static T instance;
	return &instance;
}

#endif // SINGLETON_H
