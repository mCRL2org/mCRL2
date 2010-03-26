// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/template.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TEMPLATE_H
#define MCRL2_FDR_TEMPLATE_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Template
  class template: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      template()
        : atermpp::aterm_appl(fdr::detail::constructTemplate())
      {}

      /// \brief Constructor.
      /// \param term A term
      template(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Template(m_term));
      }
  };

//--- start generated classes ---//
//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TEMPLATE_H
