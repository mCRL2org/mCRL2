// Author(s): Jan Friso Groote. Based on the aterm library by Paul Klint and others.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/aterm_int.h
/// \brief This file contains routines to create an aterm_int.

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_INT_IMPLEMENTATION_H
#define MCRL2_ATERMPP_DETAIL_ATERM_INT_IMPLEMENTATION_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/detail/aterm.h"
#include "mcrl2/atermpp/detail/aterm_int.h"
#include "mcrl2/atermpp/detail/aterm_list.h"
#include "mcrl2/atermpp/detail/aterm_implementation.h"

namespace atermpp
{

namespace detail
{

inline _aterm* aterm_int(const std::size_t val)
{
  std::size_t hnr = hash_value_aterm_int(val);

  _aterm* cur = aterm_hashtable[hnr & aterm_table_mask];
  while (cur)
  { if  (cur->function()==function_adm.AS_INT && reinterpret_cast<_aterm_int*>(cur)->value == val)
    {
      return cur;
    }
    cur = cur->next();
  }

  cur = allocate_term(TERM_SIZE_INT);
  /* Delay masking until after allocate */
  hnr &= aterm_table_mask;
  new (&const_cast<_aterm *>(cur)->function()) function_symbol(function_adm.AS_INT);
  reinterpret_cast<_aterm_int*>(const_cast<_aterm *>(cur))->value = val;

  insert_in_hashtable(cur,hnr);

  assert((hnr & aterm_table_mask) == (hash_number(cur) & aterm_table_mask));
  return cur;
}

} // namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_INT_IMPLEMENTATION_H
