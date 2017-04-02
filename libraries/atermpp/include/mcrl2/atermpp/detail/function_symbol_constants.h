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

struct constant_function_symbols
{
  public:
    function_symbol AS_DEFAULT;
    function_symbol AS_INT;
    function_symbol AS_LIST;
    function_symbol AS_EMPTY_LIST;

    constant_function_symbols()
     : AS_DEFAULT("<undefined_term>", 0),
       AS_INT("<aterm_int>", 1),
       AS_LIST("<list_constructor>",2),
       AS_EMPTY_LIST("<empty_list>", 0)
    {
      // Also set the default function symbol in the function_symbol class. 
      new (&function_symbol::AS_DEFAULT) function_symbol(AS_DEFAULT);
    }

    // We do not know whether the compiler constructs the 
    // function symbols first, or whether the function initialise
    // is called before the constructor. Therefore, we use placement
    // new as we do not know whether the memory where the elements
    // are constructed is propertly initialised. This means that the
    // reference count of these terms can be too high, and these terms
    // will not property be destroyed, but that has no consequence on
    // the correctness of the program.
    void initialise_function_symbols()
    {
      new (&AS_DEFAULT) function_symbol("<undefined_term>", 0); 
      new (&AS_INT) function_symbol("<aterm_int>", 1); 
      new (&AS_LIST) function_symbol("<list_constructor>", 2); 
      new (&AS_EMPTY_LIST) function_symbol("<empty_list>", 0); 

      // Separately copy the default function symbol to the function_symbol class.
      new (&function_symbol::AS_DEFAULT) function_symbol(AS_DEFAULT);
    } 
};


extern constant_function_symbols function_adm;

} // namespace detail
} // namespace atermpp

#endif // ATERMPP_DETAIL_FUNCTION_SYMBOL_CONSTANTS_H

