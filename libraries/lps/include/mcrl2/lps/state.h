// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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

namespace mcrl2::lps
{

using state = atermpp::term_balanced_tree<data::data_expression>;

template<class ForwardTraversalIterator, class Transformer>
void make_state(state& result,
                ForwardTraversalIterator p,
                const std::size_t size,
                Transformer transformer)
{
  // make_term_balanced_tree<data::data_expression>(result, p, size, transformer);
  make_term_balanced_tree(result, p, size, transformer);
}
 
template<class ForwardTraversalIterator>
void make_state(state& result,
                ForwardTraversalIterator p,
                const std::size_t size)
{
  // make_term_balanced_tree<data::data_expression>(result, p, size, transformer);
  make_term_balanced_tree(result, 
                          p, 
                          size, 
                          [](data::data_expression& result, const data::data_expression& t){ result=t; });
}
 
// template function overloads
inline std::string pp(const lps::state& x)
{
  return atermpp::pp(x);
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_STATE_H

