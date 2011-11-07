// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps.cpp
/// \brief

#include "mcrl2/lps/print.h"
#include "mcrl2/modal_formula/print.h"

namespace mcrl2
{

namespace lps
{

//--- start generated lps overloads ---//
std::string pp(const specification& x) { return lps::pp< specification >(x); }
std::string pp(const linear_process& x) { return lps::pp< linear_process >(x); }
std::string pp(const action& x) { return lps::pp< action >(x); }
std::string pp(const action_list& x) { return lps::pp< action_list >(x); }
std::string pp(const action_vector& x) { return lps::pp< action_vector >(x); }
std::string pp(const action_label& x) { return lps::pp< action_label >(x); }
std::string pp(const action_label_list& x) { return lps::pp< action_label_list >(x); }
std::string pp(const action_label_vector& x) { return lps::pp< action_label_vector >(x); }
std::string pp(const multi_action& x) { return lps::pp< multi_action >(x); }
std::string pp(const process_initializer& x) { return lps::pp< process_initializer >(x); }
//--- end generated lps overloads ---//

std::string pp_with_summand_numbers(const specification& x)
{
  std::ostringstream out;
  core::detail::apply_printer<lps::detail::printer> printer(out);
  printer.print_summand_numbers() = true;
  printer(x);
  return out.str();
}

// TODO: These should be removed when the ATerm code has been replaced.
std::string pp(const atermpp::aterm& x) { return x.to_string(); }
std::string pp(const atermpp::aterm_appl& x) { return x.to_string(); }

} // namespace lps

namespace action_formulas
{

//--- start generated action_formulas overloads ---//
std::string pp(const action_formula& x) { return action_formulas::pp< action_formula >(x); }
//--- end generated action_formulas overloads ---//

} // namespace action_formulas

namespace regular_formulas
{

//--- start generated regular_formulas overloads ---//
std::string pp(const regular_formula& x) { return regular_formulas::pp< regular_formula >(x); }
//--- end generated regular_formulas overloads ---//

} // namespace regular_formulas

namespace state_formulas
{

//--- start generated state_formulas overloads ---//
std::string pp(const state_formula& x) { return state_formulas::pp< state_formula >(x); }
//--- end generated state_formulas overloads ---//

} // namespace state_formulas

} // namespace mcrl2

