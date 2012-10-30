// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// 
/// \file libraries/atermpp/source/indexed_set.cpp
/// \brief A data structure for a set of aterms, where each
///        element in the set get a unique number. Useful
///        for numbering for instance the set of states
///        in a labelled transition system.


#include <stdexcept>
#include <stack>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/table.h"
#include "mcrl2/atermpp/detail/indexed_set.h"


namespace atermpp
{

void table::clear()
{
  indexed_set::clear();
  m_values.clear();
}


void table::put(const aterm &key, const aterm &value)
{
  /* insert entry key into the hashtable, and deliver
     an index. If key is already in the set, deliver 0 */

  std::pair<size_t, bool> p = indexed_set::put(key);
  detail::insertKeyOrValue(m_values,p.first,value);
}


const aterm &table::get(const aterm &key) const
{
  const size_t v = index(key);
  if (v==atermpp::npos)
  {
    return detail::static_undefined_aterm;
  }
  return detail::tableGet(m_values, v);
}


bool table::erase(const aterm &key)
{
  const bool removed=indexed_set::erase(key);
  // At the top of the stack is the freed position with the key, 
  // of which the value must still be removed.
  if (removed) 
  { 
    detail::insertKeyOrValue(m_values, free_positions.top(), aterm());
  }
  return removed;
}


aterm_list table::values() const
{
  return detail::tableContent(m_values, m_size+free_positions.size());
}

} // namespace atermpp
