// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_SMT_PROBLEM_H
#define MCRL2_SMT_SMT_PROBLEM_H

#include <set>

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"

namespace mcrl2
{

namespace smt
{

class smt_problem
{
  protected:
    std::set<data::variable> m_variables;
    std::set<data::data_expression> m_assertions;
    std::set<data::data_expression_list> m_distinct_assertions;

  public:
    const std::set<data::variable>& variables() const
    {
      return m_variables;
    }

    const std::set<data::data_expression>& assertions() const
    {
      return m_assertions;
    }

    const std::set<data::data_expression_list>& distinct_assertions() const
    {
      return m_distinct_assertions;
    }

    void add_variable(const data::variable& variable)
    {
      m_variables.insert(variable);
    }

    void add_assertion(const data::data_expression& expression)
    {
      m_assertions.insert(expression);
    }

    void add_distinct_assertion(const data::data_expression_list& distinct_terms)
    {
      m_distinct_assertions.insert(distinct_terms);
    }

    template<typename Container>
    void add_distinct_assertion(Container distinct_terms)
    {
      m_distinct_assertions.insert(data::data_expression_list(distinct_terms.begin(), distinct_terms.end()));
    }
};

}
}

#endif
