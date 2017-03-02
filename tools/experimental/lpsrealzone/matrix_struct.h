// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file matrixstruct.h
/// \brief A struct representation of a 2D matrix

#ifndef MCRL2_DATA_MATRIX_STRUCT_H
#define MCRL2_DATA_MATRIX_STRUCT_H

#include "mcrl2/data/structured_sort.h"

namespace mcrl2
{
namespace data
{

class matrix_struct
{

protected:
  int m_size;
  structured_sort m_definition;
  function_symbol_vector m_projection_functions;
  function_symbol m_cons_function;

public:
  matrix_struct(int size, sort_expression element_type) :
    m_size(size)
  {
    structured_sort_constructor_argument_list arg_list;
    for(int i = m_size - 1; i >= 0; i--)
    {
      for(int j = m_size - 1; j >= 0; j--)
      {
        arg_list.push_front(structured_sort_constructor_argument("el" + std::to_string(i) + std::to_string(j), element_type));
      }
    }
    structured_sort_constructor_list cons_list;
    structured_sort_constructor cons("dbm_s", arg_list);
    cons_list.push_front(cons);
    m_definition = structured_sort(cons_list);
    m_projection_functions = function_symbol_vector(cons.projection_functions(m_definition));
    m_cons_function = cons.constructor_function(m_definition);
  }

  structured_sort get_definition()
  {
    return m_definition;
  }

  function_symbol get_projection_function(int i, int j)
  {
    return m_projection_functions[i*m_size+j];
  }

  function_symbol get_constructor()
  {
    return m_cons_function;
  }
};

} // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_MATRIX_STRUCT_H