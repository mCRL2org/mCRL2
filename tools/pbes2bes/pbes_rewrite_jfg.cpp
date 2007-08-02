// Author(s): Alexander van Dam, Jan Friso Groote
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_rewrite_jfg.cpp
/// \brief Add your file description here.

#include "pbes_rewrite.h"
// #include "mcrl2/pbes/utility.h"
#include "print/messaging.h"
#include "atermpp/substitute.h"
#include "atermpp/indexed_set.h"

#include "sort_functions.h"
#include "libstruct.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/sort_utility.h"
#include "pbes_rewrite_jfg.h"

using namespace lps;
using namespace pbes_expr;
using namespace mcrl2::utilities;

// parameterized boolean expression
//<PBExpr>       ::= <DataExpr>
//                 | PBESTrue
//                 | PBESFalse
//                 | PBESAnd(<PBExpr>, <PBExpr>)
//                 | PBESOr(<PBExpr>, <PBExpr>)
//                 | PBESForall(<DataVarId>+, <PBExpr>)
//                 | PBESExists(<DataVarId>+, <PBExpr>)
//                 | <PropVarInst>



static data_expression initialize_internal_trueD(data_expression &t,Rewriter *r)
{
  t=(data_expression)r->toRewriteFormat(data_expr::true_());
  ATprotect((ATerm*)(&t));
  return t;
}

static bool is_true_in_internal_rewrite_formatD(data_expression d,Rewriter *rewriter)
{ static data_expression internal_true=initialize_internal_trueD(internal_true,rewriter);
  return d==internal_true;
}


static data_expression initialize_internal_falseD(data_expression &t,Rewriter *r)
{
  t=(data_expression)r->toRewriteFormat(data_expr::false_());
  ATprotect((ATerm*)(&t));
  return t;
}

static bool is_false_in_internal_rewrite_formatD(data_expression d,Rewriter *rewriter)
{ static data_expression internal_false=initialize_internal_falseD(internal_false,rewriter);
  return d==internal_false;
}


struct compare_data_variableD
{
  aterm v;

  compare_data_variableD(data_variable v_)
    : v(aterm_appl(v_))
  {}
  
  bool operator()(aterm t) const
  {
    return v == t;
  }
};

///\ret variable v occurs in l.
bool occurs_in_var(aterm_appl l, data_variable v)
{
  return find_if(l, compare_data_variableD(v)) != aterm();
}

pbes_expression pbes_expression_rewrite_and_simplifyDEPRECATED(
                   pbes_expression p, 
                   Rewriter *rewriter,
                   const t_tool_options &tool_options)
{
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
    pbes_expression left = pbes_expression_rewrite_and_simplifyDEPRECATED(lhs(p),rewriter,tool_options);
    if (is_false(left))
    { result = false_();
    }
    else
    { pbes_expression right = pbes_expression_rewrite_and_simplifyDEPRECATED(rhs(p),rewriter,tool_options);
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
    pbes_expression left = pbes_expression_rewrite_and_simplifyDEPRECATED(lhs(p),rewriter,tool_options);
    if (is_true(left))
    { result = true_();
    }
    else 
    { pbes_expression right = pbes_expression_rewrite_and_simplifyDEPRECATED(rhs(p),rewriter,tool_options);
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
    pbes_expression expr = pbes_expression_rewrite_and_simplifyDEPRECATED(quant_expr(p),rewriter,tool_options);
    //Remove data_vars which do not occur in expr
    data_variable_list occurred_data_vars;
    for (data_variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
    {
      if (occurs_in_var(expr, *i)) // The var occurs in expr
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
    pbes_expression expr = pbes_expression_rewrite_and_simplifyDEPRECATED(quant_expr(p),rewriter,tool_options);
    //Remove data_vars which does not occur in expr
    data_variable_list occurred_data_vars;
    for (data_variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
    {
      if (occurs_in_var(expr, *i)) // The var occurs in expr
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
    if (tool_options.opt_precompile_pbes)
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
    
    if (tool_options.opt_precompile_pbes)
    { 
    data_expression d = (data_expression)rewriter->rewriteInternal(rewriter->toRewriteFormat(p));
      if (is_true_in_internal_rewrite_formatD(d,rewriter))
         result = true_();
      else if (is_false_in_internal_rewrite_formatD(d,rewriter))
         result = false_();
      else
         result = val(d);
    }
    else
    { 
      data_expression d = rewriter->rewrite(p);
      if (is_true(d))
        result = true_();
      else if (is_false(d))
        result = false_();
      else
        result = val(d);
    }
  }
  
  return result;
}

/**************************************************************************************************/

static void distribute_and(const pbes_expression &expr,atermpp::set < pbes_expression> &conjunction_set)
{ /* distribute the conjuncts of expr over the conjunction_set */
  if (pbes_expr::is_and(expr))
  { distribute_and(lhs(expr),conjunction_set);
    distribute_and(lhs(expr),conjunction_set);
  }
  else
  { conjunction_set.insert(expr);
  }
}

static pbes_expression make_conjunction(const atermpp::set < pbes_expression> &conjunction_set)
{ pbes_expression t=pbes_expr::true_();

  for(atermpp::set < pbes_expression>::iterator i=conjunction_set.begin();
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

static void distribute_or(const pbes_expression &expr,atermpp::set < pbes_expression> &disjunction_set)
{ /* distribute the conjuncts of expr over the conjunction_set */
  if (pbes_expr::is_or(expr))
  { distribute_or(lhs(expr),disjunction_set);
    distribute_or(lhs(expr),disjunction_set);
  }
  else
  { disjunction_set.insert(expr);
  }
}

static pbes_expression make_disjunction(const atermpp::set < pbes_expression> &disjunction_set)
{ pbes_expression t=pbes_expr::false_();

  for(atermpp::set < pbes_expression>::iterator i=disjunction_set.begin();
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

pbes_expression pbes_expression_substitute_and_rewrite(
                   const pbes_expression &p, 
                   const data_specification &data, 
                   Rewriter *rewriter,
                   const t_tool_options &tool_options)
{ 
  pbes_expression result;

  
  if (is_and(p))
  { // p = and(left, right)
    //Rewrite left and right as far as possible
    pbes_expression left = pbes_expression_substitute_and_rewrite(lhs(p), 
                               data, rewriter,tool_options);
    if (is_false(left))
    { result = false_();
    }
    else
    { pbes_expression right = pbes_expression_substitute_and_rewrite(rhs(p), 
                 data, rewriter,tool_options);
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
                 data, rewriter,tool_options);
    if (is_true(left))
    { result = true_();
    }
    else 
    { pbes_expression right = pbes_expression_substitute_and_rewrite(rhs(p), 
                 data, rewriter,tool_options);
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
    pbes_expression expr = pbes_expression_substitute_and_rewrite(quant_expr(p), data, rewriter,tool_options);

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
              if (!occurs_in_var(*t,*i))
              { new_conjunction_set.insert(*t);
              }
              else
              { 
                function_list func=data.constructors(i->sort());
                for (function_list::iterator f=func.begin() ; f!=func.end(); f++)
                { 
                  sort_list domain_sorts=(f->sort()).domain_sorts();
                  data_variable_list function_arguments;
                  for( sort_list::iterator s=domain_sorts.begin() ;
                       s!=domain_sorts.end() ; s++ )
                  { variable_generator.set_sort(*s);
                    constructor_sorts_found=constructor_sorts_found || is_constructorsort(*s,data);
                    data_variable new_data_variable=variable_generator();
                    no_variables++;
                    if ((no_variables % 100)==0)
                    { std::cerr << "Used " << no_variables << " variables when eliminating universal quantifier\n";
                      if (!tool_options.opt_precompile_pbes)
                      { std::cerr << "Vars: " << pp(data_vars) << "\nExpression: " << pp(*t) << std::endl;
                      }
                    }
                    new_data_vars=push_front(new_data_vars,new_data_variable);
                    function_arguments=push_front(function_arguments,new_data_variable);
                  }
                  pbes_expression d(gsMakeDataApplList(*f,reverse(function_arguments)));
                  rewriter->setSubstitution(*i,rewriter->toRewriteFormat(d));
                  pbes_expression r(pbes_expression_substitute_and_rewrite(*t,data,rewriter,tool_options));
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
        if (tool_options.opt_precompile_pbes)
        { std::cerr << "Cannot eliminate universal quantifiers of variables " << pp(new_data_vars) << std::endl;
        }
        else
        { std::cerr << "Cannot eliminate universal quantifiers of variables " << pp(new_data_vars) << " in " << pp(p) << std::endl;
        }
        gsErrorMsg("Aborting\n");
        exit(1);
      }
      result=make_conjunction(conjunction_set);
    }
  }
  else if (is_exists(p))
  { 
    data_variable_list data_vars = quant_vars(p);
    pbes_expression expr = pbes_expression_substitute_and_rewrite(quant_expr(p), data, rewriter,tool_options);

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
              if (!occurs_in_var(*t,*i))
              { new_disjunction_set.insert(*t);
              }
              else
              { 
                function_list func=data.constructors(i->sort());
                for (function_list::iterator f=func.begin() ; f!=func.end(); f++)
                { 
                  sort_list domain_sorts=(f->sort()).domain_sorts();
                  // std::cerr << "Function " << f->name() << " Domain sorts " << domain_sorts << std::endl;
                  data_variable_list function_arguments;
                  for( sort_list::iterator s=domain_sorts.begin() ;
                       s!=domain_sorts.end() ; s++ )
                  { variable_generator.set_sort(*s);
                    constructor_sorts_found=constructor_sorts_found || is_constructorsort(*s,data);
                    data_variable new_data_variable=variable_generator();
                    no_variables++;
                    if ((no_variables % 100)==0)
                    { std::cerr << "Used " << no_variables << " variables when eliminating existential quantifier\n";
                      if (!tool_options.opt_precompile_pbes)
                      { std::cerr << "Vars: " << pp(data_vars) << "\nExpression: " << pp(*t) << std::endl;
                      }
                    }
                    new_data_vars=push_front(new_data_vars,new_data_variable);
                    function_arguments=push_front(function_arguments,new_data_variable);
                  }
                  pbes_expression d(gsMakeDataApplList(*f,reverse(function_arguments)));
                  rewriter->setSubstitution(*i,rewriter->toRewriteFormat(d));
                  pbes_expression r(pbes_expression_substitute_and_rewrite(*t,data,rewriter,tool_options));
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
        if (tool_options.opt_precompile_pbes)
        { std::cerr << "Cannot eliminate existential quantifiers of variables " << pp(new_data_vars) << " in " << pp(p) << std::endl;
        }
        else
        { std::cerr << "Cannot eliminate existential quantifiers of variables " << pp(new_data_vars) << std::endl;
        }
        gsErrorMsg("Aborting\n");
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
    if (tool_options.opt_precompile_pbes)
    { parameters = rewriter->rewriteInternalList(propvar.parameters());
    }
    else
    { parameters = rewriter->rewriteList(propvar.parameters());
    }
    result = pbes_expression(propositional_variable_instantiation(name, parameters));
  }
  else
  { // p is a data_expression
    if (tool_options.opt_precompile_pbes)  
    {
      data_expression d = (data_expression)rewriter->rewriteInternal((aterm)p);
      if (is_true_in_internal_rewrite_formatD(d,rewriter))   
      { result = pbes_expr::true_();
      }
      else if (is_false_in_internal_rewrite_formatD(d,rewriter))
      { result = pbes_expr::false_();
      }
      else
      { 
        result = val(d);
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
        result = val(d);
      }
    }
  }
  
  return result;
}

