///////////////////////////////////////////////////////////////////////////////
/// \file function.h
/// Contains function data structures for the LPS Library.

#ifndef LPS_FUNCTION_H
#define LPS_FUNCTION_H

#include "lps/data.h"

namespace lps {

/// \brief Note that data_operation and function are the same.
typedef data_operation function;

/// \brief Note that data_operation and function are the same.
typedef data_operation_list function_list;

/// \brief Returns true if the term t is a function
inline
bool is_function(aterm_appl t)
{
  return is_data_operation(t);
}

} // namespace lps

#endif // LPS_FUNCTION_H
