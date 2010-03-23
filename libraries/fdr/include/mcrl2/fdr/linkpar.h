// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/linkpar.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_LINKPAR_H
#define MCRL2_FDR_LINKPAR_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Linkpar
  class linkpar: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      linkpar()
        : atermpp::aterm_appl(fdr::detail::constructLinkPar())
      {}

      /// \brief Constructor.
      /// \param term A term
      linkpar(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_LinkPar(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A link list
class links: public linkpar
{
  public:
    /// \brief Default constructor.
    links()
      : linkpar(fdr::detail::constructLinks())
    {}

    /// \brief Constructor.
    /// \param term A term
    links(atermpp::aterm_appl term)
      : linkpar(term)
    {
      assert(fdr::detail::check_term_Links(m_term));
    }

    /// \brief Constructor.
    links(const link_list& links)
      : linkpar(fdr::detail::gsMakeLinks(links))
    {}

    link_list links() const
    {
      return atermpp::list_arg1(*this);
    }
};

/// \brief A link/generator list
class linksgens: public linkpar
{
  public:
    /// \brief Default constructor.
    linksgens()
      : linkpar(fdr::detail::constructLinksGens())
    {}

    /// \brief Constructor.
    /// \param term A term
    linksgens(atermpp::aterm_appl term)
      : linkpar(term)
    {
      assert(fdr::detail::check_term_LinksGens(m_term));
    }

    /// \brief Constructor.
    linksgens(const link_list& links, const generator_list& gens)
      : linkpar(fdr::detail::gsMakeLinksGens(links, gens))
    {}

    link_list links() const
    {
      return atermpp::list_arg1(*this);
    }

    generator_list gens() const
    {
      return atermpp::list_arg2(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a links expression
    /// \param t A term
    /// \return True if it is a links expression
    inline
    bool is_links(const linkpar& t)
    {
      return fdr::detail::gsIsLinks(t);
    }

    /// \brief Test for a linksgens expression
    /// \param t A term
    /// \return True if it is a linksgens expression
    inline
    bool is_linksgens(const linkpar& t)
    {
      return fdr::detail::gsIsLinksGens(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_LINKPAR_H
