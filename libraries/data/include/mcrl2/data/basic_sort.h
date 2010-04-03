// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/basic_sort.h
/// \brief The class basic_sort.

#ifndef MCRL2_DATA_BASIC_SORT_H
#define MCRL2_DATA_BASIC_SORT_H

#include <string>
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/sort_expression.h"

namespace mcrl2 {

  namespace data {

    namespace detail {

//--- start generated class ---//
/// \brief A basic sort
class basic_sort_base: public sort_expression
{
  public:
    /// \brief Default constructor.
    basic_sort_base()
      : sort_expression(core::detail::constructSortId())
    {}

    /// \brief Constructor.
    /// \param term A term
    basic_sort_base(atermpp::aterm_appl term)
      : sort_expression(term)
    {
      assert(core::detail::check_term_SortId(m_term));
    }

    /// \brief Constructor.
    basic_sort_base(const core::identifier_string& name)
      : sort_expression(core::detail::gsMakeSortId(name))
    {}

    /// \brief Constructor.
    basic_sort_base(const std::string& name)
      : sort_expression(core::detail::gsMakeSortId(core::identifier_string(name)))
    {}

    core::identifier_string name() const
    {
      return atermpp::arg1(*this);
    }
};
//--- end generated class ---//

    } //namespace detail

    /// \brief basic sort.
    ///
    /// A basic sort is sort with only a name.
    /// An example is the sort S, specified in mCRL2 with sort S;
    class basic_sort: public detail::basic_sort_base
    {
      public:

        /// \brief Default constructor. Note that this does not entail a
        ///        valid sort expression.
        ///
        basic_sort()
          : detail::basic_sort_base()
        {}

        /// \overload
        basic_sort(atermpp::aterm_appl t)
          : detail::basic_sort_base(t)
        {}

        /// \overload
        basic_sort(const std::string& s)
          : detail::basic_sort_base(s)
        {}

        /// \overload
        basic_sort(const core::identifier_string& s)
          : detail::basic_sort_base(s)
        {}

    }; // class sort_expression

    /// \brief list of basic sorts
    typedef atermpp::term_list<basic_sort> basic_sort_list;
    /// \brief vector of basic sorts
    typedef atermpp::vector<basic_sort> basic_sort_vector;

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SORT_EXPRESSION_H

