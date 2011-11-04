// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file process.cpp
/// \brief

#include "mcrl2/process/print.h"

namespace mcrl2
{

namespace process
{

//--- start generated process overloads ---//
std::string pp(const action_name_multiset& x) { return process::pp< action_name_multiset >(x); }
std::string pp(const process_identifier& x) { return process::pp< process_identifier >(x); }
std::string pp(const process_identifier_list& x) { return process::pp< process_identifier_list >(x); }
std::string pp(const process_identifier_vector& x) { return process::pp< process_identifier_vector >(x); }
std::string pp(const process_specification& x) { return process::pp< process_specification >(x); }
std::string pp(const process_expression& x) { return process::pp< process_expression >(x); }
std::string pp(const process_expression_list& x) { return process::pp< process_expression_list >(x); }
std::string pp(const process_expression_vector& x) { return process::pp< process_expression_vector >(x); }
std::string pp(const process_instance& x) { return process::pp< process_instance >(x); }
std::string pp(const process_instance_assignment& x) { return process::pp< process_instance_assignment >(x); }
std::string pp(const delta& x) { return process::pp< delta >(x); }
std::string pp(const tau& x) { return process::pp< tau >(x); }
std::string pp(const sum& x) { return process::pp< sum >(x); }
std::string pp(const block& x) { return process::pp< block >(x); }
std::string pp(const hide& x) { return process::pp< hide >(x); }
std::string pp(const rename& x) { return process::pp< rename >(x); }
std::string pp(const comm& x) { return process::pp< comm >(x); }
std::string pp(const allow& x) { return process::pp< allow >(x); }
std::string pp(const sync& x) { return process::pp< sync >(x); }
std::string pp(const at& x) { return process::pp< at >(x); }
std::string pp(const seq& x) { return process::pp< seq >(x); }
std::string pp(const if_then& x) { return process::pp< if_then >(x); }
std::string pp(const if_then_else& x) { return process::pp< if_then_else >(x); }
std::string pp(const bounded_init& x) { return process::pp< bounded_init >(x); }
std::string pp(const merge& x) { return process::pp< merge >(x); }
std::string pp(const left_merge& x) { return process::pp< left_merge >(x); }
std::string pp(const choice& x) { return process::pp< choice >(x); }
//--- end generated process overloads ---//

// TODO: These should be removed when the ATerm code has been replaced.
std::string pp(const atermpp::aterm& x) { return x.to_string(); }
std::string pp(const atermpp::aterm_appl& x) { return x.to_string(); }

} // namespace process

} // namespace mcrl2

