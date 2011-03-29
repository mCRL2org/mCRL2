// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pp_test.cpp
/// \brief Test for parser + pretty printer

#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/detail/lps_printer.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;

const std::string LIST_SPEC =
  "proc P(l_P: List(Nat)) =\n"
  "       (head(l_P) == 20) ->\n"
  "         tau .\n"
  "         P(l_P = [rhead(l_P)] ++ rtail(l_P))\n"
  "     + (l_P . 1 == 30) ->\n"
  "         tau .\n"
  "         P(l_P = tail(l_P))\n"
  "     + (1 in l_P) ->\n"
  "         tau .\n"
  "         P(l_P = 10 |> l_P ++ [#l_P] <| 100);\n"
  "\n"
  "init P([20, 30, 40]);\n"
  ;

void test_lps_printer()
{
  using namespace mcrl2::lps;

  lps::detail::lps_printer<std::ostream> print(std::cout);
  specification spec = linearise(lps::detail::ABP_SPECIFICATION());
  print(spec);
  print(spec.process());
  print.print_container(spec.process().action_summands());
  print.print_list(spec.process().process_parameters());
  //print.print_list(spec.process().global_variables());
}

void test_lps_print_lists()
{
  using namespace mcrl2::data;
  using namespace mcrl2::data::sort_bool;
  using namespace mcrl2::data::sort_list;
  using namespace mcrl2::lps;

  std::stringstream output;
  lps::detail::lps_printer<std::stringstream> print(output);

  specification spec = parse_linear_process_specification(LIST_SPEC);
  print(spec);

  BOOST_CHECK(output.str() == LIST_SPEC);
  if (output.str() != LIST_SPEC)
  {
    std::clog << "output.str() == LIST_SPEC failed: [" << output.str() << " == " << LIST_SPEC << "]" << std::endl;
  }

  data_expression list_false_true = cons_(bool_(), false_(), cons_(bool_(), true_(), nil(bool_())));
  std::stringstream list_output;
  lps::detail::lps_printer<std::stringstream> list_print(list_output);
  list_print(list_false_true);
  BOOST_CHECK(list_output.str() == "[false, true]");

  if (list_output.str() != "[false, true]")
  {
    std::clog << "list_output.str() == [false, true] failed: [" << list_output.str() << " == " << "[false, true]" << "]" << std::endl;
  }
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_lps_printer();
  test_lps_print_lists();

  return 0;
}
