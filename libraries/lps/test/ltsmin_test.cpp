// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsmin_test.cpp
/// \brief Test for the pins class that connects mCRL2 with LTSMin (http://fmt.cs.utwente.nl/tools/ltsmin/).

#include <iostream>
#include <cstdio>

#include <boost/test/minimal.hpp>

#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/ltsmin.h"
#include "mcrl2/lps/detail/test_input.h"

using namespace mcrl2;

typedef int* state_vector;

inline
std::string print_state(const state_vector& s, std::size_t size, const std::string& msg = "state: ")
{
  int* first = s;
  int* last = s + size;

  std::ostringstream out;
  out << msg << "[";
  for (int* i = first; i != last; ++i)
  {
    if (i != first)
    {
      out << ", ";
    }
    out << *i;
  }
  out << "]";
  return out.str();
}

struct state_callback_function
{
  std::size_t state_size;
  std::size_t state_count;

  state_callback_function(std::size_t size)
    : state_size(size),
      state_count(0)
  {}

  void operator()(state_vector const& next_state, int* const& labels, int group=-1)
  {
    state_count++;
    std::cout << print_state(next_state, state_size, "visit state: ") << " edge label = " << labels[0] << " group = " << group << std::endl;
  }
};

static void test_data_type(lps::pins_data_type& type)
{
  std::cout << "test_data_type(" << type.name() << ")" << std::endl;

  // check serialize/deserialize
  for (std::size_t i = 0; i < type.size(); ++i)
  {
    std::string s = type.serialize(i);
    std::cout << "serialize(" << i << ") = " << type.serialize(i) << std::endl;
    std::size_t j = type.deserialize(s);
    BOOST_CHECK(i == j);
  }

  // check print/parse
  for (std::size_t i = 0; i < type.size(); ++i)
  {
    std::string s = type.print(i);
    std::cout << "print(" <<  i << ") = " << s << std::endl;
    std::size_t j = type.parse(s);
    std::cout << "parse(" <<  s << ") = " << j << std::endl;
  }

  // test iterator
  std::size_t index = 0;
  for (auto i = type.index_begin(); i != type.index_end(); ++i)
  {
    std::cout << "iterator " << index++ << " -> " << *i << std::endl;
    BOOST_CHECK(*i == index);
  }
}

static void test_ltsmin(const std::string& rewriter_strategy)
{
  // create an input file
  lps::specification spec=remove_stochastic_operators(lps::linearise(lps::detail::ABP_SPECIFICATION()));
  std::string abp_filename = "temporary_abp.lps";
  save_lps(spec, abp_filename);

  lps::pins p(abp_filename, rewriter_strategy);
  std::size_t N = p.process_parameter_count();

  BOOST_CHECK(p.process_parameter_count() == 11);

  std::size_t G = p.guard_count();

  BOOST_CHECK(G == 13);
  BOOST_CHECK(p.guard_parameters(0).size() == 1);
  BOOST_CHECK(p.guard_info(5).size() == 2);
  BOOST_CHECK(p.guard_name(0) == "s1_S == 2" || p.guard_name(0) == "s1_S == 1");  // The naming of states depends on the ordering of aterms and is now nondeterministic.

  BOOST_CHECK(p.edge_label_count() == 1);
  for (std::size_t i = 0; i < p.edge_label_count(); i++)
  {
    BOOST_CHECK(p.edge_label_type(i) == p.datatype_count() - 1);
  }

  std::set<data::sort_expression> params;
  for (data::variable_list::const_iterator i = spec.process().process_parameters().begin(); i != spec.process().process_parameters().end(); ++i)
  {
    params.insert(i->sort());
  }
  BOOST_CHECK(p.datatype_count() == params.size() + 1);
  BOOST_CHECK(p.group_count() == 10);

  std::size_t index = 0;
  for (data::variable_list::const_iterator i = spec.process().process_parameters().begin(); i != spec.process().process_parameters().end(); ++i)
  {
  	const lps::pins_data_type& type = p.data_type(p.process_parameter_type(index++));
  	BOOST_CHECK(type.name() == data::pp(i->sort()));
  }

  // get the initial state
  state_vector initial_state = new int[N];
  state_vector dest_state = new int[N];
  int* labels = new int[p.edge_label_count()];
  p.get_initial_state(initial_state);
  std::cout << print_state(initial_state, N, "initial state: ") << std::endl;

  // find all successors of the initial state
  state_callback_function f_all(p.process_parameter_count());
  p.next_state_all(initial_state, f_all, dest_state, labels);
  std::cout << "f_all.state_count = " << f_all.state_count << std::endl;

  // find successors of the initial state for individual groups
  std::size_t count = 0;
  for (std::size_t group = 0; group < p.group_count(); group++)
  {
    state_callback_function f_long(p.process_parameter_count());
    p.next_state_long(initial_state, group, f_long, dest_state, labels);
    std::cout << "group " << group << " count = " << f_long.state_count << std::endl;
    count += f_long.state_count;
  }
  BOOST_CHECK(count == f_all.state_count);

  // find successors (without evaluating condition) of the initial state for individual groups
  count = 0;
  for (std::size_t group = 0; group < p.group_count(); group++)
  {

    std::size_t all_true = p.GUARD_TRUE;
    for(std::size_t guard = 0; guard < p.guard_info(group).size() && all_true == p.GUARD_TRUE; guard++) {
      all_true = p.eval_guard_long(initial_state, p.guard_info(group)[guard]);
    }

    if (all_true == p.GUARD_TRUE) {
      state_callback_function f_long(p.process_parameter_count());
      p.update_long(initial_state, group, f_long, dest_state, labels);
      std::cout << "group " << group << " count = " << f_long.state_count << std::endl;
      count += f_long.state_count;
    }
  }
  BOOST_CHECK(count == f_all.state_count);

  lps::pins_data_type& action_label_type = p.data_type(p.datatype_count() - 1);
  test_data_type(action_label_type);

  // cleanup temporary files
  std::remove(abp_filename.c_str());

  delete[] initial_state;
  delete[] dest_state;
  delete[] labels;

  std::cout << p.info() << std::endl;

  // test summand_action_names()
  std::cout << "--- summand action names ---" << std::endl;
  for (std::size_t i = 0; i < p.group_count(); i++)
  {
    std::cout << i << " {";
    std::set<std::string> s = p.summand_action_names(i);
    for (std::set<std::string>::iterator j = s.begin(); j != s.end(); ++j)
    {
      if (j != s.begin())
      {
        std::cout << ", ";
      }
      std::cout << *j;
    }
    std::cout << "}" << std::endl;
  }

#ifdef MCRL2_FORCE_LTSMIN_TEST_FAILURE
  BOOST_CHECK(false);
#endif
}

template <typename Iter>
std::string print_vector(Iter first, Iter last)
{
  std::ostringstream out;
  out << "[";
  for (Iter i = first; i != last; ++i)
  {
    if (i != first)
    {
      out << ", ";
    }
    out << *i;
  }
  out << "]";
  return out.str();
}

template <typename Container>
std::string print_vector(const Container& c)
{
  return print_vector(c.begin(), c.end());
}

inline
std::string read_groups(const lps::pins& p)
{
  std::ostringstream out;
  for (std::size_t i = 0; i < p.group_count(); i++)
  {
    out << print_vector(p.read_group(i)) << std::endl;
  }
  return out.str();
}

inline
std::string write_groups(const lps::pins& p)
{
  std::ostringstream out;
  for (std::size_t i = 0; i < p.group_count(); i++)
  {
    out << print_vector(p.write_group(i)) << std::endl;
  }
  return out.str();
}

inline
std::string update_groups(const lps::pins& p)
{
  std::ostringstream out;
  for (std::size_t i = 0; i < p.group_count(); i++)
  {
    out << print_vector(p.update_group(i)) << std::endl;
  }
  return out.str();
}

inline
void test_dependency_matrix()
{
  std::string ONEBIT =
    "proc P(b: Bool) =          \n"
    "       tau . P(b = false); \n"
    "                           \n"
    "init P(true);              \n"
    ;

  lps::specification spec = lps::parse_linear_process_specification(ONEBIT);
  std::string filename = "temporary_onebit.lps";
  save_lps(spec, filename);
  lps::pins p(filename, "jitty");

  for (std::size_t i = 0; i < p.group_count(); i++)
  {
    std::cout << "\n";
    std::cout << "  read_group(" << i << ") = " << print_vector(p.read_group(i)) << std::endl;
    std::cout << " write_group(" << i << ") = " << print_vector(p.write_group(i)) << std::endl;
    std::cout << "update_group(" << i << ") = " << print_vector(p.update_group(i)) << std::endl;
  }
  BOOST_CHECK(print_vector(p.read_group(0)) == "[]");
  BOOST_CHECK(print_vector(p.write_group(0)) == "[0]");
  BOOST_CHECK(print_vector(p.update_group(0)) == "[]");

  // cleanup temporary files
  std::remove(filename.c_str());
}

inline
void test_serialisation()
{
  std::string EXAMPLE  =
    "sort Piece = struct Red | Yellow | None;   \n"
    "                                           \n"
    "proc P(i: Pos, j: Nat, p: Piece) =         \n"
    "       tau . P(i = 1, j = 0, p = Red);     \n"
    "                                           \n"
    "init P(1, 1, None);                        \n"
    ;

  lps::specification spec = lps::parse_linear_process_specification(EXAMPLE);
  std::string filename = "temporary_onebit.lps";
  save_lps(spec, filename);
  lps::pins p1(filename, "jitty");
  lps::pins p2(filename, "jitty");
  std::cout << "datatypes: " << p1.datatype_count() << std::endl;
  for(size_t i=0; i < p1.datatype_count(); i++)
  {
    std::cout << i << ": " << p1.data_type(i).name() << std::endl;
  }
  std::vector<std::pair<std::string, size_t>> expressions;
  expressions.push_back(std::make_pair("1", 0));
  expressions.push_back(std::make_pair("0", 1));
  expressions.push_back(std::make_pair("1", 1));
  expressions.push_back(std::make_pair("Red", 2));
  expressions.push_back(std::make_pair("Yellow", 2));

  for(auto ei = expressions.begin(); ei != expressions.end(); ++ei)
  {
    auto e = *ei;
    std::string s = e.first;
    size_t type = e.second;

    data::data_expression d1 = data::parse_data_expression(s, spec.data());
    std::string serialised = p1.data_type(type).serialize(p1.data_type(type)[d1]);
    data::data_expression d2(p2.data_type(type).get(p2.data_type(type).deserialize(serialised)));
    std::cout << "string:     " << s << std::endl;
    std::cout << "expression: " << d1 << std::endl;
    std::cout << "serialised: " << serialised << std::endl;
    std::cout << "result:     " << d2 << std::endl;
    BOOST_CHECK(d1==d2);
  }
}

int test_main(int, char**)
{
  log::mcrl2_logger::set_reporting_level(log::debug);

  test_dependency_matrix();
  test_serialisation();
  test_ltsmin("jitty");
#ifdef MCRL2_JITTYC_AVAILABLE
  test_ltsmin("jittyc");
#endif // MCRL2_JITTYC_AVAILABLE

  return 0;
}
