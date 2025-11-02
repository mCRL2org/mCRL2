// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#define NAME std::string("rewr_jitty")

#include "mcrl2/data/detail/rewrite/jitty.h"
#include "mcrl2/data/detail/rewrite/jitty_jittyc.h"

#include <boost/config.hpp>

#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/data/replace.h"

#ifdef MCRL2_DISPLAY_REWRITE_STATISTICS
#include "mcrl2/data/detail/rewrite_statistics.h"
#endif

using namespace mcrl2::log;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;



namespace mcrl2::data::detail
{

// The function below is intended to remove the auxiliary function this_term_is_in_normal_form from a term
// such that it can for instance be pretty printed.

data_expression RewriterJitty::remove_normal_form_function(const data_expression& t)
{
  if (is_machine_number(t))
  {
    return t;
  }

  if (is_variable(t))
  {
    return t;
  }

  if (is_function_symbol(t))
  {
    assert(t!=this_term_is_in_normal_form());
    return t;
  }

  if (is_application(t))
  {
    const application& ta=atermpp::down_cast<application>(t);
    if (ta.head()==this_term_is_in_normal_form())
    {
      assert(ta.size()==1);
      return ta[0];
    }

    return application(ta.head(), ta.begin(), ta.end(), [&](const data_expression& t){ return remove_normal_form_function(t); });
  }


  if (is_where_clause(t))
  {
    const where_clause& t1=atermpp::down_cast<where_clause>(t);
    const assignment_expression_list& assignments=t1.declarations();
    const data_expression& body=t1.body();

    assignment_vector new_assignments;
    for(const assignment_expression& ae: assignments)
    {
      const assignment& assignment_expr = atermpp::down_cast<assignment>(ae);
      new_assignments.emplace_back(assignment_expr.lhs(), remove_normal_form_function(assignment_expr.rhs()));
    }
    return where_clause(remove_normal_form_function(body),assignment_list(new_assignments.begin(),new_assignments.end()));
  }

  assert(is_abstraction(t));

  const abstraction& t1=atermpp::down_cast<abstraction>(t);
  const binder_type& binder=t1.binding_operator();
  const variable_list& bound_variables=t1.variables();
  const data_expression& body=t1.body();

  return abstraction(binder, bound_variables, remove_normal_form_function(body));
}

class jitty_argument_rewriter
{
  protected:
    mutable_indexed_substitution<>& m_sigma;
    RewriterJitty& m_r;
  public:
    jitty_argument_rewriter(mutable_indexed_substitution<>& sigma, RewriterJitty& r)
     : m_sigma(sigma), m_r(r)
    {}

  void operator()(data_expression& result, const data_expression& t)
  {
    m_r.rewrite_aux(result, t, m_sigma);
  }
};

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





void RewriterJitty::make_jitty_strat_sufficiently_larger(const std::size_t i)
{
  if (i>=jitty_strat.size())
  {
    jitty_strat.resize(i+1);
  }
}

void RewriterJitty::rebuild_strategy(const data_specification& data_spec, const mcrl2::data::used_data_equation_selector& equation_selector)
{
  jitty_strat.clear();
  function_symbol_vector function_symbols=data_spec.constructors();
  function_symbols.insert(function_symbols.end(), data_spec.mappings().begin(), data_spec.mappings().end());
  for(const function_symbol& f: function_symbols)
  {
    if (equation_selector(f))
    {
      const std::size_t i=atermpp::detail::index_traits<data::function_symbol, function_symbol_key_type, 2>::index(f);
      make_jitty_strat_sufficiently_larger(i);
      std::map< function_symbol, data_equation_list >::const_iterator j=jitty_eqns.find(f);
      jitty_strat[i] =
            (j==jitty_eqns.end()
                 ?create_strategy(f,data_equation_list(), data_spec)
                 :create_strategy(f,reverse(j->second), data_spec));
    }
  }

}


RewriterJitty::RewriterJitty(
           const data_specification& data_spec,
           const mcrl2::data::used_data_equation_selector& equation_selector):
        Rewriter(data_spec,equation_selector),
        this_term_is_in_normal_form_symbol(
                         std::string("Rewritten@@term"),
                         function_sort({ untyped_sort() },untyped_sort()))
{
  thread_initialise();
  for (const data_equation& eq: data_spec.equations())
  {
    if (equation_selector(eq))
    {
      try
      {
        CheckRewriteRule(eq);
      }
      catch (std::runtime_error& e)
      {
        mCRL2log(warning) << e.what() << std::endl;
        continue;
      }

      const function_symbol& lhs_head_index=atermpp::down_cast<function_symbol>(get_nested_head(eq.lhs()));

      data_equation_list n;
      std::map< function_symbol, data_equation_list >::iterator it = jitty_eqns.find(lhs_head_index);
      if (it != jitty_eqns.end())
      {
        n = it->second;
      }
      n.push_front(eq);
      jitty_eqns[lhs_head_index] = n;
    }
  }

  rebuild_strategy(data_spec, equation_selector);
}

RewriterJitty::~RewriterJitty() = default;

// Find the variables that occur in the lhs and the rhs of the assignments;
//
std::set<variable> bound_variables_in_substitution(const jitty_assignments_for_a_rewrite_rule& assignments)
{
  std::set<variable> variables_in_substitution;
  for(std::size_t i=0; i<assignments.size; ++i)
  {
    std::set<variable> s=find_free_variables(assignments.assignment[i].term);
    variables_in_substitution.insert(s.begin(),s.end());
    variables_in_substitution.insert(assignments.assignment[i].var);
  }
  return variables_in_substitution;
}

void RewriterJitty::subst_values(
            data_expression& result,
            const jitty_assignments_for_a_rewrite_rule& assignments,
            const data_expression& t,
            data::enumerator_identifier_generator& generator) // This generator is used for the generation of fresh variable names.
{
  if (is_machine_number(t))
  {
    result=t; 
    return;
  }
  if (is_function_symbol(t))
  {
    // result=t;  The following is more efficient as it avoids a call to thread local variables. Should be removed in due time. 
    result.assign(t, *m_thread_aterm_pool);
    return;
  }
  else if (is_variable(t))
  {
    for (std::size_t i=0; i<assignments.size; i++)
    {
      if (t==assignments.assignment[i].var)
      {
        if (assignments.assignment[i].variable_is_a_normal_form)
        {
          // Variables that are in normal form get a tag that they are in normal form.
          make_application(result,this_term_is_in_normal_form(),assignments.assignment[i].term);  
          return;
        }
        // result=assignments.assignment[i].term;
        result.assign(assignments.assignment[i].term,
                      *m_thread_aterm_pool);
        return;
      }
    }
    // result=t;
    result.assign(t, *m_thread_aterm_pool);
    return;
  }
  else if (is_abstraction(t))
  {
    const abstraction& t1=atermpp::down_cast<abstraction>(t);
    const binder_type& binder=t1.binding_operator();
    const variable_list& bound_variables=t1.variables();
    // Check that variables in the left and right hand sides of equations do not clash with bound variables.
    std::set<variable> variables_in_substitution=bound_variables_in_substitution(assignments);
    /* for(std::size_t i=0; i<assignments.size; ++i)
    {
      std::set<variable> s=find_free_variables(assignments.assignment[i].term);
      variables_in_substitution.insert(s.begin(),s.end());
      variables_in_substitution.insert(assignments.assignment[i].var);
    } */

    variable_vector new_variables;
    mutable_map_substitution<> sigma;
    bool sigma_trivial=true;
    for(const variable& v: bound_variables)
    {
      if (variables_in_substitution.count(v)>0)
      {
        // Replace v in the list and in the body by a new variable name.
        const variable fresh_variable(generator(),v.sort());
        new_variables.push_back(fresh_variable);
        sigma[v]=fresh_variable;
        sigma_trivial=false;
      }
      else
      {
        new_variables.push_back(v);
      }
    }
    subst_values(result,
                 assignments,
                 (sigma_trivial?t1.body():replace_variables(t1.body(),sigma)),
                 generator);  
    result=abstraction(binder,
                       variable_list(new_variables.begin(),new_variables.end()),
                       result);
    return;
  }
  else if (is_where_clause(t))
  {
    const where_clause& t1=atermpp::down_cast<where_clause>(t);
    const assignment_expression_list& local_assignments=t1.declarations();
    const data_expression& body=t1.body();

    std::set<variable> variables_in_substitution=bound_variables_in_substitution(assignments);
    
    assignment_vector new_assignments;

    mutable_map_substitution<> sigma;
    bool sigma_trivial=true;

    for(const assignment_expression& a: local_assignments)
    {
      const assignment& assignment_expr = atermpp::down_cast<assignment>(a);
      const variable& v=assignment_expr.lhs();
      subst_values(result,assignments,assignment_expr.rhs(),generator);
      if (variables_in_substitution.count(v)>0)
      {
        // Replace variable in the assignment and in the body by a new variable name.
        const variable fresh_variable(generator(),v.sort());
        new_assignments.emplace_back(fresh_variable, result);
        sigma[v]=fresh_variable;
        sigma_trivial=false;
      }
      else
      {
        new_assignments.emplace_back(v, result);
      }
    }
    subst_values(result,
                 assignments,
                 (sigma_trivial?body:replace_variables(body,sigma)),
                 generator),
    result=where_clause(result, assignment_list(new_assignments.begin(),new_assignments.end()));
    return;
  }
  else
  {
    const application& t1 = atermpp::down_cast<application>(t);
    make_application(result,
                       t1.head(),
                       t1.begin(),
                       t1.end(),
                       [&](data_expression& result, const data_expression& t) -> void
                               { subst_values(result,assignments,t,generator); return;});
  }
}

// Match term t with the lhs p of an equation.
static bool match_jitty(
                    const data_expression& t,
                    const data_expression& p,
                    jitty_assignments_for_a_rewrite_rule& assignments,
                    const bool term_context_guarantees_normal_form)
{
  if (is_machine_number(p))
  {
    return p==t;
  }
  else if (is_function_symbol(p))
  {
    return p==t;
  }
  else if (is_variable(p))
  {

    for (std::size_t i=0; i<assignments.size; i++)
    {
      if (p==assignments.assignment[i].var)
      {
        return t==assignments.assignment[i].term;
      }
    }

    new (&assignments.assignment[assignments.size])
              jitty_variable_assignment_for_a_rewrite_rule(
                                atermpp::down_cast<variable>(p),
                                t,
                                term_context_guarantees_normal_form);
    assignments.size++;
    return true;
  }
  else
  {
    if (is_machine_number(t) || is_function_symbol(t) || is_variable(t) || is_abstraction(t) || is_where_clause(t))
    {
      return false;
    }
    // p and t must be applications.
    assert(term_context_guarantees_normal_form); // If the argument must match an expression it must be a normal form.

    const application& pa=atermpp::down_cast<application>(p);
    const application& ta=atermpp::down_cast<application>(t);
    if (pa.size()!=ta.size()) // are p and t applications of the same arity?
    {
      return false;
    }


    if (!match_jitty(ta.head(),
                     pa.head(),assignments,true))
    {
      return false;
    }

    for (std::size_t i=0; i<pa.size(); i++)
    {
      if (!match_jitty(ta[i], pa[i],assignments,true))
      {
        return false;
      }
    }

    return true;
  }
}


// This function applies the rewrite_cpp_code on a higher order term t with op as head symbol for
// which the code in rewrite_cpp_code must be applied. 
template <class ITERATOR>
void RewriterJitty::apply_cpp_code_to_higher_order_term(
                  data_expression& result,
                  const application& t,
                  const std::function<void(data_expression&, const data_expression&)> rewrite_cpp_code,
                  ITERATOR begin,
                  ITERATOR end,
                  substitution_type& sigma)
{
  if (is_function_symbol(t.head()))
  {
    data_expression intermediate;
    make_application(intermediate, t.head(), begin, end);
    rewrite_cpp_code(result, intermediate);
    return;
  }

  const application& ta=atermpp::down_cast<application>(t.head());
  std::size_t n_args=recursive_number_of_args(ta);
  apply_cpp_code_to_higher_order_term(result,ta,rewrite_cpp_code,begin,begin+n_args,sigma);
  const data_expression rewrite_result=result;  /* TODO Optimize */
  rewrite_aux(result,application(rewrite_result,
                                 begin+n_args,
                                 end,
                                 [&](const data_expression& t){ return application(this_term_is_in_normal_form(),t); } ),
                     sigma);
}


/// \brief Rewrite a term with a given substitution and put the rewritten term in result.
void RewriterJitty::rewrite_aux(
                      data_expression& result,
                      const data_expression& term,
                      substitution_type& sigma)
{
  if (is_function_symbol(term))
  {
    assert(term!=this_term_is_in_normal_form());
    rewrite_aux_const_function_symbol(result,atermpp::down_cast<const function_symbol>(term),sigma);
    return;
  }
  if (is_variable(term))
  {
    sigma.apply(atermpp::down_cast<variable>(term),result, *m_thread_aterm_pool);
    return;
  }
  if (is_machine_number(term))
  {
    result=term;
    return;
  }

  if (is_where_clause(term))
  {
    const where_clause& w = atermpp::down_cast<where_clause>(term);
    rewrite_where(result,w,sigma);
    return;
  }

  if (is_abstraction(term))
  { 
    const abstraction& ta=atermpp::down_cast<abstraction>(term);
    if (is_exists(ta))
    {
      existential_quantifier_enumeration(result,ta,sigma);
      return;
    }
    if (is_forall(ta))
    {
      universal_quantifier_enumeration(result,ta,sigma);
      return;
    }
    assert(is_lambda(ta));
    rewrite_single_lambda(result,ta.variables(),ta.body(),false,sigma);
    return;
  }

  // Here term must have the shape appl(t1,...,tn)
  assert(is_application(term));
  {
    const application& terma=atermpp::down_cast<application>(term);
    if (terma.head()==this_term_is_in_normal_form())
    {
      assert(terma.size()==1);
      assert(remove_normal_form_function(terma[0])==terma[0]);
      // result=terma[0]; the following is more efficient.
      result.assign(terma[0], *m_thread_aterm_pool);
      return;
    }

    // The variable term has the shape appl(t,t1,...,tn);
  
    // First check whether t has the shape appl(appl...appl(f,u1,...,un)(...)(...) where f is a function symbol.
    // In this case rewrite that function symbol. This is an optimisation. If this does not apply t is rewritten,
    // including all its subterms. But this is costly, as not all subterms will be rewritten again
    // in rewrite_aux_function_symbol.
  
    const data_expression& head=get_nested_head(term);
  
    if (is_function_symbol(head) && head!=this_term_is_in_normal_form())
    {
      // return rewrite_aux_function_symbol(atermpp::down_cast<function_symbol>(head),term,sigma);
      rewrite_aux_function_symbol(result, atermpp::down_cast<function_symbol>(head),terma,sigma);
      return;
    }
  
    const application& tapp=atermpp::down_cast<application>(term);
    
    // const data_expression t = rewrite_aux(tapp.head(),sigma);
    m_rewrite_stack.increase(2);

    const std::size_t t = 0;  // Index of variable t in the stack. 
    rewrite_aux(m_rewrite_stack.element(t,2),tapp.head(),sigma);

    // Here t has the shape f(u1,....,un)(u1',...,um')....: f applied several times to arguments,
    // x(u1,....,un)(u1',...,um')....: x applied several times to arguments, or
    // binder x1,...,xn.t' where the binder is a lambda, exists or forall.
  
    const std::size_t head1 = 1;  // Index of variable head1 in the stack. 
    m_rewrite_stack.element(head1,2) = get_nested_head(m_rewrite_stack.element(t,2));
    if (is_function_symbol(m_rewrite_stack.element(head1,2)))
    {
      // In this case t (is top of the rewrite stack) has the shape f(u1...un)(u1'...um')....  where all u1,...,un,u1',...,um' are normal formas.
      // In the invocation of rewrite_aux_function_symbol these terms are rewritten to normalform again.
      make_application(result, m_rewrite_stack.element(t,2) , tapp.begin(), tapp.end()); 
      rewrite_aux_function_symbol(m_rewrite_stack.element(t,2),
                                  atermpp::down_cast<function_symbol>(m_rewrite_stack.element(head1,2)),
                                  atermpp::down_cast<application>(result),
                                  sigma);
      result=m_rewrite_stack.element(t,2);
      m_rewrite_stack.decrease(2);
      return;
    }
    else if (is_variable(m_rewrite_stack.element(head1,2)))
    {
      // return appl(t,t1,...,tn) where t1,...,tn still need to be rewritten.
      jitty_argument_rewriter r(sigma,*this);
      const bool do_not_rewrite_head=false;
      make_application(result, m_rewrite_stack.element(t,2) , tapp.begin(), tapp.end(), r, do_not_rewrite_head); // Replacing r by a lambda term requires 16 more bytes on the stack. 
      m_rewrite_stack.decrease(2);
      return;
    }
    assert(is_abstraction(m_rewrite_stack.top()));
    const abstraction& ta=atermpp::down_cast<abstraction>(m_rewrite_stack.element(t,2) );
    const binder_type& binder(ta.binding_operator());
    if (is_lambda_binder(binder))
    {
      rewrite_lambda_application(result,ta,tapp,sigma);
      m_rewrite_stack.decrease(2);
      return;
    }
    if (is_exists_binder(binder))
    {
      assert(term.size()==1);
      existential_quantifier_enumeration(result,ta,sigma);
      m_rewrite_stack.decrease(2);
      return;
    }
    assert(is_forall_binder(binder));
    assert(term.size()==1);
    universal_quantifier_enumeration(result,ta,sigma);
    m_rewrite_stack.decrease(2);
    return;
  }
}

void RewriterJitty::rewrite_aux_function_symbol(
                      data_expression& result, 
                      const function_symbol& op,
                      const application& term,
                      substitution_type& sigma)
{
  assert(is_function_sort(op.sort()));

  const std::size_t arity=detail::recursive_number_of_args(term);
  assert(arity>0);
  // data_expression* rewritten = MCRL2_SPECIFIC_STACK_ALLOCATOR(data_expression, arity);
  m_rewrite_stack.increase(arity+1); 
  bool* rewritten_defined = MCRL2_SPECIFIC_STACK_ALLOCATOR(bool, arity);

  for(std::size_t i=0; i<arity; ++i)
  {
    rewritten_defined[i]=false;
  }

  const std::size_t op_value=atermpp::detail::index_traits<data::function_symbol,function_symbol_key_type, 2>::index(op);
  make_jitty_strat_sufficiently_larger(op_value);
  const strategy& strat=jitty_strat[op_value];

  if (!strat.rules().empty())
  {
    jitty_assignments_for_a_rewrite_rule assignments(
             MCRL2_SPECIFIC_STACK_ALLOCATOR(jitty_variable_assignment_for_a_rewrite_rule, strat.number_of_variables()));

    for (const strategy_rule& rule : strat.rules())
    {
      if (rule.is_rewrite_index())
      {
        const std::size_t i = rule.rewrite_index();
        if (i < arity)
        {
          assert(!rewritten_defined[i]||i==0);
          if (!rewritten_defined[i])
          {
            // new (&rewritten[i]) data_expression(rewrite_aux(detail::get_argument_of_higher_order_term(term,i),sigma));
            rewrite_aux(m_rewrite_stack.element(i,arity+1),detail::get_argument_of_higher_order_term(term,i),sigma);
            
            rewritten_defined[i]=true;
          }
          // assert(rewritten[i].defined());
          assert(m_rewrite_stack.element(i,arity+1).defined());
        }
        else
        {
          break;
        }
      }
      else if (rule.is_cpp_code())
      {
        // Here it is assumed that precompiled code only works on the exact right number of arguments and
        // precompiled functions are not used in a higher order fashion. Maybe this requires an explicit check. 
        assert(arity>0);
        if (term.head()==op) 
        { 
          // application rewriteable_term(op, &rewritten[0], &rewritten[arity]);
          assert(m_rewrite_stack.stack_size()>=arity+1);
          application rewriteable_term(op, m_rewrite_stack.stack_iterator(0,arity+1),
                                           m_rewrite_stack.stack_iterator(arity,arity+1)); /* TODO Optimize */
          rule.rewrite_cpp_code()(result, rewriteable_term);
          m_rewrite_stack.decrease(arity+1);
          return;
        }
        else
        {
          // Guarantee that all higher order arguments are in normal form. Maybe this had to be done in the strategy for higher
          // order terms. 
          for(std::size_t i=0; i<recursive_number_of_args(term); i++)
          {
            if (!rewritten_defined[i])
            {
              // new (&rewritten[i]) data_expression(rewrite_aux(detail::get_argument_of_higher_order_term(term,i),sigma));
              rewrite_aux(m_rewrite_stack.element(i,arity+1),detail::get_argument_of_higher_order_term(term,i),sigma);
              rewritten_defined[i]=true;
            }
          }
          // return apply_cpp_code_to_higher_order_term(term,  rule.rewrite_cpp_code(), &rewritten[0], &rewritten[arity], sigma);
          apply_cpp_code_to_higher_order_term(
                                           result,
                                           term,  
                                           rule.rewrite_cpp_code(),  
                                           m_rewrite_stack.stack_iterator(0,arity+1),
                                           m_rewrite_stack.stack_iterator(arity,arity+1), sigma);
          m_rewrite_stack.decrease(arity+1); 
          return;
        }
      }
      else
      {
        const data_equation& rule1=rule.equation();
        const data_expression& lhs=rule1.lhs();
        std::size_t rule_arity = (is_function_symbol(lhs)?0:detail::recursive_number_of_args(lhs));

        if (rule_arity > arity)
        {
          break;
        }

        assert(assignments.size==0);

        bool matches = true;
        for (std::size_t i=0; i<rule_arity; i++)
        {
          assert(i<arity);
          if (!match_jitty(rewritten_defined[i]?
                                 m_rewrite_stack.element(i,arity+1):
                                 detail::get_argument_of_higher_order_term(term,i),
                           detail::get_argument_of_higher_order_term(atermpp::down_cast<application>(lhs),i),
                           assignments,rewritten_defined[i]))
          {
            matches = false;
            break;
          }
        }
        if (matches)
        {
          bool condition_of_this_rule=false;
          if (rule1.condition()==sort_bool::true_())
          { 
            condition_of_this_rule=true;
          }
          else
          {
            subst_values(m_rewrite_stack.top(),assignments,rule1.condition(),m_generator);
            rewrite_aux(result, m_rewrite_stack.top(), sigma);
            if (result==sort_bool::true_())
            {
              condition_of_this_rule=true;
            }
          }
          if (condition_of_this_rule)
          {
            const data_expression& rhs=rule1.rhs();

            if (arity == rule_arity)
            {
              // const data_expression result=rewrite_aux(subst_values(assignments,rhs,m_generator),sigma);
              subst_values(m_rewrite_stack.top(),assignments,rhs,m_generator);
              rewrite_aux(result, m_rewrite_stack.top(),sigma);
              m_rewrite_stack.decrease(arity+1);
              return;
            }
            else
            {
              assert(arity>rule_arity);
              // There are more arguments than those that have been rewritten.
              // Get those, put them in rewritten.

              for(std::size_t i=rule_arity; i<arity; ++i)
              {
                m_rewrite_stack.set_element(i,arity+1,detail::get_argument_of_higher_order_term(term,i));
                rewritten_defined[i]=true;
              }

              subst_values(m_rewrite_stack.top(),assignments,rhs,m_generator);
              std::size_t i = rule_arity;
              sort_expression sort = detail::residual_sort(op.sort(),i);
              while (is_function_sort(sort) && (i < arity))
              {
                const function_sort& fsort =  atermpp::down_cast<function_sort>(sort);
                const std::size_t end=i+fsort.domain().size();
                assert(end-1<arity);
                // result = application(result,&rewritten[0]+i,&rewritten[0]+end);
                assert(m_rewrite_stack.stack_size()+i>=arity+1);
                assert(end<arity+1);
                assert(end>=i);

                make_application(m_rewrite_stack.top(),m_rewrite_stack.top(),
                                     m_rewrite_stack.stack_iterator(i,arity+1),
                                     m_rewrite_stack.stack_iterator(end,arity+1));
                i=end;
                sort = fsort.codomain();
              }

              rewrite_aux(result,m_rewrite_stack.top(),sigma);
              m_rewrite_stack.decrease(arity+1);
              return;
            }
          }
          
        }
        assignments.size=0;
      }
    }
  }

  // No rewrite rule is applicable. Rewrite the not yet rewritten arguments.
  // As we rewrite all, we do not record anymore whether terms are rewritten.

  for (std::size_t i=0; i<arity; i++)
  {
    if (!rewritten_defined[i])
    {
      // new (&rewritten[i]) data_expression(rewrite_aux(detail::get_argument_of_higher_order_term(term,i),sigma));
      rewrite_aux(m_rewrite_stack.element(i,arity+1),detail::get_argument_of_higher_order_term(term,i),sigma);
    }
  }

  // The while loop must always be iterated once. Therefore, the initial traversal is put before the
  // main loop. 
  const function_sort& fsort=atermpp::down_cast<function_sort>(op.sort());
  const std::size_t end=fsort.domain().size();

  make_application(result,op,m_rewrite_stack.stack_iterator(0,arity+1), m_rewrite_stack.stack_iterator(end,arity+1));
  std::size_t i=end;
  const sort_expression* sort = &fsort.codomain();
  while (i<arity && is_function_sort(*sort))
  {
    const function_sort& fsort=atermpp::down_cast<function_sort>(*sort);
    const std::size_t end=i+fsort.domain().size();
    assert(m_rewrite_stack.stack_size()+i>=arity+1);
    assert(end<arity+1);
    assert(end>=i);
    make_application(result,result,m_rewrite_stack.stack_iterator(i,arity+1), m_rewrite_stack.stack_iterator(end,arity+1));
    i=end;
    sort = &fsort.codomain();
  }

  m_rewrite_stack.decrease(arity+1);
  return; 
}

void RewriterJitty::rewrite_aux_const_function_symbol(
                      data_expression& result,
                      const function_symbol& op,
                      substitution_type& sigma)
{
  // This is special code to rewrite a function symbol. Note that the function symbol can be higher order,
  // e.g., it can be a function symbol f for which a rewrite rule f(n)=... exists. 

  const std::size_t op_value=atermpp::detail::index_traits<data::function_symbol,function_symbol_key_type, 2>::index(op);
  make_jitty_strat_sufficiently_larger(op_value);

  // Cache the rhs's as they are rewritten very often. 
  if (rhs_for_constants_cache.size()<=op_value)
  {
    rhs_for_constants_cache.resize(op_value+1);
  }
  const data_expression& cached_rhs = rhs_for_constants_cache[op_value];
  if (!cached_rhs.is_default_data_expression())
  {
    // result=cached_rhs;
    /* result.assign(cached_rhs,
                  this->m_busy_flag,
                  this->m_forbidden_flag,
                  *this->m_creation_depth); */                 
    result.assign(cached_rhs, *m_thread_aterm_pool);
    return;
  }

  const strategy& strat=jitty_strat[op_value];

  for (const strategy_rule& rule : strat.rules())
  {
    if (rule.is_rewrite_index())
    {
      // In this case a standalone function symbol is rewritten, which could have arguments. 
      // It is not needed to rewrite the arguments. 
      break;
    }
    else if (rule.is_cpp_code())
    {
      rule.rewrite_cpp_code()(result, op);
      rhs_for_constants_cache[op_value]=result;
      return;
    }
    else
    {
      const data_equation& rule1=rule.equation();
      const data_expression& lhs=rule1.lhs();
      std::size_t rule_arity = (is_function_symbol(lhs)?0:detail::recursive_number_of_args(lhs));

      if (rule_arity > 0)
      {
        break;
      }

      if (rule1.condition()==sort_bool::true_())
      { 
        rewrite_aux(result,rule1.rhs(),sigma);
        rhs_for_constants_cache[op_value]=result;
        return;
      }
      rewrite_aux(result,rule1.condition(),sigma);
      if (result==sort_bool::true_())
      {
        rewrite_aux(result,rule1.rhs(),sigma);
        rhs_for_constants_cache[op_value]=result;
        return;
      }
    }
  }

  rhs_for_constants_cache[op_value]=op;
  result=op; 
  return;
}

void RewriterJitty::rewrite(
     data_expression& result,
     const data_expression& term,
     substitution_type& sigma)
{
#ifdef MCRL2_DISPLAY_REWRITE_STATISTICS
  data::detail::increment_rewrite_count();
#endif
  if (rewriting_in_progress)
  {
    rewrite_aux(result, term, sigma);
  }
  else
  {
    assert(m_rewrite_stack.stack_size()==0);
    rewriting_in_progress=true;
    try
    {
      rewrite_aux(result, term, sigma);
    }
    catch (recalculate_term_as_stack_is_too_small&)
    {
      rewriting_in_progress=false; // Restart rewriting, due to a stack overflow.
                                   // The stack is a vector, and it may be relocated in memory when
                                   // resized. References to the stack loose their validity. 
      m_rewrite_stack.reserve_more_space();
      rewrite(result,term,sigma);
      return;
    }
    rewriting_in_progress=false;
    assert(m_rewrite_stack.stack_size()==0);
  }

  assert(remove_normal_form_function(result)==result);
  return;
}

data_expression RewriterJitty::rewrite(
     const data_expression& term,
     substitution_type& sigma)
{
  data_expression result;
  rewrite(result, term, sigma);
  return result;
}


rewrite_strategy RewriterJitty::getStrategy()
{
  return jitty;
}
}


