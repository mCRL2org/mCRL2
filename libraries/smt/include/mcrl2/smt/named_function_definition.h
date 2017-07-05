// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_NAMED_FUNCTION_DEFINITION_H
#define MCRL2_SMT_NAMED_FUNCTION_DEFINITION_H

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/smt/function_definition.h"

namespace mcrl2
{

namespace smt
{

/*
 * A named function definition is a definition of a function symbol that is
 * translated into a function symbol in the SMT language.
 * This is not always the case; for example, the @cDub constructor is _not_
 * mapped to a function symbol in the SMT language, but rather translated
 * via pretty printing.
 */
class named_function_definition: public function_definition
{
  protected:
    std::string m_name;

  public:
    named_function_definition(data_specification *data_specification, data::sort_expression sort, std::string name):
      function_definition(data_specification, sort),
      m_name(name)
    {
      data_specification->identifier_generator().add_identifier(core::identifier_string(name));
    }

    named_function_definition(data_specification *data_specification, std::string name):
      function_definition(data_specification),
      m_name(name)
    {
      data_specification->identifier_generator().add_identifier(core::identifier_string(name));
    }

    named_function_definition(data_specification *data_specification, data::function_symbol function):
      function_definition(data_specification, function.sort()),
      m_name(data_specification->identifier_generator()(function.name()))
    {}

    std::string name() const
    {
      return m_name;
    }

    virtual std::string generate_data_expression(const std::map<data::variable, std::string>& declared_variables, data::data_expression_vector arguments) const
    {
      return m_data_specification->generate_data_expression(declared_variables, m_name, arguments);
    }
};

}
}

#endif
