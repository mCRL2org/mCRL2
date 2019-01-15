// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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
#include "mcrl2/atermpp/detail/aterm_int.h"
#include "mcrl2/atermpp/detail/global_aterm_pool.h"

namespace atermpp
{

/// \brief An integer term stores a single std::size_t value. It carrier
/// 	       no arguments.
/// \details A special function symbol is used to identify integer terms
/// 			 internally.
class aterm_int : public aterm
{
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

  /// \brief Constructs an integer term from a value.
  explicit aterm_int(std::size_t value)
   : aterm(detail::g_term_pool().create_int(value))
  {}

  /// This class has user-declared copy constructor so declare default copy and move operators.
  aterm_int(const aterm_int& other) noexcept = default;
  aterm_int& operator=(const aterm_int& other) noexcept = default;
  aterm_int(aterm_int&& other) noexcept = default;
  aterm_int& operator=(aterm_int&& other) noexcept = default;

  /// \returns The value of the integer term.
  std::size_t value() const noexcept
  {
    return reinterpret_cast<detail::_aterm_int*>(m_term)->value();
  }

  /// \brief Swaps two integer terms without changing the protection.
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

#endif // MCRL2_ATERMPP_ATERM_INT_H
