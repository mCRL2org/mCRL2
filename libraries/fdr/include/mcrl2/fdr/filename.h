// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/filename.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_FILENAME_H
#define MCRL2_FDR_FILENAME_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A filename
class filename
{
  public:
    /// \brief Default constructor.
    filename()
      : atermpp::aterm_appl(fdr::atermpp::detail::constructFileName())
    {}

    /// \brief Constructor.
    /// \param term A term
    filename(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_FileName(m_term));
    }

    /// \brief Constructor.
    filename(const name_list& names)
      : atermpp::aterm_appl(fdr::detail::gsMakeFileName(names))
    {}

    name_list names() const
    {
      return atermpp::list_arg1(*this);
    }
};/// \brief list of filenames
    typedef atermpp::term_list<filename> filename_list;

    /// \brief vector of filenames
    typedef atermpp::vector<filename>    filename_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a filename expression
    /// \param t A term
    /// \return True if it is a filename expression
    inline
    bool is_filename(const filename& t)
    {
      return fdr::detail::gsIsFileName(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_FILENAME_H
