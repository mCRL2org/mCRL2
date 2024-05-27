// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_DATA_IO_H
#define MCRL2_DATA_IO_H

#include "mcrl2/atermpp/aterm_io.h"

namespace mcrl2::data
{

// forward declarations.
class data_specification;

namespace detail
{
  /// \returns A term that represetns this data_specification.
  atermpp::aterm data_specification_to_aterm(const data_specification& s);
}

/// \brief Transforms DataVarId to DataVarIdNoIndex and transforms OpId to OpIdNoIndex
atermpp::aterm add_index(const atermpp::aterm& x);

/// \brief Transforms DataVarIdNoIndex to DataVarId and transforms OpIdNoIndex to OpId
atermpp::aterm remove_index(const atermpp::aterm& x);

/// \brief Reads a data specification from a stream.
atermpp::aterm_istream& operator>>(atermpp::aterm_istream& stream, data_specification& spec);

/// \brief Writes the data specification to a stream.
atermpp::aterm_ostream& operator<<(atermpp::aterm_ostream& stream, const data_specification& spec);

} // namespace mcrl2::data

#endif // MCRL2_DATA_IO_H
