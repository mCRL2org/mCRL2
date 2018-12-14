// Author(s): Jan Friso Groote. Based on the aterm library by Paul Klint and others.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/aterm_implementation.h
/// \brief This file contains the code to allocate aterms in tables
///        with sufficient space for its arguments. 

#pragma once
#ifndef MCRL2_ATERMPP_ATERM_IMPLEMENTATION_H
#define MCRL2_ATERMPP_ATERM_IMPLEMENTATION_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

namespace detail
{

/* Check for reasonably sized aterm (32 bits, 4 bytes)     */
/* This check might break on perfectly valid architectures */
/* that have char == 2 bytes, and sizeof(header_type) == 2 */
static_assert(sizeof(std::size_t) == sizeof(_aterm*), "The size of an aterm pointer is not equal to the size of type std::size_t. Cannot compile the MCRL2 toolset for this platform.");
static_assert(sizeof(std::size_t) >= 4,"The size of std::size_t should at least be four bytes. Cannot compile the toolset for this platform.");

inline _aterm* address(const unprotected_aterm& t)
{
  return t.m_term;
}

} //namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_IMPLEMENTATION_H
