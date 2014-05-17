// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// 
/// \file atermpp/detail/indexed_set.cpp
/// \brief This file contains some constants and functions shared
///        between indexed_sets and tables.

#ifndef MCRL2_ATERMPP_DETAIL_INDEXED_SET_H
#define MCRL2_ATERMPP_DETAIL_INDEXED_SET_H

#include <cstdlib>
#include <vector>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_list.h"

namespace atermpp
{
namespace detail
{

static const size_t TABLE_SHIFT = 14;
static const size_t ELEMENTS_PER_TABLE = 1L<<TABLE_SHIFT;
inline
size_t modELEMENTS_PER_TABLE(const size_t n)
{
  return (n & (ELEMENTS_PER_TABLE-1));
}

inline
size_t divELEMENTS_PER_TABLE(const size_t n)
{
  return n >> TABLE_SHIFT;
}

/*-----------------------------------------------------------*/


inline const aterm &tableGet(const std::vector< std::vector<aterm> > &tableindex, size_t n)
{
  return tableindex[divELEMENTS_PER_TABLE(n)][modELEMENTS_PER_TABLE(n)];
}

inline void insertKeyOrValue(std::vector<std::vector<aterm> >  &vec, size_t n, const aterm &t)
{
  const size_t x = divELEMENTS_PER_TABLE(n);
  const size_t y = modELEMENTS_PER_TABLE(n);


  if (x>=vec.size())
  {
    vec.push_back(std::vector<aterm>(ELEMENTS_PER_TABLE));
  }

  vec[x][y] = t;
}

inline aterm_list tableContent(const std::vector< std::vector<aterm> > &tableindex,size_t nr_entries)
{
  size_t i;
  aterm t;
  aterm_list result;

  for (i=nr_entries; i>0; i--)
  {
    t = tableGet(tableindex, i-1);
    if (t.defined())
    {
      result.push_front(t);
    }
  }
  return result;
}

} // namespace detail
} // namespace atermpp 

#endif // MCRL2_ATERMPP_DETAIL_INDEXED_SET_H
