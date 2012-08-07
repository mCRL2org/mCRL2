// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_int.h
/// \brief Term containing an integer.

#ifndef MCRL2_ATERMPP_ATERM_INT_H
#define MCRL2_ATERMPP_ATERM_INT_H

#include "mcrl2/atermpp/detail/aterm_int.h"
#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

class aterm_int:public aterm
{
  public:  // These should be protected, but that is currently not yet possible.
    detail::_aterm_int & operator *() const
    {
      // Note that this operator can be applied on a NULL pointer, i.e., in the case &*m_term is checked,
      // which is done quite commonly.
      assert(m_term->reference_count()>0);
      return *reinterpret_cast<detail::_aterm_int*>(m_term); 
    }

    detail::_aterm_int *operator ->() const
    {
      assert(m_term!=NULL);
      assert(m_term->reference_count()>0);
      return reinterpret_cast<detail::_aterm_int*>(m_term);
    }

    /// \brief Get the integer value of the aterm_int.
  public:

    /// \brief Constructor.
    aterm_int()
    {}

    
    aterm_int(detail::_aterm_int *t):aterm(reinterpret_cast<detail::_aterm*>(t))
    {
    }

    explicit aterm_int(const aterm &t):aterm(t) 
    {
    }
    
    /// \brief Constructor.
    /// \param value An integer value.
    aterm_int(int value):aterm(detail::aterm_int(value))
    {
//       increase_reference_count<false>(m_term);
    }

    /// \brief Assignment operator.
    /// \param t A term representing an integer.
    aterm_int &operator=(const aterm_int &t)
    {
      copy_term(t.m_term);
      return *this;
    }

    /// \return The value of the term.
    int value() const
    {
      return reinterpret_cast<detail::_aterm_int*>(m_term)->value;
    }
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_INT_H
