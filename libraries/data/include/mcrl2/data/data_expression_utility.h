// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_expression_utility.h
/// \brief Provides utilities for working with data expression.

#ifndef MCRL2_DATA_DATA_EXPRESSION_UTILITY_H
#define MCRL2_DATA_DATA_EXPRESSION_UTILITY_H

#include <iostream>

#include "mcrl2/data/data_expression.h"

namespace mcrl2 {

  namespace data {

    /// \brief Returns the sorts of the data expressions in the input
    /// \param[in] l A range of data expressions
    /// \ret The sorts of the data expressions in l.
    template <typename T>
    sort_expression_list sorts_of_data_expressions(const boost::iterator_range<T>& l)
    {
      sort_expression_list result;

      for(T i = l.begin(); i != l.end(); ++i)
      {
        std::cerr << i->sort() << std::endl;
        result.push_back(i->sort());
      }

      return result;
    }

  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_DATA_EXPRESSION_UTILITY_H

