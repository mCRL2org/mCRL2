// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file atermpp/source/function_symbol.cpp
/// \brief This file provides the data structures for the function symbols occurring in 
///        the atermpp library. 

#include "mcrl2/atermpp/detail/function_symbol_pool.h"

#include "mcrl2/atermpp/detail/global_aterm_pool.h"

using namespace atermpp;

global_function_symbol detail::g_as_int("<aterm_int>", 0);
global_function_symbol detail::g_as_list("<list_constructor>", 2);
global_function_symbol detail::g_as_empty_list("<empty_list>", 0);

function_symbol::function_symbol(const std::string& name, const std::size_t arity, const bool check_for_registered_functions) :
  function_symbol(detail::g_term_pool().get_symbol_pool().create(name, arity, check_for_registered_functions))
{}

global_function_symbol::global_function_symbol(const std::string& name, const std::size_t arity) :
  function_symbol(detail::g_term_pool<true>().get_symbol_pool().create(name, arity, true))
{}

void function_symbol::destroy()
{
  detail::g_term_pool().get_symbol_pool().destroy(m_function_symbol.get());
}

