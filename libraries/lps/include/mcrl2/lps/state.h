// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/state.h
/// \brief The class summand.

#ifndef MCRL2_LPS_STATE_H
#define MCRL2_LPS_STATE_H

#include "mcrl2/atermpp/aterm_balanced_tree.h"
#include "mcrl2/data/data_expression.h"

namespace mcrl2
{

namespace lps
{

typedef atermpp::term_balanced_tree<data::data_expression> state;

// template function overloads
inline std::string pp(const lps::state& x)
{
  return atermpp::pp(x);
}

} // namespace lps
} // namespace mcrl2

#endif // MCRL2_LPS_STATE_H

