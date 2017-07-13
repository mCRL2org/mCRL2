// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/data/int.h"
#include "mcrl2/data/real.h"
#include "mcrl2/smt/cvc4.h"
#include "mcrl2/smt/recursive_function_definition.h"

namespace mcrl2
{

namespace smt
{

class cvc4_identifier_generator : public data::set_identifier_generator
{
  public:
    virtual core::identifier_string operator()(const std::string& hint, bool add_to_context = true)
    {
      std::string name = hint;

      if (name == "-") name = "minus";
      if (name == "[]") name = "emptylist";
      if (name == "|>") name = "cons";
      if (name == "<|") name = "snoc";
      if (name == "#") name = "count";
      if (name == "++") name = "concat";
      if (name == ".") name = "at";

      for (std::size_t i = 0; i < name.size(); i++)
      {
        if (!(
          ('0' <= name[i] && name[i] <= '9') ||
          ('a' <= name[i] && name[i] <= 'z') ||
          ('A' <= name[i] && name[i] <= 'Z')))
        {
          name[i] = '_';
        }
      }

      return data::set_identifier_generator::operator()(name, add_to_context);
    }
};

class pos_sort_definition : public sort_definition
{
  public:
    pos_sort_definition(data_specification *data_specification):
      sort_definition(data_specification, "Int")
    {}

    std::string generate_variable_declaration(const core::identifier_string& name) const
    {
      return sort_definition::generate_variable_declaration(name) + "(assert (>= " + (std::string)name + " 1))\n";
    }
};

class nat_sort_definition : public sort_definition
{
  public:
    nat_sort_definition(data_specification *data_specification):
      sort_definition(data_specification, "Int")
    {}

    std::string generate_variable_declaration(const core::identifier_string& name) const
    {
      return sort_definition::generate_variable_declaration(name) + "(assert (>= " + (std::string)name + " 0))\n";
    }
};

static std::string print_nat(const data::data_expression& number)
{
  return data::pp(number);
}

static std::string print_int(const data::data_expression& number)
{
  assert(data::sort_int::is_cint_application(number) || data::sort_int::is_cneg_application(number));
  if (data::sort_int::is_cneg_application(number))
  {
    data::application application(number);
    assert(application.size() == 1);
    return "(- " + data::pp(application[0]) + ")";
  }
  else
  {
    return data::pp(number);
  }
}

static std::string print_real(const data::data_expression& number)
{
  assert(data::sort_real::is_creal_application(number));
  data::application application(number);
  std::string numerator = print_int(application[0]);
  std::string denominator = data::pp(application[1]);
  return "(/ " + numerator + " " + denominator + ")";
}

static std::shared_ptr<function_definition> make_operator(data_specification* data_specification, const std::string& name)
{
  return std::shared_ptr<function_definition>(new named_function_definition(data_specification, name));
}

smt4_data_specification::smt4_data_specification(const data::data_specification& data_specification):
  basic_data_specification(data_specification, std::shared_ptr<data::set_identifier_generator>(new cvc4_identifier_generator))
{
  add_sort_bool(std::shared_ptr<sort_definition>(new sort_definition(this, "Bool")), "true", "false");
  add_sort_pos(std::shared_ptr<sort_definition>(new pos_sort_definition(this)), print_nat);
  add_sort_nat(std::shared_ptr<sort_definition>(new nat_sort_definition(this)), print_nat);
  add_sort_int(std::shared_ptr<sort_definition>(new sort_definition(this, "Int")), print_int);
  add_sort_real(std::shared_ptr<sort_definition>(new sort_definition(this, "Real")), print_real);
  add_constructed_sorts(data_specification);

  add_standard_operators(
    data_specification,
    make_operator(this, "="),
    make_operator(this, "distinct"),
    make_operator(this, "ite")
  );
  add_boolean_operators(
    data_specification,
    make_operator(this, "not"),
    make_operator(this, "and"),
    make_operator(this, "or"),
    make_operator(this, "=>")
  );
  add_numerical_operators(
    data_specification,
    make_operator(this, "<"),
    make_operator(this, "<="),
    make_operator(this, ">"),
    make_operator(this, ">="),
    make_operator(this, "+"),
    make_operator(this, "-"),
    make_operator(this, "*"),
    make_operator(this, "/"),
    make_operator(this, "div"),
    make_operator(this, "mod"),
    make_operator(this, "to_int"),
    nullptr, // ceil
    nullptr, // round
    make_operator(this, "-"),
    nullptr, // maximum
    nullptr, // minimum
    make_operator(this, "abs")
  );
  add_recursive_functions(data_specification);
}

std::string smt4_data_specification::generate_data_expression(const std::map<data::variable, std::string>& declared_variables, const std::string& function_name, const data::data_expression_vector& arguments) const
{
  if (arguments.empty())
  {
    return function_name;
  }
  else
  {
    std::string output;
    output += "(";
    output += function_name;
    for (data::data_expression_vector::const_iterator i = arguments.begin(); i != arguments.end(); i++)
    {
      output += " ";
      output += generate_data_expression(declared_variables, *i);
    }
    output += ")";
    return output;
  }
}

std::string smt4_data_specification::generate_variable_declaration(const std::string& type_name, const std::string& variable_name) const
{
  return "(declare-fun " + variable_name + " () " + type_name + ")\n";
}

std::string smt4_data_specification::generate_assertion(const std::map<data::variable, std::string>& declared_variables, const data::data_expression& assertion) const
{
  return "(assert " + generate_data_expression(declared_variables, assertion) + ")\n";
}

std::string smt4_data_specification::generate_distinct_assertion(const std::map<data::variable, std::string>& declared_variables, const data::data_expression_list& distinct_terms) const
{
  std::string output = "(distinct";
  for (data::data_expression_list::const_iterator i = distinct_terms.begin(); i != distinct_terms.end(); i++)
  {
    output += " ";
    output += generate_data_expression(declared_variables, *i);
  }
  output += ")\n";
  return output;
}

std::string smt4_data_specification::generate_smt_problem(const std::string& variable_declarations, const std::string& assertions) const
{
  return variable_declarations + assertions + "(check-sat)\n";
}

class cvc4_constructed_sort_definition : public constructed_sort_definition
{
  protected:
    std::map<data::function_symbol, std::string> m_constructor_names;
    std::map<data::function_symbol, std::vector<std::string> > m_field_names;

  public:
    cvc4_constructed_sort_definition(data_specification *data_specification, data::sort_expression sort, const constructors_t &constructors):
      constructed_sort_definition(data_specification, sort, constructors)
    {
      for (std::size_t i = 0; i < m_constructors.size(); i++)
      {
        std::string name = data_specification->identifier_generator()(m_constructors[i].constructor.name());
        std::string recogniser_name = "is-" + name;
        data_specification->identifier_generator().add_identifier(recogniser_name);

        m_constructor_names[m_constructors[i].constructor] = name;
        add_constructor_definition(i, new named_function_definition(data_specification, m_constructors[i].constructor.sort(), name));
        add_recogniser_definition(i, new named_function_definition(data_specification, recogniser_name));

        for (std::size_t j = 0; j < m_constructors[i].fields.size(); j++)
        {
          std::string field_name = data_specification->identifier_generator()(m_constructors[i].fields[j].projections[0].name());
          m_field_names[m_constructors[i].constructor].push_back(field_name);
          add_projection_definition(i, j, new named_function_definition(data_specification, field_name));
        }
      }
    }

    std::string generate_definition() const
    {
      std::string output = "(declare-datatypes () ( (" + m_name + "\n";
      for (std::size_t i = 0; i < m_constructors.size(); i++)
      {
        output += "  (" + m_constructor_names.at(m_constructors[i].constructor);
        for (std::size_t j = 0; j < m_constructors[i].fields.size(); j++)
        {
          assert(data::is_function_sort(m_constructors[i].fields[j].projections[0].sort()));
          std::string sort_name = m_data_specification->generate_sort_name(data::function_sort(m_constructors[i].fields[j].projections[0].sort()).codomain());
          output += " (" + m_field_names.at(m_constructors[i].constructor)[j] + " " + sort_name + ")";
        }
        output += ")\n";
      }
      output += ")))\n";
      return output;
    }
};

constructed_sort_definition *smt4_data_specification::create_constructed_sort(const data::sort_expression& sort, const constructed_sort_definition::constructors_t &constructors)
{
  return new cvc4_constructed_sort_definition(this, sort, constructors);
}

class cvc4_recursive_function_definition : public recursive_function_definition
{
  protected:
    data::sort_expression m_codomain;

  public:
    cvc4_recursive_function_definition(data_specification *data_specification, data::function_symbol function, const data::data_equation_vector& rewrite_rules):
      recursive_function_definition(data_specification, function, rewrite_rules)
    {
      if (data::is_function_sort(function.sort()))
      {
        m_codomain = data::function_sort(function.sort()).codomain();
      }
      else
      {
        m_codomain = function.sort();
      }
    }

    std::string generate_definition() const
    {
      std::map<data::variable, std::string> parameters;
      for (std::size_t i = 0; i < m_parameters.size(); i++)
      {
        std::string name = m_data_specification->identifier_generator()(m_parameters[i].name());
        parameters[m_parameters[i]] = name;
      }
      for (std::size_t i = 0; i < m_parameters.size(); i++)
      {
        m_data_specification->identifier_generator().remove_identifier(parameters[m_parameters[i]]);
      }

      std::string output = "(define-fun-rec " + m_name + " (";
      for (std::size_t i = 0; i < m_parameters.size(); i++)
      {
        output += " (" + parameters[m_parameters[i]] + " " + m_data_specification->generate_sort_name(m_parameters[i].sort()) + ")";
      }
      output += ") " + m_data_specification->generate_sort_name(m_codomain);
      output += " " + m_data_specification->generate_data_expression(parameters, m_rhs);
      output += ")\n";
      return output;
    }
};

function_definition *smt4_data_specification::create_recursive_function_definition(const data::function_symbol& function, const data::data_equation_vector& rewrite_rules)
{
  return new cvc4_recursive_function_definition(this, function, rewrite_rules);
}

} // namespace smt
} // namespace mcrl2
