// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_DATA_DETAIL_ENUM_ENUMERATOR_IMPLEMENTATION_H
#define MCRL2_DATA_DETAIL_ENUM_ENUMERATOR_IMPLEMENTATION_H

#include <cmath>
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

inline data_expression make_set(size_t function_index,
                                const sort_expression& element_sort,
                                const data_expression_vector& set_elements)
{
 data_expression result=sort_fset::empty(element_sort);
 for(data_expression_vector::const_iterator i=set_elements.begin(); i!=set_elements.end(); ++i)
 {
   if (function_index % 2==1)
   {
     result=sort_fset::insert(element_sort,*i,result);
   }
   function_index = function_index /2;
 }
 return result;
}


inline data_expression make_if_expression(size_t& function_index,
                                          const size_t argument_index,
                                          const std::vector < data_expression_vector >& data_domain_expressions,
                                          const data_expression_vector& codomain_expressions,
                                          const variable_vector& parameters)
{
 if (argument_index==data_domain_expressions.size())
 {
   size_t result_expression_index=function_index % codomain_expressions.size();
   function_index= function_index / codomain_expressions.size();

   return codomain_expressions[result_expression_index];
 }

 data_expression result;
 const data_expression_vector& current_enumerated_elements=data_domain_expressions[argument_index];
 for(data_expression_vector::const_reverse_iterator i=current_enumerated_elements.rbegin(); i!=current_enumerated_elements.rend(); ++i)
 {
   if (i==current_enumerated_elements.rbegin())
   {
     result=make_if_expression(function_index,argument_index+1,data_domain_expressions,codomain_expressions,parameters);
   }
   else
   {
     const data_expression lhs=make_if_expression(function_index,argument_index+1,data_domain_expressions,codomain_expressions,parameters);
     if (lhs!=result) // Optimize: if the lhs and rhs are equal, return the rhs.
     { result=if_(equal_to(parameters[argument_index],*i),lhs,result);
     }
   }
 }

 return result;
}

}

/// \brief This function delivers a vector with all elements of sort s.
/// \detail It is assumed that the sort s has only a finite number of elements.
// TODO: This function should probably be moved to some utility library, and
//       some applications of the enumerators can be replaced by using this code.
//
template <class REWRITER>
data_expression_vector get_all_expressions(const sort_expression& s, const data_specification& data_spec, const REWRITER& rewr)
{
  assert(data_spec.is_certainly_finite(s));
  typedef classic_enumerator<REWRITER, typename REWRITER::substitution_type> enumerator_type;
  enumerator_type enumerator(rewr, data_spec);
  data_expression_vector result;
  mutable_indexed_substitution<> sigma;
  const variable v("@var@",s);
  const variable_list vl=atermpp::make_list<variable>(v);
  for(typename enumerator_type::iterator i=enumerator.begin(sigma,enumerator_list_element_with_substitution<data_expression>(vl,sort_bool::true_()));
              i!=enumerator.end(); ++i)
  {
    i->add_assignments(vl,sigma,rewr);
    result.push_back(sigma(v));
  }
  return result;
}




template <class REWRITER, class MutableSubstitution, class EnumeratorListElement>
inline typename REWRITER::term_type classic_enumerator<REWRITER, MutableSubstitution, EnumeratorListElement>::iterator::add_negations(
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

template <class REWRITER, class MutableSubstitution, class EnumeratorListElement>
inline data_expression_list classic_enumerator<REWRITER, MutableSubstitution, EnumeratorListElement>::iterator::negate(const data_expression_list& l) const
{
  if (l.empty())
  {
    return l;
  }
  data_expression_list result=negate(l.tail());
  result.push_front(application(sort_bool::not_(),l.front()));
  return result;
}

template <class REWRITER, class MutableSubstitution, class EnumeratorListElement>
inline void classic_enumerator<REWRITER, MutableSubstitution, EnumeratorListElement>::iterator::push_on_fs_stack_and_split_or_without_rewriting(
                                const EnumeratorListElement& partial_solution,
                                const data_expression_list& negated_term_list,
                                const bool negated)
{
  /* If the negated_term_list equals t1,...,tn, store condition /\ !t1 /\ !t2 /\ ... /\ !tn
     on the fs_stack.  If the condition to be stored on the fs_stack has the shape phi \/ psi, then
     store phi and psi /\ !phi separately. This allows the equality eliminator to remove
     more equalities and therefore be more effective. */
 const variable_list& var_list=partial_solution.variables();
 const typename REWRITER::term_type& condition=partial_solution.expression();

#ifndef MCRL2_DISABLE_ENUMERATOR_SPLIT_DISJUNCTIONS
  if (is_application(condition))
  {
    const application& ca = core::down_cast<application>(condition);
    if (ca.head() == sort_bool::not_())
    {
      push_on_fs_stack_and_split_or_without_rewriting(EnumeratorListElement(var_list,ca[0],partial_solution),negate(negated_term_list),!negated);
      return;
    }
    if ((negated && ca.head() == sort_bool::and_()) ||
             (!negated && ca.head() == sort_bool::or_()))
    {
      assert(condition.size()==3);
      push_on_fs_stack_and_split_or_without_rewriting(EnumeratorListElement(var_list,ca[0],partial_solution),negated_term_list,negated);
      data_expression_list temp=negated_term_list;
      temp.push_front(ca[0]);
      push_on_fs_stack_and_split_or_without_rewriting(EnumeratorListElement(var_list,ca[1],partial_solution),temp,negated);
      return;
    }
  }
#endif

  const typename REWRITER::term_type new_expr = add_negations(condition,negated_term_list,negated);

  if (new_expr!=sort_bool::false_())
  {
    fs_stack.emplace_back(EnumeratorListElement(var_list, new_expr, partial_solution));
  }
}

template <class REWRITER, class MutableSubstitution, class EnumeratorListElement>
inline void classic_enumerator<REWRITER, MutableSubstitution, EnumeratorListElement>::iterator::push_on_fs_stack_and_split_or(
                                const EnumeratorListElement& partial_solution,
                                const data_expression_list& negated_term_list,
                                const bool negated)
{
  classic_enumerator<REWRITER, MutableSubstitution, EnumeratorListElement>::iterator::push_on_fs_stack_and_split_or_without_rewriting(
                                partial_solution,
                                negated_term_list,
                                negated);
}

template <class REWRITER, class MutableSubstitution, class EnumeratorListElement>
inline bool classic_enumerator<REWRITER, MutableSubstitution, EnumeratorListElement>::iterator::find_equality(
                        const data_expression& t,
                        const mcrl2::data::variable_list& vars,
                        mcrl2::data::variable& v,
                        data_expression& e)
{
  if (is_variable(t))
  {
    const variable& var=core::down_cast<variable>(t);
    assert(var.sort()==sort_bool::bool_());
    if (std::find(vars.begin(),vars.end(),var)!=vars.end())
    {
      // This expression is a variable that must be true. We can derive
      // the assignment var:=true from this.
      v = var;
      e = sort_bool::true_();
      return true;
    }
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
    if (f == sort_bool::not_())
    {
      // If the expression has the shape !x where x is a variable over which enumeration is taking
      // place, return x:=false.
      if (is_variable(ta[0]))
      {
        const variable& var=core::down_cast<variable>(ta[0]);
        assert(var.sort()==sort_bool::bool_());
        if (std::find(vars.begin(),vars.end(),var)!=vars.end())
        {
          v = var;
          e = sort_bool::false_();
          return true;
        }
      }
      return false;
    }
    else if (f == sort_bool::and_())
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

template <class REWRITER, class MutableSubstitution, class EnumeratorListElement>
inline void classic_enumerator<REWRITER, MutableSubstitution, EnumeratorListElement>::iterator::EliminateVars(EnumeratorListElement& e)
{
  variable var;
  data_expression val;

  while (!e.variables().empty() && find_equality(e.expression(),e.variables(),var,val))
  {
    // Use a rewrite here to remove occurrences of subexpressions the form t==t caused by
    // replacing in x==t the variable x by t.
    const data_expression old_val=(*enum_sigma)(var);
    (*enum_sigma)[var]=val;
    e=EnumeratorListElement(
                  remove_one_element(e.variables(),var),
                  m_enclosing_enumerator->m_evaluator(e.expression(),*enum_sigma),
                  e,var,val);
    (*enum_sigma)[var]=old_val;
  }
}

template <class REWRITER, class MutableSubstitution, class EnumeratorListElement>
inline void classic_enumerator<REWRITER, MutableSubstitution, EnumeratorListElement>::iterator::find_next_solution(const bool pop_front_of_stack)
{
  if (pop_front_of_stack)
  {
    fs_stack.pop_front();
  }
  for( ; !fs_stack.empty() ; fs_stack.pop_front())
  {
    EnumeratorListElement& e=fs_stack.front();  // e is intensionally a reference into fs_stack.
    EliminateVars(e);
    if (e.variables().empty() || e.expression()==sort_bool::false_())
    {
      if (e.expression()!=sort_bool::false_())
      {
        // A solution is found.
        if (!m_not_equal_to_false)
        {
          e=EnumeratorListElement(e.variables(),m_enclosing_enumerator->m_evaluator(sort_bool::not_(e.expression()),*enum_sigma),e);
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
        if (m_enclosing_enumerator->m_data_spec.is_certainly_finite(sort))
        {
          // Enumerate all functions of this sort.
          function_sort fsort=core::down_cast<function_sort>(sort);
          data_expression_vector codomain_expressions=get_all_expressions(fsort.codomain(),m_enclosing_enumerator->m_data_spec, m_enclosing_enumerator->m_evaluator);
          std::vector < data_expression_vector > domain_expressions;
          size_t total_domain_size=1;
          variable_vector function_parameters;

          for(sort_expression_list::const_iterator i=fsort.domain().begin(); i!=fsort.domain().end(); ++i)
          {
            domain_expressions.push_back(get_all_expressions(*i,m_enclosing_enumerator->m_data_spec,m_enclosing_enumerator->m_evaluator));
            total_domain_size=total_domain_size*domain_expressions.back().size();
            function_parameters.push_back(variable(m_enclosing_enumerator->m_evaluator.identifier_generator()("var_func",false),*i));
          }

          if (total_domain_size*log2(codomain_expressions.size())>=32)  // If there are at least 2^32 functions, then enumerating them makes little sense.
          {
            e.invalidate();
            if (m_enclosing_enumerator->m_throw_exceptions)
            {
              std::stringstream ss;
              ss << "Cannot generate " << codomain_expressions.size() << "^" <<
                            total_domain_size << " functions to enumerate function sort " << sort << "\n";
              throw mcrl2::runtime_error(ss.str());
            }
            return;
          }
          if (total_domain_size*log2(codomain_expressions.size())>16)  // If there are more than 2^16 functions, provide a warning.
          {
            mCRL2log(log::warning) << "Generate " << codomain_expressions.size() << "^" <<
                            total_domain_size << " functions to enumerate sort " << sort << "\n";
          }

          const size_t number_of_functions = static_cast<std::size_t>(std::pow(static_cast<double>(codomain_expressions.size()), static_cast<double>(total_domain_size)));

          const variable_list par_list(function_parameters.begin(), function_parameters.end());
          if (number_of_functions==1) // In this case generate lambda var1,var2,....,var3.result.
          {
            const data_expression lambda_term=abstraction(lambda_binder(),par_list,codomain_expressions.front());
            const data_expression old_substituted_value=(*enum_sigma)(var);
            (*enum_sigma)[var]=lambda_term;
            const data_expression rewritten_expr=m_enclosing_enumerator->m_evaluator(e.expression(),*enum_sigma);
            (*enum_sigma)[var]=old_substituted_value;

            if (rewritten_expr!=sort_bool::false_())
            {
              push_on_fs_stack_and_split_or_without_rewriting(EnumeratorListElement(uvars,rewritten_expr,e,var,lambda_term),
                                    data_expression_list(),
                                    false);
            }
          }
          else
          {
            const data_expression old_substituted_value=(*enum_sigma)(var);
            for(size_t i=0; i<number_of_functions; ++i)
            {
              size_t function_index=i; // function_index is changed in make_if_expression. A copy is therefore required.
              const data_expression lambda_term=abstraction(lambda_binder(),
                                                            par_list,
                                                            detail::make_if_expression(function_index,0,domain_expressions,codomain_expressions,function_parameters));
              assert(function_index==0);
              (*enum_sigma)[var]=lambda_term;
              const data_expression rewritten_expr=m_enclosing_enumerator->m_evaluator(e.expression(),*enum_sigma);

              if (rewritten_expr!=sort_bool::false_())
              {
                push_on_fs_stack_and_split_or_without_rewriting(EnumeratorListElement(uvars,rewritten_expr,e,var,lambda_term),
                                      data_expression_list(),
                                      false);
              }
            }
            (*enum_sigma)[var]=old_substituted_value;
          }
        }
        else
        {
          e.invalidate();
          if (m_enclosing_enumerator->m_throw_exceptions)
          {
            throw mcrl2::runtime_error("Cannot enumerate elements of the function sort " + data::pp(sort));
          }
          return;
        }
      }
      else if (sort_bag::is_bag(sort))
      {
        e.invalidate();
        if (m_enclosing_enumerator->m_throw_exceptions)
        {
          throw mcrl2::runtime_error("Cannot enumerate elements of a bag " + data::pp(sort));
        }
        return;
      }
      else if (sort_fbag::is_fbag(sort))
      {
        e.invalidate();
        if (m_enclosing_enumerator->m_throw_exceptions)
        {
          throw mcrl2::runtime_error("Cannot enumerate elements of a finite bag " + data::pp(sort));
        }
        return;
      }
      else if (sort_set::is_set(sort))
      {
        const sort_expression element_sort=container_sort(sort).element_sort();
        if (m_enclosing_enumerator->m_data_spec.is_certainly_finite(element_sort))
        {
          const data_expression lambda_term=abstraction(
                                              lambda_binder(),
                                              atermpp::make_list<variable>(variable(m_enclosing_enumerator->m_evaluator.identifier_generator()("var_func",false),element_sort)),
                                              sort_bool::false_());
          const variable fset_variable(m_enclosing_enumerator->m_evaluator.identifier_generator()("@var_fset@",false),sort_fset::fset(element_sort));
          const data_expression term=sort_set::constructor(element_sort,lambda_term,fset_variable);
          const data_expression old_substituted_value=(*enum_sigma)(var);
          (*enum_sigma)[var]=term;
          const data_expression rewritten_expr=m_enclosing_enumerator->m_evaluator(e.expression(),*enum_sigma);
          (*enum_sigma)[var]=old_substituted_value;

          if (rewritten_expr!=sort_bool::false_())
          {
            push_on_fs_stack_and_split_or_without_rewriting(EnumeratorListElement(uvars+atermpp::make_list(fset_variable),rewritten_expr,e,var,term),
                                      data_expression_list(),
                                      false);
          }
        }
        else
        {
          e.invalidate();
          if (m_enclosing_enumerator->m_throw_exceptions)
          {
            throw mcrl2::runtime_error("Cannot enumerate all elements of a set " + data::pp(sort));
          }
          return;
        }
      }
      else if (sort_fset::is_fset(sort))
      {
        const sort_expression& element_sort=container_sort(sort).element_sort();
        if (m_enclosing_enumerator->m_data_spec.is_certainly_finite(element_sort))
        {
          data_expression_vector all_element_expressions=get_all_expressions(element_sort,m_enclosing_enumerator->m_data_spec, m_enclosing_enumerator->m_evaluator);
          if (all_element_expressions.size()>=32)  // If there are at least 2^32 functions, then enumerating them makes little sense.
          {
            e.invalidate();
            if (m_enclosing_enumerator->m_throw_exceptions)
            {
              std::stringstream ss;
              ss << "Will not generate the 2^" <<
                            all_element_expressions.size() << " sets to enumerate all sets of sort " << sort << "\n";
              throw mcrl2::runtime_error(ss.str());
            }
            return;
          }
          if (all_element_expressions.size()>16)  // If there are more than 2^16 functions, provide a warning.
          {
            mCRL2log(log::warning) << "Generate 2^" <<
                            all_element_expressions.size() << " sets to enumerate sort " << sort << "\n";
          }

          const size_t number_of_sets = static_cast<std::size_t>(std::pow(2.0, all_element_expressions.size()));

          const data_expression old_substituted_value=(*enum_sigma)(var);

          for(size_t i=0; i<number_of_sets; ++i)
          {
            const data_expression set=m_enclosing_enumerator->m_evaluator(detail::make_set(i,element_sort,all_element_expressions),*enum_sigma);
            (*enum_sigma)[var]=set;
            const data_expression rewritten_expr=m_enclosing_enumerator->m_evaluator(e.expression(),*enum_sigma);

            if (rewritten_expr!=sort_bool::false_())
            {
              push_on_fs_stack_and_split_or_without_rewriting(EnumeratorListElement(uvars,rewritten_expr,e,var,set),
                                    data_expression_list(),
                                    false);
            }
          }
          (*enum_sigma)[var]=old_substituted_value;

        }
        else
        {
          e.invalidate();
          if (m_enclosing_enumerator->m_throw_exceptions)
          {
            throw mcrl2::runtime_error("Cannot enumerate all elements of a finite set " + data::pp(sort));
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
            throw mcrl2::runtime_error("Cannot enumerate elements of sort " + data::pp(sort) + " as it does not have constructor functions");
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
            if (rewritten_expr!=sort_bool::false_())
            {
              push_on_fs_stack_and_split_or_without_rewriting(EnumeratorListElement(uvars+var_list,rewritten_expr,e,var,term_rf),
                                    data_expression_list(),
                                    false);
            }
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
            if (rewritten_expr!=sort_bool::false_())
            {
              push_on_fs_stack_and_split_or_without_rewriting(EnumeratorListElement(uvars,rewritten_expr,e,var,term_rf),
                                    data_expression_list(),
                                    false);
            }
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
