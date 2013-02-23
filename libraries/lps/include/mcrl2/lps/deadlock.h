// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/deadlock.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DEADLOCK_H
#define MCRL2_LPS_DEADLOCK_H

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/real.h"

namespace mcrl2
{

namespace lps
{

/// \brief Represents a deadlock
/// \detail A deadlock is 'delta' with an optional time tag.
class deadlock
{
    friend class deadlock_summand;

  protected:
    /// \brief The time of the deadlock. If <tt>m_time == data::data_expression()</tt>
    /// the multi action has no time.
    data::data_expression m_time;

  public:
    /// \brief Constructor
    deadlock(data::data_expression time = data::data_expression())
      : m_time(time)
    {}

    /// \brief Returns true if time is available.
    /// \return True if time is available.
    bool has_time() const
    {
      return m_time != data::data_expression();
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

    /// \brief Returns a string representation of the deadlock
    std::string to_string() const
    {
      return std::string("delta") + (has_time() ? (" @ " + data::pp(m_time)) : "");
    }

    /// \brief Comparison operator
    bool operator==(const deadlock& other) const
    {
      return m_time == other.m_time;
    }

    /// \brief Comparison operator
    bool operator!=(const deadlock& other) const
    {
      return !(*this == other);
    }

    /// \brief Protects the term from being freed during garbage collection.
    void protect() const
    {
      m_time.protect();
    }

    /// \brief Unprotect the term.
    /// Releases protection of the term which has previously been protected through a
    /// call to protect.
    void unprotect() const
    {
      m_time.unprotect();
    }
};

// template function overloads
std::set<data::variable> find_variables(const lps::deadlock& x);
std::set<data::variable> find_free_variables(const lps::deadlock& x);

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DEADLOCK_H
