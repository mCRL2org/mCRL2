// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/data/detail/rewrite/strategy_rule.h"
#include "mcrl2/data/detail/rewrite/jitty.h"
#include "mcrl2/data/detail/rewrite/jitty_jittyc.h"



namespace mcrl2::data::detail
{

class dependencies_rewrite_rule_pair
{
  protected:
    std::set<std::size_t> m_dependencies;
    data_equation m_equation;

  public:
    dependencies_rewrite_rule_pair(std::set<std::size_t>& dependencies, const data_equation& eq)
     : m_dependencies(dependencies), m_equation(eq)
    {}

    const std::set<std::size_t>& dependencies() const
    {
      return m_dependencies;
    }

    data_equation equation() const
    {
      return m_equation;
    }
};

// Create a strategy for the rewrite rules belonging to one particular symbol.
// It is a prerequisite for this function to that all rewrite rules in rules1 have
// the same main function symbol in the lhs. 
strategy RewriterJitty::create_a_rewriting_based_strategy(const function_symbol& f, const data_equation_list& rules1)
{
  static_cast<void>(f); // Avoid an unused variable warning. 
  data_equation_list rules=rules1;
  std::vector<strategy_rule> strat;

  std::vector <bool> used;

  std::size_t arity = 0;
  std::size_t max_number_of_variables = 0;
  while (!rules.empty())
  {
    data_equation_list l;
    std::vector<dependencies_rewrite_rule_pair> m;

    std::vector<int> args(arity,-1);

    for (const data_equation& this_rule: rules)
    {
      max_number_of_variables=std::max(this_rule.variables().size(),max_number_of_variables);
      const data_expression& this_rule_lhs = this_rule.lhs();
      if ((is_function_symbol(this_rule_lhs)?1:detail::recursive_number_of_args(this_rule_lhs)+1) == arity + 1)
      {
        const data_expression& cond = this_rule.condition();
        atermpp::term_list<variable_list> vars = { get_free_vars(cond) };

        std::vector < bool> bs(arity,false);

        for (std::size_t i = 0; i < arity; i++)
        {
          const data_expression this_rule_lhs_iplus1_arg=detail::get_argument_of_higher_order_term(atermpp::down_cast<application>(this_rule_lhs),i);
          if (!is_variable(this_rule_lhs_iplus1_arg))
          {
            bs[i] = true;
            const variable_list evars = get_free_vars(this_rule_lhs_iplus1_arg);
            for (const variable& evar : evars)
            {
              int j=0;
              const atermpp::term_list <variable_list>& next_vars=vars.tail();
              for (const atermpp::term_list<variable>& next_var : next_vars)
              {
                if (std::find(next_var.begin(), next_var.end(), evar) != next_var.end())
                {
                  bs[j] = true;
                }
                j++;
              }
            }
            vars=push_back(vars,get_free_vars(this_rule_lhs_iplus1_arg));
          }
          else
          {
            int j = -1;
            bool b = false;
            for (const atermpp::term_list<variable>& var : vars)
            {
              if (std::find(var.begin(), var.end(), variable(this_rule_lhs_iplus1_arg)) != var.end())
              {
                if (j >= 0)
                {
                  bs[j] = true;
                }
                b = true;
              }
              j++;
            }
            if (b)
            {
              bs[i] = true;
            }
            vars=push_back(vars,get_free_vars(this_rule_lhs_iplus1_arg));
          }
        }

        double_variable_traverser<data::variable_traverser> lhs_doubles;
        double_variable_traverser<data::variable_traverser> rhs_doubles;
        std::set<variable> condition_vars = find_free_variables(this_rule.condition());
        lhs_doubles.apply(this_rule.lhs());
        rhs_doubles.apply(this_rule.rhs());

        std::set<std::size_t> deps;
        for (std::size_t i = 0; i < arity; i++)
        {
          if (i>=used.size())
          {
            used.resize(i+1,false);
          }
          // Check whether argument i is a variable that occurs more than once in
          // the left or right hand side, or occurs in the condition. It is not clear whether it is
          // useful to check that it occurs in the condition, but this is what the jittyc rewriter also does.
          const data_expression& arg_i = get_argument_of_higher_order_term(atermpp::down_cast<application>(this_rule.lhs()), i);
          if ((bs[i] ||
               (is_variable(arg_i) && (lhs_doubles.result().count(atermpp::down_cast<variable>(arg_i)) > 0 ||
                                       condition_vars.count(atermpp::down_cast<variable>(arg_i)) > 0 ||
                                       rhs_doubles.result().count(atermpp::down_cast<variable>(arg_i)) > 0))
              ) && !used[i])
          {
            deps.insert(i);
            args[i] += 1;
          }
        }

        m.emplace_back(deps, this_rule);
      }
      else
      {
        l.push_front(this_rule);
      }
    }

    while (!m.empty())
    {
      std::vector<dependencies_rewrite_rule_pair> m2;
      for (const dependencies_rewrite_rule_pair& p: m)
      {
        if (p.dependencies().empty())
        {
          const data_equation rule = p.equation();
          strat.emplace_back(rule);
        }
        else
        {
          m2.push_back(p);
        }
      }
      m = m2;

      if (m.empty())
      {
        break;
      }

      int max = -1;
      std::size_t maxidx = 0;

      for (std::size_t i = 0; i < arity; i++)
      {
        assert(i<((std::size_t)1)<<(8*sizeof(int)-1));
        if (args[i] > max)
        {
          maxidx = i+1;
          max = args[i];
        }
      }

      if (maxidx > 0)
      {
        args[maxidx-1] = -1;
        if (maxidx>used.size())
        {
          used.resize(maxidx,false);
        }
        used[maxidx-1] = true;

        const std::size_t k(maxidx-1);
        strat.emplace_back(k);
        m2.clear();
        for (const dependencies_rewrite_rule_pair& p: m)
        {
          const data_equation eq=p.equation();
          std::set<std::size_t> dependencies=p.dependencies();
          dependencies.erase(k);
          m2.emplace_back(dependencies, eq);
        }
        m = m2;
      }
    }

    rules = reverse(l);
    arity++;
  }
  return strategy(max_number_of_variables,strat);
}

// Create an explicit rewrite strategy when rewriting using an explicitly given 
// C++ function. First rewrite all the arguments, then apply the function. 
strategy RewriterJitty::create_a_cpp_function_based_strategy(const function_symbol& f, const data_specification& data_spec)
{
  size_t number_of_arguments=0;
  if (is_function_sort(f.sort()))
  {
    number_of_arguments=atermpp::down_cast<function_sort>(f.sort()).domain().size();
  }
  // Indicate that all arguments must be rewritten first. 
  std::vector<strategy_rule> result;
  for(size_t i=0; i<number_of_arguments; ++i)
  {
    result.emplace_back(i);
  }
  result.emplace_back(data_spec.cpp_implemented_functions().find(f)->second.first);

  return strategy(0,result);
}

// Create a strategy to rewrite terms. This can either be a strategy that is based on rewrite
// rules or it can be a strategy based on an explicitly given c++ function for this function symbol. 
strategy RewriterJitty::create_strategy(const function_symbol& f, const data_equation_list& rules1, const data_specification& data_spec)
{
  if (data_spec.cpp_implemented_functions().count(f)==0)    // There is no explicit implementation.
  {
    return create_a_rewriting_based_strategy(f, rules1);
  } 
  else 
  {
    assert(rules1.size()==0);  // There should be no explicit rewrite rules, as this function is implemented by 
                               // an explicit C++ function. 
    return create_a_cpp_function_based_strategy(f, data_spec);
  }
}

} // namespace mcrl2::data::detail


