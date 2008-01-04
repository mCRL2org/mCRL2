// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file garbage.cpp
/// \brief Add your file description here.

#include <sstream>
#include "mcrl2/core/struct.h"
#include "mcrl2/core/detail/parse.h"
#include "mcrl2/core/detail/typecheck.h"
#include "mcrl2/core/core_init.h"
#include "gc.h"

const std::string SPECIFICATION =
"act a:Nat;                               \n"
"                                         \n"
"map smaller: Nat#Nat -> Bool;            \n"
"                                         \n"
"var x,y : Nat;                           \n"
"                                         \n"
"eqn smaller(x,y) = x < y;                \n"
"                                         \n"
"proc P(n:Nat) = sum m: Nat. a(m). P(m);  \n"
"                                         \n"
"init P(0);                               \n";

int main()
{
  MCRL2_CORE_LIBRARY_INIT()

  std::stringstream spec_stream;
  spec_stream << SPECIFICATION;
  ATermAppl t = parse_spec(spec_stream);
  AT_collect();
  if (t == NULL)
    return 1;
  t = type_check_spec(t);

  return 0;
}
