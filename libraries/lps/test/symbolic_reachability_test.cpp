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

#include "mcrl2/lps/find.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/lps/next_state_generator.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/linearise.h"

using namespace mcrl2;

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

    template <typename SummandVector>
    void gather_summands(const SummandVector& summands, const data::variable_list& parameter_list, const std::set<data::variable>& parameter_set, std::size_t& summand_index)
    {
      for (typename SummandVector::const_iterator i = summands.begin(); i != summands.end(); ++i)
      {
        std::set<data::variable> used_variables = lps::find_free_variables(*i);
        std::set<data::variable> used_parameters;
        std::set_intersection(used_variables.begin(), used_variables.end(), parameter_set.begin(), parameter_set.end(), std::inserter(used_parameters, used_parameters.begin()));

        std::size_t j_index = 0;
        for (data::variable_list::const_iterator j = parameter_list.begin(); j != parameter_list.end(); ++j)
        {
          if (used_parameters.find(*j) != used_parameters.end())
          {
            m_group_indices[summand_index].push_back(j_index);
          }
          j_index++;
        }
        summand_index++;
      }
    }

    void gather(lps::specification const& l)
    {
      lps::linear_process specification(l.process());

      std::size_t size = specification.action_summands().size() + specification.deadlock_summands().size();
      m_group_indices.resize(size);

      std::set<data::variable> parameter_set = data::find_all_variables(specification.process_parameters());
      std::size_t summand_index = 0;
      gather_summands(specification.action_summands(), specification.process_parameters(), parameter_set, summand_index);
      gather_summands(specification.deadlock_summands(), specification.process_parameters(), parameter_set, summand_index);
    }

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

    next_state_generator explorer(model, rewriter);

    std::stack< state >     stack;
    std::set< state >   known;

    stack.push(explorer.initial_state());
    known.insert(stack.top());

    while (!stack.empty())
    {
      state current(stack.top());
      stack.pop();

      for (size_t i = 0; i < model.process().summand_count(); ++i)
      {
        for(next_state_generator::iterator j = explorer.begin(current, i); j != explorer.end(); ++j)
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
