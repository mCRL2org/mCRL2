// Author(s): Aad Mathijssen
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

using namespace std;

void PrintPart_CXX(std::ostream &OutStream, const ATerm Part,
  t_pp_format pp_format)
{
  PrintPart__CXX(OutStream, (ATerm) Part, pp_format);
}

string PrintPart_CXX(const ATerm Part, t_pp_format pp_format)
{
  stringstream ss;
  PrintPart_CXX(ss, Part, pp_format);
  return ss.str();
}

