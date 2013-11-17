// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/utility.h
/// \brief Utility functions for the pbes library.

#ifndef MCRL2_PBES_UTILITY_H
#define MCRL2_PBES_UTILITY_H

#include "mcrl2/atermpp/algorithm.h"

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/sort_expression.h"

#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/detail/rewriter_wrapper.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

struct compare_variableL
{
  atermpp::aterm v;

  compare_variableL(data::variable v_)
    : v(atermpp::aterm_appl(v_))
  {}

  bool operator()(atermpp::aterm t) const
  {
    return v == t;
  }
};

//  variable v occurs in l.

static bool occurs_in_varL(
                  const pbes_system::pbes_expression& l,
                  const data::variable &v,
                  const bool use_internal_rewrite_format)
{
  // In the internal format, the pbes_expression l contains data expressions
  // in internal rewrite format. Therefore the function find_if defined on
  // aterms must be used, instead of the function search variable which works
  // on a pbes_expression, and which will not recognize (and therefore skip)
  // expressions in internal format. So, it will not search the whole expression.
  if (use_internal_rewrite_format)
  {
    return find_if(l, compare_variableL(v)) != atermpp::aterm_appl();
  }
  else
  {
    return pbes_system::search_variable(l, v);
  }
}

/// \brief gives the rank of a propositional_variable_instantiation. It is assumed that the variable
/// occurs in the pbes_specification.

inline unsigned long get_rank(const propositional_variable_instantiation& current_variable_instantiation,
                              const pbes& pbes_spec)
{
  unsigned long rank=0;
  const std::vector<pbes_equation>& eqsys = pbes_spec.equations();
  fixpoint_symbol current_fixpoint_symbol=eqsys.begin()->symbol();
  for (auto eqi = eqsys.begin(); eqi != eqsys.end(); eqi++)
  {
    if (eqi->variable().name()==current_variable_instantiation.name())
    {
      return rank;
    }
    if (eqi->symbol()!=current_fixpoint_symbol)
    {
      current_fixpoint_symbol=eqi->symbol();
      rank=rank+1;
    }
  }
  assert(0); // It is assumed that the current_variable_instantiation occurs in the pbes_spec.
  return 0;
}


/// \brief gives the fixed point symbol of a propositional_variable_instantiation. It is assumed that the variable
/// occurs in the pbes_specification. Returns false if the symbol is mu, and true if the symbol in nu.

inline bool get_fixpoint_symbol(const propositional_variable_instantiation current_variable_instantiation,
                                const pbes& pbes_spec)
{
  const std::vector<pbes_equation>& eqsys = pbes_spec.equations();
  for (auto eqi = eqsys.begin(); eqi != eqsys.end(); eqi++)
  {
    if (eqi->variable().name()==current_variable_instantiation.name())
    {
      return (eqi->symbol().is_nu());
    }
  }
  assert(0); // It is assumed that the variable instantiation occurs in the PBES.
  return false;
}

/**************************************************************************************************/

// given a pbes expression of the form p1 && p2 && .... && pn, this will yield a
// set of the form { p1,p2, ..., pn}, assuming that pi does not have a && as main
// function symbol.

static void distribute_and(const pbes_expression& expr,std::set < pbes_expression> &conjunction_set)
{
  /* distribute the conjuncts of expr over the conjunction_set */
  if (is_pbes_and(expr))
  {
    distribute_and(accessors::left(expr),conjunction_set);
    distribute_and(accessors::right(expr),conjunction_set);
  }
  else
  {
    conjunction_set.insert(expr);
  }
}

// given a set { p1, p2, ... , pn}, the function below yields an expression
// of the form p1 && ... && pn.

static pbes_expression make_conjunction(const std::set < pbes_expression> &conjunction_set)
{
  pbes_expression t=pbes_expr::true_();

  for (std::set < pbes_expression>::const_iterator i=conjunction_set.begin();
       i!=conjunction_set.end() ; i++)
  {
    if (is_pbes_true(t))
    {
      t=*i;
    }
    else
    {
      t=pbes_expr::and_(*i,t);
    }
  }
  return t;
}

// see distribute_and.

static void distribute_or(const pbes_expression& expr,std::set < pbes_expression> &disjunction_set)
{
  /* distribute the conjuncts of expr over the conjunction_set */
  if (is_pbes_or(expr))
  {
    distribute_or(accessors::left(expr),disjunction_set);
    distribute_or(accessors::right(expr),disjunction_set);
  }
  else
  {
    disjunction_set.insert(expr);
  }
}

// see make_conjunction.

static pbes_expression make_disjunction(const std::set < pbes_expression> &disjunction_set)
{
  pbes_expression t=pbes_expr::false_();

  for (std::set < pbes_expression>::const_iterator i=disjunction_set.begin();
       i!=disjunction_set.end() ; i++)
  {
    if (is_pbes_false(t))
    {
      t=*i;
    }
    else
    {
      t=pbes_expr::or_(*i,t);
    }
  }
  return t;
}

// The function below restores a saved substitution.
static void restore_saved_substitution(const std::map<data::variable,data::data_expression> &saved_substitutions,
                                       data::detail::legacy_rewriter& /* r */,
                                       const bool use_internal_rewrite_format,
                                       data::detail::legacy_rewriter::substitution_type &sigma,
                                       data::detail::legacy_rewriter::internal_substitution_type &sigma_internal)
{
  for (std::map<data::variable,data::data_expression>::const_iterator i=saved_substitutions.begin();
       i!=saved_substitutions.end(); ++i)
  {
    if (use_internal_rewrite_format)
    {
      // r.set_internally_associated_value(i->first,static_cast<atermpp::aterm_appl>(i->second));
      sigma_internal[i->first]=i->second;
    }
    else
    {
      // r.set_internally_associated_value(i->first,i->second);
      sigma[i->first]=data::data_expression(i->second);
    }
  }
}

///\brief Substitute in p, remove quantifiers and return the result after rewriting.
/// Given a substitution, which is prepared within the rewriter, this function first
/// applies this substitution. Then it will attempt to eliminate all quantifiers.
/// I.e. if S is a constructorsort (which means that there exists at least one constructor
/// with S as target sort), then an expression of the form forall x:S.p(x) is replaced
/// by forall x1..xn.p(f1(x1..xn) &&  forall x1..xm.p(f2(x1..xm)) && .... for all
/// \brief Constructors fi with S as targetsort (this is done similarly for the exists).
/// if the constructors are constants (i.e. n=0, m=0, etc.), no new quantifications
/// will be generated. And if the expressions are not constant, expressions such as
/// p(f1(x1..xn)) are simplified, which can lead to the situation that certain
/// variables x1..xn do not occur anymore. In that case the quantors can be removed
/// also. The function pbes_expression_substitute_and_rewrite will continue substituting
/// \brief Constructors for quantified variables until there are no variables left, or
/// until there are only quantifications over non constructor sorts. In the last case,
/// the function will throw an exception. For every 100 new variables being used
/// in new quantifications, a message is printed.
///   The data_specification is needed to determine the constructors of a certain
/// sort. The rewriter is used to perform the rewriting steps. The rewriter can
/// either work on internal rewriting format, in which case all data expressions in
/// the pbes are already in internal rewriting format. In this case the data expressions
/// in the result are also in internal format. The use of internal format saves
/// a lot of internal compilation time.

/* Header below was used up till version 7262 but has been reverted to
   obtain an acceptable performance. Once the pbes rewriters are up to
   speed, the code below is superfluous, and can be replaced by the
   standard pbes rewriters.
   inline pbes_expression pbes_expression_substitute_and_rewrite(
                   const pbes_expression &p,
                   const data::data_specification &data,
                   const data::rewriter& r,
                   const bool use_internal_rewrite_format,
                   data::mutable_map_substitution< > &sigma) */

inline pbes_expression pbes_expression_substitute_and_rewrite(
  const pbes_expression& p,
  const data::data_specification& data,
  data::detail::legacy_rewriter& r,
  const bool use_internal_rewrite_format,
  data::detail::legacy_rewriter::substitution_type &sigma,
  data::detail::legacy_rewriter::internal_substitution_type &sigma_internal)
{
  using namespace pbes_system::pbes_expr;
  using namespace pbes_system::accessors;
  pbes_expression result;

  if (is_pbes_and(p))
  {
    // p = and(left, right)
    //Rewrite left and right as far as possible
    pbes_expression l = pbes_expression_substitute_and_rewrite(left(p),
                        data, r,use_internal_rewrite_format,sigma,sigma_internal);
    if (is_pbes_false(l))
    {
      result = pbes_system::pbes_expr::false_();
    }
    else
    {
      pbes_expression rt = pbes_expression_substitute_and_rewrite(right(p),
                           data, r,use_internal_rewrite_format,sigma,sigma_internal);
      //Options for left and right
      if (is_pbes_false(rt))
      {
        result = pbes_system::pbes_expr::false_();
      }
      else if (is_pbes_true(l))
      {
        result = rt;
      }
      else if (is_pbes_true(rt))
      {
        result = l;
      }
      else
      {
        result = pbes_system::pbes_expr::and_(l,rt);
      }
    }
  }
  else if (is_pbes_or(p))
  {
    // p = or(left, right)
    //Rewrite left and right as far as possible

    pbes_expression l = pbes_expression_substitute_and_rewrite(left(p),
                        data, r,use_internal_rewrite_format,sigma,sigma_internal);
    if (is_pbes_true(l))
    {
      result = pbes_system::pbes_expr::true_();
    }
    else
    {
      pbes_expression rt = pbes_expression_substitute_and_rewrite(right(p),
                           data, r,use_internal_rewrite_format,sigma,sigma_internal);
      if (is_pbes_true(rt))
      {
        result = pbes_system::pbes_expr::true_();
      }
      else if (is_pbes_false(l))
      {
        result = rt;
      }
      else if (is_pbes_false(rt))
      {
        result = l;
      }
      else
      {
        result = pbes_system::pbes_expr::or_(l,rt);
      }
    }
  }
  else if (is_pbes_imp(p))
  {
    // p = or(left, right)
    //Rewrite left and right as far as possible

    pbes_expression l = pbes_expression_substitute_and_rewrite(left(p),
                        data, r,use_internal_rewrite_format,sigma,sigma_internal);
    if (is_pbes_false(l))
    {
      result = pbes_system::pbes_expr::true_();
    }
    else
    {
      pbes_expression rt = pbes_expression_substitute_and_rewrite(right(p),
                           data, r,use_internal_rewrite_format,sigma,sigma_internal);
      if (is_pbes_true(rt))
      {
        result = pbes_system::pbes_expr::true_();
      }
      else if (is_pbes_true(l))
      {
        result = rt;
      }
      else if (is_pbes_false(rt))
      {
        result = mcrl2::pbes_system::pbes_expr::not_(l);
      }
      else
      {
        result = pbes_system::pbes_expr::imp(l,rt);
      }
    }
  }
  else if (is_pbes_true(p))
  {
    // p is True
    result = p;
  }
  else if (is_pbes_false(p))
  {
    // p is False
    result = p;
  }
  else if (is_pbes_not(p))
  {
    pbes_expression l = pbes_expression_substitute_and_rewrite(arg(p),
                              data, r,use_internal_rewrite_format,sigma,sigma_internal);
    result = pbes_expr::not_(l);
  }
  else if (is_pbes_forall(p))
  {
    data::variable_list data_vars = var(p);

    // If no data_vars
    if (data_vars.empty())
    {
      pbes_expression expr = pbes_expression_substitute_and_rewrite(arg(p), data, r,use_internal_rewrite_format,sigma,sigma_internal);
      result = expr;
    }
    else
    {
      /* Replace the quantified variables of constructor sorts by constructors.
         E.g. forall x:Nat.phi(x) is replaced by phi(0) && forall x':Nat.phi(successor(x')),
         assuming 0 and successor are the constructors of Nat  (which is btw. not the case
         in de data-implementation of mCRL2).  Simplify the resulting expressions.
         First the substitutions for existing variables that are also bound in the
         quantifier are saved and reset to the variable. At the end they must be reset. */

      std::map < data::variable, data::data_expression > saved_substitutions;
      for (data::variable_list::const_iterator i=data_vars.begin(); i!=data_vars.end(); ++i)
      {
        data::data_expression d;
        if (use_internal_rewrite_format)
        {
          d=r.rewrite_internal(*i,sigma_internal);
        }
        else
        {
          d=r(*i,sigma);
        }

        if (*i!=d)
        {
          saved_substitutions[*i]=d;
          // r.clear_internally_associated_value(*i);
          if (use_internal_rewrite_format)
          {
            sigma_internal[*i]= *i;
          }
          else
          {
            sigma[*i]= *i;
          }

        }
      }
      pbes_expression expr = pbes_expression_substitute_and_rewrite(arg(p), data, r,use_internal_rewrite_format,sigma,sigma_internal);


      data::set_identifier_generator variable_name_generator;
      variable_name_generator.add_identifiers(pbes_system::find_identifiers(expr));
      size_t no_variables=0;
      data::variable_list new_data_vars;
      std::set < pbes_expression > conjunction_set;
      distribute_and(expr,conjunction_set);
      bool constructor_sorts_found=true;
      for (; constructor_sorts_found ;)
      {
        constructor_sorts_found=false;
        for (data::variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
        {

          if (!data.is_constructor_sort(i->sort()))
          {
            /* The sort of variable i is not a constructor sort.  */
            new_data_vars.push_front(*i);
          }
          else
          {
            std::set <pbes_expression> new_conjunction_set;
            for (std::set <pbes_expression >::iterator t=conjunction_set.begin() ;
                 t!=conjunction_set.end() ; t++)
            {
              if (!occurs_in_varL(*t,*i,use_internal_rewrite_format))
              {
                new_conjunction_set.insert(*t);
              }
              else
              {
                data::function_symbol_vector i_constructors(data.constructors(i->sort()));
                for (data::function_symbol_vector::const_iterator rf = i_constructors.begin(); rf != i_constructors.end(); ++rf)
                {
                  data::sort_expression_list dsorts;

                  if (is_function_sort(rf->sort()))
                  {
                    const data::function_sort& sa = core::static_down_cast<const data::function_sort&>(rf->sort());
                    // In case the function f has a sort A->(B->C),
                    // then the function below does not work correctly.
                    // This code must be replaced by enumerator code,
                    // developed by Wieger.
                    if (is_function_sort(sa.codomain()))
                    {
                      throw mcrl2::runtime_error("Cannot deal with constructors of type (A->(B->C)): " +
                                                 data::pp(*rf) + ":" + data::pp(rf->sort()));
                    }
                    dsorts=sa.domain();
                  }
                  // else dsorts is empty.

                  // XXXXXXXXXXXXXX argument_sorts(), result_sort()  =source(f->sort());
                  data::variable_list function_arguments;
                  for (data::sort_expression_list::const_iterator s=dsorts.begin() ;
                       s!=dsorts.end() ; ++s)
                  {
                    constructor_sorts_found=constructor_sorts_found || data.is_constructor_sort(*s);
                    data::variable new_variable(variable_name_generator("internally_generated_variable_for_forall"), *s);
                    ++no_variables;
                    if ((no_variables % 100)==0)
                    {
                      mCRL2log(log::verbose) << "Used " << no_variables << " variables when eliminating universal quantifier\n";
                      if (!use_internal_rewrite_format)
                      {
                        mCRL2log(log::verbose) << "Vars: " << data::pp(data_vars) << "\nExpression: " << mcrl2::pbes_system::pp(*t) << std::endl;
                      }
                    }
                    new_data_vars.push_front(new_variable);
                    function_arguments.push_front(new_variable);
                  }

                  const data::data_expression d((!function_arguments.empty())?
                                    static_cast<data::data_expression>(data::application(*rf, reverse(function_arguments))):
                                    static_cast<data::data_expression>(*rf));
                  // sigma[*i]=d;
                  // r.set_internally_associated_value(*i,d);
                  if (use_internal_rewrite_format)
                  {
                    sigma_internal[*i]= r.rewrite_internal(r.convert_to(d),sigma_internal);
                  }
                  else
                  {
                    sigma[*i]= r(d);
                  }
                  pbes_expression rt(pbes_expression_substitute_and_rewrite(*t,data,r,use_internal_rewrite_format,sigma,sigma_internal));
                  // sigma[*i] = *i; // erase *i
                  // r.clear_internally_associated_value(*i);
                  if (use_internal_rewrite_format)
                  {
                    sigma_internal[*i]= *i;
                  }
                  else
                  {
                    sigma[*i]= *i;
                  }
                  if (is_pbes_false(rt)) /* the resulting expression is false, so we can terminate */
                  {
                    restore_saved_substitution(saved_substitutions,r,use_internal_rewrite_format,sigma,sigma_internal);
                    return pbes_expr::false_();
                  }
                  else
                  {
                    new_conjunction_set.insert(rt);
                  }
                }
              }
            }
            conjunction_set=new_conjunction_set;
          }
        }
        data_vars=new_data_vars;
        new_data_vars=data::variable_list();
      }

      if (!new_data_vars.empty())
      {
        std::string message("Cannot eliminate universal quantifiers of variables ");

        message.append(data::pp(new_data_vars));

        if (!use_internal_rewrite_format)
        {
          message.append(" in ").append(mcrl2::data::pp(p));
        }

        throw mcrl2::runtime_error(message);
      }
      result=make_conjunction(conjunction_set);
      restore_saved_substitution(saved_substitutions,r,use_internal_rewrite_format,sigma,sigma_internal);
    }
  }
  else if (is_pbes_exists(p))
  {
    data::variable_list data_vars = var(p);
    // If no data_vars
    if (data_vars.empty())
    {
      pbes_expression expr = pbes_expression_substitute_and_rewrite(arg(p), data, r,use_internal_rewrite_format,sigma,sigma_internal);
      result = expr;
    }
    else
    {
      /* Replace the quantified variables of constructor sorts by constructors.
         E.g. forall x:Nat.phi(x) is replaced by phi(0) || forall x':Nat.phi(successor(x')),
         assuming 0 and successor are the constructors of Nat  (which is btw. not the case
         in de data-implementation of mCRL2).  Simplify the resulting expressions.
         First the substitutions for existing variables that are also bound in the
         quantifier are saved and reset to the variable. At the end they must be reset.
      */

      std::map < data::variable, data::data_expression > saved_substitutions;
      for (data::variable_list::const_iterator i=data_vars.begin(); i!=data_vars.end(); ++i)
      {
        data::data_expression d;
        if (use_internal_rewrite_format)
        {
          d=r.rewrite_internal(*i,sigma_internal);
        }
        else
        {
          d=r(*i,sigma);
        }

        if (*i!=d)
        {
          saved_substitutions[*i]=d;
          // r.clear_internally_associated_value(*i);
          if (use_internal_rewrite_format)
          {
            sigma_internal[*i]= *i;
          }
          else
          {
            sigma[*i]= *i;
          }
        }
      }

      pbes_expression expr = pbes_expression_substitute_and_rewrite(arg(p), data, r, use_internal_rewrite_format,sigma,sigma_internal);

      data::set_identifier_generator variable_name_generator;
      variable_name_generator.add_identifiers(pbes_system::find_identifiers(expr));
      size_t no_variables=0;
      data::variable_list new_data_vars;
      std::set < pbes_expression > disjunction_set;
      distribute_or(expr,disjunction_set);
      bool constructor_sorts_found=true;
      while (constructor_sorts_found)
      {
        constructor_sorts_found=false;
        for (data::variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
        {
          if (!data.is_constructor_sort(i->sort()))
          {
            /* The sort of variable i is not a constructor sort.  */
            new_data_vars.push_front(*i);
          }
          else
          {
            std::set <pbes_expression> new_disjunction_set;
            for (std::set <pbes_expression >::iterator t=disjunction_set.begin() ;
                 t!=disjunction_set.end() ; t++)
            {
              if (!occurs_in_varL(*t,*i,use_internal_rewrite_format))
              {
                new_disjunction_set.insert(*t);
              }
              else
              {

                data::function_symbol_vector i_constructors(data.constructors(i->sort()));
                for (data::function_symbol_vector::const_iterator rf = i_constructors.begin(); rf != i_constructors.end(); ++rf)
                {
                  data::sort_expression_list dsorts;
                  if (is_function_sort(rf->sort()))
                  {
                    data::function_sort sa = core::static_down_cast<const data::function_sort&>(rf->sort());
                    // In case the function f has a sort A->(B->C),
                    // then the function below does not work correctly.
                    // This code must be replaced by enumerator code,
                    // developed by Wieger.
                    if (is_function_sort(sa.codomain()))
                    {
                      throw mcrl2::runtime_error("Cannot deal with constructors of type (A->(B->C)): " +
                                                 data::pp(*rf) + ":" + data::pp(rf->sort()));
                    }
                    dsorts=sa.domain();
                  }
                  // else dsorts is empty.

                  data::variable_list function_arguments;
                  for (data::sort_expression_list::const_iterator s=dsorts.begin() ;
                       s!=dsorts.end() ; ++s)
                  {
                    constructor_sorts_found=constructor_sorts_found || data.is_constructor_sort(*s);
                    data::variable new_variable(variable_name_generator("internally_generated_variable_for_forall"), *s);
                    ++no_variables;
                    if ((no_variables % 100)==0)
                    {
                      mCRL2log(log::verbose) << "Used " << no_variables << " variables when eliminating existential quantifier\n";
                      if (!use_internal_rewrite_format)
                      {
                        mCRL2log(log::verbose) << "Vars: " << data::pp(data_vars) << "\nExpression: " << mcrl2::pbes_system::pp(*t) << std::endl;
                      }
                    }
                    new_data_vars.push_front(new_variable);
                    function_arguments.push_front(new_variable);
                  }
                  const data::data_expression d((!function_arguments.empty())?
                                    static_cast<data::data_expression>(data::application(*rf, reverse(function_arguments))):
                                    static_cast<data::data_expression>(*rf));
                  //r.set_internally_associated_value(*i,d);
                  // sigma[*i]=d;
                  if (use_internal_rewrite_format)
                  {
                    sigma_internal[*i]=r.rewrite_internal(r.convert_to(d),sigma_internal);
                  }
                  else
                  {
                    sigma[*i]=r(d);
                  }
                  pbes_expression rt(pbes_expression_substitute_and_rewrite(*t,data,r,use_internal_rewrite_format,sigma,sigma_internal));
                  // sigma[*i] = *i; // erase *i
                  // r.clear_internally_associated_value(*i);
                  if (use_internal_rewrite_format)
                  {
                    sigma_internal[*i]= *i;
                  }
                  else
                  {
                    sigma[*i]= *i;
                  }
                  if (is_pbes_true(rt)) /* the resulting expression is true, so we can terminate */
                  {
                    restore_saved_substitution(saved_substitutions,r,use_internal_rewrite_format,sigma,sigma_internal);
                    return pbes_expr::true_();
                  }
                  else
                  {
                    new_disjunction_set.insert(rt);
                  }
                }
              }
            }
            disjunction_set=new_disjunction_set;
          }
        }
        data_vars=new_data_vars;
        new_data_vars=data::variable_list();
      }

      if (!new_data_vars.empty())
      {
        std::string message("Cannot eliminate existential quantifiers of variables ");

        message.append(data::pp(new_data_vars));

        if (!use_internal_rewrite_format)
        {
          message.append(" in ").append(mcrl2::pbes_system::pp(p));
        }

        throw mcrl2::runtime_error(message);
      }
      result=make_disjunction(disjunction_set);
      restore_saved_substitution(saved_substitutions,r,use_internal_rewrite_format,sigma,sigma_internal);
    }
  }
  else if (is_propositional_variable_instantiation(p))
  {
    // p is a propositional variable
    const propositional_variable_instantiation& propvar = core::static_down_cast<const propositional_variable_instantiation&>(p);
    core::identifier_string name = propvar.name();
    data::data_expression_list current_parameters(propvar.parameters());
    data::data_expression_list parameters;
    if (use_internal_rewrite_format)
    {
      atermpp::term_list< data::data_expression > expressions(r.rewrite_internal_list(
            data::data_expression_list(current_parameters.begin(), current_parameters.end()),sigma_internal));
      parameters=data::data_expression_list(expressions.begin(), expressions.end());
    }
    else
    {
      for (data::data_expression_list::const_iterator l=current_parameters.begin();
           l != current_parameters.end(); ++l)
      {
        parameters.push_front(atermpp::aterm_cast<data::data_expression>(r(*l,sigma)));
      }
      parameters = atermpp::reverse(parameters);
    }
    result = pbes_expression(propositional_variable_instantiation(name, parameters));
  }
  else
  {
    // p is a data::data_expression
    const data::data_expression& dp = atermpp::aterm_cast<const data::data_expression>(p);
    if (use_internal_rewrite_format)
    {
      data::data_expression d = (data::data_expression)r.rewrite_internal(dp,sigma_internal);
      if (d==r.get_rewriter().internal_true)
      {
        result = pbes_expr::true_();
      }
      else if (d==r.get_rewriter().internal_false)
      {
        result = pbes_expr::false_();
      }
      else
      {
        result = d;
      }
    }
    else
    {
      data::data_expression d(r(dp, sigma));
      if (d == data::sort_bool::true_())
      {
        result = pbes_expr::true_();
      }
      else if (d == data::sort_bool::false_())
      {
        result = pbes_expr::false_();
      }
      else
      {
        result = d;
      }
    }
  }
  return result;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_UTILITY_H
