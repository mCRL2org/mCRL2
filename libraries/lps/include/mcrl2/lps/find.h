// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/find.h
/// \brief Search functions of the data library.

#ifndef MCRL2_LPS_FIND_H
#define MCRL2_LPS_FIND_H

#include <algorithm>
#include <functional>
#include "mcrl2/lps/action.h"
#include "mcrl2/lps/summand.h"
#include "mcrl2/data/find.h"
#include "mcrl2/lps/detail/lps_free_variable_finder.h"

namespace mcrl2 {

  namespace lps {

    /// \brief Returns all data variables that occur in a range of expressions
    /// \param[in] container a container with expressions
    /// \return All data variables that occur in the term t
    /// TODO replace uses by data::find_free_variables
    template <typename Container, typename OutputIterator >
    void find_free_variables(Container const& container, OutputIterator const& o)
    {
//      detail::make_lps_variable_traverser(o)(container);
    }

    /// \brief Returns all data variables that occur in a range of expressions
    /// \param[in] container a container with expressions
    /// \return All data variables that occur in the term t
    /// TODO replace uses by data::find_free_variables
    template <typename Container, typename OutputIterator, typename Sequence >
    void find_free_variables(Container const& container, OutputIterator const& o, Sequence bound)
    {
//      detail::make_lps_variable_traverser(o)(container);
    }

    /// \brief Returns all data variables that occur in a range of expressions
    /// \param[in] container a container with expressions
    /// \return All data variables that occur in the term t
    /// TODO replace uses by data::find_free_variables
    template <typename Container >
    std::set< data::variable > find_free_variables(Container const& container)
    {
      std::set< data::variable > result;

      find_free_variables(container, std::inserter(result, result.end()));

      return result;
    }

    /// \brief Returns all free (unbound) data variables that appear in the
    /// LPS object o.
    template <typename Object>
    std::set<data::variable> lps_find_all_free_variables(const Object& o)
    {
      std::set<data::variable> result;
      lps::detail::lps_free_variable_finder<std::insert_iterator<std::set<data::variable> > > finder(std::inserter(result, result.end()));
      finder(o);
      return result;
    }

  } // namespace  lps
} // namespace mcrl2

#endif

