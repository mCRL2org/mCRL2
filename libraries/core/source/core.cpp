// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file core.cpp

#include "mcrl2/atermpp/detail/utility.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/print.h"

namespace mcrl2 {

namespace core {

namespace detail {

atermpp::function_symbol function_symbol_OpId_ = atermpp::function_symbol("OpId", 3);
atermpp::function_symbol function_symbol_DataVarId_ = atermpp::function_symbol("DataVarId", 3);
std::vector<atermpp::function_symbol> function_symbols_DataAppl_;

} // namespace detail

//--- start generated core overloads ---//
std::string pp(const core::identifier_string& x) { return core::pp< core::identifier_string >(x); }
std::string pp(const core::nil& x) { return core::pp< core::nil >(x); }
//--- end generated core overloads ---//

std::string pp(const atermpp::aterm& x) { return to_string(x); }
std::string pp(const atermpp::aterm_appl& x) { return to_string(x); }

} // namespace core

} // namespace mcrl2
