// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/aterm_ext.h"
#include "mcrl2/core/detail/memory_utility.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/core/print.h"
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
  atermpp::aterm_appl m_term;

  public:
    test_equal(const atermpp::aterm_appl &t) : m_term(t)
    {}

    bool operator ()(const atermpp::aterm_appl &other) const
    { 
      return m_term==other;
    }
}; 

atermpp::aterm_appl EnumeratorSolutionsStandard::add_negations(
                                const atermpp::aterm_appl condition,
                                const atermpp::term_list< atermpp::aterm_appl > negation_term_list,
                                const bool negated) const
{ /* If negation_term_list is [t1,...,tn], generate an expression of the form
     condition /\ !t1 /\ !t2 /\ ... /\ !tn in internal format. */
  if (negation_term_list.empty())
  { 
    if (negated)
    { 
      return (ATerm)Apply1((ATerm)(ATermInt)m_enclosing_enumerator->opidNot,
                           (ATerm)(ATermAppl)condition);
    }
    return condition;
  }

  return Apply2((ATerm)(ATermInt)m_enclosing_enumerator->opidAnd,
                (ATerm)(ATermAppl)add_negations(condition,pop_front(negation_term_list),negated),
                (negated?
                    (ATerm)(ATermAppl)negation_term_list.front():
                    (ATerm)Apply1((ATerm)(ATermInt)m_enclosing_enumerator->opidNot,
                                  (ATerm)(ATermAppl)negation_term_list.front())));
}

atermpp::term_list< atermpp::aterm_appl > EnumeratorSolutionsStandard::negate(const atermpp::term_list< atermpp::aterm_appl > l) const
{
  if (l.empty())
  {
    return l;
  }
  return push_front(negate(pop_front(l)),
                    static_cast<atermpp::aterm_appl>(Apply1(
                                             (ATerm)(ATermInt)m_enclosing_enumerator->opidNot,
                                             (ATerm)(ATermAppl)l.front())));
}

void EnumeratorSolutionsStandard::push_on_fs_stack_and_split_or(
                                atermpp::deque < fs_expr> &fs_stack,
                                const variable_list var_list,
                                const variable_list substituted_vars,
                                const atermpp::term_list< atermpp::aterm_appl > substitution_terms,
                                const atermpp::aterm_appl condition,
                                const atermpp::term_list< atermpp::aterm_appl > negated_term_list,
                                const bool negated) const
{
   /* If the negated_term_list equals t1,...,tn, store condition /\ !t1 /\ !t2 /\ ... /\ !tn
      on the fs_stack.  If the condition to be stored on the fs_stack has the shape phi \/ psi, then
      store phi and psi /\ !phi separately. This allows the equality eliminator to remove
      more equalities and therefore be more effective. */
   const atermpp::aterm_appl condition1 = m_enclosing_enumerator->rewr_obj->rewriteInternal((ATerm)(ATermAppl)condition);
   if (condition1(0) == m_enclosing_enumerator->opidNot)
   {
     push_on_fs_stack_and_split_or(fs_stack,var_list,substituted_vars,substitution_terms,condition1(1),negate(negated_term_list),!negated);
   }
   else if ((negated && condition1(0) == m_enclosing_enumerator->opidAnd) ||
            (!negated && condition1(0) == m_enclosing_enumerator->opidOr))
   { 
     assert(condition1.size()==3);
     push_on_fs_stack_and_split_or(fs_stack,var_list,substituted_vars,substitution_terms,condition1(1),negated_term_list,negated);
     push_on_fs_stack_and_split_or(fs_stack,var_list,substituted_vars,substitution_terms,condition1(2),
                            push_front(negated_term_list,static_cast<atermpp::aterm_appl>(condition1(1))),negated);
   }
   else
   { 
     const atermpp::aterm_appl new_expr = m_enclosing_enumerator->rewr_obj->rewriteInternal(
                 (ATerm)(ATermAppl)add_negations(condition1,negated_term_list,negated));

     if (new_expr!=m_enclosing_enumerator->rewr_false)
     { 
       fs_stack.push_back(fs_expr(var_list,
                              substituted_vars,
                              substitution_terms, 
                              new_expr));
     }
   }
}

bool EnumeratorSolutionsStandard::FindInnerCEquality(
                        const atermpp::aterm_appl t,
                        const mcrl2::data::variable_list vars, 
                        mcrl2::data::variable &v, 
                        atermpp::aterm_appl &e)
{
  if (is_variable(t))
  { 
    assert(variable(t).sort()==sort_bool::bool_());
    return false;
  } 

  if (t(0) == m_enclosing_enumerator->opidAnd)
  {
    assert(t.size()==3);
    return FindInnerCEquality(t(1),vars,v,e) || FindInnerCEquality(t(2),vars,v,e);
  }
  else if (m_enclosing_enumerator->eqs.find(t(0)) != m_enclosing_enumerator->eqs.end())  // Does term t have an equality as its function symbol?
  {
    const atermpp::aterm_appl a1 = t(1); 
    const atermpp::aterm_appl a2 = t(2); 
    if (a1!=a2)
    {
      if (is_variable(a1) && (find(vars.begin(),vars.end(),a1)!=vars.end()) && 
                               (find_if(a2,test_equal(a1))==atermpp::aterm_appl()))        // true if a1 does not occur in a2.
      {
        v = a1;
        e = a2; 
        return true;
      }
      if (is_variable(a2) && (find(vars.begin(),vars.end(),a2)!=vars.end()) &&
                               (find_if(a1,test_equal(a2))==atermpp::aterm_appl()))        // true if a2 does not occur in a1.
      {
        v = a2;
        e = a1;
        return true;
      }
    }
  }

  return false;
}

void EnumeratorSolutionsStandard::EliminateVars(fs_expr &e)
{
  variable_list vars = e.vars();
  variable_list substituted_vars = e.substituted_vars();
  atermpp::term_list< atermpp::aterm_appl > vals = e.vals();
  atermpp::aterm_appl expr = e.expr();

  variable var;
  atermpp::aterm_appl val;
  while (!vars.empty() && FindInnerCEquality(expr,vars,var,val))
  {
    vars = (variable_list)ATremoveElement((ATermList)vars, (ATerm)(ATermAppl)var);
    m_enclosing_enumerator->rewr_obj->setSubstitutionInternal((ATermAppl) var,(ATerm)(ATermAppl)val);
    substituted_vars=push_front(substituted_vars,var);
    vals = push_front(vals,val);
    expr = (atermpp::aterm_appl)m_enclosing_enumerator->rewr_obj->rewriteInternal((ATerm)(ATermAppl)expr);
    m_enclosing_enumerator->rewr_obj->clearSubstitution((ATermAppl) var);
  }

  e=fs_expr(vars,substituted_vars,vals,expr);
}

atermpp::aterm_appl EnumeratorSolutionsStandard::build_solution_single(
                 const atermpp::aterm_appl t,                  
                 variable_list substituted_vars,
                 atermpp::term_list < atermpp::aterm_appl> exprs) const
{
  assert(substituted_vars.size()==exprs.size());
  while (!substituted_vars.empty() && t!=substituted_vars.front())
  {
    substituted_vars=pop_front(substituted_vars); 
    exprs=pop_front(exprs); 
  }

  if (substituted_vars.empty())
  {
    return t;
  }
  else
  {
    return build_solution_aux_innerc(exprs.front(),pop_front(substituted_vars),pop_front(exprs));
  }
}

atermpp::aterm_appl EnumeratorSolutionsStandard::build_solution_aux_innerc(
                 const atermpp::aterm_appl t,
                 const variable_list substituted_vars,
                 const atermpp::term_list < atermpp::aterm_appl> exprs) const
{
  if (is_variable(t))
  {
    return build_solution_single(t,substituted_vars,exprs);
  }
  else
  {
    atermpp::aterm_appl head = t(0);
    size_t arity = t.size();
    size_t extra_arity = 0;

    if (!ATisInt((ATerm)(ATermAppl)head))
    {
      head = build_solution_single(head,substituted_vars,exprs);
      if (!is_variable(head))
      {
        extra_arity = head.size()-1; 
      }
    }

    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,ATermAppl,arity+extra_arity);
    AFun fun = ATgetAFun((ATermAppl) t);
    size_t k = 1;

    if (!ATisInt((ATerm)(ATermAppl)head) && !is_variable((ATermAppl) head))
    {
      fun = ATmakeAFun("@appl_bs@",arity+extra_arity,false);
      k = extra_arity+1;
      for (size_t i=1; i<k; i++)
      {
        args[i] = (ATermAppl)(ATerm)head(i);
      }
      head = head(0);
    }

    args[0] = head;
    for (size_t i=1; i<arity; i++,k++)
    {
      args[k] = (ATermAppl)build_solution_aux_innerc(t(i),substituted_vars,exprs);
    }

    atermpp::aterm_appl r = ATmakeApplArray(fun,(ATerm *)args);
    return r;
  }
}

atermpp::term_list < atermpp::aterm_appl> EnumeratorSolutionsStandard::build_solution2(
                 const variable_list vars, 
                 const variable_list substituted_vars, 
                 const atermpp::term_list < atermpp::aterm_appl> exprs) const
{
  if (vars.empty())
  {
    return atermpp::term_list < atermpp::aterm_appl>();
  }
  else
  {
    return push_front(build_solution2(pop_front(vars),substituted_vars,exprs),
           (atermpp::aterm_appl)m_enclosing_enumerator->rewr_obj->rewriteInternal((ATerm)(ATermAppl)build_solution_single(vars.front(),substituted_vars,exprs)));
  }
}

atermpp::term_list < atermpp::aterm_appl> EnumeratorSolutionsStandard::build_solution(
                 const variable_list vars, 
                 const variable_list substituted_vars, 
                 const atermpp::term_list < atermpp::aterm_appl> exprs) const
{
  return build_solution2(vars,reverse(substituted_vars),reverse(exprs));
}

bool EnumeratorSolutionsStandard::next(
              bool &solution_is_exact,
              atermpp::term_list<atermpp::aterm_appl> &solution,
              bool &solution_possible)
{
  while (ss_stack.empty() && !fs_stack.empty())
  {
    fs_expr e=fs_stack.front();
    EliminateVars(e);
    fs_stack.pop_front();

    if (e.vars().empty() || e.expr()==m_enclosing_enumerator->rewr_false) 
    { 
      if (e.expr()!=m_enclosing_enumerator->rewr_false) // So e.vars() is empty.
      {
        ss_stack.push_back(
                       ss_solution(build_solution(
                                        enum_vars,e.substituted_vars(),
                                        e.vals()),
                                   e.expr()==m_enclosing_enumerator->rewr_true));
      }
    }
    else 
    {
      assert(!e.vars().empty());
      assert(e.expr()!=m_enclosing_enumerator->rewr_false);
      const variable var = e.vars().front();
      const sort_expression sort = var.sort();
      variable_list uvars = pop_front(e.vars());

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
          throw mcrl2::runtime_error("cannot enumerate elements of the function sort " + pp(sort));
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
          throw mcrl2::runtime_error("cannot enumerate elements of a bag of sort " + pp(sort));
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
            throw mcrl2::runtime_error("cannot enumerate all elements of a set of sort " + pp(sort));
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
            throw mcrl2::runtime_error("cannot enumerate elements of sort " + pp(sort) + " as it does not have constructor functions");
          }
        }
  
        assert(!it->empty());
        for( ; it!=constructors_for_sort.end() ; ++it)
        {
          // Construct the domain and target sort for the constructor.
          sort_expression target_sort=it->sort();
          sort_expression_list domain_sorts;
          if (is_function_sort(target_sort))
          { 
            domain_sorts=function_sort(target_sort).domain();
            target_sort=function_sort(target_sort).codomain();
          }
          assert(target_sort==sort);
  
          variable_list var_list;
          for (sort_expression_list::const_iterator i=domain_sorts.begin(); i!=domain_sorts.end(); ++i) 
          {
            char Name[40];
            static size_t fresh_variable_index=0;
            sprintf(Name, "@enum@%ld", fresh_variable_index++);
            const variable fv = variable(gsMakeDataVarId(gsFreshString2ATermAppl(Name,(ATerm)ATempty,true),*i));
            var_list = push_front(var_list,fv);
  
            used_vars++;
            if (m_max_internal_variables!=0 && used_vars > m_max_internal_variables)
            {
              if (solution_possible)
              {
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
                  exception_message << pp(*k) << ":" << pp(k->sort());
                }
                exception_message << " that satisfy " << pp(m_enclosing_enumerator->rewr_obj->fromRewriteFormat((ATerm)(ATermAppl)enum_expr));
                throw mcrl2::runtime_error(exception_message.str());
              }
            }
            else if (used_vars > max_vars) 
            {
              cerr << "need more than " << max_vars << " variables to find all valuations of ";
              for (variable_list::const_iterator k=enum_vars.begin(); k!=enum_vars.end(); ++k)
              {
                if (k != enum_vars.begin())
                {
                  cerr << ", ";
                }
                cerr << pp(*k) << ":" << pp(k->sort());
              }
              cerr << " that satisfy " << pp(m_enclosing_enumerator->rewr_obj->fromRewriteFormat((ATerm)(ATermAppl)enum_expr)) << endl;
              max_vars *= MAX_VARS_FACTOR;
            }
          }
          data_expression cons_term;
          if (var_list.empty())
          { 
            cons_term=*it;
          }
          else
          {
            cons_term = application(*it, reverse(var_list));
          }
          atermpp::aterm_appl term_rf = m_enclosing_enumerator->rewr_obj->rewriteInternal(m_enclosing_enumerator->rewr_obj->toRewriteFormat(cons_term));
  
          m_enclosing_enumerator->rewr_obj->setSubstitutionInternal(var,(ATerm)(ATermAppl)term_rf);
          /* ATerm new_expr = m_enclosing_enumerator->rewr_obj->rewriteInternal((ATerm)(ATermAppl)e.expr());
  
          if (new_expr!=m_enclosing_enumerator->rewr_false) 
          { */
            push_on_fs_stack_and_split_or(
                                  fs_stack,
                                  uvars+var_list,
                                  push_front(e.substituted_vars(),var),
                                  push_front(e.vals(),term_rf),
                                  (atermpp::aterm_appl)e.expr(),
                                  atermpp::term_list < atermpp::aterm_appl > (),
                                  false); 
          // }
          m_enclosing_enumerator->rewr_obj->clearSubstitution(var);
        }
      }
    }
  }
  if (!ss_stack.empty())
  {
    solution = ss_stack.back().solution();
    solution_is_exact = ss_stack.back().solution_is_exact();
    ss_stack.pop_back();
    return true;
  }
  else
  {
    return false;
  }
}

bool EnumeratorSolutionsStandard::next(
          atermpp::term_list<atermpp::aterm_appl> &solution)
{
  bool dummy_solution_is_exact;
  return next(dummy_solution_is_exact,solution);
}

bool EnumeratorSolutionsStandard::next(
          bool &solution_is_exact,
          atermpp::term_list<atermpp::aterm_appl> &solution)
{
  bool dummy_solution_possible=false;
  return next(solution_is_exact,solution,dummy_solution_possible); 

}

bool EnumeratorSolutionsStandard::next(
          atermpp::term_list<atermpp::aterm_appl> &solution, 
          bool &solution_possible)
{
  bool dummy_solution_is_exact;
  return next(dummy_solution_is_exact,solution,solution_possible);
}

void EnumeratorSolutionsStandard::reset(const bool not_equal_to_false)
{
  push_on_fs_stack_and_split_or(fs_stack,
                                enum_vars,
                                variable_list(),
                                atermpp::term_list< atermpp::aterm_appl>(),
                                enum_expr,
                                atermpp::term_list< atermpp::aterm_appl>(),
                                !not_equal_to_false);
}

EnumeratorStandard::EnumeratorStandard(const mcrl2::data::data_specification &data_spec, Rewriter* r): 
  m_data_spec(data_spec)
{
  rewr_obj = r;

  rewr_true=NULL;
  rewr_true.protect();
  rewr_true = (atermpp::aterm_appl)rewr_obj->toRewriteFormat(sort_bool::true_());
  rewr_false=NULL;
  rewr_false.protect();
  rewr_false = (atermpp::aterm_appl)rewr_obj->toRewriteFormat(sort_bool::false_());

  opidAnd.protect();
  opidOr.protect();
  opidNot.protect();
  atermpp::aterm_appl t_and=rewr_obj->toRewriteFormat(sort_bool::and_());
  opidAnd = t_and(0);
  atermpp::aterm_appl t_or=rewr_obj->toRewriteFormat(sort_bool::or_());
  opidOr = t_or(0);
  atermpp::aterm_appl t_not=rewr_obj->toRewriteFormat(sort_bool::not_());
  opidNot = t_not(0);

  const function_symbol_vector mappings(data_spec.mappings());
  for (function_symbol_vector::const_iterator i = mappings.begin(); i != mappings.end(); ++i)
  {
    if (i->name() == "==")
    {
      atermpp::aterm_appl t=rewr_obj->toRewriteFormat(*i);
      eqs.insert(t(0));
    }
  }
}

EnumeratorStandard::~EnumeratorStandard()
{
  rewr_true.unprotect();
  rewr_false.unprotect();

  opidAnd.unprotect();
  opidOr.unprotect();
  opidNot.unprotect();
}

} // namespace detail
} // namespace data
} // namespace mcrl2
