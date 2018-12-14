// Author(s): Jan Friso Groote, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file atermpp/source/aterm_implementation.cpp
/// \brief This file provides basic routines for garbage collection of aterms.
///        Furthermore, it contains a hooking mechanism that is used to call 
///        auxiliary functions when a term is created or destroyed. 

#include "mcrl2/atermpp/detail/aterm.h"
#include "mcrl2/atermpp/detail/global_aterm_pool.h"

using namespace atermpp;
using namespace atermpp::detail;

// Static functions defined in aterm.h
    
void atermpp::add_creation_hook(const function_symbol& function, term_callback callback)
{
  g_term_pool().add_creation_hook(function, callback);
}

void atermpp::add_deletion_hook(const function_symbol& function, term_callback callback)
{  
  g_term_pool().add_deletion_hook(function, callback);
}
