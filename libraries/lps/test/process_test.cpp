// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file process_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/lps/process.h"
#include "mcrl2/lps/process_expression_visitor.h"
#include "mcrl2/lps/process_expression_builder.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::lps;

void visit_process_expression(const process_expression& x)
{
  process_expression_visitor<> visitor;
  visitor.visit(x);
}

void build_process_expression(const process_expression& x)
{
  process_expression_builder<> visitor;
  visitor.visit(x);
}

void test_processes()
{
  const std::string SPECIFICATION =
    "act a;                  \n"
    "proc X(i: Nat) = a.X(i);\n"
    "init X(2);              \n"
    ;

  process_specification spec = parse_process_specification(SPECIFICATION);
  std::cout << core::pp(spec.data()) << std::endl
            << core::pp(spec.actions()) << std::endl
            << spec.equations() << std::endl
            << spec.init() << std::endl;

  for (process_equation_list::iterator i = spec.equations().begin(); i != spec.equations().end(); ++i)
  {
    std::cout << "<equation>" << core::pp(*i) << std::endl;
    visit_process_expression(i->expression());
    build_process_expression(i->expression());
  }
}

int test_main(int argc, char** argv )
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_processes();

  return 0;
}
