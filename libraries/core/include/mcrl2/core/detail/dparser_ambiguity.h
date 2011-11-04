// Author(s): Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/dparser_ambiguity.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_DETAIL_DPARSER_AMBIGUITY_H
#define MCRL2_CORE_DETAIL_DPARSER_AMBIGUITY_H

#include "mcrl2/core/dparser.h"
#include "mcrl2/exception.h"

namespace mcrl2 {

namespace core {

namespace detail {

/// \brief Function for resolving ambiguities in the '_ -> _ <> _' operator for
/// process expressions.
struct D_ParseNode* ambiguity_fn(struct D_Parser*, unsigned int, struct D_ParseNode**);

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_DPARSER_AMBIGUITY_H
