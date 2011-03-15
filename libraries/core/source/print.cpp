// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file print.cpp

#include "mcrl2/core/print.h"


#include <sstream>

#define PRINT_CXX
#include "mcrl2/core/detail/print_implementation.h"

namespace mcrl2
{
namespace core
{

void PrintPart_CXX(std::ostream& out_stream, const ATerm part,
                   t_pp_format pp_format)
{
  mcrl2::core::detail::PrintPart__CXX(out_stream, part, pp_format);
}

std::string PrintPart_CXX(const ATerm part, t_pp_format pp_format)
{
  std::stringstream ss;
  PrintPart_CXX(ss, part, pp_format);
  return ss.str();
}

}
}
