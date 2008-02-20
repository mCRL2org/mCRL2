// Author(s): Jan Friso Groote, Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriter2.h
/// \brief Pbes expression rewriters.

#ifndef MCRL2_PBES_REWRITER2_H
#define MCRL2_PBES_REWRITER2_H

#include "mcrl2/pbes/pbes.h"
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/sort_utility.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/core/detail/data_implementation.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/pbes/gauss.h"

namespace mcrl2 {

namespace pbes_system {

// Code below is added by JFG. At certain points an L is added to distinguish
// the function from the original version. The code needs to be cleaned up
// to conform to the standard use in the library.

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

inline pbes_expression pbes_expression_rewrite_and_simplify1(
                   pbes_expression p, 
                   Rewriter *rewriter)
{
  using namespace pbes_system::pbes_expr;
  using namespace pbes_system::accessors;
  pbes_expression result;
  
  if (is_pbes_true(p))
  { // p is True
    result = p;
  }
  else if (is_pbes_false(p))
  { // p is False
    result = p;
  }
  else if (is_pbes_and(p))
  { // p = and(left, right)
    //Rewrite left and right as far as possible
    pbes_expression left = pbes_expression_rewrite_and_simplify1(lhs(p),rewriter);
    if (is_pbes_false(left))
    { result = false_();
    }
    else
    { pbes_expression right = pbes_expression_rewrite_and_simplify1(rhs(p),rewriter);
      //Options for left and right
      if (is_pbes_false(right))
      { result = false_();
      }
      else if (is_pbes_true(left))
      { result = right;
      }
      else if (is_pbes_true(right))
      { result = left;
      }
      else result = and_(left,right);
    }
  }
  else if (is_pbes_or(p))
  { // p = or(left, right)
    //Rewrite left and right as far as possible
    pbes_expression left = pbes_expression_rewrite_and_simplify1(lhs(p),rewriter);
    if (is_pbes_true(left))
    { result = true_();
    }
    else 
    { pbes_expression right = pbes_expression_rewrite_and_simplify1(rhs(p),rewriter);
      if (is_pbes_true(right))
      { result = true_();
      }
      else if (is_pbes_false(left))
      { result = right;
      }
      else if (is_pbes_false(right))
      { result = left;
      }
      else result = or_(left,right);
    }
  }
  else if (is_forall(p))
  { // p = forall(data::data_expression_list, pbes_expression)
    data::data_variable_list data_vars = quant_vars(p);
    pbes_expression expr = pbes_expression_rewrite_and_simplify1(quant_expr(p),rewriter);
    //Remove data_vars which do not occur in expr
    data::data_variable_list occurred_data_vars;
    for (data::data_variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
    {
      if (data::find_data_variable(expr, *i)) // The var occurs in expr
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
  { // p = exists(data::data_expression_list, pbes_expression)
    data::data_variable_list data_vars = quant_vars(p);
    pbes_expression expr = pbes_expression_rewrite_and_simplify1(quant_expr(p),rewriter);
    //Remove data_vars which does not occur in expr
    data::data_variable_list occurred_data_vars;
    for (data::data_variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
    {
      if (data::find_data_variable(expr, *i)) // The var occurs in expr
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
    core::identifier_string name = propvar.name();
    data::data_expression_list parameters = rewriter->rewriteList(propvar.parameters());
    result = pbes_expression(propositional_variable_instantiation(name, parameters));
  }
  else
  { // p is a data::data_expression
    data::data_expression d = rewriter->rewrite(p);
    if (data::data_expr::is_true(d))
    { result = true_();
    }
    else if (data::data_expr::is_false(d))
    { result = false_();
    }
    else
    { result = val(d);
    }
  }
  
  return result;
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
/// also. The function pbes_expression_substitute_and_rewrite1 will continue substituting
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

inline pbes_expression pbes_expression_substitute_and_rewrite1(
                   const pbes_expression &p, 
                   const data::data_specification &data, 
                   Rewriter *rewriter)
{ 
  // std::cerr << "SUBSTANDREWR " << pp(p) << "\n";
  using namespace pbes_system::pbes_expr;
  using namespace pbes_system::accessors;
  pbes_expression result;

  
  if (is_pbes_and(p))
  { // p = and(left, right)
    //Rewrite left and right as far as possible
    pbes_expression left = pbes_expression_substitute_and_rewrite1(lhs(p), 
                               data, rewriter);
    if (is_pbes_false(left))
    { result = false_();
    }
    else
    { pbes_expression right = pbes_expression_substitute_and_rewrite1(rhs(p), 
                 data, rewriter);
      //Options for left and right
      if (is_pbes_false(right))
      { result = false_();
      }
      else if (is_pbes_true(left))
      { result = right;
      }
      else if (is_pbes_true(right))
      { result = left;
      }
      else result = and_(left,right);
    }
  }
  else if (is_pbes_or(p))
  { // p = or(left, right)
    //Rewrite left and right as far as possible
    
    // std::cerr << "SUB&REWR OR: " << pp(p) << "\n";
    pbes_expression left = pbes_expression_substitute_and_rewrite1(lhs(p), 
                 data, rewriter);
    // std::cerr << "SUB&REWR OR LEFT: " << pp(left) << "\n";
    if (is_pbes_true(left))
    { result = true_();
    }
    else 
    { pbes_expression right = pbes_expression_substitute_and_rewrite1(rhs(p), 
                 data, rewriter);
      // std::cerr << "SUB&REWR OR RIGHT: " << pp(right) << "\n";
      if (is_pbes_true(right))
      { result = true_();
      }
      else if (is_pbes_false(left))
      { result = right;
      }
      else if (is_pbes_false(right))
      { result = left;
      }
      else result = or_(left,right);
    }
   // std::cerr << "SUB&REWR OR RESULT: " << pp(result) << "\n";
  }
  else if (is_pbes_true(p))
  { // p is True
    result = p;
  }
  else if (is_pbes_false(p))
  { // p is False
    result = p;
  }
  else if (is_forall(p))
  { 

    data::data_variable_list data_vars = quant_vars(p);
    pbes_expression expr = pbes_expression_substitute_and_rewrite1(quant_expr(p), data, rewriter);

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

      data::fresh_variable_generator variable_generator;
      unsigned int no_variables=0;
      variable_generator.set_context(expr);
      variable_generator.set_hint("x");
      data::data_variable_list new_data_vars;
      atermpp::set < pbes_expression > conjunction_set = pbes_expr::split_and(expr);
      bool constructor_sorts_found=true;
      for( ; constructor_sorts_found ; )
      { constructor_sorts_found=false;
        for (data::data_variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
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
              if (!data::find_data_variable(*t,*i))
              { new_conjunction_set.insert(*t);
              }
              else
              { 
                data::data_operation_list func=data.constructors(i->sort());
                for (data::data_operation_list::iterator f=func.begin() ; f!=func.end(); f++)
                { 
                  data::sort_expression_list dsorts=domain_sorts(f->sort());
                  data::data_variable_list function_arguments;
                  for( data::sort_expression_list::iterator s=dsorts.begin() ;
                       s!=dsorts.end() ; s++ )
                  { variable_generator.set_sort(*s);
                    constructor_sorts_found=constructor_sorts_found || is_constructorsort(*s,data);
                    data::data_variable new_data_variable=variable_generator();
                    no_variables++;
                    if ((no_variables % 100)==0)
                    {
                      std::cerr << "Used " << no_variables << " variables when eliminating universal quantifier\n";
                      std::cerr << "Vars: " << pp(data_vars) << "\nExpression: " << pp(*t) << std::endl;
                    }
                    new_data_vars=push_front(new_data_vars,new_data_variable);
                    function_arguments=push_front(function_arguments,new_data_variable);
                  }
                  pbes_expression d(gsMakeDataApplList(*f,reverse(function_arguments)));
                  rewriter->setSubstitution(*i,rewriter->toRewriteFormat(d));
                  pbes_expression r(pbes_expression_substitute_and_rewrite1(*t,data,rewriter));
                  rewriter->clearSubstitution(*i);
                  if (pbes_expr::is_pbes_false(r)) /* the resulting expression is false, so we can terminate */
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
        new_data_vars=data::data_variable_list();
      }

      if (!new_data_vars.empty())
      { 
        std::cerr << "Cannot eliminate universal quantifiers of variables " << pp(new_data_vars) << " in " << pp(p) << std::endl;
        std::cerr << "Aborting\n";
        exit(1);
      }
      result = pbes_expr::join_and(conjunction_set.begin(), conjunction_set.end());
    }
  }
  else if (is_exists(p))
  { 
    // std::cerr << "EXISTS_: " << pp(p) << "\n";
    data::data_variable_list data_vars = quant_vars(p);
    pbes_expression expr = pbes_expression_substitute_and_rewrite1(quant_expr(p), data, rewriter);
    // std::cerr << "REWRITTEN EXPR " << pp(expr) << "\n";
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

      data::fresh_variable_generator variable_generator;
      unsigned int no_variables=0;
      variable_generator.set_context(expr);
      variable_generator.set_hint("x");
      data::data_variable_list new_data_vars;
      atermpp::set < pbes_expression > disjunction_set = pbes_expr::split_or(expr);
      bool constructor_sorts_found=true;
      for( ; constructor_sorts_found ; )
      { constructor_sorts_found=false;
        for (data::data_variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
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
              if (!data::find_data_variable(*t,*i))
              { new_disjunction_set.insert(*t);
              }
              else
              { 
                data::data_operation_list func=data.constructors(i->sort());
                for (data::data_operation_list::iterator f=func.begin() ; f!=func.end(); f++)
                { 
                  data::sort_expression_list dsorts=domain_sorts(f->sort());
                  // std::cerr << "Function " << f->name() << " Domain sorts " << dsorts << std::endl;
                  data::data_variable_list function_arguments;
                  for( data::sort_expression_list::iterator s=dsorts.begin() ;
                       s!=dsorts.end() ; s++ )
                  { variable_generator.set_sort(*s);
                    constructor_sorts_found=constructor_sorts_found || is_constructorsort(*s,data);
                    data::data_variable new_data_variable=variable_generator();
                    no_variables++;
                    if ((no_variables % 100)==0)
                    { 
                      std::cerr << "Used " << no_variables << " variables when eliminating existential quantifier\n";
                      std::cerr << "Vars: " << pp(data_vars) << "\nExpression: " << pp(*t) << std::endl;
                    }
                    new_data_vars=push_front(new_data_vars,new_data_variable);
                    function_arguments=push_front(function_arguments,new_data_variable);
                  }
                  pbes_expression d(gsMakeDataApplList(*f,reverse(function_arguments)));
                  rewriter->setSubstitution(*i,rewriter->toRewriteFormat(d));
                  // std::cerr << "SETVARIABLE " << pp(*i) << ":=" << pp(d) << "\n";
                  pbes_expression r(pbes_expression_substitute_and_rewrite1(*t,data,rewriter));
                  rewriter->clearSubstitution(*i);
                  if (pbes_expr::is_pbes_true(r)) /* the resulting expression is true, so we can terminate */
                  { // std::cerr << "Return true\n";
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
        new_data_vars=data::data_variable_list();
      }

      if (!new_data_vars.empty())
      { 
        std::cerr << "Cannot eliminate existential quantifiers of variables " << pp(new_data_vars) << std::endl;
        std::cerr << "Aborting\n";
        exit(1);
      }
      result = pbes_expr::join_or(disjunction_set.begin(), disjunction_set.end());
    }
    // std::cerr << "Return result " << pp(result) << "\n";
  }
  else if (is_propositional_variable_instantiation(p))
  { // p is a propositional variable
    propositional_variable_instantiation propvar = p;
    core::identifier_string name = propvar.name();
    data::data_expression_list parameters = rewriter->rewriteList(propvar.parameters());
    result = pbes_expression(propositional_variable_instantiation(name, parameters));
  }
  else
  { // p is a data::data_expression
    data::data_expression d = rewriter->rewrite(p);
    // std::cerr << "REWRITE DATA EXPR: " << pp(p) << " ==> " << pp(d) << "\n";
    if (data::data_expr::is_true(d))
    { result = pbes_expr::true_();
    }
    else if (data::data_expr::is_false(d))
    { result = pbes_expr::false_();
    }
    else
    { 
      result = pbes_expr::val(d);
    }
  } 
  return result;
}

class simplify_rewriter1
{
  data::rewriter datar;
  
  public:
    simplify_rewriter1(const data::data_specification& data)
      : datar(data)
    { }
    
    pbes_expression operator()(pbes_expression p)
    {
      return pbes_expression_rewrite_and_simplify1(p, datar.get_rewriter());
    }   
};

class substitute_rewriter
{
  data::rewriter& datar_;
  const data::data_specification& data_spec;
 
  public:
    substitute_rewriter(data::rewriter& datar, const data::data_specification& data)
      : datar_(datar), data_spec(data)
    { }
    
    pbes_expression operator()(pbes_expression p)
    {
      return pbes_expression_substitute_and_rewrite1(p, data_spec, datar_.get_rewriter());
    }   
};

class substitute_rewriter_jfg
{
  data::rewriter& datar_;
  const data::data_specification& data_spec;
 
  public:
    substitute_rewriter_jfg(data::rewriter& datar, const data::data_specification& data)
      : datar_(datar), data_spec(data)
    { }
    
    pbes_expression operator()(pbes_expression p)
    {
std::cout << "<rewrite>" << pp(p) << std::endl;
std::cout << "<result>" << pp(pbes_expression_substitute_and_rewrite(p, data_spec, datar_.get_rewriter(), false)) << std::endl; 
      return pbes_expression_substitute_and_rewrite(p, data_spec, datar_.get_rewriter(), false);
    }   
};

class pbessolve_rewriter
{
  data::rewriter& datar_;
  const data::data_specification& data_spec;
  int n;
  data_variable_list fv;
  BDD_Prover* prover;
 
  public:
    pbessolve_rewriter(data::rewriter& datar, const data::data_specification& data, RewriteStrategy rewrite_strategy, SMT_Solver_Type solver_type)
      : datar_(datar), data_spec(data), n(0)
    {
      prover = new BDD_Prover(data_spec, rewrite_strategy, 0, false, solver_type, false);
    }

    ~pbessolve_rewriter()
    {
      delete prover;
    }

    pbes_expression operator()(pbes_expression p)
    {
      return pbes_expression_simplify(p, &n, &fv, prover);
    }   
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_JFG_REWRITER_H
