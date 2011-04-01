// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file aterm/memory_utility.h
/// \brief This file contains a workaround that allows to
/// assign a small array of elements on the stack. For most
/// systems this can be done by:
///
///    type var[n];
///
/// for variable n, but for the MSVC compilers, the
/// special function _alloca must be used.


// Declare a local array NAME of type TYPE and SIZE elements (where SIZE
// is not a constant value). These will be allocated on the stack, and
// therefore any aterms put in there will be automatically protected.


#ifndef ATERM_MEMORY_UTILITY_H
#define ATERM_MEMORY_UTILITY_H

/* Declare a local array NAME of type TYPE and SIZE elements (where SIZE
   is not a constant value) */
#ifdef _MSC_VER
#include "malloc.h"
#define ATERM_MCRL2_SYSTEM_SPECIFIC_ALLOCA(NAME,TYPE,SIZE)  TYPE *NAME = (TYPE *) _alloca((SIZE)*sizeof(TYPE))
#else
#define ATERM_MCRL2_SYSTEM_SPECIFIC_ALLOCA(NAME,TYPE,SIZE)  TYPE *NAME = (TYPE *) alloca((SIZE)*sizeof(TYPE))
#endif

#endif

