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

#include "mcrl2/data/variable.h"
#include "mcrl2/data/detail/find.h"
#include "mcrl2/lps/detail/lps_binding_aware_traverser.h"

namespace mcrl2 {

  namespace lps {

    /// \brief Returns all data variables that occur in a range of expressions
    /// \param[in] container a container with expressions
    /// \return All data variables that occur in the term t
    template <typename Container, typename OutputIterator >
    void find_free_variables(Container const& container, OutputIterator o)
    {
      data::detail::make_free_variable_find_helper< lps::detail::lps_binding_aware_traverser >(o)(container);
    }

    /// \brief Returns all data variables that occur in a range of expressions
    /// \param[in] container a container with expressions
    /// \return All data variables that occur in the term t
    /// TODO replace uses by data::find_free_variables
    template <typename Container >
    std::set< data::variable > find_free_variables(Container const& container)
    {
      std::set< data::variable > result;

      lps::find_free_variables(container, std::inserter(result, result.end()));

      return result;
    }

  } // namespace  lps
} // namespace mcrl2

#endif

