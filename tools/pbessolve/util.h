


//  Copyright 2007 Simona Orzan. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./util.h

//======================================================================
bool var_in_list(data_variable vx, data_variable_list y)
//======================================================================
{
  // comparing only the variable name
  
 //  gsVerboseMsg("\n============= vx.name=%s, y=%s\n",pp(vx.name()).c_str(),pp(y).c_str());
  
  data_variable_list::iterator i = y.begin();
  for ( ; i != y.end(); i++)
    if (i->name() == vx.name()) break;
  return (i != y.end());
}



//======================================================================
data_variable_list intersect(data_variable_list x, data_variable_list y)
//======================================================================
{
  data_variable_list result;

  for (data_variable_list::iterator vx = x.begin(); vx != x.end(); vx++)
    if (var_in_list(*vx,y))
      result = push_back(result,*vx);
  return result;
}


//======================================================================
// disjoint union
 data_variable_list dunion(data_variable_list x, data_variable_list y)
//======================================================================
{
  data_variable_list result(y);
  for (data_variable_list::iterator vx = x.begin(); vx != x.end(); vx++)
    if (!var_in_list(*vx,y))
      result = push_back(result,*vx);    
  return result;
}

//======================================================================
// disjoint union
 void dunion(data_variable_list* x, data_variable_list y)
//======================================================================
{
  for (data_variable_list::iterator vy = y.begin(); vy != y.end(); vy++)
    if (!var_in_list(*vy,*x))
      (*x) = push_back(*x,*vy);    
}

//======================================================================
data_variable_list substract(data_variable_list x, data_variable_list y)
//======================================================================
{
  data_variable_list result;
  for (data_variable_list::iterator vx = x.begin(); vx != x.end(); vx++)
    if (!var_in_list(*vx,y))
      result = push_back(result,*vx);
  return result;
}
//======================================================================

