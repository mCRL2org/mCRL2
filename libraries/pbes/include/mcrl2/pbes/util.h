// Author(s): Simona Orzan.
//
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file util.h

#ifndef MCRL2_PBES_UTIL_H
#define MCRL2_PBES_UTIL_H

#include "mcrl2/new_data/data.h"


//======================================================================
bool var_in_list(mcrl2::new_data::variable vx, mcrl2::new_data::variable_list y)
//======================================================================
{
  // comparing only the variable name

 //  gsVerboseMsg("\n============= vx.name=%s, y=%s\n",pp(vx.name()).c_str(),pp(y).c_str());

  mcrl2::new_data::variable_list::iterator i = y.begin();
  for ( ; i != y.end(); i++)
    if (i->name() == vx.name()) break;
  return (i != y.end());
}



//======================================================================
mcrl2::new_data::variable_list intersect(mcrl2::new_data::variable_list x, mcrl2::new_data::variable_list y)
//======================================================================
{
  mcrl2::new_data::variable_list result;

  for (mcrl2::new_data::variable_list::iterator vx = x.begin(); vx != x.end(); vx++)
    if (var_in_list(*vx,y))
      result = push_back(result,*vx);
  return result;
}


//======================================================================
// disjoint union
 mcrl2::new_data::variable_list dunion(mcrl2::new_data::variable_list x, mcrl2::new_data::variable_list y)
//======================================================================
{
  mcrl2::new_data::variable_list result(y);
  for (mcrl2::new_data::variable_list::iterator vx = x.begin(); vx != x.end(); vx++)
    if (!var_in_list(*vx,y))
      result = push_back(result,*vx);
  return result;
}

//======================================================================
// disjoint union
 void dunion(mcrl2::new_data::variable_list* x, mcrl2::new_data::variable_list y)
//======================================================================
{
  for (mcrl2::new_data::variable_list::iterator vy = y.begin(); vy != y.end(); vy++)
    if (!var_in_list(*vy,*x))
      (*x) = push_back(*x,*vy);
}

//======================================================================
mcrl2::new_data::variable_list substract(mcrl2::new_data::variable_list x, mcrl2::new_data::variable_list y)
//======================================================================
{
  mcrl2::new_data::variable_list result;
  for (mcrl2::new_data::variable_list::iterator vx = x.begin(); vx != x.end(); vx++)
    if (!var_in_list(*vx,y))
      result = push_back(result,*vx);
  return result;
}
//======================================================================

#endif // MCRL2_PBES_UTIL_H
