AC_PREREQ(2.57)
AC_INIT([GenSpect parser], [0.1.14], [A.H.J.Mathijssen@tue.nl])
AC_PREFIX_DEFAULT([$HOME])
AC_CONFIG_SRCDIR([gslexer.l])

# Checks for programs.
AC_PROG_CC
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
LDFLAGS="-L. -lm $LDFLAGS"

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

# Checks for libraries
AC_CHECK_LIB([ATerm], [ATinit], , [AC_MSG_ERROR([cannot find ATerm library])])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h string.h])
AC_CHECK_HEADERS([aterm2.h], , [AC_MSG_ERROR([cannot find ATerm headers])])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strdup])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
