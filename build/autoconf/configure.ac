AC_INIT([mCRL2 toolset],[1.0],[http://www.mcrl2.org/issuetracker])
AC_CONFIG_AUX_DIR(build/autoconf)
AC_CONFIG_SRCDIR([boost-build.jam])
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
BJAM=build/bin/bjam
BUILD_OPTIONS="--no-display-notice"

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
    case $host in
      arm-*-*)
        BUILD_OPTIONS="${BUILD_OPTIONS} architecture=arm"
      ;;
      i386-*-*)
        BUILD_OPTIONS="${BUILD_OPTIONS} architecture=x86"
      ;;
      powerpc-*-*)
        BUILD_OPTIONS="${BUILD_OPTIONS} architecture=power"
      ;;
    esac
    HOST_OS="MACOSX"
    R_CFLAGS="-Wno-unused -O3"
    R_CXXFLAGS="-Wno-unused -O3"
    R_LDFLAGS="-bundle -undefined dynamic_lookup"
    R_SLDFLAGS=""
    BJAM_CXX="darwin"
    TOOLSET_FLAGS="<linker-type>darwin"
    ;;
  *-sun-*)
    HOST_OS="SUNOS"
    TOOLSET_FLAGS="<linker-type>sun"
    ;;
  *)
    HOST_OS="UNKNOWN"
    ;;
esac
AC_SUBST([BJAM])
AC_SUBST([BJAM_CXX])
AC_SUBST([TOOLSET_FLAGS])
AC_SUBST([HOST_OS])


# Fix for ticket 643

# Default is 32 bits, test is probably very incomplete
#case $host in
#  *64*-*-*)
#    ADDRESS_MODEL=64
#    ;;
#  *32*-*-*)
#    ADDRESS_MODEL=32
#    ;;
#  *16*-*-*)
#    ADDRESS_MODEL=16
#    ;;
#  *)
#    ADDRESS_MODEL=32
#    ;;
#esac
#
AC_CHECK_SIZEOF([void *])
case "$ac_cv_sizeof_void_p" in
    4) ADDRESS_MODEL=32 ;;
    8) ADDRESS_MODEL=64 ;;
    *)  AC_MSG_FAILURE([unknown address model]) ;;
esac

BUILD_OPTIONS="${BUILD_OPTIONS} address-model=${ADDRESS_MODEL}"

# Checks for programs.
AC_PROG_CPP
AC_PROG_CXX

#check if the g++ compiler works
AC_CACHE_CHECK([whether the C++ compiler works], [avahi_cv_sys_cxx_works], [
    AC_LANG_PUSH([C++])
    AC_COMPILE_IFELSE([int main() { }], [avahi_cv_sys_cxx_works=yes],
        [avahi_cv_sys_cxx_works=no])
    AC_LANG_POP([C++])
    ])
[ if [ "x$avahi_cv_sys_cxx_works" = "xno" ]; then ]
    AC_MSG_FAILURE([The C++ compiler does not work. Please (re)install the C++ compiler])
[ fi ]

# Check for compatible flex
AC_CHECK_PROG([FLEX], [flex], [flex], [])
if test -n $FLEX; then
  AC_MSG_CHECKING([whether flex option c++ is available])
  if $FLEX -I -o/dev/null build/autoconf/test.l &> /dev/null; then
    AC_MSG_RESULT([yes]);
    AC_MSG_CHECKING([whether flex C++ scanner can be used])
    $FLEX -I -otest.cpp build/autoconf/test.l
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
if test -n $BISON; then
  AC_MSG_CHECKING([whether bison option %glr-parser is available])
  if $BISON build/autoconf/test.y -o/dev/null &> /dev/null; then
    AC_MSG_RESULT([yes]);
    AC_MSG_CHECKING([whether bison GLR parser can be used])
    $BISON build/autoconf/test.y -o test.cpp
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
R_CFLAGS="${R_CFLAGS} -std=c99"
R_CXXFLAGS="${R_CXXFLAGS} -std=c++98"
CFLAGS="-std=c99"
CXXFLAGS="-std=c++98"

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

# Check whether optimization must be suppressed
AC_ARG_ENABLE(
  [optimisation],
  AS_HELP_STRING([--disable-optimisation],
                 [disables compile-time optimisation])
)

# Check for debug/profile options
AC_ARG_ENABLE(
  [debug],
  AS_HELP_STRING([--enable-debug],
                 [adds debugging information to libraries and executables])
)
if test "x$enable_debug" = xyes; then
  CFLAGS="$CFLAGS -g -Wall $OFLAGS -W -pedantic"
  CXXFLAGS="$CXXFLAGS -g -Wall $OFLAGS -W"
  BFLAGS="-t $BFLAGS"
  BUILD_VARIANT="debug"
  BUILD_OPTIONS="${BUILD_OPTIONS} debug-symbols=on runtime-debugging=on"
  if test "x$enable_optimisation" = xyes; then
   BUILD_OPTIONS="${BUILD_OPTIONS} optimization=speed"
  fi
else
  CPPFLAGS="-DNDEBUG $CPPFLAGS"
  CFLAGS="$CFLAGS $OFLAGS -w"
  CXXFLAGS="$CXXFLAGS $OFLAGS -w"
  BUILD_VARIANT="release"
  if test "x$enable_optimisation" = xno; then
   BUILD_OPTIONS="${BUILD_OPTIONS} optimization=off"
  fi
fi
AC_SUBST([BFLAGS],[])

AC_ARG_ENABLE(
  [profile],
  AS_HELP_STRING([--enable-profile],[adds profiling support to libraries and executables])
)
if test "x$enable_profile" = xyes; then
  BUILD_VARIANT="profile"
  BUILD_OPTIONS="profiling=on ${BUILD_OPTIONS}"
  CPPFLAGS="-pg $CPPFLAGS"
  LDFLAGS="-pg $LDFLAGS"
fi

# Check for debug/profile options
AC_ARG_ENABLE(
  [static],
  AS_HELP_STRING([--enable-static],[produce and link to static libraries])
)
if test "x$enable_static" = xyes; then
  BUILD_OPTIONS="${BUILD_OPTIONS} link=static"
else
  BUILD_OPTIONS="${BUILD_OPTIONS} link=shared"
fi

# Check whether symbol table stripping from binaries is requested
AC_ARG_ENABLE(
  [experimental],
  AS_HELP_STRING([--enable-experimental],[also build experimental tools
  ])
)
if test "x$enable_experimental" = xyes; then
  BUILD_OPTIONS="--enable-experimental $BUILD_OPTIONS"
fi

# Check whether symbol table stripping from binaries is requested
AC_ARG_ENABLE(
  [deprecated],
  AS_HELP_STRING([--enable-deprecated],[also build deprecated tools
  ])
)
if test "x$enable_deprecated" = xyes; then
  BUILD_OPTIONS="--enable-deprecated $BUILD_OPTIONS"
fi

AC_CHECK_LIB([dl], [dlopen], DL=dl, )
AC_SUBST([DL],[])

# Check whether optimization must be suppressed
AC_ARG_ENABLE(
  [squadt_support],
  AS_HELP_STRING([--disable-squadt-support],[do not compile squadt support in tools
  ])
)
if test "x$enable_squadt_support" = xno; then
  BUILD_OPTIONS="--disable-squadt-support $BUILD_OPTIONS"
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
    ATERM_CPPFLAGS="-I\\\\"$ATERM_ROOT/include$ATERM_SUFFIX\\\\""
    ATERM_LDFLAGS="-L\\\\"$ATERM_LIB$ATERM_SUFFIX\\\\""
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
  ATERM_CPPFLAGS="-I\\\"$PWD/3rd-party/aterm/include\\\""
  ATERM_LDFLAGS="-L\\\"$PWD/3rd-party/aterm/include\\\""
  ATERM_NAME="ATerm"
  BUILD_ATERM="builtin"

  if test "$ADDRESS_MODEL" = "64"; then
    ATERM_CPPFLAGS="-DAT_64BIT $ATERM_CPPFLAGS"
  fi
fi
AC_SUBST([ATERM_CPPFLAGS],[])
AC_SUBST([ATERM_LDFLAGS],[])
AC_SUBST([ATERM_NAME],[])

# Check whether C types fulfil ATerm library assumption
AC_MSG_CHECKING([ATerm library assumptions])
if test "$ADDRESS_MODEL" = "64"; then
  EXP_SIZEOF_LONG=8
else
  EXP_SIZEOF_LONG=4
fi
AC_LANG(C)
AC_RUN_IFELSE([AC_LANG_SOURCE([[
  #include <stdio.h>
  int main(int argc,char **argv)
  {
    int r = 0;
    if ( sizeof(long) != $EXP_SIZEOF_LONG )
    {
      fprintf(stderr,"error: sizeof(long) is not %i but %i\n",$EXP_SIZEOF_LONG,sizeof(long));
      r = 1;
    }
    if ( sizeof(long) != sizeof(void *) )
    {
      fprintf(stderr,"error: sizeof(long) and sizeof(void *) differ (%i vs. %i)\n",sizeof(long),sizeof(void *));
      r = 1;
    }
    if ( sizeof(int) != 4 )
    {
      fprintf(stderr,"error: sizeof(int) is not 4 but %i\n",sizeof(int));
      r = 1;
    }
    return r;
  }
]])],[ATERM_CHECK=yes],[ATERM_CHECK=no],[ATERM_CHECK=no])
AC_MSG_RESULT($ATERM_CHECK)
if test "$ATERM_CHECK" = "no"; then
  AC_MSG_ERROR([assumptions of the ATerm library are not fulfilled])
fi

AC_ARG_WITH(
  [cadp],
  AS_HELP_STRING([--with-cadp=prefix],[try this for the CADP prefix directory])  
)
if test ! ${with_cadp}; then
  with_cadp="${CADP}"
fi
AC_MSG_CHECKING([for CADP in $with_cadp])
USE_BCG=no
if test ${with_cadp}; then
  AC_COMPILE_IFELSE(
     [
      #include <$with_cadp/incl/bcg_user.h>
     ],
     [
       LIBS="-L${with_cadp}/bin.`${with_cadp}/com/arch` -lBCG_IO"
       AC_LINK_IFELSE(
       [
        #include <$with_cadp/incl/bcg_user.h>
     
        int main() {
          return (0);
        }
       ],
       [
         AC_SUBST([CADP_HEADERS],["$with_cadp/incl"])
         AC_SUBST([CADP_LIBRARIES],["${with_cadp}/bin.`${with_cadp}/com/arch`"])
         USE_BCG=yes
       ])
     ])
fi
AC_MSG_RESULT($USE_BCG)

#AC_ARG_WITH(
#  [cvc],
#  AS_HELP_STRING([--with-cvc=prefix],[try this for the CVC3 prefix directory])  
#)
#USE_CVC3=no
#if test ${with_cvc}; then
#  AC_MSG_CHECKING([for CVC3 (with GMP) in $with_cvc])
#  CPPFLAGS="-I${with_cvc}/include/cvc3"
#  AC_LANG_PUSH(C++)
#  AC_COMPILE_IFELSE(
#     [
#      #include <$with_cvc/include/cvc3/vc.h>
#     ],
#     [
#       LIBS="-L${with_cvc}/lib -lcvc3"
#       AC_LINK_IFELSE(
#       [
#        #include <$with_cvc/include/cvc3/vc.h>
#     
#        int main() {
#          return (0);
#        }
#       ],
#       [],
#       [
#         LIBS="-L${with_cvc}/lib -lcvc3 -lgmp"
#         AC_LINK_IFELSE(
#         [
#          #include <$with_cvc/include/cvc3/vc.h>
#      
#          int main() {
#            return (0);
#          }
#         ],
#         [
#           USE_CVC="$with_cvc"
#           USE_CVC3=yes
#         ])
#       ])
#     ])
#  AC_LANG_POP()
#  AC_MSG_RESULT($USE_CVC3)
#fi
AC_SUBST([USE_CVC],[])

# Check for Boost C++ library
AC_ARG_WITH(
  [boost],
  AS_HELP_STRING([--with-boost=prefix],[try this for the boost prefix install directory])
)
USE_BOOST="yes ($with_boost)"
if test $with_boost; then
  AC_MSG_CHECKING([for the Boost C++ libraries])
  AC_COMPILE_IFELSE(
     [
      #include "$with_boost/include/boost/version.hpp"
     ],
     [
       AC_RUN_IFELSE(
       [
        #include "$with_boost/include/boost/version.hpp"
      
        int main() {
          return !(103500 <= BOOST_VERSION);
        }
       ],
       [
        BOOST_INCLUDE_PATH=$with_boost/include
        BOOST_LIBRARY_PATH=$with_boost/lib
        AC_MSG_RESULT(yes)
       ],
       [
         AC_MSG_RESULT(not usable)
         USE_BOOST=builtin
       ],
       [
         AC_MSG_RESULT(no (safe default))
         USE_BOOST=builtin
       ]
       )
     ],
     [
       AC_MSG_RESULT(no (or not usable))
       USE_BOOST=builtin
     ])
else 
  USE_BOOST=builtin
fi

AC_SUBST([BOOST_INCLUDE_PATH],[])
AC_SUBST([BOOST_LIBRARY_PATH],[])

# Check for OpenGL
AC_ARG_WITH([opengl],
            AS_HELP_STRING([--with-opengl=DIR], [OpenGL/Mesa installation directory]))
AC_MSG_CHECKING([for OpenGL])

case $host in
  *-*-mingw32*)
    OPENGL_NAME="opengl32"
    ;;
  *-*-cygwin*)
    OPENGL_NAME="opengl32"
    ;;
  *-*-darwin*)
    OPENGL_NAME="OpenGL"
    USE_OPENGL="yes (framework OpenGL)"
    ;;
  *)
    OPENGL_NAME="GL"
    ;;
esac

if test "$USE_OPENGL"; then
  AC_MSG_RESULT([yes])
else
  LIBS=-l$OPENGL_NAME
  if test $with_opengl; then
    CFLAGS="-I$with_opengl/include"
    LDFLAGS="-L$with_opengl/lib"
    AC_LINK_IFELSE(
      AC_LANG_PROGRAM([
        #include <GL/gl.h>
        #include <GL/glu.h>],
        [ glPointSize(1.0f); ]),
        USE_OPENGL="yes ($with_opengl)"
        AC_SUBST([OPENGL_IPATH],[$with_opengl/include])
        AC_SUBST([OPENGL_LPATH],[$with_opengl/lib]),
        AC_MSG_ERROR(["Problem with finding OpenGL header files or libraries in path: `$with_opengl'"])
     )
  else
    AC_LINK_IFELSE(
      AC_LANG_PROGRAM([
        #include <GL/gl.h>
        #include <GL/glu.h>],
        [ glPointSize(1.0f); ]),
      USE_OPENGL="yes",
    )
  fi
  if test "$USE_OPENGL"; then
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
    USE_OPENGL="no"
    OPENGL_NAME=""
  fi
  LIBS=""
fi
AC_SUBST([OPENGL_NAME],[])

# Check for wxWidgets
AC_ARG_WITH(
  [wx-config],
  AS_HELP_STRING([--with-wx-config=FILE],[use FILE for wxWidgets configuration program wx-config])
)
if test "$with_wx_config"; then
  if test $($with_wx_config --version 2>/dev/null); then
    WX_CONFIG="$with_wx_config"
  fi
else
  AC_CHECK_PROG([WX_CONFIG],[wx-config],[wx-config])
  if test -z $WX_CONFIG; then
    AC_CHECK_PROG([WX_CONFIG],[wx-config-2.8],[wx-config-2.8])
  fi
fi
if test "$WX_CONFIG"; then
  USE_WX="yes ($($WX_CONFIG --prefix))"
  WX_CPPFLAGS="$($WX_CONFIG --cppflags)"
  if test -z "$($WX_CONFIG --static=no 2>/dev/null)"; then
    WX_BASE_LDFLAGS="$($WX_CONFIG --static=no --libs core,base,adv)"
    WX_GL_LDFLAGS="$($WX_CONFIG --static=no --libs core,base,gl,xml,html,adv)"
    WX_TYPE="shared"
  else
    WX_BASE_LDFLAGS="$($WX_CONFIG --static=yes --libs core,base,adv)"
    WX_GL_LDFLAGS="$($WX_CONFIG --static=yes --libs core,base,gl,xml,html,adv)"
    WX_TYPE="static"
  fi
else
  USE_WX=no
fi
AC_SUBST([WX_TYPE],[])
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
  prefix="$ac_default_prefix"
fi
if test "x$exec_prefix" = xNONE
then
  exec_prefix='${prefix}'
fi

if test "$BUILD_ATERM" = "builtin"; then
  eval "eval IATERM_CPPFLAGS=\\\"${includedir}/mcrl2/aterm\\\""
  if test "$ADDRESS_MODEL" = "64"; then
    AC_SUBST([IATERM_CPPFLAGS],["-m64 -DAT_64BIT -I\\\"${IATERM_CPPFLAGS}\\\""])
  else
    AC_SUBST([IATERM_CPPFLAGS],["-m32 -I\\\"${IATERM_CPPFLAGS}\\\""])
  fi

  eval "eval IATERM_LDFLAGS=\\\"${libdir}/mcrl2\\\""
  AC_SUBST([IATERM_LDFLAGS],["-L\\\"${IATERM_LDFLAGS}\\\" -lATerm"])
else
  AC_SUBST([IATERM_CPPFLAGS],["${ATERM_CPPFLAGS}"])
  AC_SUBST([IATERM_LDFLAGS],["${ATERM_LDFLAGS} -lATerm"])
fi

# Set install path related settings
BUILD_OPTIONS="$BUILD_OPTIONS --prefix=\"${prefix}\" variant=${BUILD_VARIANT}"

eval "eval includedir=${includedir}"
if test ! "${prefix}/include" = "${includedir}"; then
  BUILD_OPTIONS="$BUILD_OPTIONS --includedir=\"${includedir}\""
fi
eval "eval bindir=${bindir}"
if test ! "${prefix}/bin" = "${bindir}"; then
  BUILD_OPTIONS="$BUILD_OPTIONS --bindir=\"${bindir}\""
fi
eval "eval libdir=${libdir}"
if test ! "${prefix}/lib" = "${libdir}"; then
  BUILD_OPTIONS="$BUILD_OPTIONS --libdir=\"${libdir}\""
fi
eval "eval datadir=${datadir}"
if test ! "${prefix}/share" = "${datadir}"; then
  BUILD_OPTIONS="$BUILD_OPTIONS --datadir=\"${datadir}\""
fi

AC_SUBST([BUILD_OPTIONS])

# Runtime compilation flags
AC_SUBST([R_CFLAGS],["$R_CFLAGS"])
AC_SUBST([R_SCFLAGS],["$R_SCFLAGS"])
AC_SUBST([R_CXXFLAGS],["$R_CXXFLAGS"])
AC_SUBST([R_SCXXFLAGS],["$R_SCXXFLAGS"])
AC_SUBST([R_CPPFLAGS],["$CPPFLAGS"])
AC_SUBST([R_LDFLAGS],["$R_LDFLAGS"])
AC_SUBST([R_SLDFLAGS],["$R_SLDFLAGS"])

AC_MSG_RESULT([
   Configured $BUILD_VARIANT build using libraries:

     ATerm         :  $BUILD_ATERM
     Boost         :  $USE_BOOST
     CADP          :  $USE_BCG
     OpenGL        :  $USE_OPENGL
     wxWidgets     :  $USE_WX
     
   Some tools will not be built if any of these libraries are missing.
])

if test "$USE_BCG" = "no"; then
  AC_SUBST([USE_BCG],["//"])
else
  AC_SUBST([USE_BCG],[""])
fi

AC_CONFIG_FILES([build/Makefile:build/autoconf/Makefile.in
                 build/config.jam:build/autoconf/config.jam.in
                 libraries/utilities/include/mcrl2/setup.h:build/autoconf/setup.h.in])
AC_OUTPUT

