// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file symbolic_reachability_test.cpp
/// \brief Add your file description here.

#include <algorithm>
#include <iterator>
#include <iostream>
#include <memory>
#include <string>
#include <set>
#include <vector>
#include <stack>

#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/convert.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/lps/next_state_generator.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/linearise.h"


const std::string case_no_influenced_parameters(
  "act a;\n\n"
  "proc X(i: Nat) = a.X(i);\n\n"
  "init X(0);\n");

const std::string case_influenced_condition(
  "act a;\n\n"
  "proc X(i: Nat) = (i < 4) -> a.X(i) <> delta;\n\n"
  "init X(0);\n");

const std::string case_influenced_next(
  "act a;\n\n"
  "proc X(i: Nat, j : Nat) = a.X(j, i);\n\n"
  "init X(0, 0);\n");

const std::string case_influenced_action(
  "act a : Nat;\n\n"
  "proc X(i: Nat) = a(i + 1).X(i);\n\n"
  "init X(0);\n");

const std::string case_influenced_time(
  "act a;\n\n"
  "proc X(i: Nat) = a@(i + 1).X(i);\n\n"
  "init X(0);\n");

const std::string case_two_parameters(
  "act a : Nat;\n\n"
  "proc X(i: Nat, j : Nat) = (j < 4) -> a(i).X(i, j + 1) <> delta;\n\n"
  "init X(0, 1);\n");

const std::string case_summands(
  "act a: Nat;\n\n"
  "proc X(i: Nat, j : Nat) = a(j + 1).X(i, j) + a(i + 1).X(i, j);\n\n"
  "init X(0, 1);\n");

const std::string case_last(
  "act a: Nat;\n\n"
  "sort D = struct d1 | d2;\n"
  "proc X(id : Nat, n : Nat, dd:D) = sum d:D. a(id).X(id,(n + 1) mod 10, d);\n\n"
  "init allow({a},X(1,0,d1) || X(2,0,d1));\n");

class group_information
{

  private:

    mcrl2::lps::specification const&     m_model;

    std::vector< std::vector< size_t > > m_group_indices;

  private:

    void gather(mcrl2::lps::specification const& l);

  public:

    /**
     * \brief constructor from an mCRL2 lps
     **/
    group_information(mcrl2::lps::specification const& l) : m_model(l)
    {
      gather(l);
    }

    /**
     * \brief The number of groups (summands in the LPS)
     * \return lps::specification(l).summands().size()
     **/
    inline size_t number_of_groups() const
    {
      return m_group_indices.size();
    }

    inline size_t number_of_parameters() const
    {
      return m_model.process().process_parameters().size();
    }

    /**
     * \brief Indices of process parameters that influence event or next state of a summand
     * \param[in] index the selected summand
     * \returns reference to a vector of indices of parameters
     **/
    inline std::vector< size_t > const& operator[](size_t index) const
    {
      return m_group_indices[index];
    }
};

void group_information::gather(mcrl2::lps::specification const& l)
{
  using namespace mcrl2;

  using data::find_variables;
  using data::variable;

  struct local
  {
    static void add_used_variables(std::set< variable >& r, std::set< variable > const& c)
    {
      r.insert(c.begin(), c.end());
    }
  };

  lps::linear_process specification(l.process());

  // the set with process parameters
  std::set< variable > parameters = find_variables(specification.process_parameters());

  // the list of summands
  std::vector< lps::deprecated::summand > summands = atermpp::convert<std::vector<lps::deprecated::summand> >(lps::deprecated::linear_process_summands(specification));

  m_group_indices.resize(summands.size());

  for (std::vector< lps::deprecated::summand >::const_iterator i = summands.begin(); i != summands.end(); ++i)
  {
    std::set< variable > used_variables;

    local::add_used_variables(used_variables, find_variables(i->condition()));
    local::add_used_variables(used_variables, lps::find_variables(i->actions()));

    if (i->has_time())
    {
      local::add_used_variables(used_variables, find_variables(i->time()));
    }

    data::assignment_list assignments(i->assignments());

    for (data::assignment_list::const_iterator j = assignments.begin(); j != assignments.end(); ++j)
    {
      if (j->lhs() != j->rhs())
      {
        local::add_used_variables(used_variables, find_variables(j->lhs()));
        local::add_used_variables(used_variables, find_variables(j->rhs()));
      }
    }

    // process parameters used in condition or action of summand
    std::set< variable > used_parameters;

    std::set_intersection(used_variables.begin(), used_variables.end(),
                          parameters.begin(), parameters.end(), std::inserter(used_parameters, used_parameters.begin()));

    std::vector< variable > parameters_list(specification.process_parameters().begin(), specification.process_parameters().end());

    for (std::vector< variable >::const_iterator j = parameters_list.begin(); j != parameters_list.end(); ++j)
    {
      if (used_parameters.find(*j) != used_parameters.end())
      {
        m_group_indices[i - summands.begin()].push_back(j - parameters_list.begin());
      }
    }
  }
}

void check_info(mcrl2::lps::specification const& model)
{
  group_information info(model);

  BOOST_CHECK(info.number_of_groups() == model.process().summand_count());

#ifdef SHOW_INFO
  for (size_t i = 0; i < info.number_of_groups(); ++i)
  {
    std::vector< size_t > const& group_info(info[i]);

    std::cerr << "group " << i << " : {";

    if (!group_info.empty())
    {
      std::cerr << group_info[0];
    }

    for (std::vector< size_t >::const_iterator j = ++group_info.begin(); j < group_info.end(); ++j)
    {

      std::cerr << "," << *j;
    }

    std::cerr << "}" << std::endl;
  }
#endif
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv);

  using namespace mcrl2;
  using namespace mcrl2::lps;

  check_info(linearise(case_no_influenced_parameters));
  check_info(linearise(case_influenced_condition));
  check_info(linearise(case_influenced_action));
  check_info(linearise(case_influenced_time));
  check_info(linearise(case_influenced_next));
  check_info(linearise(case_two_parameters));
  check_info(linearise(case_summands));
  check_info(linearise(case_last));

  lps::specification model(linearise(case_no_influenced_parameters));

  if (1 < argc)
  {
    model.load(argv[1]);

    model.process().deadlock_summands().clear();

    data::rewriter        rewriter(model.data());
    next_state_generator::substitution_t dummy;

    next_state_generator explorer(model, rewriter);

    std::stack< state >     stack;
    atermpp::set< state >   known;

    stack.push(explorer.initial_state());
    known.insert(stack.top());

    while (!stack.empty())
    {
      state current(stack.top());
      stack.pop();

      for (size_t i = 0; i < model.process().summand_count(); ++i)
      {
        for(next_state_generator::iterator j = explorer.begin(current, &dummy, i); j != explorer.end(); ++j)
        {
          if (known.find(j->state()) == known.end())
          {
            std::cerr << pp(j->state()) << std::endl;
            known.insert(j->state());
            stack.push(j->state());
          }
        }
      }
    }
  }

  return 0;
}
