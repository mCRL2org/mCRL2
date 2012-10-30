// Author(s): Wieger Wesselink, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linearization_test1.cpp
/// \brief This test contains more examples of linearizations that
//         failed in the past. See also linearization_test.cpp.

#include <iostream>
#include <string>

#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/lps/linearise.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/test_utilities.h"

using namespace mcrl2;
using mcrl2::utilities::collect_after_test_case;
using namespace mcrl2::lps;

BOOST_GLOBAL_FIXTURE(collect_after_test_case)

typedef data::basic_rewriter<data::data_expression>::strategy rewrite_strategy;
typedef std::vector<rewrite_strategy> rewrite_strategy_vector;

void run_linearisation_instance(const std::string& spec, const t_lin_options& options, bool expect_success)
{
  if (expect_success)
  {
    lps::specification s = linearise(spec, options);
    BOOST_CHECK(s != lps::specification());
  }
  else
  {
    BOOST_CHECK_THROW(linearise(spec, options), mcrl2::runtime_error);
  }
}

void run_linearisation_test_case(const std::string& spec, const bool expect_success = true)
{
  // Set various rewrite strategies
  rewrite_strategy_vector rewrite_strategies = utilities::get_test_rewrite_strategies(false);

  for (rewrite_strategy_vector::const_iterator i = rewrite_strategies.begin(); i != rewrite_strategies.end(); ++i)
  {
    std::clog << std::endl << "Testing with rewrite strategy " << data::pp(*i) << std::endl;

    t_lin_options options;
    options.rewrite_strategy=*i;

    std::clog << "  Default options" << std::endl;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method regular2" << std::endl;
    options.lin_method=lmRegular2;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method stack" << std::endl;
    options.lin_method=lmStack;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method stack; binary enabled" << std::endl;
    options.binary=true;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method regular; binary enabled" << std::endl;
    options.lin_method=lmRegular;
    run_linearisation_instance(spec, options, expect_success);

    std::clog << "  Linearisation method regular; no intermediate clustering" << std::endl;
    options.binary=false; // reset binary
    options.no_intermediate_cluster=true;
    run_linearisation_instance(spec, options, expect_success);
  }
}

// The test case below went wrong due to a lambda expression
// that was applied to an empty list of arguments, which was not
// properly dealt with.

BOOST_AUTO_TEST_CASE(test_lambda_expressions)
{
  const std::string spec =
     "map infty : Nat;\n"
     "map N,k : Nat;\n"
     "\n"
     "var n:Nat;\n"
     "eqn\n"
     "infty = 100000;\n"
     "\n"
     "N = 3;\n"
     "k = 2;\n"
     "\n"
     "\n"
     "proc NCS (p:Nat)\n"
     "=\n"
     "set_want(p).\n"
     "sum t:Nat.\n"
     "(\n"
     "  set_ticket(p,t).\n"
     "  set_want(p).  WAIT(p,{i:Nat | (1 <= i) && (i <= N) && (i != p)},false,t,{i:Nat | (1 <= i) && (i <= N) && (i != p)}, N-1)\n"
     ")\n"
     ";\n"
     "\n"
     "proc WAIT (p:Nat,pred:Set(Nat),captured:Bool,ticket:Nat,j:Set(Nat),cnt:Int)\n"
     "=\n"
     "(cnt >= k && !captured)\n"
     "->\n"
     "(\n"
     "  (j != {})\n"
     "  ->\n"
     "  sum l,m:Nat. (l in j) -> rec_want(m,l).\n"
     "                          ( (ticket < l && p < l) ->\n"
     "                               WAIT(pred = pred- {l}, j = j-{l}, cnt = cnt-1)\n"
     "                            <> WAIT(j = j-{l}) )\n"
     "  <> sum b:Bool. get_captured(p,b).WAIT(captured=b)\n"
     ")\n"
     "<>\n"
     "(\n"
     " snd_capture_reset(p).\n"
     " enter_cs(p).\n"
     " leave_cs(p).\n"
     " EXIT(p)\n"
     ")\n"
     ";\n"
     "\n"
     "proc EXIT (p:Nat)\n"
     "=\n"
     " set_want_reset(p).\n"
     " NCS(p)\n"
     ";\n"
     "\n"
     "proc MEM (want: Nat->(Nat->Nat),ticket : Nat->Nat,capture: Nat->(Nat->Nat),Max:Nat)\n"
     "=\n"
     "(sum p:Nat. (p <= N) -> get_want(p). MEM(want = want[p -> lambda i:Nat. ticket(p)]))\n"
     "+\n"
     "sum i,p:Nat. (i <= N && p <= N) -> send_want(want(i)(p),i). MEM()\n"
     "+\n"
     "(sum p:Nat. (p <= N) -> get_ticket(p,Max+1). MEM(ticket = ticket[p -> Max+1], Max = Max+1))\n"
     "+\n"
     "(sum p:Nat. (p <= N) -> set_captured(p, exists i:Nat. ((i <= N) && (ticket(p) < capture(i)(p)) )). MEM(capture = capture[p -> lambda i:Nat. ticket(p)]))\n"
     "+\n"
     "(sum p:Nat. (p <= N) -> get_want_reset(p). MEM(want = want[p -> lambda i:Nat. infty]))\n"
     "+\n"
     "(sum p:Nat. (p <= N) -> rcv_capture_reset(p). MEM(capture = capture[p -> lambda i:Nat. ticket(p)]) )\n"
     ";\n"
     "\n"
     "act\n"
     "snd_capture_reset,rcv_capture_reset,capture_reset : Nat;\n"
     "get_want,set_want, want : Nat;\n"
     "get_want_reset,set_want_reset, want_reset : Nat;\n"
     "get_captured,set_captured,captured : Nat#Bool;\n"
     "get_ticket,set_ticket,ticket : Nat # Nat;\n"
     "enter_cs,leave_cs : Nat;\n"
     "send_want,rec_want, comm_want:Nat # Nat;\n"
     "\n"
     "\n"
     "init\n"
     "allow(\n"
     "{want, want_reset, captured, ticket, enter_cs, leave_cs},\n"
     "  comm(\n"
     "  {get_want|set_want -> want,\n"
     "   get_want_reset|set_want_reset -> want_reset,\n"
     "   get_captured|set_captured->captured,\n"
     "   get_ticket|set_ticket->ticket,\n"
     "   snd_capture_reset|rcv_capture_reset -> capture_reset,\n"
     "   send_want|rec_want -> comm_want\n"
     "  },\n"
     "  NCS(1)||NCS(2)||NCS(3)||MEM(lambda i:Nat. (lambda j:Nat. infty),lambda i:Nat.0,lambda i:Nat. (lambda j:Nat. 0), 0 )\n"
     "  )\n"
     ")\n"
     ";\n";

  run_linearisation_test_case(spec);
}


// The testcase below is added because the typechecker could not deal with this
// case. The three options for the action a caused it to become confused.
BOOST_AUTO_TEST_CASE(test_multiple_action_types)
{
  const std::string spec =
     "sort Id = Pos;\n"
     "\n"
     "map FALSE: Id -> Bool;\n"
     "var n: Id;\n"
     "eqn FALSE(n) = false;\n"
     "\n"
     "act a: (Id -> Bool);\n"
     "    a: Bool;\n"
     "    a: Pos;\n"
     "\n"
     "proc P = a(FALSE) . delta;\n"
     "init P;\n";
 
  run_linearisation_test_case(spec);
}


boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}

