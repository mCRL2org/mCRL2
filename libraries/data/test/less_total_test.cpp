// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file less_total_test.cpp
/// \brief Regression tests for less_total, the total order that Set, Bag, FSet and FBag use internally to keep
///        their element lists canonically sorted. The ordinary < on sets is subset inclusion, which is only a
///        partial order, so it cannot be used for this purpose.

#define BOOST_TEST_MODULE less_total_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/bag.h"
#include "mcrl2/data/fbag.h"
#include "mcrl2/data/fset.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/set.h"

#include <sstream>
#include <string>
#include <vector>

using namespace mcrl2;
using namespace mcrl2::data;

namespace
{

/// \brief Returns a data specification that contains all the (nested) sorts used in the tests below.
///
/// The sorts must be declared explicitly, because the rewriter only knows the equations of the sorts that
/// occur in the specification. Numerals such as 1 have sort Pos, so most sorts occur in a Pos and in a Nat variant.
data_specification make_specification()
{
  data_specification specification;

  const sort_expression pos = sort_pos::pos();
  const sort_expression nat = sort_nat::nat();

  specification.add_context_sort(pos);
  specification.add_context_sort(nat);
  specification.add_context_sort(sort_int::int_());
  specification.add_context_sort(sort_real::real_());

  for (const sort_expression& element: {pos, nat})
  {
    specification.add_context_sort(sort_list::list(element));
    specification.add_context_sort(sort_set::set_(element));
    specification.add_context_sort(sort_bag::bag(element));
    specification.add_context_sort(sort_fset::fset(element));
    specification.add_context_sort(sort_fbag::fbag(element));

    // Sets and bags of sets, that are ordered by less_total on the sets, rather than by <.
    const sort_expression set_element = sort_set::set_(element);
    specification.add_context_sort(sort_set::set_(set_element));
    specification.add_context_sort(sort_bag::bag(set_element));

    const sort_expression fset_element = sort_fset::fset(element);
    specification.add_context_sort(sort_fset::fset(fset_element));
    specification.add_context_sort(sort_fbag::fbag(fset_element));
  }

  // Function sorts of arity two, for which less_total is defined by the generic rules in standard.h.
  specification.add_context_sort(function_sort(sort_expression_list({nat, nat}), nat));

  return specification;
}

/// \brief The rewriter (and thereby the specification) is shared between all tests.
data::rewriter& get_rewriter()
{
  static data_specification specification = make_specification();
  static data::rewriter normaliser(specification);
  return normaliser;
}

/// \brief Rewrites the given text to normal form.
data_expression normalise(const std::string& text)
{
  return get_rewriter()(parse_data_expression(text));
}

/// \brief Rewrites the given expression to normal form.
data_expression normalise(const data_expression& expression)
{
  return get_rewriter()(expression);
}

std::string to_string(const data_expression& expression)
{
  std::ostringstream out;
  out << expression;
  return out.str();
}

/// \brief Checks that the expression rewrites to true.
void check_holds(const std::string& text)
{
  const data_expression result = normalise(text);
  BOOST_CHECK_MESSAGE(result == sort_bool::true_(), "`" << text << "` should hold, but rewrote to " << result);
}

/// \brief Checks that the expression rewrites to false.
void check_fails(const std::string& text)
{
  const data_expression result = normalise(text);
  BOOST_CHECK_MESSAGE(result == sort_bool::false_(), "`" << text << "` should not hold, but rewrote to " << result);
}

/// \brief Evaluates less_total(a, b) on two ground expressions to a boolean; the result must be a proper boolean.
bool less_total_holds(const std::string& a, const std::string& b)
{
  const data_expression result = normalise("less_total(" + a + ", " + b + ")");
  BOOST_REQUIRE_MESSAGE(sort_bool::is_true_function_symbol(result) || sort_bool::is_false_function_symbol(result),
    "less_total(" << a << ", " << b << ") did not rewrite to a boolean, but to " << result);
  return sort_bool::is_true_function_symbol(result);
}

/// \brief Checks that less_total is a strict total order on the given ground values, that are pairwise distinct
///        according to ==.
///
/// In particular this checks irreflexivity, trichotomy (exactly one of a < b, a == b and b < a holds),
/// and transitivity.
void check_strict_total_order(const std::vector<std::string>& values)
{
  const std::size_t n = values.size();

  // Distinct values are assumed by the trichotomy check, so verify it.
  for (std::size_t i = 0; i < n; ++i)
  {
    for (std::size_t j = i + 1; j < n; ++j)
    {
      BOOST_REQUIRE_MESSAGE(normalise("(" + values[i] + ") == (" + values[j] + ")") == sort_bool::false_(),
        "the test values `" << values[i] << "` and `" << values[j] << "` are expected to be distinct");
    }
  }

  std::vector<std::vector<bool>> less(n, std::vector<bool>(n, false));
  for (std::size_t i = 0; i < n; ++i)
  {
    for (std::size_t j = 0; j < n; ++j)
    {
      less[i][j] = less_total_holds("(" + values[i] + ")", "(" + values[j] + ")");
    }
  }

  for (std::size_t i = 0; i < n; ++i)
  {
    BOOST_CHECK_MESSAGE(!less[i][i], "less_total is not irreflexive on `" << values[i] << "`");

    for (std::size_t j = 0; j < n; ++j)
    {
      if (i != j)
      {
        BOOST_CHECK_MESSAGE(less[i][j] != less[j][i],
          "less_total is not total and antisymmetric on `" << values[i] << "` and `" << values[j] << "`");
      }

      for (std::size_t k = 0; k < n; ++k)
      {
        if (less[i][j] && less[j][k])
        {
          BOOST_CHECK_MESSAGE(less[i][k],
            "less_total is not transitive on `" << values[i] << "`, `" << values[j] << "` and `" << values[k] << "`");
        }
      }
    }
  }
}

/// \brief Constructs the finite set {elements} of sort FSet(sort) using only the FSet constructors.
data_expression make_fset(const sort_expression& sort, const std::vector<data_expression>& elements)
{
  data_expression result = sort_fset::empty(sort);
  for (const data_expression& element: elements)
  {
    result = sort_fset::insert(sort, element, result);
  }
  return result;
}

/// \brief Constructs the finite bag of sort FBag(sort) with the given elements, all having multiplicity one.
data_expression make_fbag(const sort_expression& sort, const std::vector<data_expression>& elements)
{
  data_expression result = sort_fbag::empty(sort);
  for (const data_expression& element: elements)
  {
    result = sort_fbag::insert(sort, element, sort_pos::c1(), result);
  }
  return result;
}

} // namespace

BOOST_AUTO_TEST_CASE(test_less_total_is_typechecked_for_all_sorts)
{
  // less_total is a system defined function that is available on every sort, also those without a proper order.
  check_holds("less_total(false, true)");
  check_holds("less_total(1, 2)");
  check_holds("less_total(1, 2 + 1)");
  check_holds("less_total(-3, -2)");
  check_holds("less_total(1/2, 3/4)");
  check_holds("less_total([1], [2])");
  check_holds("less_total({1}, {2})");
  check_holds("less_total({1: 1}, {2: 1})");
}

BOOST_AUTO_TEST_CASE(test_less_total_on_basic_sorts)
{
  // Bool: false < true.
  check_holds("less_total(false, true)");
  check_fails("less_total(true, false)");
  check_fails("less_total(true, true)");
  check_fails("less_total(false, false)");

  // Pos, Nat, Int and Real: less_total coincides with <.
  check_holds("less_total(1, 2)");
  check_fails("less_total(2, 1)");
  check_fails("less_total(2, 2)");
  check_holds("less_total(0, Int2Nat(1))");
  check_fails("less_total(Int2Nat(1), 0)");
  check_holds("less_total(-2, -1)");
  check_holds("less_total(-1, Nat2Int(0))");
  check_fails("less_total(Nat2Int(0), -1)");
  check_holds("less_total(1/3, 1/2)");
  check_fails("less_total(1/2, 1/3)");
  check_fails("less_total(1/2, 2/4)");

  // Agreement with the ordinary order on the numeric sorts.
  check_holds("less_total(2, 5) == (2 < 5)");
  check_holds("less_total(5, 2) == (5 < 2)");
  check_holds("less_total(-5, -2) == (-5 < -2)");
  check_holds("less_total(-2, -5) == (-2 < -5)");
  check_holds("less_total(1/2, 2/3) == (1/2 < 2/3)");
}

BOOST_AUTO_TEST_CASE(test_less_total_is_a_strict_total_order)
{
  check_strict_total_order({"false", "true"});
  check_strict_total_order({"1", "2", "3", "100"});
  // The values of the numeric sorts are converted explicitly, since less_total does not upcast mixed arguments.
  check_strict_total_order({"0", "Int2Nat(1)", "Int2Nat(2)", "Int2Nat(1000000)"});
  check_strict_total_order({"-5", "-1", "Nat2Int(0)", "Pos2Int(1)", "Pos2Int(7)"});
  check_strict_total_order({"-1/2", "Nat2Real(0)", "1/3", "1/2", "Pos2Real(2)"});
  check_strict_total_order({"tail([1])", "[1]", "[2]", "[1, 1]", "[1, 2]", "[2, 1]", "[3]"});
}

BOOST_AUTO_TEST_CASE(test_less_total_is_a_strict_total_order_on_sets_and_bags)
{
  // The values {1, 2} and {3} are incomparable under <, which is subset inclusion on sets, but less_total orders them.
  check_strict_total_order({"{1} - {1}", "{1}", "{2}", "{1, 2}", "{3}", "{1, 3}", "{1, 2, 3}"});
  check_strict_total_order({"{1: 1} - {1: 1}", "{1: 1}", "{1: 2}", "{2: 1}", "{1: 1, 2: 1}", "{3: 1}", "{1: 2, 3: 1}"});
}

BOOST_AUTO_TEST_CASE(test_less_total_differs_from_the_subset_order)
{
  // Neither of these sets is a subset of the other, so < and <= fail in both directions ...
  check_fails("{1, 2} < {3}");
  check_fails("{3} < {1, 2}");
  check_fails("{1, 2} <= {3}");
  check_fails("{3} <= {1, 2}");

  // ... whereas exactly one direction holds for less_total.
  BOOST_CHECK(less_total_holds("{1, 2}", "{3}") != less_total_holds("{3}", "{1, 2}"));
  check_fails("less_total({1, 2}, {1, 2})");

  // The same holds for bags.
  check_fails("{1: 1, 2: 1} < {3: 1}");
  check_fails("{3: 1} < {1: 1, 2: 1}");
  BOOST_CHECK(less_total_holds("{1: 1, 2: 1}", "{3: 1}") != less_total_holds("{3: 1}", "{1: 1, 2: 1}"));
}

BOOST_AUTO_TEST_CASE(test_less_total_on_lists_is_lexicographic_on_head_and_tail)
{
  // The empty list is written as tail([1]), since the sort of [] cannot be inferred from less_total alone.
  check_holds("less_total(tail([1]), [1])");
  check_fails("less_total([1], tail([1]))");
  check_fails("less_total(tail([1]), tail([1]))");
  check_holds("less_total([1], [2])");
  check_holds("less_total([1], [1, 1])");
  check_holds("less_total([1, 5], [2])");
  check_fails("less_total([2], [1, 5])");
}

BOOST_AUTO_TEST_CASE(test_less_total_on_functions)
{
  // less_total(f, f) must be false for any function, so that (equal) set comparisons terminate with false.
  check_fails("less_total(lambda x: Nat. x, lambda x: Nat. x)");
  check_fails("less_total(lambda x: Nat, y: Nat. x + y, lambda x: Nat, y: Nat. x + y)");
}

/// \brief {1, 2} and {3} are incomparable under the subset order of Set(Nat), so building nested sets out of them
///        exercises exactly the guards of the insertion and union rules that switched from < to less_total.
BOOST_AUTO_TEST_CASE(test_nested_set_union_reduces_to_a_canonical_normal_form)
{
  const data_expression forward = normalise("{{1, 2}} + {{3}}");
  const data_expression backward = normalise("{{3}} + {{1, 2}}");

  // Canonical: the same set, built in either order, normalises identically.
  BOOST_CHECK_MESSAGE(forward == backward, "not canonical: " << forward << " versus " << backward);

  // Fully reduced: no leftover union or insert node remains in the normal form.
  const std::string text = to_string(forward);
  BOOST_CHECK_MESSAGE(text.find('+') == std::string::npos, "got a stuck normal form: " << text);
  BOOST_CHECK_MESSAGE(text.find("@fset_insert") == std::string::npos, "got a stuck normal form: " << text);
}

BOOST_AUTO_TEST_CASE(test_nested_set_intersection_and_difference)
{
  check_holds("{{1, 2}, {3}} * {{3}, {4}} == {{3}}");
  check_holds("{{3}, {4}} * {{1, 2}, {3}} == {{3}}");
  check_holds("{{1, 2}, {3}} - {{3}} == {{1, 2}}");
  check_holds("{{1, 2}, {3}} - {{1, 2}} == {{3}}");
  check_holds("{{1, 2}, {3}} - {{1, 2}, {3}} == {} - {}");
}

BOOST_AUTO_TEST_CASE(test_nested_set_cardinality_and_membership_after_union)
{
  check_holds("#({{1, 2}, {3}}) == 2");
  check_holds("#({{1, 2}} + {{3}}) == 2");
  check_holds("#({{3}} + {{1, 2}}) == 2");
  check_holds("#({{1, 2}} + {{1, 2}}) == 1");

  check_holds("{1, 2} in ({{1, 2}} + {{3}})");
  check_holds("{3} in ({{1, 2}} + {{3}})");
  check_holds("{3} in ({{3}} + {{1, 2}})");
  check_fails("{1} in ({{1, 2}} + {{3}})");
  check_fails("{1, 3} in ({{1, 2}} + {{3}})");
}

BOOST_AUTO_TEST_CASE(test_nested_set_is_independent_of_insertion_order)
{
  check_holds("{{1, 2}, {3}, {1}} == {{1}, {3}, {1, 2}}");
  check_holds("{{1, 2}, {3}, {1}} == {{3}, {1}, {1, 2}}");
  check_holds("{{1, 2}, {3}, {1}} == {{1, 2}, {3}, {1}, {3}, {1, 2}}");
  check_fails("{{1, 2}, {3}} == {{1, 2}, {3}, {1}}");
}

/// \brief The same non-total < gap exists for Bag, whose elements here are themselves Set(Nat) values.
BOOST_AUTO_TEST_CASE(test_nested_bag_union_reduces_to_a_canonical_normal_form)
{
  const data_expression forward = normalise("{{1, 2}: 1} + {{3}: 1}");
  const data_expression backward = normalise("{{3}: 1} + {{1, 2}: 1}");

  BOOST_CHECK_MESSAGE(forward == backward, "not canonical: " << forward << " versus " << backward);

  const std::string text = to_string(forward);
  BOOST_CHECK_MESSAGE(text.find('+') == std::string::npos, "got a stuck normal form: " << text);
  BOOST_CHECK_MESSAGE(text.find("@fbag_insert") == std::string::npos, "got a stuck normal form: " << text);

  check_holds("count({1, 2}, {{1, 2}: 1} + {{3}: 1}) == 1");
  check_holds("count({3}, {{1, 2}: 1} + {{3}: 1}) == 1");
  check_holds("count({1}, {{1, 2}: 1} + {{3}: 1}) == 0");
  check_holds("count({3}, {{3}: 2} + {{3}: 1}) == 3");
  check_holds("{{1, 2}: 1} + {{3}: 1} == {{3}: 1} + {{1, 2}: 1}");
}

/// \brief Builds FSet(FSet(Nat)) directly from the internal FSet constructors, which are not reachable through the
///        user syntax, and checks that insertion (which is guarded by less_total) sorts the elements canonically.
BOOST_AUTO_TEST_CASE(test_nested_fset_insert_is_canonical)
{
  const sort_expression nat = sort_nat::nat();
  const sort_expression fset_nat = sort_fset::fset(nat);

  const data_expression one = sort_nat::nat(1);
  const data_expression two = sort_nat::nat(2);
  const data_expression three = sort_nat::nat(3);

  const data_expression a = make_fset(nat, {one, two});
  const data_expression b = make_fset(nat, {three});

  // The elements a = {1, 2} and b = {3} are incomparable under the subset order.
  BOOST_CHECK(normalise(less(a, b)) != sort_bool::true_());
  BOOST_CHECK(normalise(less(b, a)) != sort_bool::true_());
  BOOST_CHECK(normalise(less_total(a, b)) != normalise(less_total(b, a)));

  const data_expression forward = normalise(make_fset(fset_nat, {a, b}));
  const data_expression backward = normalise(make_fset(fset_nat, {b, a}));
  BOOST_CHECK_MESSAGE(forward == backward, "not canonical: " << forward << " versus " << backward);

  // The result is a proper sorted list, that is fully reduced to @fset_cons and {}.
  BOOST_CHECK(sort_fset::is_cons_application(forward));

  // Both elements are present, and their union is idempotent.
  const data_expression pick_forward = normalise(sort_fset::union_(fset_nat, forward, backward));
  BOOST_CHECK_MESSAGE(pick_forward == forward, "the union of a set with itself changed it: " << pick_forward);
  BOOST_CHECK(normalise(sort_fset::in(fset_nat, a, forward)) == sort_bool::true_());
  BOOST_CHECK(normalise(sort_fset::in(fset_nat, b, forward)) == sort_bool::true_());
  BOOST_CHECK(normalise(sort_fset::in(fset_nat, make_fset(nat, {one}), forward)) == sort_bool::false_());
}

BOOST_AUTO_TEST_CASE(test_nested_fset_union_is_canonical)
{
  const sort_expression nat = sort_nat::nat();
  const sort_expression fset_nat = sort_fset::fset(nat);

  const data_expression a = make_fset(nat, {sort_nat::nat(1), sort_nat::nat(2)});
  const data_expression b = make_fset(nat, {sort_nat::nat(3)});

  const data_expression set_a = make_fset(fset_nat, {a});
  const data_expression set_b = make_fset(fset_nat, {b});

  const data_expression forward = normalise(sort_fset::union_(fset_nat, set_a, set_b));
  const data_expression backward = normalise(sort_fset::union_(fset_nat, set_b, set_a));

  BOOST_CHECK_MESSAGE(forward == backward, "not canonical: " << forward << " versus " << backward);

  const std::string text = to_string(forward);
  BOOST_CHECK_MESSAGE(text.find("@fset_insert") == std::string::npos, "got a stuck normal form: " << text);
  BOOST_CHECK_MESSAGE(text.find("union") == std::string::npos, "got a stuck normal form: " << text);

  // The cardinality is two, and it does not depend on the argument order.
  const data_expression card_forward = normalise(sort_fset::count(fset_nat, forward));
  const data_expression card_backward = normalise(sort_fset::count(fset_nat, backward));
  BOOST_CHECK_MESSAGE(card_forward == normalise(sort_nat::nat(2)), "unexpected cardinality: " << card_forward);
  BOOST_CHECK(card_forward == card_backward);

  BOOST_CHECK(normalise(sort_fset::in(fset_nat, a, forward)) == sort_bool::true_());
  BOOST_CHECK(normalise(sort_fset::in(fset_nat, b, forward)) == sort_bool::true_());
}

/// \brief The same non-total < gap exists for FBag, whose elements here are themselves FSet(Nat) values.
BOOST_AUTO_TEST_CASE(test_nested_fbag_union_is_canonical)
{
  const sort_expression nat = sort_nat::nat();
  const sort_expression fset_nat = sort_fset::fset(nat);

  const data_expression a = make_fset(nat, {sort_nat::nat(1), sort_nat::nat(2)});
  const data_expression b = make_fset(nat, {sort_nat::nat(3)});

  const data_expression bag_a = make_fbag(fset_nat, {a});
  const data_expression bag_b = make_fbag(fset_nat, {b});

  const data_expression forward = normalise(sort_fbag::union_(fset_nat, bag_a, bag_b));
  const data_expression backward = normalise(sort_fbag::union_(fset_nat, bag_b, bag_a));

  BOOST_CHECK_MESSAGE(forward == backward, "not canonical: " << forward << " versus " << backward);

  const std::string text = to_string(forward);
  BOOST_CHECK_MESSAGE(text.find("@fbag_insert") == std::string::npos, "got a stuck normal form: " << text);
  BOOST_CHECK_MESSAGE(text.find("union") == std::string::npos, "got a stuck normal form: " << text);

  const data_expression card_forward = normalise(sort_fbag::count_all(fset_nat, forward));
  const data_expression card_backward = normalise(sort_fbag::count_all(fset_nat, backward));
  BOOST_CHECK_MESSAGE(card_forward == normalise(sort_nat::nat(2)), "unexpected cardinality: " << card_forward);
  BOOST_CHECK(card_forward == card_backward);

  BOOST_CHECK(normalise(sort_fbag::count(fset_nat, a, forward)) == normalise(sort_nat::nat(1)));
  BOOST_CHECK(normalise(sort_fbag::count(fset_nat, b, forward)) == normalise(sort_nat::nat(1)));
  BOOST_CHECK(
    normalise(sort_fbag::count(fset_nat, make_fset(nat, {sort_nat::nat(1)}), forward)) == normalise(sort_nat::nat(0)));
}
