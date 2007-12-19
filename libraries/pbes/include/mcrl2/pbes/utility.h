// Author(s): Wieger Wesselink and Jan Friso Groote
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/utility.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_UTILITY_H
#define MCRL2_PBES_UTILITY_H

#include "mcrl2/pbes/pbes.h"
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/sort_utility.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/core/detail/data_implementation.h" // implement_data_data_expr

// JFG:
#include "mcrl2/data/rewrite.h"

namespace lps {


/// \brief Returns all propositional variables that occur in the term t
template <typename Term>
std::set<propositional_variable> find_propositional_variables(Term t)
{
  std::set<propositional_variable> variables;
  atermpp::find_all_if(t, is_propositional_variable, std::inserter(variables, variables.end()));
  return variables;
}

/// \brief Returns all propositional variable instantiations that occur in the term t
template <typename Term>
std::set<propositional_variable_instantiation> find_propositional_variable_instantiations(Term t)
{
  std::set<propositional_variable_instantiation> variables;
  atermpp::find_all_if(t, is_propositional_variable_instantiation, std::inserter(variables, variables.end()));
  return variables;
}

// Code below is added by JFG. At certain points an L is added to distinguish
// the function from the original version. The code needs to be cleaned up
// to conform to the standard use in the library.


static data_expression initialize_internal_true(data_expression &t,Rewriter *r)
{
  t=(data_expression)r->toRewriteFormat(data_expr::true_());
  ATprotect((ATerm*)(&t));
  return t;
}

static bool is_true_in_internal_rewrite_format(data_expression d,Rewriter *rewriter)
{ static data_expression internal_true=initialize_internal_true(internal_true,rewriter);
  return d==internal_true;
}


static data_expression initialize_internal_false(data_expression &t,Rewriter *r)
{
  t=(data_expression)r->toRewriteFormat(data_expr::false_());
  ATprotect((ATerm*)(&t));
  return t;
}

static bool is_false_in_internal_rewrite_format(data_expression d,Rewriter *rewriter)
{ static data_expression internal_false=initialize_internal_false(internal_false,rewriter);
  return d==internal_false;
}


struct compare_data_variableL
{
  aterm v;

  compare_data_variableL(data_variable v_)
    : v(aterm_appl(v_))
  {}

  bool operator()(aterm t) const
  {
    return v == t;
  }
};

//  variable v occurs in l.
//
static bool occurs_in_varL(aterm_appl l, data_variable v)
{
  return find_if(l, compare_data_variableL(v)) != aterm();
}


///\brief returns a pbes expression which has been rewritten en where unnecessary quantifications have been removed. The result can optionally be translated to internal rewrite format.
/// This function simplifies all data expressions in p by applying the rewriter to it. 
/// Data expressions that are true or false are translated to the pbes expressions true and false.
/// Quantified variables that do not occur in the body are removed.
/// Conjunctions and disjunctions of which one of the arguments is true or false are simplified.
/// If the boolean yield_internal_rewriter_format is set, the data expressions in the resulting
/// pbes expressions are translated into the internal format belonging to the rewriter. The 
/// advantage of this is that the rewriter does not have to translate the data expressions
/// to internal format the next time the rewriter is applied to it. This is for instance useful
/// in the tool pbes2bool (or pbes2bes) where pbes expressions must iteratively be rewritten.

inline pbes_expression pbes_expression_rewrite_and_simplify(
                   pbes_expression p, 
                   Rewriter *rewriter,
                   const bool yield_internal_rewriter_format=false)
{
  using namespace pbes_expr;
  pbes_expression result;
  
  if (is_true(p))
  { // p is True
    result = p;
  }
  else if (is_false(p))
  { // p is False
    result = p;
  }
  else if (is_and(p))
  { // p = and(left, right)
    //Rewrite left and right as far as possible
    pbes_expression left = pbes_expression_rewrite_and_simplify(lhs(p),rewriter,yield_internal_rewriter_format);
    if (is_false(left))
    { result = false_();
    }
    else
    { pbes_expression right = pbes_expression_rewrite_and_simplify(rhs(p),rewriter,yield_internal_rewriter_format);
      //Options for left and right
      if (is_false(right))
      { result = false_();
      }
      else if (is_true(left))
      { result = right;
      }
      else if (is_true(right))
      { result = left;
      }
      else result = and_(left,right);
    }
  }
  else if (is_or(p))
  { // p = or(left, right)
    //Rewrite left and right as far as possible
    pbes_expression left = pbes_expression_rewrite_and_simplify(lhs(p),rewriter,yield_internal_rewriter_format);
    if (is_true(left))
    { result = true_();
    }
    else 
    { pbes_expression right = pbes_expression_rewrite_and_simplify(rhs(p),rewriter,yield_internal_rewriter_format);
      if (is_true(right))
      { result = true_();
      }
      else if (is_false(left))
      { result = right;
      }
      else if (is_false(right))
      { result = left;
      }
      else result = or_(left,right);
    }
  }
  else if (is_forall(p))
  { // p = forall(data_expression_list, pbes_expression)
    data_variable_list data_vars = quant_vars(p);
    pbes_expression expr = pbes_expression_rewrite_and_simplify(quant_expr(p),rewriter,yield_internal_rewriter_format);
    //Remove data_vars which do not occur in expr
    data_variable_list occurred_data_vars;
    for (data_variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
    {
      if (occurs_in_varL(expr, *i)) // The var occurs in expr
      { occurred_data_vars = push_front(occurred_data_vars, *i);
      }
    }

    // If no data_vars
    if (occurred_data_vars.empty())
    { result = expr;
    }
    else
    { result=forall(occurred_data_vars,expr);
    }
  }
  else if (is_exists(p))
  { // p = exists(data_expression_list, pbes_expression)
    data_variable_list data_vars = quant_vars(p);
    pbes_expression expr = pbes_expression_rewrite_and_simplify(quant_expr(p),rewriter,yield_internal_rewriter_format);
    //Remove data_vars which does not occur in expr
    data_variable_list occurred_data_vars;
    for (data_variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
    {
      if (occurs_in_varL(expr, *i)) // The var occurs in expr
      { occurred_data_vars = push_front(occurred_data_vars, *i);
      }
    }
    
    //If no data_vars remaining
    if (occurred_data_vars.empty())
      result = expr;
    else
    { result=exists(occurred_data_vars,expr);
    }
  }
  else if (is_propositional_variable_instantiation(p))
  { // p is a propositional variable
    propositional_variable_instantiation propvar = p;
    identifier_string name = propvar.name();
    data_expression_list parameters;
    if (yield_internal_rewriter_format)
    { 
      for( data_expression_list::iterator l=propvar.parameters().begin();
           l!=propvar.parameters().end(); l++)
      { 
        parameters=push_front(parameters,
                              (data_expression)rewriter->rewriteInternal(
                                rewriter->toRewriteFormat(*l)));
      }
      parameters=reverse(parameters);
    }
    else
    { parameters=rewriter->rewriteList(propvar.parameters());
    }
    result = pbes_expression(propositional_variable_instantiation(name, parameters));
  }
  else
  { // p is a data_expression
    
    if (yield_internal_rewriter_format)
    { 
    data_expression d = (data_expression)rewriter->rewriteInternal(rewriter->toRewriteFormat(p));
      if (is_true_in_internal_rewrite_format(d,rewriter))
         result = true_();
      else if (is_false_in_internal_rewrite_format(d,rewriter))
         result = false_();
      else
         result = val(d);
    }
    else
    { 
      data_expression d = rewriter->rewrite(p);
      if (data_expr::is_true(d))
        result = true_();
      else if (data_expr::is_false(d))
        result = false_();
      else
        result = val(d);
    }
  }
  
  return result;
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

template <typename Container>
inline pbes_expression give_the_instantiated_rhs( 
                   const propositional_variable_instantiation current_variable_instantiation, 
                   pbes<Container> pbes_spec,
                   Rewriter *rewriter,
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

  data_expression_list::iterator elist=current_variable_instantiation.parameters().begin();
      
  for(data_variable_list::iterator vlist=current_pbeq.variable().parameters().begin() ;
               vlist!=current_pbeq.variable().parameters().end() ; vlist++)
  { 
    assert(elist!=current_variable_instantiation.parameters().end());

    if (use_internal_rewriter_format)
    { rewriter->setSubstitution(*vlist,(aterm)*elist);
    }
    else
    { 
      rewriter->setSubstitution(*vlist,rewriter->toRewriteFormat(*elist));
    }
    elist++;
  }
  assert(elist==current_variable_instantiation.parameters().end());
  return  pbes_expression_substitute_and_rewrite(
                                current_pbeq.formula(), 
                                pbes_spec.data(),
                                rewriter,
                                use_internal_rewriter_format);
}




/**************************************************************************************************/

// given a pbes expression of the form p1 && p2 && .... && pn, this will yield a 
// set of the form { p1,p2, ..., pn}, assuming that pi does not have a && as main 
// function symbol.

static void distribute_and(const pbes_expression &expr,atermpp::set < pbes_expression> &conjunction_set)
{ /* distribute the conjuncts of expr over the conjunction_set */
  if (pbes_expr::is_and(expr))
  { distribute_and(pbes_expr::lhs(expr),conjunction_set);
    distribute_and(pbes_expr::rhs(expr),conjunction_set);
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
  { if (pbes_expr::is_true(t))
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
  if (pbes_expr::is_or(expr))
  { distribute_or(pbes_expr::lhs(expr),disjunction_set);
    distribute_or(pbes_expr::rhs(expr),disjunction_set);
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
  { if (pbes_expr::is_false(t))
    { t=*i;
    }
    else
    { t=pbes_expr::or_(*i,t);
    } 
  }
  return t;
}

///\brief Substitute in p, remove quantifiers and return the result after rewriting.
/// Given a substitution, which is prepared within the rewriter, this function first
/// applies this substitution. Then it will attempt to eliminate all quantifiers.
/// I.e. if S is a constructorsort (which means that there exists at least one constructor 
/// with S as target sort), then an expression of the form forall x:S.p(x) is replaced
/// by forall x1..xn.p(f1(x1..xn) &&  forall x1..xm.p(f2(x1..xm)) && .... for all
/// constructors fi with S as targetsort (this is done similarly for the exists). 
/// if the constructors are constants (i.e. n=0, m=0, etc.), no new quantifications
/// will be generated. And if the expressions are not constant, expressions such as
/// p(f1(x1..xn)) are simplified, which can lead to the situation that certain 
/// variables x1..xn do not occur anymore. In that case the quantors can be removed
/// also. The function pbes_expression_substitute_and_rewrite will continue substituting
/// constructors for quantified variables until there are no variables left, or
/// until there are only quantifications over non constructor sorts. In the last case,
/// the function will halt with an exit(1). For every 100 new variables being used
/// in new quantifications, a message is printed.
///   The data_specification is needed to determine the constructors of a certain 
/// sort. The rewriter is used to perform the rewriting steps. The rewriter can
/// either work on internal rewriting format, in which case all data expressions in
/// the pbes are already in internal rewriting format. In this case the data expressions
/// in the result are also in internal format. The use of internal format saves
/// a lot of internal compilation time.

inline pbes_expression pbes_expression_substitute_and_rewrite(
                   const pbes_expression &p, 
                   const data_specification &data, 
                   Rewriter *rewriter,
                   const bool use_internal_rewrite_format)
{ 
  using namespace pbes_expr;
  pbes_expression result;

  
  if (is_and(p))
  { // p = and(left, right)
    //Rewrite left and right as far as possible
    pbes_expression left = pbes_expression_substitute_and_rewrite(lhs(p), 
                               data, rewriter,use_internal_rewrite_format);
    if (is_false(left))
    { result = false_();
    }
    else
    { pbes_expression right = pbes_expression_substitute_and_rewrite(rhs(p), 
                 data, rewriter,use_internal_rewrite_format);
      //Options for left and right
      if (is_false(right))
      { result = false_();
      }
      else if (is_true(left))
      { result = right;
      }
      else if (is_true(right))
      { result = left;
      }
      else result = and_(left,right);
    }
  }
  else if (is_or(p))
  { // p = or(left, right)
    //Rewrite left and right as far as possible
    pbes_expression left = pbes_expression_substitute_and_rewrite(lhs(p), 
                 data, rewriter,use_internal_rewrite_format);
    if (is_true(left))
    { result = true_();
    }
    else 
    { pbes_expression right = pbes_expression_substitute_and_rewrite(rhs(p), 
                 data, rewriter,use_internal_rewrite_format);
      if (is_true(right))
      { result = true_();
      }
      else if (is_false(left))
      { result = right;
      }
      else if (is_false(right))
      { result = left;
      }
      else result = or_(left,right);
    }
  }
  else if (is_true(p))
  { // p is True
    result = p;
  }
  else if (is_false(p))
  { // p is False
    result = p;
  }
  else if (is_forall(p))
  { 

    data_variable_list data_vars = quant_vars(p);
    pbes_expression expr = pbes_expression_substitute_and_rewrite(quant_expr(p), data, rewriter,use_internal_rewrite_format);

    // If no data_vars
    if (data_vars.empty())
    { 
      result = expr;
    }
    else
    { /* Replace the quantified variables of constructor sorts by constructors. 
         E.g. forall x:Nat.phi(x) is replaced by phi(0) && forall x':Nat.phi(successor(x')),
         assuming 0 and successor are the constructors of Nat  (which is btw. not the case
         in de data-implementation of mCRL2).  Simplify the resulting expressions. */ 

      fresh_variable_generator variable_generator;
      unsigned int no_variables=0;
      variable_generator.set_context(expr);
      variable_generator.set_hint("x");
      data_variable_list new_data_vars;
      atermpp::set < pbes_expression > conjunction_set;
      distribute_and(expr,conjunction_set);
      bool constructor_sorts_found=true;
      for( ; constructor_sorts_found ; )
      { constructor_sorts_found=false;
        for (data_variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
        { 
          if (!is_constructorsort(i->sort(),data))
          { /* The sort of variable i is not a constructor sort.  */
                new_data_vars=push_front(new_data_vars,*i);
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
                data_operation_list func=data.constructors(i->sort());
                for (data_operation_list::iterator f=func.begin() ; f!=func.end(); f++)
                { 
                  sort_expression_list dsorts=domain_sorts(f->sort());
                  data_variable_list function_arguments;
                  for( sort_expression_list::iterator s=dsorts.begin() ;
                       s!=dsorts.end() ; s++ )
                  { variable_generator.set_sort(*s);
                    constructor_sorts_found=constructor_sorts_found || is_constructorsort(*s,data);
                    data_variable new_data_variable=variable_generator();
                    no_variables++;
                    if ((no_variables % 100)==0)
                    { std::cerr << "Used " << no_variables << " variables when eliminating universal quantifier\n";
                      if (!use_internal_rewrite_format)
                      { std::cerr << "Vars: " << pp(data_vars) << "\nExpression: " << pp(*t) << std::endl;
                      }
                    }
                    new_data_vars=push_front(new_data_vars,new_data_variable);
                    function_arguments=push_front(function_arguments,new_data_variable);
                  }
                  pbes_expression d(gsMakeDataApplList(*f,reverse(function_arguments)));
                  rewriter->setSubstitution(*i,rewriter->toRewriteFormat(d));
                  pbes_expression r(pbes_expression_substitute_and_rewrite(*t,data,rewriter,use_internal_rewrite_format));
                  rewriter->clearSubstitution(*i);
                  if (pbes_expr::is_false(r)) /* the resulting expression is false, so we can terminate */
                  { 
                    return pbes_expr::false_();
                  }
                  else 
                  { new_conjunction_set.insert(r);
                  }
                }
              }
            }
            conjunction_set=new_conjunction_set;
          }
        }
        data_vars=new_data_vars;
        new_data_vars=data_variable_list();
      }

      if (!new_data_vars.empty())
      { 
        if (use_internal_rewrite_format)
        { std::cerr << "Cannot eliminate universal quantifiers of variables " << pp(new_data_vars) << std::endl;
        }
        else
        { std::cerr << "Cannot eliminate universal quantifiers of variables " << pp(new_data_vars) << " in " << pp(p) << std::endl;
        }
        std::cerr << "Aborting\n";
        exit(1);
      }
      result=make_conjunction(conjunction_set);
    }
  }
  else if (is_exists(p))
  { 
    data_variable_list data_vars = quant_vars(p);
    pbes_expression expr = pbes_expression_substitute_and_rewrite(quant_expr(p), data, rewriter,use_internal_rewrite_format);

    // If no data_vars
    if (data_vars.empty())
    { 
      result = expr;
    }
    else
    { /* Replace the quantified variables of constructor sorts by constructors. 
         E.g. forall x:Nat.phi(x) is replaced by phi(0) || forall x':Nat.phi(successor(x')),
         assuming 0 and successor are the constructors of Nat  (which is btw. not the case
         in de data-implementation of mCRL2).  Simplify the resulting expressions. */ 

      fresh_variable_generator variable_generator;
      unsigned int no_variables=0;
      variable_generator.set_context(expr);
      variable_generator.set_hint("x");
      data_variable_list new_data_vars;
      atermpp::set < pbes_expression > disjunction_set;
      distribute_or(expr,disjunction_set);
      bool constructor_sorts_found=true;
      for( ; constructor_sorts_found ; )
      { constructor_sorts_found=false;
        for (data_variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
        { 
          if (!is_constructorsort(i->sort(),data))
          { /* The sort of variable i is not a constructor sort.  */
                new_data_vars=push_front(new_data_vars,*i);
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
                data_operation_list func=data.constructors(i->sort());
                for (data_operation_list::iterator f=func.begin() ; f!=func.end(); f++)
                { 
                  sort_expression_list dsorts=domain_sorts(f->sort());
                  // std::cerr << "Function " << f->name() << " Domain sorts " << dsorts << std::endl;
                  data_variable_list function_arguments;
                  for( sort_expression_list::iterator s=dsorts.begin() ;
                       s!=dsorts.end() ; s++ )
                  { variable_generator.set_sort(*s);
                    constructor_sorts_found=constructor_sorts_found || is_constructorsort(*s,data);
                    data_variable new_data_variable=variable_generator();
                    no_variables++;
                    if ((no_variables % 100)==0)
                    { std::cerr << "Used " << no_variables << " variables when eliminating existential quantifier\n";
                      if (!use_internal_rewrite_format)
                      { std::cerr << "Vars: " << pp(data_vars) << "\nExpression: " << pp(*t) << std::endl;
                      }
                    }
                    new_data_vars=push_front(new_data_vars,new_data_variable);
                    function_arguments=push_front(function_arguments,new_data_variable);
                  }
                  pbes_expression d(gsMakeDataApplList(*f,reverse(function_arguments)));
                  rewriter->setSubstitution(*i,rewriter->toRewriteFormat(d));
                  pbes_expression r(pbes_expression_substitute_and_rewrite(*t,data,rewriter,use_internal_rewrite_format));
                  rewriter->clearSubstitution(*i);
                  if (pbes_expr::is_true(r)) /* the resulting expression is true, so we can terminate */
                  { 
                    return pbes_expr::true_();
                  }
                  else 
                  { new_disjunction_set.insert(r);
                  }
                }
              }
            }
            disjunction_set=new_disjunction_set;
          }
        }
        data_vars=new_data_vars;
        new_data_vars=data_variable_list();
      }

      if (!new_data_vars.empty())
      { 
        if (use_internal_rewrite_format)
        { std::cerr << "Cannot eliminate existential quantifiers of variables " << pp(new_data_vars) << " in " << pp(p) << std::endl;
        }
        else
        { std::cerr << "Cannot eliminate existential quantifiers of variables " << pp(new_data_vars) << std::endl;
        }
        std::cerr << "Aborting\n";
        exit(1);
      }
      result=make_disjunction(disjunction_set);
    }
  }
  else if (is_propositional_variable_instantiation(p))
  { // p is a propositional variable
    propositional_variable_instantiation propvar = p;
    identifier_string name = propvar.name();
    data_expression_list parameters;
    if (use_internal_rewrite_format)
    { parameters = rewriter->rewriteInternalList(propvar.parameters());
    }
    else
    { parameters = rewriter->rewriteList(propvar.parameters());
    }
    result = pbes_expression(propositional_variable_instantiation(name, parameters));
  }
  else
  { // p is a data_expression
    if (use_internal_rewrite_format)  
    {
      data_expression d = (data_expression)rewriter->rewriteInternal((aterm)p);
      if (is_true_in_internal_rewrite_format(d,rewriter))   
      { result = pbes_expr::true_();
      }
      else if (is_false_in_internal_rewrite_format(d,rewriter))
      { result = pbes_expr::false_();
      }
      else
      { 
        result = pbes_expr::val(d);
      }
    }
    else
    {
      data_expression d = rewriter->rewrite(p);
      if (data_expr::is_true(d))
      { result = pbes_expr::true_();
      }
      else if (data_expr::is_false(d))
      { result = pbes_expr::false_();
      }
      else
      { 
        result = pbes_expr::val(d);
      }
    }
  }
  
  return result;
}


} // namespace lps

#endif // MCRL2_PBES_UTILITY_H
