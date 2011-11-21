// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file normalize_sorts_test.cpp
/// \brief Test for normalizing sorts.

#include <algorithm>
#include <iterator>
#include <set>
#include <vector>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/normalize_sorts.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::data;

inline
data::function_symbol parse_function_symbol(std::string text, const std::string& dataspec_text)
{
  const std::string prefix = "UNIQUE_FUNCTION_SYMBOL_PREFIX";
  boost::algorithm::trim(text);
  std::string::size_type pos = text.find_first_of(':');
  std::string name = boost::algorithm::trim_copy(text.substr(0, pos));
  std::string type = prefix + text.substr(pos);
  std::string spec_text = dataspec_text + "\nmap " + prefix + type + ";\n";
  data::data_specification dataspec = data::parse_data_specification(spec_text);
  data::function_symbol f = dataspec.user_defined_mappings().back();
  data::function_symbol result = data::function_symbol(name, f.sort());
  return result;
}

void test_normalize_sorts()
{
  std::string DATASPEC =
    "sort Bit = struct e0 | e1;      \n"
    "     AbsBit = struct arbitrary; \n"
    "                                \n"
    "map  inv: Bit -> Bit;           \n"
    "     h: Bit -> AbsBit;          \n"
    "                                \n"
    "eqn  inv(e0)  =  e1;            \n"
    "     inv(e1)  =  e0;            \n"
    ;

  data_specification dataspec = parse_data_specification(DATASPEC);

  function_symbol f;
  f = parse_function_symbol("abseq : AbsBit # AbsBit -> Set(Bool)", DATASPEC);
  dataspec.add_mapping(f);
  f = parse_function_symbol("absinv : AbsBit -> Set(AbsBit)", DATASPEC);
  dataspec.add_mapping(f);

  data_equation_vector equations = dataspec.user_defined_equations();
  data::normalize_sorts(equations, dataspec);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_normalize_sorts();

  return 0;
}
