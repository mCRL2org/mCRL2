// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file struct_core.cpp

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/numeric_string.h"

using namespace mcrl2::core;

namespace mcrl2 {
  namespace core {
    namespace detail {

ATermAppl gsFreshString2ATermAppl(const char *s, ATerm Term, bool TryNoSuffix)
{
  bool found = false;
  ATermAppl NewTerm = gsString2ATermAppl(s);
  if (TryNoSuffix) {
    //try "s"
    found = !gsOccurs((ATerm) NewTerm, Term);
  }
  if (!found) {
    //find "sk" that does not occur in Term
    char *Name = (char *) malloc((strlen(s)+NrOfChars(INT_MAX)+1)*sizeof(char));
    for (int i = 0; i < INT_MAX && !found; i++) {
      sprintf(Name, "%s%d", s, i);
      NewTerm = gsString2ATermAppl(Name);
      found = !gsOccurs((ATerm) NewTerm, Term);
    }
    free(Name);
  }
  if (found) {
    return NewTerm;
  } else {
    //there is no fresh ATermAppl "si", with 0 <= i < INT_MAX
    fprintf(stderr, "error: cannot generate fresh ATermAppl with prefix %s\n", s);
    return NULL;
  }
}

   } //namespace detail
 }   //namespace core
}    //namespace mcrl2
