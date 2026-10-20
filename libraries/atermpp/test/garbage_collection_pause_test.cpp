// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file garbage_collection_pause_test.cpp
/// \brief Tests for postponing automatic garbage collection with garbage_collection_pause.

#define BOOST_TEST_MODULE garbage_collection_pause_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/detail/global_aterm_pool.h"

using namespace atermpp;

/// Automatic garbage collection is considered every this many term creations, see aterm_pool::created_term.
constexpr std::size_t collection_check_interval = 10000;

/// Every garbage term that is created also creates an integer term.
constexpr std::size_t terms_per_garbage_term = 2;

/// The number of deleted garbage terms, counted by a deletion hook.
static std::size_t deleted_garbage_terms = 0;

/// The integer argument of the next garbage term, which makes every garbage term a new term.
static std::size_t next_garbage_argument = 0;

static void on_delete_garbage_term(const aterm&)
{
  ++deleted_garbage_terms;
}

static const function_symbol& garbage_symbol()
{
  static function_symbol symbol = function_symbol("garbage_collection_pause_test_garbage", 1);
  static bool hook_added = false;
  if (!hook_added)
  {
    add_deletion_hook(symbol, on_delete_garbage_term);
    hook_added = true;
  }
  return symbol;
}

/// \brief Collects all garbage and resets the deletion counter, such that every test starts from the same state.
static void start_test()
{
  garbage_symbol();
  detail::g_thread_term_pool().collect();
  deleted_garbage_terms = 0;
}

/// \brief Creates enough garbage terms to trigger an automatic garbage collection, unless it is paused.
/// \returns The number of garbage terms created.
static std::size_t create_garbage()
{
  // After a collection, the next automatic one takes place once about as many terms have been created as there are
  // terms in the pool and in the protection sets. Creating twice that number, plus some checks, is always enough.
  const std::size_t number_of_garbage_terms
    = (2 * detail::g_term_pool().size() + 2 * collection_check_interval) / terms_per_garbage_term;
  for (std::size_t i = 0; i < number_of_garbage_terms; ++i)
  {
    const aterm garbage(garbage_symbol(), aterm_int(next_garbage_argument++));
  }
  return number_of_garbage_terms;
}

BOOST_AUTO_TEST_CASE(collection_without_pause_test)
{
  start_test();
  create_garbage();
  BOOST_CHECK_GT(deleted_garbage_terms, 0U);
}

BOOST_AUTO_TEST_CASE(pause_test)
{
  start_test();
  {
    const garbage_collection_pause pause;
    create_garbage();
    BOOST_CHECK_EQUAL(deleted_garbage_terms, 0U);
  }
  create_garbage();
  BOOST_CHECK_GT(deleted_garbage_terms, 0U);
}

BOOST_AUTO_TEST_CASE(nested_pause_test)
{
  start_test();
  {
    const garbage_collection_pause outer_pause;
    {
      const garbage_collection_pause inner_pause;
      create_garbage();
    }
    create_garbage();
    BOOST_CHECK_EQUAL(deleted_garbage_terms, 0U);
  }
  create_garbage();
  BOOST_CHECK_GT(deleted_garbage_terms, 0U);
}

BOOST_AUTO_TEST_CASE(explicit_collection_during_pause_test)
{
  start_test();
  const garbage_collection_pause pause;
  const std::size_t number_of_garbage_terms = create_garbage();
  BOOST_CHECK_EQUAL(deleted_garbage_terms, 0U);
  detail::g_thread_term_pool().collect();
  BOOST_CHECK_EQUAL(deleted_garbage_terms, number_of_garbage_terms);
}
