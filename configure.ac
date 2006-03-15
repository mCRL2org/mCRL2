AC_INIT([mCRL2 toolset], [0.2], [bug@mcrl2.org])
AC_CANONICAL_HOST
AC_PREFIX_DEFAULT([$HOME])
AC_CONFIG_SRCDIR([src/liblowlevel.h])

# Default toolset to use with Boost.Build
BJAM_TOOLSET="gcc"

# Setting host OS
case $host in
  *-*-mingw32*)
    HOST_OS="MINGW"
    ;;
  *-*-cygwin*)
    HOST_OS="CYGWIN"
    ;;
  *-*-msdos* | *-*-go32* | *-*-windows*)
    HOST_OS="WINDOWS"
    BJAM_TOOLSET="vc-7_1"
    ;;
  *-*-linux*)
    HOST_OS="LINUX"
    ;;
  *-*-*bsd*)
    HOST_OS="BSD"
    ;;
  *-*-darwin*)
    HOST_OS="MACOSX"
    BJAM_TOOLSET="darwin"
    ;;
  *)
    HOST_OS="UNKNOWN"
    ;;
esac
AC_SUBST([BJAM_TOOLSET])
AC_SUBST([HOST_OS])

# Default is 32 bits, test is probably very incomplete
case $host in
  *64*-*-*)
    USE_64_BIT=1
    ;;
esac

# Checks for programs.
AC_PROG_CPP
AC_PROG_CC
AC_PROG_CXX

# Check for flex
AC_CHECK_PROG([FLEX], [flex], [flex], [])
if test -z $FLEX; then
  AC_MSG_ERROR([cannot find flex])
fi

# Check for compatible bison
AC_CHECK_PROG([BISON], [bison], [bison], [])
if test -z $BISON; then
  AC_MSG_ERROR([cannot find bison])
fi
AC_MSG_CHECKING([whether bison option %glr-parser is available])
if $BISON test.y -o/dev/null &> /dev/null; then
  AC_MSG_RESULT([yes]);
else
  AC_MSG_RESULT([no])
  AC_MSG_ERROR([bison option %glr-parser not available]);
fi
AC_MSG_CHECKING([whether bison GLR parser can be used])
$BISON test.y -o test.c
if $CC -c test.c -o /dev/null &> /dev/null; then
  glr_usable="yes"
fi
rm -f test.c
if test $glr_usable; then
  AC_MSG_RESULT([yes]);
else
  AC_MSG_RESULT([no])
  AC_MSG_ERROR([bison GLR parser cannot be used with the $CC compiler; please upgrade to bison 1.875a or later]);
fi

# Set flags
CPPFLAGS=""
CFLAGS="-std=c99"
CXXFLAGS="-std=c++98"
LDFLAGS="-L$PWD/src -L$PWD/src/svc"
BFLAGS=""

# Check for C99 standard
AC_MSG_CHECKING([for C99 standard])
AC_RUN_IFELSE(
  AC_LANG_PROGRAM(,
    [#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
     return 0;
     #else
     return 1;
     #endif
    ]
  ),
  AC_MSG_RESULT([yes]),
  AC_MSG_RESULT([no])
  AC_MSG_ERROR([C99 standard not supported])
)

AC_MSG_CHECKING([for C++ 98 standard])
AC_LANG_PUSH(C++)
AC_RUN_IFELSE(
  AC_LANG_PROGRAM(,
    [#if defined __cplusplus && (__cplusplus == 199711L || __cplusplus == 1)
     return 0;
     #else
     return 1;
     #endif
    ]
  ),
  [
    AC_MSG_RESULT([yes])
  ],
  [
    AC_MSG_RESULT([no])
    CXXFLAGS=""
  ]
)
AC_LANG_POP(C++)

# Check whether optimization must be suppressed
AC_ARG_ENABLE(
  [optimisation],
  AC_HELP_STRING(
    [--disable-optimisation],
    [compile without compile-time optimisations]
  )
)
if test $enable_optimisation; then
 OFLAGS=""
else
 OFLAGS="-O2"
fi

# Check for debug/profile options
AC_ARG_ENABLE(
  [debug],
  AC_HELP_STRING(
    [--enable-debug],
    [compile with debugging information]
  )
)
if test $enable_debug; then
  AC_MSG_NOTICE([compiling with debugging information])
  if test $USE_64_BIT; then
    CPPFLAGS="-DAT_64BIT $CPPFLAGS"
  fi
  CFLAGS="$CFLAGS -g -Wall $OFLAGS -W -pedantic"
  CXXFLAGS="$CXXFLAGS -g -Wall $OFLAGS -W"
  BFLAGS="-t $BFLAGS"
else
  CPPFLAGS="-DNDEBUG $CPPFLAGS"
  CFLAGS="$CFLAGS $OFLAGS -w"
  CXXFLAGS="$CXXFLAGS $OFLAGS -w"
fi
AC_SUBST([BFLAGS],[])

AC_ARG_ENABLE(
  [profile],
  AC_HELP_STRING(
    [--enable-profile],
    [compile with profiling support]
  )
)
if test $enable_profile; then
  AC_MSG_NOTICE([compiling with profiling support])
  CPPFLAGS="-pg $CPPFLAGS"
  LDFLAGS="-pg $LDFLAGS"
fi

# Check whether dymbol table stripping from binaries is requested
AC_ARG_ENABLE(
  [experimental],
  AC_HELP_STRING(
    [--enable-experimental],
    [also build experimental tools]
  )
)
if test $enable_experimental; then
  BUILD_EXPERIMENTAL="experimental"
fi
AC_SUBST([BUILD_EXPERIMENTAL])

# Check for ATerm library and header files
AC_ARG_WITH(
  [aterm],
  AC_HELP_STRING(
    [[[[--with-aterm=dir@<:@,suf@:>@]]]],
    [use ATerm headers from dir/include@<:@/suf@:>@ and the ATerm library from
     dir/lib@<:@/suf@:>@
    ]
  )
)
if test $with_aterm; then
  ATERM_ROOT=$(echo $with_aterm, | cut -d, -f1)
  ATERM_SUFFIX=$(echo $with_aterm, | cut -d, -f2)
  if test $ATERM_ROOT; then
    if test $ATERM_SUFFIX; then
      ATERM_INCLUDE="$ATERM_ROOT/include/$ATERM_SUFFIX"
      ATERM_LIB="$ATERM_ROOT/lib/$ATERM_SUFFIX"
    else
      ATERM_INCLUDE="$ATERM_ROOT/include"
      ATERM_LIB="$ATERM_ROOT/lib"
    fi
    ATERM_CPPFLAGS="-I$ATERM_INCLUDE"
    ATERM_LDFLAGS="-L$ATERM_LIB"
  fi
  if test $enable_profile; then
    AC_CHECK_LIB([ATerm-prof], [ATinit], , [AC_MSG_ERROR([cannot find ATerm-prof library])])
    ATERM_NAME="ATerm-prof"
  else
    if test $enable_debug; then
      AC_CHECK_LIB([ATerm-dbg], [ATinit], , [AC_MSG_ERROR([cannot find ATerm-dbg library])])
      ATERM_NAME="ATerm-debug"
    else
      AC_CHECK_LIB([ATerm], [ATinit], , [AC_MSG_ERROR([cannot find ATerm library])])
      ATERM_NAME="ATerm"
    fi
  fi;
  AC_CHECK_HEADERS([aterm2.h], , [AC_MSG_ERROR([cannot find ATerm headers])])
else
  AC_MSG_NOTICE([using local ATerm library])
  ATERM_CPPFLAGS="-I$PWD/src"
  ATERM_LDFLAGS="-L$PWD/src/aterm"
  ATERM_NAME="ATerm"
  BUILD_ATERM=1
fi
AC_SUBST([ATERM_CPPFLAGS],[])
AC_SUBST([ATERM_LDFLAGS],[])
AC_SUBST([ATERM_NAME],[])
AC_SUBST([BUILD_ATERM],[])
# debug: AC_MSG_RESULT([CPPFLAGS=$CPPFLAGS])
# debug: AC_MSG_RESULT([LDFLAGS=$LDFLAGS])

# Check for xml2 library and header files
AC_ARG_WITH(
  [xml2],
  AC_HELP_STRING(
    [--with-libxml2=prefix],
    [try this for the libxml2 prefix install directory]
  )  
)
if test $with_libxml2; then
  AC_MSG_CHECKING([for libXML2 in $with_libxml2])
  AC_COMPILE_IFELSE(
     [
      #include "$with_xml2/include/libxml2/libxml/xmlversion.h"
     ],
     [
       AC_MSG_RESULT(yes)
       USE_LIBXML2=1
       AC_SUBST([LIBXML2_CPPFLAGS],[-I$with_libxml2/include/libxml2])
     ],
     [
       AC_MSG_RESULT(no)
     ])
else
  #Headers may be in a standard location, ask xml2-config
  AC_CHECK_PROG([XML2_CONFIG],[xml2-config],[xml2-config])

  if test $XML2_CONFIG; then
    USE_LIBXML2=1
    LIBXML2_CPPFLAGS="$($XML2_CONFIG --cflags)"
    LIBXML2_LDFLAGS="$($XML2_CONFIG --libs)"
  fi
fi

if test -z $USE_LIBXML2; then
  AC_MSG_NOTICE(libXML2 is required for some tools in the toolset.)
  AC_MSG_NOTICE(it can be obtained from http://www.xmlsoft.org.)
fi
AC_SUBST([LIBXML2_CONFIG],[])
AC_SUBST([LIBXML2_CPPFLAGS],[])
AC_SUBST([LIBXML2_LDFLAGS],[])
AC_SUBST([USE_LIBXML2],[])

# Check for xmlto
AC_ARG_WITH(
  [xmlto],
  AC_HELP_STRING(
    [--with-xmlto=prefix],
    [try this for the xmlto prefix install directory]
  )
)
if test $with_xmlto; then
  AC_CHECK_PROG([XMLTO],[xmlto],[$with_xmlto/xmlto],[],[$with_xmlto])
else 
  AC_CHECK_PROG([XMLTO],[xmlto],[xmlto])
fi
if test $XMLTO; then
  USE_XMLTO=1
else
  AC_MSG_NOTICE(documentation cannot be build because xmlto was not found.)
  AC_MSG_NOTICE(xmlto can be obtained from http://cyberelk.net/tim/xmlto/.)
fi
AC_SUBST([USE_XMLTO],[])

# Check for Boost C++ library
AC_ARG_WITH(
  [boost],
  AC_HELP_STRING(
    [--with-boost=prefix],
    [try this for the boost prefix install directory]
  )
)
AC_MSG_CHECKING([for the Boost C++ libraries])
if test $with_boost; then
  AC_COMPILE_IFELSE(
     [
      #include "$with_boost/include/boost/version.hpp"
     ],
     [
       AC_SUBST([BOOST_PREFIX],[$with_boost])
       AC_MSG_RESULT(yes)
       USE_BOOST=1
     ],
     [
       AC_MSG_RESULT(no)
       AC_MSG_NOTICE(the Boost C++ libraries are required for certain tools in the toolset.)
       AC_MSG_NOTICE(they can be obtained from http://www.boost.org.)
     ])
else 
  USE_BOOST=1
  AC_MSG_RESULT(yes)
  AC_SUBST([BOOST_PREFIX],[boost])
  AC_MSG_NOTICE(using local Boost libraries.)
fi
AC_SUBST([USE_BOOST],[])

# Check for wxWidgets
AC_ARG_WITH(
  [wx],
  AC_HELP_STRING(
    [--with-wx=dir],
    [use wxWidgets configuration program wx-config in dir]
  )
)
if test $with_wx; then
  AC_CHECK_PROG([WX_CONFIG],[wx-config],[$with_wx/wx-config],[],[$with_wx])
else 
  AC_CHECK_PROG([WX_CONFIG],[wx-config],[wx-config])
fi
if test $WX_CONFIG; then
  USE_WX=1
  WX_CPPFLAGS="$($WX_CONFIG --cppflags)"
  WX_LDFLAGS="$($WX_CONFIG --libs core,base,adv,net)"
fi
AC_SUBST([USE_WX],[])
AC_SUBST([WX_CONFIG],[])
AC_SUBST([WX_CPPFLAGS],[])
AC_SUBST([WX_LDFLAGS],[])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strdup])

AC_MSG_NOTICE()
AC_MSG_NOTICE()
if test $USE_WX; then
  AC_MSG_NOTICE(wxWidgets     :  FOUND)
else
  AC_MSG_NOTICE(wxWidgets     :  MISSING)
fi
if test $USE_LIBXML2; then
  AC_MSG_NOTICE(libXML2       :  FOUND)
else
  AC_MSG_NOTICE(libXML2       :  MISSING)
fi

#evaluate some location variables for use with bjam
if test "x$prefix" = xNONE
then
  prefix=$ac_default_prefix
fi
if test "x$exec_prefix" = xNONE
then
  exec_prefix='${prefix}'
fi

eval "eval PREFIX_DIR=${prefix}"
AC_SUBST([PREFIX_DIR],[])
eval "eval EXEC_PREFIX_DIR=${exec_prefix}"
AC_SUBST([EXEC_PREFIX_DIR],[])
eval "eval DATA_DIR=${datadir}"
AC_SUBST([DATA_DIR],[])
eval "eval BIN_DIR=${bindir}"
AC_SUBST([LIB_DIR],[])
eval "eval LIB_DIR=${libdir}"
AC_SUBST([BIN_DIR],[])
eval "eval INCLUDE_DIR=${includedir}"
AC_SUBST([INCLUDE_DIR],[])
eval "eval MAN_DIR=${mandir}"
AC_SUBST([MAN_DIR],[])

AC_MSG_NOTICE()
AC_MSG_NOTICE(Some tools will not be built if any of these libraries are missing.)
AC_MSG_NOTICE()

AC_CONFIG_FILES([src/boost/tools/build/v2/user-config.jam src/squadt/examples/tool_catalog src/rewr_innerc_aux.h src/setup.h config config.jam])
AC_OUTPUT

