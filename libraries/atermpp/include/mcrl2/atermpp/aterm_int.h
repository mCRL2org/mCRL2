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

class aterm_int: public aterm
{
  protected:
    /// \brief Constructor
    aterm_int(detail::_aterm_int* t) noexcept 
     : aterm(reinterpret_cast<detail::_aterm*>(t))
    {
    } 

  public:

    /// \brief Default constructor.
    aterm_int() noexcept
    {}

    /// \brief Construct an aterm_int from an aterm.
    /// \details The aterm must be of type AT_INT.
    explicit aterm_int(const aterm& t) noexcept 
     : aterm(t) 
    {
      assert(t.type_is_int());
    }
    
    /// \brief Copy constructor
    /// \details The aterm must be of type AT_INT.
    aterm_int(const aterm_int&) noexcept = default;
    
    /// \brief Move construct an aterm_int from an aterm.
    /// \details The aterm must be of type AT_INT.
    aterm_int(aterm_int&&) noexcept = default;
    
    /// \brief Constructor.
    /// \param value An integer value.
    explicit aterm_int(std::size_t value)
     : aterm(detail::aterm_int(value))
    {}

    /// \brief Assignment operator.
    /// \param t A term representing an integer.
    aterm_int& operator=(const aterm_int&) = default;

    /// \brief Move assignment operator.
    /// \param t A term representing an integer.
    aterm_int& operator=(aterm_int&&) noexcept = default;

    /// \brief Get the integer value of the aterm_int.
    /// \return The value of the term.
    std::size_t value() const
    {
      return reinterpret_cast<detail::_aterm_int*>(m_term)->value;
    }

    void swap(aterm_int& t) noexcept
    {
      aterm::swap(t);
    }
};

} // namespace atermpp

namespace std
{

/// \brief Swaps two aterm_ints.
/// \details This operation is more efficient than exchanging terms by an assignment,
///          as swapping does not require to change the protection of terms.
///          In order to be used in the standard containers, the declaration must
///          be preceded by an empty template declaration.
/// \param t1 The first term
/// \param t2 The second term

template <>  
inline void swap(atermpp::aterm_int& t1, atermpp::aterm_int& t2) noexcept
{
  t1.swap(t2);
} 
} // namespace std 

#include "mcrl2/atermpp/detail/aterm_int.h"
#include "mcrl2/atermpp/detail/aterm_int_implementation.h"

#endif // MCRL2_ATERMPP_ATERM_INT_H
