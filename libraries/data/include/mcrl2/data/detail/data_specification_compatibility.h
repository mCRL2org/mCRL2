// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/compatibility.h
/// \brief Conversion utilities for transforming between 
///        data specifications in the old and the new format.

#ifndef MCRL2_DATA_DETAIL_DATA_SPECIFICATION_COMPATIBILITY_H
#define MCRL2_DATA_DETAIL_DATA_SPECIFICATION_COMPATIBILITY_H

#include "mcrl2/data/data_specification.h"

namespace mcrl2 {

  namespace data {

    namespace detail {

      /// \brief Convert a data specification in the new data format to a data
      ///        specification in the old data format before data
      ///        implementation.
      /// \param s A data specification in the new data format.
      /// \return The data specification in the old data format equivalent to s.
      atermpp::aterm_appl data_specification_to_aterm_data_spec(const data_specification& s);

    } // namespace detail

  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_DETAIL_DATA_SPECIFICATION_COMPATIBILITY_H

