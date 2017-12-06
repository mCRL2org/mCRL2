// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_IDENTITY_FUNCTION_DEFINITION_H
#define MCRL2_SMT_IDENTITY_FUNCTION_DEFINITION_H

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/smt/function_definition.h"

namespace mcrl2
{
namespace smt
{

/*
 * An identity function definition is used for
 * functions such as @cNat and @cInt. They can
 * be ignored when generating an SMT expression.
 */
class identity_function_definition: public function_definition
{
  public:
    identity_function_definition(data_specification *data_specification):
      function_definition(data_specification)
    {}

    virtual std::string generate_data_expression(const std::map<data::variable, std::string>& declared_variables, data::data_expression_vector arguments) const
    {
      assert(arguments.size() == 1);
      return m_data_specification->generate_data_expression(declared_variables, arguments[0]);
    }
};

}
}

#endif // MCRL2_SMT_IDENTITY_FUNCTION_DEFINITION_H
