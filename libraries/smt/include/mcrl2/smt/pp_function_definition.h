// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_PP_FUNCTION_DEFINITION_H
#define MCRL2_SMT_PP_FUNCTION_DEFINITION_H

#include "mcrl2/smt/function_definition.h"

namespace mcrl2
{

namespace smt
{

template<typename Printer>
class pp_function_definition: public function_definition
{
  protected:
    data::function_symbol m_function;
    Printer m_printer;

  public:
    pp_function_definition(data_specification *data_specification, data::sort_expression sort, data::function_symbol function, Printer printer):
      function_definition(data_specification, sort),
      m_function(function),
      m_printer(printer)
    {}

    virtual std::string generate_data_expression(const std::map<data::variable, std::string>& /* declared_variables */, data::data_expression_vector arguments) const
    {
      data::data_expression term;
      if (data::is_function_sort(m_function.sort()))
      {
        term = data::application(m_function, arguments);
      }
      else
      {
        term = m_function;
      }
      return m_printer(term);
    }
};


}
}

#endif
