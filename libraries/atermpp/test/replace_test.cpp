// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file replace_test.cpp
/// \brief Add your file description here.

#include <vector>
#include <iostream>
#include <iterator>
#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"

using namespace std;
using namespace atermpp;

// function object to test if it is an aterm_appl with function symbol "f"
struct is_f
{
  bool operator()(const aterm_appl& t) const
  {
    return t.function().name() == "f";
  }
};

// function object to test if it is an aterm_appl with function symbol "g"
struct is_g
{
  bool operator()(const aterm_appl& t) const
  {
    return t.function().name() == "g";
  }
};

// function object to test if it is an aterm_appl with function symbol "z"
struct is_z
{
  bool operator()(const aterm_appl& t) const
  {
    return t.function().name() == "z";
  }
};

// function object to test if it is an aterm_appl with function symbol "a" or "b"
struct is_a_or_b
{
  bool operator()(const aterm_appl& t) const
  {
    return t.function().name() == "a" || t.function().name() == "b";
  }
};

// replaces function names f by g and vice versa
struct fg_replacer
{
  aterm_appl operator()(const aterm_appl& t) const
  {
    if (t.function().name() == "f")
    {
      return aterm_appl(function_symbol("g", t.function().arity()), t.begin(), t.end());
    }
    else if (t.function().name() == "g")
    {
      return aterm_appl(function_symbol("f", t.function().arity()), t.begin(), t.end());
    }
    else
    {
      return t;
    }
  }
};

// replaces function names f by g and vice versa, but stops the recursion once an f or g term is found
struct fg_partial_replacer
{
  std::pair< aterm_appl, bool> operator()(aterm_appl t) const
  {
    if (t.function().name() == "f")
    {
      return std::make_pair(aterm_appl(function_symbol("g", t.function().arity()), t.begin(), t.end()), false);
    }
    else if (t.function().name() == "g")
    {
      return std::make_pair(aterm_appl(function_symbol("f", t.function().arity()), t.begin(), t.end()), false);
    }
    else
    {
      return std::make_pair(t, true);
    }
  }
};

BOOST_AUTO_TEST_CASE(find_test)
{
  aterm_appl a(read_term_from_string("h(g(x),f(y),p(a(x,y),q(f(z))))"));

  aterm_appl t = find_if(a, is_f());
  BOOST_CHECK(t == read_term_from_string("f(y)"));

  aterm_appl a1(read_term_from_string("h(g(x),g(f(y)))"));
  t = partial_find_if(a1, is_f(), is_g());
  BOOST_CHECK(t == aterm_appl());
  t = partial_find_if(a1, is_f(), is_z());
  BOOST_CHECK(t == read_term_from_string("f(y)"));

  std::vector< aterm_appl> v;
  find_all_if(a, is_f(), back_inserter(v));
  BOOST_CHECK(v.front() == read_term_from_string("f(y)"));
  BOOST_CHECK(v.back() == read_term_from_string("f(z)"));
}

BOOST_AUTO_TEST_CASE(replace_test1)
{
  BOOST_CHECK(replace(aterm_appl(read_term_from_string("x")), atermpp::aterm_appl(read_term_from_string("x")), atermpp::aterm_appl(read_term_from_string("f(a)"))) == read_term_from_string("f(a)"));
  BOOST_CHECK(replace(aterm_appl(read_term_from_string("x")), atermpp::aterm_appl(read_term_from_string("x")), atermpp::aterm_appl(read_term_from_string("f(x)"))) == read_term_from_string("f(x)"));
  BOOST_CHECK(replace(atermpp::aterm_list(read_term_from_string("[x]")), atermpp::aterm_appl(read_term_from_string("x")), atermpp::aterm_appl(read_term_from_string("f(x)"))) == read_term_from_string("[f(x)]"));

  aterm_appl a(read_term_from_string("f(f(x))"));
  aterm_appl b(replace(a, atermpp::aterm_appl(read_term_from_string("f(x)")), atermpp::aterm_appl(read_term_from_string("x"))));
  BOOST_CHECK(b == read_term_from_string("f(x)"));
  b = bottom_up_replace(a, atermpp::aterm_appl(read_term_from_string("f(x)")), atermpp::aterm_appl(read_term_from_string("x")));
  BOOST_CHECK(b == read_term_from_string("x"));

  atermpp::aterm f = read_term_from_string("[]");
  atermpp::aterm g = replace(f, a, b);
  BOOST_CHECK(f == read_term_from_string("[]"));
  BOOST_CHECK(g == read_term_from_string("[]"));

  atermpp::aterm x = read_term_from_string("g(f(x),f(y),h(f(x)))");
  atermpp::aterm y = replace(x, fg_replacer());
  atermpp::aterm z = partial_replace(x, fg_partial_replacer());

  BOOST_CHECK(y == read_term_from_string("f(f(x),f(y),h(f(x)))"));
  BOOST_CHECK(z == read_term_from_string("f(f(x),f(y),h(f(x)))"));
}

inline
const atermpp::function_symbol& f2()
{
  static atermpp::function_symbol f = atermpp::function_symbol("f", 2);
  return f;
}

inline
const atermpp::function_symbol& f3()
{
  static atermpp::function_symbol f = atermpp::function_symbol("f", 3);
  return f;
}

struct replace_f
{
  atermpp::aterm_appl operator()(const atermpp::aterm_appl& x) const
  {
    if (x.function() == f3())
    {
      return atermpp::aterm_appl(f2(), x.begin(), --x.end());
    }
    return x;
  }
};

// DataVarId
inline
const atermpp::function_symbol& function_symbol_DataVarId()
{
  static atermpp::function_symbol function_symbol_DataVarId = atermpp::function_symbol("DataVarId", 3);
  return function_symbol_DataVarId;
}

// DataVarIdNoIndex
inline
const atermpp::function_symbol& function_symbol_DataVarIdNoIndex()
{
  static atermpp::function_symbol f = atermpp::function_symbol("DataVarIdNoIndex", 2);
  return f;
}

// OpId
inline
const atermpp::function_symbol& function_symbol_OpId()
{
  static atermpp::function_symbol function_symbol_OpId = atermpp::function_symbol("OpId", 3);
  return function_symbol_OpId;
}

// OpIdIndex
inline
const atermpp::function_symbol& function_symbol_OpIdNoIndex()
{
  static atermpp::function_symbol f = atermpp::function_symbol("OpIdNoIndex", 2);
  return f;
}

// PropVarInst
inline
const atermpp::function_symbol& function_symbol_PropVarInst()
{
  static atermpp::function_symbol function_symbol_PropVarInst = atermpp::function_symbol("PropVarInst", 3);
  return function_symbol_PropVarInst;
}

// PropVarInstNoIndex
inline
const atermpp::function_symbol& function_symbol_PropVarInstNoIndex()
{
  static atermpp::function_symbol f = atermpp::function_symbol("PropVarInstNoIndex", 2);
  return f;
}

struct index_remover
{
  atermpp::aterm_appl operator()(const atermpp::aterm_appl& x) const
  {
    if (x.function() == function_symbol_DataVarId())
    {
      return atermpp::aterm_appl(function_symbol_DataVarIdNoIndex(), x.begin(), --x.end());
    }
    else if (x.function() == function_symbol_OpId())
    {
      return atermpp::aterm_appl(function_symbol_OpIdNoIndex(), x.begin(), --x.end());
    }
    else if (x.function() == function_symbol_PropVarInst())
    {
      return atermpp::aterm_appl(function_symbol_PropVarInstNoIndex(), x.begin(), --x.end());
    }
    return x;
  }
};

BOOST_AUTO_TEST_CASE(replace_test2)
{
  atermpp::aterm t = atermpp::read_term_from_string("g(h(x,[f(y,p(q),1)]))");
  t = atermpp::replace(t, replace_f());
  BOOST_CHECK(t == atermpp::read_term_from_string("g(h(x,[f(y,p(q))]))"));
}

BOOST_AUTO_TEST_CASE(replace_test3)
{
  atermpp::aterm t  = atermpp::read_term_from_string("g(h(z(x,[f(y,p(q),1)],0)))");
  atermpp::aterm t1 = atermpp::replace(t, replace_f());
  atermpp::aterm t2 = atermpp::read_term_from_string("g(h(z(x,[f(y,p(q))],0)))");
  BOOST_CHECK(t1 == t2);
}

BOOST_AUTO_TEST_CASE(bottom_up_replace_test)
{
  atermpp::aterm t = atermpp::read_term_from_string("PBES(PBInit(PropVarInst(X,[OpId(@c0,SortId(Nat),131)],0)))");
  atermpp::aterm t1 = atermpp::replace(t, index_remover());
  atermpp::aterm t2 = atermpp::read_term_from_string("PBES(PBInit(PropVarInstNoIndex(X,[OpId(@c0,SortId(Nat),131)])))");
  atermpp::aterm t3 = atermpp::bottom_up_replace(t, index_remover());
  atermpp::aterm t4 = atermpp::read_term_from_string("PBES(PBInit(PropVarInstNoIndex(X,[OpIdNoIndex(@c0,SortId(Nat))])))");
  BOOST_CHECK(t1 == t2);
  BOOST_CHECK(t3 == t4);
}

BOOST_AUTO_TEST_CASE(cached_bottom_up_replace_test)
{
  std::unordered_map<aterm_appl, aterm> cache;
  atermpp::aterm t  = atermpp::read_term_from_string("h(g(f(x),f(x)),g(f(x),f(x)))");
  atermpp::aterm t1 = atermpp::bottom_up_replace(t, fg_replacer(), cache);
  atermpp::aterm t2 = atermpp::read_term_from_string("h(f(g(x),g(x)),f(g(x),g(x)))");
  BOOST_CHECK(t1 == t2);
  BOOST_CHECK(cache.size() == 4);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
