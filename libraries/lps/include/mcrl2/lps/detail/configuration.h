// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// \file mcrl2/lps/detail/configuration.h
// \brief Configuration options for the LPS library.

#ifndef MCRL2_LPS_DETAIL_CONFIGURATION_H_
#define MCRL2_LPS_DETAIL_CONFIGURATION_H_

namespace mcrl2::lps::detail
{

/// \brief If true, statistics are logged for the linearisation of allow and block operators.
constexpr bool EnableLineariseStatistics = false;

} // namespace mcrl2::lps::detail

#endif // MCRL2_LPS_DETAIL_CONFIGURATION_H_