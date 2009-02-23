// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/deadlock.h
/// \brief Deadlock class.

#ifndef MCRL2_LPS_DEADLOCK_H
#define MCRL2_LPS_DEADLOCK_H

namespace mcrl2 {

namespace lps {

  class deadlock
  {
    protected:
      /// \brief The time of the deadlock. If <tt>m_time == data::data_expression()</tt>
      /// the deadlock has no time.
      data::data_expression m_time;

    public:
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
      data::data_expression time() const
      {
        return m_time;
      }
  };

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DEADLOCK_H
