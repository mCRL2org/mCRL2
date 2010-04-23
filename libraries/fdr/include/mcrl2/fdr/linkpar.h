// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/linkpar.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_LINKPAR_H
#define MCRL2_FDR_LINKPAR_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/link.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A link list
class links: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    links()
      : atermpp::aterm_appl(fdr::detail::constructLinks())
    {}

    /// \brief Constructor.
    /// \param term A term
    links(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Links(m_term));
    }

    /// \brief Constructor.
    links(const link_list& linkpars)
      : atermpp::aterm_appl(fdr::detail::gsMakeLinks(linkpars))
    {}

    link_list linkpars() const
    {
      return atermpp::list_arg1(*this);
    }
};

/// \brief list of linkss
typedef atermpp::term_list<links> links_list;

/// \brief vector of linkss
typedef atermpp::vector<links>    links_vector;


/// \brief A link/generator list
class linksgens: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    linksgens()
      : atermpp::aterm_appl(fdr::detail::constructLinksGens())
    {}

    /// \brief Constructor.
    /// \param term A term
    linksgens(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_LinksGens(m_term));
    }

    /// \brief Constructor.
    linksgens(const link_list& linkpars, const comprehension_list& comprs)
      : atermpp::aterm_appl(fdr::detail::gsMakeLinksGens(linkpars, comprs))
    {}

    link_list linkpars() const
    {
      return atermpp::list_arg1(*this);
    }

    comprehension_list comprs() const
    {
      return atermpp::list_arg2(*this);
    }
};

/// \brief list of linksgenss
typedef atermpp::term_list<linksgens> linksgens_list;

/// \brief vector of linksgenss
typedef atermpp::vector<linksgens>    linksgens_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_LINKPAR_H
