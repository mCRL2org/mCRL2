// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/timed_multi_action.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_TIMED_MULTI_ACTION_H
#define MCRL2_PROCESS_TIMED_MULTI_ACTION_H

#include "mcrl2/process/process_expression.h"

namespace mcrl2 {

namespace process {

//--- start generated class timed_multi_action ---//
/// \brief A timed multi-action
class timed_multi_action: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    timed_multi_action()
      : atermpp::aterm_appl(core::detail::default_values::TimedMultAct)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit timed_multi_action(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_TimedMultAct(*this));
    }

    /// \brief Constructor.
    timed_multi_action(const process::action_list& actions, const data::data_expression& time)
      : atermpp::aterm_appl(core::detail::function_symbol_TimedMultAct(), actions, time)
    {}

    /// Move semantics
    timed_multi_action(const timed_multi_action&) noexcept = default;
    timed_multi_action(timed_multi_action&&) noexcept = default;
    timed_multi_action& operator=(const timed_multi_action&) noexcept = default;
    timed_multi_action& operator=(timed_multi_action&&) noexcept = default;

    const process::action_list& actions() const
    {
      return atermpp::down_cast<process::action_list>((*this)[0]);
    }

    const data::data_expression& time() const
    {
      return atermpp::down_cast<data::data_expression>((*this)[1]);
    }
//--- start user section timed_multi_action ---//
    /// \brief Returns true if time is available.
    /// \return True if time is available.
    bool has_time() const
    {
      return time() != data::undefined_real();
    }

    /// \brief Returns the multiaction in which the list of actions is sorted. 
    /// \return A multi-action with a sorted list.
    timed_multi_action sort_actions() const
    {
      if (actions().size()<=1)  // Almost always there is only one action. 
      {
        return *this;
      }
      static std::multiset<action>sorted_actions(actions().begin(),actions().end());  // This set is static, as otherwise it must be constructed 
                                                                                  // too often. 
      return timed_multi_action(process::action_list(sorted_actions.begin(), sorted_actions.end()),time());
    }

//--- end user section timed_multi_action ---//
};

/// \brief list of timed_multi_actions
typedef atermpp::term_list<timed_multi_action> timed_multi_action_list;

/// \brief vector of timed_multi_actions
typedef std::vector<timed_multi_action>    timed_multi_action_vector;

/// \brief Test for a timed_multi_action expression
/// \param x A term
/// \return True if \a x is a timed_multi_action expression
inline
bool is_timed_multi_action(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::TimedMultAct;
}

// prototype declaration
std::string pp(const timed_multi_action& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const timed_multi_action& x)
{
  return out << process::pp(x);
}

/// \brief swap overload
inline void swap(timed_multi_action& t1, timed_multi_action& t2)
{
  t1.swap(t2);
}
//--- end generated class timed_multi_action ---//

} // namespace process

} // namespace mcrl2

namespace std {

/// \brief specialization of the standard std::hash function.
template<>
struct hash<mcrl2::process::timed_multi_action>
{
  std::size_t operator()(const mcrl2::process::timed_multi_action& x) const
  {
    std::size_t seed = std::hash<atermpp::aterm>()(x.actions());
    if (!x.actions().empty())
    {
      seed = std::hash<atermpp::aterm>()(x.actions().front().arguments()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    if (x.has_time())
    {
      seed = std::hash<atermpp::aterm>()(x.time()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

} // namespace std

#endif // MCRL2_PROCESS_TIMED_MULTI_ACTION_H
