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

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/data/data_specification.h"

#include "mcrl2/data/detail/compatibility.h"

namespace mcrl2 {

  namespace data {

    namespace detail {

      /// \brief Convert a data specification in the old data format before data
      ///        implementation to a data specification in the new data format.
      /// \param data_spec A data specification in the old data format before
      ///                  data implementation.
      /// \ret The data specification in the new data format equivalent to
      ///      data_spec.
      inline
      data_specification aterm_data_spec_to_data_specification(const atermpp::aterm_appl& data_spec)
      {
        assert(core::detail::gsIsDataSpec(data_spec));

        return data_specification(aterm_sort_spec_to_sort_expression_list(atermpp::arg1(data_spec)),
                                  aterm_cons_spec_to_constructor_map(atermpp::arg2(data_spec)),
                                  aterm_map_spec_to_function_list(atermpp::arg3(data_spec)),
                                  aterm_data_eqn_spec_to_equation_list(atermpp::arg4(data_spec)));
      }

      /// \brief Convert a data specification in the new data format to a data
      ///        specification in the old data format before data
      ///        implementation.
      /// \param s A data specification in the new data format.
      /// \ret The data specification in the old data format equivalent to s.
      inline
      atermpp::aterm_appl data_specification_to_aterm_data_spec(const data_specification& s)
      {
        return core::detail::gsMakeDataSpec(sort_expression_list_to_aterm_sort_spec(s.sorts()),
                                            constructor_list_to_aterm_cons_spec(s.constructors()),
                                            function_list_to_aterm_map_spec(s.functions()),
                                            data_equation_list_to_aterm_eqn_spec(s.equations()));
      }

    } // namespace detail
    
  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_DETAIL_DATA_SPECIFICATION_COMPATIBILITY_H

