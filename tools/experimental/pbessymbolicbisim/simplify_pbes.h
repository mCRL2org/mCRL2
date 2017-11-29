// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simplify_pbes.h

#include "mcrl2/data/fourier_motzkin.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2
{
namespace pbes_system
{

struct fourier_motzkin_rewriter
{
  pbes_expression operator()(const pbes_expression& e)
  {
    if(data::is_data_expression(e))
    {
      return data::replace_data_expressions(data_expression(e), fourier_motzkin_sigma, true);
    }
    return e;
  }
};



} // namespace pbes_system
} // namespace mcrl2
