// Author(s): Wieger Wesselink
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

namespace mcrl2
{

namespace data
{

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

    bool is_solution() const
    {
      return v.empty();
    }

    /// \brief Invalidates the element, by giving phi an undefined value
    void invalidate()
    {
      phi = data::undefined_data_expression();
    }

    /// \brief Returns true if the element has been invalidated. This is used to signal that
    /// the enumeration has been aborted.
    bool is_invalid() const
    {
      return phi == data::undefined_data_expression();
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

    /// \brief Adds the assignments that corresponds with this element to the substitution result.
    template <typename MutableSubstitution>
    void add_assignments(const data::variable_list& v, MutableSubstitution& result) const
    {
      data::enumerator_substitution sigma(m_variables, m_expressions);
      sigma.revert();
      for (auto i = v.begin(); i != v.end(); ++i)
      {
        result[*i] = sigma(*i);
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
    Rewriter& R;

    /// \brief A data specification.
    const data::data_specification& dataspec;

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

    bool is_finite_set(const data::sort_expression& x) const
    {
      return false;
    }

  public:
    enumerator_algorithm(Rewriter& R_,
                         const data::data_specification& dataspec_,
                         std::size_t max_count = std::numeric_limits<std::size_t>::max(),
                         bool throw_exceptions = false
                       )
      : R(R_), dataspec(dataspec_), m_max_count(max_count), m_throw_exceptions(throw_exceptions)
    {}

    /// \brief Enumerates the front element of the todo list P.
    /// \param P The todo list of the algorithm.
    /// \param sigma A mutable substitution that is applied by the rewriter.
    /// \param accept Elements p for which accept(p) is false are discarded.
    /// \pre !P.empty()
    template <typename EnumeratorListElement, typename MutableSubstitution, typename Filter>
    void enumerate_front(std::deque<EnumeratorListElement>& P, MutableSubstitution& sigma, Filter accept) const
    {
      assert(!P.empty());

      auto p = P.front();
      auto const& v = p.variables();
      auto const& phi = p.expression();
      mCRL2log(log::debug) << "  process " << p << std::endl;
      P.pop_front();

      auto const& v1 = v.front();
      auto const& vtail = v.tail();
      auto const& C = constructors(v1.sort());

      if (!C.empty())
      {
        for (auto i = C.begin(); i != C.end(); ++i)
        {
          auto const& constructor = *i;
          if (data::is_function_sort(constructor.sort()))
          {
            auto const& domain = atermpp::aterm_cast<data::function_sort>(constructor.sort()).domain();
            data::variable_list y(domain.begin(), domain.end(), sort_name_generator(id_generator));
            data::application cy(constructor, y.begin(), y.end());
            sigma[v1] = cy;
            auto phi1 = R(phi, sigma);
            sigma[v1] = v1;
            if (accept(phi1))
            {
              if (phi1 == phi)
              {
                P.push_back(EnumeratorListElement(vtail, phi1, p, v1, cy));
              }
              else
              {
                P.push_back(EnumeratorListElement(vtail + y, phi1, p, v1, cy));
              }
              mCRL2log(log::debug) << "  add " << P.back() << " with " << v1 << " := " << cy << std::endl;
            }
          }
          else
          {
            sigma[v1] = constructor;
            auto phi1 = R(phi, sigma);
            sigma[v1] = v1;
            if (accept(phi1))
            {
              P.push_back(EnumeratorListElement(vtail, phi1, p, v1, constructor));
              mCRL2log(log::debug) << "  add " << P.back() << " with " << v1 << " := " << constructor << std::endl;
            }
          }
        }
      }

//      else if (is_finite_set(x1.sort()))
//      {
//      }
//      else if (data::is_function_sort(x1.sort()))
//      {
//        mCRL2log(log::debug) << "Function sort: " << x1.sort() << std::endl;
//      }
      else
      {
        throw mcrl2::runtime_error("Cannot enumerate variable " + print(v1));
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
          mCRL2log(log::debug) << "  solution " << P.front() << std::endl;
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

    bool max_count() const
    {
      return m_max_count;
    }

    bool throw_exceptions() const
    {
      return m_throw_exceptions;
    }
};

/// \brief An enumerator algorithm with an iterator interface.
template <typename Rewriter, typename MutableSubstitution, typename EnumeratorListElement>
class enumerator_algorithm_with_iterator: public enumerator_algorithm<Rewriter>
{
  public:
    typedef enumerator_algorithm<Rewriter> super;

    /// \brief A class to enumerate solutions for terms.
    /// \details Solutions are presented as data_expression_lists of the same length as
    ///          the list of variables for which a solution is sought.
    template <typename Filter>
    class iterator: public boost::iterator_facade<iterator<Filter>, const EnumeratorListElement, boost::forward_traversal_tag>
    {
      protected:
        const enumerator_algorithm_with_iterator<Rewriter, MutableSubstitution, EnumeratorListElement>* E;
        MutableSubstitution* sigma;
        std::deque<EnumeratorListElement> P;
        Filter accept;
        std::size_t count;

      public:
        iterator(const enumerator_algorithm_with_iterator<Rewriter, MutableSubstitution, EnumeratorListElement>* E_,
                 MutableSubstitution* sigma_,
                 const EnumeratorListElement& p,
                 Filter accept_
                )
          : E(E_), sigma(sigma_), accept(accept_), count(0)
        {
          P.push_back(p);
          count += E->next(P, *sigma, accept);
        }

        iterator(Filter accept_)
          : E(0), sigma(0), accept(accept_), count(0)
        { }

      protected:
        friend class boost::iterator_core_access;

        void increment()
        {
          assert(!P.empty());
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
              P.front().invalidate();
              return;
            }
          }
          P.pop_front();
          count += E->next(P, *sigma, Filter());
        }

        bool equal(iterator<Filter> const& other) const
        {
          return P.size() == other.P.size();
        }

        const EnumeratorListElement& dereference() const
        {
          assert(!P.empty());
          return P.front();
        }
    };

    enumerator_algorithm_with_iterator(Rewriter& R, const data::data_specification& dataspec)
      : super(R, dataspec)
    {}

    /// \brief Returns an iterator that enumerates solutions for variables that satisfy a condition
    /// \param E An enumerator
    /// \param sigma A mutable substitution that is applied by the rewriter contained in E
    /// \param p The condition that is solved, together with the list of variables
    /// \param accept Enumerator elements p for which accept(p) is false are discarded.
    /// Otherwise an invalidated enumerator element is returned when it is dereferenced.
    template <typename Filter>
    iterator<Filter> begin(MutableSubstitution& sigma, const EnumeratorListElement& p, Filter accept) const
    {
      return iterator<Filter>(this, &sigma, p, accept);
    }

    template <typename Filter>
    iterator<Filter> end(Filter accept) const
    {
      return iterator<Filter>(accept);
    }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ENUMERATOR_H
