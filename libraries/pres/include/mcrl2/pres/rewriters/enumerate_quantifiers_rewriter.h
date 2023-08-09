// Author(s): Jan Friso Groote. Based on pbes/rewriters/enumerate_quantifiers_rewriter.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h
/// \brief A rewriter that enumerates minall, maxall and sum operators in pres expressions. 

#ifndef MCRL2_PRES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H
#define MCRL2_PRES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H

#include <numeric>
#include "mcrl2/atermpp/standard_containers/vector.h"
#include "mcrl2/data/detail/split_finite_variables.h"
#include "mcrl2/data/real_utilities.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/pres/rewriters/simplify_rewriter.h"

namespace mcrl2 {

namespace pres_system {

namespace detail {

// Simplifying PRES rewriter that eliminates quantifiers using enumeration.
/// \param MutableSubstitution This must be a MapSubstitution.
template <typename Derived, typename DataRewriter, typename MutableSubstitution>
struct enumerate_quantifiers_builder: public simplify_data_rewriter_builder<Derived, DataRewriter, MutableSubstitution>
{
  typedef simplify_data_rewriter_builder<Derived, DataRewriter, MutableSubstitution> super;
  typedef enumerate_quantifiers_builder<Derived, DataRewriter, MutableSubstitution> self;
  typedef data::enumerator_list_element<pres_expression> enumerator_element;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
  using super::sigma;

  const data::data_specification& m_dataspec;

  /// If true, quantifier variables of infinite sort are enumerated.
  bool m_enumerate_infinite_sorts;

  /// The enumerator
  data::enumerator_algorithm<self> E;

  /// \brief Constructor.
  /// \param r A data rewriter.
  /// \param sigma A mutable substitution.
  /// \param dataspec A data specification.
  /// \param id_generator A generator to generate fresh variable names.
  /// \param enumerate_infinite_sorts If true, quantifier variables of infinite sort are enumerated as well.
  enumerate_quantifiers_builder(const DataRewriter& r,
                                MutableSubstitution& sigma,
                                const data::data_specification& dataspec,
                                data::enumerator_identifier_generator& id_generator,
                                bool enumerate_infinite_sorts = true)
    : super(r, sigma), m_dataspec(dataspec), m_enumerate_infinite_sorts(enumerate_infinite_sorts), E(*this, m_dataspec, r, id_generator, (std::numeric_limits<std::size_t>::max)())
  { }

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  atermpp::vector<data::data_expression> undo_substitution(const data::variable_list& variables)
  {
    atermpp::vector<data::data_expression> result;
    for (const data::variable& v : variables)
    {
      result.push_back(sigma(v));
      sigma[v] = v;
    }
    return result;
  }

  void redo_substitution(const data::variable_list& v, const atermpp::vector<data::data_expression>& undo)
  {
    assert(v.size() == undo.size());
    auto i = v.begin();
    auto j = undo.begin();
    while (i != v.end())
    {
      sigma[*i++] = *j++;
    }
  }

  void enumerate_minall(pres_expression& result, const data::variable_list& v, const pres_expression& phi)
  {
    assert(&result!=&phi);
    auto undo = undo_substitution(v);
    result = true_();
    pres_expression phi_;
    derived().apply(phi_, phi);
    E.enumerate(enumerator_element(v, phi_),
                sigma,
                [&](const enumerator_element& p)
                {
                  data::optimized_and(result, result, p.expression());
                  return is_false(result);
                },
                is_true,
                is_false
               );

    redo_substitution(v, undo);
  }

  void enumerate_maxall(pres_expression& result, const data::variable_list& v, const pres_expression& phi)
  {
    assert(&result!=&phi);
    auto undo = undo_substitution(v);
    result = false_();
    pres_expression phi_;
    derived().apply(phi_, phi);
    E.enumerate(enumerator_element(v, phi_),
                sigma,
                [&](const enumerator_element& p)
                {
                  data::optimized_or(result, result, p.expression());
                  return is_true(result);
                },
                is_false,
                is_true
    );

    redo_substitution(v, undo);
  }

  void enumerate_sum(pres_expression& result, const data::variable_list& v, const pres_expression& phi)
  {
    assert(&result!=&phi);
    atermpp::vector<data::data_expression> undo = undo_substitution(v);
    result = data::sort_real::real_zero();
    pres_expression phi_;
    derived().apply(phi_, phi);
    E.enumerate(enumerator_element(v, phi_),
                sigma,
                [&](const enumerator_element& p)
                {
                  optimized_plus(result, result, p.expression());
                  return is_true(result);
                },
                data::sort_real::is_zero,  
                is_true
    );

    redo_substitution(v, undo);
  }

  template <class T>
  void apply(T& result, const minall& x)
  {
    if (m_enumerate_infinite_sorts)
    {
      data::variable_list enumerable;
      data::variable_list non_enumerable;
      data::detail::split_enumerable_variables(x.variables(), m_dataspec, super::R, enumerable, non_enumerable);
      enumerate_minall(result, enumerable, x.body());
      optimized_minall(result, non_enumerable, result);
    }
    else
    {
      data::variable_list finite;
      data::variable_list infinite;
      data::detail::split_finite_variables(x.variables(), m_dataspec, finite, infinite);
      if (finite.empty())
      {
        pres_expression body;
        derived().apply(body, x.body());
        optimized_minall(result, infinite, body);
      }
      else
      {
        enumerate_minall(result, finite, x.body());
        optimized_minall(result, infinite, result);
      }
    }
  }

  template <class T>
  void apply(T& result, const maxall& x)
  {
    if (m_enumerate_infinite_sorts)
    {
      data::variable_list enumerable;
      data::variable_list non_enumerable;
      data::detail::split_enumerable_variables(x.variables(), m_dataspec, super::R, enumerable, non_enumerable);
      enumerate_maxall(result, enumerable, x.body());
      optimized_maxall(result, non_enumerable, result);
    }
    else
    {
      data::variable_list finite;
      data::variable_list infinite;
      data::detail::split_finite_variables(x.variables(), m_dataspec, finite, infinite);
      if (finite.empty())
      {
        pres_expression body;
        derived().apply(body, x.body());
        optimized_maxall(result, infinite, body);
      }
      else
      {
        enumerate_maxall(result, finite, x.body());
        optimized_maxall(result, infinite, result);
      }
    }
  }

  template <class T>
  void apply(T& result, const sum& x)
  {
    if (m_enumerate_infinite_sorts)
    {
      data::variable_list enumerable;
      data::variable_list non_enumerable;
      data::detail::split_enumerable_variables(x.variables(), m_dataspec, super::R, enumerable, non_enumerable);
      enumerate_sum(result, enumerable, x.body());
      optimized_sum(result, non_enumerable, result);
    }
    else
    {
      data::variable_list finite;
      data::variable_list infinite;
      data::detail::split_finite_variables(x.variables(), m_dataspec, finite, infinite);
      if (finite.empty())
      {
        pres_expression body;
        derived().apply(body, x.body());
        optimized_sum(result, infinite, body);
      }
      else
      {
        enumerate_sum(result, finite, x.body());
        optimized_sum(result, infinite, result);
      }
    }
  }



  // N.B. This function has been added to make this class operate well with the enumerator.
  pres_expression operator()(const pres_expression& x, MutableSubstitution& sigma)
  {
    pres_expression result;
    derived().apply(result, x);
    return result;
  }

  // N.B. This function has been added to make this class operate well with the enumerator.
  //      that employs the "make_rewrite" variant with an explicit result. 
  void operator()(pres_expression& result, const pres_expression& x, MutableSubstitution& )
  {
    derived().apply(result, x);
  }
};

template <template <class, class, class> class Builder, class DataRewriter, class MutableSubstitution>
struct apply_enumerate_builder: public Builder<apply_enumerate_builder<Builder, DataRewriter, MutableSubstitution>, DataRewriter, MutableSubstitution>
{
  typedef Builder<apply_enumerate_builder<Builder, DataRewriter, MutableSubstitution>, DataRewriter, MutableSubstitution> super;
  using super::enter;
  using super::leave;

  apply_enumerate_builder(const DataRewriter& R, MutableSubstitution& sigma, const data::data_specification& dataspec, data::enumerator_identifier_generator& id_generator, bool enumerate_infinite_sorts)
    : super(R, sigma, dataspec, id_generator, enumerate_infinite_sorts)
  {}
}; 

template <template <class, class, class> class Builder, class DataRewriter, class MutableSubstitution>
apply_enumerate_builder<Builder, DataRewriter, MutableSubstitution>
make_apply_enumerate_builder(const DataRewriter& R, MutableSubstitution& sigma, const data::data_specification& dataspec, data::enumerator_identifier_generator& id_generator, bool enumerate_infinite_sorts)
{
  return apply_enumerate_builder<Builder, DataRewriter, MutableSubstitution>(R, sigma, dataspec, id_generator, enumerate_infinite_sorts);
}

} // namespace detail

/// \brief An attempt for improving the efficiency.
struct enumerate_quantifiers_rewriter
{
  protected:
    /// \brief A data rewriter
    data::rewriter m_rewriter;
    // pres_system::data_rewriter m_rewriter;

    /// \brief A data specification
    data::data_specification m_dataspec;

    /// \brief If true, quantifier variables of infinite sort are enumerated.
    bool m_enumerate_infinite_sorts;

    mutable data::enumerator_identifier_generator m_id_generator;

  public:
    typedef pres_expression term_type;
    typedef data::variable variable_type;

    enumerate_quantifiers_rewriter(const data::rewriter& R, const data::data_specification& dataspec, bool enumerate_infinite_sorts = true)
      : m_rewriter(R), m_dataspec(dataspec), m_enumerate_infinite_sorts(enumerate_infinite_sorts)
    {}

    pres_expression operator()(const pres_expression& x) const
    {
      data::rewriter::substitution_type sigma;
      pres_expression result;
      detail::apply_enumerate_builder<detail::enumerate_quantifiers_builder, data::rewriter, data::rewriter::substitution_type>(m_rewriter, sigma, m_dataspec, m_id_generator, m_enumerate_infinite_sorts).apply(result, x);
      return result;
    }

    template <typename MutableSubstitution>
    pres_expression operator()(const pres_expression& x, MutableSubstitution& sigma) const
    {
      pres_expression result;
      detail::apply_enumerate_builder<detail::enumerate_quantifiers_builder, data::rewriter, MutableSubstitution>(m_rewriter, sigma, m_dataspec, m_id_generator, m_enumerate_infinite_sorts).apply(result, x);
      return result;
    }

    template <typename MutableSubstitution>
    void operator()(pres_expression& result, const pres_expression& x, MutableSubstitution& sigma) const
    {
      detail::apply_enumerate_builder<detail::enumerate_quantifiers_builder, data::rewriter, MutableSubstitution>(m_rewriter, sigma, m_dataspec, m_id_generator, m_enumerate_infinite_sorts).apply(result, x);
    }

    void clear_identifier_generator()
    {
      m_id_generator.clear();
    }

    /// \brief Create a clone of the rewriter in which the underlying rewriter is copied, and not passed as a shared pointer. 
    /// \details This is useful when the rewriter is used in different parallel processes. One rewriter can only be used sequentially. 
    /// \return A rewriter, with a copy of the underlying jitty, jittyc or jittyp rewriting engine. 
    enumerate_quantifiers_rewriter clone()
    {
      return enumerate_quantifiers_rewriter(m_rewriter.clone(), m_dataspec, m_enumerate_infinite_sorts);
    }

    /// \brief Initialises this rewriter with thread dependent information. 
    /// \details This function sets a pointer to the m_busy_flag and m_forbidden_flag of this.
    ///          process, such that rewriter can access these faster than via the general thread.
    ///          local mechanism. It is expected that this is not needed when compilers become.
    ///          faster, and should be removed in due time. 
    void thread_initialise()
    {
      m_rewriter.thread_initialise();
    }
};

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H

