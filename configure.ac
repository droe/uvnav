# UV Navigator - Auswertungsvisualisierung fuer Universum V
# Copyright (C) 2004 Daniel Roethlisberger <roe@chronator.ch>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see http://www.gnu.org/copyleft/
#
# $Id$

AC_PREREQ(2.57)
AC_INIT([UV Navigator], [0.0.0], [uvnav-bugs@projects.roe.ch], [uvnav])
AC_REVISION([[$Rev$]])
AC_CANONICAL_TARGET
# sobald ChangeLog verfuegbar hier von foreign auf gnu umstellen
AM_INIT_AUTOMAKE([foreign std-options check-news dist-bzip2 dist-zip])
AC_CONFIG_SRCDIR([src/global.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PATH_PROG([SVNVERSION], [svnversion], [], [$PATH:/bin:/usr/bin:/usr/local/bin])

# Checks for libraries.

# Checks for header files.
# AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_CXX_HAVE_HASH_MAP
# AC_CXX_HAVE_HASH_SET
AC_CXX_HAVE_STD_ITERATOR
# AC_CXX_HAVE_BI_ITERATOR
# AC_CXX_HAVE_FWD_ITERATOR

# Checks for library functions.
AC_CHECK_FUNCS([atexit])

# Check for SDL
SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION,
			:,
			[AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])]
)
AC_SUBST(SDL_CFLAGS)
SDL_LIBS="$SDL_LIBS -lSDL_image -lSDL_ttf -lSDL_gfx"
AC_SUBST(SDL_LIBS)

# Check for PCRE++
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_PATH_LIB_LIBCONFIG
AC_PATH_LIB([pcrepp], , [pcre++.h], [pcre++-config])
AC_LANG_RESTORE
AC_SUBST(PCREPP_CFLAGS)
AC_SUBST(PCREPP_LIBS)

# Enable lots of compiler warnings
CFLAGS_WARNINGS=" -W -Wall -Wbad-function-cast -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wconversion -Wformat -Wimplicit -Wimplicit-function-declaration -Wimplicit-int -Wlong-long -Wmain -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wno-import -Wno-parentheses -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wsign-compare -Wstrict-prototypes -Wswitch -Wtrigraphs -Wunused -Wwrite-strings"
CFLAGS="$CFLAGS $CFLAGS_WARNINGS"
#CFLAGS_PEDANTIC=" -Waggregate-return -Winline -pedantic -Wformat-nonliteral -Wformat-security -Wformat-extra-args -Wformat=2 -std=gnu99"
#CFLAGS="$CFLAGS $CFLAGS_PEDANTIC"
CXXFLAGS_WARNINGS=" -W -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wconversion -Wformat -Wimplicit -Wlong-long -Wmissing-prototypes -Wno-import -Wno-parentheses -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wsign-compare -Wstrict-prototypes -Wswitch -Wtrigraphs -Wunused -Wwrite-strings"
CXXFLAGS="$CXXFLAGS $CXXFLAGS_WARNINGS"
#CXXFLAGS_PEDANTIC=" -Waggregate-return -Winline -pedantic -Wformat-nonliteral -Wformat-security -Wformat-extra-args -Wformat=2"
#CXXFLAGS="$CXXFLAGS $CXXFLAGS_PEDANTIC"

# Additional compiler flags
CXXFLAGS="$CXXFLAGS -fno-rtti -DDATADIR=\\\"\$(datadir)\\\""
CFLAGS="$CFLAGS -DDATADIR=\\\"\$(datadir)\\\""

# Options
AC_ARG_ENABLE([tools],
              AC_HELP_STRING([--enable-tools], [Also build (but not install) useful tools for developers or porters]),
              [ want_tools="yes" ])
AM_CONDITIONAL([COND_TOOLS], [test "$want_tools" = "yes"])

AC_CONFIG_FILES([Makefile
                 tools/Makefile
                 src/Makefile
                 share/Makefile])
AC_OUTPUT
