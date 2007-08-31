#include "atermpp/make_list.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/algorithm.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/pbes_translate_impl.h"
#include "mcrl2/data/utility.h"

namespace lps {

inline
pbes_expression step(const linear_process& m, const summand& m_summand, const linear_process& s, identifier_string X)
{
  namespace p = lps::pbes_expr;

  atermpp::vector<pbes_expression> terms;
  for (non_delta_summand_list::iterator i = s.non_delta_summands().begin(); i != s.non_delta_summands().end(); ++i)
  {
    pbes_expression term1 = p::val(i->condition());
    pbes_expression term2 = detail::equal_data_parameters(m_summand.actions(), i->actions());
    ATermList list =
      m.process_parameters().substitute(assignment_list_substitution(m_summand.assignments())) +
      s.process_parameters().substitute(assignment_list_substitution(i->assignments()));
    data_expression_list params(list);
    pbes_expression term3 = propositional_variable_instantiation(X, params);

    data_variable_list sumvars = i->summation_variables();
    pbes_expression expr = p::and_(term1, p::and_(term2, term3));

    pbes_expression term = p::exists(sumvars, expr);
    terms.push_back(term);
  }
  return p::multi_or(terms.begin(), terms.end());
}

inline
pbes_expression match(const linear_process& m, const linear_process& s, identifier_string X)
{
  namespace d = lps::data_expr;
  namespace p = lps::pbes_expr;

  atermpp::vector<pbes_expression> terms;
  for (non_delta_summand_list::iterator i = m.non_delta_summands().begin(); i != m.non_delta_summands().end(); ++i)
  {
    pbes_expression term1 = i->condition();
    pbes_expression term2 = step(m, *i, s, X);

    data_variable_list sumvars = i->summation_variables();
    pbes_expression expr = p::or_(p::val(d::not_(term1)), term2);
    pbes_expression term = p::forall(sumvars, expr);
    terms.push_back(term);
  }
  return p::multi_and(terms.begin(), terms.end());
}

/// \brief Function object that returns the name of a data variable
struct variable_name: public std::unary_function<data_variable, identifier_string>
{
  identifier_string operator()(const data_variable& v) const
  {
    return v.name();
  }
};

/// Rename all summation variables in lps that occur in forbidden_names.
inline
linear_process rename_summation_variables(const linear_process& lps, const std::set<identifier_string>& forbidden_names)
{
  atermpp::vector<summand> new_summands;

  for (summand_list::iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
  {
    std::set<data_variable> summand_variables = find_variables(*i);     
    std::vector<identifier_string> src;
    std::set_intersection(boost::make_transform_iterator(summand_variables.begin(), variable_name()),
                          boost::make_transform_iterator(summand_variables.end(), variable_name()),
                          forbidden_names.begin(),
                          forbidden_names.end(),
                          std::back_inserter(src)
                         );

    std::set<identifier_string> all;
    std::set_union(boost::make_transform_iterator(summand_variables.begin(), variable_name()),
                   boost::make_transform_iterator(summand_variables.end(), variable_name()),
                   forbidden_names.begin(),
                   forbidden_names.end(),
                   std::inserter(all, all.end())
                  );

    std::vector<std::string> dest;
    for (std::vector<identifier_string>::const_iterator j = src.begin(); j != src.end(); ++j)
    {
      dest.push_back(fresh_identifier(all, *j, postfix_identifier_creator("_S")));
    }

    new_summands.push_back(atermpp::partial_replace(*i, detail::make_data_variable_name_replacer(src, dest)));
  }
  
  return set_summands(lps, summand_list(new_summands.begin(), new_summands.end()));
}

inline
pbes strong_bisimulation(const specification& M, const specification& S)
{ 
  using atermpp::make_list;
  namespace p = lps::pbes_expr;
    
  // First we resolve name clashes between process parameters of m1 and s.
  linear_process m = detail::remove_parameter_clashes(M.process(), S.process());

  // Second we resolve name clashes between summation variables in s and
  // (process parameters + free variables + summation variables) of m.
  // We do this by renaming the summation variables of the summands in s.
  std::set<identifier_string> used_names;

  // add process parameters of m to used_names
  for (data_variable_list::iterator i = m.process_parameters().begin(); i != m.process_parameters().end(); ++i)
    used_names.insert(i->name());
  
  // add free variables of m to used_names
  for (data_variable_list::iterator i = m.free_variables().begin(); i != m.free_variables().end(); ++i)
    used_names.insert(i->name());
  
  // add summation variables of m to used_names
  for (summand_list::iterator i = m.summands().begin(); i != m.summands().end(); ++i)
    for (data_variable_list::iterator j = i->summation_variables().begin(); j != i->summation_variables().end(); ++j)
      used_names.insert(j->name());

  linear_process s = rename_summation_variables(S.process(), used_names);

  identifier_string ms_name = fresh_identifier(make_list(M, S), "Xms");
  identifier_string sm_name = fresh_identifier(make_list(S, M), "Xsm");

  data_variable_list ms_parameters = m.process_parameters() + s.process_parameters();
  data_variable_list sm_parameters = s.process_parameters() + m.process_parameters();

  propositional_variable Xms(ms_name, ms_parameters);
  propositional_variable Xsm(sm_name, sm_parameters); 
  propositional_variable_instantiation Xms_init(ms_name, data_expression_list() + ms_parameters);

  pbes_expression ms_formula = p::and_(match(m, s, ms_name), match(s, m, sm_name));
  pbes_expression sm_formula = Xms_init;
  
  lps::equation_system eqn;
  eqn.push_back(pbes_equation(fixpoint_symbol::nu(), Xms, ms_formula));
  eqn.push_back(pbes_equation(fixpoint_symbol::nu(), Xsm, sm_formula));
  
  data_specification data = M.data();
  propositional_variable_instantiation init(ms_name, M.initial_process().state() + S.initial_process().state());
  return pbes(data, eqn, init);
}

} // namespace lps
