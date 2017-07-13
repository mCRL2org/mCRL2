// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_DATA_SPECIFICATION_H
#define MCRL2_SMT_DATA_SPECIFICATION_H

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/smt/smt_problem.h"

namespace mcrl2
{

namespace smt
{

/*
 * This class represents the interface to SMT data specifications.
 * Actual implementations are based on basic_data_specification augmented with solver-specific details.
 */
class data_specification
{
  public:
    virtual data::set_identifier_generator& identifier_generator() = 0;
    virtual data::data_expression representative(const data::sort_expression& sort) = 0;

    virtual bool is_constructed_sort(const data::sort_expression& sort) const = 0;
    virtual bool is_constructor(const data::function_symbol& function) const = 0;
    virtual const std::set<data::function_symbol>& constructors(const data::sort_expression& constructed_sort) const = 0;
    virtual const data::function_symbol& recogniser_function(const data::function_symbol& constructor) const = 0;
    virtual const data::function_symbol& projection_function(const data::function_symbol& constructor, const std::size_t& field_index) const = 0;

    virtual std::string generate_sort_name(const data::sort_expression& sort) const = 0;

    virtual std::string generate_data_expression(const std::map<data::variable, std::string>& declared_variables, const std::string& function_name, const data::data_expression_vector& arguments) const = 0;
    virtual std::string generate_data_expression(const std::map<data::variable, std::string>& declared_variables, const data::data_expression& expression) const = 0;

    virtual std::string generate_variable_declaration(const std::string& type_name, const std::string& variable_name) const = 0;

    virtual std::string generate_data_specification() const = 0;

    virtual std::string generate_smt_problem(const smt_problem& problem) = 0;
};

}
}

#endif
