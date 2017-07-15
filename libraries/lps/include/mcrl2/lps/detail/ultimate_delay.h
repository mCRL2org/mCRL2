// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ultimate_delay.h
/// \brief This file defines the class ultimate_delay.
///        An ultimate delay consists of a time variable t
///        and expression exp(t,x) and free variables x.
///        It can be read as a predicate on t of the form
///        exists x.exp(t,x). If this is true for certain t, this
///        indicates that a process can wait until time t.  


#ifndef MCRL2_LPS_DETAIL_ULTIMATE_DELAY_H
#define MCRL2_LPS_DETAIL_ULTIMATE_DELAY_H

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/real.h"

namespace mcrl2
{
namespace lps
{
namespace detail
{

using namespace data;

class ultimate_delay
{
  protected:
    variable m_time;
    variable_list m_vl;
    data_expression m_data;

  public:
    /// \brief Constructor.
    ultimate_delay()
     : m_data(sort_bool::false_())
     {}


    /// \brief Constructor.
    ultimate_delay(const variable& t)
     : m_time(t), m_vl(), m_data(sort_bool::true_())
    {
      assert(t.sort()==sort_real::real_());
    }

    /// \brief Constructor.
    ultimate_delay(const variable& t, const variable_list& vl, const data_expression& d)
     : m_time(t), m_vl(vl), m_data(d)
    {
      assert(d.sort()==sort_bool::bool_());
      assert(t.sort()==sort_real::real_());
    }

    /// \brief Obtain the variables. 
    const variable_list& variables() const
    {
      return m_vl;
    }

    /// \brief Reference to the variables. 
    variable_list& variables()
    {
      return m_vl;
    }

    /// \brief Obtain the constraint in the ultimate delay.
    const data_expression& constraint() const
    {
      return m_data;
    }

    /// \brief Obtain a reference to the constraint. 
    data_expression& constraint()
    {
      return m_data;
    }

    /// \brief Obtain the constraint in the ultimate delay.
    const variable& time_var() const
    {
      return m_time;
    }

    /// \brief Obtain a reference to the time variable in the ultimate delay.
    variable& time_var()
    {
      return m_time;
    }
};



/// \brief Returns the conjunction of the two delay conditions and the join of the variables, where
///        the variables in delay2 are renamed to avoid conflict with those in delay1. 
ultimate_delay combine_ultimate_delays(const ultimate_delay& delay1, const ultimate_delay& delay2);

}  // namespace detail
}  // namespace lps
}  // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_ULTIMATE_DELAY_H

