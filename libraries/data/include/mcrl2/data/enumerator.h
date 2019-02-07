// Author(s): Wieger Wesselink, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/enumerator.h
/// \brief The class enumerator.

#ifndef MCRL2_DATA_ENUMERATOR_H
#define MCRL2_DATA_ENUMERATOR_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/detail/enumerator_identifier_generator.h"
#include "mcrl2/data/detail/enumerator_variable_limit.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitutions/enumerator_substitution.h"
#include "mcrl2/data/substitutions/mutable_indexed_substitution.h"
#include "mcrl2/utilities/math.h"
#include <boost/iterator/iterator_facade.hpp>
#include <deque>
#include <limits>
#include <map>
#include <sstream>
#include <utility>

namespace mcrl2
{

namespace data
{

namespace detail
{

inline
data_expression make_set_(std::size_t function_index, const sort_expression& element_sort, const data_expression_vector& set_elements)
{
  data_expression result = sort_fset::empty(element_sort);
  for (const auto & set_element : set_elements)
  {
    if (function_index % 2 == 1)
    {
      result=sort_fset::insert(element_sort, set_element, result);
    }
    function_index = function_index / 2;
  }
  return result;
}

inline
data_expression make_if_expression_(std::size_t& function_index,
                                   const std::size_t argument_index,
                                   const std::vector<data_expression_vector>& data_domain_expressions,
                                   const data_expression_vector& codomain_expressions,
                                   const variable_vector& parameters)
{
  if (argument_index == data_domain_expressions.size())
  {
    std::size_t result_expression_index = function_index % codomain_expressions.size();
    function_index = function_index / codomain_expressions.size();
    return codomain_expressions[result_expression_index];
  }

  data_expression result;
  const data_expression_vector& current_enumerated_elements = data_domain_expressions[argument_index];
  for (auto i = current_enumerated_elements.rbegin(); i != current_enumerated_elements.rend(); ++i)
  {
    if (i == current_enumerated_elements.rbegin())
    {
      result = make_if_expression_(function_index, argument_index + 1, data_domain_expressions, codomain_expressions, parameters);
    }
    else
    {
      const data_expression lhs = make_if_expression_(function_index, argument_index + 1, data_domain_expressions, codomain_expressions, parameters);
      if (lhs != result) // Optimize: if the lhs and rhs are equal, return the rhs.
      {
        result = if_(equal_to(parameters[argument_index], *i), lhs, result);
      }
    }
  }
  return result;
}

/// \brief Computes the elements of a finite set sort, and puts them in result. If there are too many elements, false is returned.
template <class Rewriter, class MutableSubstitution>
bool compute_finite_set_elements(const container_sort& sort,
                                 const data_specification& dataspec,
                                 Rewriter datar, MutableSubstitution& sigma,
                                 data_expression_vector& result,
                                 enumerator_identifier_generator& id_generator)
{
  // TODO: This routine would really benefit from caching the calculated set of elements, as the same set is often generated
  // over and over again.
  data_expression_vector all_element_expressions = enumerate_expressions(sort.element_sort(), dataspec, datar, id_generator);
  if (all_element_expressions.size() >= 32)  // If there are at least 2^32 functions, then enumerating them makes little sense.
  {
    return false;
  }
  if (all_element_expressions.size() > 16)  // If there are more than 2^16 functions, provide a warning.
  {
    mCRL2log(log::warning) << "Generate 2^" << all_element_expressions.size() << " sets to enumerate sort " << sort << "\n";
  }
  const std::size_t number_of_sets = utilities::power_size_t(2, all_element_expressions.size());
  for (std::size_t i = 0; i < number_of_sets; ++i)
  {
    result.push_back(datar(make_set_(i, sort.element_sort(), all_element_expressions), sigma));
  }
  return true;
}

/// \brief Computes the elements of a finite function sort, and puts them in result. If there are too many elements, false is returned.
template <class Rewriter>
bool compute_finite_function_sorts(const function_sort& sort,
                                   enumerator_identifier_generator& id_generator,
                                   const data::data_specification& dataspec,
                                   Rewriter datar,
                                   data_expression_vector& result,
                                   variable_list& function_parameter_list
                                  )
{
  data_expression_vector codomain_expressions = enumerate_expressions(sort.codomain(), dataspec, datar, id_generator);
  std::vector<data_expression_vector> domain_expressions;
  std::size_t total_domain_size = 1;
  variable_vector function_parameters;

  for (const sort_expression& s: sort.domain())
  {
    domain_expressions.push_back(enumerate_expressions(s, dataspec, datar, id_generator));
    total_domain_size = total_domain_size * domain_expressions.back().size();
    function_parameters.push_back(variable(id_generator(), s));
  }

  if (total_domain_size * utilities::ceil_log2(codomain_expressions.size()) >= 32)  // If there are at least 2^32 functions, then enumerating them makes little sense.
  {
    return false;
  }

  if (total_domain_size * utilities::ceil_log2(codomain_expressions.size()) > 16)  // If there are more than 2^16 functions, provide a warning.
  {
    mCRL2log(log::warning) << "Generate " << codomain_expressions.size() << "^" << total_domain_size << " functions to enumerate sort " << sort << "\n";
  }

  function_parameter_list = variable_list(function_parameters.begin(), function_parameters.end());

  const std::size_t number_of_functions = utilities::power_size_t(codomain_expressions.size(), total_domain_size);

  if (number_of_functions == 1)
  {
    result.push_back(abstraction(lambda_binder(), function_parameter_list, codomain_expressions.front()));
  }
  else
  {
    for (std::size_t i = 0; i < number_of_functions; ++i)
    {
      std::size_t function_index = i; // function_index is changed in make_if_expression. A copy is therefore required.
      result.push_back(abstraction(lambda_binder(), function_parameter_list, make_if_expression_(function_index, 0, domain_expressions, codomain_expressions, function_parameters)));
    }
  }
  return true;
}

template <typename Rewriter>
bool is_enumerable(const data_specification& dataspec, const Rewriter& rewr, const sort_expression& sort, std::list<sort_expression>& parents)
{
  if(sort_bag::is_bag(sort) || sort_fbag::is_fbag(sort))
  {
    return false;
  }
  else if (is_function_sort(sort))
  {
    const auto& func = atermpp::down_cast<function_sort>(sort);
    enumerator_identifier_generator id_gen;
    data_expression_vector expr_vec;
    variable_list var_list;
    return dataspec.is_certainly_finite(func) &&
      detail::compute_finite_function_sorts(func, id_gen, dataspec, rewr, expr_vec, var_list);
  }
  else if(sort_set::is_set(sort) || sort_fset::is_fset(sort))
  {
    enumerator_identifier_generator id_gen;
    data_expression_vector expr_vec;
    mutable_indexed_substitution<> mut_sub;
    return dataspec.is_certainly_finite(atermpp::down_cast<container_sort>(sort).element_sort()) &&
      (!sort_fset::is_fset(sort) || detail::compute_finite_set_elements(atermpp::down_cast<container_sort>(sort), dataspec, rewr, mut_sub, expr_vec, id_gen));
  }
  else
  {
    const function_symbol_vector& constructors = dataspec.constructors(sort);
    if(constructors.empty())
    {
      return false;
    }
    else
    {
      if(std::find(parents.begin(), parents.end(), sort) != parents.end())
      {
        return true;
      }
      parents.push_back(sort);
      bool result = std::all_of(constructors.begin(), constructors.end(), [&](const function_symbol& constructor)
        {
          return !is_function_sort(constructor.sort()) ||
            std::all_of(atermpp::down_cast<function_sort>(constructor.sort()).domain().begin(), atermpp::down_cast<function_sort>(constructor.sort()).domain().end(),
              [&](const sort_expression& arg_sort){ return is_enumerable(dataspec, rewr, arg_sort, parents); });
        });
      parents.pop_back();
      return result;
    }
  }
}

} // namespace detail

template <typename Rewriter>
static bool is_enumerable(const data_specification& dataspec, const Rewriter& rewr, const sort_expression& sort)
{
  std::list<sort_expression> parentstack;
  return detail::is_enumerable(dataspec, rewr, sort, parentstack);
}

struct is_not_false
{
  bool operator()(const data_expression& x) const
  {
    return !sort_bool::is_false_function_symbol(x);
  }
};

struct is_not_true
{
  bool operator()(const data_expression& x) const
  {
    return !sort_bool::is_true_function_symbol(x);
  }
};

/// \brief The default element for the todo list of the enumerator
template <typename Expression = data::data_expression>
class enumerator_list_element
{
  protected:
    data::variable_list v;
    Expression phi;

  public:
    typedef Expression expression_type;

    /// \brief Default constructor
    enumerator_list_element() = default;

    /// \brief Constructs the element (v, phi)
    enumerator_list_element(const data::variable_list& v_, const Expression& phi_)
      : v(v_), phi(phi_)
    {}

    /// \brief Constructs the element (v, phi)
    enumerator_list_element(const data::variable_list& v_,
                            const Expression& phi_,
                            const enumerator_list_element&
                           )
      : v(v_), phi(phi_)
    {}

    /// \brief Constructs the element (v, phi)
    enumerator_list_element(const data::variable_list& v_,
                            const Expression& phi_,
                            const enumerator_list_element&,
                            const data::variable&,
                            const data::data_expression&
                           )
      : v(v_), phi(phi_)
    {}

    const data::variable_list& variables() const
    {
      return v;
    }

    const Expression& expression() const
    {
      return phi;
    }

    Expression& expression()
    {
      return phi;
    }

    bool is_solution() const
    {
      return v.empty();
    }

    /// \brief Invalidates the element, by giving phi an undefined value
    void invalidate()
    {
      phi = data::undefined_data_expression();
    }

    /// \brief Returns true if the element is valid. If it becomes false, this is used to signal that
    /// the enumeration has been aborted.
    bool is_valid() const
    {
      return phi != data::undefined_data_expression();
    }
};

/// \brief An element for the todo list of the enumerator that collects the substitution
/// corresponding to the expression phi
template <typename Expression = data::data_expression>
class enumerator_list_element_with_substitution: public enumerator_list_element<Expression>
{
  protected:
    data::variable_list m_variables;
    data::data_expression_list m_expressions;

    // TODO: this is a hack to solve an efficiency problem in the data rewriter
    static mutable_indexed_substitution<>& empty_substitution()
    {
      static mutable_indexed_substitution<> result;
      return result;
    }

  public:
    typedef Expression expression_type;

    /// \brief Default constructor
    enumerator_list_element_with_substitution() = default;

    /// \brief Constructs the element (v, phi, [])
    enumerator_list_element_with_substitution(const data::variable_list& v, const Expression& phi)
      : enumerator_list_element<Expression>(v, phi)
    {}

    /// \brief Constructs the element (v, phi, e.sigma[v := x])
    enumerator_list_element_with_substitution(const data::variable_list& v,
                            const Expression& phi,
                            const enumerator_list_element_with_substitution<Expression>& elem
                           )
      : enumerator_list_element<Expression>(v, phi),
        m_variables(elem.m_variables),
        m_expressions(elem.m_expressions)
    {
    }

    /// \brief Constructs the element (v, phi, e.sigma[v := x])
    enumerator_list_element_with_substitution(
                            const data::variable_list& v,
                            const Expression& phi,
                            const enumerator_list_element_with_substitution<Expression>& elem,
                            const data::variable& d,
                            const data::data_expression& e
                           )
      : enumerator_list_element<Expression>(v, phi),
        m_variables(elem.m_variables),
        m_expressions(elem.m_expressions)
    {
      m_variables.push_front(d);
      m_expressions.push_front(e);
    }

    /// \brief Adds the assignments that correspond with this element to the substitution result.
    template <typename VariableList, typename MutableSubstitution, typename Rewriter>
    void add_assignments(const VariableList& v, MutableSubstitution& result, const Rewriter& rewriter) const
    {
      data::enumerator_substitution sigma(m_variables, m_expressions);
      sigma.revert();
      for (const data::variable& v_i: v)
      {
        result[v_i] = rewriter(sigma(v_i), empty_substitution());
      }
    }
};

template <typename Expression>
std::ostream& operator<<(std::ostream& out, const enumerator_list_element<Expression>& p)
{
  out << "{ [";
  const auto& variables = p.variables();
  for (auto i = variables.begin(); i != variables.end(); ++i)
  {
    if (i != variables.begin())
    {
      out << ", ";
    }
    out << *i << ": " << i->sort();
  }
  return out << "], " << p.expression() << " }";
}

/// \brief Contains the enumerator queue.
template <typename EnumeratorListElement>
class enumerator_queue
{
  protected:
    std::deque<EnumeratorListElement> P;

  public:
    typedef EnumeratorListElement value_type;
    typedef typename std::deque<EnumeratorListElement>::size_type size_type;

    /// \brief Default constructor
    enumerator_queue() = default;

    /// \brief Initializes the enumerator queue with the given value
    explicit enumerator_queue(const EnumeratorListElement& value)
            : P(1, value)
    { }

    void push_back(const EnumeratorListElement& x)
    {
#ifdef MCRL2_LOG_ENUMERATOR
      std::cout << "push_back " << x << std::endl;
#endif
      P.push_back(x);
    }

    bool empty() const
    {
      return P.empty();
    }

    void clear()
    {
      P.clear();
    }

    typename std::deque<EnumeratorListElement>::size_type size() const
    {
      return P.size();
    }

    const EnumeratorListElement& front() const
    {
      return P.front();
    }

    EnumeratorListElement& front()
    {
      return P.front();
    }

    const EnumeratorListElement& back() const
    {
      return P.back();
    }

    EnumeratorListElement& back()
    {
      return P.back();
    }

    void pop_front()
    {
      P.pop_front();
    }

    void pop_back()
    {
      P.pop_back();
    }
};

/// \brief An enumerator algorithm that generates solutions of a condition.
template <typename Rewriter = data::rewriter, typename DataRewriter = data::rewriter>
class enumerator_algorithm
{
  protected:
    // A rewriter
    const Rewriter& R;

    /// \brief A data specification.
    const data::data_specification& dataspec;

    // Needed for enumerate_expressions
    const DataRewriter& r;

    // A name generator
    enumerator_identifier_generator& id_generator;

    /// \brief max_count The enumeration is aborted after max_count iterations
    std::size_t m_max_count;

    std::string print(const data::variable& x) const
    {
      std::ostringstream out;
      out << x << ": " << x.sort();
      return out.str();
    }

  public:
    enumerator_algorithm(const Rewriter& R_,
                         const data::data_specification& dataspec_,
                         const DataRewriter& datar_,
                         enumerator_identifier_generator& id_generator_,
                         std::size_t max_count = (std::numeric_limits<std::size_t>::max)()
    )
      : R(R_), dataspec(dataspec_), r(datar_), id_generator(id_generator_), m_max_count(max_count)
    {}

    enumerator_algorithm(const enumerator_algorithm<Rewriter, DataRewriter>&) = delete;

    template <typename T>
    struct always_false
    {
      bool operator()(const T&) { return false; }
    };

    /// \brief Enumerates the front element of the todo list P.
    /// The enumeration is interrupted when report_solution returns true for the reported solution.
    /// \param P The todo list of the algorithm.
    /// \param sigma A mutable substitution that is applied by the rewriter.
    /// \param reject Elements p for which reject(p) is true are discarded.
    /// \param accept Elements p for which accept(p) is true are reported as a solution, even if the list of variables of the enumerator element is non-empty.
    /// \param report_solution A callback function that is called whenever a solution is found.
    /// It takes an enumerator element as argument.
    /// If report_solution returns true, the enumeration is interrupted.
    /// N.B. If the enumeration is resumed after an interruption, the element p that
    /// was interrupted will be enumerated again.
    /// \pre !P.empty()
    /// \return If the return value is true, enumeration will be interrupted
    template <typename EnumeratorListElement,
              typename MutableSubstitution,
              typename ReportSolution,
              typename Reject = always_false<typename EnumeratorListElement::expression_type>,
              typename Accept = always_false<typename EnumeratorListElement::expression_type>
             >
    bool enumerate_front(enumerator_queue<EnumeratorListElement>& P,
                         MutableSubstitution& sigma,
                         ReportSolution report_solution,
                         Reject reject = Reject(),
                         Accept accept = Accept()
                        ) const
    {
      assert(!P.empty());
      const auto& p = P.front();

      auto add_element = [&](const data::variable_list& variables,
                             const typename EnumeratorListElement::expression_type& phi,
                             const data::variable& v,
                             const data::data_expression& e
      )
      {
        auto phi1 = const_cast<Rewriter&>(R)(phi, sigma);
        if (reject(phi1))
        {
          return false;
        }
        if (variables.empty() || accept(phi1))
        {
          EnumeratorListElement q(variables, phi1, p, v, e);
          return report_solution(q);
        }
        P.push_back(EnumeratorListElement(variables, phi1, p, v, e));
        return false;
      };

      auto add_element_with_variables = [&](const data::variable_list& variables,
                                            const data::variable_list& added_variables,
                                            const typename EnumeratorListElement::expression_type& phi,
                                            const data::variable& v,
                                            const data::data_expression& e
      )
      {
        auto phi1 = const_cast<Rewriter&>(R)(phi, sigma);
        if (reject(phi1))
        {
          return false;
        }
        if (accept(phi1) || (variables.empty() && (phi1 == phi || added_variables.empty())))
        {
          EnumeratorListElement q(variables, phi1, p, v, e);
          return report_solution(q);
        }
        if (phi1 == phi)
        {
          // Discard the added_variables, since we know they do not appear in phi1
          P.push_back(EnumeratorListElement(variables, phi1, p, v, e));
        }
        else
        {
          // Additional variables are put at the end of the list!
          P.push_back(EnumeratorListElement(variables + added_variables, phi1, p, v, e));
        }
        return false;
      };

      const auto& v = p.variables();
      const auto& phi = p.expression();

      if (v.empty())
      {
        bool result = report_solution(p);
        P.pop_front();
        return result;
      }

      if (reject(phi))
      {
        P.pop_front();
        return false;
      }

      const auto& v1 = v.front();
      const auto& v_tail = v.tail();
      const auto& v1_sort = v1.sort();

      if (data::is_function_sort(v1_sort))
      {
        const function_sort& function = atermpp::down_cast<function_sort>(v1_sort);
        if (dataspec.is_certainly_finite(function))
        {
          data_expression_vector function_sorts;
          variable_list function_parameter_list;
          bool result = detail::compute_finite_function_sorts(function, id_generator, dataspec, r, function_sorts, function_parameter_list);
          if (!result)
          {
            throw mcrl2::runtime_error("Sort " + data::pp(v1_sort) + " has too many elements to enumerate.");
          }

          data_expression sigma_v1 = sigma(v1);
          for (const data_expression& f: function_sorts)
          {
            sigma[v1] = f;
            if (add_element(v_tail, phi, v1, f))
            {
              return true;
            }
          }
          sigma[v1] = sigma_v1;
        }
        else
        {
          throw mcrl2::runtime_error("Cannot enumerate elements of function sort " + data::pp(v1_sort) + ".");
        }
      }
      else if (sort_set::is_set(v1_sort))
      {
        const sort_expression& element_sort = atermpp::down_cast<container_sort>(v1_sort).element_sort();
        if (dataspec.is_certainly_finite(element_sort))
        {
          const data_expression lambda_term = abstraction(lambda_binder(), { variable(id_generator(), element_sort) }, sort_bool::false_());
          const variable fset_variable(id_generator(), sort_fset::fset(element_sort));
          data_expression e = sort_set::constructor(element_sort, lambda_term, fset_variable);
          data_expression sigma_v1 = sigma(v1);
          sigma[v1] = e;
          if (add_element(v_tail, phi, v1, e))
          {
            return true;
          }
          sigma[v1] = sigma_v1;
        }
        else
        {
          throw mcrl2::runtime_error("Cannot enumerate elements of set sort " + data::pp(v1_sort) + ".");
        }
      }
      else if (sort_fset::is_fset(v1_sort))
      {
        const auto& fset = atermpp::down_cast<container_sort>(v1_sort);
        if (dataspec.is_certainly_finite(fset.element_sort()))
        {
          data_expression_vector set_elements;
          bool result = detail::compute_finite_set_elements(fset, dataspec, r, sigma, set_elements, id_generator);
          if (!result)
          {
            throw mcrl2::runtime_error("Finite set sort " + data::pp(v1_sort) + " has too many elements to enumerate.");
          }

          data_expression sigma_v1 = sigma(v1);
          for (const data_expression& e: set_elements)
          {
            sigma[v1] = e;
            if (add_element(v_tail, phi, v1, e))
            {
              return true;
            }
          }
          sigma[v1] = sigma_v1;
        }
        else
        {
          throw mcrl2::runtime_error("Cannot enumerate elements of finite set sort " + data::pp(v1_sort) + ".");
        }
      }
      else if (sort_bag::is_bag(v1_sort))
      {
        throw mcrl2::runtime_error("Cannot enumerate elements of bag sort " + data::pp(v1_sort) + ".");
      }
      else if (sort_fbag::is_fbag(v1_sort))
      {
        throw mcrl2::runtime_error("Cannot enumerate elements of finite bag sort " + data::pp(v1_sort) + ".");
      }
      else
      {
        const function_symbol_vector& C = dataspec.constructors(v1_sort);
        if (!C.empty())
        {
          for (const function_symbol& c: C)
          {
            if (data::is_function_sort(c.sort()))
            {
              auto const& domain = atermpp::down_cast<data::function_sort>(c.sort()).domain();
              data::variable_list y(domain.begin(), domain.end(), [&](const data::sort_expression& s) { return data::variable(id_generator(), s); });
              // TODO: We want to apply r without the substitution sigma, but that is currently an inefficient operation of data::rewriter.
              data_expression cy = r(application(c, y.begin(), y.end()), sigma);
              sigma[v1] = cy;
              if (add_element_with_variables(v_tail, y, phi, v1, cy))
              {
                return true;
              }
              sigma[v1] = v1;
            }
            else
            {
              // TODO: We want to apply r without the substitution sigma, but that is currently an inefficient operation of data::rewriter.
              const auto e1 = r(c, sigma);
              sigma[v1] = e1;
              if (add_element(v_tail, phi, v1, e1))
              {
                return true;
              }
              sigma[v1] = v1;
            }
          }
        }
        else
        {
          throw mcrl2::runtime_error("Cannot enumerate elements of sort " + data::pp(v1_sort) + " without constructors.");
        }
      }

      P.pop_front();
      return false;
    }

    /// \brief Enumerates until P is empty. Solutions are reported using the callback function report_solution.
    /// The enumeration is interrupted when report_solution returns true for the reported solution.
    /// \param P The todo list of the algorithm.
    /// \param sigma A substitution.
    /// \param reject Elements p for which reject(p) is true are discarded.
    /// \param accept Elements p for which accept(p) is true are reported as a solution, even if the list of variables of the enumerator element is non-empty.
    /// \param report_solution A callback function that is called whenever a solution is found.
    /// It takes an enumerator element as argument.
    /// If report_solution returns true, the enumeration is interrupted.
    /// N.B. If the enumeration is resumed after an interruption, the element p that
    /// was interrupted will be enumerated again.
    /// \return The number of elements that have been processed
    template <typename EnumeratorListElement,
              typename MutableSubstitution,
              typename ReportSolution,
              typename Reject = always_false<typename EnumeratorListElement::expression_type>,
              typename Accept = always_false<typename EnumeratorListElement::expression_type>
             >
    std::size_t enumerate_all(enumerator_queue<EnumeratorListElement>& P,
                              MutableSubstitution& sigma,
                              ReportSolution report_solution,
                              Reject reject = Reject(),
                              Accept accept = Accept()
    ) const
    {
      std::size_t count = 0;
      while (!P.empty())
      {
        if (count++ >= m_max_count)
        {
          break;
        }
        if (enumerate_front(P, sigma, report_solution, reject, accept))
        {
          break;
        }
      }
      return count;
    }

    /// \brief Enumerates the element p. Solutions are reported using the callback function report_solution.
    /// The enumeration is interrupted when report_solution returns true for the reported solution.
    /// \param p An enumerator element, i.e. an expression with a list of variables.
    /// \param sigma A substitution.
    /// \param reject Elements p for which reject(p) is true are discarded.
    /// \param accept Elements p for which accept(p) is true are reported as a solution, even if the list of variables of the enumerator element is non-empty.
    /// \param report_solution A callback function that is called whenever a solution is found.
    /// It takes an enumerator element as argument.
    /// If report_solution returns true, the enumeration is interrupted.
    /// N.B. If the enumeration is resumed after an interruption, the element p that
    /// was interrupted will be enumerated again.
    /// \return The number of elements that have been processed
    template <typename EnumeratorListElement,
              typename MutableSubstitution,
              typename ReportSolution,
              typename Reject = always_false<typename EnumeratorListElement::expression_type>,
              typename Accept = always_false<typename EnumeratorListElement::expression_type>
    >
    std::size_t enumerate(const EnumeratorListElement& p,
                          MutableSubstitution& sigma,
                          ReportSolution report_solution,
                          Reject reject = Reject(),
                          Accept accept = Accept()
    ) const
    {
      enumerator_queue<EnumeratorListElement> P(p);
      return enumerate_all(P, sigma, report_solution, reject, accept);
    }

    std::size_t max_count() const
    {
      return m_max_count;
    }
};

/// \brief Returns a vector with all expressions of sort s.
/// \param s A sort expression.
/// \param dataspec The data specification defining the terms of sort \a s.
/// \param rewr A rewriter to be used to simplify terms and conditions.
/// \param id_generator An identifier generator used to generate new names for variables.
/// \details It is assumed that the sort s has only a finite number of elements.
template <class Rewriter>
data_expression_vector enumerate_expressions(const sort_expression& s,
                                             const data_specification& dataspec,
                                             const Rewriter& rewr,
                                             enumerator_identifier_generator& id_generator)
{
  typedef typename Rewriter::term_type term_type;
  typedef enumerator_list_element_with_substitution<term_type> enumerator_element;
  assert(dataspec.is_certainly_finite(s));

  enumerator_algorithm<Rewriter, Rewriter> E(rewr, dataspec, rewr, id_generator);
  data_expression_vector result;
  mutable_indexed_substitution<> sigma;
  const variable v("@var@", s);
  const variable_list v_list{ v };
  E.enumerate(enumerator_element(v_list, sort_bool::true_()),
              sigma,
              [&](const enumerator_element& p) {
                  p.add_assignments(v_list, sigma, rewr);
                  result.push_back(sigma(v));
                  return false;
              },
              sort_bool::is_false_function_symbol
  );
  return result;
}

/// \brief Returns a vector with all expressions of sort s.
/// \param s A sort expression.
/// \param dataspec The data specification defining the terms of sort \a s.
/// \param rewr A rewriter to be used to simplify terms and conditions.
/// \details It is assumed that the sort s has only a finite number of elements.
template <class Rewriter>
data_expression_vector enumerate_expressions(const sort_expression& s,
                                             const data_specification& dataspec,
                                             const Rewriter& rewr)
{
  enumerator_identifier_generator id_generator;
  return enumerate_expressions(s,dataspec, rewr, id_generator);
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ENUMERATOR_H
