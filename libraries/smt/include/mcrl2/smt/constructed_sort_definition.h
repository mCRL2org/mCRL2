// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_CONSTRUCTED_SORT_DEFINITION_H
#define MCRL2_SMT_CONSTRUCTED_SORT_DEFINITION_H

#include <vector>

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/smt/function_definition.h"
#include "mcrl2/smt/named_function_definition.h"
#include "mcrl2/smt/sort_definition.h"

namespace mcrl2
{

namespace smt
{

class constructed_sort_definition: public sort_definition
{
  public:
    struct field_t
    {
      data::function_symbol_vector projections;
    };
    struct constructor_t
    {
      data::function_symbol constructor;
      data::function_symbol_vector recognisers;
      std::vector<field_t> fields;
    };
    typedef std::vector<constructor_t> constructors_t;

  protected:
    constructors_t m_constructors;

  public:
    constructed_sort_definition(data_specification *data_specification, data::sort_expression sort, const constructors_t &constructors):
      sort_definition(data_specification, sort),
      m_constructors(constructors)
    {}

    void add_constructor_definition(std::size_t constructor_index, function_definition *definition)
    {
      m_auxiliary_function_definitions[m_constructors[constructor_index].constructor] = std::shared_ptr<function_definition>(definition);
    }

    void add_recogniser_definition(std::size_t constructor_index, function_definition *definition)
    {
      for (data::function_symbol_vector::const_iterator i = m_constructors[constructor_index].recognisers.begin(); i != m_constructors[constructor_index].recognisers.end(); i++)
      {
        m_auxiliary_function_definitions[*i] = std::shared_ptr<function_definition>(definition);
      }
    }

    void add_projection_definition(std::size_t constructor_index, std::size_t field_index, function_definition *definition)
    {
      for (data::function_symbol_vector::const_iterator i = m_constructors[constructor_index].fields[field_index].projections.begin();
           i != m_constructors[constructor_index].fields[field_index].projections.end(); i++)
      {
        m_auxiliary_function_definitions[*i] = std::shared_ptr<function_definition>(definition);
      }
    }

    virtual std::string generate_definition() const = 0;
};

}
}

#endif
