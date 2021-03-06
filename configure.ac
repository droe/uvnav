# UV Navigator - Auswertungsvisualisierung fuer Universum V
# Copyright (C) 2004-2006 Daniel Roethlisberger <roe@chronator.ch>
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

AC_PREREQ(2.59)
AC_INIT([UV Navigator], [0.0.4], [uvnav-bugs@projects.roe.ch], [uvnav])
AC_REVISION([[$Rev$]])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([gnu std-options check-news dist-bzip2 dist-zip])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PATH_PROG([SVNVERSION], [svnversion], [], [$PATH:/bin:/usr/bin:/usr/local/bin])

# Checks for libraries.

# Checks for header files.
# AC_HEADER_STDC
AC_CHECK_HEADERS([sys/types.h sys/stat.h sysexits.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CXX_HAVE_HASH_MAP
# AC_CXX_HAVE_HASH_SET
AC_CXX_HAVE_STD_ITERATOR
# AC_CXX_HAVE_BI_ITERATOR
# AC_CXX_HAVE_FWD_ITERATOR

# Checks for library functions.
AC_CHECK_FUNCS([atexit], , [AC_MSG_ERROR([need the atexit function])])
AC_CHECK_FUNCS([eaccess])

# Check for X11
case $host in
	*mingw32)
		X_CFLAGS=""
		X_LIBS=""
		X_EXTRA_LIBS=""
		X_PRE_LIBS=""
		;;
	*)
		AC_PATH_XTRA
		# defines X_CFLAGS X_LIBS X_EXTRA_LIBS X_PRE_LIBS
		X_LIBS="$X_PRE_LIBS $X_LIBS -lX11 $X_EXTRA_LIBS"
		;;
esac
AC_SUBST(X_CFLAGS)
AC_SUBST(X_LIBS)

# Platform specific libraries
case $host in
	*mingw32)
		AC_CHECK_LIB(shlwapi,main,,[AC_MSG_ERROR([libshlwapi missing!])])
		;;
	*)
		;;
esac

# Check for SDL
SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION,
			:,
			[AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])]
)
SAVED_LIBS="$LIBS"
LIBS="$LIBS $SDL_LIBS"
AC_CHECK_LIB(SDL_image,main,,[AC_MSG_ERROR([need SDL_image. See http://www.libsdl.org/])])
AC_CHECK_LIB(SDL_ttf,main,,[AC_MSG_ERROR([need SDL_ttf. See http://www.libsdl.org/])])
AC_CHECK_LIB(SDL_gfx,main,,[AC_MSG_ERROR([need SDL_gfx. See http://www.libsdl.org/])])
LIBS="$SAVED_LIBS"
SDL_LIBS="$SDL_LIBS -lSDL_image -lSDL_ttf -lSDL_gfx"
AC_SUBST(SDL_CFLAGS)
AC_SUBST(SDL_LIBS)

# Static PCRE
PCRE_CFLAGS="-I\$(top_srcdir)/lib/pcre/include"
PCRE_LIBS="\$(top_builddir)/lib/pcre/libpcre.a"
AC_SUBST(PCRE_CFLAGS)
AC_SUBST(PCRE_LIBS)

# Provide lots of optional compiler warnings
CFLAGS_WARNINGS=" -W -Wall -Wbad-function-cast -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wconversion -Wformat -Wimplicit -Wimplicit-function-declaration -Wimplicit-int -Wlong-long -Wmain -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wsign-compare -Wstrict-prototypes -Wswitch -Wtrigraphs -Wunused -Wwrite-strings"
AC_SUBST(CFLAGS_WARNINGS)
CFLAGS_PEDANTIC=" -Waggregate-return -Winline -pedantic -Wformat-nonliteral -Wformat-security -Wformat-extra-args -Wformat=2 -std=gnu99"
AC_SUBST(CFLAGS_PEDANTIC)
CXXFLAGS_WARNINGS=" -W -Wall -Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wconversion -Wformat -Wimplicit -Wlong-long -Wparentheses -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wsign-compare -Wswitch -Wtrigraphs -Wunused -Wwrite-strings"
AC_SUBST(CXXFLAGS_WARNINGS)
CXXFLAGS_PEDANTIC=" -Weffc++ -Waggregate-return -Winline -pedantic -Wformat-nonliteral -Wformat-security -Wformat-extra-args -Wformat=2"
AC_SUBST(CXXFLAGS_PEDANTIC)

# Additional compiler flags
CXXFLAGS="$CXXFLAGS -fno-rtti -Wno-non-virtual-dtor -DDATADIR=\\\"\$(datadir)\\\""
CFLAGS="$CFLAGS -DDATADIR=\\\"\$(datadir)\\\""

# Option: Debug
AC_MSG_CHECKING([whether to use debug code])
AC_ARG_ENABLE([debug],
              AC_HELP_STRING([--enable-debug], [Enable debugging code]),
              [ want_debug="yes" ], [ want_debug="no" ])
if test "x$want_debug" == "xyes" ; then
    CXXFLAGS="$CXXFLAGS -DDEBUG"
    CFLAGS="$CFLAGS -DDEBUG"
fi
AC_MSG_RESULT([$want_debug])

# Option: SDL Surface Locking
AC_MSG_CHECKING([whether to use surface locking])
AC_ARG_ENABLE([locking],
              AC_HELP_STRING([--enable-locking], [Enable SDL surface locking]),
              [ want_locking="yes" ], [ want_locking="no" ])
if test "x$want_locking" == "xyes" ; then
    CXXFLAGS="$CXXFLAGS -DWITH_SURFACE_LOCKING"
    CFLAGS="$CFLAGS -DWITH_SURFACE_LOCKING"
fi
AC_MSG_RESULT([$want_locking])

# Option: Tools
AC_MSG_CHECKING([whether to build developer tools])
AC_ARG_ENABLE([tools],
              AC_HELP_STRING([--enable-tools], [Also install some additional useful tools]),
              [ want_tools="yes" ], [ want_tools="no" ])
AM_CONDITIONAL([COND_TOOLS], [test "$want_tools" = "yes"])
AC_MSG_RESULT([$want_tools])

AC_MSG_CHECKING([which dimension we are building in])
AC_MSG_RESULT([0 - Real Life])

AC_CONFIG_FILES([Makefile
                 util/Makefile
                 share/Makefile
                 lib/Makefile
                 lib/pcre/Makefile
                 lib/pcre/include/Makefile
                 src/Makefile
                 tools/Makefile])
AC_OUTPUT

# The end
if test "`uname -s`" = "Linux" ; then
	make=make
else
	make=gmake
fi
echo "Done. To build and install now, run:"
echo -e "\t$make"
echo -e "\t$make install"
