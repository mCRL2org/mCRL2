// Author(s): Jan Friso Groote, Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//
/// \file mcrl2/data/standard_containers/vector.h
/// \brief This file contains a vector class that behaves 
///        exactly as a standard vector. It can only be used
///        to store class instances that derive from aterms.
///        The stored aterms are protected as a whole, i.e.,
///        time and memory is saved as individual protection
///        per element is unnecessary. 

#ifndef MCRL2_ATERMPP_STANDARD_CONTAINER_VECTOR_H
#define MCRL2_ATERMPP_STANDARD_CONTAINER_VECTOR_H

#include <vector.h>
#include "mcrl2/atermpp/type_traits.h"

/// \brief The main namespace for the aterm++ library.
namespace atermpp
{

/// \brief A vector class in which aterms can be stored. 
class aterm : public unprotected_aterm
{
public:

  /// \brief Default constructor.
  aterm() noexcept;

  /// \brief Standard destructor.
  ~aterm() noexcept;

  /// \brief Constructor based on an internal term data structure. This is not for public use.
  /// \details Takes ownership of the passed underlying term.
  /// \param t A pointer to an internal aterm data structure.
  /// \todo Should be protected, but this cannot yet be done due to a problem
  ///       in the compiling rewriter.
  explicit aterm(const detail::_aterm *t) noexcept;

  /// \brief Copy constructor.
  /// \param other The aterm that is copied.
  /// \details  This class has a non-trivial destructor so explicitly define the copy and move operators.
  aterm(const aterm& other) noexcept;

  /// \brief Move constructor.
  /// \param other The aterm that is moved into the new term. This term may have changed after this operation.
  /// \details This operation does not employ increments and decrements of reference counts and is therefore more
  ///          efficient than the standard copy construct.
  aterm(aterm&& other) noexcept;

  /// \brief Assignment operator.
  /// \param other The aterm that will be assigned.
  /// \return A reference to the assigned term.
  aterm& operator=(const aterm& other) noexcept
  {
#ifdef MCRL2_ATERMPP_REFERENCE_COUNTED
    // Increment first to prevent the same term from becoming reference zero temporarily.
    other.increment_reference_count();

    // Decrement the reference from the term that is currently referred to.
    decrement_reference_count();
#endif

    m_term = other.m_term;
    return *this;
  }

  /// \brief Move assignment operator.
  /// \brief This move assignment operator
  /// \param other The aterm that will be assigned.
  /// \return A reference to the assigned term.
  aterm& operator=(aterm&& other) noexcept
  {
#ifdef MCRL2_ATERMPP_REFERENCE_COUNTED
    std::swap(m_term, other.m_term);
#else
    m_term = other.m_term;    // Using hash set protection it is cheaper just to move the value to the new term.
#endif
    return *this;
  }

protected:
#ifdef MCRL2_ATERMPP_REFERENCE_COUNTED
  /// \brief Increment the reference count.
  /// \details This increments the reference count unless the term contains null.
  ///          Use with care as this destroys the reference count mechanism.
  void increment_reference_count() const
  {
    if (defined())
    {
      m_term->increment_reference_count();
    }
  }

  /// \brief Decrement the reference count.
  /// \details This decrements the reference count unless the term contains null.
  ///          Use with care as this destroys the reference count mechanism.
  void decrement_reference_count() const
  {
    if (defined())
    {
      m_term->decrement_reference_count();
    }
  }
#endif
};

} // namespace atermpp
#endif // MCRL2_ATERMPP_STANDARD_CONTAINER_VECTOR_H
