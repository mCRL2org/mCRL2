// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes.cpp
/// \brief

#include "mcrl2/pbes/print.h"

namespace mcrl2
{

namespace pbes_system
{

//--- start generated pbes_system overloads ---//
std::string pp(const fixpoint_symbol& x) { return pbes_system::pp< fixpoint_symbol >(x); }
std::string pp(const pbes<>& x) { return pbes_system::pp< pbes<> >(x); }
std::string pp(const pbes_equation& x) { return pbes_system::pp< pbes_equation >(x); }
std::string pp(const atermpp::vector<pbes_equation>& x) { return pbes_system::pp< atermpp::vector<pbes_equation> >(x); }
std::string pp(const pbes_expression& x) { return pbes_system::pp< pbes_expression >(x); }
std::string pp(const pbes_expression_list& x) { return pbes_system::pp< pbes_expression_list >(x); }
std::string pp(const pbes_expression_vector& x) { return pbes_system::pp< pbes_expression_vector >(x); }
std::string pp(const propositional_variable& x) { return pbes_system::pp< propositional_variable >(x); }
std::string pp(const propositional_variable_list& x) { return pbes_system::pp< propositional_variable_list >(x); }
std::string pp(const propositional_variable_vector& x) { return pbes_system::pp< propositional_variable_vector >(x); }
std::string pp(const propositional_variable_instantiation& x) { return pbes_system::pp< propositional_variable_instantiation >(x); }
std::string pp(const propositional_variable_instantiation_list& x) { return pbes_system::pp< propositional_variable_instantiation_list >(x); }
std::string pp(const propositional_variable_instantiation_vector& x) { return pbes_system::pp< propositional_variable_instantiation_vector >(x); }
//--- end generated pbes_system overloads ---//

// TODO: These should be removed when the ATerm code has been replaced.
std::string pp(const atermpp::aterm& x) { return x.to_string(); }
std::string pp(const atermpp::aterm_appl& x) { return x.to_string(); }
std::string pp(const core::identifier_string& x) { return core::pp(x); }

} // namespace pbes_system

} // namespace mcrl2

