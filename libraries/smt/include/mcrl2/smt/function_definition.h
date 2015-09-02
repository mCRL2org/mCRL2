// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_FUNCTION_DEFINITION_H
#define MCRL2_SMT_FUNCTION_DEFINITION_H

#include <set>

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/smt/data_specification.h"

namespace mcrl2
{

namespace smt
{

/*
 * A function definition describes the way an mCRL2 function symbol is translated in an SMT language.
 * It consists of a function to translate invocations of the function symbol into the SMT language;
 * furthermore, it contains an optional function definition in the SMT language that needs to be
 * included before the function is used, and optional references to other definitions that must be
 * defined before this function definition is included.
 */
class function_definition
{
  protected:
    data_specification *m_data_specification;
    std::set<data::sort_expression> m_sort_dependencies;
    std::set<data::function_symbol> m_function_dependencies;

  public:
    function_definition(data_specification *data_specification, data::sort_expression sort):
      m_data_specification(data_specification)
    {
      if (data::is_function_sort(sort))
      {
        data::function_sort function_sort(sort);
        m_sort_dependencies.insert(function_sort.codomain());
        for (data::sort_expression_list::const_iterator i = function_sort.domain().begin(); i != function_sort.domain().end(); i++)
        {
          m_sort_dependencies.insert(*i);
        }
      }
      else
      {
        m_sort_dependencies.insert(sort);
      }
    }

    function_definition(data_specification *data_specification):
      m_data_specification(data_specification)
    {}

    virtual ~function_definition()
    {}

    const std::set<data::sort_expression>& sort_dependencies() const
    {
      return m_sort_dependencies;
    }

    const std::set<data::function_symbol>& function_dependencies() const
    {
      return m_function_dependencies;
    }

    virtual std::string generate_definition() const
    {
      return "";
    }

    virtual std::string generate_data_expression(const std::map<data::variable, std::string>& declared_variables, data::data_expression_vector arguments) const = 0;
};

}
}

#endif
