AC_INIT([mCRL2 toolset], [0.2], [A.H.J.Mathijssen@tue.nl])
AC_PREFIX_DEFAULT([$HOME])
AC_CONFIG_SRCDIR([gslowlevel.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX

# Check for compatible bison
AC_CHECK_PROG([BISON], [bison], [bison], [])
if test -z $BISON; then
  AC_MSG_ERROR([cannot find bison])
fi
AC_MSG_CHECKING([whether bison option %glr-parser is available])
if $BISON test.y -o/dev/null &> /dev/null; then
  AC_MSG_RESULT([yes]);
else
  AC_MSG_ERROR([bison option %glr-parser not available]);
fi

# Set flags
CFLAGS="$CFLAGS -std=c99 -Wall -pedantic"
CXXFLAGS="$CXXFLAGS -Wall"
LDFLAGS="-L. $LDFLAGS"

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
  AC_MSG_ERROR([C99 standard not supported])
)

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
    CPPFLAGS="-I$ATERM_INCLUDE $CPPFLAGS"
    LDFLAGS="-L$ATERM_LIB $LDFLAGS"
  fi
fi
# debug: AC_MSG_RESULT([CPPFLAGS=$CPPFLAGS])
# debug: AC_MSG_RESULT([LDFLAGS=$LDFLAGS])
AC_CHECK_LIB([ATerm], [ATinit], , [AC_MSG_ERROR([cannot find ATerm library])])
AC_CHECK_HEADERS([aterm2.h], , [AC_MSG_ERROR([cannot find ATerm headers])])

# Check for xml2 library and header files
AC_ARG_WITH(
  [xml2],
  AC_HELP_STRING(
    [--with-xml2-header=dir],
    [use xml2 headers from dir/libxml; defaults to /usr/include/libxml2]
  )  
)
if test $with_xml2_header; then
  xml_h_dir=$with_xml2_header
else
  xml_h_dir=/usr/include/libxml2
fi
CPPFLAGS="-I$xml_h_dir $CPPFLAGS"
AC_CHECK_LIB([xml2], [xmlParseFile], [have_libxml2=1])
AC_CHECK_HEADERS([libxml/parser.h], [have_headerxml2=1])
if test $have_libxml2; then
  if test $have_headerxml2; then
    USE_XML2=1
  fi
fi
AC_SUBST([USE_XML2],[])

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
  CPPFLAGS="$($WX_CONFIG --cppflags) $CPPFLAGS"
  WX_LIBS="$($WX_CONFIG --libs)"
fi
AC_SUBST([USE_WX],[])
AC_SUBST([WX_LIBS],[])

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

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
