// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/iota.h
/// \brief The iota function.

#ifndef MCRL2_CORE_DETAIL_IOTA_H
#define MCRL2_CORE_DETAIL_IOTA_H

namespace mcrl2 {

namespace core {

namespace detail {

  /// \brief Generates the sequence value, value + 1, ... and writes it to the sequence [first, last)
  /// \param first Start of a sequence
  /// \param last End of a sequence
  /// \param value A value
  template <class Iter, class T>
  void iota(Iter first, Iter last, T value)
  {
    while (first != last)
    {
      *first++ = value++;
    }
  }

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_IOTA_H
