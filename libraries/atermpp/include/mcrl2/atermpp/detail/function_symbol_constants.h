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
    static const function_symbol& AS_INT()
    {
      static function_symbol f("<aterm_int>",1);
      return f;
    }

    static const function_symbol& AS_LIST()
    {
      static function_symbol f("<list_constructor>",2);
      return f;
    }

    static const function_symbol& AS_EMPTY_LIST()
    {
      static function_symbol f("<empty_list>",0);
      return f;
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

