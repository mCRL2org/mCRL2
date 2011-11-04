// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data.cpp
/// \brief

#include "mcrl2/data/print.h"

namespace mcrl2
{

namespace data
{

//--- start generated data overloads ---//
std::string pp(const sort_expression& x) { return data::pp< sort_expression >(x); }
std::string pp(const sort_expression_list& x) { return data::pp< sort_expression_list >(x); }
std::string pp(const sort_expression_vector& x) { return data::pp< sort_expression_vector >(x); }
std::string pp(const data_expression& x) { return data::pp< data_expression >(x); }
std::string pp(const data_expression_list& x) { return data::pp< data_expression_list >(x); }
std::string pp(const data_expression_vector& x) { return data::pp< data_expression_vector >(x); }
std::string pp(const assignment& x) { return data::pp< assignment >(x); }
std::string pp(const assignment_list& x) { return data::pp< assignment_list >(x); }
std::string pp(const assignment_vector& x) { return data::pp< assignment_vector >(x); }
std::string pp(const variable& x) { return data::pp< variable >(x); }
std::string pp(const variable_list& x) { return data::pp< variable_list >(x); }
std::string pp(const variable_vector& x) { return data::pp< variable_vector >(x); }
std::string pp(const function_symbol& x) { return data::pp< function_symbol >(x); }
std::string pp(const function_symbol_list& x) { return data::pp< function_symbol_list >(x); }
std::string pp(const function_symbol_vector& x) { return data::pp< function_symbol_vector >(x); }
std::string pp(const structured_sort_constructor& x) { return data::pp< structured_sort_constructor >(x); }
std::string pp(const structured_sort_constructor_list& x) { return data::pp< structured_sort_constructor_list >(x); }
std::string pp(const structured_sort_constructor_vector& x) { return data::pp< structured_sort_constructor_vector >(x); }
std::string pp(const data_equation& x) { return data::pp< data_equation >(x); }
std::string pp(const data_equation_list& x) { return data::pp< data_equation_list >(x); }
std::string pp(const data_equation_vector& x) { return data::pp< data_equation_vector >(x); }
//--- end generated data overloads ---//

// TODO: These should be removed when the ATerm code has been replaced.
std::string pp(const atermpp::aterm& x) { return x.to_string(); }
std::string pp(const atermpp::aterm_appl& x) { return x.to_string(); }

} // namespace data

} // namespace mcrl2

