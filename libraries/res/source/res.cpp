// Author(s): Jan Friso Groote. Based on bes/bes.cpp by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file res.cpp
/// \brief

#include "mcrl2/res/find.h"
#include "mcrl2/res/print.h"

namespace mcrl2
{

namespace res
{

//--- start generated res overloads ---//
std::string pp(const res::and_& x) { return res::pp< res::and_ >(x); }
std::string pp(const res::res_equation& x) { return res::pp< res::res_equation >(x); }
std::string pp(const res::res_equation_system& x) { return res::pp< res::res_equation_system >(x); }
std::string pp(const res::res_expression& x) { return res::pp< res::res_expression >(x); }
std::string pp(const res::res_variable& x) { return res::pp< res::res_variable >(x); }
std::string pp(const res::false_& x) { return res::pp< res::false_ >(x); }
std::string pp(const res::imp& x) { return res::pp< res::imp >(x); }
std::string pp(const res::minus& x) { return res::pp< res::minus >(x); }
std::string pp(const res::or_& x) { return res::pp< res::or_ >(x); }
std::string pp(const res::true_& x) { return res::pp< res::true_ >(x); }
//--- end generated res overloads ---//

std::set<res_variable> res_equation_system::occurring_variables() const
{
  std::set<res_variable> result;
  for (const res_equation& eqn: m_equations)
  {
    find_res_variables(eqn.formula(), std::inserter(result, result.end()));
  }
  find_res_variables(m_initial_state, std::inserter(result, result.end()));
  return result;
}

} // namespace res

} // namespace mcrl2
