// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simplifier_identity.h


#ifndef MCRL2_PBESSYMBOLICBISIM_SIMPLIFIER_IDENTITY_H
#define MCRL2_PBESSYMBOLICBISIM_SIMPLIFIER_IDENTITY_H

#include "simplifier.h"

namespace mcrl2::data
{

class simplifier_identity: public simplifier
{
  using super = simplifier;

protected:

  data_expression simplify_expression(const data_expression& expr) override { return expr; }

public:
  simplifier_identity(const rewriter& r, const rewriter& pr)
  : super(r, pr)
  {}
  ~simplifier_identity() override = default;
};


} // namespace mcrl2
// namespace data

#endif // MCRL2_PBESSYMBOLICBISIM_SIMPLIFIER_IDENTITY_H
