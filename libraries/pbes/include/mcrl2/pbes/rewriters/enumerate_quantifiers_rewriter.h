// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H
#define MCRL2_PBES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H

#include <numeric>
#include <set>
#include <utility>
#include <deque>
#include <sstream>
#include <vector>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/split_finite_variables.h"
#include "mcrl2/pbes/rewriters/simplify_rewriter.h"
#include "mcrl2/pbes/enumerator.h"
#include "mcrl2/utilities/optimized_boolean_operators.h"
#include "mcrl2/utilities/detail/join.h"
#include "mcrl2/utilities/optimized_boolean_operators.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

// Simplifying PBES rewriter that eliminates quantifiers using enumeration.
/// \param MutableSubstitution This must be a MapSubstitution.
template <typename Derived, typename DataRewriter, typename MutableSubstitution>
struct enumerate_quantifiers_builder: public simplify_data_rewriter_builder<Derived, DataRewriter, MutableSubstitution>
{
  typedef simplify_data_rewriter_builder<Derived, DataRewriter, MutableSubstitution> super;
  typedef enumerate_quantifiers_builder<Derived, DataRewriter, MutableSubstitution> self;
  typedef core::term_traits<pbes_expression> tr;
  typedef data::enumerator_list_element<pbes_expression> enumerator_element;

  using super::enter;
  using super::leave;
  using super::operator();
  using super::sigma;
  using super::R;

  const data::data_specification& m_dataspec;

  /// If true, quantifier variables of infinite sort are enumerated.
  bool m_enumerate_infinite_sorts;

  /// The enumerator
  data::enumerator_algorithm<self> E;

  /// \brief Constructor.
  /// \param r A data rewriter
  /// \param dataspec A data specification
  /// \param enumerate_infinite_sorts If true, quantifier variables of infinite sort are enumerated as well
  enumerate_quantifiers_builder(const DataRewriter& R, MutableSubstitution& sigma, const data::data_specification& dataspec, data::enumerator_identifier_generator& id_generator, bool enumerate_infinite_sorts = true)
    : super(R, sigma), m_dataspec(dataspec), m_enumerate_infinite_sorts(enumerate_infinite_sorts), E(*this, m_dataspec, R, id_generator)
  {
    id_generator.clear();
  }

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  std::vector<data::data_expression> undo_substitution(const data::variable_list& v)
  {
    std::vector<data::data_expression> result;
    for (auto i = v.begin(); i != v.end(); ++i)
    {
      result.push_back(sigma(*i));
      sigma[*i] = *i;
    }
    return result;
  }

  void redo_substitution(const data::variable_list& v, const std::vector<data::data_expression>& undo)
  {
    assert(v.size() == undo.size());
    auto i = v.begin();
    auto j = undo.begin();
    while (i != v.end())
    {
      sigma[*i++] = *j++;
    }
  }

  pbes_expression enumerate_forall(const data::variable_list& v, const pbes_expression& phi)
  {
    auto undo = undo_substitution(v);
    pbes_expression result = tr::true_();
    std::deque<enumerator_element> P;
    P.push_back(enumerator_element(v, derived()(phi)));
    E.next(P, sigma, is_not_true());
    while (!P.empty())
    {
      result = utilities::optimized_and(result, P.front().expression());
      P.pop_front();
      if (tr::is_false(result))
      {
        break;
      }
      E.next(P, sigma, is_not_true());
    }
    redo_substitution(v, undo);
    return result;
  }

  pbes_expression enumerate_exists(const data::variable_list& v, const pbes_expression& phi)
  {
    auto undo = undo_substitution(v);
    pbes_expression result = tr::false_();
    std::deque<enumerator_element> P;
    P.push_back(enumerator_element(v, derived()(phi)));
    E.next(P, sigma, is_not_false());
    while (!P.empty())
    {
      result = utilities::optimized_or(result, P.front().expression());
      P.pop_front();
      if (tr::is_true(result))
      {
        break;
      }
      E.next(P, sigma, is_not_false());
    }
    redo_substitution(v, undo);
    return result;
  }

  pbes_expression operator()(const forall& x)
  {
    pbes_expression result;
    if (m_enumerate_infinite_sorts)
    {
      result = enumerate_forall(x.variables(), x.body());
    }
    else
    {
      data::variable_list finite;
      data::variable_list infinite;
      data::detail::split_finite_variables(x.variables(), m_dataspec, finite, infinite);
      if (finite.empty())
      {
        result = utilities::optimized_forall(infinite, derived()(x.body()));
      }
      else
      {
        result = enumerate_forall(finite, x.body());
        result = utilities::optimized_forall_no_empty_domain(infinite, result);
      }
    }
    return result;
  }

  pbes_expression operator()(const exists& x)
  {
    pbes_expression result;
    if (m_enumerate_infinite_sorts)
    {
      result = enumerate_exists(x.variables(), x.body());
    }
    else
    {
      data::variable_list finite;
      data::variable_list infinite;
      data::detail::split_finite_variables(x.variables(), m_dataspec, finite, infinite);
      if (finite.empty())
      {
        result = utilities::optimized_exists(infinite, derived()(x.body()));
      }
      else
      {
        result = enumerate_exists(finite, x.body());
        result = utilities::optimized_exists_no_empty_domain(infinite, result);
      }
    }
    return result;
  }

  // N.B. This function has been added to make this class operate well with the enumerator.
  pbes_expression operator()(const pbes_expression& x, MutableSubstitution&)
  {
    return derived()(x);
  }
};

template <template <class, class, class> class Builder, class DataRewriter, class MutableSubstitution>
struct apply_enumerate_builder: public Builder<apply_enumerate_builder<Builder, DataRewriter, MutableSubstitution>, DataRewriter, MutableSubstitution>
{
  typedef Builder<apply_enumerate_builder<Builder, DataRewriter, MutableSubstitution>, DataRewriter, MutableSubstitution> super;
  using super::enter;
  using super::leave;
  using super::operator();

  apply_enumerate_builder(const DataRewriter& R, MutableSubstitution& sigma, const data::data_specification& dataspec, data::enumerator_identifier_generator& id_generator, bool enumerate_infinite_sorts)
    : super(R, sigma, dataspec, id_generator, enumerate_infinite_sorts)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
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
  /// \brief A data rewriter
  data::rewriter m_rewriter;

  /// \brief A data specification
  data::data_specification m_dataspec;

  /// \brief If true, quantifier variables of infinite sort are enumerated.
  bool m_enumerate_infinite_sorts;

  mutable data::enumerator_identifier_generator m_id_generator;

  typedef pbes_expression term_type;
  typedef data::variable variable_type;

  enumerate_quantifiers_rewriter(const data::rewriter& R, const data::data_specification& dataspec, bool enumerate_infinite_sorts = true)
    : m_rewriter(R), m_dataspec(dataspec), m_enumerate_infinite_sorts(enumerate_infinite_sorts)
  {}

  pbes_expression operator()(const pbes_expression& x) const
  {
    data::rewriter::substitution_type sigma;
    m_id_generator.clear();
    return detail::apply_enumerate_builder<detail::enumerate_quantifiers_builder, data::rewriter, data::rewriter::substitution_type>(m_rewriter, sigma, m_dataspec, m_id_generator, m_enumerate_infinite_sorts)(x);
  }

  template <typename MutableSubstitution>
  pbes_expression operator()(const pbes_expression& x, MutableSubstitution& sigma) const
  {
    m_id_generator.clear();
    return detail::apply_enumerate_builder<detail::enumerate_quantifiers_builder, data::rewriter, MutableSubstitution>(m_rewriter, sigma, m_dataspec, m_id_generator, m_enumerate_infinite_sorts)(x);
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H

