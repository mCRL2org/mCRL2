// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/transform.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TRANSFORM_H
#define MCRL2_PBES_TRANSFORM_H

#include "mcrl2/pbes/replace.h"

namespace mcrl2 {

namespace pbes_system {

inline
void pbes_transform(pbes& x, unsigned int iterations, bool mu_value = false, bool nu_value = false)
{
  propositional_variable_substitution sigma;
  propositional_variable_substitution sigma_final;
  typedef typename core::term_traits<pbes_expression> tr;

  for (auto i = x.equations().begin(); i != x.equations().end(); ++i)
  {
    sigma[i->variable()] = i->formula();
    if (i->symbol().is_mu())
    {
      sigma_final[i->variable()] = mu_value ? tr::true_() : tr::false_();
    }
    else
    {
      sigma_final[i->variable()] = nu_value ? tr::true_() : tr::false_();
    }
  }

  for (unsigned int i = 0; i < iterations; i++)
  {
    pbes_system::replace_propositional_variables(x, sigma);
  }

  pbes_system::replace_propositional_variables(x, sigma_final);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TRANSFORM_H
