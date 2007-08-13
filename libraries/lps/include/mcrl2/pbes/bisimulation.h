#include "atermpp/make_list.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/pbes_translate_impl.h"
#include "mcrl2/data/utility.h" // fresh_identifier

namespace lps {

inline
pbes_expression step(const linear_process& m, const summand& m_summand, const linear_process& s, identifier_string X)
{
  namespace p = lps::pbes_expr;

  atermpp::vector<pbes_expression> terms;
  for (summand_list::iterator i = s.summands().begin(); i != s.summands().end(); ++i)
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
  for (summand_list::iterator i = m.summands().begin(); i != m.summands().end(); ++i)
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

inline
pbes strong_bisimulation(const specification& M, const specification& S)
{ 
  using atermpp::make_list;
  namespace p = lps::pbes_expr;

  const linear_process& m = M.process();
  const linear_process& s = S.process();

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
  return pbes(data, eqn, Xms_init);
}

} // namespace lps
