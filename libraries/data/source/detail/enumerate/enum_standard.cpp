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
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/print.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/alias.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/detail/enum/standard.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

#define MAX_VARS_INIT   1000
#define MAX_VARS_FACTOR 5

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

bool EnumeratorSolutionsStandard::FindInnerCEquality(
                        const atermpp::aterm_appl t,
                        const mcrl2::data::variable_list vars, 
                        mcrl2::data::variable &v, 
                        atermpp::aterm_appl &e)
{
  if (t(0) == m_enclosing_enumerator->opidAnd)
  {
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

    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,atermpp::aterm_appl,arity+extra_arity);
    AFun fun = ATgetAFun((ATermAppl) t);
    size_t k = 1;

    if (!ATisInt((ATerm)(ATermAppl)head) && !is_variable((ATermAppl) head))
    {
      fun = ATmakeAFun("@appl_bs@",arity+extra_arity,false);
      k = extra_arity+1;
      for (size_t i=1; i<k; i++)
      {
        args[i] = head(i);
      }
      head = head(0);
    }

    args[0] = head;
    for (size_t i=1; i<arity; i++,k++)
    {
      args[k] = build_solution_aux_innerc(t(i),substituted_vars,exprs);
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

// bool EnumeratorSolutionsStandard::next(atermpp::term_list<atermpp::aterm_appl> &solution, bool &solution_is_exact)
bool EnumeratorSolutionsStandard::next(
              bool &solution_is_exact,
              atermpp::term_list<atermpp::aterm_appl> &solution,
              bool &solution_possible)
{
  // There can only be one EnumeratorSolutionsStandard per EnumeratorStandard:
  assert(m_enclosing_enumerator->current_enumerator_count==current_enumerator_count); 

  while (m_enclosing_enumerator->ss_stack.empty() && !m_enclosing_enumerator->fs_stack.empty())
  {
    const fs_expr e=m_enclosing_enumerator->fs_stack.front();
    m_enclosing_enumerator->fs_stack.pop_front();
    assert(!e.vars().empty());

    variable_list variables_in_use=e.vars();
    const variable var = e.vars().front();
    const sort_expression sort = var.sort();
    variable_list uvars = pop_front(e.vars());

    if (is_function_sort(sort))
    {
      // HIER MOETEN FUNCTIETERMEN WORDEN TOEGEVOEGD.
      if (solution_possible)
      {
        solution_possible=false;
        return false;
      }
      else
      { 
        m_enclosing_enumerator->fs_stack.clear();
        throw mcrl2::runtime_error("cannot enumerate all elements of function sort " + pp(sort));
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
          m_enclosing_enumerator->fs_stack.clear(); 
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
          const variable fv = variable(gsMakeDataVarId(gsFreshString2ATermAppl("@enum@",(ATerm)(ATermList)variables_in_use,false),*i));
          var_list = push_front(var_list,fv);
          variables_in_use=push_front(variables_in_use,fv);

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
              m_enclosing_enumerator->fs_stack.clear();
              stringstream exception_message;
              exception_message << "needed more than " << m_max_internal_variables << " variables to find all valuations of ";
              for (variable_list::const_iterator k=m_enclosing_enumerator->enum_vars.begin(); k!=m_enclosing_enumerator->enum_vars.end(); ++k)
              {
                if (k != m_enclosing_enumerator->enum_vars.begin())
                {
                  exception_message << ", ";
                }
                exception_message << pp(*k) << ":" << pp(k->sort());
              }
              exception_message << " that satisfy " << pp(m_enclosing_enumerator->rewr_obj->fromRewriteFormat((ATerm)(ATermAppl)m_enclosing_enumerator->enum_expr));
              throw mcrl2::runtime_error(exception_message.str());
            }
          }
          else if (used_vars > max_vars) 
          {
            cerr << "need more than " << max_vars << " variables to find all valuations of ";
            for (variable_list::const_iterator k=m_enclosing_enumerator->enum_vars.begin(); k!=m_enclosing_enumerator->enum_vars.end(); ++k)
            {
              if (k != m_enclosing_enumerator->enum_vars.begin())
              {
                cerr << ", ";
              }
              cerr << pp(*k) << ":" << pp(k->sort());
            }
            cerr << " that satisfy " << pp(m_enclosing_enumerator->rewr_obj->fromRewriteFormat((ATerm)(ATermAppl)m_enclosing_enumerator->enum_expr)) << endl;
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
        ATerm new_expr = m_enclosing_enumerator->rewr_obj->rewriteInternal((ATerm)(ATermAppl)e.expr());

        if (new_expr!=forbidden_truth_value) 
        {
          m_enclosing_enumerator->fs_stack.push_back(fs_expr(
                                uvars+var_list,
                                push_front(e.substituted_vars(),var),
                                push_front(e.vals(),term_rf),
                                (atermpp::aterm_appl)new_expr));
          if ((m_enclosing_enumerator->fs_stack.back().vars().empty()) || 
                   (EliminateVars(m_enclosing_enumerator->fs_stack.back()), (m_enclosing_enumerator->fs_stack.back().vars().empty()))) 
          {
            m_enclosing_enumerator->ss_stack.push_back(
                       ss_solution(build_solution(
                                        m_enclosing_enumerator->enum_vars,m_enclosing_enumerator->fs_stack.back().substituted_vars(),
                                        m_enclosing_enumerator->fs_stack.back().vals()),
                                   m_enclosing_enumerator->fs_stack.back().expr()==desired_truth_value));
            m_enclosing_enumerator->fs_stack.pop_back();
          }
        }
        m_enclosing_enumerator->rewr_obj->clearSubstitution(var);
      }
    }
  }
  if (!m_enclosing_enumerator->ss_stack.empty())
  {
    solution = m_enclosing_enumerator->ss_stack.back().solution();
    solution_is_exact = m_enclosing_enumerator->ss_stack.back().solution_is_exact();
    m_enclosing_enumerator->ss_stack.pop_back();
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

void EnumeratorSolutionsStandard::reset(const variable_list &Vars, const atermpp::aterm_appl &Expr, const bool not_equal_to_false)
{
  m_enclosing_enumerator->enum_vars = Vars;
  m_enclosing_enumerator->enum_expr = (atermpp::aterm_appl)m_enclosing_enumerator->rewr_obj->rewriteInternal((ATerm)(ATermAppl)Expr);
  // m_not_equal_to_false = netf;
  if (not_equal_to_false)
  {
    desired_truth_value=m_enclosing_enumerator->rewr_true;
    forbidden_truth_value=m_enclosing_enumerator->rewr_false;
  }
  else
  {
    desired_truth_value=m_enclosing_enumerator->rewr_false;
    forbidden_truth_value=m_enclosing_enumerator->rewr_true;
  }

  m_enclosing_enumerator->fs_stack.clear();

  used_vars = 0;
  max_vars = MAX_VARS_INIT;

  m_enclosing_enumerator->fs_stack.push_back(fs_expr(m_enclosing_enumerator->enum_vars,variable_list(),atermpp::term_list< atermpp::aterm_appl>(),m_enclosing_enumerator->enum_expr));
  if (!m_enclosing_enumerator->enum_vars.empty())
  {
    EliminateVars(m_enclosing_enumerator->fs_stack.front());
  }

  if (m_enclosing_enumerator->fs_stack.front().expr()==forbidden_truth_value)
  {
    m_enclosing_enumerator->fs_stack.pop_back();
  }
  else if (m_enclosing_enumerator->fs_stack.front().vars().empty())
  {
    if (m_enclosing_enumerator->fs_stack.front().expr()!=desired_truth_value) 
    {
      throw mcrl2::runtime_error("term does not evaluate to true or false " +
                           pp(m_enclosing_enumerator->rewr_obj->fromRewriteFormat((ATerm)(ATermAppl)m_enclosing_enumerator->fs_stack.front().expr())));
    }
    else
    {
      m_enclosing_enumerator->ss_stack.push_back(
                                  ss_solution(
                                        build_solution(
                                            m_enclosing_enumerator->enum_vars,
                                            m_enclosing_enumerator->fs_stack.front().substituted_vars(),
                                            m_enclosing_enumerator->fs_stack.front().vals()),
                                        true));
    }
    m_enclosing_enumerator->fs_stack.pop_back();
  }
}

EnumeratorStandard::EnumeratorStandard(const mcrl2::data::data_specification &data_spec, Rewriter* r): 
#ifndef NDEBUG
  t current_enumerator_count(0),
#endif
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
  if ((rewr_obj->getStrategy() == GS_REWR_INNER) || (rewr_obj->getStrategy() == GS_REWR_INNER_P))
  {
    throw mcrl2::runtime_error("The classic enumerator does not work (anymore) with the INNER and INNER_P rewriter.");
  }
  else
  {
    atermpp::aterm_appl t=rewr_obj->toRewriteFormat(sort_bool::and_());
    opidAnd = t(0);

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
}

EnumeratorStandard::~EnumeratorStandard()
{
  rewr_true.unprotect();
  rewr_false.unprotect();

  opidAnd.unprotect();

  /* if (clean_up_rewr_obj)
  {
    delete rewr_obj;
  } */
}

/* EnumeratorSolutionsStandard *EnumeratorStandard::findSolutions(
               const variable_list vars, 
               const atermpp::aterm_appl expr, 
               const bool not_equal_to_false, EnumeratorSolutionsStandard* old)
{
  if (old == NULL)
  {
    return new EnumeratorSolutionsStandard(vars,expr,not_equal_to_false,*this);
  }
  else
  {
    ((EnumeratorSolutionsStandard*) old)->reset(vars,expr,not_equal_to_false);
    return old;
  }
} */

} // namespace detail
} // namespace data
} // namespace mcrl2
