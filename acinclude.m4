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


# Configure paths for SDL
# Daniel Roethlisberger, 2004-06-11, FreeBSDified
# stolen from Sam Lantinga
# 1999-09-21 stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_SDL([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for SDL, and define SDL_CFLAGS and SDL_LIBS
dnl
AC_DEFUN([AM_PATH_SDL],
[dnl 
dnl Get the cflags and libraries from the sdl-config script
dnl
AC_ARG_WITH(sdl-prefix,[  --with-sdl-prefix=PFX   Prefix where SDL is installed (optional)],
            sdl_prefix="$withval", sdl_prefix="")
AC_ARG_WITH(sdl-exec-prefix,[  --with-sdl-exec-prefix=PFX Exec prefix where SDL is installed (optional)],
            sdl_exec_prefix="$withval", sdl_exec_prefix="")
AC_ARG_ENABLE(sdltest, [  --disable-sdltest       Do not try to compile and run a test SDL program],
		    , enable_sdltest=yes)

  if test x$sdl_exec_prefix != x ; then
     sdl_args="$sdl_args --exec-prefix=$sdl_exec_prefix"
     if test x${SDL_CONFIG+set} != xset ; then
        SDL_CONFIG=$sdl_exec_prefix/bin/sdl-config
     fi
  fi
  if test x$sdl_prefix != x ; then
     sdl_args="$sdl_args --prefix=$sdl_prefix"
     if test x${SDL_CONFIG+set} != xset ; then
        SDL_CONFIG=$sdl_prefix/bin/sdl-config
     fi
  fi

  AC_REQUIRE([AC_CANONICAL_TARGET])
  PATH="$prefix/bin:$prefix/usr/bin:$prefix/usr/local/bin:$PATH"
  AC_PATH_PROG(SDL_CONFIG, sdl-config, no, [$PATH])
  if test "$SDL_CONFIG" = "no" ; then
    AC_PATH_PROG(SDL11_CONFIG, sdl11-config, no, [$PATH])
    SDL_CONFIG="$SDL11_CONFIG"
  fi
  min_sdl_version=ifelse([$1], ,0.11.0,$1)
  AC_MSG_CHECKING(for SDL - version >= $min_sdl_version)
  no_sdl=""
  if test "$SDL_CONFIG" = "no" ; then
    no_sdl=yes
  else
    SDL_CFLAGS=`$SDL_CONFIG $sdlconf_args --cflags`
    SDL_LIBS=`$SDL_CONFIG $sdlconf_args --libs`

    sdl_major_version=`$SDL_CONFIG $sdl_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    sdl_minor_version=`$SDL_CONFIG $sdl_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    sdl_micro_version=`$SDL_CONFIG $sdl_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_sdltest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $SDL_CFLAGS"
      LIBS="$LIBS $SDL_LIBS"
dnl
dnl Now check if the installed SDL is sufficiently new. (Also sanity
dnl checks the results of sdl-config to some extent
dnl
      rm -f conf.sdltest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"

char*
my_strdup (char *str)
{
  char *new_str;
  
  if (str)
    {
      new_str = (char *)malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
  
  return new_str;
}

int main (int argc, char *argv[])
{
  int major, minor, micro;
  char *tmp_version;

  /* This hangs on some systems (?)
  system ("touch conf.sdltest");
  */
  { FILE *fp = fopen("conf.sdltest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_sdl_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_sdl_version");
     exit(1);
   }

   if (($sdl_major_version > major) ||
      (($sdl_major_version == major) && ($sdl_minor_version > minor)) ||
      (($sdl_major_version == major) && ($sdl_minor_version == minor) && ($sdl_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'sdl-config --version' returned %d.%d.%d, but the minimum version\n", $sdl_major_version, $sdl_minor_version, $sdl_micro_version);
      printf("*** of SDL required is %d.%d.%d. If sdl-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If sdl-config was wrong, set the environment variable SDL_CONFIG\n");
      printf("*** to point to the correct copy of sdl-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_sdl=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_sdl" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$SDL_CONFIG" = "no" ; then
       echo "*** The sdl-config script installed by SDL could not be found"
       echo "*** If SDL was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the SDL_CONFIG environment variable to the"
       echo "*** full path to sdl-config."
     else
       if test -f conf.sdltest ; then
        :
       else
          echo "*** Could not run SDL test program, checking why..."
          CFLAGS="$CFLAGS $SDL_CFLAGS"
          LIBS="$LIBS $SDL_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
#include "SDL.h"

int main(int argc, char *argv[])
{ return 0; }
#undef  main
#define main K_and_R_C_main
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding SDL or finding the wrong"
          echo "*** version of SDL. If it is not finding SDL, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means SDL was incorrectly installed"
          echo "*** or that you have moved SDL since it was installed. In the latter case, you"
          echo "*** may want to edit the sdl-config script: $SDL_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     SDL_CFLAGS=""
     SDL_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(SDL_CFLAGS)
  AC_SUBST(SDL_LIBS)
  rm -f conf.sdltest
])


dnl =======================================================================
dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_path_lib.html
dnl
## Table of Contents:
## 1. The main macro
## 2. Core macros
## 3. Integrity checks
## 4. Error reporting
## 5. Feature macros


## ------------------ ##
## 1. The main macro. ##
## ------------------ ##


# AC_PATH_LIB(LIBRARY, MINIMUM-VERSION, HEADER, CONFIG-SCRIPT,
#              MODULES, ACTION-IF-FOUND, ACTION-IF-NOT-FOUND)
# -----------------------------------------------------------
# Check for the presence of libLIBRARY, with a minumum version
# MINIMUM-VERSION.
#
# If needed, use the libconfig script CONFIG-SCRIPT.  If the script
# needs extra modules specifying, pass them as MODULES.
#
# Run ACTION-IF-FOUND if the library is present and all tests pass, or
# ACTION-IF-NOT-FOUND if it is not present or any tests fail.
AC_DEFUN([AC_PATH_LIB],[# check for presence of lib$1
dnl We're going to need uppercase, lowercase and user-friendly
dnl versions of the string `LIBRARY', and long and cache variants.
m4_pushdef([UP], m4_translit([$1], [a-z], [A-Z]))dnl
m4_pushdef([DOWN], m4_translit([$1], [A-Z], [a-z]))dnl
m4_pushdef([LDOWN], ac_path_lib_[]DOWN)dnl
m4_pushdef([CACHE], ac_cv_path_lib_[]DOWN)dnl
m4_pushdef([ERRORLOG], error.[]DOWN[]test)dnl
_AC_PATH_LIB_INIT([$1], [$3], [$4])
_AC_PATH_LIB_CHECK([$1], [$2], [$5])
_AC_PATH_LIB_CHECK_TESTS([$2])
_AC_PATH_LIB_ERROR_DUMP
_AC_PATH_LIB_FINI([$6], [$7])
dnl Pop the macros we defined earlier.
m4_popdef([UP])dnl
m4_popdef([DOWN])dnl
m4_popdef([LDOWN])dnl
m4_popdef([CACHE])dnl
m4_popdef([ERRORLOG])dnl
])# AC_PATH_LIB




## --------------- ##
## 2. Core macros. ##
## --------------- ##


# _AC_PATH_LIB_INIT(LIBRARY, HEADER, CONFIG-SCRIPT)
# -----------------------------------------
# Initialisation of defaults and options.
# Remove error log from previous runs.
AC_DEFUN([_AC_PATH_LIB_INIT],
[_AC_PATH_LIB_DEFAULTS([$1], [$2], [$3])
_AC_PATH_LIB_OPTIONS
rm -f ERRORLOG
# Save variables in case check fails.
ac_save_[]UP[]_CFLAGS="$UP[]_CFLAGS"
ac_save_[]UP[]_LIBS="$UP[]_LIBS"
ac_save_[]UP[]_VERSION="$UP[]_VERSION"
])


# _AC_PATH_LIB_DEFAULTS(LIBRARY, HEADER, CONFIG-SCRIPT)
# -----------------------------------------------------
# Set up default behaviour.
AC_DEFUN([_AC_PATH_LIB_DEFAULTS],
[dnl Set up pkgconfig as default config script.
m4_ifdef([AC_PATH_LIB_USEPKGCONFIG],, [AC_PATH_LIB_PKGCONFIG])
dnl Set default header and config script names.
LDOWN[]_header="m4_default([$2], [$1/$1.h])"
LDOWN[]_config="m4_default([$3], [$1-config])"
ifdef([_AC_PATH_LIB_VERSION_PREFIX],,
      [m4_define([_AC_PATH_LIB_VERSION_PREFIX],
                 DOWN[_])
      ])
ifdef([_AC_PATH_LIB_VERSION_MAJOR],,
      [m4_define([_AC_PATH_LIB_VERSION_MAJOR],
                 [major])
      ])
ifdef([_AC_PATH_LIB_VERSION_MINOR],,
      [m4_define([_AC_PATH_LIB_VERSION_MINOR],
                 [minor])
      ])
ifdef([_AC_PATH_LIB_VERSION_MICRO],,
      [m4_define([_AC_PATH_LIB_VERSION_MICRO],
                 [micro])
      ])
ifdef([_AC_PATH_LIB_VERSION_SUFFIX],,
      [m4_define([_AC_PATH_LIB_VERSION_SUFFIX],
                 [_version])
      ])
ifdef([_AC_PATH_LIB_DEFVERSION_PREFIX],,
      [m4_define([_AC_PATH_LIB_DEFVERSION_PREFIX],
                 UP[_])
      ])
ifdef([_AC_PATH_LIB_DEFVERSION_MAJOR],,
      [m4_define([_AC_PATH_LIB_DEFVERSION_MAJOR],
                 [MAJOR])
      ])
ifdef([_AC_PATH_LIB_DEFVERSION_MINOR],,
      [m4_define([_AC_PATH_LIB_DEFVERSION_MINOR],
                 [MINOR])
      ])
ifdef([_AC_PATH_LIB_DEFVERSION_MICRO],,
      [m4_define([_AC_PATH_LIB_DEFVERSION_MICRO],
                 [MICRO])
      ])
ifdef([_AC_PATH_LIB_DEFVERSION_SUFFIX],,
      [m4_define([_AC_PATH_LIB_DEFVERSION_SUFFIX],
                 [_VERSION])
      ])
])# _AC_PATH_LIB_DEFAULTS


# _AC_PATH_LIB_OPTIONS
# --------------------
# configure options.
AC_DEFUN([_AC_PATH_LIB_OPTIONS],
[m4_if(AC_PATH_LIB_USEPKGCONFIG, [true], ,
[AC_ARG_WITH(DOWN-prefix,
            AC_HELP_STRING([--with-DOWN-prefix=PFX],
                           [prefix where UP is installed (optional)]),
            [LDOWN[]_config_prefix="$withval"],
            [LDOWN[]_config_prefix=""])dnl
AC_ARG_WITH(DOWN-exec-prefix,
            AC_HELP_STRING([--with-DOWN-exec-prefix=PFX],
                           [exec-prefix where UP is installed (optional)]),
            [LDOWN[]_config_exec_prefix="$withval"],
            [LDOWN[]_config_exec_prefix=""])dnl
])
AC_ARG_ENABLE(DOWN[]test,
              AC_HELP_STRING([--disable-DOWN[]test],
                             [do not try to compile and run a test UP program]),
              [LDOWN[]_test_enabled="no"],
              [LDOWN[]_test_enabled="yes"])dnl
])# _AC_PATH_LIB_OPTIONS


# _AC_PATH_LIB_CHECK(LIBRARY, MINIMUM-VERSION, MODULES)
# -----------------------------------------------------
# Obtain library CFLAGS, LIBS and VERSION information.  Cache results,
# but set avoidcache to no if config program is not available.  Break
# up available and minumum version into major, minor and micro version
# numbers.
AC_DEFUN([_AC_PATH_LIB_CHECK],
[
# Set up LIBRARY-config script parameters
m4_if([$3], , ,
      [LDOWN[]_config_args="$LDOWN[]_config_args $3"])
LDOWN[]_min_version=`echo "$2" | sed -e 's/ -nocheck//'`
m4_if([$2], , ,[if test "$LDOWN[]_min_version" = "$2" ; then
                  LDOWN[]_version_test_enabled="yes"
                fi])
m4_if(AC_PATH_LIB_USEPKGCONFIG, [true],
[LDOWN[]_config_args="$1 $LDOWN[]_config_args"
],
[  if test x$LDOWN[]_config_exec_prefix != x ; then
    LDOWN[]_config_args="$LDOWN[]_config_args --exec-prefix=$LDOWN[]_config_exec_prefix"
  fi
  if test x$LDOWN[]_config_prefix != x ; then
    LDOWN[]_config_args="$LDOWN[]_config_args --prefix=$LDOWN[]_config_prefix"
  fi
])
dnl set --version for libconfig or --modversion for pkgconfig
m4_if(AC_PATH_LIB_USEPKGCONFIG, [true],
      [AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
       if test x$PKG_CONFIG != xno ; then
         if pkg-config --atleast-pkgconfig-version 0.7 ; then
           :
         else
           AC_PATH_LIB_ERROR([A new enough version of pkg-config was not found:])
           AC_PATH_LIB_ERROR([version 0.7 or better required.])
           AC_PATH_LIB_ERROR([See http://pkgconfig.sourceforge.net])
           PKG_CONFIG=no
         fi
       fi
       UP[]_CONFIG="$PKG_CONFIG"
       LDOWN[]_config="pkg-config"
       m4_pushdef([LIBCONFIG_CFLAGS], [--cflags])
       m4_pushdef([LIBCONFIG_LIBS], [--libs])
       m4_pushdef([LIBCONFIG_VERSION], [--modversion])
      ],
      [AC_PATH_PROG(UP[]_CONFIG, $LDOWN[]_config, no)
       m4_pushdef([LIBCONFIG_CFLAGS], [--cflags])
       m4_pushdef([LIBCONFIG_LIBS], [--libs])
       m4_pushdef([LIBCONFIG_VERSION], [--version])
       if test x$UP[]_CONFIG = xno ; then
         AC_PATH_LIB_ERROR([The $LDOWN[]_config script installed by UP could not be found.])
         AC_PATH_LIB_ERROR([If UP was installed in PREFIX, make sure PREFIX/bin is in])
         AC_PATH_LIB_ERROR([your path, or set the UP[]_CONFIG environment variable to the])
         AC_PATH_LIB_ERROR([full path to $LDOWN[]_config.])
       fi
      ])

if test x$UP[]_CONFIG = xno ; then
  LDOWN[]_present_avoidcache="no"
else
  LDOWN[]_present_avoidcache="yes"

  AC_CACHE_CHECK([for UP CFLAGS],
                 [CACHE[]_cflags],
                 [CACHE[]_cflags=`$UP[]_CONFIG $LDOWN[]_config_args LIBCONFIG_CFLAGS`])
  AC_CACHE_CHECK([for UP LIBS],
                 [CACHE[]_libs],
                 [CACHE[]_libs=`$UP[]_CONFIG $LDOWN[]_config_args LIBCONFIG_LIBS`])
  AC_CACHE_CHECK([for UP VERSION],
                 [CACHE[]_version],
                 [CACHE[]_version=`$UP[]_CONFIG $LDOWN[]_config_args LIBCONFIG_VERSION`])
  UP[]_CFLAGS="$CACHE[]_cflags"
  UP[]_LIBS="$CACHE[]_libs"
  UP[]_VERSION="$CACHE[]_version"
  LDOWN[]_config_major_version=`echo "$CACHE[]_version" | \
      sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
  LDOWN[]_config_minor_version=`echo "$CACHE[]_version" | \
      sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
  LDOWN[]_config_micro_version=`echo "$CACHE[]_version" | \
      sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
  LDOWN[]_min_major_version=`echo "$LDOWN[]_min_version" | \
      sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
  LDOWN[]_min_minor_version=`echo "$LDOWN[]_min_version" | \
      sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
  LDOWN[]_min_micro_version=`echo "$LDOWN[]_min_version" | \
      sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
fi
m4_popdef([LIBCONFIG_CFLAGS])dnl
m4_popdef([LIBCONFIG_LIBS])dnl
m4_popdef([LIBCONFIG_VERSION])dnl
])# _AC_PATH_LIB_CHECK


# _AC_PATH_LIB_FINI(ACTION-IF-FOUND, ACTION-IF-NOT-FOUND)
# -------------------------------------------------------
# Finish: report errors and define/substitute variables.  Run any
# user-supplied code for success or failure.  Restore defaults.
AC_DEFUN([_AC_PATH_LIB_FINI],
[dnl define variables and run extra code
UP[]_CFLAGS="$CACHE[]_cflags"
UP[]_LIBS="$CACHE[]_libs"
UP[]_VERSION="$CACHE[]_version"
AC_SUBST(UP[]_CFLAGS)dnl
AC_SUBST(UP[]_LIBS)dnl
AC_SUBST(UP[]_VERSION)dnl
# Run code which depends upon the test result.
if test x$CACHE[]_present = xyes ; then
  m4_if([$1], , :, [$1])
else
# Restore saved variables if test fails
  UP[]_CFLAGS="$ac_save_[]UP[]_CFLAGS"
  UP[]_LIBS="$ac_save_[]UP[]_LIBS"
  UP[]_VERSION="$ac_save_[]UP[]_VERSION"
  m4_if([$2], , :, [$2])
fi
dnl Restore defaults
AC_PATH_LIB_CHECK_REGISTER_DEFAULTS
AC_PATH_LIB_PKGCONFIG
])# _AC_PATH_LIB_FINI




## -------------------- ##
## 3. Integrity checks. ##
## -------------------- ##


# _AC_PATH_LIB_CHECK_TESTS(MINIMUM-VERSION)
# -----------------------------------------
# Now check if the installed UP is sufficiently new. (Also sanity
# checks the results of DOWN-config to some extent
AC_DEFUN([_AC_PATH_LIB_CHECK_TESTS],
[AC_CACHE_CHECK([for UP - m4_if([$1], ,
                               [any version],
                               [version >= $LDOWN[]_min_version])],
               [CACHE[]_present],
[CACHE[]_present="$LDOWN[]_present_avoidcache"
if test x$CACHE[]_present = xyes -a x$LDOWN[]_test_enabled = xyes -a \
    x$LDOWN[]_version_test_enabled = xyes ; then
  m4_default([_AC_PATH_LIB_CHECK_TEST_COMPILE],
             [_AC_PATH_LIB_CHECK_TEST_COMPILE],
             [_AC_PATH_LIB_CHECK_TEST_COMPILE_DEFAULT])
else
  m4_default([_AC_PATH_LIB_CHECK_VERSION],
             [_AC_PATH_LIB_CHECK_VERSION],
             [_AC_PATH_LIB_CHECK_VERSION_DEFAULT])
# If the user allowed it, try linking with the library
  if test x$LDOWN[]_test_enabled = xyes ; then
    _AC_PATH_LIB_CHECK_LINK(, [
      CACHE[]_present="no"
      if test x$LDOWN[]_version_test_error = xyes ; then
        AC_PATH_LIB_ERROR
      fi
      AC_PATH_LIB_ERROR([The test program failed to compile or link.  See the file])
      AC_PATH_LIB_ERROR([config.log for the exact error that occured.  This usually])
      AC_PATH_LIB_ERROR([means UP was not installed, was incorrectly installed])
      AC_PATH_LIB_ERROR([or that you have moved UP since it was installed.  In])
      AC_PATH_LIB_ERROR([the latter case, you may want to edit the $LDOWN[]_config])
      AC_PATH_LIB_ERROR([script: $UP[]_CONFIG])
    ])
  fi
fi])
dnl end tests
])# _AC_PATH_LIB_CHECK_TESTS


# _AC_PATH_LIB_CHECK_TEST_COMPILE_DEFAULT
# ---------------------------------------
# Check if the installed UP is sufficiently new. (Also sanity checks
# the results of DOWN-config to some extent.  The test program must
# compile, link and run sucessfully
AC_DEFUN([_AC_PATH_LIB_CHECK_TEST_COMPILE],
[m4_pushdef([RUNLOG], run.[]DOWN[]test)dnl
m4_pushdef([MAJOR], _AC_PATH_LIB_VERSION_PREFIX[]_AC_PATH_LIB_VERSION_MAJOR[]_AC_PATH_LIB_VERSION_SUFFIX)dnl
m4_pushdef([MINOR], _AC_PATH_LIB_VERSION_PREFIX[]_AC_PATH_LIB_VERSION_MINOR[]_AC_PATH_LIB_VERSION_SUFFIX)dnl
m4_pushdef([MICRO], _AC_PATH_LIB_VERSION_PREFIX[]_AC_PATH_LIB_VERSION_MICRO[]_AC_PATH_LIB_VERSION_SUFFIX)dnl
m4_pushdef([DEFMAJOR], _AC_PATH_LIB_DEFVERSION_PREFIX[]_AC_PATH_LIB_DEFVERSION_MAJOR[]_AC_PATH_LIB_DEFVERSION_SUFFIX)dnl
m4_pushdef([DEFMINOR], _AC_PATH_LIB_DEFVERSION_PREFIX[]_AC_PATH_LIB_DEFVERSION_MINOR[]_AC_PATH_LIB_DEFVERSION_SUFFIX)dnl
m4_pushdef([DEFMICRO], _AC_PATH_LIB_DEFVERSION_PREFIX[]_AC_PATH_LIB_DEFVERSION_MICRO[]_AC_PATH_LIB_DEFVERSION_SUFFIX)dnl
  ac_save_CFLAGS="$CFLAGS"
  ac_save_LIBS="$LIBS"
  CFLAGS="$CFLAGS $UP[]_CFLAGS"
  LIBS="$UP[]_LIBS $LIBS"
  rm -f RUNLOG
  AC_TRY_RUN([
#include <$]LDOWN[_header>
#include <stdio.h>
#include <stdlib.h>

/*
 * XXX FIXME Francesco:
 *   This is a pigsty patch for undefined strdup (defined in string.h).
 *   Maybe we should look for strdup() or wrapping it using functions
 *   like malloc && strcpy().
 */
#include <string.h>

int
main ()
{
  int major, minor, micro;
  char *tmp_version;
  FILE *errorlog;

  if ((errorlog = fopen("]ERRORLOG[", "w")) == NULL) {
     exit(1);
   }

  system ("touch ]RUNLOG[");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = strdup("$]LDOWN[_min_version");
  if (!tmp_version) {
     exit(1);
   }
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     fprintf(errorlog, "*** %s: bad version string\n", "$]LDOWN[_min_version");
     exit(1);
   }

  if ((]MAJOR[ != $]LDOWN[_config_major_version) ||
      (]MINOR[ != $]LDOWN[_config_minor_version) ||
      (]MICRO[ != $]LDOWN[_config_micro_version))
    {
      fprintf(errorlog, "*** '$]UP[_CONFIG ]LIBCONFIG_VERSION[' returned %d.%d.%d, but \n", \
        $]LDOWN[_config_major_version, $]LDOWN[_config_minor_version, \
        $]LDOWN[_config_micro_version);
      fprintf(errorlog, "*** ]UP[ (%d.%d.%d) was found!\n", \
        ]MAJOR[, ]MINOR[, ]MICRO[);
      fprintf(errorlog, "***\n");
      fprintf(errorlog, "*** If $]LDOWN[_config was correct, then it is best to remove\n");
      fprintf(errorlog, "*** the old version of ]UP[.  You may also be able to\n");
      fprintf(errorlog, "*** fix the error by modifying your LD_LIBRARY_PATH enviroment\n");
      fprintf(errorlog, "*** variable, or by editing /etc/ld.so.conf.  Make sure you have\n");
      fprintf(errorlog, "*** run ldconfig if that is required on your system.\n");
      fprintf(errorlog, "*** If $]LDOWN[_config was wrong, set the environment\n");
      fprintf(errorlog, "*** variable ]UP[_CONFIG to point to the correct copy of\n");
      fprintf(errorlog, "*** $]LDOWN[_config, and remove the file config.cache\n");
      fprintf(errorlog, "*** before re-running configure.\n");
    }
#if defined (]DEFMAJOR[) && defined (]DEFMINOR[) && defined (]DEFMICRO[)
  else if ((]MAJOR[ != ]DEFMAJOR[) ||
           (]MINOR[ != ]DEFMINOR[) ||
           (]MICRO[ != ]DEFMICRO[))
    {
      fprintf(errorlog, "*** ]UP[ header files (version %d.%d.%d) do not match\n",
             ]DEFMAJOR[, ]DEFMINOR[, ]DEFMICRO[);
      fprintf(errorlog, "*** library (version %d.%d.%d)\n",
             ]MAJOR[, ]MINOR[, ]MICRO[);
    }
#endif /* defined (]DEFMAJOR[) ... */
  else
    {
      if ((]MAJOR[ > major) ||
        ((]MAJOR[ == major) && (]MINOR[  > minor)) ||
        ((]MAJOR[ == major) && (]MINOR[ == minor) && (]MICRO[ >= micro)))
      {
        return 0;
       }
     else
      {
        fprintf(errorlog, "*** An old version of ]UP[ (%d.%d.%d) was found.\n",
               ]MAJOR[, ]MINOR[, ]MICRO[);
        fprintf(errorlog, "*** You need a version of ]UP[ newer than %d.%d.%d.\n",
               major, minor, micro);
        /*fprintf(errorlog, "*** The latest version of ]UP[ is always available from ftp://ftp.my.site\n");*/
        fprintf(errorlog, "***\n");
        fprintf(errorlog, "*** If you have already installed a sufficiently new version, this\n");
        fprintf(errorlog, "*** error probably means that the wrong copy of the $]LDOWN[_config\n");
        fprintf(errorlog, "*** shell script is being found.  The easiest way to fix this is to\n");
        fprintf(errorlog, "*** remove the old version of ]UP[, but you can also set the\n");
        fprintf(errorlog, "*** ]UP[_CONFIG environment variable to point to the correct\n");
        fprintf(errorlog, "*** copy of $]LDOWN[_config.  (In this case, you will have to\n");
        fprintf(errorlog, "*** modify your LD_LIBRARY_PATH environment variable, or edit\n");
        fprintf(errorlog, "*** /etc/ld.so.conf so that the correct libraries are found at\n");
        fprintf(errorlog, "*** run-time.)\n");
      }
    }
  return 1;
}
],, [CACHE[]_present="no"],
    [AC_PATH_LIB_ERROR([cross compiling; assumed OK.])])
  CFLAGS="$ac_save_CFLAGS"
  LIBS="$ac_save_LIBS"

if test -f RUNLOG ; then
  :
elif test x$LDOWN[]_version_test_enabled = xyes ; then
  AC_PATH_LIB_ERROR([Could not run UP test program, checking why...])
  AC_PATH_LIB_ERROR
  _AC_PATH_LIB_CHECK_LINK(dnl
    [AC_PATH_LIB_ERROR([The test program compiled, but did not run.  This usually])
     AC_PATH_LIB_ERROR([means that the run-time linker is not finding UP or])
     AC_PATH_LIB_ERROR([finding the wrong version of UP.  If it is not finding])
     AC_PATH_LIB_ERROR([UP, you will need to set your LD_LIBRARY_PATH environment])
     AC_PATH_LIB_ERROR([variable, or edit /etc/ld.so.conf to point to the installed])
     AC_PATH_LIB_ERROR([location.  Also, make sure you have run ldconfig if that is])
     AC_PATH_LIB_ERROR([required on your system.])
     AC_PATH_LIB_ERROR
     AC_PATH_LIB_ERROR([If you have an old version installed, it is best to remove])
     AC_PATH_LIB_ERROR([it, although you may also be able to get things to work by])
     AC_PATH_LIB_ERROR([modifying LD_LIBRARY_PATH])
    ],
    [AC_PATH_LIB_ERROR([The test program failed to compile or link.  See the file])
     AC_PATH_LIB_ERROR([config.log for the exact error that occured.  This usually])
     AC_PATH_LIB_ERROR([means UP was incorrectly installed or that you have])
     AC_PATH_LIB_ERROR([moved UP since it was installed.  In the latter case,])
     AC_PATH_LIB_ERROR([you may want to edit the $LDOWN[]_config script:])
     AC_PATH_LIB_ERROR([$UP[]_CONFIG])
    ])
fi
rm -f RUNLOG
m4_popdef([RUNLOG])dnl
m4_popdef([MAJOR])dnl
m4_popdef([MINOR])dnl
m4_popdef([MICRO])dnl
m4_popdef([DEFMAJOR])dnl
m4_popdef([DEFMINOR])dnl
m4_popdef([DEFMICRO])dnl
])# _AC_PATH_LIB_CHECK_TEST_COMPILE_DEFAULT


# _AC_PATH_LIB_CHECK_VERSION_DEFAULT
# ----------------------------------
# Check that the library version (config) is greater than or equal to
# the requested (minimum) version.
AC_DEFUN([_AC_PATH_LIB_CHECK_VERSION],
[m4_if([$2], , ,
       [if test x$LDOWN[]_present_avoidcache = xyes ; then
         if test \
             "$LDOWN[]_config_major_version" -lt "$LDOWN[]_min_major_version" -o \
             "$LDOWN[]_config_major_version" -eq "$LDOWN[]_min_major_version" -a \
             "$LDOWN[]_config_minor_version" -lt "$LDOWN[]_min_minor_version" -o \
             "$LDOWN[]_config_major_version" -eq "$LDOWN[]_min_major_version" -a \
             "$LDOWN[]_config_minor_version" -eq "$LDOWN[]_min_minor_version" -a \
             "$LDOWN[]_config_micro_version" -lt "$LDOWN[]_min_micro_version" ; then
           CACHE[]_present="no"
           LDOWN[]_version_test_error="yes"
           AC_PATH_LIB_ERROR([$UP[]_CONFIG --version' returned $LDOWN[]_config_major_version.$LDOWN[]_config_minor_version.$LDOWN[]_config_micro_version, but])
           AC_PATH_LIB_ERROR([UP (>= $LDOWN[]_min_version) was needed.])
           AC_PATH_LIB_ERROR
           AC_PATH_LIB_ERROR([If $]LDOWN[_config was wrong, set the environment])
           AC_PATH_LIB_ERROR([variable ]UP[_CONFIG to point to the correct copy of])
           AC_PATH_LIB_ERROR([$]LDOWN[_config, and remove the file config.cache])
           AC_PATH_LIB_ERROR([before re-running configure.])
         else
           CACHE[]_present="yes"
         fi
       fi])
])# _AC_PATH_LIB_CHECK_VERSION_DEFAULT


# _AC_PATH_LIB_CHECK_LINK_DEFAULT(SUCCESS, FAIL)
# ----------------------------------------------
# Check if the library will link successfully.  If specified, run
# SUCCESS or FAIL on success or failure
AC_DEFUN([_AC_PATH_LIB_CHECK_LINK],
[ac_save_CFLAGS="$CFLAGS"
ac_save_LIBS="$LIBS"
CFLAGS="$CFLAGS $UP[]_CFLAGS"
LIBS="$LIBS $UP[]_LIBS"
AC_TRY_LINK([ #include <stdio.h> ], ,
            [m4_if([$1], , :, [$1])],
            [m4_if([$2], , :, [$2])])
CFLAGS="$ac_save_CFLAGS"
LIBS="$ac_save_LIBS"
])# _AC_PATH_LIB_CHECK_LINK_DEFAULT




## ------------------- ##
## 4. Error reporting. ##
## ------------------- ##


# AC_PATH_LIB_ERROR(MESSAGE)
# --------------------------
# Print an error message, MESSAGE, to the error log.
AC_DEFUN([AC_PATH_LIB_ERROR],
[echo '*** m4_if([$1], , , [$1])' >>ERRORLOG])


# _AC_PATH_LIB_ERROR_DUMP
# -----------------------
# Print the error log (after main AC_CACHE_CHECK completes).
AC_DEFUN([_AC_PATH_LIB_ERROR_DUMP],
[if test -f ERRORLOG ; then
  cat ERRORLOG
  rm -f ERRORLOG
fi])




## ------------------ ##
## 5. Feature macros. ##
## ------------------ ##


# AC_PATH_LIB_PKGCONFIG
# ---------------------
# Enable pkgconfig support in libconfig script (default).
AC_DEFUN([AC_PATH_LIB_PKGCONFIG],
[m4_define([AC_PATH_LIB_USEPKGCONFIG], [true])
])dnl


# AC_PATH_LIB_LIBCONFIG
# ---------------------
# Disable pkgconfig support in libconfig script.
AC_DEFUN([AC_PATH_LIB_LIBCONFIG],
[m4_define([AC_PATH_LIB_USEPKGCONFIG], [false])
])dnl


# AC_PATH_LIB_REGISTER (MACRO, REPLACEMENT)
# -----------------------------------------
# Register a macro to replace the default checks.  Use the REPLACEMENT
# macro for the check macro MACRO.
#
# Possible MACROs are:
#   _AC_PATH_LIB_CHECK_COMPILE and
#   _AC_PATH_LIB_CHECK_VERSION
# You should make sure that replacement macros use the same arguments
# (if any), and do error logging in the same manner and behave in the
# same way as the original.

# Non-default header names may be specified, as well as version
# variable names in the library itself (needed for
# _AC_PATH_LIB_CHECK_COMPILE):
#   _AC_PATH_LIB_HEADER
#   _AC_PATH_LIB_VERSION_PREFIX (default library_)
#   _AC_PATH_LIB_VERSION_MAJOR (default major)
#   _AC_PATH_LIB_VERSION_MINOR (default minor)
#   _AC_PATH_LIB_VERSION_MICRO (default micro)
#   _AC_PATH_LIB_VERSION_SUFFIX (default _version)
#   _AC_PATH_LIB_DEFVERSION_PREFIX (default LIBRARY_)
#   _AC_PATH_LIB_DEFVERSION_MAJOR (default MAJOR)
#   _AC_PATH_LIB_DEFVERSION_MINOR (default MINOR)
#   _AC_PATH_LIB_DEFVERSION_MICRO (default MICRO)
#   _AC_PATH_LIB_DEFVERSION_SUFFIX (default _VERSION)
# For example, library_major_version.
AC_DEFUN([AC_PATH_LIB_REGISTER],
[m4_define([$1], [$2])])


# AC_PATH_LIB_CHECK_REGISTER_DEFAULTS
# -----------------------------------
# Restore the default check macros.
AC_DEFUN([AC_PATH_LIB_CHECK_REGISTER_DEFAULTS],
[_AC_PATH_LIB_CHECK_REGISTER_DEFAULTS([_AC_PATH_LIB_CHECK_COMPILE],
                                       [_AC_PATH_LIB_CHECK_VERSION],
                                       [_AC_PATH_LIB_HEADER],
                                       [_AC_PATH_LIB_VERSION_PREFIX],
                                       [_AC_PATH_LIB_VERSION_MAJOR],
                                       [_AC_PATH_LIB_VERSION_MINOR],
                                       [_AC_PATH_LIB_VERSION_MICRO],
                                       [_AC_PATH_LIB_VERSION_SUFFIX],
                                       [_AC_PATH_LIB_DEFVERSION_PREFIX],
                                       [_AC_PATH_LIB_DEFVERSION_MAJOR],
                                       [_AC_PATH_LIB_DEFVERSION_MINOR],
                                       [_AC_PATH_LIB_DEFVERSION_MICRO],
                                       [_AC_PATH_LIB_DEFVERSION_SUFFIX])
])# AC_PATH_LIB_CHECK_REGISTER_DEFAULTS


# _AC_PATH_LIB_CHECK_REGISTER_DEFAULTS(MACROs ...)
# ------------------------------------------------
# Undefine MACROs.
AC_DEFUN([AC_PATH_LIB_CHECK_REGISTER_DEFAULTS],
[m4_if([$1], , ,
       [m4_ifdef([$1],
                 [m4_undefine([$1])])
        AC_PATH_LIB_CHECK_REGISTER_DEFAULTS(m4_shift($@))
       ])
])





##############################################################################
# Check for C++ namespace support.  This is from
# http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_namespaces.html
#
AC_DEFUN([AC_CXX_NAMESPACES],
[AC_CACHE_CHECK(whether the compiler implements namespaces,
ac_cv_cxx_namespaces,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([namespace Outer { namespace Inner { int i = 0; }}],
                [using namespace Outer::Inner; return i;],
 ac_cv_cxx_namespaces=yes, ac_cv_cxx_namespaces=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_namespaces" = yes; then
  AC_DEFINE(HAVE_NAMESPACES,,[define if the compiler implements namespaces])
fi
])


##############################################################################
# Check for hash_map extension.  This is from
# http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_have_ext_hash_map.html
AC_DEFUN([AC_CXX_HAVE_STD_EXT_HASH_MAP],
[AC_CACHE_CHECK([whether the compiler has <ext/hash_map> defining template class std::hash_map],
 ac_cv_cxx_have_std_ext_hash_map,
 [AC_REQUIRE([AC_CXX_NAMESPACES])
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([#include <ext/hash_map>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[hash_map<int, int> t;],
  [ac_cv_cxx_have_std_ext_hash_map=yes], [ac_cv_cxx_have_std_ext_hash_map=no])
  AC_LANG_RESTORE])
 HAVE_STD_EXT_HASH_MAP=0
 if test "$ac_cv_cxx_have_std_ext_hash_map" = yes
 then
   HAVE_STD_EXT_HASH_MAP=1
   AC_DEFINE(HAVE_STD_EXT_HASH_MAP, , [define if the compiler has <ext/hash_map> defining template class std::hash_map])
 fi
 AC_SUBST(HAVE_STD_EXT_HASH_MAP)])

AC_DEFUN([AC_CXX_HAVE_GNU_EXT_HASH_MAP],
[AC_CACHE_CHECK([whether the compiler has <ext/hash_map> defining template class __gnu_cxx::hash_map],
 ac_cv_cxx_have_gnu_ext_hash_map,
 [AC_REQUIRE([AC_CXX_NAMESPACES])
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([#include <ext/hash_map>
#ifdef HAVE_NAMESPACES
using namespace __gnu_cxx;
#endif],[hash_map<int,int> t; ],
  [ac_cv_cxx_have_gnu_ext_hash_map=yes],[ac_cv_cxx_have_gnu_ext_hash_map=no])
  AC_LANG_RESTORE])
 HAVE_GNU_EXT_HASH_MAP=0
 if test "$ac_cv_cxx_have_gnu_ext_hash_map" = yes
 then
   HAVE_GNU_EXT_HASH_MAP=1
   AC_DEFINE(HAVE_GNU_EXT_HASH_MAP, , [define if the compiler has <ext/hash_map> defining template class __gnu_cxx::hash_map])
 fi
 AC_SUBST(HAVE_GNU_EXT_HASH_MAP)])

AC_DEFUN([AC_CXX_HAVE_GLOBAL_HASH_MAP],
[AC_CACHE_CHECK([whether the compiler has <hash_map> defining template class ::hash_map],
 ac_cv_cxx_have_global_hash_map,
 [AC_REQUIRE([AC_CXX_NAMESPACES])
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([#include <hash_map>],[hash_map<int,int> t; ],
  [ac_cv_cxx_have_global_hash_map=yes], [ac_cv_cxx_have_global_hash_map=no])
  AC_LANG_RESTORE])
 HAVE_GLOBAL_HASH_MAP=0
 if test "$ac_cv_cxx_have_global_hash_map" = yes
 then
   HAVE_GLOBAL_HASH_MAP=1
   AC_DEFINE(HAVE_GLOBAL_HASH_MAP, , [define if the compiler has <hash_map> defining template class ::hash_map])
 fi
 AC_SUBST(HAVE_GLOBAL_HASH_MAP)])

AC_DEFUN([AC_CXX_HAVE_HASH_MAP],
[AC_CXX_HAVE_STD_EXT_HASH_MAP
 AC_CXX_HAVE_GNU_EXT_HASH_MAP
 AC_CXX_HAVE_GLOBAL_HASH_MAP])


##############################################################################
# Check for hash_set extension.  This is modified from
# http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_have_ext_hash_set.html
AC_DEFUN([AC_CXX_HAVE_STD_EXT_HASH_SET],
[AC_CACHE_CHECK([whether the compiler has <ext/hash_set> defining template class std::hash_set],
 ac_cv_cxx_have_std_ext_hash_set,
 [AC_REQUIRE([AC_CXX_NAMESPACES])
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([#include <ext/hash_set>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[hash_set<int> t; ],
  [ac_cv_cxx_have_std_ext_hash_set=yes], [ac_cv_cxx_have_std_ext_hash_set=no])
  AC_LANG_RESTORE])
 HAVE_STD_EXT_HASH_SET=0
 if test "$ac_cv_cxx_have_std_ext_hash_set" = yes
 then
   HAVE_STD_EXT_HASH_SET=1
   AC_DEFINE(HAVE_STD_EXT_HASH_SET, , [define if the compiler has <ext/hash_set> defining template class std::hash_set])
 fi
 AC_SUBST(HAVE_STD_EXT_HASH_SET)])

AC_DEFUN([AC_CXX_HAVE_GNU_EXT_HASH_SET],
[AC_CACHE_CHECK(
 [whether the compiler has <ext/hash_set> defining template class __gnu_cxx::hash_set],
 ac_cv_cxx_have_gnu_ext_hash_set,
 [AC_REQUIRE([AC_CXX_NAMESPACES])
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([#include <ext/hash_set>
#ifdef HAVE_NAMESPACES
using namespace __gnu_cxx;
#endif],[hash_set<int> t; ],
  [ac_cv_cxx_have_gnu_ext_hash_set=yes], [ac_cv_cxx_have_gnu_ext_hash_set=no])
  AC_LANG_RESTORE])
 HAVE_GNU_EXT_HASH_SET=0
 if test "$ac_cv_cxx_have_gnu_ext_hash_set" = yes
 then
   HAVE_GNU_EXT_HASH_SET=1
   AC_DEFINE(HAVE_GNU_EXT_HASH_SET, , [define if the compiler has <ext/hash_set> defining template class __gnu_cxx::hash_set])
 fi
 AC_SUBST(HAVE_GNU_EXT_HASH_SET)])

AC_DEFUN([AC_CXX_HAVE_GLOBAL_HASH_SET],
[AC_CACHE_CHECK([whether the compiler has <hash_set> defining template class ::hash_set],
 ac_cv_cxx_have_global_hash_set,
 [AC_REQUIRE([AC_CXX_NAMESPACES])
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([#include <hash_set>],[hash_set<int> t; return 0;],
  [ac_cv_cxx_have_global_hash_set=yes], [ac_cv_cxx_have_global_hash_set=no])
  AC_LANG_RESTORE])
 HAVE_GLOBAL_HASH_SET=0
 if test "$ac_cv_cxx_have_global_hash_set" = yes
 then
   HAVE_GLOBAL_HASH_SET=1
   AC_DEFINE(HAVE_GLOBAL_HASH_SET, , [define if the compiler has <hash_set> defining template class ::hash_set])
 fi
 AC_SUBST(HAVE_GLOBAL_HASH_SET)])

AC_DEFUN([AC_CXX_HAVE_HASH_SET],
[AC_CXX_HAVE_STD_EXT_HASH_SET
 AC_CXX_HAVE_GNU_EXT_HASH_SET
 AC_CXX_HAVE_GLOBAL_HASH_SET])


##############################################################################
# Check for standard iterator extension.  This is modified from
# http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_have_ext_hash_set.html
AC_DEFUN([AC_CXX_HAVE_STD_ITERATOR],
[AC_CACHE_CHECK(whether the compiler has the standard iterator,
ac_cv_cxx_have_std_iterator,
[AC_REQUIRE([AC_CXX_NAMESPACES])
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([#include <iterator>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[iterator<int,int,int> t; return 0;],
  ac_cv_cxx_have_std_iterator=yes, ac_cv_cxx_have_std_iterator=no)
  AC_LANG_RESTORE
])
HAVE_STD_ITERATOR=0
if test "$ac_cv_cxx_have_std_iterator" = yes
then
   HAVE_STD_ITERATOR=1
   AC_DEFINE(HAVE_STD_ITERATOR, , [define if the compiler has the standard iterator])
fi
AC_SUBST(HAVE_STD_ITERATOR)])


##############################################################################
# Check for bidirectional iterator extension.  This is modified from
# http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_have_ext_hash_set.html
#
AC_DEFUN([AC_CXX_HAVE_BI_ITERATOR],
[AC_CACHE_CHECK(whether the compiler has the bidirectional iterator,
ac_cv_cxx_have_bi_iterator,
[AC_REQUIRE([AC_CXX_NAMESPACES])
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([#include <iterator>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[bidirectional_iterator<int,int> t; return 0;],
  ac_cv_cxx_have_bi_iterator=yes, ac_cv_cxx_have_bi_iterator=no)
  AC_LANG_RESTORE
])
HAVE_BI_ITERATOR=0
if test "$ac_cv_cxx_have_bi_iterator" = yes
then
   HAVE_BI_ITERATOR=1
   AC_DEFINE(HAVE_BI_ITERATOR, , [define if the compiler has the bidirectional iterator])
fi
AC_SUBST(HAVE_BI_ITERATOR)])


##############################################################################
# Check for forward iterator extension.  This is modified from
# http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_have_ext_hash_set.html
AC_DEFUN([AC_CXX_HAVE_FWD_ITERATOR],
[AC_CACHE_CHECK(whether the compiler has forward iterators,
ac_cv_cxx_have_fwd_iterator,
[AC_REQUIRE([AC_CXX_NAMESPACES])
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([#include <iterator>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[forward_iterator<int,int> t; return 0;],
  ac_cv_cxx_have_fwd_iterator=yes, ac_cv_cxx_have_fwd_iterator=no)
  AC_LANG_RESTORE
])
HAVE_FWD_ITERATOR=0
if test "$ac_cv_cxx_have_fwd_iterator" = yes
then
   HAVE_FWD_ITERATOR=1
   AC_DEFINE(HAVE_FWD_ITERATOR, , [define if the compiler has forward iterators])
fi
AC_SUBST(HAVE_FWD_ITERATOR)])
