// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/enumerator_with_iterator.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_ENUMERATOR_WITH_ITERATOR_H
#define MCRL2_DATA_ENUMERATOR_WITH_ITERATOR_H

#include "mcrl2/atermpp/detail/aterm_configuration.h"
#include "mcrl2/data/enumerator.h"

namespace mcrl2::data
{

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

/// \brief An enumerator algorithm that generates solutions of a condition.
template <typename Rewriter = data::rewriter, typename DataRewriter = data::rewriter>
class enumerator_algorithm_without_callback: public enumerator_algorithm<Rewriter, DataRewriter>
{
  protected:
    using super = enumerator_algorithm<Rewriter, DataRewriter>;
    using super::r;
    using super::R;
    using super::dataspec;
    using super::id_generator;
    using super::m_max_count;
    using super::rewrite;

    /// \brief throw_exceptions If true, an exception is thrown when the enumeration is aborted.
    bool m_throw_exceptions;

    template <typename EnumeratorListElement>
    void cannot_enumerate(EnumeratorListElement& e, const std::string& msg) const
    {
      e.invalidate();
      if (m_throw_exceptions)
      {
        throw mcrl2::runtime_error(msg);
      }
    }

    // add element without additional variables
    template <typename EnumeratorListElement, typename MutableSubstitution, typename Filter, typename Expression>
    void add_element(enumerator_queue<EnumeratorListElement>& P,
                     MutableSubstitution& sigma,
                     Filter accept,
                     const data::variable_list& variables,
                     const Expression& phi,
                     const EnumeratorListElement& p,
                     const data::variable& v,
                     const data::data_expression& e) const
    {
      auto phi1 = rewrite(phi, sigma);
      if (accept(phi1))
      {
        P.push_back(EnumeratorListElement(variables, phi1, p, v, e));
      }
    }

    // add element with additional variables
    template <typename EnumeratorListElement, typename MutableSubstitution, typename Filter, typename Expression>
    void add_element(enumerator_queue<EnumeratorListElement>& P,
                     MutableSubstitution& sigma,
                     Filter accept,
                     const data::variable_list& variables,
                     const data::variable_list& added_variables,
                     const Expression& phi,
                     const EnumeratorListElement& p,
                     const data::variable& v,
                     const data::data_expression& e) const
    {
      auto phi1 = rewrite(phi, sigma);
      if (accept(phi1))
      {
        // Additional variables are put at the end of the list!
        P.push_back(EnumeratorListElement(variables + added_variables, phi1, p, v, e));
      }
    }

    // specialization for enumerator_list_element; in this case we are not interested in the substitutions,
    // and this allows an optimization
    template <typename MutableSubstitution, typename Filter, typename Expression>
    void add_element(enumerator_queue<enumerator_list_element<Expression>>& P,
                     MutableSubstitution& sigma,
                     Filter accept,
                     const data::variable_list& variables,
                     const data::variable_list& added_variables,
                     const Expression& phi,
                     const enumerator_list_element<Expression>& p,
                     const data::variable& v,
                     const data::data_expression& e) const
    {
      auto phi1 = rewrite(phi, sigma);
      if (accept(phi1))
      {
        if (phi1 == phi)
        {
          // Discard the added_variables, since we know they do not appear in phi1
          P.push_back(enumerator_list_element<Expression>(variables, phi1, p, v, e));
        }
        else
        {
          // Additional variables are put at the end of the list!
          P.push_back(enumerator_list_element<Expression>(variables + added_variables, phi1, p, v, e));
        }
        //mCRL2log(log::debug) << "  <add-element> " << P.back() << " with assignment " << v << " := " << e << std::endl;
      }
    }

  public:
    enumerator_algorithm_without_callback(const Rewriter& R,
                         const data::data_specification& dataspec,
                         const DataRewriter& r,
                         enumerator_identifier_generator& id_generator,
                         std::size_t max_count = (std::numeric_limits<std::size_t>::max)(),
                         bool throw_exceptions = false
    )
            : super(R, dataspec, r, id_generator, false, max_count), m_throw_exceptions(throw_exceptions)
    {}

    enumerator_algorithm_without_callback(const enumerator_algorithm<Rewriter, DataRewriter>&) = delete;

    template <typename T>
    struct always_false
    {
      bool operator()(const T&) { return false; }
    };

    /// \brief Enumerates the front element of the todo list P.
    /// \param P The todo list of the algorithm.
    /// \param sigma A mutable substitution that is applied by the rewriter.
    /// \param accept Elements p for which accept(p) is false are discarded.
    /// \pre !P.empty()
    template <typename EnumeratorListElement, typename MutableSubstitution, typename Filter>
    void step(enumerator_queue<EnumeratorListElement>& P, MutableSubstitution& sigma, Filter accept) const
    {
      assert(!P.empty());

      auto p = P.front();
      const auto& v = p.variables();
      const auto& phi = p.expression();
      //mCRL2log(log::debug) << "  <process-element> " << p << std::endl;
      P.pop_front();

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
            cannot_enumerate(p, "Sort " + data::pp(v1_sort) + " has too many elements to enumerate.");
          }

          data_expression sigma_v1 = sigma(v1);
          for (const data_expression& f: function_sorts)
          {
            sigma[v1] = f;
            add_element(P, sigma, accept, v_tail, phi, p, v1, f);
          }
          sigma[v1] = sigma_v1;
        }
        else
        {
          cannot_enumerate(p, "Cannot enumerate elements of function sort " + data::pp(v1_sort) + ".");
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
          add_element(P, sigma, accept, v_tail, { fset_variable }, phi, p, v1, e);
          sigma[v1] = sigma_v1;
        }
        else
        {
          cannot_enumerate(p, "Cannot enumerate elements of set sort " + data::pp(v1_sort) + ".");
          return;
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
            cannot_enumerate(p, "Finite set sort " + data::pp(v1_sort) + " has too many elements to enumerate.");
          }

          data_expression sigma_v1 = sigma(v1);
          for (const data_expression& e: set_elements)
          {
            sigma[v1] = e;
            add_element(P, sigma, accept, v_tail, phi, p, v1, e);
          }
          sigma[v1] = sigma_v1;
        }
        else
        {
          cannot_enumerate(p, "Cannot enumerate elements of finite set sort " + data::pp(v1_sort) + ".");
          return;
        }
      }
      else if (sort_bag::is_bag(v1_sort))
      {
        cannot_enumerate(p, "Cannot enumerate elements of bag sort " + data::pp(v1_sort) + ".");
        return;
      }
      else if (sort_fbag::is_fbag(v1_sort))
      {
        cannot_enumerate(p, "Cannot enumerate elements of finite bag sort " + data::pp(v1_sort) + ".");
        return;
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
              // TODO: We want to apply datar without the substitution sigma, but that is currently an inefficient operation of data::rewriter.
              data_expression cy = r(application(c, y.begin(), y.end()), sigma);
              sigma[v1] = cy;
              add_element(P, sigma, accept, v_tail, y, phi, p, v1, cy);
              sigma[v1] = v1;
            }
            else
            {
              // TODO: We want to apply r without the substitution sigma, but that is currently an inefficient operation of data::rewriter.
              const auto e1 = r(c, sigma);
              sigma[v1] = e1;
              add_element(P, sigma, accept, v_tail, phi, p, v1, e1);
              sigma[v1] = v1;
            }
          }
        }
        else
        {
          cannot_enumerate(p, "Cannot enumerate elements of sort " + data::pp(v1_sort) + " without constructors.");
          return;
        }
      }
    }

    /// \brief Enumerates the front elements of the todo list P until a solution
    /// has been found, or until P is empty.
    /// \param P The todo list of the algorithm.
    /// \param sigma A substitution.
    /// \param accept Elements p for which accept(p) is false are discarded.
    /// \return The number of elements that have been processed
    /// \post Either P.empty() or P.front().is_solution()
    template <typename EnumeratorListElement, typename MutableSubstitution, typename Filter>
    std::size_t next(enumerator_queue<EnumeratorListElement>& P, MutableSubstitution& sigma, Filter accept) const
    {
      std::size_t count = 0;
      while (!P.empty())
      {
        if (count++ >= m_max_count)
        {
          break;
        }
        if (P.front().is_solution())
        {
          break;
        }
        else
        {
          step(P, sigma, accept);
        }
      }
      return count;
    }

    bool throw_exceptions() const
    {
      return m_throw_exceptions;
    }
};

/// \brief An enumerator algorithm with an iterator interface.
template <typename Rewriter = data::rewriter, typename EnumeratorListElement = enumerator_list_element_with_substitution<>, typename Filter = data::is_not_false, typename DataRewriter = data::rewriter, typename MutableSubstitution = data::mutable_indexed_substitution<> >
class enumerator_algorithm_with_iterator: public enumerator_algorithm_without_callback<Rewriter, DataRewriter>
{
  protected:
    Filter m_accept;

  public:
    using super = enumerator_algorithm_without_callback<Rewriter, DataRewriter>;
    using super::rewrite;

    /// \brief A class to enumerate solutions for terms.
    /// \details Solutions are presented as data_expression_lists of the same length as
    ///          the list of variables for which a solution is sought.
    class iterator: public boost::iterator_facade<iterator, const EnumeratorListElement, boost::forward_traversal_tag>
    {
      protected:
        enumerator_algorithm_with_iterator<Rewriter, EnumeratorListElement, Filter, DataRewriter, MutableSubstitution>* E;
        MutableSubstitution* sigma;
        enumerator_queue<EnumeratorListElement>* P;
        Filter accept;
        std::size_t count;

        static enumerator_queue<EnumeratorListElement>& default_deque()
        {
#ifdef MCRL2_ENABLE_MULTITHREADING 
          static_assert(mcrl2::utilities::detail::GlobalThreadSafe);
          thread_local enumerator_queue<EnumeratorListElement> result; // Changed this static variable to thread local,
                                                                       // as it could be the cause of a thread conflict. 
#else
          static_assert(!mcrl2::utilities::detail::GlobalThreadSafe);
          static enumerator_queue<EnumeratorListElement> result; // Changed this static variable to thread local,
#endif
          return result;
        }

      public:
        iterator(enumerator_algorithm_with_iterator<Rewriter, EnumeratorListElement, Filter, DataRewriter, MutableSubstitution>* E_,
                 enumerator_queue<EnumeratorListElement>* P_,
                 MutableSubstitution* sigma_,
                 Filter accept_ = Filter()
        )
                : E(E_), sigma(sigma_), P(P_), accept(accept_), count(0)
        {
          count += E->next(*P, *sigma, accept);
        }

        explicit iterator(Filter accept_ = Filter())
                : E(nullptr), sigma(nullptr), P(&default_deque()), accept(accept_), count(0)
        { }

      protected:
        friend class boost::iterator_core_access;

        void increment()
        {
          assert(!P->empty());
          P->pop_front();
          count += E->next(*P, *sigma, accept);
          if (count >= E->max_count())
          {
            if (E->throw_exceptions())
            {
              std::ostringstream out;
              out << "enumeration was aborted, since it did not complete within " << E->max_count() << " iterations";
              throw mcrl2::runtime_error(out.str());
            }
            else
            {
              P->clear();
              return;
            }
          }
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
            const DataRewriter& datar,
            enumerator_identifier_generator& id_generator,
            std::size_t max_count = (std::numeric_limits<std::size_t>::max)(),
            bool throw_exceptions = false,
            const Filter& f = Filter())
            : super(R, dataspec, datar, id_generator, max_count, throw_exceptions)
            , m_accept(f)
    {}

    /// \brief Returns an iterator that enumerates solutions for variables that satisfy a condition
    /// \param sigma A mutable substitution that is applied by the rewriter contained in E
    /// \param P The condition that is solved, together with the list of variables
    /// Otherwise an invalidated enumerator element is returned when it is dereferenced.
    iterator begin(MutableSubstitution& sigma, enumerator_queue<EnumeratorListElement>& P)
    {
      assert(P.size() == 1);
      auto& p = P.front();
      p.expression() = rewrite(p.expression(), sigma);
      if (m_accept(p.expression()))
      {
        return iterator(const_cast<enumerator_algorithm_with_iterator<Rewriter, EnumeratorListElement, Filter, DataRewriter, MutableSubstitution>*>(this), &P, &sigma, m_accept);
      }
      else
      {
        return end();
      }
    }

    const iterator& end()
    {
#ifdef MCRL2_ENABLE_MULTITHREADING
      static_assert(mcrl2::utilities::detail::GlobalThreadSafe);
      thread_local iterator result(m_accept);  
#else
      static_assert(!mcrl2::utilities::detail::GlobalThreadSafe);
      static iterator result(m_accept);  
#endif
      return result;
    }
};

} // namespace mcrl2::data

#endif // MCRL2_DATA_ENUMERATOR_WITH_ITERATOR_H
