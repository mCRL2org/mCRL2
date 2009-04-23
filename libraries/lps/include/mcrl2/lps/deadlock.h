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
#include "mcrl2/data/utility.h" // for pp

namespace mcrl2 {

namespace lps {

  class deadlock
  {
    protected:
      /// \brief The time of the deadlock. If <tt>m_time == data::data_expression()</tt>
      /// the multi action has no time.
      data::data_expression m_time;

    public:
      /// \brief Constructor
      deadlock(data::data_expression time = atermpp::aterm_appl(core::detail::gsMakeNil()))
        : m_time(time)
      {}

      /// \brief Returns true if time is available.
      /// \return True if time is available.
      bool has_time() const
      {
        return m_time != core::detail::gsMakeNil();
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

      /// \brief Applies a low level substitution function to this term and returns the result.
      /// \param f A
      /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
      /// This function is applied to all <tt>aterm</tt> noded appearing in this term.
      /// \deprecated
      /// \return The substitution result.
      template <typename Substitution>
      deadlock substitute(Substitution f)
      {
        return deadlock(substitute(f, m_time));
      }

      /// \brief Returns a string representation of the deadlock
      std::string to_string() const
      {
        return std::string("delta") + (has_time() ? (" @ " + data::pp(m_time)) : "");
      }

      /// \brief Comparison operator
      bool operator==(const deadlock& other)
      {
        return m_time == other.m_time;
      }

      /// \brief Comparison operator
      bool operator!=(const deadlock& other)
      {
        return !(*this == other);
      }
  };

/// \brief Traverses the deadlock, and writes all sort expressions
/// that are encountered to the output range [dest, ...).
template <typename OutIter>
void traverse_sort_expressions(const deadlock& d, OutIter dest)
{
  if (d.has_time())
  {
    *dest++ = d.time().sort();
  }
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DEADLOCK_H
