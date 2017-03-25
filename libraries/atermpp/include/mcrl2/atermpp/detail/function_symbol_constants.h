// Author(s): Jan Friso Groote, based on the aterm library.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/function_symbol_constants.h
/// \brief A class containing some constant function symbols. 

#ifndef ATERMPP_DETAIL_FUNCTION_SYMBOL_CONSTANTS_H
#define ATERMPP_DETAIL_FUNCTION_SYMBOL_CONSTANTS_H

#include "mcrl2/atermpp/function_symbol.h"


namespace atermpp
{
namespace detail
{
  const size_t AS_DEFAULT_NUMBER=0;
  const size_t AS_INT_NUMBER=1;
  const size_t AS_LIST_NUMBER=2;
  const size_t AS_EMPTY_LIST_NUMBER=3;


struct constant_function_symbols
{

  public:
    atermpp::function_symbol AS_DEFAULT;
    atermpp::function_symbol AS_INT;
    atermpp::function_symbol AS_LIST;
    atermpp::function_symbol AS_EMPTY_LIST;

    constant_function_symbols():
       AS_DEFAULT("<undefined_term>",0),
       AS_INT("<aterm_int>",1),
       AS_LIST("<list_constructor>",2),
       AS_EMPTY_LIST("<empty_list>",0)
    {
      /* assert(AS_DEFAULT.number()==AS_DEFAULT_NUMBER);
      assert(AS_INT.number()==AS_INT_NUMBER);
      assert(AS_LIST.number()==AS_LIST_NUMBER);
      assert(AS_EMPTY_LIST.number()==AS_EMPTY_LIST_NUMBER); */
    } 


    // This function is used to explicitly initialise
    // the default constant function symbols, in case 
    // they are being used before being properly initialised.
    // This happens with initialising global variables and 
    // global and sometimes even with static variables in
    // functions, but this is compiler dependent.
    void initialise_function_symbols()
    {
      new (&AS_DEFAULT) function_symbol("<undefined_term>", 0);
      new (&AS_INT) function_symbol("<aterm_int>", 1);
      new (&AS_LIST) function_symbol("<list_constructor>", 2);
      new (&AS_EMPTY_LIST) function_symbol("<empty_list>", 0);
      // The following numbers are expected to be used. If not
      // something is most likely wrong. Moreover, some code
      // depends on low numbers to be assigned to the basic 
      // function symbols (e.g. type_is_appl).
      /* assert(AS_DEFAULT.number()==AS_DEFAULT_NUMBER);
      assert(AS_INT.number()==AS_INT_NUMBER);
      assert(AS_LIST.number()==AS_LIST_NUMBER);
      assert(AS_EMPTY_LIST.number()==AS_EMPTY_LIST_NUMBER); */
    }
};


extern constant_function_symbols function_adm;

inline size_t addressf(const function_symbol& t)
{
  return reinterpret_cast<size_t>(t.m_function_symbol);
}

} // namespace detail
} // namespace atermpp

#endif // ATERMPP_DETAIL_FUNCTION_SYMBOL_CONSTANTS_H

