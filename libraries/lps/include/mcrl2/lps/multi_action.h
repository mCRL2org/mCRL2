// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/multi_action.h
/// \brief Multi-action class.

#ifndef MCRL2_LPS_MULTI_ACTION_H
#define MCRL2_LPS_MULTI_ACTION_H

#include <iterator>
#include <stdexcept>
#include <sstream>
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/core/detail/struct_core.h" // gsMakeNil
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/utility.h" // substitute
#include "mcrl2/lps/action.h"
#include "mcrl2/core/print.h"

namespace mcrl2
{

namespace lps
{

/// \brief Represents a multi action
/// \detail Multi actions consist of a list of actions together with an optional time tag.
class multi_action
{
    friend class action_summand;
    friend struct atermpp::aterm_traits<multi_action>;

  protected:
    /// \brief The actions of the summand
    action_list m_actions;

    /// \brief The time of the summand. If <tt>m_time == data::data_expression()</tt>
    /// the multi action has no time.
    data::data_expression m_time;

    /// \brief Protects the term from being freed during garbage collection.
    void protect() const
    {
      m_time.protect();
      m_actions.protect();
    }

    /// \brief Unprotect the term.
    /// Releases protection of the term which has previously been protected through a
    /// call to protect.
    void unprotect() const
    {
      m_time.unprotect();
      m_actions.unprotect();
    }

    /// \brief Mark the term for not being garbage collected.
    void mark() const
    {
      m_time.mark();
      m_actions.mark();
    }

  public:
    /// \brief Constructor
    multi_action(action_list actions = action_list(), data::data_expression time = atermpp::aterm_appl(core::detail::gsMakeNil()))
      : m_actions(actions), m_time(time)
    {}

    /// \brief Constructor
    multi_action(const atermpp::aterm_appl& t) : m_time(core::detail::gsMakeNil())
    {
      assert(core::detail::gsIsAction(t) || core::detail::gsIsMultAct(t));
      m_actions = (core::detail::gsIsAction(t)) ? atermpp::term_list< action >(atermpp::make_list(t)) : atermpp::term_list< action >(t(0));
    }

    /// \brief Constructor
    multi_action(const action& l)
      : m_actions(atermpp::push_front(action_list(), l)),
        m_time(core::detail::gsMakeNil())
    {}

    /// \brief Returns true if time is available.
    /// \return True if time is available.
    bool has_time() const
    {
      // TODO: remove the Nil
      return m_time != core::detail::gsMakeNil();
    }

    /// \brief Returns the sequence of actions.
    /// \return The sequence of actions.
    const action_list& actions() const
    {
      return m_actions;
    }

    /// \brief Returns the sequence of actions.
    /// \return The sequence of actions.
    action_list& actions()
    {
      return m_actions;
    }

    /// \brief Returns the time.
    /// \return The time.
    const data::data_expression& time() const
    {
      return m_time;
    }

    /// \brief Returns the time.
    /// \return The time.
    data::data_expression& time()
    {
      return m_time;
    }

    /// \brief Returns the name of the first action.
    /// \return The name of the first action.
    core::identifier_string name() const
    {
      return front(m_actions).label().name();
    }

    /// \brief Returns the arguments of the multi action.
    /// \return The arguments of the multi action.
    data::data_expression_list arguments() const
    {
      return front(m_actions).arguments();
    }

    /// \brief Joins the actions of both multi actions.
    /// \pre The time of both multi actions must be equal.
    multi_action operator+(const multi_action& other) const
    {
      assert(m_time == other.m_time);
      return multi_action(m_actions + other.m_actions, m_time);
    }

    /// \brief Comparison operator
    bool operator==(const multi_action& other) const
    {
      return m_actions == other.m_actions && m_time == other.m_time;
    }

    /// \brief Comparison operator
    bool operator!=(const multi_action& other) const
    {
      return !(*this == other);
    }

    /// \brief Inequality on multi_actions
    bool operator<(const multi_action& other) const
    {
      return m_actions < other.m_actions || (m_actions == other.m_actions && m_time < other.m_time);
    }
};

/// \brief Returns true if the term t is a multi action
inline
bool is_multi_action(const atermpp::aterm_appl& t)
{
  return core::detail::gsIsMultAct(t);
}

// template function overloads
std::string pp(const multi_action& x);
void normalize_sorts(multi_action& x, const data::data_specification& dataspec);
void translate_user_notation(lps::multi_action& x);
std::set<data::variable> find_variables(const lps::multi_action& x);
std::set<data::variable> find_free_variables(const lps::multi_action& x);

/// \cond INTERNAL_DOCS
namespace detail
{

/// \brief Conversion to ATermAppl.
/// \return The multi action converted to ATerm format.
inline
atermpp::aterm_appl multi_action_to_aterm(const multi_action& m)
{
  return core::detail::gsMakeMultAct(m.actions());
}

/// \brief Visits all permutations of the arrays, and calls f for each instance.
/// \pre The range [first, last) contains sorted arrays.
/// \param first Start of a sequence of arrays
/// \param last End of a sequence of arrays
/// \param f A function
template <typename Iter, typename Function>
void forall_permutations(Iter first, Iter last, Function f)
{
  if (first == last)
  {
    f();
    return;
  }
  Iter next = first;
  ++next;
  forall_permutations(next, last, f);
  while (std::next_permutation(first->first, first->second))
  {
    forall_permutations(next, last, f);
  }
}

/// \brief Returns true if the actions in a and b have the same names, and the same sorts.
/// \pre a and b are sorted w.r.t. to the names of the actions.
/// \param a A sequence of actions
/// \param b A sequence of actions
/// \return True if the actions in a and b have the same names, and the same sorts.
inline bool equal_action_signatures(const std::vector<action>& a, const std::vector<action>& b)
{
  if (a.size() != b.size())
  {
    return false;
  }
  std::vector<action>::const_iterator i, j;
  for (i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j)
  {
    if (i->label() != j->label())
    {
      return false;
    }
  }
  return true;
}

/// \brief Compares action labels
struct compare_action_labels
{
  /// \brief Function call operator
  /// \param a An action
  /// \param b An action
  /// \return The function result
  bool operator()(const action& a, const action& b) const
  {
    return a.label() < b.label();
  }
};

/// \brief Compares action labels and arguments
struct compare_action_label_arguments
{
  /// \brief Function call operator
  /// \param a An action
  /// \param b An action
  /// \return The function result
  bool operator()(const action& a, const action& b) const
  {
    if (a.label() != b.label())
    {
      return a.label() < b.label();
    }
    return a < b;
  }
};

/// \brief Used for building an expression for the comparison of data parameters.
struct equal_data_parameters_builder
{
  const std::vector<action>& a;
  const std::vector<action>& b;
  std::set<data::data_expression>& result;

  equal_data_parameters_builder(const std::vector<action>& a_,
                                const std::vector<action>& b_,
                                std::set<data::data_expression>& result_
                               )
    : a(a_),
      b(b_),
      result(result_)
  {}

  /// \brief Adds the expression 'a == b' to result.
  void operator()()
  {
    using namespace data::lazy;
    namespace d = data;

    std::vector<data::data_expression> v;
    std::vector<action>::const_iterator i, j;
    for (i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j)
    {
      data::data_expression_list d1 = i->arguments();
      data::data_expression_list d2 = j->arguments();
      assert(d1.size() == d2.size());
      data::data_expression_list::iterator i1, i2;
      for (i1 = d1.begin(), i2 = d2.begin(); i1 != d1.end(); ++i1, ++i2)
      {
        v.push_back(d::lazy::equal_to(*i1, *i2));
      }
    }
    data::data_expression expr = d::lazy::join_and(v.begin(), v.end());
#ifdef MCRL2_EQUAL_MULTI_ACTIONS_DEBUG
    mCRL2log(debug) << "  <and-term> " << data::pp(expr) << std::endl;
#endif
    result.insert(expr);
  }
};

/// \brief Used for building an expression for the comparison of data parameters.
struct not_equal_multi_actions_builder
{
  const std::vector<action>& a;
  const std::vector<action>& b;
  std::vector<data::data_expression>& result;

  not_equal_multi_actions_builder(const std::vector<action>& a_,
                                  const std::vector<action>& b_,
                                  std::vector<data::data_expression>& result_
                                 )
    : a(a_),
      b(b_),
      result(result_)
  {}

  /// \brief Adds the expression 'a == b' to result.
  void operator()()
  {
    using namespace data::lazy;

    std::vector<data::data_expression> v;
    std::vector<action>::const_iterator i, j;
    for (i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j)
    {
      data::data_expression_list d1 = i->arguments();
      data::data_expression_list d2 = j->arguments();
      assert(d1.size() == d2.size());
      data::data_expression_list::iterator i1, i2;
      for (i1 = d1.begin(), i2 = d2.begin(); i1 != d1.end(); ++i1, ++i2)
      {
        v.push_back(data::not_equal_to(*i1, *i2));
      }
    }
    result.push_back(data::lazy::join_or(v.begin(), v.end()));
  }
};

} // namespace detail
/// \endcond

/// \brief Returns a data expression that expresses under which conditions the
/// multi actions a and b are equal. The multi actions may contain free variables.
/// \param a A sequence of actions
/// \param b A sequence of actions
/// \return Necessary conditions for the equality of a and b
inline data::data_expression equal_multi_actions(const multi_action& a, const multi_action& b)
{
#ifdef MCRL2_EQUAL_MULTI_ACTIONS_DEBUG
  mCRL2log(debug) << "\n<equal multi actions>" << std::endl;
  mCRL2log(debug) << "a = " << lps::pp(a.actions()) << std::endl;
  mCRL2log(debug) << "b = " << lps::pp(b.actions()) << std::endl;
#endif
  using namespace data::lazy;

  // make copies of a and b and sort them
  std::vector<action> va(a.actions().begin(), a.actions().end()); // protection not needed
  std::vector<action> vb(b.actions().begin(), b.actions().end()); // protection not needed
  std::sort(va.begin(), va.end(), detail::compare_action_label_arguments());
  std::sort(vb.begin(), vb.end(), detail::compare_action_label_arguments());

  if (!detail::equal_action_signatures(va, vb))
  {
#ifdef MCRL2_EQUAL_MULTI_ACTIONS_DEBUG
    mCRL2log(debug) << "different action signatures detected!" << std::endl;
    mCRL2log(debug) << "a = " << action_list(va.begin(), va.end()) << std::endl;
    mCRL2log(debug) << "b = " << action_list(vb.begin(), vb.end()) << std::endl;
#endif
    return data::sort_bool::false_();
  }

  // compute the intervals of a with equal names
  typedef std::vector<action>::iterator action_iterator;
  std::vector<std::pair<action_iterator, action_iterator> > intervals;
  action_iterator first = va.begin();
  while (first != va.end())
  {
    action_iterator next = std::upper_bound(first, va.end(), *first, detail::compare_action_labels());
    intervals.push_back(std::make_pair(first, next));
    first = next;
  }

  std::set<data::data_expression> z;
  detail::equal_data_parameters_builder f(va, vb, z);
  detail::forall_permutations(intervals.begin(), intervals.end(), f);
  data::data_expression result = data::lazy::join_or(z.begin(), z.end());
  return result;
}

/// \brief Returns a pbes expression that expresses under which conditions the
/// multi actions a and b are not equal. The multi actions may contain free variables.
/// \param a A sequence of actions
/// \param b A sequence of actions
/// \return Necessary conditions for the inequality of a and b
inline data::data_expression not_equal_multi_actions(const multi_action& a, const multi_action& b)
{
  using namespace data::lazy;

  // make copies of a and b and sort them
  std::vector<action> va(a.actions().begin(), a.actions().end());
  std::vector<action> vb(b.actions().begin(), b.actions().end());
  std::sort(va.begin(), va.end(), detail::compare_action_label_arguments());
  std::sort(vb.begin(), vb.end(), detail::compare_action_label_arguments());

  if (!detail::equal_action_signatures(va, vb))
  {
    return data::sort_bool::true_();
  }

  // compute the intervals of a with equal names
  typedef std::vector<action>::iterator action_iterator;
  std::vector<std::pair<action_iterator, action_iterator> > intervals;
  action_iterator first = va.begin();
  while (first != va.end())
  {
    action_iterator next = std::upper_bound(first, va.end(), *first, detail::compare_action_labels());
    intervals.push_back(std::make_pair(first, next));
    first = next;
  }
  std::vector<data::data_expression> z;
  detail::not_equal_multi_actions_builder f(va, vb, z);
  detail::forall_permutations(intervals.begin(), intervals.end(), f);
  data::data_expression result = data::lazy::join_and(z.begin(), z.end());
  return result;
}

} // namespace lps

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
template<>
struct aterm_traits<mcrl2::lps::multi_action>
{
  static void protect(const mcrl2::lps::multi_action& t)
  {
    t.protect();
  }
  static void unprotect(const mcrl2::lps::multi_action& t)
  {
    t.unprotect();
  }
  static void mark(const mcrl2::lps::multi_action& t)
  {
    t.mark();
  }
};
} // namespace atermpp
/// \endcond

#endif // MCRL2_LPS_MULTI_ACTION_H
