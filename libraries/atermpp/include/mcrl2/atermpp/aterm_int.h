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

#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

class _ATermInt:public _ATerm
{
  public:
    union
    {
      int value;
      MachineWord reserved; /* Only use lower 32 bits as int. The value is used ambiguously
                               as integer and as MachineWord. For all cases using bitwise
                               operations, the MachineWord version must be used,
                               as failing to do so may lead to improper initialisation
                               of the last 32 bits during casting. */
    };
};

static const size_t TERM_SIZE_INT = sizeof(_ATermInt)/sizeof(size_t);

class aterm_int:public aterm
{
  public:

    /// \brief Constructor.
    aterm_int()
    {}

    
    aterm_int(_ATermInt *t):aterm(reinterpret_cast<_ATerm*>(t))
    {
    }

    explicit aterm_int(const aterm &t):aterm(t) 
    {
    }
    
    /// \brief Constructor.
    /// \param value An integer value.
    aterm_int(int value);

    /// \brief Assignment operator.
    /// \param t A term representing an integer.
    aterm_int &operator=(const aterm_int &t)
    {
      copy_term(t.m_term);
      return *this;
    }

    _ATermInt & operator *() const
    {
      // Note that this operator can be applied on a NULL pointer, i.e., in the case &*m_term is checked,
      // which is done quite commonly.
      assert(m_term==NULL || m_term->reference_count>0);
      return *reinterpret_cast<_ATermInt*>(m_term); 
    }

    _ATermInt *operator ->() const
    {
      assert(m_term!=NULL);
      assert(m_term->reference_count>0);
      return reinterpret_cast<_ATermInt*>(m_term);
    }

    /// \brief Get the integer value of the aterm_int.
    /// \return The value of the term.
    int value() const
    {
      return reinterpret_cast<_ATermInt*>(&*m_term)->value;
    }
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_INT_H
