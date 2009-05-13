// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sumelm.h
/// \brief Implements the sum elimination theorem.

#ifndef MCRL2_LPS_SUMELM_H
#define MCRL2_LPS_SUMELM_H

#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

  /// \brief Apply the sum elimination lemma to each of the summands in a
  ///        specification.
  /// \param s a specification
  /// \return specification s from which superfluous sum variables have been
  ///         eliminated.
  specification sumelm(const specification& s);

  /// \brief Apply the sum elimination lemma to a summand.
  /// \param s a summand
  /// \return summand s from which superfluous sum variables have been
  ///         eliminated.
  summand sumelm(const summand& s);

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_SUMELM_H

