#ifndef MCRL2_PRETTY_PRINT_H
#define MCRL2_PRETTY_PRINT_H

#include "atermpp/atermpp.h"
#include "libprint.h"
#include <string>

namespace mcrl2 {

  inline
  std::string pretty_print(ATerm t)
  {
    return PrintPart_CXX(t, ppAdvanced);
  }

  /// Returns a pretty print representation of the term_list.
  ///
  template <typename Term>
  std::string pp(atermpp::term_list<Term> l)
  {
    return pretty_print(atermpp::aterm(l));
  }

} // namespace mcrl

#endif // MCRL2_PRETTY_PRINT_H
