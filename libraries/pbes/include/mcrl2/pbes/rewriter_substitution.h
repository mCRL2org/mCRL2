// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriter_substitution.h
/// \brief The substitution used by the pbes rewriter.

#ifndef MCRL2_PBES_REWRITER_SUBSTITUTION_H
#define MCRL2_PBES_REWRITER_SUBSTITUTION_H

#include "mcrl2/core/print.h"
#include "mcrl2/old_data/rewriter.h"
#include "mcrl2/old_data/enumerator.h"

namespace mcrl2 {

namespace pbes_system {

  struct pbes_rewriter_substitution: public old_data::rewriter::substitution, public old_data::enumerator_expression
  {
    pbes_rewriter_substitution()
    {}
   
    pbes_rewriter_substitution(const old_data::data_variable& variable, old_data::rewriter& datar)
      : old_data::rewriter::substitution(datar, variable, variable),
        old_data::enumerator_expression(ATermAppl(variable), make_list(variable))
    {}
    
    pbes_rewriter_substitution(const old_data::data_variable& variable,
                             const old_data::enumerator_expression& t,
                             old_data::rewriter& datar
                            )
      : old_data::rewriter::substitution(datar, variable, t.expression()),
        old_data::enumerator_expression(t)
    {}
    
    std::string to_string() const
    {
      return core::pp(expression()) + " " + core::pp(variables());
    }
  };

} // namespace pbes_system

} // namespace mcrl2


#endif // MCRL2_PBES_REWRITER_SUBSTITUTION_H
