#include <stdio.h>

int main(int argc, char* argv[])
{
 int c;
 char** v;
 v = argv;
 c = argc;

#ifdef WIN32
 puts("WIN32");
#endif

#ifdef AIX
 puts("AIX");
#endif
#ifdef AZTEC
 puts("AZTEC");
#endif
#ifdef BORLANDC
 puts("BORLANDC");
#endif
#ifdef EOF
 puts("EOF");
#endif
#ifdef EXIT_FAILURE
 puts("EXIT_FAILURE");
#endif
#ifdef EXIT_SUCCESS
 puts("EXIT_SUCCESS");
#endif
#ifdef FALSE
 puts("FALSE");
#endif
#ifdef HIGH_C
 puts("HIGH_C");
#endif
#ifdef HPUX
 puts("HPUX");
#endif
#ifdef HYPERC
 puts("HYPERC");
#endif
#ifdef LANG
 puts("LANG");
#endif
#ifdef LC_CTYPE
 puts("LC_CTYPE");
#endif
#ifdef MIPSEL
 puts("MIPSEL");
#endif
#ifdef MSDOS
 puts("MSDOS");
#endif
#ifdef M_BITFIELDS
 puts("M_BITFIELDS");
#endif
#ifdef M_COFF
 puts("M_COFF");
#endif
#ifdef M_I186
 puts("M_I186");
#endif
#ifdef M_I286
 puts("M_I286");
#endif
#ifdef M_I386
 puts("M_I386");
#endif
#ifdef M_I8086
 puts("M_I8086");
#endif
#ifdef M_I86
 puts("M_I86");
#endif
#ifdef M_I86CM
 puts("M_I86CM");
#endif
#ifdef M_I86HM
 puts("M_I86HM");
#endif
#ifdef M_I86LM
 puts("M_I86LM");
#endif
#ifdef M_I86MM
 puts("M_I86MM");
#endif
#ifdef M_I86SM
 puts("M_I86SM");
#endif
#ifdef M_INTERNAT
 puts("M_INTERNAT");
#endif
#ifdef M_LDATA
 puts("M_LDATA");
#endif
#ifdef M_LTEXT
 puts("M_LTEXT");
#endif
#ifdef M_SDATA
 puts("M_SDATA");
#endif
#ifdef M_STEXT
 puts("M_STEXT");
#endif
#ifdef M_SYS3
 puts("M_SYS3");
#endif
#ifdef M_SYS5
 puts("M_SYS5");
#endif
#ifdef M_SYSIII
 puts("M_SYSIII");
#endif
#ifdef M_SYSV
 puts("M_SYSV");
#endif
#ifdef M_UNIX
 puts("M_UNIX");
#endif
#ifdef M_WORDSWAP
 puts("M_WORDSWAP");
#endif
#ifdef M_XENIX
 puts("M_XENIX");
#endif
#ifdef M_XOUT
 puts("M_XOUT");
#endif
#ifdef NEVER
 puts("NEVER");
#endif
#ifdef NOTDEF
 puts("NOTDEF");
#endif
#ifdef NULL
 puts("NULL");
#endif
#ifdef OS2
 puts("OS2");
#endif
#ifdef POSIX
 puts("POSIX");
#endif
#ifdef PWB
 puts("PWB");
#endif
#ifdef RES
 puts("RES");
#endif
#ifdef RPC_CLNT
 puts("RPC_CLNT");
#endif
#ifdef RPC_HDR
 puts("RPC_HDR");
#endif
#ifdef RPC_SVC
 puts("RPC_SVC");
#endif
#ifdef RPC_XDR
 puts("RPC_XDR");
#endif
#ifdef RT
 puts("RT");
#endif
#ifdef SCO
 puts("SCO");
#endif
#ifdef STDC
 puts("STDC");
#endif
#ifdef TRUE
 puts("TRUE");
#endif
#ifdef UNIX
 puts("UNIX");
#endif
#ifdef X_NOT_POSIX
 puts("X_NOT_POSIX");
#endif
#ifdef X_NOT_STDC_ENV
 puts("X_NOT_STDC_ENV");
#endif
#ifdef _AIX
 puts("_AIX");
#endif
#ifdef _ALL_SOURCE
 puts("_ALL_SOURCE");
#endif
#ifdef _ANSI_C_SOURCE
 puts("_ANSI_C_SOURCE");
#endif
#ifdef _D_NAME_MAX
 puts("_D_NAME_MAX");
#endif
#ifdef _HPUX_SOURCE
 puts("_HPUX_SOURCE");
#endif
#ifdef _KR
 puts("_KR");
#endif
#ifdef _MSC_VER
 puts("_MSC_VER");
#endif
#ifdef _M_BITFIELDS
 puts("_M_BITFIELDS");
#endif
#ifdef _M_COFF
 puts("_M_COFF");
#endif
#ifdef _M_I186
 puts("_M_I186");
#endif
#ifdef _M_I286
 puts("_M_I286");
#endif
#ifdef _M_I386
 puts("_M_I386");
#endif
#ifdef _M_I8086
 puts("_M_I8086");
#endif
#ifdef _M_I86
 puts("_M_I86");
#endif
#ifdef _M_I86CM
 puts("_M_I86CM");
#endif
#ifdef _M_I86HM
 puts("_M_I86HM");
#endif
#ifdef _M_I86LM
 puts("_M_I86LM");
#endif
#ifdef _M_I86MM
 puts("_M_I86MM");
#endif
#ifdef _M_I86SM
 puts("_M_I86SM");
#endif
#ifdef _M_INTERNAT
 puts("_M_INTERNAT");
#endif
#ifdef _M_LDATA
 puts("_M_LDATA");
#endif
#ifdef _M_LTEXT
 puts("_M_LTEXT");
#endif
#ifdef _M_SDATA
 puts("_M_SDATA");
#endif
#ifdef _M_STEXT
 puts("_M_STEXT");
#endif
#ifdef _M_SYS3
 puts("_M_SYS3");
#endif
#ifdef _M_SYS5
 puts("_M_SYS5");
#endif
#ifdef _M_SYSIII
 puts("_M_SYSIII");
#endif
#ifdef _M_SYSV
 puts("_M_SYSV");
#endif
#ifdef _M_UNIX
 puts("_M_UNIX");
#endif
#ifdef _M_WORDSWAP
 puts("_M_WORDSWAP");
#endif
#ifdef _M_XENIX
 puts("_M_XENIX");
#endif
#ifdef _PA_RISC1_0
 puts("_PA_RISC1_0");
#endif
#ifdef _PA_RISC1_1
 puts("_PA_RISC1_1");
#endif
#ifdef _POSIX_SOURCE
 puts("_POSIX_SOURCE");
#endif
#ifdef _PWB
 puts("_PWB");
#endif
#ifdef _SIO
 puts("_SIO");
#endif
#ifdef _STRICT_NAMES
 puts("_STRICT_NAMES");
#endif
#ifdef _SVID
 puts("_SVID");
#endif
#ifdef _WSIO
 puts("_WSIO");
#endif
#ifdef _XOPEN_SOURCE
 puts("_XOPEN_SOURCE");
#endif
#ifdef __AIX
 puts("__AIX");
#endif
#ifdef __AIX__
 puts("__AIX__");
#endif
#ifdef __ANSI__
 puts("__ANSI__");
#endif
#ifdef __BCPLUSPLUS__
 puts("__BCPLUSPLUS__");
#endif
#ifdef __BORLANDCPP__
 puts("__BORLANDCPP__");
#endif
#ifdef __BORLANDC__
 puts("__BORLANDC__");
#endif
#ifdef __EXTENDED__
 puts("__EXTENDED__");
#endif
#ifdef __GNUC__
 puts("__GNUC__");
#endif
#ifdef __MATH__
 puts("__MATH__");
#endif
#ifdef __MSDOS__
 puts("__MSDOS__");
#endif
#ifdef __POSIX__
 puts("__POSIX__");
#endif
#ifdef __SAA_L2__
 puts("__SAA_L2__");
#endif
#ifdef __SAA__
 puts("__SAA__");
#endif
#ifdef __STDC__
 puts("__STDC__");
#endif
#ifdef __STRICT_ANSI__
 puts("__STRICT_ANSI__");
#endif
#ifdef __STR__
 puts("__STR__");
#endif
#ifdef __SVR4
 puts("__SVR4");
#endif
#ifdef __TCPLUSPLUS__
 puts("__TCPLUSPLUS__");
#endif
#ifdef __TIMESTAMP__
 puts("__TIMESTAMP__");
#endif
#ifdef __TURBOC__
 puts("__TURBOC__");
#endif
#ifdef __aix
 puts("__aix");
#endif
#ifdef __bsdi__
 puts("__bsdi__");
#endif
#ifdef __cplusplus
 puts("__cplusplus");
#endif
#ifdef __hp9000s300
 puts("__hp9000s300");
#endif
#ifdef __hp9000s700
 puts("__hp9000s700");
#endif
#ifdef __hp9000s800
 puts("__hp9000s800");
#endif
#ifdef __hppa
 puts("__hppa");
#endif
#ifdef __hpux
 puts("__hpux");
#endif
#ifdef __i386
 puts("__i386");
#endif
#ifdef __i386__
 puts("__i386__");
#endif
#ifdef __lint
 puts("__lint");
#endif
#ifdef __unix
 puts("__unix");
#endif
#ifdef __unix__
 puts("__unix__");
#endif
#ifdef __vax__
 puts("__vax__");
#endif
#ifdef _aix
 puts("_aix");
#endif
#ifdef aix
 puts("aix");
#endif
#ifdef bsd4_2
 puts("bsd4_2");
#endif
#ifdef bsdi
 puts("bsdi");
#endif
#ifdef false
 puts("false");
#endif
#ifdef gcos
 puts("gcos");
#endif
#ifdef host_mips
 puts("host_mips");
#endif
#ifdef hp9000ipc
 puts("hp9000ipc");
#endif
#ifdef hp9000s200
 puts("hp9000s200");
#endif
#ifdef hp9000s300
 puts("hp9000s300");
#endif
#ifdef hp9000s500
 puts("hp9000s500");
#endif
#ifdef hp9000s700
 puts("hp9000s700");
#endif
#ifdef hp9000s800
 puts("hp9000s800");
#endif
#ifdef hppa
 puts("hppa");
#endif
#ifdef hpux
 puts("hpux");
#endif
#ifdef i386
 puts("i386");
#endif
#ifdef iAPX286
 puts("iAPX286");
#endif
#ifdef ibm
 puts("ibm");
#endif
#ifdef interdata
 puts("interdata");
#endif
#ifdef lint
 puts("lint");
#endif
#ifdef mc68000
 puts("mc68000");
#endif
#ifdef mc68010
 puts("mc68010");
#endif
#ifdef mc68020
 puts("mc68020");
#endif
#ifdef mips
 puts("mips");
#endif
#ifdef notdef
 puts("notdef");
#endif
#ifdef ns32000
 puts("ns32000");
#endif
#ifdef os
 puts("os");
#endif
#ifdef pdp11
 puts("pdp11");
#endif
#ifdef posix
 puts("posix");
#endif
#ifdef sparc
 puts("sparc");
#endif
#ifdef sun
 puts("sun");
#endif
#ifdef true
 puts("true");
#endif
#ifdef tss
 puts("tss");
#endif
#ifdef u370
 puts("u370");
#endif
#ifdef u3b
 puts("u3b");
#endif
#ifdef u3b15
 puts("u3b15");
#endif
#ifdef u3b2
 puts("u3b2");
#endif
#ifdef u3b20d
 puts("u3b20d");
#endif
#ifdef u3b5
 puts("u3b5");
#endif
#ifdef ultrix
 puts("ultrix");
#endif
#ifdef unix
 puts("unix");
#endif
#ifdef vax
 puts("vax");
#endif
 return 0;
}
