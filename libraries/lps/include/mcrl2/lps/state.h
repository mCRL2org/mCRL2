// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/summand.h
/// \brief The class summand.

#ifndef MCRL2_LPS_STATE_H
#define MCRL2_LPS_STATE_H

#include "mcrl2/data/data_expression.h"

namespace mcrl2
{

namespace lps
{


/// \brief A state type of fixed length in which data expressions can be stored.
//         The length is determined at runtime. 
class state: public std::vector < mcrl2::data::data_expression > 
{
  public:
    state()
    {
    }

    state(data::data_expression_list arguments):
      std::vector<mcrl2::data::data_expression>(arguments.begin(), arguments.end())
    {
    }
};

// template function overloads
std::string pp(const lps::state& x);

} // namespace lps
} // namespace mcrl2

#endif // MCRL2_LPS_STATE_H

