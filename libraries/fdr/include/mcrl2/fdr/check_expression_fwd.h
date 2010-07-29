// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// ile mcrl2/fdr/check_expression.h
/// rief Forward declaration of class check_expression.

#ifndef MCRL2_FDR_CHECK_EXPRESSION_FWD_H
#define MCRL2_FDR_CHECK_EXPRESSION_FWD_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/vector.h"

namespace mcrl2 {

namespace fdr {

class check_expression;
typedef atermpp::term_list<check_expression> check_expression_list;
typedef atermpp::vector<check_expression> check_expression_vector;

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_CHECK_EXPRESSION_FWD_H
