// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_RECURSIVE_FUNCTION_DEFINITION_H
#define MCRL2_SMT_RECURSIVE_FUNCTION_DEFINITION_H

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/smt/named_function_definition.h"

namespace mcrl2
{

namespace smt
{

/*
 * A recursive function definition is a function defined by a
 * (hopefully, complete and non-redundant) set of rewrite rules, which may be recursive.
 */
class recursive_function_definition: public named_function_definition
{
  protected:
    data::variable_vector m_parameters;
    data::data_expression m_rhs;

  public:
    recursive_function_definition(data_specification *data_specification, data::function_symbol function, const data::data_equation_vector& rewrite_rules);

    virtual std::string generate_definition() const = 0;

    static bool is_pattern_matching_rule(data_specification *data_specification, data::data_equation rewrite_rule);
};

}
}

#endif
