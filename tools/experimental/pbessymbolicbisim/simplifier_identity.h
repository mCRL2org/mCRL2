// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simplifier_identity.h


#ifndef MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_IDENTITY_H
#define MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_IDENTITY_H

#include "mcrl2/data/enumerator.h"

#include "simplifier.h"

namespace mcrl2
{
namespace data
{

class simplifier_identity: public simplifier
{
  typedef simplifier super;

protected:

  data_expression simplify_expression(const data_expression& expr)
  {
    return expr;
  }

public:
  simplifier_identity(const rewriter& r, const rewriter& pr)
  : super(r, pr)
  {}

};


} // namespace mcrl2
} // namespace data

#endif // MCRL2_LPSSYMBOLICBISIM_SIMPLIFIER_IDENTITY_H