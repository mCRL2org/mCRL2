// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <queue>
#include <vector>

#include "mcrl2/data/join.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/smt/basic_data_specification.h"
#include "mcrl2/smt/recursive_function_definition.h"
#include "mcrl2/smt/translation_error.h"

namespace mcrl2
{

namespace smt
{

template<class T> std::vector<T> topological_sort(std::map<T, std::set<T> > dependencies)
{
  std::queue<T> vertices_without_dependencies;
  std::map<T, std::set<T> > reverse_dependencies;
  for (typename std::map<T, std::set<T> >::const_iterator i = dependencies.begin(); i != dependencies.end(); i++)
  {
    for (typename std::set<T>::const_iterator j = i->second.begin(); j != i->second.end(); j++)
    {
      reverse_dependencies[*j].insert(i->first);
    }

    if (i->second.empty())
    {
      vertices_without_dependencies.push(i->first);
    }
  }

  std::vector<T> output;
  output.reserve(dependencies.size());

  while (!vertices_without_dependencies.empty())
  {
    T vertex = vertices_without_dependencies.front();
    vertices_without_dependencies.pop();
    output.push_back(vertex);

    const std::set<T> &edges = reverse_dependencies[vertex];
    for (typename std::set<T>::const_iterator i = edges.begin(); i != edges.end(); i++)
    {
      dependencies[*i].erase(vertex);
      if (dependencies[*i].empty())
      {
        vertices_without_dependencies.push(*i);
      }
    }
  }

  if (output.size() != dependencies.size())
  {
    for (typename std::vector<T>::const_iterator i = output.begin(); i != output.end(); i++)
    {
      dependencies.erase(*i);
    }
    assert(!dependencies.empty());
    throw translation_error("Dependency loop trying to resolve dependencies for " + data::pp(dependencies.begin()->first));
  }
  else
  {
    return output;
  }
}

basic_data_specification::basic_data_specification(data::data_specification &data_specification, std::shared_ptr<data::set_identifier_generator> identifier_generator):
  m_identifier_generator(identifier_generator),
  m_representative_generator(data_specification)
{
  find_rewrite_rules(data_specification);
}

std::string basic_data_specification::generate_data_expression(const std::map<data::variable, std::string>& declared_variables, data::data_expression expression) const
{
  if (data::is_variable(expression))
  {
    data::variable variable(expression);
    if (!declared_variables.count(variable))
    {
      throw translation_error("Undeclared variable: " + data::pp(expression));
    }
    return declared_variables.at(variable);
  }
  else if (data::is_application(expression) || data::is_function_symbol(expression))
  {
    data::function_symbol function;
    data::data_expression_vector arguments;

    if (data::is_application(expression))
    {
      data::application application(expression);
      if (data::is_function_symbol(application.head()))
      {
        function = data::function_symbol(application.head());
      }
      else
      {
        throw translation_error("Application " + data::pp(expression) + " does not have a function symbol as its head");
      }
      arguments.insert(arguments.end(), application.begin(), application.end());
    }
    else
    {
      function = data::function_symbol(expression);
    }

    if (!m_functions.count(function))
    {
      throw translation_error("Untranslatable function symbol " + data::pp(function));
    }
    return m_functions.at(function)->generate_data_expression(declared_variables, arguments);
  }
  else
  {
    throw translation_error("Unsupported expression " + data::pp(expression));
  }
}

std::string basic_data_specification::generate_data_specification(std::set<data::sort_expression> required_sorts, std::set<data::function_symbol> required_functions) const
{
  std::queue<data::function_symbol> function_queue;
  for (std::set<data::function_symbol>::const_iterator i = required_functions.begin(); i != required_functions.end(); i++)
  {
    function_queue.push(*i);
  }
  std::map<data::function_symbol, std::set<data::function_symbol> > function_dependencies;
  while (!function_queue.empty())
  {
    data::function_symbol function = function_queue.front();
    function_queue.pop();
    if (!m_functions.count(function))
    {
      throw translation_error("Untranslated function " + data::pp(function));
    }

    const std::set<data::function_symbol>& dependencies = m_functions.at(function)->function_dependencies();
    function_dependencies[function] = dependencies;
    function_dependencies[function].erase(function);
    for (std::set<data::function_symbol>::const_iterator i = dependencies.begin(); i != dependencies.end(); i++)
    {
      if (!required_functions.count(*i))
      {
        required_functions.insert(*i);
        function_queue.push(*i);
      }
    }

    const std::set<data::sort_expression>& sorts = m_functions.at(function)->sort_dependencies();
    for (std::set<data::sort_expression>::const_iterator i = sorts.begin(); i != sorts.end(); i++)
    {
      required_sorts.insert(*i);
    }
  }

  std::queue<data::sort_expression> sort_queue;
  for (std::set<data::sort_expression>::const_iterator i = required_sorts.begin(); i != required_sorts.end(); i++)
  {
    sort_queue.push(*i);
  }
  std::map<data::sort_expression, std::set<data::sort_expression> > sort_dependencies;
  while (!sort_queue.empty())
  {
    data::sort_expression sort = sort_queue.front();
    sort_queue.pop();
    if (!m_sorts.count(sort))
    {
      throw translation_error("Untranslated sort " + data::pp(sort));
    }

    const std::set<data::sort_expression>& dependencies = m_sorts.at(sort)->sort_dependencies();
    sort_dependencies[sort] = dependencies;
    sort_dependencies[sort].erase(sort);
    for (std::set<data::sort_expression>::const_iterator i = dependencies.begin(); i != dependencies.end(); i++)
    {
      if (!required_sorts.count(*i))
      {
        required_sorts.insert(*i);
        sort_queue.push(*i);
      }
    }
  }

  std::vector<data::sort_expression> sort_order = topological_sort(sort_dependencies);
  std::vector<data::function_symbol> function_order = topological_sort(function_dependencies);

  std::string output;
  for (std::vector<data::sort_expression>::const_iterator i = sort_order.begin(); i != sort_order.end(); i++)
  {
    assert(m_sorts.count(*i));
    output += m_sorts.at(*i)->generate_definition();
  }
  for (std::vector<data::function_symbol>::const_iterator i = function_order.begin(); i != function_order.end(); i++)
  {
    assert(m_functions.count(*i));
    output += m_functions.at(*i)->generate_definition();
  }
  return output;
}

std::string basic_data_specification::generate_smt_problem(const smt_problem& problem)
{
  std::set<data::sort_expression> required_sorts;
  std::set<data::function_symbol> required_functions;

  std::string variable_declarations;
  std::string assertions;

  class identifier_storage: public std::set<core::identifier_string>
  {
    public:
      data::set_identifier_generator& m_generator;
      identifier_storage(data::set_identifier_generator& generator):
        m_generator(generator)
      {}

      ~identifier_storage()
      {
        m_generator.remove_identifiers(*this);
      }
  };
  identifier_storage variable_identifiers(identifier_generator());
  std::map<data::variable, std::string> declared_variables;

  for (std::set<data::variable>::const_iterator i = problem.variables().begin(); i != problem.variables().end(); i++)
  {
    core::identifier_string name = identifier_generator()(i->name());
    variable_identifiers.insert(name);
    declared_variables[*i] = name;

    required_sorts.insert(i->sort());

    if (!m_sorts.count(i->sort()))
    {
      throw translation_error("Untranslated sort " + data::pp(*i));
    }
    variable_declarations += m_sorts.at(i->sort())->generate_variable_declaration(name);
  }

  for (std::set<data::data_expression>::const_iterator i = problem.assertions().begin(); i != problem.assertions().end(); i++)
  {
    std::set<data::function_symbol> functions = data::find_function_symbols(*i);
    required_functions.insert(functions.begin(), functions.end());

    assertions += generate_assertion(declared_variables, *i);
  }

  for (std::set<data::data_expression_list>::const_iterator i = problem.distinct_assertions().begin(); i != problem.distinct_assertions().end(); i++)
  {
    std::set<data::function_symbol> functions = data::find_function_symbols(*i);
    required_functions.insert(functions.begin(), functions.end());

    if (i->size() == 0 || i->size() == 1)
    {
      continue;
    }

    assertions += generate_distinct_assertion(declared_variables, *i);
  }

  std::string data_specification = generate_data_specification(required_sorts, required_functions);

  return generate_smt_problem(data_specification, variable_declarations, assertions);
}

std::string basic_data_specification::generate_distinct_assertion(const std::map<data::variable, std::string>& declared_variables, const data::data_expression_list& distinct_terms) const
{
  std::string output;
  for (data::data_expression_list::const_iterator i = distinct_terms.begin(); i != distinct_terms.end(); i++)
  {
    data::data_expression_list::const_iterator j = i;
    j++;
    for (; j != distinct_terms.end(); j++)
    {
      output += generate_assertion(declared_variables, data::not_equal_to(*i, *j));
    }
  }
  return output;
}

void basic_data_specification::find_rewrite_rules(const data::data_specification& data_specification)
{
  for (data::data_equation_vector::const_iterator i = data_specification.equations().begin(); i != data_specification.equations().end(); i++)
  {
    data::data_expression lhs = i->lhs();
    data::function_symbol function;
    if (data::is_application(lhs))
    {
      data::application application(lhs);
      if (data::is_function_symbol(application.head()))
      {
        function = data::function_symbol(application.head());
      }
      else
      {
        continue;
      }
    }
    else if (data::is_function_symbol(lhs))
    {
      function = data::function_symbol(lhs);
    }
    else
    {
      continue;
    }

    m_rewrite_rules[function].push_back(*i);
  }

  /*
   * For each mapping, find out whether it is a recogniser or projection function.
   */
  for (std::map<data::function_symbol, data::data_equation_vector>::const_iterator i = m_rewrite_rules.begin(); i != m_rewrite_rules.end(); i++)
  {
    if (!data::is_function_sort(i->first.sort()))
    {
      continue;
    }
    data::function_sort sort(i->first.sort());
    if (sort.domain().size() != 1)
    {
      continue;
    }
    data::sort_expression domain = sort.domain().front();
    data::function_symbol_vector constructor_vector = data_specification.constructors(domain);
    if (constructor_vector.empty())
    {
      continue;
    }
    std::set<data::function_symbol> constructors;
    constructors.insert(constructor_vector.begin(), constructor_vector.end());

    /*
     * Check for recognisers.
     */
    if (data::sort_bool::is_bool(sort.codomain()))
    {
      std::set<data::function_symbol> positive_recogniser_equation_seen;
      std::set<data::function_symbol> negative_recogniser_equation_seen;
      bool invalid_equations_seen = false;
      for (data::data_equation_vector::const_iterator j = i->second.begin(); j != i->second.end(); j++)
      {
        if (j->condition() != data::sort_bool::true_())
        {
          invalid_equations_seen = true;
          break;
        }
        if (!data::is_application(j->lhs()))
        {
          invalid_equations_seen = true;
          break;
        }
        data::application application(j->lhs());
        assert(application.head() == i->first);
        assert(application.size() == 1);
        data::data_expression argument(application[0]);
        data::function_symbol constructor;
        if (data::is_application(argument))
        {
          data::application constructor_application(argument);
          if (data::is_function_symbol(constructor_application.head()))
          {
            constructor = data::function_symbol(constructor_application.head());
          }
          else
          {
            invalid_equations_seen = true;
            break;
          }
          std::set<data::variable> arguments;
          for (data::application::const_iterator k = constructor_application.begin(); k != constructor_application.end(); k++)
          {
            if (data::is_variable(*k))
            {
              arguments.insert(data::variable(*k));
            }
            else
            {
              invalid_equations_seen = true;
              break;
            }
          }
          if (invalid_equations_seen)
          {
            break;
          }
          if (arguments.size() != constructor_application.size())
          {
            invalid_equations_seen = true;
            break;
          }
        }
        else if (data::is_function_symbol(argument))
        {
          constructor = data::function_symbol(argument);
        }
        else
        {
          invalid_equations_seen = true;
          break;
        }
        if (constructors.count(constructor) == 0)
        {
          invalid_equations_seen = true;
          break;
        }

        if (j->rhs() == data::sort_bool::true_())
        {
          positive_recogniser_equation_seen.insert(constructor);
          if (negative_recogniser_equation_seen.count(constructor) != 0)
          {
            invalid_equations_seen = true;
            break;
          }
        }
        else if (j->rhs() == data::sort_bool::false_())
        {
          negative_recogniser_equation_seen.insert(constructor);
          if (negative_recogniser_equation_seen.count(constructor) != 0)
          {
            invalid_equations_seen = true;
            break;
          }
        }
        else
        {
          invalid_equations_seen = true;
          break;
        }
      }
      if (!invalid_equations_seen &&
          positive_recogniser_equation_seen.size() == 1 &&
          positive_recogniser_equation_seen.size() + negative_recogniser_equation_seen.size() == constructors.size())
      {
        data::function_symbol constructor = *positive_recogniser_equation_seen.begin();
        m_all_recognisers[constructor].push_back(i->first);
      }
    }

    /*
     * Check for projections.
     */
    if (i->second.size() == 1)
    {
      data::data_equation equation = i->second[0];
      if (equation.condition() == data::sort_bool::true_() &&
          data::is_variable(equation.rhs()) &&
          data::is_application(equation.lhs()))
      {
        data::application application(equation.lhs());
        assert(application.head() == i->first);
        assert(application.size() == 1);
        data::data_expression argument(application[0]);
        if (data::is_application(argument) &&
            data::is_function_symbol(data::application(argument).head()) &&
            constructors.count(data::function_symbol(data::application(argument).head())) == 1)
        {
          data::application constructor_application(argument);
          data::function_symbol constructor(constructor_application.head());
          size_t index = 0;
          bool found = false;
          std::set<data::variable> arguments;
          for (data::application::const_iterator j = constructor_application.begin(); j != constructor_application.end(); j++)
          {
            if (!data::is_variable(*j))
            {
              break;
            }
            data::variable variable(*j);
            arguments.insert(variable);
            if (variable == equation.rhs())
            {
              found = true;
            }
            if (!found)
            {
              index++;
            }
          }
          if (found && arguments.size() == constructor_application.size())
          {
            m_all_projections[constructor][index].push_back(i->first);
          }
        }
      }
    }
  }
}

void basic_data_specification::add_constructed_sort(const data::data_specification& data_specification, const data::sort_expression& sort)
{
  m_constructed_sort_constructors[sort].insert(data_specification.constructors(sort).begin(), data_specification.constructors(sort).end());

  constructed_sort_definition::constructors_t constructor_specification;
  for (std::set<data::function_symbol>::const_iterator i = m_constructed_sort_constructors[sort].begin(); i != m_constructed_sort_constructors[sort].end(); i++)
  {
    constructor_specification.push_back(constructed_sort_definition::constructor_t());
    constructed_sort_definition::constructor_t& constructor = constructor_specification.back();

    constructor.constructor = *i;
    if (m_all_recognisers[*i].empty())
    {
      std::string name = std::string("recognise-") + std::string(i->name());
      m_all_recognisers[*i].push_back(data::function_symbol(identifier_generator()(name), data::make_function_sort(sort, data::sort_bool::bool_())));
    }
    constructor.recognisers = m_all_recognisers[*i];
    m_recognisers[*i] = constructor.recognisers[0];

    if (data::is_function_sort(i->sort()))
    {
      data::function_sort constructor_sort(i->sort());
      size_t index = 0;
      for (data::sort_expression_list::const_iterator j = constructor_sort.domain().begin(); j != constructor_sort.domain().end(); j++)
      {
        constructor.fields.push_back(constructed_sort_definition::field_t());
        constructed_sort_definition::field_t& field = constructor.fields.back();

        if (m_all_projections[*i][index].empty())
        {
          std::string name = std::string(i->name()) + "-field-" + std::to_string(index);
          m_all_projections[*i][index].push_back(data::function_symbol(identifier_generator()(name), data::make_function_sort(sort, *j)));
        }
        field.projections = m_all_projections[*i][index];
        m_projections[*i][index] = field.projections[0];

        index++;
      }
    }
  }

  constructed_sort_definition *definition = create_constructed_sort(sort, constructor_specification);
  add_sort_definition(sort, std::shared_ptr<sort_definition>(definition));
}

/*
 * TODO: This should be moved to the data library.
 */
static bool is_constructed_sort(const data::data_specification& data_specification, const data::sort_expression& sort)
{
  data::function_symbol_vector constructors = data_specification.constructors(sort);
  if (constructors.empty())
  {
    return false;
  }

  data::rewriter r(data_specification);

  for (data::function_symbol_vector::const_iterator i = constructors.begin(); i != constructors.end(); i++)
  {
    for (data::function_symbol_vector::const_iterator j = constructors.begin(); j != constructors.end(); j++)
    {
      size_t index = 1;

      data::variable_vector variables_i;
      data::data_expression term_i;
      if (data::is_function_sort(i->sort()))
      {
        data::function_sort sort(i->sort());
        for (data::sort_expression_list::const_iterator k = sort.domain().begin(); k != sort.domain().end(); k++)
        {
          variables_i.push_back(data::variable("v" + std::to_string(index++), *k));
        }
        term_i = data::application(*i, variables_i);
      }
      else
      {
        term_i = *i;
      }

      data::variable_vector variables_j;
      data::data_expression term_j;
      if (data::is_function_sort(j->sort()))
      {
        data::function_sort sort(j->sort());
        for (data::sort_expression_list::const_iterator k = sort.domain().begin(); k != sort.domain().end(); k++)
        {
          variables_j.push_back(data::variable("v" + std::to_string(index++), *k));
        }
        term_j = data::application(*j, variables_j);
      }
      else
      {
        term_j = *j;
      }

      data::data_expression equality = data::equal_to(term_i, term_j);
      data::data_expression result = r(equality);

      if (*i == *j)
      {
        std::set<data::data_expression> conjuncts = data::split_and(result);
        if (conjuncts.count(data::sort_bool::true_()))
        {
          conjuncts.erase(data::sort_bool::true_());
        }

        assert(variables_i.size() == variables_j.size());
        for (size_t k = 0; k < variables_i.size(); k++)
        {
          bool found = false;
          if (conjuncts.count(data::equal_to(variables_i[k], variables_j[k])))
          {
            conjuncts.erase(data::equal_to(variables_i[k], variables_j[k]));
            found = true;
          }
          if (conjuncts.count(data::equal_to(variables_j[k], variables_i[k])))
          {
            conjuncts.erase(data::equal_to(variables_j[k], variables_i[k]));
            found = true;
          }
          if (!found)
          {
            return false;
          }
        }
        if (!conjuncts.empty())
        {
          return false;
        }
      }
      else
      {
        if (result != data::sort_bool::false_())
        {
          return false;
        }
      }
    }
  }

  return true;
}

void basic_data_specification::add_constructed_sorts(const data::data_specification& data_specification)
{
  for (std::set<data::sort_expression>::const_iterator i = data_specification.sorts().begin(); i != data_specification.sorts().end(); i++)
  {
    /*
     * bool, pos, nat, and int are constructed sorts, but they are not translated as constructed sorts
     * but are instead mapped to solver-specific builtin sorts.
     */
    if (data::sort_bool::is_bool(*i) ||
        data::sort_pos::is_pos(*i) ||
        data::sort_nat::is_nat(*i) ||
        data::sort_int::is_int(*i))
    {
      continue;
    }

    if (m_sorts.count(*i))
    {
      continue;
    }

    if (smt::is_constructed_sort(data_specification, *i))
    {
      add_constructed_sort(data_specification, *i);
    }
  }
}

void basic_data_specification::add_standard_operators(
  const data::data_specification& data_specification,
  const std::shared_ptr<function_definition>& equal_to,
  const std::shared_ptr<function_definition>& not_equal_to,
  const std::shared_ptr<function_definition>& if_)
{
  for (std::set<data::sort_expression>::const_iterator i = data_specification.sorts().begin(); i != data_specification.sorts().end(); i++)
  {
    add_function_definition(data::equal_to(*i), equal_to);
    add_function_definition(data::not_equal_to(*i), not_equal_to);
    add_function_definition(data::if_(*i), if_);
  }
}

void basic_data_specification::add_boolean_operators(
  const data::data_specification& /* data_specification */,
  const std::shared_ptr<function_definition>& not_,
  const std::shared_ptr<function_definition>& and_,
  const std::shared_ptr<function_definition>& or_,
  const std::shared_ptr<function_definition>& implies)
{
  add_function_definition(data::sort_bool::not_(), not_);
  add_function_definition(data::sort_bool::and_(), and_);
  add_function_definition(data::sort_bool::or_(), or_);
  add_function_definition(data::sort_bool::implies(), implies);
}

void basic_data_specification::add_numerical_operators(
  const data::data_specification& /* data_specification */,
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
  const std::shared_ptr<function_definition>& ceil,
  const std::shared_ptr<function_definition>& round,
  const std::shared_ptr<function_definition>& unary_minus,
  const std::shared_ptr<function_definition>& maximum,
  const std::shared_ptr<function_definition>& minimum,
  const std::shared_ptr<function_definition>& abs)
{
  /*
   * Missing functions:
   * - exp
   * - sqrt
   */

  const data::basic_sort& bool_ = data::sort_bool::bool_();
  const data::basic_sort& pos = data::sort_pos::pos();
  const data::basic_sort& nat = data::sort_nat::nat();
  const data::basic_sort& int_ = data::sort_int::int_();
  const data::basic_sort& real = data::sort_real::real_();

  data::variable p1("p1", pos);
  data::variable p2("p2", pos);
  data::variable n1("n1", nat);
  data::variable n2("n2", nat);
  data::variable i1("i1", int_);
  data::variable i2("i2", int_);
  data::variable r1("r1", real);
  data::variable r2("r2", real);

  const data::data_expression& pos_one = data::sort_pos::c1();
  data::data_expression pos_two = data::sort_pos::cdub(data::sort_bool::false_(), pos_one);
  const data::data_expression& nat_zero = data::sort_nat::c0();
  data::data_expression nat_one = data::sort_nat::cnat(pos_one);
  data::data_expression int_zero = data::sort_int::cint(nat_zero);
  data::data_expression int_one = data::sort_int::cint(nat_one);
  data::data_expression real_zero = data::sort_real::creal(int_zero, pos_one);
  data::data_expression real_one = data::sort_real::creal(int_one, pos_one);
  data::data_expression real_half = data::sort_real::divides(pos_one, pos_two);

  class identity_function_definition: public function_definition
  {
    public:
      identity_function_definition(smt::data_specification *data_specification):
        function_definition(data_specification)
      {}

      virtual std::string generate_data_expression(const std::map<data::variable, std::string>& declared_variables, data::data_expression_vector arguments) const
      {
        assert(arguments.size() == 1);
        return m_data_specification->generate_data_expression(declared_variables, arguments[0]);
      }
  };
  std::shared_ptr<function_definition> identity(new identity_function_definition(this));

  add_function_definition(data::less(pos), less);
  add_function_definition(data::less_equal(pos), less_equal);
  add_function_definition(data::greater(pos), greater);
  add_function_definition(data::greater_equal(pos), greater_equal);
  add_function_definition(data::sort_pos::plus(), plus);
  add_function_definition(data::sort_pos::times(), times);
  add_function_definition(data::sort_pos::maximum(), maximum, atermpp::make_vector(p1, p2), data::if_(data::greater(p1, p2), p1, p2));
  add_function_definition(data::sort_pos::minimum(), minimum, atermpp::make_vector(p1, p2), data::if_(data::less(p1, p2), p1, p2));
  add_function_definition(data::sort_pos::succ(), atermpp::make_vector(p1), data::sort_pos::plus(p1, pos_one));

  add_function_definition(data::less(nat), less);
  add_function_definition(data::less_equal(nat), less_equal);
  add_function_definition(data::greater(nat), greater);
  add_function_definition(data::greater_equal(nat), greater_equal);
  add_function_definition(data::sort_nat::plus(pos, nat), plus);
  add_function_definition(data::sort_nat::plus(nat, pos), plus);
  add_function_definition(data::sort_nat::plus(nat, nat), plus);
  add_function_definition(data::sort_nat::times(nat, nat), times);
  add_function_definition(data::sort_nat::div(), div);
  add_function_definition(data::sort_nat::mod(), mod);
  add_function_definition(data::sort_nat::maximum(pos, nat), maximum, atermpp::make_vector(p1, n1), data::sort_nat::nat2pos(data::if_(data::greater(data::sort_nat::pos2nat(p1), n1), data::sort_nat::pos2nat(p1), n1)));
  add_function_definition(data::sort_nat::maximum(nat, pos), maximum, atermpp::make_vector(n1, p1), data::sort_nat::nat2pos(data::if_(data::greater(data::sort_nat::pos2nat(p1), n1), data::sort_nat::pos2nat(p1), n1)));
  add_function_definition(data::sort_nat::maximum(nat, nat), maximum, atermpp::make_vector(n1, n2), data::if_(data::greater(n1, n2), n1, n2));
  add_function_definition(data::sort_nat::minimum(nat, nat), minimum, atermpp::make_vector(n1, n2), data::if_(data::less(n1, n2), n1, n2));
  add_function_definition(data::sort_nat::pos2nat(), identity);
  add_function_definition(data::sort_nat::nat2pos(), identity);
  add_function_definition(data::sort_nat::succ(nat), make_vector(n1), data::sort_nat::plus(n1, pos_one));
  add_function_definition(data::sort_nat::pred(), make_vector(p1), data::sort_int::int2nat(data::sort_int::minus(p1, pos_one)));

  add_function_definition(data::less(int_), less);
  add_function_definition(data::less_equal(int_), less_equal);
  add_function_definition(data::greater(int_), greater);
  add_function_definition(data::greater_equal(int_), greater_equal);
  add_function_definition(data::sort_int::plus(int_, int_), plus);
  add_function_definition(data::sort_int::minus(pos, pos), minus);
  add_function_definition(data::sort_int::minus(nat, nat), minus);
  add_function_definition(data::sort_int::minus(int_, int_), minus);
  add_function_definition(data::sort_int::times(int_, int_), times);
  add_function_definition(data::sort_int::div(int_, pos), div);
  add_function_definition(data::sort_int::mod(int_, pos), mod);
  add_function_definition(data::sort_int::negate(pos), unary_minus, atermpp::make_vector(p1), data::sort_int::minus(int_zero, data::sort_int::pos2int(p1)));
  add_function_definition(data::sort_int::negate(nat), unary_minus, atermpp::make_vector(n1), data::sort_int::minus(int_zero, data::sort_int::nat2int(n1)));
  add_function_definition(data::sort_int::negate(int_), unary_minus, atermpp::make_vector(i1), data::sort_int::minus(int_zero, i1));
  add_function_definition(data::sort_int::maximum(pos, int_), maximum, atermpp::make_vector(p1, i1), data::sort_int::int2pos(data::if_(data::greater(data::sort_int::pos2int(p1), i1), data::sort_int::pos2int(p1), i1)));
  add_function_definition(data::sort_int::maximum(int_, pos), maximum, atermpp::make_vector(i1, p1), data::sort_int::int2pos(data::if_(data::greater(data::sort_int::pos2int(p1), i1), data::sort_int::pos2int(p1), i1)));
  add_function_definition(data::sort_int::maximum(nat, int_), maximum, atermpp::make_vector(n1, i1), data::sort_int::int2nat(data::if_(data::greater(data::sort_int::nat2int(n1), i1), data::sort_int::nat2int(n1), i1)));
  add_function_definition(data::sort_int::maximum(int_, nat), maximum, atermpp::make_vector(i1, n1), data::sort_int::int2nat(data::if_(data::greater(data::sort_int::nat2int(n1), i1), data::sort_int::nat2int(n1), i1)));
  add_function_definition(data::sort_int::maximum(int_, int_), maximum, atermpp::make_vector(i1, i2), data::if_(data::greater(i1, i2), i1, i2));
  add_function_definition(data::sort_int::minimum(int_, int_), minimum, atermpp::make_vector(i1, i2), data::if_(data::less(i1, i2), i1, i2));
  add_function_definition(data::sort_int::abs(), abs, atermpp::make_vector(i1), data::if_(data::less(i1, int_zero), data::sort_int::negate(i1), i1));
  add_function_definition(data::sort_int::pos2int(), identity);
  add_function_definition(data::sort_int::int2pos(), identity);
  add_function_definition(data::sort_int::nat2int(), identity);
  add_function_definition(data::sort_int::int2nat(), identity);
  add_function_definition(data::sort_int::succ(int_), atermpp::make_vector(i1), data::sort_int::plus(i1, int_one));
  add_function_definition(data::sort_int::pred(nat), atermpp::make_vector(n1), data::sort_int::minus(n1, nat_one));
  add_function_definition(data::sort_int::pred(int_), atermpp::make_vector(i1), data::sort_int::minus(i1, int_one));

  add_function_definition(data::less(real), less);
  add_function_definition(data::less_equal(real), less_equal);
  add_function_definition(data::greater(real), greater);
  add_function_definition(data::greater_equal(real), greater_equal);
  add_function_definition(data::sort_real::plus(real, real), plus);
  add_function_definition(data::sort_real::minus(real, real), minus);
  add_function_definition(data::sort_real::times(real, real), times);
  add_function_definition(data::sort_real::divides(pos, pos), divides);
  add_function_definition(data::sort_real::divides(nat, nat), divides);
  add_function_definition(data::sort_real::divides(int_, int_), divides);
  add_function_definition(data::sort_real::divides(real, real), divides);
  add_function_definition(data::sort_real::floor(), floor);
  add_function_definition(data::sort_real::ceil(), ceil, atermpp::make_vector(r1), data::sort_real::negate(data::sort_real::floor(data::sort_real::negate(r1))));
  add_function_definition(data::sort_real::round(), round, atermpp::make_vector(r1), data::sort_real::floor(data::sort_real::plus(r1, real_half)));
  add_function_definition(data::sort_real::negate(real), unary_minus, atermpp::make_vector(r1), data::sort_real::minus(real_zero, r1));
  add_function_definition(data::sort_real::maximum(real, real), maximum, atermpp::make_vector(r1, r2), data::if_(data::greater(r1, r2), r1, r2));
  add_function_definition(data::sort_real::minimum(real, real), minimum, atermpp::make_vector(r1, r2), data::if_(data::less(r1, r2), r1, r2));
  add_function_definition(data::sort_real::abs(real), abs, atermpp::make_vector(r1), data::if_(data::less(r1, real_zero), data::sort_real::negate(r1), r1));
  add_function_definition(data::sort_real::pos2real(), identity);
  add_function_definition(data::sort_real::real2pos(), identity);
  add_function_definition(data::sort_real::nat2real(), identity);
  add_function_definition(data::sort_real::real2nat(), identity);
  add_function_definition(data::sort_real::int2real(), identity);
  add_function_definition(data::sort_real::real2int(), identity);
  add_function_definition(data::sort_real::succ(real), atermpp::make_vector(r1), data::sort_real::plus(r1, real_one));
  add_function_definition(data::sort_real::pred(real), atermpp::make_vector(r1), data::sort_real::minus(r1, real_one));
}

void basic_data_specification::add_recursive_function(const data::function_symbol& function)
{
  if (!m_rewrite_rules.count(function))
  {
    return;
  }

  data::data_equation_vector pattern_matching_rules;
  for (data::data_equation_vector::const_iterator i = m_rewrite_rules.at(function).begin(); i != m_rewrite_rules.at(function).end(); i++)
  {
    if (recursive_function_definition::is_pattern_matching_rule(this, *i))
    {
      pattern_matching_rules.push_back(*i);
    }
  }
  if (pattern_matching_rules.empty())
  {
    return;
  }

  function_definition *definition = create_recursive_function_definition(function, pattern_matching_rules);
  add_function_definition(function, std::shared_ptr<function_definition>(definition));
}

void basic_data_specification::add_recursive_functions(const data::data_specification& data_specification)
{
  data::function_symbol_vector candidate_functions = data_specification.user_defined_mappings();
  for (std::set<data::sort_expression>::const_iterator i = data_specification.sorts().begin(); i != data_specification.sorts().end(); i++)
  {
    if (data_specification.sorts().count(data::sort_list::list(*i)))
    {
      data::function_symbol_vector list_functions = data::sort_list::list_generate_functions_code(*i);
      candidate_functions.insert(candidate_functions.end(), list_functions.begin(), list_functions.end());

      // Add the element_at function.
      data::variable n("n", data::sort_nat::nat());
      data::variable l("l", data::sort_list::list(*i));
      add_function_definition(data::sort_list::element_at(*i), atermpp::make_vector(l, n), data::if_(data::equal_to(n, data::sort_nat::c0()), data::sort_list::head(*i)(l), data::sort_list::element_at(*i)(data::sort_list::tail(*i)(l), data::sort_nat::pred(n))));
    }
  }

  for (data::function_symbol_vector::const_iterator i = candidate_functions.begin(); i != candidate_functions.end(); i++)
  {
    add_recursive_function(*i);
  }
}

} // namespace smt
} // namespace mcrl2
