AC_INIT([mCRL2 toolset],[0.2],[bug@mcrl2.org])
AC_CONFIG_AUX_DIR(build/autoconf)
AC_CONFIG_SRCDIR([src/setup.h.in])
AC_CANONICAL_HOST
AC_PREFIX_DEFAULT([$HOME])

# Flags for runtime compilation and linking
R_CFLAGS="-Wno-unused -O3 -rdynamic"
R_SCFLAGS="-fPIC"
R_CXXFLAGS="-Wno-unused -O3 -rdynamic"
R_SCXXFLAGS="-fPIC"
R_CPPFLAGS=""
R_LDFLAGS=""
R_SLDFLAGS="-shared"

# Default toolset to use with Boost.Build
BJAM_CXX="gcc"
BJAM=bin/bjam

# Setting host OS
case $host in
  *-*-mingw32*)
    HOST_OS="MINGW"
    BJAM="$BJAM.exe"
    ;;
  *-*-cygwin*)
    HOST_OS="CYGWIN"
    BJAM="$BJAM.exe"
    ;;
  *-*-msdos* | *-*-go32* | *-*-windows*)
    HOST_OS="WINDOWS"
    BJAM="$BJAM.exe"
    BJAM_CXX="vc-7_1"
    ;;
  *-*-linux*)
    HOST_OS="LINUX"
    ;;
  *-*-*bsd*)
    HOST_OS="BSD"
    ;;
  *-*-darwin*)
    HOST_OS="MACOSX"
    R_CFLAGS="-Wno-unused -O3"
    R_CXXFLAGS="-Wno-unused -O3"
    R_LDFLAGS="-bundle -undefined dynamic_lookup"
    R_SLDFLAGS=""
    BJAM_CXX="darwin"
    ;;
  *)
    HOST_OS="UNKNOWN"
    ;;
esac
AC_SUBST([BJAM])
AC_SUBST([BJAM_CXX])
AC_SUBST([HOST_OS])

# Default is 32 bits, test is probably very incomplete
case $host in
  *64*-*-*)
    ADDRESS_MODEL=64
    ;;
  *32*-*-*)
    ADDRESS_MODEL=32
    ;;
  *16*-*-*)
    ADDRESS_MODEL=16
    ;;
  *)
    ADDRESS_MODEL=32
    ;;
esac

AC_SUBST([ADDRESS_MODEL],[])

# Checks for programs.
AC_PROG_CPP
AC_PROG_CXX

# Check for compatible flex
AC_CHECK_PROG([FLEX], [flex], [flex], [])
if test -z $FLEX; then
  AC_MSG_RESULT([no])
#  AC_MSG_ERROR([cannot find flex])
else
  AC_MSG_CHECKING([whether flex option c++ is available])
  if $FLEX -I -o/dev/null build/test.l &> /dev/null; then
    AC_MSG_RESULT([yes]);
    AC_MSG_CHECKING([whether flex C++ scanner can be used])
    $FLEX -I -otest.cpp build/test.l
    if $CXX -c test.cpp -o /dev/null &> /dev/null; then
      flex_usable="yes"
    fi
    rm -f test.cpp
    if test $flex_usable; then
      AC_MSG_RESULT([yes]);
    else
      AC_MSG_RESULT([no])
    #  AC_MSG_ERROR([flex C++ scanner cannot be used with the $CXX compiler; please upgrade to flex 2.5.4a-12 or later]);
    fi
  else
    AC_MSG_RESULT([no])
  #  AC_MSG_ERROR([flex option c++ not available; please upgrade to flex 2.5.4a-12 or later]);
  fi
fi

# Check for compatible bison
AC_CHECK_PROG([BISON], [bison], [bison], [])
if test -z $BISON; then
  AC_MSG_RESULT([no])
#  AC_MSG_ERROR([cannot find bison])
else
  AC_MSG_CHECKING([whether bison option %glr-parser is available])
  if $BISON build/test.y -o/dev/null &> /dev/null; then
    AC_MSG_RESULT([yes]);
    AC_MSG_CHECKING([whether bison GLR parser can be used])
    $BISON build/test.y -o test.cpp
    if $CXX -c test.cpp -o /dev/null &> /dev/null; then
      glr_usable="yes"
    fi
    rm -f test.cpp
    if test $glr_usable; then
      AC_MSG_RESULT([yes]);
    else
      AC_MSG_RESULT([no])
    #  AC_MSG_ERROR([bison GLR parser cannot be used with the $CXX compiler; please upgrade to bison 2.0 or later]);
    fi
  else
    AC_MSG_RESULT([no])
#  AC_MSG_ERROR([bison option %glr-parser not available; please upgrade to bison 2.0 or later]);
  fi
fi

# Set flags
CPPFLAGS=""
CFLAGS="-std=c99"
CXXFLAGS="-std=c++98"
LDFLAGS=""
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
  AC_MSG_RESULT([no]),
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
  [AC_MSG_RESULT([yes])],
  [AC_MSG_RESULT([no])],
  [AC_MSG_ERROR([C++ 98 standard not supported])]
)
AC_LANG_POP(C++)

# Add standard compatibility check flags for runtime compilation
R_CFLAGS="$R_CFLAGS $CFLAGS"
R_CXXFLAGS="$R_CXXFLAGS $CXXFLAGS"

# Check whether optimization must be suppressed
AC_ARG_ENABLE(
  [optimisation],
  AS_HELP_STRING([--disable-optimisation],
                 [disables compile-time optimisations])
)
if test $enable_optimisation; then
 OFLAGS=""
else
 OFLAGS="-O2"
fi

# Check for debug/profile options
AC_ARG_ENABLE(
  [debug],
  AS_HELP_STRING([--enable-debug],
                 [adds debugging information to libraries and executables])
)
if test $enable_debug; then
  CFLAGS="$CFLAGS -g -Wall $OFLAGS -W -pedantic"
  CXXFLAGS="$CXXFLAGS -g -Wall $OFLAGS -W"
  BFLAGS="-t $BFLAGS"
  BUILD_VARIANT="debug"
else
  CPPFLAGS="-DNDEBUG $CPPFLAGS"
  CFLAGS="$CFLAGS $OFLAGS -w"
  CXXFLAGS="$CXXFLAGS $OFLAGS -w"
  BUILD_VARIANT="release"
fi
AC_SUBST([BFLAGS],[])

AC_ARG_ENABLE(
  [profile],
  AS_HELP_STRING([--enable-profile],[adds profiling support to libraries and executables])
)
if test $enable_profile; then
  BUILD_VARIANT="profile"
  CPPFLAGS="-pg $CPPFLAGS"
  LDFLAGS="-pg $LDFLAGS"
fi

# Check for debug/profile options
AC_ARG_WITH(
  [static],
  AS_HELP_STRING([--enable-static],[produce and link to static libraries])
)
if test $enable_static; then
  BUILD_OPTIONS="\"<link>static\""
fi

# Check whether symbol table stripping from binaries is requested
AC_ARG_ENABLE(
  [experimental],
  AS_HELP_STRING([--enable-experimental],[also build experimental tools
  ])
)
if test $enable_experimental; then
  BUILD_EXTRA="--enable-experimental $BUILD_EXTRA"
fi

# Check whether symbol table stripping from binaries is requested
AC_ARG_ENABLE(
  [deprecated],
  AS_HELP_STRING([--enable-deprecated],[also build deprecated tools
  ])
)
if test $enable_experimental; then
  BUILD_EXTRA="--enable-deprecated $BUILD_EXTRA"
fi

# Check whether optimization must be suppressed
AC_ARG_ENABLE(
  [squadt_support],
  AS_HELP_STRING([--disable-squadt-support],[do not compile squadt support in tools
  ])
)
if test $enable_squadt_support; then
  BUILD_EXTRA="--disable-squadt-support $BUILD_EXTRA"
fi

# Check for ATerm library and header files
AC_ARG_WITH(
  [aterm],
  AS_HELP_STRING([[[[--with-aterm=dir@<:@,suf@:>@]]]],
     [use ATerm headers from dir/include@<:@/suf@:>@ and the ATerm library from dir/lib@<:@/suf@:>@])
)
BUILD_ATERM="yes ($with_aterm)"
if test $with_aterm; then
  AC_MSG_CHECKING([for ATerm library in $with_aterm])
  ATERM_ROOT=$(echo $with_aterm, | cut -d, -f1)
  ATERM_SUFFIX=/$(echo $with_aterm, | cut -d, -f2 -s)
  if test $ATERM_ROOT; then
    ATERM_CPPFLAGS="-I$ATERM_ROOT/include$ATERM_SUFFIX"
    ATERM_LDFLAGS="-L$ATERM_LIB$ATERM_SUFFIX"
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
  ATERM_CPPFLAGS="-I$PWD/src/aterm"
  ATERM_LDFLAGS="-L$PWD/src/aterm"
  ATERM_NAME="ATerm"
  BUILD_ATERM="builtin"

  if test "$ADDRESS_MODEL" = "64"; then
    ATERM_CPPFLAGS="-DAT_64BIT $ATERM_CPPFLAGS"
  fi
fi
AC_SUBST([ATERM_CPPFLAGS],[])
AC_SUBST([ATERM_LDFLAGS],[])
AC_SUBST([ATERM_NAME],[])

AC_ARG_WITH(
  [cadp],
  AS_HELP_STRING([--with-cadp=prefix],[try this for the CADP prefix directory])  
)
USE_BCG=no
if test ${with_cadp}; then
  AC_MSG_CHECKING([for CADP in $with_cadp])
  AC_COMPILE_IFELSE(
     [
      #include <$with_cadp/incl/bcg_user.h>
     ],
     [
       AC_SUBST([CADP_HEADERS],["$with_cadp/incl"])
       AC_SUBST([CADP_LIBRARIES],["${with_cadp}/bin.`${with_cadp}/com/arch`"])
       USE_BCG=yes
     ])
  AC_MSG_RESULT($USE_BCG)
fi
AC_SUBST([USE_BCG])

# Check for Boost C++ library
AC_ARG_WITH(
  [boost],
  AS_HELP_STRING([--with-boost=prefix],[try this for the boost prefix install directory])
)
AC_MSG_CHECKING([for the Boost C++ libraries])
USE_BOOST="yes $with_boost"
if test $with_boost; then
  AC_COMPILE_IFELSE(
     [
      #include "$with_boost/include/boost/version.hpp"
     ],
     [
       BOOST_PREFIX=$with_boost
       AC_MSG_RESULT(yes)
     ],
     [
       AC_MSG_RESULT(no)
       AC_MSG_ERROR([the Boost C++ libraries are required for a number of tools in the toolset.]
                    [they can be obtained from http://www.boost.org.])
     ])
else 
  USE_BOOST=builtin
  AC_MSG_RESULT(yes)
  BOOST_PREFIX=boost
fi
AC_SUBST([BOOST_PREFIX],[$with_boost])

# Check for OpenGL
AC_ARG_WITH([OpenGL],
            AS_HELP_STRING([--with-opengl=DIR], [OpenGL/Mesa installation directory]))
AC_MSG_CHECKING([for OpenGL])
if test $with_opengl; then
  CFLAGS="-I$with_opengl/include"
  LDFLAGS="-L$with_opengl/lib"
fi

case $host in
  *-*-mingw32*)
    LIBS="opengl32"
    ;;
  *-*-cygwin*)
    LIBS="opengl32"
    ;;
  *-*-darwin*)
    LIBS="OpenGL"
    USE_OPENGL="yes (framework OpenGL)"
    ;;
  *)
    LIBS="GL"
    ;;
esac

if test "$USE_OPENGL"; then
  AC_SUBST([OPENGL_NAME],[$LIBS])

  AC_MSG_RESULT([yes])
else
  USE_OPENGL=$with_opengl
  AC_COMPILE_IFELSE(AC_LANG_PROGRAM([
                   #include <GL/gl.h>
                   #include <GL/glu.h>],
                  [glPointSize(1.0f);]),
                  USE_OPENGL="yes",USE_OPENGL=no)

  if test $USE_OPENGL; then
    AC_MSG_RESULT([yes])

    if test $with_opengl; then
      USE_OPENGL="yes ($with_opengl)"
      AC_SUBST([OPENGL_IPATH],[$with_opengl/include])
      AC_SUBST([OPENGL_LPATH],[$with_opengl/lib])
    fi

    AC_SUBST([OPENGL_NAME],[$LIBS])
  else
    AC_MSG_RESULT([no])
  fi
fi

# Check for xml2 library and header files
AC_ARG_WITH(
  [xml2],
  AS_HELP_STRING([--with-libxml2=prefix],[try this for the libxml2 prefix install directory])  
)
USE_XML2=no
if test $with_libxml2; then
  AC_MSG_CHECKING([for libxml2 in $with_libxml2])
  AC_COMPILE_IFELSE(
     [
      #include <$with_xml2/include/libxml2/libxml/xmlversion.h>
     ],
     [
       USE_XML2=yes
       XML2_CPPFLAGS="-I$with_libxml2/include/libxml2"
       XML2_LDFLAGS="-I$with_libxml2/lib"
     ],)

  AC_MSG_RESULT($USE_XML2)
  USE_XML2="yes ($with_libxml2)"
else
  #Headers may be in a standard location, ask xml2-config
  AC_CHECK_PROG([XML2_CONFIG],[xml2-config],[xml2-config])

  if test $XML2_CONFIG; then
    USE_XML2="yes ($($XML2_CONFIG --prefix))"
    XML2_CPPFLAGS="$($XML2_CONFIG --cflags)"
    XML2_LDFLAGS="$($XML2_CONFIG --libs)"
  fi
fi

AC_SUBST([XML2_CONFIG],[])
AC_SUBST([XML2_CPPFLAGS],[])
AC_SUBST([XML2_LDFLAGS],[])

# Check for xmlto
AC_ARG_WITH(
  [xmlto],
  AS_HELP_STRING([--with-xmlto=prefix],[try this for the xmlto prefix install directory])
)
if test $with_xmlto; then
  AC_CHECK_PROG([XMLTO],[xmlto],[$with_xmlto/xmlto],[],[$with_xmlto])
else 
  AC_CHECK_PROG([XMLTO],[xmlto],[xmlto])
fi
if test $XMLTO; then
  USE_XMLTO=xmlto_available
else
  AC_MSG_NOTICE(documentation cannot be built because xmlto was not found.)
  AC_MSG_NOTICE(xmlto can be obtained from http://cyberelk.net/tim/xmlto/.)
fi
AC_SUBST([USE_XMLTO],[])

# Check for wxWidgets
AC_ARG_WITH(
  [wx],
  AS_HELP_STRING([--with-wx=DIR],[use wxWidgets configuration program wx-config in DIR])
)
if test $with_wx; then
  AC_CHECK_PROG([WX_CONFIG],[wx-config],[$with_wx/wx-config],[],[$with_wx])
else 
  AC_CHECK_PROG([WX_CONFIG],[wx-config],[wx-config])
fi
if test $WX_CONFIG; then
  USE_WX="yes ($($WX_CONFIG --prefix))"
  WX_CPPFLAGS="$($WX_CONFIG --cppflags)"
  WX_BASE_LDFLAGS="$($WX_CONFIG --libs core,base)"
  WX_GL_LDFLAGS="$($WX_CONFIG --libs core,base,gl)"
else
  USE_WX=no
fi
AC_SUBST([WX_CONFIG],[])
AC_SUBST([WX_CPPFLAGS],[])
AC_SUBST([WX_BASE_LDFLAGS],[])
AC_SUBST([WX_GL_LDFLAGS],[])

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

#evaluate some location variables for use with Boost Build
if test "x$prefix" = xNONE
then
  prefix=$ac_default_prefix
fi
if test "x$exec_prefix" = xNONE
then
  exec_prefix='${prefix}'
fi

AC_SUBST([BUILD_VARIANT],[])
AC_SUBST([BUILD_OPTIONS],[])
AC_SUBST([BUILD_EXTRA])

if test "$BUILD_ATERM" = "builtin"; then
  eval "eval IATERM_CPPFLAGS=${includedir}/mcrl2"
  if test "$ADDRESS_MODEL" = "64"; then
    IATERM_CPPFLAGS="-DAT_64BIT $(IATERM_CPPFLAGS)"
  fi
  AC_SUBST([IATERM_CPPFLAGS],["-I$IATERM_CPPFLAGS/aterm"])

  eval "eval IATERM_LDFLAGS=${libdir}/mcrl2"
  AC_SUBST([IATERM_LDFLAGS],["-L${IATERM_LDFLAGS} -lATerm"])
else
  AC_SUBST([IATERM_CPPFLAGS],["$ATERM_CPPFLAGS"])
  AC_SUBST([IATERM_LDFLAGS],["$ATERM_LDFLAGS -lATerm"])
fi

eval "eval PREFIX_DIR=${prefix}"
eval "eval EXEC_PREFIX_DIR=${exec_prefix}"

prefix="\$(prefix)"
includedir="$includedir/mcrl2"
datadir="$datadir/mcrl2"
libdir="$libdir/mcrl2"

eval "eval DATA_DIR=${datadir}"
eval "eval BIN_DIR=${bindir}"
eval "eval LIB_DIR=${libdir}"
eval INCLUDE_DIR=${includedir}
eval "eval MAN_DIR=${mandir}"

AC_SUBST([PREFIX_DIR],[])
AC_SUBST([EXEC_PREFIX_DIR],[])
AC_SUBST([DATA_DIR],[])
AC_SUBST([LIB_DIR],[])
AC_SUBST([BIN_DIR],[])
AC_SUBST([INCLUDE_DIR],[])
AC_SUBST([MAN_DIR],[])

# Runtime compilation flags
AC_SUBST([R_CFLAGS],[])
AC_SUBST([R_SCFLAGS],["$R_SCFLAGS"])
AC_SUBST([R_CXXFLAGS],[])
AC_SUBST([R_SCXXFLAGS],["$R_SCXXFLAGS"])
AC_SUBST([R_CPPFLAGS],["$CPPFLAGS"])
AC_SUBST([R_LDFLAGS],["$R_LDFLAGS"])
AC_SUBST([R_SLDFLAGS],["$R_SLDFLAGS"])

AC_MSG_RESULT([
   Configured $BUILD_VARIANT variant-build using libraries:

     ATerm         :  $BUILD_ATERM
     Boost         :  $USE_BOOST
     CADP          :  $USE_BCG
     libxml2       :  $USE_XML2
     OpenGL        :  $USE_OPENGL
     wxWidgets     :  $USE_WX
     
   Some tools will not be built if any of these libraries are missing.
])

AC_CONFIG_FILES([build/config.mk build/config.jam src/setup.h])
AC_OUTPUT

