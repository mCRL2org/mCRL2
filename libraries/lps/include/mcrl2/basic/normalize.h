// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/basic/normalize_old.h
/// \brief Add your file description here.

#ifndef MCRL2_BASIC_NORMALIZE_H
#define MCRL2_BASIC_NORMALIZE_H

#include "mcrl2/basic/state_formula_builder.h"

namespace lps {

/// \internal
/// Puts a logical not around state variables named X
struct state_variable_negation
{
  identifier_string X;
  
  state_variable_negation(identifier_string X_)
    : X(X_)
  {}
  
  aterm_appl operator()(aterm_appl t) const
  {
    using namespace lps::state_frm;

    if (is_var(t) && (var_name(t) == X))
    {
      return not_(t);
    }
    else
    {
      return t;
    }
  }
};

/// Visitor that normalizes a state formula.
struct state_formula_normalize_builder: public state_formula_builder
{
  bool inside_not;
  
  state_formula_normalize_builder()
    : inside_not(false)
  {}

  state_formula visit_data_expression(const state_formula& f, const data_expression& d)
  {
    using namespace lps::state_frm;
    return inside_not ? data_expr::not_(d)
                      : d;
  }

  state_formula visit_true(const state_formula& /* f */)
  {
    using namespace lps::state_frm;
    return inside_not ? data_expr::false_()
                      : data_expr::true_();
  }

  state_formula visit_false(const state_formula& /* f */)
  {
    using namespace lps::state_frm;
    return inside_not ? data_expr::true_()
                      : data_expr::false_();
  }

  state_formula visit_not(const state_formula& /* f */, const state_formula& arg )
  {
    using namespace lps::state_frm;
    inside_not = !inside_not;
    state_formula result = visit(arg);
    inside_not = !inside_not;
    return result;
  }

  state_formula visit_and(const state_formula& /* f */, const state_formula& left, const state_formula& right)
  {
    using namespace lps::state_frm;
    bool b = inside_not;
    inside_not = false;
    return b ? or_(visit(not_(left)), visit(not_(right)))
             : and_(visit(left), visit(right));
  }

  state_formula visit_or(const state_formula& /* f */, const state_formula& left, const state_formula& right)
  {
    using namespace lps::state_frm;
    bool b = inside_not;
    inside_not = false;
    return b ? and_(visit(not_(left)), visit(not_(right)))
             : or_(visit(left), visit(right));
  }    

  state_formula visit_imp(const state_formula& /* f */, const state_formula& left, const state_formula& right)
  {
    using namespace lps::state_frm;
    bool b = inside_not;
    inside_not = false;
    return b ? and_(visit(left), visit(not_(right)))
             : or_(visit(left), visit(not_(right)));
  }    

  state_formula visit_forall(const state_formula& /* f */, const data_variable_list& variables, const state_formula& expression)
  {
    using namespace lps::state_frm;
    bool b = inside_not;
    inside_not = false;
    return b ? exists(variables, visit(not_(expression)))
             : forall(variables, visit(expression));
  }

  state_formula visit_exists(const state_formula& /* f */, const data_variable_list& variables, const state_formula& expression)
  {
    using namespace lps::state_frm;
    bool b = inside_not;
    inside_not = false;
    return b ? forall(variables, visit(not_(expression)))
             : exists(variables, visit(expression));
  }

  state_formula visit_must(const state_formula& /* f */, const regular_formula& r, const state_formula& formula)
  {
    using namespace lps::state_frm;
    bool b = inside_not;
    inside_not = false;
    return b ? must(r, visit(not_(formula)))
             : must(r, visit(formula));
  }

  state_formula visit_may(const state_formula& /* f */, const regular_formula& r, const state_formula& formula)
  {
    using namespace lps::state_frm;
    bool b = inside_not;
    inside_not = false;
    return b ? may(r, visit(not_(formula)))
             : may(r, visit(formula));
  }

  state_formula visit_yaled(const state_formula& /* f */)
  {
    using namespace lps::state_frm;
    return inside_not ? delay()
                      : yaled();
  }

  state_formula visit_yaled_timed(const state_formula& /* f */, const data_expression& t)
  {
    using namespace lps::state_frm;
    return inside_not ? delay_timed(t)
                      : yaled_timed(t);   
  }

  state_formula visit_delay(const state_formula& /* f */)
  {
    using namespace lps::state_frm;
    return inside_not ? yaled()
                      : delay();
  }

  state_formula visit_delay_timed(const state_formula& /* f */, const data_expression& t)
  {
    using namespace lps::state_frm;
    return inside_not ? yaled_timed(t)
                      : delay_timed(t);   
  }

  state_formula visit_var(const state_formula& f, const identifier_string& /* n */, const data_expression_list& /* l */)
  {
    using namespace lps::state_frm;
    if (inside_not)
    {
      throw std::runtime_error(std::string("normalize error: illegal expression ") + f.to_string());
    }
    return f;
  }

  state_formula visit_mu(const state_formula& /* f */, const identifier_string& name, const data_assignment_list& a, const state_formula& formula)
  {
    using namespace lps::state_frm;
    bool b = inside_not;
    inside_not = false;
    return b ? nu(name, a, visit(not_(formula.substitute(state_variable_negation(name)))))
             : mu(name, a, visit(formula));   
  }

  state_formula visit_nu(const state_formula& /* f */, const identifier_string& name, const data_assignment_list& a, const state_formula& formula)
  {
    using namespace lps::state_frm;
    bool b = inside_not;
    inside_not = false;
    return b ? mu(name, a, visit(not_(formula.substitute(state_variable_negation(name)))))
             : nu(name, a, visit(formula));   
  }
};

/// \internal
/// The function normalize_old brings a state formula into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
inline
state_formula normalize_old(state_formula f)
{
  using namespace lps::state_frm;

  if (is_not(f))
  {
    f = not_arg(f); // remove the not
    if (is_data(f)) {
      return data_expr::not_(f);
    } else if (is_true(f)) {
      return false_();
    } else if (is_false(f)) {
      return true_();
    } else if (is_not(f)) {
      return normalize_old(not_arg(f));
    } else if (is_and(f)) {
      return or_(normalize_old(not_(lhs(f))), normalize_old(not_(rhs(f))));
    } else if (is_or(f)) {
      return and_(normalize_old(not_(lhs(f))), normalize_old(not_(rhs(f))));
    } else if (is_imp(f)) {
      return and_(normalize_old(lhs(f)), normalize_old(not_(rhs(f))));
    } else if (is_forall(f)) {
      return exists(quant_vars(f), normalize_old(not_(quant_form(f))));
    } else if (is_exists(f)) {
      return forall(quant_vars(f), normalize_old(not_(quant_form(f))));
    } else if (is_must(f)) {
      return may(mod_act(f), normalize_old(not_(mod_form(f))));
    } else if (is_may(f)) {
      return must(mod_act(f), normalize_old(not_(mod_form(f))));
    } else if (is_yaled_timed(f)) {
      return delay_timed(time(f));
    } else if (is_yaled(f)) {
      return delay();
    } else if (is_delay_timed(f)) {
      return yaled_timed(time(f));
    } else if (is_delay(f)) {
      return yaled();
    } else if (is_var(f)) {
      throw std::runtime_error(std::string("normalize_old error: illegal argument ") + f.to_string());
    } else if (is_mu(f)) {
      return nu(mu_name(f), mu_params(f), mu_form(normalize_old(not_(f.substitute(state_variable_negation(mu_name(f)))))));
    } else if (is_nu(f)) {
      return mu(mu_name(f), mu_params(f), mu_form(normalize_old(not_(f.substitute(state_variable_negation(mu_name(f)))))));
    }
  }
  else // !is_not(f)
  {
    if (is_data(f)) {
      return f;
    } else if (is_true(f)) {
      return f;
    } else if (is_false(f)) {
      return f;
    //} else if (is_not(f)) {
    // ;
    } else if (is_and(f)) {
      return and_(normalize_old(lhs(f)), normalize_old(rhs(f)));
    } else if (is_or(f)) {
      return or_(normalize_old(lhs(f)), normalize_old(rhs(f)));
    } else if (is_imp(f)) {
      return or_(normalize_old(not_(lhs(f))), normalize_old(rhs(f)));
    } else if (is_forall(f)) {
      return forall(quant_vars(f), normalize_old(quant_form(f)));
    } else if (is_exists(f)) {
      return exists(quant_vars(f), normalize_old(quant_form(f)));
    } else if (is_must(f)) {
      return must(mod_act(f), normalize_old(mod_form(f)));
    } else if (is_may(f)) {
      return may(mod_act(f), normalize_old(mod_form(f)));
    } else if (is_yaled_timed(f)) {
      return f;
    } else if (is_yaled(f)) {
      return f;
    } else if (is_delay_timed(f)) {
      return f;
    } else if (is_delay(f)) {
      return f;
    } else if (is_var(f)) {
      return f;
    } else if (is_mu(f)) {
      return mu(mu_name(f), mu_params(f), normalize_old(mu_form(f)));
    } else if (is_nu(f)) {
      return nu(mu_name(f), mu_params(f), normalize_old(mu_form(f)));
    }
  }
  throw std::runtime_error(std::string("normalize_old error: unknown argument ") + f.to_string());
  return state_formula();
}

/// The function normalize brings a state formula into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
inline
state_formula normalize(state_formula f)
{
  return normalize_old(f);
}

} // namespace lps

#endif // MCRL2_BASIC_NORMALIZE_H
