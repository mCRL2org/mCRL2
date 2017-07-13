// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_CVC4_H
#define MCRL2_SMT_CVC4_H

#include "mcrl2/smt/basic_data_specification.h"

namespace mcrl2
{

namespace smt
{

class smt4_data_specification: public basic_data_specification
{
  public:
    smt4_data_specification(const data::data_specification& data_specification);

    std::string generate_data_expression(const std::map<data::variable, std::string>& declared_variables, const std::string& function_name, const data::data_expression_vector& arguments) const;
    using basic_data_specification::generate_data_expression;

    std::string generate_variable_declaration(const std::string& type_name, const std::string& variable_name) const;

    using basic_data_specification::generate_smt_problem;

  protected:
    std::string generate_assertion(const std::map<data::variable, std::string>& declared_variables, const data::data_expression& assertion) const;

    std::string generate_distinct_assertion(const std::map<data::variable, std::string>& declared_variables, const data::data_expression_list& distinct_terms) const;

    std::string generate_smt_problem(const std::string& variable_declarations, const std::string& assertions) const;

    constructed_sort_definition* create_constructed_sort(const data::sort_expression& sort, const constructed_sort_definition::constructors_t& constructors);

    function_definition* create_recursive_function_definition(const data::function_symbol& function, const data::data_equation_vector& rewrite_rules);
};

}
}

#endif
