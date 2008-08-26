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
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/data_expression_with_variables.h"

namespace mcrl2 {

namespace pbes_system {

  struct pbes_rewriter_substitution: public data::rewriter::substitution, public data::data_expression_with_variables
  {
    pbes_rewriter_substitution()
    {}
   
    pbes_rewriter_substitution(const data::data_variable& variable, data::rewriter& datar)
      : data::rewriter::substitution(datar, variable, variable),
        data::data_expression_with_variables(ATermAppl(variable), make_list(variable))
    {}
    
    pbes_rewriter_substitution(const data::data_variable& variable,
                             const data::data_expression_with_variables& t,
                             data::rewriter& datar
                            )
      : data::rewriter::substitution(datar, variable, t),
        data::data_expression_with_variables(t)
    {}
    
    std::string to_string() const
    {
      const data::data_expression_with_variables& d = *this;
      return core::pp(d) + " " + core::pp(variables());
    }
  };

} // namespace pbes_system

} // namespace mcrl2


#endif // MCRL2_PBES_REWRITER_SUBSTITUTION_H
