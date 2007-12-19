#include <algorithm>
#include <iterator>
#include <set>
#include <vector>
#include <boost/iterator/transform_iterator.hpp>
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/lps/rename.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/algorithm.h"
#include "mcrl2/lps/detail/sorted_sequence_algorithm.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/pbes_translate_impl.h"

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

inline
pbes<> strong_bisimulation(const specification& M, const specification& S)
{ 
  using atermpp::make_list;
  namespace p = lps::pbes_expr;

  linear_process m = M.process();
  linear_process s = S.process();
    
  // Resolve name clashes between m and s
  std::set<identifier_string> used_names;
  used_names.insert(boost::make_transform_iterator(m.process_parameters().begin(), detail::data_variable_name()),
                    boost::make_transform_iterator(m.process_parameters().end()  , detail::data_variable_name())
                   );
  used_names.insert(boost::make_transform_iterator(m.free_variables().begin(), detail::data_variable_name()),
                    boost::make_transform_iterator(m.free_variables().end()  , detail::data_variable_name())
                   );
  for (summand_list::iterator i = m.summands().begin(); i != m.summands().end(); ++i)
  {
    used_names.insert(boost::make_transform_iterator(i->summation_variables().begin(), detail::data_variable_name()),
                      boost::make_transform_iterator(i->summation_variables().end()  , detail::data_variable_name())
                     );
  }
  s = rename_process_parameters(s, used_names, "_S");
  s = rename_free_variables(s, used_names, "_S");
  s = rename_summation_variables(s, used_names, "_S");

  identifier_string ms_name = fresh_identifier(make_list(M, S), "Xms");
  identifier_string sm_name = fresh_identifier(make_list(S, M), "Xsm");

  data_variable_list ms_parameters = m.process_parameters() + s.process_parameters();
  data_variable_list sm_parameters = s.process_parameters() + m.process_parameters();

  propositional_variable Xms(ms_name, ms_parameters);
  propositional_variable Xsm(sm_name, sm_parameters); 
  propositional_variable_instantiation Xms_init(ms_name, data_expression_list() + ms_parameters);

  pbes_expression ms_formula = p::and_(match(m, s, ms_name), match(s, m, sm_name));
  pbes_expression sm_formula = Xms_init;
  
  atermpp::vector<pbes_equation> eqn;
  eqn.push_back(pbes_equation(fixpoint_symbol::nu(), Xms, ms_formula));
  eqn.push_back(pbes_equation(fixpoint_symbol::nu(), Xsm, sm_formula));

  atermpp::set<data_variable> free_variables;
  std::copy(m.free_variables().begin(), m.free_variables().end(), std::inserter(free_variables, free_variables.begin()));
  std::copy(s.free_variables().begin(), s.free_variables().end(), std::inserter(free_variables, free_variables.begin()));

  data_specification data = M.data();
  propositional_variable_instantiation init(ms_name, M.initial_process().state() + S.initial_process().state());
  return pbes<>(data, eqn, free_variables, init);
}

} // namespace lps
