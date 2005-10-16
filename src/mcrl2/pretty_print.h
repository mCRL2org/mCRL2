#ifndef MCRL2_PRETTY_PRINT_H
#define MCRL2_PRETTY_PRINT_H

#include "atermpp/aterm.h"
#include "libprint_cxx.h"

namespace mcrl2 {

  inline
  std::string pretty_print(ATerm t)
  {
    return PrintPart_CXX(t, ppAdvanced);
  }

} // namespace mcrl

#endif // MCRL2_PRETTY_PRINT_H
