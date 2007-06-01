///////////////////////////////////////////////////////////////////////////////
/// \file identifier_string.h

#ifndef LPS_IDENTIFIER_STRING_H
#define LPS_IDENTIFIER_STRING_H

#include "atermpp/aterm_string.h"

namespace lps {

  /// \brief String type of the LPS library
  /// Identifier strings are represented internally as ATerms.
  typedef atermpp::aterm_string identifier_string;
  
  /// Test if a term is an identifier string.
  inline
  bool is_identifier_string(atermpp::aterm t)
  {
    return t.type() == AT_APPL && atermpp::aterm_appl(t).size() == 0;
  }

} // namespace lps

#endif // LPS_IDENTIFIER_STRING_H
