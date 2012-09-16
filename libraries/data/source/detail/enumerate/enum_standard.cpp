// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <sstream>
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/aterm_ext.h"
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
      else if (condition.type()==AT_APPL && condition(0) == m_enclosing_enumerator->rewr_obj->internal_not)
      {
        return condition(1);
      }
      return Apply1(atermpp::aterm(m_enclosing_enumerator->rewr_obj->internal_not), condition);
    }
    return condition;
  }

  const atermpp::aterm_appl first_argument=add_negations(condition,pop_front(negation_term_list),negated);
  atermpp::aterm_appl second_argument= negation_term_list.front();
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
    else if (second_argument(0) == m_enclosing_enumerator->rewr_obj->internal_not)
    {
      second_argument=second_argument(1);
    }
    else
    {
      second_argument=Apply1(m_enclosing_enumerator->rewr_obj->internal_not,second_argument);
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
    return  Apply2(m_enclosing_enumerator->rewr_obj->internal_and,
                first_argument,
                second_argument);
  }
}

atermpp::term_list< atermpp::aterm_appl > EnumeratorSolutionsStandard::negate(const atermpp::term_list< atermpp::aterm_appl > l) const
{
  if (l.empty())
  {
    return l;
  }
  return push_front(negate(pop_front(l)),
                    static_cast<atermpp::aterm_appl>(Apply1(
                                             m_enclosing_enumerator->rewr_obj->internal_not,
                                             l.front())));
}

void EnumeratorSolutionsStandard::push_on_fs_stack_and_split_or_without_rewriting(
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
  if (condition(0) == m_enclosing_enumerator->rewr_obj->internal_not)
  {
    push_on_fs_stack_and_split_or_without_rewriting(fs_stack,var_list,substituted_vars,substitution_terms,condition(1),negate(negated_term_list),!negated);
  }
  else if ((negated && condition(0) == m_enclosing_enumerator->rewr_obj->internal_and) ||
           (!negated && condition(0) == m_enclosing_enumerator->rewr_obj->internal_or))
  {
    assert(condition.size()==3);
    push_on_fs_stack_and_split_or_without_rewriting(fs_stack,var_list,substituted_vars,substitution_terms,condition(1),negated_term_list,negated);
    push_on_fs_stack_and_split_or_without_rewriting(fs_stack,var_list,substituted_vars,substitution_terms,condition(2),
                           push_front(negated_term_list,static_cast<atermpp::aterm_appl>(condition(1))),negated);
  }
  else
  {
    const atermpp::aterm_appl new_expr = add_negations(condition,negated_term_list,negated);

    if (new_expr!=m_enclosing_enumerator->rewr_obj->internal_false)
    {
#ifndef NDEBUG
      // Check that substituted variables do not occur in the expression expr.
      std::set <variable> s=data::find_free_variables(m_enclosing_enumerator->rewr_obj->fromRewriteFormat(new_expr));
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
  EnumeratorSolutionsStandard::push_on_fs_stack_and_split_or_without_rewriting(
                                fs_stack,
                                var_list,
                                substituted_vars,
                                substitution_terms,
                                m_enclosing_enumerator->rewr_obj->rewrite_internal(condition,enum_sigma),
                                negated_term_list,
                                negated);
}

bool EnumeratorSolutionsStandard::find_equality(
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

  if (is_abstraction(t) || is_where_clause(t))
  {
    return false;
  }

  if (t(0) == m_enclosing_enumerator->rewr_obj->internal_and)
  {
    assert(t.size()==3);
    return find_equality(t(1),vars,v,e) || find_equality(t(2),vars,v,e);
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

  while (!vars.empty() && find_equality(expr,vars,var,val))
  {
    vars = remove_one_element(vars, var);
    substituted_vars=push_front(substituted_vars,var);
    vals = push_front(vals,val);

    // Use a rewrite here to remove occurrences of subexpressions the form t==t caused by
    // replacing in x==t the variable x by t.
    const atermpp::aterm_appl old_val=enum_sigma(var);
    enum_sigma[var]=val;
    expr = m_enclosing_enumerator->rewr_obj->rewrite_internal(expr,enum_sigma);
    enum_sigma[var]=old_val;
  }

#ifndef NDEBUG
  // Check that substituted variables do not occur in the expression expr.

  std::set <variable> s=data::find_free_variables(m_enclosing_enumerator->rewr_obj->fromRewriteFormat(expr));
  for(std::set <variable>::const_iterator it=s.begin(); it!=s.end(); ++it)
  {
    assert(std::find(substituted_vars.begin(),substituted_vars.end(),*it)==substituted_vars.end());
  }

#endif
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
    return build_solution_aux(exprs.front(),pop_front(substituted_vars),pop_front(exprs));
  }
}

atermpp::aterm_appl EnumeratorSolutionsStandard::build_solution_aux(
                 const atermpp::aterm_appl t,
                 const variable_list substituted_vars,
                 const atermpp::term_list < atermpp::aterm_appl> exprs) const
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
    const atermpp::aterm_appl t1=t;
    const atermpp::aterm_appl binder=t1(0);
    const variable_list bound_variables=t1(1);
    const atermpp::aterm_appl body=build_solution_aux(t1(2),substituted_vars,exprs);
    return gsMakeBinder(binder,bound_variables,body);
  }
  else
  {
    // t has the shape #REWR#(u1,...,un)

    atermpp::aterm head = t(0);
    size_t arity = t.size();
    size_t extra_arity = 0;

    if (head.type()!=AT_INT)
    {
      head = build_solution_single(head,substituted_vars,exprs);
      if (!is_variable(head))
      {
        extra_arity = atermpp::aterm_appl(head).size()-1;
      }
    }

    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,atermpp::aterm,arity+extra_arity);
    size_t k = 1;

    if (head.type()!=AT_INT && !is_variable(head))
    {
      k = extra_arity+1;
      for (size_t i=1; i<k; i++)
      {
        args[i] = atermpp::aterm_appl(head)(i);
      }
      head = atermpp::aterm_appl(head)(0);
    }

    args[0] = head;
    for (size_t i=1; i<arity; i++,k++)
    {
      args[k] = build_solution_aux(t(i),substituted_vars,exprs);
    }

    atermpp::aterm_appl r = ApplyArray(arity+extra_arity,args);
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
               m_enclosing_enumerator->rewr_obj->rewrite_internal(build_solution_single(vars.front(),substituted_vars,exprs),enum_sigma));
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
              atermpp::aterm_appl &evaluated_condition,
              atermpp::term_list<atermpp::aterm_appl> &solution,
              bool &solution_possible)
{
  vector < atermpp::aterm > var_array; // This does not need to be a atermpp vector, as its content
                                       // is protected elsewhere.
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
          sort_expression target_sort=it->sort();
          sort_expression_list domain_sorts;
          if (is_function_sort(target_sort))
          {
            domain_sorts=function_sort(target_sort).domain();
            target_sort=function_sort(target_sort).codomain();
          }
          assert(target_sort==sort);

          variable_list var_list;
          assert(var_array.size()==0);
          var_array.push_back(OpId2Int(*it));
          
          for (sort_expression_list::const_iterator i=domain_sorts.begin(); i!=domain_sorts.end(); ++i)
          {
            const variable fv(m_enclosing_enumerator->rewr_obj->generator("@x@",false),*i);
            var_list = push_front(var_list,fv);
            var_array.push_back(fv);

            used_vars++;
            if (m_max_internal_variables!=0 && used_vars > m_max_internal_variables)
            {
              if (solution_possible && max_vars != 0)
              {
                mCRL2log(log::debug)   << "Enumerating expression: "<< data::pp(m_enclosing_enumerator->rewr_obj->fromRewriteFormat(enum_expr)) << std::endl;
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
                exception_message << " that satisfy " << data::pp(m_enclosing_enumerator->rewr_obj->fromRewriteFormat(enum_expr));
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
                cerr << data::pp(*k) << ":" << data::pp(k->sort());
              }
              cerr << " that satisfy " << data::pp(m_enclosing_enumerator->rewr_obj->fromRewriteFormat(enum_expr)) << endl;
              max_vars *= MAX_VARS_FACTOR;
            }
          }
          // Substitutions must contain normal forms.  term_rf is almost always a normal form, but this is
          // not guaranteed and must be guaranteed by rewriting it explicitly. In the line below enum_sigma has no effect, but
          // using it is much cheaper than using a default substitution.
          const atermpp::aterm_appl term_rf = m_enclosing_enumerator->rewr_obj->rewrite_internal(ApplyArray(domain_sorts.size()+1,&var_array[0]),enum_sigma);
          var_array.clear();

          const atermpp::aterm_appl old_substituted_value=enum_sigma(var);
          enum_sigma[var]=term_rf;
          const atermpp::aterm_appl rewritten_expr=m_enclosing_enumerator->rewr_obj->rewrite_internal(e.expr(),enum_sigma);
          enum_sigma[var]=old_substituted_value;
          push_on_fs_stack_and_split_or_without_rewriting(
                                  fs_stack,
                                  uvars+var_list,
                                  push_front(e.substituted_vars(),var),
                                  push_front(e.vals(),term_rf),
                                  rewritten_expr,
                                  atermpp::term_list < atermpp::aterm_appl > (),
                                  false);
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

bool EnumeratorSolutionsStandard::next(atermpp::term_list<atermpp::aterm_appl> &solution)
{
  atermpp::aterm_appl dummy_evaluated_condition;
  return next(dummy_evaluated_condition,solution);
}

bool EnumeratorSolutionsStandard::next(
          atermpp::aterm_appl &evaluated_condition,
          atermpp::term_list<atermpp::aterm_appl> &solution)
{
  bool dummy_solution_possible=false;
  return next(evaluated_condition,solution,dummy_solution_possible);

}

bool EnumeratorSolutionsStandard::next(
          atermpp::term_list<atermpp::aterm_appl> &solution,
          bool &solution_possible)
{
  atermpp::aterm_appl dummy_evaluated_condition;
  return next(dummy_evaluated_condition,solution,solution_possible);
}

void EnumeratorSolutionsStandard::reset(const bool not_equal_to_false,const bool expr_is_normal_form)
{
  if (expr_is_normal_form)
  {
    push_on_fs_stack_and_split_or_without_rewriting(fs_stack,
                                  enum_vars,
                                  variable_list(),
                                  atermpp::term_list< atermpp::aterm_appl>(),
                                  enum_expr,
                                  atermpp::term_list< atermpp::aterm_appl>(),
                                  !not_equal_to_false);
  }
  else
  {
    push_on_fs_stack_and_split_or(fs_stack,
                                  enum_vars,
                                  variable_list(),
                                  atermpp::term_list< atermpp::aterm_appl>(),
                                  enum_expr,
                                  atermpp::term_list< atermpp::aterm_appl>(),
                                  !not_equal_to_false);
  }
}

EnumeratorStandard::EnumeratorStandard(const mcrl2::data::data_specification &data_spec, Rewriter* r):
  m_data_spec(data_spec)
{
  rewr_obj = r;

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
}

} // namespace detail
} // namespace data
} // namespace mcrl2
