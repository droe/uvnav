# UV Navigator - Auswertungsvisualisierung fuer Universum V
# Copyright (C) 2004-2005 Daniel Roethlisberger <roe@chronator.ch>
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
