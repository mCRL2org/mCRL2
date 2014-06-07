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
#include "mcrl2/data/classic_enumerator.h"

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

}

template <class REWRITER, class MutableSubstitution>
inline typename REWRITER::term_type classic_enumerator<REWRITER, MutableSubstitution>::iterator::add_negations(
                                const typename REWRITER::term_type& condition,
                                const data_expression_list& negation_term_list,
                                const bool negated) const
{ /* If negation_term_list is [t1,...,tn], generate an expression of the form
     condition /\ !t1 /\ !t2 /\ ... /\ !tn. Using ad hoc
     rewriting is cheaper than using a full fledged rewriter, that will
     again normalise all subterms.
  */

  typedef core::term_traits<typename REWRITER::term_type> tr;

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

template <class REWRITER, class MutableSubstitution>
inline data_expression_list classic_enumerator<REWRITER, MutableSubstitution>::iterator::negate(const data_expression_list& l) const
{
  if (l.empty())
  {
    return l;
  }
  data_expression_list result=negate(l.tail());
  result.push_front(application(sort_bool::not_(),l.front()));
  return result;
}

template <class REWRITER, class MutableSubstitution>
inline void classic_enumerator<REWRITER, MutableSubstitution>::iterator::push_on_fs_stack_and_split_or_without_rewriting(
                                const enumerator_list_element_with_substitution<typename REWRITER::term_type>& partial_solution,
                                const data_expression_list& negated_term_list,
                                const bool negated) 
{
  /* If the negated_term_list equals t1,...,tn, store condition /\ !t1 /\ !t2 /\ ... /\ !tn
     on the fs_stack.  If the condition to be stored on the fs_stack has the shape phi \/ psi, then
     store phi and psi /\ !phi separately. This allows the equality eliminator to remove
     more equalities and therefore be more effective. */
 const variable_list& var_list=partial_solution.variables();
 const typename REWRITER::term_type& condition=partial_solution.expression();
  if (is_application(condition))
  {
    const application& ca = core::down_cast<application>(condition);
    if (ca.head() == sort_bool::not_())
    {
      push_on_fs_stack_and_split_or_without_rewriting(partial_solution_type(var_list,ca[0],partial_solution),negate(negated_term_list),!negated);
      return;
    }
    if ((negated && ca.head() == sort_bool::and_()) ||
             (!negated && ca.head() == sort_bool::or_()))
    {
      assert(condition.size()==3);
      push_on_fs_stack_and_split_or_without_rewriting(partial_solution_type(var_list,ca[0],partial_solution),negated_term_list,negated);
      data_expression_list temp=negated_term_list;
      temp.push_front(ca[0]);
      push_on_fs_stack_and_split_or_without_rewriting(partial_solution_type(var_list,ca[1],partial_solution),temp,negated);
      return;
    }
  }

  const typename REWRITER::term_type new_expr = add_negations(condition,negated_term_list,negated);

  if (new_expr!=sort_bool::false_())
  {
    fs_stack.emplace_back(partial_solution_type(var_list, new_expr, partial_solution));
  }
}

template <class REWRITER, class MutableSubstitution>
inline void classic_enumerator<REWRITER, MutableSubstitution>::iterator::push_on_fs_stack_and_split_or(
                                const enumerator_list_element_with_substitution<typename REWRITER::term_type>& partial_solution,
                                const data_expression_list& negated_term_list,
                                const bool negated)
{
  classic_enumerator<REWRITER, MutableSubstitution>::iterator::push_on_fs_stack_and_split_or_without_rewriting(
                                partial_solution,
                                negated_term_list,
                                negated);
}

template <class REWRITER, class MutableSubstitution>
inline bool classic_enumerator<REWRITER, MutableSubstitution>::iterator::find_equality(
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
                          (atermpp::find_if(static_cast<const atermpp::aterm_appl&>(a2),detail::test_equal(a1))==atermpp::aterm_appl()))        // true if a1 does not occur in a2.
        {
          v = core::down_cast<variable>(a1);
          e = a2;
          return true;
        }
        if (is_variable(a2) && (std::find(vars.begin(),vars.end(),variable(a2))!=vars.end()) &&
                                 (atermpp::find_if(a1,detail::test_equal(a2))==atermpp::aterm_appl()))        // true if a2 does not occur in a1.
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

template <class REWRITER, class MutableSubstitution>
inline void classic_enumerator<REWRITER, MutableSubstitution>::iterator::EliminateVars(partial_solution_type& e)
{
  variable var;
  data_expression val;

  while (!e.variables().empty() && find_equality(e.expression(),e.variables(),var,val))
  {
    // Use a rewrite here to remove occurrences of subexpressions the form t==t caused by
    // replacing in x==t the variable x by t.
    const data_expression old_val=(*enum_sigma)(var);
    (*enum_sigma)[var]=val;
    e=partial_solution_type(
                  remove_one_element(e.variables(),var),
                  m_enclosing_enumerator->m_evaluator(e.expression(),*enum_sigma),
                  e,var,val);
    (*enum_sigma)[var]=old_val;
  }
}

template <class REWRITER, class MutableSubstitution>
inline void classic_enumerator<REWRITER, MutableSubstitution>::iterator::find_next_solution(const bool pop_front_of_stack)
{
  if (pop_front_of_stack)
  {
    fs_stack.pop_front();
  }
  for( ; !fs_stack.empty() ; fs_stack.pop_front())
  {
    partial_solution_type& e=fs_stack.front();  // e is intensionally a reference into fs_stack.
    EliminateVars(e);
    if (e.variables().empty() || e.expression()==sort_bool::false_())
    {
      if (e.expression()!=sort_bool::false_())
      { 
        // A solution is found. 
        if (!m_not_equal_to_false)
        {
          e=partial_solution_type(e.variables(),m_enclosing_enumerator->m_evaluator(sort_bool::not_(e.expression()),*enum_sigma),e);
        }
        return;
      }
    }
    else
    {
      assert(!e.variables().empty());
      assert(e.expression()!=sort_bool::false_());
      const variable& var = e.variables().front();
      const sort_expression& sort = var.sort();
      const variable_list& uvars = e.variables().tail();


      if (is_function_sort(sort))
      {
        e.invalidate();
        if (m_enclosing_enumerator->m_throw_exceptions)
        {
          throw mcrl2::runtime_error("cannot enumerate elements of the function sort " + data::pp(sort));
        }
        return;
      }
      else if (sort_bag::is_bag(sort))
      {
        e.invalidate();
        if (m_enclosing_enumerator->m_throw_exceptions)
        {
          throw mcrl2::runtime_error("cannot enumerate elements of a bag of sort " + data::pp(sort));
        }
        return;
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
          e.invalidate();
          if (m_enclosing_enumerator->m_throw_exceptions)
          {
            throw mcrl2::runtime_error("cannot enumerate all elements of a set of sort " + data::pp(sort));
          }
          return;
        }
      }
      else
      {
        const function_symbol_vector& constructors_for_sort = m_enclosing_enumerator->m_data_spec.constructors(sort);
        function_symbol_vector::const_iterator it=constructors_for_sort.begin();

        if ( it == constructors_for_sort.end() )
        {
          e.invalidate();
          if (m_enclosing_enumerator->m_throw_exceptions)
          {
            throw mcrl2::runtime_error("cannot enumerate elements of sort " + data::pp(sort) + " as it does not have constructor functions");
          }
          return;
        }

        assert(!it->empty());
        for( ; it!=constructors_for_sort.end() ; ++it)
        {
          const sort_expression& it_sort=it->sort();
          if (is_function_sort(it_sort))
          {
            const sort_expression_list& domain_sorts=core::down_cast<function_sort>(it_sort).domain();
            assert(function_sort(it_sort).codomain()==sort);

            variable_list var_list;

            for (sort_expression_list::const_iterator i=domain_sorts.begin(); i!=domain_sorts.end(); ++i)
            {
              const variable fv(m_enclosing_enumerator->m_evaluator.identifier_generator()("@x@",false),*i);
              var_list.push_front(fv);

              used_vars++;
              if (m_enclosing_enumerator->m_max_internal_variables!=0 && used_vars > m_enclosing_enumerator->m_max_internal_variables)
              {
                if (!m_enclosing_enumerator->m_throw_exceptions && max_vars != 0)
                {
                  mCRL2log(log::debug)   << "Enumerating expression: "<< enum_expr << std::endl;
                  mCRL2log(log::warning) << "Terminated enumeration of variables because more than " << m_enclosing_enumerator->m_max_internal_variables << " are used.\n";
                  e.invalidate();
                  return;
                }
                else
                {
                  std::stringstream exception_message;
                  exception_message << "needed more than " << m_enclosing_enumerator->m_max_internal_variables << " variables to find all valuations of ";
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

            variable_list reverted_var_list=reverse(var_list);
            const data_expression term_rf = m_enclosing_enumerator->m_evaluator(
                       application(*it,reverted_var_list.begin(),reverted_var_list.end()),*enum_sigma);


            const data_expression old_substituted_value=(*enum_sigma)(var);
            (*enum_sigma)[var]=term_rf;
            const data_expression rewritten_expr=m_enclosing_enumerator->m_evaluator(e.expression(),*enum_sigma);
            (*enum_sigma)[var]=old_substituted_value;
            push_on_fs_stack_and_split_or_without_rewriting(partial_solution_type(uvars+var_list,rewritten_expr,e,var,term_rf),
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
            const data_expression term_rf = m_enclosing_enumerator->m_evaluator(*it,*enum_sigma);

            const data_expression old_substituted_value=(*enum_sigma)(var);
            (*enum_sigma)[var]=term_rf;
            const data_expression rewritten_expr=m_enclosing_enumerator->m_evaluator(e.expression(),*enum_sigma);

            (*enum_sigma)[var]=old_substituted_value;
            push_on_fs_stack_and_split_or_without_rewriting(partial_solution_type(uvars,rewritten_expr,e,var,term_rf),
                                    data_expression_list(),
                                    false);
          }
        }
      }
    }
  }
  /* There are no more solutions */
  return;
}

} // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_ENUM_ENUMERATOR_IMPLEMENTATION_H
