// Author(s): Jan Friso Groote, based on the aterm library.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/function_symbol.h
/// \brief Function symbol class.

#ifndef ATERMPP_DETAIL_FUNCTION_SYMBOL_CONSTANTS_H
#define ATERMPP_DETAIL_FUNCTION_SYMBOL_CONSTANTS_H

#include "mcrl2/atermpp/function_symbol.h"


namespace atermpp
{
namespace detail
{

struct constant_function_symbols
{
  public:
    atermpp::function_symbol AS_DEFAULT;
    atermpp::function_symbol AS_INT;
    atermpp::function_symbol AS_LIST;
    atermpp::function_symbol AS_EMPTY_LIST;

    // The function symbols below intentionally contain spaces, such that they are not
    // confused with function symbols that are used by applications of the aterms.
    constant_function_symbols():
      AS_DEFAULT("<undefined term>", 0),
      AS_INT("<aterm int>", 1),
      AS_LIST("<list constructor>", 2),
      AS_EMPTY_LIST("<empty list>", 0)   
    {} 

    // This function is used to explicitly initialise
    // the default constant function symbols, in case 
    // they are being used before being properly initialised.
    // This happens with initialising global variables and 
    // global and sometimes even with static variables in
    // functions, but this is compiler dependent.
    void initialise_function_symbols()
    {
      new (&AS_DEFAULT) function_symbol("<undefined term>", 0);
      new (&AS_INT) function_symbol("<aterm int>", 1);
      new (&AS_LIST) function_symbol("<list constructor>", 2);
      new (&AS_EMPTY_LIST) function_symbol("<empty list>", 0);
      // The following numbers are expected to be used. If not
      // something is most likely wrong. Moreover, some code
      // depends on low numbers to be assigned to the basic 
      // function symbols (e.g. type_is_appl).
      assert(AS_DEFAULT.number()==0);
      assert(AS_INT.number()==1);
      assert(AS_LIST.number()==2);
      assert(AS_EMPTY_LIST.number()==3);
    }
};


extern constant_function_symbols function_adm;

} // namespace detail
} // namespace atermpp

#endif // ATERMPP_DETAIL_FUNCTION_SYMBOL_CONSTANTS_H

