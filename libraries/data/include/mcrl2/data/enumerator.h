// Author(s): Wieger Wesselink, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/enumerator.h
/// \brief The class enumerator.

#ifndef MCRL2_DATA_ENUMERATOR_H
#define MCRL2_DATA_ENUMERATOR_H

#include <deque>
#include <limits>
#include <map>
#include <sstream>
#include <utility>
#include <boost/iterator/iterator_facade.hpp>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitutions/enumerator_substitution.h"
#include "mcrl2/data/substitutions/mutable_indexed_substitution.h"
#include "mcrl2/utilities/math.h"

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
  for (auto i = set_elements.begin(); i != set_elements.end(); ++i)
  {
    if (function_index % 2 == 1)
    {
      result=sort_fset::insert(element_sort, *i, result);
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
bool compute_finite_set_elements(const container_sort& sort, const data_specification& dataspec, Rewriter datar, const MutableSubstitution& sigma, data_expression_vector& result)
{
  data_expression_vector all_element_expressions = enumerate_expressions(sort.element_sort(), dataspec, datar);
  if (all_element_expressions.size() >= 32)  // If there are at least 2^32 functions, then enumerating them makes little sense.
  {
    return false;
  }
  if (all_element_expressions.size() > 16)  // If there are more than 2^16 functions, provide a warning.
  {
    mCRL2log(log::warning) << "Generate 2^" << all_element_expressions.size() << " sets to enumerate sort " << sort << "\n";
  }
  const size_t number_of_sets = utilities::power_size_t(2, all_element_expressions.size());
  for (std::size_t i = 0; i < number_of_sets; ++i)
  {
    result.push_back(datar(make_set_(i, sort.element_sort(), all_element_expressions), sigma));
  }
  return true;
}

/// \brief Computes the elements of a finite function sort, and puts them in result. If there are too many elements, false is returned.
template <class IdentifierGenerator, class Rewriter, class MutableSubstitution>
bool compute_finite_function_sorts(const function_sort& sort,
                                   const IdentifierGenerator& id_generator,
                                   const data::data_specification& dataspec,
                                   Rewriter datar,
                                   const MutableSubstitution& sigma,
                                   data_expression_vector& result,
                                   variable_list& function_parameter_list
                                  )
{
  data_expression_vector codomain_expressions = enumerate_expressions(sort.codomain(), dataspec, datar);
  std::vector<data_expression_vector> domain_expressions;
  std::size_t total_domain_size = 1;
  variable_vector function_parameters;

  for (auto i = sort.domain().begin(); i != sort.domain().end(); ++i)
  {
    domain_expressions.push_back(enumerate_expressions(*i, dataspec, datar));
    total_domain_size = total_domain_size * domain_expressions.back().size();
    function_parameters.push_back(variable(const_cast<IdentifierGenerator&>(id_generator)("var_func", false), *i));
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

} // namespace detail

struct is_not_false
{
  typedef core::term_traits<data_expression> tr;
  bool operator()(const data_expression& x) const
  {
    return !tr::is_false(x);
  }
};

struct is_not_true
{
  typedef core::term_traits<data_expression> tr;
  bool operator()(const data_expression& x) const
  {
    return !tr::is_true(x);
  }
};

/// \brief The default element for the todo list of the enumerator
template <typename Expression>
class enumerator_list_element
{
  protected:
    data::variable_list v;
    Expression phi;

  public:
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
template <typename Expression>
class enumerator_list_element_with_substitution: public enumerator_list_element<Expression>
{
  protected:
    data::variable_list m_variables;
    data::data_expression_list m_expressions;

  public:
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
      for (typename VariableList::const_iterator i = v.begin(); i != v.end(); ++i)
      {
        result[*i] = rewriter(sigma(*i));
      }
    }
};

template <typename Expression>
std::ostream& operator<<(std::ostream& out, const enumerator_list_element<Expression>& p)
{
  return out << p.expression() << " " << core::detail::print_list(p.variables());
}

struct sort_name_generator
{
  data::set_identifier_generator& id_generator;

  sort_name_generator(data::set_identifier_generator& id_generator_)
    : id_generator(id_generator_)
  {}

  data::variable operator()(const data::sort_expression& s) const
  {
    return data::variable(id_generator("@x"), s);
  }
};

/// \brief An enumerator algorithm that generates solutions of a condition.
template <typename Rewriter>
class enumerator_algorithm
{
  /// \brief A map that caches the constructors corresponding to sort expressions.
  typedef std::map<data::sort_expression, std::vector<data::function_symbol> > constructor_map;

  protected:
    // A rewriter
    const Rewriter& R;

    /// \brief A data specification.
    const data::data_specification& dataspec;

    // Needed for enumerate_expressions
    const data::rewriter& datar;

    // A name generator
    mutable data::set_identifier_generator id_generator;

    /// \brief A mapping with constructors.
    mutable constructor_map m_constructors;

    /// \brief max_count The enumeration is aborted after max_count iterations
    std::size_t m_max_count;

    /// \brief throw_exceptions If true, an exception is thrown when the enumeration is aborted.
    bool m_throw_exceptions;

    /// \brief Returns the constructors with target s.
    /// \param s A sort expression
    /// \return The constructors corresponding to the sort expression.
    const std::vector<data::function_symbol>& constructors(const data::sort_expression& s) const
    {
      auto i = m_constructors.find(s);
      if (i != m_constructors.end())
      {
        return i->second;
      }
      m_constructors[s] = dataspec.constructors(s);
      return m_constructors[s];
    }

    std::string print(const data::variable& x) const
    {
      std::ostringstream out;
      out << x << ": " << x.sort();
      return out.str();
    }

    bool is_finite_set(const data::sort_expression& ) const
    {
      return false;
    }

    template <typename EnumeratorListElement>
    void cannot_enumerate(EnumeratorListElement& e, const std::string& msg) const
    {
      e.invalidate();
      if (m_throw_exceptions)
      {
        throw mcrl2::runtime_error(msg);
      }
    }

  public:
    enumerator_algorithm(const Rewriter& R_,
                         const data::data_specification& dataspec_,
                         const data::rewriter& datar_,
                         std::size_t max_count = (std::numeric_limits<std::size_t>::max)(),
                         bool throw_exceptions = false
                       )
      : R(R_), dataspec(dataspec_), datar(datar_), m_max_count(max_count), m_throw_exceptions(throw_exceptions)
    {}

    /// \brief Enumerates the front element of the todo list P.
    /// \param P The todo list of the algorithm.
    /// \param sigma A mutable substitution that is applied by the rewriter.
    /// \param accept Elements p for which accept(p) is false are discarded.
    /// \pre !P.empty()
    template <typename EnumeratorListElement, typename MutableSubstitution, typename Filter>
    void enumerate_front(std::deque<EnumeratorListElement>& P, MutableSubstitution& sigma, Filter accept) const
    {
      auto& R_ = const_cast<Rewriter&>(R);
      assert(!P.empty());

      auto p = P.front();
      auto const& v = p.variables();
      auto const& phi = p.expression();
      P.pop_front();

      auto const& vtail = v.tail();
      auto const& v1 = v.front();
      auto const& sort = v1.sort();

      if (data::is_function_sort(sort))
      {
        const function_sort& function = atermpp::aterm_cast<function_sort>(sort);
        if (dataspec.is_certainly_finite(function))
        {
          data_expression_vector function_sorts;
          variable_list function_parameter_list;
          bool result = detail::compute_finite_function_sorts(function, id_generator, dataspec, datar, sigma, function_sorts, function_parameter_list);
          if (!result)
          {
            cannot_enumerate(p, "Sort " + data::pp(sort) + " has too many elements to enumerate");
          }
          const data_expression old_substituted_value = sigma(v1);
          for(auto i = function_sorts.begin(); i != function_sorts.end(); ++i)
          {
            sigma[v1] = *i;
            auto phi1 = R_(phi, sigma);
            if (accept(phi1))
            {
              P.emplace_back(EnumeratorListElement(vtail, phi1, p, v1, *i));
            }
          }
          sigma[v1] = old_substituted_value;
        }
        else
        {
          cannot_enumerate(p, "Cannot enumerate elements of function sort " + data::pp(sort));
        }
      }
      else if (sort_set::is_set(sort))
      {
        const sort_expression element_sort = container_sort(sort).element_sort();
        if (dataspec.is_certainly_finite(element_sort))
        {
          const data_expression lambda_term = abstraction(lambda_binder(), atermpp::make_list<variable>(variable(id_generator("var_func", false), element_sort)), sort_bool::false_());
          const variable fset_variable(id_generator("@var_fset@", false), sort_fset::fset(element_sort));
          const data_expression term = sort_set::constructor(element_sort, lambda_term, fset_variable);
          const data_expression old_substituted_value = sigma(v1);
          sigma[v1] = term;
          auto phi1 = R_(phi, sigma);
          sigma[v1] = old_substituted_value;
          if (accept(phi1))
          {
            P.emplace_back(EnumeratorListElement(vtail + atermpp::make_list(fset_variable), phi1, p, v1, term));
          }
        }
        else
        {
          cannot_enumerate(p, "Cannot enumerate elements of set sort " + data::pp(sort));
          return;
        }
      }
      else if (sort_fset::is_fset(sort))
      {
        const container_sort& fset = atermpp::aterm_cast<container_sort>(sort);
        if (dataspec.is_certainly_finite(fset.element_sort()))
        {
          data_expression_vector set_elements;
          bool result = detail::compute_finite_set_elements(fset, dataspec, datar, sigma, set_elements);
          if (!result)
          {
            cannot_enumerate(p, "Finite set sort " + data::pp(sort) + " has too many elements to enumerate");
          }
          const data_expression old_substituted_value = sigma(v1);
          for (auto i = set_elements.begin(); i != set_elements.end(); ++i)
          {
            sigma[v1] = *i;
            auto phi1 = R_(phi, sigma);
            if (accept(phi1))
            {
              P.emplace_back(EnumeratorListElement(vtail, phi1, p, v1, *i));
            }
          }
          sigma[v1] = old_substituted_value;
        }
        else
        {
          cannot_enumerate(p, "Cannot enumerate elements of finite set sort " + data::pp(sort));
          return;
        }
      }
      else if (sort_bag::is_bag(sort))
      {
        cannot_enumerate(p, "Cannot enumerate elements of bag sort " + data::pp(sort));
        return;
      }
      else if (sort_fbag::is_fbag(sort))
      {
        cannot_enumerate(p, "Cannot enumerate elements of finite bag sort " + data::pp(sort));
        return;
      }
      else
      {
        auto const& C = constructors(sort);
        if (C.empty())
        {
          cannot_enumerate(p, "Cannot enumerate elements of sort without constructors " + data::pp(sort));
          return;
        }
        for (auto i = C.begin(); i != C.end(); ++i)
        {
          auto const& constructor = *i;
          if (data::is_function_sort(constructor.sort()))
          {
            auto const& domain = atermpp::aterm_cast<data::function_sort>(constructor.sort()).domain();
            data::variable_list y(domain.begin(), domain.end(), sort_name_generator(id_generator));
            data::application cy(constructor, y.begin(), y.end());
            sigma[v1] = cy;
            auto phi1 = R_(phi, sigma);
            sigma[v1] = v1;
            if (accept(phi1))
            {
              if (phi1 == phi)
              {
                P.emplace_back(EnumeratorListElement(vtail, phi1, p, v1, cy));
              }
              else
              {
                P.emplace_back(EnumeratorListElement(vtail + y, phi1, p, v1, cy));
              }
            }
          }
          else
          {
            sigma[v1] = constructor;
            auto phi1 = R_(phi, sigma);
            sigma[v1] = v1;
            if (accept(phi1))
            {
              P.emplace_back(EnumeratorListElement(vtail, phi1, p, v1, constructor));
            }
          }
        }
      }
    }

    /// \brief Enumerates the front elements of the todo list P until a solution
    /// has been found, or until P is empty.
    /// \param P The todo list of the algorithm.
    /// \param accept Elements p for which accept(p) is false are discarded.
    /// \return The number of elements that have been processed
    /// \post Either P.empty() or P.front().is_solution()
    template <typename EnumeratorListElement, typename MutableSubstitution, typename Filter>
    std::size_t next(std::deque<EnumeratorListElement>& P, MutableSubstitution& sigma, Filter accept) const
    {
      std::size_t count = 0;
      while (!P.empty())
      {
        if (P.front().is_solution())
        {
          break;
        }
        else
        {
          enumerate_front(P, sigma, accept);
          count++;
          if (count >= m_max_count)
          {
            break;
          }
        }
      }
      return count;
    }

    std::size_t max_count() const
    {
      return m_max_count;
    }

    bool throw_exceptions() const
    {
      return m_throw_exceptions;
    }
};

/// \brief An enumerator algorithm with an iterator interface.
template <typename Rewriter, typename MutableSubstitution, typename EnumeratorListElement, typename Filter>
class enumerator_algorithm_with_iterator: public enumerator_algorithm<Rewriter>
{
  public:
    typedef enumerator_algorithm<Rewriter> super;

    /// \brief A class to enumerate solutions for terms.
    /// \details Solutions are presented as data_expression_lists of the same length as
    ///          the list of variables for which a solution is sought.
    class iterator: public boost::iterator_facade<iterator, const EnumeratorListElement, boost::forward_traversal_tag>
    {
      protected:
        const enumerator_algorithm_with_iterator<Rewriter, MutableSubstitution, EnumeratorListElement, Filter>* E;
        MutableSubstitution* sigma;
        std::deque<EnumeratorListElement>* P;
        Filter accept;
        std::size_t count;

        static std::deque<EnumeratorListElement>& default_deque()
        {
          static std::deque<EnumeratorListElement> result;
          return result;
        }

      public:
        iterator(const enumerator_algorithm_with_iterator<Rewriter, MutableSubstitution, EnumeratorListElement, Filter>* E_,
                 std::deque<EnumeratorListElement>* P_,
                 MutableSubstitution* sigma_,
                 Filter accept_ = Filter()
                )
          : E(E_), sigma(sigma_), P(P_), accept(accept_), count(0)
        {
          count += E->next(*P, *sigma, accept);
        }

        iterator(Filter accept_ = Filter())
          : E(0), sigma(0), P(&default_deque()), accept(accept_), count(0)
        { }

      protected:
        friend class boost::iterator_core_access;

        void increment()
        {
          assert(!P->empty());
          if (count >= E->max_count())
          {
            if (E->throw_exceptions())
            {
              std::ostringstream out;
              out << "enumeration was aborted, since it did complete within " << E->max_count() << " iterations";
              throw mcrl2::runtime_error(out.str());
            }
            else
            {
              P->front().invalidate();
              return;
            }
          }
          P->pop_front();
          count += E->next(*P, *sigma, Filter());
        }

        bool equal(iterator const& other) const
        {
          return P->size() == other.P->size();
        }

        const EnumeratorListElement& dereference() const
        {
          assert(!P->empty());
          return P->front();
        }
    };

    enumerator_algorithm_with_iterator(
                const Rewriter& R,
                const data::data_specification& dataspec,
                const data::rewriter& datar,
                std::size_t max_count = (std::numeric_limits<std::size_t>::max)(),
                bool throw_exceptions = false)
      : super(R, dataspec, datar, max_count, throw_exceptions)
    {}

    /// \brief Returns an iterator that enumerates solutions for variables that satisfy a condition
    /// \param E An enumerator
    /// \param sigma A mutable substitution that is applied by the rewriter contained in E
    /// \param p The condition that is solved, together with the list of variables
    /// \param accept Enumerator elements p for which accept(p) is false are discarded.
    /// Otherwise an invalidated enumerator element is returned when it is dereferenced.
    iterator begin(MutableSubstitution& sigma, std::deque<EnumeratorListElement>& P, Filter accept = Filter()) const
    {
      assert(!P.empty());
      auto& p = P.front();
      p.expression() = super::R(p.expression(), sigma);
      if (accept(p.expression()))
      {
        return iterator(this, &P, &sigma, accept);
      }
      else
      {
        return end(accept);
      }
    }

    const iterator& end(Filter accept = Filter()) const
    {
      static iterator result(accept);
      return result;
    }
};

/// \brief Returns a vector with all expressions of sort s.
/// \detail It is assumed that the sort s has only a finite number of elements.
template <class Rewriter>
data_expression_vector enumerate_expressions(const sort_expression& s, const data_specification& dataspec, const Rewriter& rewr)
{
  typedef typename Rewriter::term_type term_type;
  typedef enumerator_list_element_with_substitution<term_type> enumerator_element;
  assert(dataspec.is_certainly_finite(s));
  enumerator_algorithm_with_iterator<Rewriter, mutable_indexed_substitution<>, enumerator_element, data::is_not_false> E(rewr, dataspec, rewr);
  data_expression_vector result;
  mutable_indexed_substitution<> sigma;
  const variable v("@var@", s);
  const variable_list vl = atermpp::make_list<variable>(v);
  std::deque<enumerator_list_element_with_substitution<data_expression> > P;
  P.emplace_back(enumerator_element(vl, sort_bool::true_()));
  for (auto i = E.begin(sigma, P); i != E.end(); ++i)
  {
    i->add_assignments(vl, sigma, rewr);
    result.push_back(sigma(v));
  }
  return result;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ENUMERATOR_H
