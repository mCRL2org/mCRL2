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

#include "mcrl2/core/detail/struct_core.h" // gsMakeNil
#include "mcrl2/lps/action.h"
#include "mcrl2/data/standard_utility.h"

namespace mcrl2 {

namespace lps {

/// \brief Contains multi actions 
/// \detail Multi actions consist of a list of actions together with an optional time tag.
  class multi_action
  {
    protected:
      /// \brief The actions of the summand
      action_list m_actions;

      /// \brief The time of the summand. If <tt>m_time == data::data_expression()</tt>
      /// the multi action has no time.
      data::data_expression m_time;

    public:
      /// \brief Constructor
      multi_action(action_list actions = action_list(), data::data_expression time = atermpp::aterm_appl(core::detail::gsMakeNil()))
        : m_actions(actions), m_time(time)
      {}

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

      /// \brief Applies a low level substitution function to this term and returns the result.
      /// \param f A
      /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
      /// This function is applied to all <tt>aterm</tt> noded appearing in this term.
      /// \deprecated
      /// \return The substitution result.
      template <typename Substitution>
      multi_action substitute(Substitution f)
      {
        return multi_action(m_actions.substitute(f), data::substitute(f, m_time));
      }

      /// \brief Returns a string representation of the multi action
      std::string to_string() const
      {
        return core::pp(m_actions) + (has_time() ? (" @ " + core::pp(m_time)) : "");
      }

      /// \brief Joins the actions of both multi actions.
      /// \pre The time of both multi actions must be equal.
      multi_action operator+(const multi_action& other) const
      {
        assert(m_time == other.m_time);
        return multi_action(m_actions + other.m_actions, m_time);
      }

      /// \brief Comparison operator
      bool operator==(const multi_action& other)
      {
        return m_actions == other.m_actions && m_time == other.m_time;
      }

      /// \brief Comparison operator
      bool operator!=(const multi_action& other)
      {
        return !(*this == other);
      }
  };

/// \cond INTERNAL_DOCS
namespace detail {
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
          return false;
      }
      return true;
    }

    /// \brief Compares names and sorts of two actions
    struct compare_actions
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

    /// \brief Compares names and sorts of two actions
    struct compare_actions2
    {
      /// \brief Function call operator
      /// \param a An action
      /// \param b An action
      /// \return The function result
      bool operator()(const action& a, const action& b) const
      {
        return a.label() < b.label();
        if (a.label().name() != b.label().name())
        {
          return a.label().name() ==  b.label().name();
        }
        return a.label().sorts() < b.label().sorts();
      }
    };

    /// \brief Used for building an expression for the comparison of data parameters.
    struct equal_data_parameters_builder
    {
      const std::vector<action>& a;
      const std::vector<action>& b;
      atermpp::set<data::data_expression>& result;

      equal_data_parameters_builder(const std::vector<action>& a_,
                                    const std::vector<action>& b_,
                                    atermpp::set<data::data_expression>& result_
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

        atermpp::vector<data::data_expression> v;
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
        data::data_expression expr = join_and(v.begin(), v.end());
#ifdef MCRL2_EQUAL_MULTI_ACTIONS_DEBUG
std::cerr << "  <and-term> " << pp(expr) << std::endl;
#endif
        result.insert(expr);
      }
    };

    /// \brief Used for building an expression for the comparison of data parameters.
    struct not_equal_multi_actions_builder
    {
      const std::vector<action>& a;
      const std::vector<action>& b;
      atermpp::vector<data::data_expression>& result;

      not_equal_multi_actions_builder(const std::vector<action>& a_,
                                      const std::vector<action>& b_,
                                      atermpp::vector<data::data_expression>& result_
                                     )
        : a(a_),
          b(b_),
          result(result_)
      {}

      /// \brief Adds the expression 'a == b' to result.
      void operator()()
      {
        using namespace data::lazy;

        atermpp::vector<data::data_expression> v;
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
        result.push_back(join_or(v.begin(), v.end()));
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
std::cerr << "\n<equal multi actions>" << std::endl;
std::cerr << "a = " << pp(a.actions()) << std::endl;
std::cerr << "b = " << pp(b.actions()) << std::endl;
#endif
      using namespace data::lazy;

      // make copies of a and b and sort them
      std::vector<action> va(a.actions().begin(), a.actions().end()); // protection not needed
      std::vector<action> vb(b.actions().begin(), b.actions().end()); // protection not needed
      std::sort(va.begin(), va.end(), detail::compare_actions());
      std::sort(vb.begin(), vb.end(), detail::compare_actions());

      if (!detail::equal_action_signatures(va, vb))
      {
#ifdef MCRL2_EQUAL_MULTI_ACTIONS_DEBUG
std::cerr << "different action signatures detected!" << std::endl;
std::cerr << "a = " << action_list(va.begin(), va.end()) << std::endl;
std::cerr << "b = " << action_list(vb.begin(), vb.end()) << std::endl;
#endif
        return data::sort_bool_::false_();
      }

      // compute the intervals of a with equal names
      typedef std::vector<action>::iterator action_iterator;
      std::vector<std::pair<action_iterator, action_iterator> > intervals;
      action_iterator first = va.begin();
      while (first != va.end())
      {
        action_iterator next = std::upper_bound(first, va.end(), *first, detail::compare_actions());
        intervals.push_back(std::make_pair(first, next));
        first = next;
      }

      atermpp::set<data::data_expression> z;
      detail::equal_data_parameters_builder f(va, vb, z);
      detail::forall_permutations(intervals.begin(), intervals.end(), f);
      data::data_expression result = join_or(z.begin(), z.end());
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
      std::sort(va.begin(), va.end(), detail::compare_actions());
      std::sort(vb.begin(), vb.end(), detail::compare_actions());

      if (!detail::equal_action_signatures(va, vb))
      {
        return data::sort_bool_::true_();
      }

      // compute the intervals of a with equal names
      typedef std::vector<action>::iterator action_iterator;
      std::vector<std::pair<action_iterator, action_iterator> > intervals;
      action_iterator first = va.begin();
      while (first != va.end())
      {
        action_iterator next = std::upper_bound(first, va.end(), *first, detail::compare_actions());
        intervals.push_back(std::make_pair(first, next));
        first = next;
      }
      atermpp::vector<data::data_expression> z;
      detail::not_equal_multi_actions_builder f(va, vb, z);
      detail::forall_permutations(intervals.begin(), intervals.end(), f);
      data::data_expression result = join_and(z.begin(), z.end());
      return result;
    }

/// \brief Traverses the multi action, and writes all sort expressions
/// that are encountered to the output range [dest, ...).
template <typename OutIter>
void traverse_sort_expressions(const multi_action& m, OutIter dest)
{
  if (m.has_time())
  {
    data::traverse_sort_expressions(m.time(), dest);
  }
  data::traverse_sort_expressions(m.actions(), dest);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_MULTI_ACTION_H
