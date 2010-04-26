// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/fdr/filename.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_FILENAME_H
#define MCRL2_FDR_FILENAME_H

#include "mcrl2/fdr/term_include_files.h"
#include "mcrl2/fdr/name.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A filename
class filename: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    filename()
      : atermpp::aterm_appl(fdr::detail::constructFileName())
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
};

/// \brief list of filenames
typedef atermpp::term_list<filename> filename_list;

/// \brief vector of filenames
typedef atermpp::vector<filename>    filename_vector;

//--- end generated classes ---//

//--- start generated is-functions ---//
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_FILENAME_H
