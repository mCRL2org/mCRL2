// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/map.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_MAP_H
#define MCRL2_FDR_MAP_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Map
  class map: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      map()
        : atermpp::aterm_appl(fdr::detail::constructMap())
      {}

      /// \brief Constructor.
      /// \param term A term
      map(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Map(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A map
class map: public map
{
  public:
    /// \brief Default constructor.
    map()
      : map(fdr::detail::constructMap())
    {}

    /// \brief Constructor.
    /// \param term A term
    map(atermpp::aterm_appl term)
      : map(term)
    {
      assert(fdr::detail::check_term_Map(m_term));
    }

    /// \brief Constructor.
    map(const dotted_expression& left, const dotted_expression& right)
      : map(fdr::detail::gsMakeMap(left, right))
    {}

    dotted_expression left() const
    {
      return atermpp::arg1(*this);
    }

    dotted_expression right() const
    {
      return atermpp::arg2(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a map expression
    /// \param t A term
    /// \return True if it is a map expression
    inline
    bool is_map(const map& t)
    {
      return fdr::detail::gsIsMap(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_MAP_H
