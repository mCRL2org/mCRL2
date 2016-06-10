// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_SORT_DEFINITION_H
#define MCRL2_SMT_SORT_DEFINITION_H

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/smt/data_specification.h"
#include "mcrl2/smt/function_definition.h"

namespace mcrl2
{

namespace smt
{

/*
 * A sort definition describes the way an mCRL2 sort is translated into a sort in an SMT language.
 * It consists of a name, and a function to translate variable declarations into the SMT langauge;
 * furthermore, it contains an optional sort definition, and optional references to other sorts that
 * need to be defined before this sort definition is included.
 * Finally, it includes a set of function definitions that are generated as part of the 
 * sort definition, such as recogniser and projection functions for constructed sorts.
 */
class sort_definition
{
  protected:
    data_specification *m_data_specification;
    std::string m_name;
    std::set<data::sort_expression> m_sort_dependencies;
    std::map<data::function_symbol, std::shared_ptr<function_definition> > m_auxiliary_function_definitions;

  public:
    sort_definition(data_specification *data_specification, std::string name):
      m_data_specification(data_specification),
      m_name(name)
    {
      data_specification->identifier_generator().add_identifier(core::identifier_string(name));
    }

    sort_definition(data_specification *data_specification, data::sort_expression sort):
      m_data_specification(data_specification),
      m_name(data_specification->identifier_generator()(data::pp(sort)))
    {}

    virtual ~sort_definition()
    {}

    std::string name() const
    {
      return m_name;
    }

    std::set<data::sort_expression> sort_dependencies() const
    {
      std::set<data::sort_expression> dependencies = m_sort_dependencies;
      for (std::map<data::function_symbol, std::shared_ptr<function_definition> >::const_iterator i = m_auxiliary_function_definitions.begin(); i != m_auxiliary_function_definitions.end(); i++)
      {
        if (data::is_function_sort(i->first.sort()))
        {
          data::function_sort sort(i->first.sort());
          for (data::sort_expression_list::const_iterator j = sort.domain().begin(); j != sort.domain().end(); j++)
          {
            dependencies.insert(*j);
          }
          dependencies.insert(sort.codomain());
        }
        else
        {
          dependencies.insert(i->first.sort());
        }
      }
      return dependencies;
    }

    const std::map<data::function_symbol, std::shared_ptr<function_definition> >& auxiliary_function_definitions() const
    {
      return m_auxiliary_function_definitions;
    }

    virtual std::string generate_definition() const
    {
      return "";
    }

    virtual std::string generate_variable_declaration(const core::identifier_string& name) const
    {
      return m_data_specification->generate_variable_declaration(m_name, name);
    }
};

}
}

#endif
