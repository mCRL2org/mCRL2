// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <sstream>
#include <boost/signals2/detail/auto_buffer.hpp>
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/alias.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/detail/enum/standard.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

using namespace std;

namespace mcrl2
{
namespace data
{
namespace detail
{

// Ugly add hoc class. Ought to be replaced when lambda notation can be used.
class test_equal
{
  const aterm_appl &m_term;

  public:
    test_equal(const data_expression &t) : m_term(t)
    {}

    bool operator ()(const aterm_appl &other) const
    {
      return m_term==other;
    }
};

data_expression EnumeratorSolutionsStandard::add_negations(
                                const data_expression& condition,
                                const data_expression_list& negation_term_list,
                                const bool negated) const
{ /* If negation_term_list is [t1,...,tn], generate an expression of the form
     condition /\ !t1 /\ !t2 /\ ... /\ !tn in internal format. Using ad hoc
     rewriting is cheaper than using a full fledged rewriter, that will
     again normalise all subterms.
  */
  if (negation_term_list.empty())
  {
    if (negated)
    {
      if (condition == m_enclosing_enumerator->rewr_obj->internal_true)
      {
        return m_enclosing_enumerator->rewr_obj->internal_false;
      }
      else if (condition == m_enclosing_enumerator->rewr_obj->internal_false)
      {
        return m_enclosing_enumerator->rewr_obj->internal_true;
      }
      else if (is_application(condition))
      {
        const application& ca(condition);
        if (ca.head() == m_enclosing_enumerator->rewr_obj->internal_not)
        {
          return ca[0];
        }
      }
      return application(m_enclosing_enumerator->rewr_obj->internal_not, condition);
    }
    return condition;
  }

  const data_expression first_argument=add_negations(condition,negation_term_list.tail(),negated);
  data_expression second_argument= negation_term_list.front();
  if (!negated)
  {
    if (second_argument == m_enclosing_enumerator->rewr_obj->internal_true)
    {
      return m_enclosing_enumerator->rewr_obj->internal_false;
    }
    else if (second_argument == m_enclosing_enumerator->rewr_obj->internal_false)
    {
      return m_enclosing_enumerator->rewr_obj->internal_true;
    }
    else if (is_application(second_argument) && aterm_cast<const application>(second_argument).head()== m_enclosing_enumerator->rewr_obj->internal_not)
    {
      second_argument=aterm_cast<const application>(second_argument)[0];
    }
    else
    {
      second_argument=application(m_enclosing_enumerator->rewr_obj->internal_not,second_argument);
    }
  }

  if (first_argument==m_enclosing_enumerator->rewr_obj->internal_true)
  {
    return second_argument;
  }
  else if (first_argument==m_enclosing_enumerator->rewr_obj->internal_false)
  {
    return m_enclosing_enumerator->rewr_obj->internal_false;
  }
  if (second_argument==m_enclosing_enumerator->rewr_obj->internal_true)
  {
    return first_argument;
  }
  else if (second_argument==m_enclosing_enumerator->rewr_obj->internal_false)
  {
    return m_enclosing_enumerator->rewr_obj->internal_false;
  }
  else
  {
    return application(m_enclosing_enumerator->rewr_obj->internal_and,
                first_argument,
                second_argument);
  }
}

data_expression_list EnumeratorSolutionsStandard::negate(const data_expression_list& l) const
{
  if (l.empty())
  {
    return l;
  }
  data_expression_list result=negate(l.tail());
  result.push_front(application(m_enclosing_enumerator->rewr_obj->internal_not,l.front()));
  return result;
}

void EnumeratorSolutionsStandard::push_on_fs_stack_and_split_or_without_rewriting(
                                std::deque < fs_expr> &fs_stack,
                                const variable_list &var_list,
                                const variable_list &substituted_vars,
                                const data_expression_list &substitution_terms,
                                const data_expression &condition,
                                const data_expression_list &negated_term_list,
                                const bool negated) const
{
  /* If the negated_term_list equals t1,...,tn, store condition /\ !t1 /\ !t2 /\ ... /\ !tn
     on the fs_stack.  If the condition to be stored on the fs_stack has the shape phi \/ psi, then
     store phi and psi /\ !phi separately. This allows the equality eliminator to remove
     more equalities and therefore be more effective. */
  if (is_application(condition))
  {
    const application& ca(condition);
    if (ca.head() == m_enclosing_enumerator->rewr_obj->internal_not)
    {
      push_on_fs_stack_and_split_or_without_rewriting(fs_stack,var_list,substituted_vars,substitution_terms,ca[0],negate(negated_term_list),!negated);
      return;
    }
    if ((negated && ca.head() == m_enclosing_enumerator->rewr_obj->internal_and) ||
             (!negated && ca.head() == m_enclosing_enumerator->rewr_obj->internal_or))
    {
      assert(condition.size()==3);
      push_on_fs_stack_and_split_or_without_rewriting(fs_stack,var_list,substituted_vars,substitution_terms,ca[0],negated_term_list,negated);
      data_expression_list temp=negated_term_list;
      temp.push_front(ca[1]);
      push_on_fs_stack_and_split_or_without_rewriting(fs_stack,var_list,substituted_vars,substitution_terms,ca[1], temp,negated);
      return;
    }
  }
  
  const data_expression new_expr = add_negations(condition,negated_term_list,negated);

  if (new_expr!=m_enclosing_enumerator->rewr_obj->internal_false)
  {
#ifndef NDEBUG
    // Check that substituted variables do not occur in the expression expr.
    std::set <variable> s=data::find_free_variables(new_expr);
    for(std::set <variable>::const_iterator it=s.begin(); it!=s.end(); ++it)
    {
      assert(std::find(substituted_vars.begin(),substituted_vars.end(),*it)==substituted_vars.end());
    }
#endif

    fs_stack.push_back(fs_expr(var_list,
                               substituted_vars,
                               substitution_terms,
                               new_expr));
  }
}

void EnumeratorSolutionsStandard::push_on_fs_stack_and_split_or(
                                std::deque < fs_expr> &fs_stack,
                                const variable_list &var_list,
                                const variable_list &substituted_vars,
                                const data_expression_list &substitution_terms,
                                const data_expression &condition,
                                const data_expression_list &negated_term_list,
                                const bool negated) const
{
  EnumeratorSolutionsStandard::push_on_fs_stack_and_split_or_without_rewriting(
                                fs_stack,
                                var_list,
                                substituted_vars,
                                substitution_terms,
                                m_enclosing_enumerator->rewr_obj->rewrite(condition,enum_sigma),
                                negated_term_list,
                                negated);
}

bool EnumeratorSolutionsStandard::find_equality(
                        const data_expression &t,
                        const mcrl2::data::variable_list &vars,
                        mcrl2::data::variable &v,
                        data_expression &e)
{
  if (is_variable(t))
  {
    assert(variable(t).sort()==sort_bool::bool_());
    return false;
  }

  if (is_abstraction(t) || is_where_clause(t))
  {
    return false;
  }

  if (is_function_symbol(t))
  {
    assert(data::function_symbol(t).sort()==sort_bool::bool_());
    return false;
  }
  
  const application& ta(t);
  
  if (is_function_symbol(ta.head()))
  {
    const function_symbol& f(ta.head());
    if (f == m_enclosing_enumerator->rewr_obj->internal_and)
    {
      assert(ta.size()==2);
      return find_equality(ta[0],vars,v,e) || find_equality(ta[1],vars,v,e);
    }
    else if (to_string(f.name()) == "==")
    {
      const data_expression& a1 = ta[0];
      const data_expression& a2 = ta[1];
      if (a1!=a2)
      {
        if (is_variable(a1) && (find(vars.begin(),vars.end(),variable(a1))!=vars.end()) &&
                          (atermpp::find_if(static_cast<const aterm_appl&>(a2),test_equal(a1))==aterm_appl()))        // true if a1 does not occur in a2.
        {
          v = aterm_cast<variable>(a1);
          e = a2;
          return true;
        }
        if (is_variable(a2) && (find(vars.begin(),vars.end(),variable(a2))!=vars.end()) &&
                                 (atermpp::find_if(a1,test_equal(a2))==aterm_appl()))        // true if a2 does not occur in a1.
        {
          v = aterm_cast<variable>(a2);
          e = a1;
          return true;
        }
      }
    }
  }

  return false;
}

void EnumeratorSolutionsStandard::EliminateVars(fs_expr &e)
{
  variable_list vars = e.vars();
  variable_list substituted_vars = e.substituted_vars();
  data_expression_list vals = e.vals();
  data_expression expr = e.expr();

  variable var;
  data_expression val;

  while (!vars.empty() && find_equality(expr,vars,var,val))
  {
    vars = remove_one_element(vars, var);
    substituted_vars.push_front(var);
    vals.push_front(val);

    // Use a rewrite here to remove occurrences of subexpressions the form t==t caused by
    // replacing in x==t the variable x by t.
    const data_expression old_val=enum_sigma(var);
    enum_sigma[var]=val;
    expr = m_enclosing_enumerator->rewr_obj->rewrite(expr,enum_sigma);
    enum_sigma[var]=old_val;
  }

#ifndef NDEBUG
  // Check that substituted variables do not occur in the expression expr.

  std::set <variable> s=data::find_free_variables(expr);
  for(std::set <variable>::const_iterator it=s.begin(); it!=s.end(); ++it)
  {
    assert(std::find(substituted_vars.begin(),substituted_vars.end(),*it)==substituted_vars.end());
  }

#endif
  e=fs_expr(vars,substituted_vars,vals,expr);
}

data_expression EnumeratorSolutionsStandard::build_solution_single(
                 const data_expression &t,
                 variable_list substituted_vars,
                 data_expression_list exprs) const
{
  assert(substituted_vars.size()==exprs.size());
  while (!substituted_vars.empty() && t!=substituted_vars.front())
  {
    substituted_vars.pop_front();
    exprs.pop_front();
  }

  if (substituted_vars.empty())
  {
    return t;
  }
  else
  {
    return build_solution_aux(exprs.front(),substituted_vars.tail(),exprs.tail());
  }
}

data_expression EnumeratorSolutionsStandard::build_solution_aux(
                 const data_expression &t,
                 const variable_list &substituted_vars,
                 const data_expression_list &exprs) const
{
  if (is_variable(t))
  {
    return build_solution_single(t,substituted_vars,exprs);
  }
  else if (is_where_clause(t))
  {
    assert(0); // This is a non expected case as t is a normalform.
    return t; // compiler warning
  }
  else if (is_abstraction(t))
  {
    const data_expression &t1=t;
    const binder_type &binder=aterm_cast<const binder_type>(t1[0]);
    const variable_list &bound_variables=aterm_cast<const variable_list>(t1[1]);
    const data_expression &body=build_solution_aux(aterm_cast<const data_expression>(t1[2]),substituted_vars,exprs);
    return abstraction(binder,bound_variables,body);
  }
  else if (is_function_symbol(t))
  {
    return t;
  }
  else
  {
    // t has the shape application(u1,...,un)
    const application t_appl(t); 
    data_expression head = t_appl.head(); 
   
    if (!is_function_symbol(head))
    {
      head = build_solution_single(head,substituted_vars,exprs);
    }

    data_expression_vector args;
    for (application::const_iterator i=t_appl.begin(); i!=t_appl.end(); i++)
    {
      args.push_back(build_solution_aux(*i,substituted_vars,exprs));
    }

    return application(head,args);
  }
}

data_expression_list EnumeratorSolutionsStandard::build_solution2(
                 const variable_list &vars,
                 const variable_list &substituted_vars,
                 const data_expression_list &exprs) const
{
  if (vars.empty())
  {
    return data_expression_list();
  }
  else
  {
    data_expression_list result=build_solution2(vars.tail(),substituted_vars,exprs);
    result.push_front(m_enclosing_enumerator->rewr_obj->rewrite(build_solution_single(vars.front(),substituted_vars,exprs),enum_sigma));
    return result;
  }
}

data_expression_list EnumeratorSolutionsStandard::build_solution(
                 const variable_list &vars,
                 const variable_list &substituted_vars,
                 const data_expression_list &exprs) const
{
  return build_solution2(vars,reverse(substituted_vars),reverse(exprs));
}

bool EnumeratorSolutionsStandard::next(
              data_expression &evaluated_condition,
              data_expression_list &solution,
              bool &solution_possible)
{
  data_expression_vector var_array; // TODO: locating var_array on stack is most likely more efficient.

  while (ss_stack.empty() && !fs_stack.empty())
  {
    fs_expr e=fs_stack.front();
    EliminateVars(e);
    fs_stack.pop_front();
    if (e.vars().empty() || e.expr()==m_enclosing_enumerator->rewr_obj->internal_false)
    {
      if (e.expr()!=m_enclosing_enumerator->rewr_obj->internal_false) // So e.vars() is empty.
      {
        ss_stack.push_back(
                       ss_solution(build_solution(
                                        enum_vars,e.substituted_vars(),
                                        e.vals()),
                                   e.expr()));
      }
    }
    else
    {
      assert(!e.vars().empty());
      assert(e.expr()!=m_enclosing_enumerator->rewr_obj->internal_false);
      const variable &var = e.vars().front();
      const sort_expression &sort = var.sort();
      const variable_list &uvars = e.vars().tail();


      if (is_function_sort(sort))
      {
        if (solution_possible)
        {
          solution_possible=false;
          return false;
        }
        else
        {
          fs_stack.clear();
          throw mcrl2::runtime_error("cannot enumerate elements of the function sort " + data::pp(sort));
        }

      }
      else if (sort_bag::is_bag(sort))
      {
        if (solution_possible)
        {
          solution_possible=false;
          return false;
        }
        else
        {
          fs_stack.clear();
          throw mcrl2::runtime_error("cannot enumerate elements of a bag of sort " + data::pp(sort));
        }

      }
      else if (sort_set::is_set(sort))
      {
        //const sort_expression element_sort=container_sort(sort).element_sort();
        /* if (m_enclosing_enumerator->m_data_spec.is_certainly_finite(element_sort))
        {
          / * Enumerate and store
          for( TODO
          {
          } * /
        }
        else */
        {
          if (solution_possible)
          {
            solution_possible=false;
            return false;
          }
          else
          {
            fs_stack.clear();
            throw mcrl2::runtime_error("cannot enumerate all elements of a set of sort " + data::pp(sort));
          }
        }
      }
      else
      {
        const function_symbol_vector &constructors_for_sort = m_enclosing_enumerator->m_data_spec.constructors(sort);
        function_symbol_vector::const_iterator it=constructors_for_sort.begin();

        if ( it == constructors_for_sort.end() )
        {
          if (solution_possible)
          {
            solution_possible=false;
            return false;
          }
          else
          {
            fs_stack.clear();
            throw mcrl2::runtime_error("cannot enumerate elements of sort " + data::pp(sort) + " as it does not have constructor functions");
          }
        }

        assert(!it->empty());
        for( ; it!=constructors_for_sort.end() ; ++it)
        {
          // Construct the domain and target sort for the constructor.
          // sort_expression target_sort=it->sort();
          // sort_expression_list domain_sorts;
          const sort_expression &it_sort=it->sort();
          if (is_function_sort(it_sort))
          {
            const sort_expression_list& domain_sorts=aterm_cast<function_sort>(it_sort).domain();
            assert(function_sort(it_sort).codomain()==sort);

            variable_list var_list;
            assert(var_array.size()==0);
            // var_array.push_back(OpId2Int(*it));
            // var_array.push_back(*it);

            for (sort_expression_list::const_iterator i=domain_sorts.begin(); i!=domain_sorts.end(); ++i)
            {
              const variable fv(m_enclosing_enumerator->rewr_obj->generator("@x@",false),*i);
              var_list.push_front(fv);
              var_array.push_back(fv);

              used_vars++;
              if (m_max_internal_variables!=0 && used_vars > m_max_internal_variables)
              {
                if (solution_possible && max_vars != 0)
                {
                  mCRL2log(log::debug)   << "Enumerating expression: "<< data::pp(enum_expr) << std::endl;
                  mCRL2log(log::warning) << "Terminated enumeration of variables because more than " << m_max_internal_variables << " are used.\n";
                  solution_possible=false;
                  return false;
                }
                else
                {
                  fs_stack.clear();
                  stringstream exception_message;
                  exception_message << "needed more than " << m_max_internal_variables << " variables to find all valuations of ";
                  for (variable_list::const_iterator k=enum_vars.begin(); k!=enum_vars.end(); ++k)
                  {
                    if (k != enum_vars.begin())
                    {
                      exception_message << ", ";
                    }
                    exception_message << data::pp(*k) << ":" << data::pp(k->sort());
                  }
                  exception_message << " that satisfy " << data::pp(enum_expr);
                  throw mcrl2::runtime_error(exception_message.str());
                }
              }
              else if (used_vars > max_vars)
              {
                mCRL2log(log::info) << "need more than " << max_vars << " variables to find all valuations of ";
                for (variable_list::const_iterator k=enum_vars.begin(); k!=enum_vars.end(); ++k)
                {
                  if (k != enum_vars.begin())
                  {
                    mCRL2log(log::info) << ", ";
                  }
                  mCRL2log(log::info) << data::pp(*k) << ":" << data::pp(k->sort());
                }
                mCRL2log(log::info) << " that satisfy " << data::pp(enum_expr) << endl;
                max_vars *= MAX_VARS_FACTOR;
              }
            }
            // Substitutions must contain normal forms.  term_rf is almost always a normal form, but this is
            // not guaranteed and must be guaranteed by rewriting it explicitly. In the line below enum_sigma has no effect, but
            // using it is much cheaper than using a default substitution.
            const data_expression term_rf = m_enclosing_enumerator->rewr_obj->rewrite(
                       application(*it,var_array),enum_sigma);
            var_array.clear();

            const data_expression old_substituted_value=enum_sigma(var);
            enum_sigma[var]=term_rf;
            const data_expression rewritten_expr=m_enclosing_enumerator->rewr_obj->rewrite(e.expr(),enum_sigma);
            enum_sigma[var]=old_substituted_value;
            variable_list templist1=e.substituted_vars();
            templist1.push_front(var);
            data_expression_list templist2=e.vals();
            templist2.push_front(term_rf);
            push_on_fs_stack_and_split_or_without_rewriting(
                                    fs_stack,
                                    uvars+var_list,
                                    templist1,
                                    templist2,
                                    rewritten_expr,
                                    data_expression_list(),
                                    false);
          }
          else 
          {
            // is_function_sort(it->sort()) does not hold.
            // Construct the domain and target sort for the constructor.
            assert(it->sort()==sort);
          
            // Substitutions must contain normal forms.  term_rf is almost always a normal form, but this is
            // not guaranteed and must be guaranteed by rewriting it explicitly. In the line below enum_sigma has no effect, but
            // using it is much cheaper than using a default substitution.
            // const data_expression term_rf = m_enclosing_enumerator->rewr_obj->rewrite(aterm_appl(get_appl_afun_value(1),OpId2Int(*it)),enum_sigma);
            const data_expression term_rf = m_enclosing_enumerator->rewr_obj->rewrite(*it,enum_sigma);

            const data_expression old_substituted_value=enum_sigma(var);
            enum_sigma[var]=term_rf;
            const data_expression rewritten_expr=m_enclosing_enumerator->rewr_obj->rewrite(e.expr(),enum_sigma);
            enum_sigma[var]=old_substituted_value;
            variable_list templist1=e.substituted_vars();
            templist1.push_front(var);
            data_expression_list templist2=e.vals();
            templist2.push_front(term_rf);
            push_on_fs_stack_and_split_or_without_rewriting(
                                    fs_stack,
                                    uvars,
                                    templist1,
                                    templist2,
                                    rewritten_expr,
                                    data_expression_list(),
                                    false);
          }
        }
      }
    }
  }
  if (!ss_stack.empty())
  {
    solution = ss_stack.back().solution();
    evaluated_condition = ss_stack.back().evaluated_condition();
    ss_stack.pop_back();
    return true;
  }
  else
  {
    return false;
  }
}

bool EnumeratorSolutionsStandard::next(data_expression_list &solution)
{
  data_expression dummy_evaluated_condition;
  return next(dummy_evaluated_condition,solution);
}

bool EnumeratorSolutionsStandard::next(
          data_expression &evaluated_condition,
          data_expression_list &solution)
{
  bool dummy_solution_possible=false;
  return next(evaluated_condition,solution,dummy_solution_possible);

}

bool EnumeratorSolutionsStandard::next(
          data_expression_list &solution,
          bool &solution_possible)
{
  data_expression dummy_evaluated_condition;
  return next(dummy_evaluated_condition,solution,solution_possible);
}

void EnumeratorSolutionsStandard::reset(const bool not_equal_to_false,const bool expr_is_normal_form)
{
  if (expr_is_normal_form)
  {
    push_on_fs_stack_and_split_or_without_rewriting(fs_stack,
                                  enum_vars,
                                  variable_list(),
                                  data_expression_list(),
                                  enum_expr,
                                  data_expression_list(),
                                  !not_equal_to_false);
  }
  else
  {
    push_on_fs_stack_and_split_or(fs_stack,
                                  enum_vars,
                                  variable_list(),
                                  data_expression_list(),
                                  enum_expr,
                                  data_expression_list(),
                                  !not_equal_to_false);
  }
}

EnumeratorStandard::EnumeratorStandard(const mcrl2::data::data_specification &data_spec, Rewriter* r):
  m_data_spec(data_spec)
{
  rewr_obj = r;
}

EnumeratorStandard::~EnumeratorStandard()
{
}

} // namespace detail
} // namespace data
} // namespace mcrl2
