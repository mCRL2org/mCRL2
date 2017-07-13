// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_BASIC_DATA_SPECIFICATION_H
#define MCRL2_SMT_BASIC_DATA_SPECIFICATION_H

#include <memory>
#include <set>

#include "mcrl2/data/bool.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/smt/constructed_sort_definition.h"
#include "mcrl2/smt/data_specification.h"
#include "mcrl2/smt/named_function_definition.h"
#include "mcrl2/smt/pp_function_definition.h"
#include "mcrl2/smt/sort_definition.h"

namespace mcrl2
{

namespace smt
{

class basic_data_specification: public data_specification
{
  protected:
    data::data_specification m_data_specification;
    std::shared_ptr<data::set_identifier_generator> m_identifier_generator;
    data::representative_generator m_representative_generator;

    std::map<data::sort_expression, std::shared_ptr<sort_definition> > m_sorts;
    std::map<data::function_symbol, std::shared_ptr<function_definition> > m_functions;

    std::map<data::sort_expression, std::set<data::function_symbol> > m_constructed_sort_constructors;
    std::map<data::function_symbol, data::function_symbol> m_recognisers;
    std::map<data::function_symbol, std::map<std::size_t, data::function_symbol> > m_projections;

    std::map<data::function_symbol, data::data_equation_vector> m_rewrite_rules;
    std::map<data::function_symbol, data::function_symbol_vector> m_all_recognisers;
    std::map<data::function_symbol, std::map<std::size_t, data::function_symbol_vector> > m_all_projections;

  public:
    basic_data_specification(const data::data_specification& data_specification, std::shared_ptr<data::set_identifier_generator> identifier_generator);

    virtual ~basic_data_specification()
    {}

    data::set_identifier_generator& identifier_generator()
    {
      return *m_identifier_generator;
    }

    virtual data::data_expression representative(const data::sort_expression& sort)
    {
      return m_representative_generator(sort);
    }

    bool is_constructed_sort(const data::sort_expression& sort) const
    {
      return m_constructed_sort_constructors.count(sort) != 0;
    }

    bool is_constructor(const data::function_symbol& function) const
    {
      data::sort_expression codomain;
      if (data::is_function_sort(function.sort()))
      {
        codomain = data::function_sort(function.sort()).codomain();
      }
      else
      {
        codomain = function.sort();
      }
      if (!is_constructed_sort(codomain))
      {
        return false;
      }
      return constructors(codomain).count(function) != 0;
    }

    const std::set<data::function_symbol>& constructors(const data::sort_expression& constructed_sort) const
    {
      assert(is_constructed_sort(constructed_sort));
      return m_constructed_sort_constructors.at(constructed_sort);
    }

    const data::function_symbol& recogniser_function(const data::function_symbol& constructor) const
    {
      assert(m_recognisers.count(constructor));
      return m_recognisers.at(constructor);
    }

    const data::function_symbol& projection_function(const data::function_symbol& constructor, const std::size_t& field_index) const
    {
      assert(m_projections.count(constructor));
      assert(m_projections.at(constructor).count(field_index));
      return m_projections.at(constructor).at(field_index);
    }

    std::string generate_sort_name(const data::sort_expression& sort) const
    {
      assert(m_sorts.count(sort));
      return m_sorts.at(sort)->name();
    }

    std::string generate_data_expression(const std::map<data::variable, std::string>& declared_variables, const data::data_expression& expression) const;

    std::string generate_data_specification() const;

    std::string generate_smt_problem(const smt_problem& problem);

  /*
   * Start of implementation-specific interface
   */
  public:
    virtual std::string generate_data_expression(const std::map<data::variable, std::string>& declared_variables, const std::string& function_name, const data::data_expression_vector& arguments) const = 0;

    virtual std::string generate_variable_declaration(const std::string& type_name, const std::string& variable_name) const = 0;

  protected:
    virtual std::string generate_assertion(const std::map<data::variable, std::string>& declared_variables, const data::data_expression& assertion) const = 0;

    virtual std::string generate_distinct_assertion(const std::map<data::variable, std::string>& declared_variables, const data::data_expression_list& distinct_terms) const;

    virtual std::string generate_smt_problem(const std::string& variable_declarations, const std::string& assertions) const = 0;

    virtual constructed_sort_definition* create_constructed_sort(const data::sort_expression& sort, const constructed_sort_definition::constructors_t &constructors) = 0;

    virtual function_definition* create_recursive_function_definition(const data::function_symbol& function, const data::data_equation_vector& rewrite_rules) = 0;
  /*
   * End of implementation-specific interface
   */

  protected:
    void find_rewrite_rules(const data::data_specification& data_specification);


    void add_sort_definition(const data::sort_expression& sort, const std::shared_ptr<sort_definition>& definition)
    {
      if (!m_sorts.count(sort) && definition)
      {
        m_sorts[sort] = definition;
        for (std::map<data::function_symbol, std::shared_ptr<function_definition> >::const_iterator i = definition->auxiliary_function_definitions().begin(); i != definition->auxiliary_function_definitions().end(); i++)
        {
          add_function_definition(i->first, i->second);
        }
      }
    }

    void add_sort_bool(const std::shared_ptr<sort_definition>& bool_definition, const std::string& true_string, const std::string& false_string)
    {
      add_sort_definition(data::sort_bool::bool_(), bool_definition);
      m_functions[data::sort_bool::true_()] = std::shared_ptr<function_definition>(new named_function_definition(this, data::sort_bool::bool_(), true_string));
      m_functions[data::sort_bool::false_()] = std::shared_ptr<function_definition>(new named_function_definition(this, data::sort_bool::bool_(), false_string));
    }

    template<typename Printer>
    void add_sort_pos(const std::shared_ptr<sort_definition>& pos_definition, Printer printer)
    {
      add_sort_definition(data::sort_pos::pos(), pos_definition);
      m_functions[data::sort_pos::c1()] = std::shared_ptr<function_definition>(new pp_function_definition<Printer>(this, data::sort_pos::pos(), data::sort_pos::c1(), printer));
      m_functions[data::sort_pos::cdub()] = std::shared_ptr<function_definition>(new pp_function_definition<Printer>(this, data::sort_pos::pos(), data::sort_pos::cdub(), printer));
    }

    template<typename Printer>
    void add_sort_nat(const std::shared_ptr<sort_definition>& nat_definition, Printer printer)
    {
      add_sort_definition(data::sort_nat::nat(), nat_definition);
      m_functions[data::sort_nat::c0()] = std::shared_ptr<function_definition>(new pp_function_definition<Printer>(this, data::sort_nat::nat(), data::sort_nat::c0(), printer));
      m_functions[data::sort_nat::cnat()] = std::shared_ptr<function_definition>(new pp_function_definition<Printer>(this, data::sort_nat::nat(), data::sort_nat::cnat(), printer));
    }

    template<typename Printer>
    void add_sort_int(const std::shared_ptr<sort_definition>& int_definition, Printer printer)
    {
      add_sort_definition(data::sort_int::int_(), int_definition);
      m_functions[data::sort_int::cint()] = std::shared_ptr<function_definition>(new pp_function_definition<Printer>(this, data::sort_int::int_(), data::sort_int::cint(), printer));
      m_functions[data::sort_int::cneg()] = std::shared_ptr<function_definition>(new pp_function_definition<Printer>(this, data::sort_int::int_(), data::sort_int::cneg(), printer));
    }

    template<typename Printer>
    void add_sort_real(const std::shared_ptr<sort_definition>& real_definition, Printer printer)
    {
      add_sort_definition(data::sort_real::real_(), real_definition);
      m_functions[data::sort_real::creal()] = std::shared_ptr<function_definition>(new pp_function_definition<Printer>(this, data::sort_real::real_(), data::sort_real::creal(), printer));
    }

    void add_constructed_sort(const data::data_specification& data_specification, const data::sort_expression& sort);
    void add_constructed_sorts(const data::data_specification& data_specification);


    void add_function_definition(const data::function_symbol& function, const std::shared_ptr<function_definition>& definition)
    {
      if (!m_functions.count(function) && definition)
      {
        m_functions[function] = definition;
      }
    }
    void add_function_definition(const data::function_symbol& function, const data::variable_vector& parameters, const data::data_expression& rhs)
    {
      data::data_expression lhs;
      if (parameters.empty())
      {
        lhs = function;
      }
      else
      {
        lhs = data::application(function, parameters);
      }
      data::data_equation equation(parameters, data::sort_bool::true_(), lhs, rhs);
      function_definition *definition = create_recursive_function_definition(function, atermpp::make_vector(equation));
      add_function_definition(function, std::shared_ptr<function_definition>(definition));
    }
    void add_function_definition(const data::function_symbol& function, const std::shared_ptr<function_definition>& definition, const data::variable_vector& parameters, const data::data_expression& rhs)
    {
      if (definition)
      {
        add_function_definition(function, definition);
      }
      else
      {
        add_function_definition(function, parameters, rhs);
      }
    }
    void add_standard_operators(const data::data_specification& data_specification,
                                const std::shared_ptr<function_definition>& equal_to,
                                const std::shared_ptr<function_definition>& not_equal_to,
                                const std::shared_ptr<function_definition>& if_);
    void add_boolean_operators(const data::data_specification& data_specification,
                               const std::shared_ptr<function_definition>& not_,
                               const std::shared_ptr<function_definition>& and_,
                               const std::shared_ptr<function_definition>& or_,
                               const std::shared_ptr<function_definition>& implies);
    void add_numerical_operators(const data::data_specification& data_specification,
                               const std::shared_ptr<function_definition>& less,
                               const std::shared_ptr<function_definition>& less_equal,
                               const std::shared_ptr<function_definition>& greater,
                               const std::shared_ptr<function_definition>& greater_equal,
                               const std::shared_ptr<function_definition>& plus,
                               const std::shared_ptr<function_definition>& minus,
                               const std::shared_ptr<function_definition>& times,
                               const std::shared_ptr<function_definition>& divides,
                               const std::shared_ptr<function_definition>& div,
                               const std::shared_ptr<function_definition>& mod,
                               const std::shared_ptr<function_definition>& floor,
                               const std::shared_ptr<function_definition>& ceil = nullptr,
                               const std::shared_ptr<function_definition>& round = nullptr,
                               const std::shared_ptr<function_definition>& unary_minus = nullptr,
                               const std::shared_ptr<function_definition>& maximum = nullptr,
                               const std::shared_ptr<function_definition>& minimum = nullptr,
                               const std::shared_ptr<function_definition>& abs = nullptr);

    void add_recursive_function(const data::function_symbol& function);
    void add_recursive_functions(const data::data_specification& data_specification);
};

}
}

#endif
