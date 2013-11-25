// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bes.cpp
/// \brief

#include "mcrl2/bes/print.h"

namespace mcrl2
{

namespace bes
{

//--- start generated bes overloads ---//
std::string pp(const bes::and_& x) { return bes::pp< bes::and_ >(x); }
std::string pp(const bes::boolean_equation& x) { return bes::pp< bes::boolean_equation >(x); }
std::string pp(const bes::boolean_equation_system& x) { return bes::pp< bes::boolean_equation_system >(x); }
std::string pp(const bes::boolean_expression& x) { return bes::pp< bes::boolean_expression >(x); }
std::string pp(const bes::boolean_variable& x) { return bes::pp< bes::boolean_variable >(x); }
std::string pp(const bes::false_& x) { return bes::pp< bes::false_ >(x); }
std::string pp(const bes::imp& x) { return bes::pp< bes::imp >(x); }
std::string pp(const bes::not_& x) { return bes::pp< bes::not_ >(x); }
std::string pp(const bes::or_& x) { return bes::pp< bes::or_ >(x); }
std::string pp(const bes::true_& x) { return bes::pp< bes::true_ >(x); }
//--- end generated bes overloads ---//

#ifdef MCRL2_USE_INDEX_TRAITS
static bool register_hooks()
{
  register_boolean_variable_hooks();
  return true;
}
static bool mcrl2_register_bes(register_hooks());
#endif

} // namespace bes

} // namespace mcrl2
