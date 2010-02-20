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

#include "mcrl2/pbes/pbes.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/fresh_variable_generator.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/atermpp/algorithm.h"

// JFG:
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/detail/rewrite.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

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

    static mcrl2::data::data_expression initialize_internal_true(mcrl2::data::data_expression &t,boost::shared_ptr<mcrl2::data::detail::Rewriter> r)
      { using namespace mcrl2::data;
        t=(data_expression)r->toRewriteFormat(sort_bool::true_());
        ATprotect((ATerm*)(&t));
        return t;
      }

      static bool is_true_in_internal_rewrite_format(mcrl2::data::data_expression d,boost::shared_ptr<mcrl2::data::detail::Rewriter> rewriter)
      { using namespace mcrl2::data;
        static data_expression internal_true=initialize_internal_true(internal_true,rewriter);
        return d==internal_true;
      }


      static mcrl2::data::data_expression initialize_internal_false(mcrl2::data::data_expression &t,boost::shared_ptr<mcrl2::data::detail::Rewriter> r)
      { using namespace mcrl2::data;
        t=(data_expression)r->toRewriteFormat(sort_bool::false_());
        ATprotect((ATerm*)(&t));
        return t;
      }

      static bool is_false_in_internal_rewrite_format(mcrl2::data::data_expression d,boost::shared_ptr<mcrl2::data::detail::Rewriter> rewriter)
      { using namespace mcrl2::data;
        static data_expression internal_false=initialize_internal_false(internal_false,rewriter);
        return d==internal_false;
      }

//  variable v occurs in l.
//
static bool occurs_in_varL(atermpp::aterm_appl l, data::variable v)
{
  return find_if(l, compare_variableL(v)) != atermpp::aterm();
}

/// \brief gives the rank of a propositional_variable_instantiation. It is assumed that the variable
/// occurs in the pbes_specification.

template <typename Container>
inline unsigned long get_rank(const propositional_variable_instantiation current_variable_instantiation,
                              pbes<Container> pbes_spec)
{
  unsigned long rank=0;
  Container eqsys = pbes_spec.equations();
  fixpoint_symbol current_fixpoint_symbol=eqsys.begin()->symbol();
  for (typename Container::iterator eqi = eqsys.begin(); eqi != eqsys.end(); eqi++)
  {
    if (eqi->variable().name()==current_variable_instantiation.name())
    { return rank;
    }
    if (eqi->symbol()!=current_fixpoint_symbol)
    { current_fixpoint_symbol=eqi->symbol();
      rank=rank+1;
    }
  }
  assert(0); // It is assumed that the current_variable_instantiation occurs in the pbes_spec.
  return 0;
}


/// \brief gives the fixed point symbol of a propositional_variable_instantiation. It is assumed that the variable
/// occurs in the pbes_specification. Returns false if the symbol is mu, and true if the symbol in nu.

template <typename Container>
inline bool get_fixpoint_symbol(const propositional_variable_instantiation current_variable_instantiation,
                                pbes<Container> pbes_spec)
{
  Container eqsys = pbes_spec.equations();
  for (typename Container::iterator eqi = eqsys.begin(); eqi != eqsys.end(); eqi++)
  {
    if (eqi->variable().name()==current_variable_instantiation.name())
    { return (is_nu(eqi->symbol()));
    }
  }
  assert(0); // It is assumed that the variable instantiation occurs in the PBES.
  return false;
}


/// \brief Gives the instantiated right hand side for a propositional_variable_instantiation

/* template <typename Container>
inline pbes_expression give_the_instantiated_rhs(
                   const propositional_variable_instantiation current_variable_instantiation,
                   pbes<Container> pbes_spec,
                   const data::rewriter& r,
                   const bool use_internal_rewriter_format=false)
{
  Container eqsys = pbes_spec.equations();

  // Fill the pbes_equations table

  assert(eqsys.size()>0); // There should be at least one equation
  fixpoint_symbol current_fixpoint_symbol=eqsys.begin()->symbol();

  pbes_equation current_pbeq;
  for (typename Container::iterator eqi = eqsys.begin(); eqi != eqsys.end(); eqi++)
  {
    if (current_variable_instantiation.name()==eqi->variable().name())
    { current_pbeq=pbes_equation(eqi->symbol(),eqi->variable(),eqi->formula());
      break;
    }
  }

  data::data_expression_list::iterator elist=current_variable_instantiation.parameters().begin();

  data::mutable_map_substitution< > sigma;
  for(data::variable_list::iterator vlist=current_pbeq.variable().parameters().begin() ;
               vlist!=current_pbeq.variable().parameters().end() ; vlist++)
  {
    assert(elist!=current_variable_instantiation.parameters().end());

    if (use_internal_rewriter_format)
    { assert(0); // Internal format is not treated separately.
      / * r.setSubstitutionInternal(*vlist,(atermpp::aterm)*elist);
 *    * /
    }
    else
    {
      // r.setSubstitution(*vlist,*elist);
      sigma[*vlist]=*elist;
    }
    elist++;
  }
  assert(elist==current_variable_instantiation.parameters().end());
  return  pbes_expression_substitute_and_rewrite(
                                current_pbeq.formula(),
                                pbes_spec.data(),
                                r,
                                use_internal_rewriter_format,
                                sigma);
} */




/**************************************************************************************************/

// given a pbes expression of the form p1 && p2 && .... && pn, this will yield a
// set of the form { p1,p2, ..., pn}, assuming that pi does not have a && as main
// function symbol.

static void distribute_and(const pbes_expression &expr,atermpp::set < pbes_expression> &conjunction_set)
{ /* distribute the conjuncts of expr over the conjunction_set */
  if (pbes_expr::is_pbes_and(expr))
  { distribute_and(accessors::left(expr),conjunction_set);
    distribute_and(accessors::right(expr),conjunction_set);
  }
  else
  { conjunction_set.insert(expr);
  }
}

// given a set { p1, p2, ... , pn}, the function below yields an expression
// of the form p1 && ... && pn.

static pbes_expression make_conjunction(const atermpp::set < pbes_expression> &conjunction_set)
{ pbes_expression t=pbes_expr::true_();

  for(atermpp::set < pbes_expression>::const_iterator i=conjunction_set.begin();
          i!=conjunction_set.end() ; i++)
  { if (pbes_expr::is_pbes_true(t))
    { t=*i;
    }
    else
    { t=pbes_expr::and_(*i,t);
    }
  }
  return t;
}

// see distribute_and.

static void distribute_or(const pbes_expression &expr,atermpp::set < pbes_expression> &disjunction_set)
{ /* distribute the conjuncts of expr over the conjunction_set */
  if (pbes_expr::is_pbes_or(expr))
  { distribute_or(accessors::left(expr),disjunction_set);
    distribute_or(accessors::right(expr),disjunction_set);
  }
  else
  { disjunction_set.insert(expr);
  }
}

// see make_conjunction.

static pbes_expression make_disjunction(const atermpp::set < pbes_expression> &disjunction_set)
{ pbes_expression t=pbes_expr::false_();

  for(atermpp::set < pbes_expression>::const_iterator i=disjunction_set.begin();
          i!=disjunction_set.end() ; i++)
  {
    if (pbes_expr::is_pbes_false(t))
    { t=*i;
    }
    else
    { t=pbes_expr::or_(*i,t);
    }
  }
  return t;
}

// The function below restores a saved substitution.
static void restore_saved_substitution(const atermpp::map<data::variable,data::data_expression> &saved_substitutions,
                                       boost::shared_ptr<mcrl2::data::detail::Rewriter> r,
                                       const bool use_internal_rewrite_format)
{ for(atermpp::map<data::variable,data::data_expression>::const_iterator i=saved_substitutions.begin();
                 i!=saved_substitutions.end(); ++i)
  { if (use_internal_rewrite_format)
    { r->setSubstitutionInternal((ATermAppl)i->first,(ATerm)(ATermAppl)i->second); 
    } 
    else 
    { r->setSubstitution(i->first,i->second); 
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
                   const pbes_expression &p,
                   const data::data_specification &data,
                   boost::shared_ptr<mcrl2::data::detail::Rewriter> r,
                   const bool use_internal_rewrite_format)
{ 
  using namespace pbes_system::pbes_expr;
  using namespace pbes_system::accessors;
  pbes_expression result;

  if (is_pbes_and(p))
  { // p = and(left, right)
    //Rewrite left and right as far as possible
    pbes_expression l = pbes_expression_substitute_and_rewrite(left(p),
                               data, r,use_internal_rewrite_format);
    if (is_pbes_false(l))
    { result = false_();
    }
    else
    { pbes_expression rt = pbes_expression_substitute_and_rewrite(right(p),
                 data, r,use_internal_rewrite_format);
      //Options for left and right
      if (is_pbes_false(rt))
      { result = false_();
      }
      else if (is_pbes_true(l))
      { result = rt;
      }
      else if (is_pbes_true(rt))
      { result = l;
      }
      else result = and_(l,rt);
    }
  }
  else if (is_pbes_or(p))
  { // p = or(left, right)
    //Rewrite left and right as far as possible

    pbes_expression l = pbes_expression_substitute_and_rewrite(left(p),
                 data, r,use_internal_rewrite_format);
    if (is_pbes_true(l))
    { result = true_();
    }
    else
    { pbes_expression rt = pbes_expression_substitute_and_rewrite(right(p),
                 data, r,use_internal_rewrite_format);
      if (is_pbes_true(rt))
      { result = true_();
      }
      else if (is_pbes_false(l))
      { result = rt;
      }
      else if (is_pbes_false(rt))
      { result = l;
      }
      else result = or_(l,rt);
    }
  }
  else if (is_pbes_true(p))
  { // p is True
    result = p;
  }
  else if (is_pbes_false(p))
  { // p is False
    result = p;
  }
  else if (is_pbes_forall(p))
  {
    data::variable_list data_vars = var(p);

    // If no data_vars
    if (data_vars.empty())
    {
      pbes_expression expr = pbes_expression_substitute_and_rewrite(arg(p), data, r,use_internal_rewrite_format);
      result = expr;
    }
    else
    { /* Replace the quantified variables of constructor sorts by constructors.
         E.g. forall x:Nat.phi(x) is replaced by phi(0) && forall x':Nat.phi(successor(x')),
         assuming 0 and successor are the constructors of Nat  (which is btw. not the case
         in de data-implementation of mCRL2).  Simplify the resulting expressions. 
         First the substitutions for existing variables that are also bound in the 
         quantifier are saved and reset to the variable. At the end they must be reset. */

      atermpp::map < data::variable, data::data_expression > saved_substitutions;
      for(data::variable_list::const_iterator i=data_vars.begin(); i!=data_vars.end(); ++i)
      { data::data_expression d(pbes_expression_substitute_and_rewrite(*i,data,r,use_internal_rewrite_format));
        
        if (*i!=d)
        { saved_substitutions[*i]=d;
          r->clearSubstitution(*i);
        }
      }
      pbes_expression expr = pbes_expression_substitute_and_rewrite(arg(p), data, r,use_internal_rewrite_format);

      
      data::fresh_variable_generator<> variable_generator(expr, "x");
      unsigned int no_variables=0;
      data::variable_list new_data_vars;
      atermpp::set < pbes_expression > conjunction_set;
      distribute_and(expr,conjunction_set);
      bool constructor_sorts_found=true;
      for( ; constructor_sorts_found ; )
      { constructor_sorts_found=false;
        for (data::variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
        { 

          if (!data.is_constructor_sort(i->sort()))
          { /* The sort of variable i is not a constructor sort.  */
             new_data_vars = atermpp::push_front(new_data_vars, *i);
          }
          else
          {
            atermpp::set <pbes_expression> new_conjunction_set;
            for(atermpp::set <pbes_expression >::iterator t=conjunction_set.begin() ;
                         t!=conjunction_set.end() ; t++)
            {
              if (!occurs_in_varL(*t,*i))
              { new_conjunction_set.insert(*t);
              }
              else
              {
                for (data::data_specification::constructors_const_range rf(data.constructors(i->sort())); !rf.empty(); rf.advance_begin(1))
                { 
                  data::sort_expression_list dsorts;

                  // boost::iterator_range< data::sort_expression_list::const_iterator > dsorts;
                  if (rf.front().sort().is_function_sort())
                  {
                    data::function_sort sa=rf.front().sort();
                    // In case the function f has a sort A->(B->C),
                    // then the function below does not work correctly.
                    // This code must be replaced by enumerator code,
                    // developed by Wieger.
                    if (sa.codomain().is_function_sort())
                    { throw mcrl2::runtime_error("Cannot deal with constructors of type (A->(B->C)): " + 
                                        pp(rf.front()) + ":" + pp(rf.front().sort()));
                    }
                    dsorts=sa.domain();
                  }
                  // else dsorts is empty.

                  // XXXXXXXXXXXXXX argument_sorts(), result_sort()  =source(f->sort());
                  data::variable_list function_arguments;
                  for( data::sort_expression_list::const_iterator s=dsorts.begin() ;
                       s!=dsorts.end() ; ++s )
                  {
                    constructor_sorts_found=constructor_sorts_found || data.is_constructor_sort(*s);
                    data::variable new_variable=variable_generator(*s);
                    ++no_variables;
                    if ((no_variables % 100)==0)
                    { std::cerr << "Used " << no_variables << " variables when eliminating universal quantifier\n";
                      if (!use_internal_rewrite_format)
                      { std::cerr << "Vars: " << mcrl2::data::pp(data_vars) << "\nExpression: " << mcrl2::core::pp(*t) << std::endl;
                      }
                    }
                    new_data_vars = atermpp::push_front(new_data_vars, new_variable);
                    function_arguments = atermpp::push_front(function_arguments, new_variable);
                  }
               
                  pbes_expression d((!function_arguments.empty())?
                                         static_cast<data::data_expression>(data::application(rf.front(), reverse(function_arguments))):
                                         static_cast<data::data_expression>(rf.front()));
                  // sigma[*i]=d;
                  r->setSubstitution(*i,d);
                  pbes_expression rt(pbes_expression_substitute_and_rewrite(*t,data,r,use_internal_rewrite_format));
                  r->clearSubstitution(*i);
                  // sigma[*i] = *i; // erase *i
                  if (pbes_expr::is_pbes_false(rt)) /* the resulting expression is false, so we can terminate */
                  { restore_saved_substitution(saved_substitutions,r,use_internal_rewrite_format);
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
          message.append(" in ").append(mcrl2::core::pp(p));
        }

        throw mcrl2::runtime_error(message);
      }
      result=make_conjunction(conjunction_set);
      restore_saved_substitution(saved_substitutions,r,use_internal_rewrite_format);
    }
  }
  else if (is_pbes_exists(p))
  {
    data::variable_list data_vars = var(p);
    // If no data_vars
    if (data_vars.empty())
    {
      pbes_expression expr = pbes_expression_substitute_and_rewrite(arg(p), data, r,use_internal_rewrite_format);
      result = expr;
    }
    else
    { /* Replace the quantified variables of constructor sorts by constructors.
         E.g. forall x:Nat.phi(x) is replaced by phi(0) || forall x':Nat.phi(successor(x')),
         assuming 0 and successor are the constructors of Nat  (which is btw. not the case
         in de data-implementation of mCRL2).  Simplify the resulting expressions. 
         First the substitutions for existing variables that are also bound in the 
         quantifier are saved and reset to the variable. At the end they must be reset.
      */

      atermpp::map < data::variable, data::data_expression > saved_substitutions;
      for(data::variable_list::const_iterator i=data_vars.begin(); i!=data_vars.end(); ++i)
      { data::data_expression d(pbes_expression_substitute_and_rewrite(*i,data,r,use_internal_rewrite_format));
        
        if (*i!=d)
        { saved_substitutions[*i]=d;
          r->clearSubstitution(*i);
        }
      }

      pbes_expression expr = pbes_expression_substitute_and_rewrite(arg(p), data, r,use_internal_rewrite_format);

      data::fresh_variable_generator<> variable_generator(expr, "x");
      unsigned int no_variables=0;
      data::variable_list new_data_vars;
      atermpp::set < pbes_expression > disjunction_set;
      distribute_or(expr,disjunction_set);
      bool constructor_sorts_found=true;
      for( ; constructor_sorts_found ; )
      { constructor_sorts_found=false;
        for (data::variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
        {
          if (!data.is_constructor_sort(i->sort()))
          { /* The sort of variable i is not a constructor sort.  */
             new_data_vars = atermpp::push_front(new_data_vars, *i);
          }
          else
          {
            atermpp::set <pbes_expression> new_disjunction_set;
            for(atermpp::set <pbes_expression >::iterator t=disjunction_set.begin() ;
                         t!=disjunction_set.end() ; t++)
            {
              if (!occurs_in_varL(*t,*i))
              { new_disjunction_set.insert(*t);
              }
              else
              {
                for (data::data_specification::constructors_const_range rf(data.constructors(i->sort())); !rf.empty(); rf.advance_begin(1))
                {
                  data::sort_expression_list dsorts;
                  if (rf.front().sort().is_function_sort())
                  {
                    data::function_sort sa=rf.front().sort();
                    // In case the function f has a sort A->(B->C),
                    // then the function below does not work correctly.
                    // This code must be replaced by enumerator code,
                    // developed by Wieger.
                    if (sa.codomain().is_function_sort())
                    { throw mcrl2::runtime_error("Cannot deal with constructors of type (A->(B->C)): " + 
                                        pp(rf.front()) + ":" + pp(rf.front().sort()));
                    }
                    dsorts=sa.domain();
                  }
                  // else dsorts is empty.

                  data::variable_list function_arguments;
                  for( data::sort_expression_list::const_iterator s=dsorts.begin() ;
                       s!=dsorts.end() ; ++s )
                  {
                    constructor_sorts_found=constructor_sorts_found || data.is_constructor_sort(*s);
                    data::variable new_variable=variable_generator(*s);
                    ++no_variables;
                    if ((no_variables % 100)==0)
                    { std::cerr << "Used " << no_variables << " variables when eliminating existential quantifier\n";
                      if (!use_internal_rewrite_format)
                      { std::cerr << "Vars: " << mcrl2::data::pp(data_vars) << "\nExpression: " << mcrl2::core::pp(*t) << std::endl;
                      }
                    }
                    new_data_vars = atermpp::push_front(new_data_vars, new_variable);
                    function_arguments = atermpp::push_front(function_arguments, new_variable);
                  }
                  pbes_expression d((!function_arguments.empty())?
                                         static_cast<data::data_expression>(data::application(rf.front(), reverse(function_arguments))):
                                         static_cast<data::data_expression>(rf.front()));
                  r->setSubstitution(*i,d);
                  // sigma[*i]=d;
                  pbes_expression rt(pbes_expression_substitute_and_rewrite(*t,data,r,use_internal_rewrite_format));
                  // sigma[*i] = *i; // erase *i
                  r->clearSubstitution(*i);
                  if (pbes_expr::is_pbes_true(rt)) /* the resulting expression is true, so we can terminate */
                  { restore_saved_substitution(saved_substitutions,r,use_internal_rewrite_format);
                    return pbes_expr::true_();
                  }
                  else
                  { new_disjunction_set.insert(rt);
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
          message.append(" in ").append(mcrl2::core::pp(p));
        }

        throw mcrl2::runtime_error(message);
      }
      result=make_disjunction(disjunction_set);
      restore_saved_substitution(saved_substitutions,r,use_internal_rewrite_format); 
    }
  }
  else if (is_propositional_variable_instantiation(p))
  { // p is a propositional variable
    propositional_variable_instantiation propvar = p;
    core::identifier_string name = propvar.name();
    data::data_expression_list current_parameters(propvar.parameters());
    data::data_expression_list parameters;
    if (use_internal_rewrite_format)
    { 
      atermpp::term_list< data::data_expression > expressions(r->rewriteInternalList(
            atermpp::term_list< data::data_expression >(current_parameters.begin(), current_parameters.end())));
      parameters=data::data_expression_list(expressions.begin(), expressions.end());
    }
    else
    {
      for( data::data_expression_list::const_iterator l=current_parameters.begin();
           l != current_parameters.end(); ++l)
      {
        // parameters = atermpp::push_front(parameters, r(*l,sigma));
        parameters = atermpp::push_front(parameters, (data::data_expression)r->rewrite(*l));
      }
      parameters = atermpp::reverse(parameters);

      atermpp::term_list< data::data_expression > expressions(r->rewriteList(
            atermpp::term_list< data::data_expression >(current_parameters.begin(), current_parameters.end())));
      parameters=data::data_expression_list(expressions.begin(), expressions.end()); 
    }
    result = pbes_expression(propositional_variable_instantiation(name, parameters));
  }
  else
  { // p is a data::data_expression
    if (use_internal_rewrite_format)
    {
      data::data_expression d = (data::data_expression)r->rewriteInternal((atermpp::aterm)p);
      if (is_true_in_internal_rewrite_format(d,r))
      { result = pbes_expr::true_();
      }
      else if (is_false_in_internal_rewrite_format(d,r))
      { result = pbes_expr::false_();
      }
      else
      {
        result = d;
      } 
    }
    else
    {
      data::data_expression d(r->rewrite(p));
      if (d == data::sort_bool::true_())
      { result = pbes_expr::true_();
      }
      else if (d == data::sort_bool::false_())
      { result = pbes_expr::false_();
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
