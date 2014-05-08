// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_DATA_DETAIL_ENUM_ENUMERATOR_IMPLEMENTATION_H
#define MCRL2_DATA_DETAIL_ENUM_ENUMERATOR_IMPLEMENTATION_H

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/core/term_traits.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/detail/enum/standard.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

// Ugly add hoc class. Ought to be replaced when lambda notation can be used.
class test_equal
{
  const atermpp::aterm_appl& m_term;

  public:
    test_equal(const data_expression& t) : m_term(t)
    {}

    bool operator ()(const atermpp::aterm_appl& other) const
    {
      return m_term==other;
    }
};

template <class TERM, class REWRITER>
inline TERM EnumeratorSolutionsStandard<TERM,REWRITER>::add_negations(
                                const TERM& condition,
                                const data_expression_list& negation_term_list,
                                const bool negated) const
{ /* If negation_term_list is [t1,...,tn], generate an expression of the form
     condition /\ !t1 /\ !t2 /\ ... /\ !tn. Using ad hoc
     rewriting is cheaper than using a full fledged rewriter, that will
     again normalise all subterms.
  */

  typedef core::term_traits<TERM> tr;

  if (negation_term_list.empty())
  {
    if (negated)
    {
      if (condition == sort_bool::true_())
      {
        return sort_bool::false_();
      }
      else if (condition == sort_bool::false_())
      {
        return sort_bool::true_();
      }
      else if (tr::is_not(condition))
      {
        return tr::not_arg(condition);
      }
      return tr::not_(condition);
    }
    return condition;
  }

  const data_expression first_argument=add_negations(condition,negation_term_list.tail(),negated);
  data_expression second_argument= negation_term_list.front();
  if (!negated)
  {
    if (second_argument == sort_bool::true_())
    {
      return sort_bool::false_();
    }
    else if (second_argument == sort_bool::false_())
    {
      return sort_bool::true_();
    }
    else if (is_application(second_argument) && core::down_cast<application>(second_argument).head()== sort_bool::not_())
    {
      second_argument=core::down_cast<application>(second_argument)[0];
    }
    else
    {
      second_argument=application(sort_bool::not_(),second_argument);
    }
  }

  if (first_argument==sort_bool::true_())
  {
    return second_argument;
  }
  else if (first_argument==sort_bool::false_())
  {
    return sort_bool::false_();
  }
  if (second_argument==sort_bool::true_())
  {
    return first_argument;
  }
  else if (second_argument==sort_bool::false_())
  {
    return sort_bool::false_();
  }
  else
  {
    return application(sort_bool::and_(), first_argument, second_argument);
  }
}

template <class TERM, class REWRITER>
inline data_expression_list EnumeratorSolutionsStandard<TERM,REWRITER>::negate(const data_expression_list& l) const
{
  if (l.empty())
  {
    return l;
  }
  data_expression_list result=negate(l.tail());
  result.push_front(application(sort_bool::not_(),l.front()));
  return result;
}

template <class TERM, class REWRITER>
inline void EnumeratorSolutionsStandard<TERM,REWRITER>::push_on_fs_stack_and_split_or_without_rewriting(
                                std::deque < fs_expr<TERM> >& fs_stack,
                                const variable_list& var_list,
                                const variable_list& substituted_vars,
                                const data_expression_list& substitution_terms,
                                const TERM& condition,
                                const data_expression_list& negated_term_list,
                                const bool negated) const
{
  /* If the negated_term_list equals t1,...,tn, store condition /\ !t1 /\ !t2 /\ ... /\ !tn
     on the fs_stack.  If the condition to be stored on the fs_stack has the shape phi \/ psi, then
     store phi and psi /\ !phi separately. This allows the equality eliminator to remove
     more equalities and therefore be more effective. */
  if (is_application(condition))
  {
    const application& ca = core::down_cast<application>(condition);
    if (ca.head() == sort_bool::not_())
    {
      push_on_fs_stack_and_split_or_without_rewriting(fs_stack,var_list,substituted_vars,substitution_terms,ca[0],negate(negated_term_list),!negated);
      return;
    }
    if ((negated && ca.head() == sort_bool::and_()) ||
             (!negated && ca.head() == sort_bool::or_()))
    {
      assert(condition.size()==3);
      push_on_fs_stack_and_split_or_without_rewriting(fs_stack,var_list,substituted_vars,substitution_terms,ca[0],negated_term_list,negated);
      data_expression_list temp=negated_term_list;
      temp.push_front(ca[0]);
      push_on_fs_stack_and_split_or_without_rewriting(fs_stack,var_list,substituted_vars,substitution_terms,ca[1], temp,negated);
      return;
    }
  }

  const TERM new_expr = add_negations(condition,negated_term_list,negated);

  if (new_expr!=sort_bool::false_())
  {
#ifndef NDEBUG
    // Check that substituted variables do not occur in the expression expr.
    std::set <variable> s=data::find_free_variables(new_expr);
    for(std::set <variable>::const_iterator it=s.begin(); it!=s.end(); ++it)
    {
      assert(std::find(substituted_vars.begin(),substituted_vars.end(),*it)==substituted_vars.end());
    }
#endif

    fs_stack.push_back(fs_expr<TERM>(var_list,
                               substituted_vars,
                               substitution_terms,
                               new_expr));
  }
}

template <class TERM, class REWRITER>
inline void EnumeratorSolutionsStandard<TERM,REWRITER>::push_on_fs_stack_and_split_or(
                                std::deque < fs_expr<TERM> >& fs_stack,
                                const variable_list& var_list,
                                const variable_list& substituted_vars,
                                const data_expression_list& substitution_terms,
                                const TERM& condition,
                                const data_expression_list& negated_term_list,
                                const bool negated) const
{
  EnumeratorSolutionsStandard<TERM,REWRITER>::push_on_fs_stack_and_split_or_without_rewriting(
                                fs_stack,
                                var_list,
                                substituted_vars,
                                substitution_terms,
                                m_rewr_obj->rewrite(condition,enum_sigma),
                                negated_term_list,
                                negated);
}

template <class TERM, class REWRITER>
inline bool EnumeratorSolutionsStandard<TERM,REWRITER>::find_equality(
                        const data_expression& t,
                        const mcrl2::data::variable_list& vars,
                        mcrl2::data::variable& v,
                        data_expression& e)
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

  const application& ta = core::down_cast<application>(t);

  if (is_function_symbol(ta.head()))
  {
    const function_symbol& f = core::down_cast<function_symbol>(ta.head());
    if (f == sort_bool::and_())
    {
      assert(ta.size()==2);
      return find_equality(ta[0],vars,v,e) || find_equality(ta[1],vars,v,e);
    }
    else if (static_cast<const std::string&>(f.name()) == "==")
    {
      const data_expression& a1 = ta[0];
      const data_expression& a2 = ta[1];
      if (a1!=a2)
      {
        if (is_variable(a1) && (std::find(vars.begin(),vars.end(),variable(a1))!=vars.end()) &&
                          (atermpp::find_if(static_cast<const atermpp::aterm_appl&>(a2),test_equal(a1))==atermpp::aterm_appl()))        // true if a1 does not occur in a2.
        {
          v = core::down_cast<variable>(a1);
          e = a2;
          return true;
        }
        if (is_variable(a2) && (std::find(vars.begin(),vars.end(),variable(a2))!=vars.end()) &&
                                 (atermpp::find_if(a1,test_equal(a2))==atermpp::aterm_appl()))        // true if a2 does not occur in a1.
        {
          v = core::down_cast<variable>(a2);
          e = a1;
          return true;
        }
      }
    }
  }

  return false;
}

template <class TERM, class REWRITER>
inline void EnumeratorSolutionsStandard<TERM,REWRITER>::EliminateVars(fs_expr<TERM>& e)
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
    expr = m_rewr_obj->rewrite(expr,enum_sigma);
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
  e=fs_expr<TERM>(vars,substituted_vars,vals,expr);
}

static data_expression build_solution_aux(
                 const data_expression& t,
                 const variable_list& substituted_vars,
                 const data_expression_list& exprs);

static data_expression build_solution_single(
                 const variable& t,
                 variable_list substituted_vars,
                 data_expression_list exprs)
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

class apply_build_solution_aux
{
  protected:
    const variable_list& m_substituted_vars;
    const data_expression_list& m_expr;

  public:
    apply_build_solution_aux(const variable_list& substituted_vars, const data_expression_list& expr):
       m_substituted_vars(substituted_vars), m_expr(expr)
    {}

    data_expression operator()(const data_expression& t) const
    {
      return build_solution_aux(t,m_substituted_vars,m_expr);
    }
};

static data_expression build_solution_aux(
                 const data_expression& t,
                 const variable_list& substituted_vars,
                 const data_expression_list& exprs)
{
  assert(!is_where_clause(t)); // This is a non expected case as t is a normalform.
  if (is_variable(t))
  {
    return build_solution_single(atermpp::aterm_cast<variable>(t),substituted_vars,exprs);
  }
  else if (is_abstraction(t))
  {
    const abstraction& t1=core::down_cast<abstraction>(t);
    const binder_type& binder=t1.binding_operator();
    const variable_list& bound_variables=t1.variables();
    const data_expression& body=build_solution_aux(t1.body(),substituted_vars,exprs);
    return abstraction(binder,bound_variables,body);
  }
  else if (is_function_symbol(t))
  {
    return t;
  }

  assert(is_application(t));
  {
    // t has the shape application(u1,...,un)
    const application t_appl(t);
    const data_expression& head = t_appl.head();

    if (is_function_symbol(head))
    {
      return application(head,t_appl.begin(),t_appl.end(),apply_build_solution_aux(substituted_vars,exprs));
    }

    /* The head is more complex, rewrite it first; */

    data_expression head1 = build_solution_aux(head,substituted_vars,exprs);
    return application(head1,t_appl.begin(),t_appl.end(),apply_build_solution_aux(substituted_vars,exprs));
  }
}

template <class TERM, class REWRITER>
inline data_expression_list EnumeratorSolutionsStandard<TERM,REWRITER>::build_solution2(
                 const variable_list& vars,
                 const variable_list& substituted_vars,
                 const data_expression_list& exprs) const
{
  if (vars.empty())
  {
    return data_expression_list();
  }
  else
  {
    data_expression_list result=build_solution2(vars.tail(),substituted_vars,exprs);
    result.push_front(m_rewr_obj->rewrite(build_solution_single(vars.front(),substituted_vars,exprs),enum_sigma));
    return result;
  }
}

template <class TERM, class REWRITER>
inline data_expression_list EnumeratorSolutionsStandard<TERM,REWRITER>::build_solution(
                 const variable_list& vars,
                 const variable_list& substituted_vars,
                 const data_expression_list& exprs) const
{
  return build_solution2(vars, atermpp::reverse(substituted_vars), atermpp::reverse(exprs));
}

template <class TERM, class REWRITER>
inline bool EnumeratorSolutionsStandard<TERM,REWRITER>::next(
              data_expression& evaluated_condition,
              data_expression_list& solution,
              bool& solution_possible)
{
  data_expression_vector var_array; // TODO: locating var_array on stack is most likely more efficient.

  while (!fs_stack.empty())
  {
    fs_expr<TERM> e=fs_stack.front();
    EliminateVars(e);
    fs_stack.pop_front();
    if (e.vars().empty() || e.expr()==sort_bool::false_())
    {
      if (e.expr()!=sort_bool::false_())
      { // A solution is found. Construct and return it.
        solution = build_solution(enum_vars,e.substituted_vars(), e.vals());
        if (m_not_equal_to_false)
        {
          evaluated_condition = e.expr();
        }
        else
        {
          evaluated_condition = m_rewr_obj->rewrite(sort_bool::not_(e.expr()),enum_sigma);
        }
             
        return true;
      }
    }
    else
    {
      assert(!e.vars().empty());
      assert(e.expr()!=sort_bool::false_());
      const variable& var = e.vars().front();
      const sort_expression& sort = var.sort();
      const variable_list& uvars = e.vars().tail();


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
        const function_symbol_vector& constructors_for_sort = m_data_spec.constructors(sort);
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
          const sort_expression& it_sort=it->sort();
          if (is_function_sort(it_sort))
          {
            const sort_expression_list& domain_sorts=core::down_cast<function_sort>(it_sort).domain();
            assert(function_sort(it_sort).codomain()==sort);

            variable_list var_list;
            assert(var_array.size()==0);
            // var_array.push_back(OpId2Int(*it));
            // var_array.push_back(*it);

            for (sort_expression_list::const_iterator i=domain_sorts.begin(); i!=domain_sorts.end(); ++i)
            {
              const variable fv(m_rewr_obj->generator("@x@",false),*i);
              var_list.push_front(fv);
              var_array.push_back(fv);

              used_vars++;
              if (m_max_internal_variables!=0 && used_vars > m_max_internal_variables)
              {
                if (solution_possible && max_vars != 0)
                {
                  mCRL2log(log::debug)   << "Enumerating expression: "<< enum_expr << std::endl;
                  mCRL2log(log::warning) << "Terminated enumeration of variables because more than " << m_max_internal_variables << " are used.\n";
                  solution_possible=false;
                  return false;
                }
                else
                {
                  fs_stack.clear();
                  std::stringstream exception_message;
                  exception_message << "needed more than " << m_max_internal_variables << " variables to find all valuations of ";
                  for (variable_list::const_iterator k=enum_vars.begin(); k!=enum_vars.end(); ++k)
                  {
                    if (k != enum_vars.begin())
                    {
                      exception_message << ", ";
                    }
                    exception_message << *k << ":" << k->sort();
                  }
                  exception_message << " that satisfy " << enum_expr;
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
                mCRL2log(log::info) << " that satisfy " << enum_expr << std::endl;
                max_vars *= MAX_VARS_FACTOR;
              }
            }
            // Substitutions must contain normal forms.  term_rf is almost always a normal form, but this is
            // not guaranteed and must be guaranteed by rewriting it explicitly. In the line below enum_sigma has no effect, but
            // using it is much cheaper than using a default substitution.

            const data_expression term_rf = m_rewr_obj->rewrite(
                       application(*it,var_array),enum_sigma);
            var_array.clear();

            const data_expression old_substituted_value=enum_sigma(var);
            enum_sigma[var]=term_rf;
            const data_expression rewritten_expr=m_rewr_obj->rewrite(e.expr(),enum_sigma);
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
            // const data_expression term_rf = m_rewr_obj->rewrite(atermpp::aterm_appl(get_appl_afun_value(1),OpId2Int(*it)),enum_sigma);
            const data_expression term_rf = m_rewr_obj->rewrite(*it,enum_sigma);

            const data_expression old_substituted_value=enum_sigma(var);
            enum_sigma[var]=term_rf;
            const data_expression rewritten_expr=m_rewr_obj->rewrite(e.expr(),enum_sigma);

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
  /* There are no more solutions */
  return false;
}

template <class TERM, class REWRITER>
inline bool EnumeratorSolutionsStandard<TERM,REWRITER>::next(data_expression_list& solution)
{
  data_expression dummy_evaluated_condition;
  return next(dummy_evaluated_condition,solution);
}

template <class TERM, class REWRITER>
inline bool EnumeratorSolutionsStandard<TERM,REWRITER>::next(
          data_expression& evaluated_condition,
          data_expression_list& solution)
{
  bool dummy_solution_possible=false;
  return next(evaluated_condition,solution,dummy_solution_possible);

}

template <class TERM, class REWRITER>
inline bool EnumeratorSolutionsStandard<TERM,REWRITER>::next(
          data_expression_list& solution,
          bool& solution_possible)
{
  data_expression dummy_evaluated_condition;
  return next(dummy_evaluated_condition,solution,solution_possible);
}

template <class TERM, class REWRITER>
inline void EnumeratorSolutionsStandard<TERM,REWRITER>::reset(const bool expr_is_normal_form)
{
  if (expr_is_normal_form)
  {
    push_on_fs_stack_and_split_or_without_rewriting(fs_stack,
                                  enum_vars,
                                  variable_list(),
                                  data_expression_list(),
                                  enum_expr,
                                  data_expression_list(),
                                  !m_not_equal_to_false);
  }
  else
  {
    push_on_fs_stack_and_split_or(fs_stack,
                                  enum_vars,
                                  variable_list(),
                                  data_expression_list(),
                                  enum_expr,
                                  data_expression_list(),
                                  !m_not_equal_to_false);
  }
}

} // namespace detail
} // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_ENUM_ENUMERATOR_IMPLEMENTATION_H
