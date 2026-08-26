// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesparvalues_test.cpp
/// \brief Test the static exploration of reachable parameter values in a PBES

#define BOOST_TEST_MODULE pbesparvalues_test
#include <boost/test/included/unit_test.hpp>

#include <limits>

#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbesparvalues.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

// Maps parameters to their possible values, represented by the strings
// "(equation, parameter)" and the pretty printed values.
std::map<std::string, std::set<std::string>> domains(const std::vector<pbes_parameter_domain>& result)
{
  std::map<std::string, std::set<std::string>> result_map;
  for (const pbes_parameter_domain& entry: result)
  {
    std::set<std::string>& values =
      result_map["(" + core::pp(entry.equation) + ", " + core::pp(entry.parameter.name()) + ")"];
    for (const data::data_expression& value: entry.values)
    {
      values.insert(pp(value));
    }
  }
  return result_map;
}

void check_domains(
  const std::map<std::string, std::set<std::string>>& actual,
  const std::map<std::string, std::set<std::string>>& expected,
  const std::string& title)
{
  if (actual != expected)
  {
    auto print = [](const std::map<std::string, std::set<std::string>>& domains_)
    {
      for (const auto& [parameter, values]: domains_)
      {
        std::cout << "  " << parameter << " := {";
        for (auto i = values.begin(); i != values.end(); ++i)
        {
          if (i != values.begin())
          {
            std::cout << ", ";
          }
          std::cout << *i;
        }
        std::cout << "}\n";
      }
    };
    std::cout << "--- failure in " << title << " ---\n";
    std::cout << "actual:\n";
    print(actual);
    std::cout << "expected:\n";
    print(expected);
  }
  BOOST_CHECK(actual == expected);
}

pbes_parvalues_algorithm<data::rewriter>::result_type compute_domains(
  const std::string& pbes_text,
  bool expand_finite_sorts = false)
{
  pbes p = txt2pbes(pbes_text);
  data::rewriter r(p.data());
  return pbes_parvalues_algorithm<data::rewriter>(p, r, 100, std::numeric_limits<std::size_t>::max(),
                                                  expand_finite_sorts).run();
}

// The guard n < 5 restricts the values passed to Y, and X grows its own
// parameter through the self-reference X(n + 1).
void test_guarded_chain()
{
  const std::string spec =
    "pbes nu X(n: Nat) =                    \n"
    "  (val(n < 5)) => Y(n + 1, true)       \n"
    "                  && X(n + 1);         \n"
    "nu Y(m: Nat, b: Bool) = Y(m,b);          \n"
    "init X(0);                             \n";

  const std::map<std::string, std::set<std::string>> expected =
    {
      { "(X, n)", { "0", "1", "2", "3", "4", "5" } },
      { "(Y, m)", { "1", "2", "3", "4", "5" } },
      { "(Y, b)", { "true" } }
    };
  const auto actual = domains(compute_domains(spec));
  check_domains(actual, expected, "test_guarded_chain");
}

// Quantifiers over finite and infinite sorts: the update k := m + e is guarded
// by e <= 2, hence only e in {0, 1, 2} contributes, and c := f ranges over all
// booleans because of the existential quantifier.
void test_quantifiers_and_shared_names()
{
  const std::string spec =
    "pbes nu X(n: Nat) =                                    \n"
    "  (val(n < 5)) => Y(n + 1, true) && X(n + 1);          \n"
    "nu Y(m: Nat, b: Bool) =                                \n"
    "  forall e: Nat.                                       \n"
    "    (val(e <= 2)) => exists f: Bool. Z(m + e, f);      \n"
    "nu Z(k: Nat, c: Bool) =                                \n"
    "  (val(k == 0 || c)) => W(k);                          \n"
    "nu W(n: Nat) =                                         \n"
    "  val(n >= 0);                                         \n"
    "init X(0);                                             \n";

  const std::map<std::string, std::set<std::string>> expected =
    {
      { "(X, n)", { "0", "1", "2", "3", "4", "5" } },
      { "(Y, m)", { "1", "2", "3", "4", "5" } },
      { "(Y, b)", { "true" } },
      { "(Z, k)", { "1", "2", "3", "4", "5", "6", "7" } },
      { "(Z, c)", { "false", "true" } },
      // W uses the same parameter name n as X; alpha-renaming keeps the domains apart.
      { "(W, n)", { "1", "2", "3", "4", "5", "6", "7" } }
    };
  const auto actual = domains(compute_domains(spec));
  check_domains(actual, expected, "test_quantifiers_and_shared_names");

  // Expanding quantifiers over finite sorts does not change the outcome.
  const auto expanded = domains(compute_domains(spec, true));
  check_domains(expanded, expected, "test_quantifiers_expanded");
}

// Equations that cannot be reached from the initial state do not contribute.
void test_unreachable_equations()
{
  const std::string spec =
    "pbes nu X(b: Bool) = val(!b) || X(!b); \n"
    "mu U(u: Nat) = val(u < 10);            \n"
    "init X(true);                          \n";

  const std::map<std::string, std::set<std::string>> expected =
    {
      { "(X, b)", { "false", "true" } }
    };
  const auto actual = domains(compute_domains(spec));
  check_domains(actual, expected, "test_unreachable_equations");
}

// The quantifier ranges over the finite sort E, but the guard filters out e0,
// hence the domain of d does not contain all elements of E.
void test_partial_finite_domain()
{
  const std::string spec =
    "sort E = struct e0 | e1 | e2;                        \n"
    "pbes nu X(n: Nat) =                                  \n"
    "  (val(n < 3)) => Y(n) && X(n + 1);                  \n"
    "nu Y(m: Nat) =                                       \n"
    "  forall e: E.                                       \n"
    "    (val(e == e1 || e == e2)) => Z(m, e);            \n"
    "nu Z(k: Nat, d: E) = true;                           \n"
    "init X(0);                                           \n";

  const std::map<std::string, std::set<std::string>> expected =
    {
      // The update n := n + 1 is guarded by n < 3, hence 3 is included.
      { "(X, n)", { "0", "1", "2", "3" } },
      { "(Y, m)", { "0", "1", "2" } },
      { "(Z, k)", { "0", "1", "2" } },
      // Note that e0 is missing: the guard excludes it.
      { "(Z, d)", { "e1", "e2" } }
    };
  const auto actual = domains(compute_domains(spec));
  check_domains(actual, expected, "test_partial_finite_domain");

  // Expanding quantifiers over finite sorts does not change the outcome.
  const auto expanded = domains(compute_domains(spec, true));
  check_domains(expanded, expected, "test_partial_finite_domain_expanded");
}

// An unconstrained quantifier over the infinite sort List(Bool) feeds every
// list into Z. Its domain cannot be enumerated finitely; the algorithm should
// report the parameter as unbounded instead of aborting.
void test_unbounded_domain()
{
  const std::string spec =
    "pbes nu X(n: Nat) =                            \n"
    "  (val(n < 2)) => Y(n) && X(n + 1);            \n"
    "nu Y(m: Nat) = forall l: List(Bool). Z(m, l);  \n"
    "nu Z(k: Nat, w: List(Bool)) = true;            \n"
    "init X(0);                                     \n";

  const auto actual = compute_domains(spec);

  const std::map<std::string, std::set<std::string>> expected =
    {
      // Exploration stops as soon as (Z, w) is found to be unbounded, so
      // domains downstream of Y are truncated: X reached {0, 1} but 2 was
      // never propagated, and Z received no values at all.
      { "(X, n)", { "0", "1" } },
      { "(Y, m)", { "0" } },
      { "(Z, k)", {} },
      // No values can be enumerated for the unbounded parameter.
      { "(Z, w)", {} }
    };
  check_domains(domains(actual), expected, "test_unbounded_domain");

  // Only the list-valued parameter of Z depends on an unconstrained variable
  // with an infinite sort; it is reported as (potentially) unbounded.
  std::map<std::string, bool> unbounded;
  for (const pbes_parameter_domain& entry: actual)
  {
    unbounded["(" + core::pp(entry.equation) + ", " + core::pp(entry.parameter.name()) + ")"] = entry.unbounded;
  }
  const std::map<std::string, bool> expected_unbounded =
    {
      { "(X, n)", false },
      { "(Y, m)", false },
      { "(Z, k)", false },
      { "(Z, w)", true }
    };
  BOOST_CHECK(unbounded == expected_unbounded);
}

BOOST_AUTO_TEST_CASE(test_pbesparvalues)
{
  test_guarded_chain();
  test_quantifiers_and_shared_names();
  test_unreachable_equations();
  test_partial_finite_domain();
  test_unbounded_domain();
}
