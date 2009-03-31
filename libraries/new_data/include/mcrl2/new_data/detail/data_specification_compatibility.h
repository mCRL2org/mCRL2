// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/detail/compatibility.h
/// \brief Conversion utilities for transforming between 
///        new_data specifications in the old and the new format.

#ifndef MCRL2_NEW_DATA_DETAIL_DATA_SPECIFICATION_COMPATIBILITY_H
#define MCRL2_NEW_DATA_DETAIL_DATA_SPECIFICATION_COMPATIBILITY_H

#include "mcrl2/new_data/data_specification.h"

namespace mcrl2 {

  namespace new_data {

    namespace detail {

      /// \brief Convert a new_data specification in the new new_data format to a new_data
      ///        specification in the old new_data format before new_data
      ///        implementation.
      /// \param s A new_data specification in the new new_data format.
      /// \return The new_data specification in the old new_data format equivalent to s.
      atermpp::aterm_appl data_specification_to_aterm_data_spec(const data_specification& s);

    } // namespace detail

  } // namespace new_data

} // namespace mcrl2

#endif //MCRL2_NEW_DATA_DETAIL_DATA_SPECIFICATION_COMPATIBILITY_H

