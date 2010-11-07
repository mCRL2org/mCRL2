// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/print.h
/// \brief add your file description here.


// Declare a local array NAME of type TYPE and SIZE elements (where SIZE
// is not a constant value). These will be allocated on the stack, and
// therefore any aterms put in there will be automatically protected.

#ifdef _MSC_VER
#include "malloc.h"
#define SYSTEM_SPECIFIC_ALLOCA(NAME,TYPE,SIZE)  TYPE *NAME = (TYPE *) _alloca((SIZE)*sizeof(TYPE))
#else
#define SYSTEM_SPECIFIC_ALLOCA(NAME,TYPE,SIZE)  TYPE NAME[SIZE]
#endif

