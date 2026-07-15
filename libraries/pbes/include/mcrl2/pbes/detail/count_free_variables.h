#ifndef MCRL2_PBES_DETAIL_COUNT_FREE_VARIABLES_H
#define MCRL2_PBES_DETAIL_COUNT_FREE_VARIABLES_H

#include "mcrl2/data/find.h"
#include "mcrl2/pbes/traverser.h"

namespace mcrl2::pbes_system::detail
{

struct count_free_variable_occurrences_traverser
  : public pbes_expression_traverser<count_free_variable_occurrences_traverser>
{
  using super = pbes_expression_traverser<count_free_variable_occurrences_traverser>;
  using super::apply;
  using super::enter;
  using super::leave;

  data::variable_list bound_variables;
  std::vector<data::variable_list> quantifier_stack;
  std::map<data::variable, std::size_t> result;
  bool search_propositional_variables;

  count_free_variable_occurrences_traverser(bool search_propositional_variables_ = true)
    : search_propositional_variables(search_propositional_variables_)
  {}

  count_free_variable_occurrences_traverser(const data::variable_list& bound_variables_,
    bool search_propositional_variables_ = true)
    : bound_variables(bound_variables_),
      search_propositional_variables(search_propositional_variables_)
  {}

  bool is_bound(const data::variable& v) const
  {
    if (std::find(bound_variables.begin(), bound_variables.end(), v) != bound_variables.end())
    {
      return true;
    }
    for (const data::variable_list& vars: quantifier_stack)
    {
      if (std::find(vars.begin(), vars.end(), v) != vars.end())
      {
        return true;
      }
    }
    return false;
  }

  void push(const data::variable_list& v) { quantifier_stack.push_back(v); }

  void pop() { quantifier_stack.pop_back(); }

  void enter(const forall& x) { push(x.variables()); }

  void leave(const forall&) { pop(); }

  void enter(const exists& x) { push(x.variables()); }

  void leave(const exists&) { pop(); }

  void enter(const propositional_variable& x)
  {
    if (search_propositional_variables)
    {
      for (const data::variable& v: data::find_free_variables(x.parameters()))
      {
        if (!is_bound(v))
        {
          result[v]++;
        }
      }
    }
  }

  void enter(const data::data_expression& x)
  {
    for (const data::variable& v: data::find_free_variables(x))
    {
      if (!is_bound(v))
      {
        result[v]++;
      }
    }
  }
};

inline std::map<data::variable, std::size_t> count_free_variable_occurrences(const pbes_expression& x,
  const data::variable_list& bound_variables = data::variable_list(),
  bool search_propositional_variables = true)
{
  count_free_variable_occurrences_traverser f(bound_variables, search_propositional_variables);
  f.apply(x);
  return f.result;
}

inline std::map<data::variable, std::size_t> count_free_variable_occurrences(const pbes_expression& x,
  bool search_propositional_variables = true)
{
  count_free_variable_occurrences_traverser f(data::variable_list(), search_propositional_variables);
  f.apply(x);
  return f.result;
}

} // namespace mcrl2::pbes_system::detail

#endif // MCRL2_PBES_DETAIL_COUNT_FREE_VARIABLES_H
