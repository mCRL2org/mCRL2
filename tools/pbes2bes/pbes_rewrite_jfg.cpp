// Author(s): Alexander van Dam, Jan Friso Groote
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_rewrite_jfg.cpp
/// \brief Add your file description here.

#include "pbes_rewrite.h"
#include "mcrl2/pbes/utility.h"
#include "print/messaging.h"
#include "atermpp/substitute.h"
#include "atermpp/indexed_set.h"

#include "sort_functions.h"
#include "libstruct.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/sort_utility.h"

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

pbes_expression pbes_expression_substitute_and_rewrite(
                       const pbes_expression &p,
                       const data_specification &data,
                       Rewriter *rewriter);

struct compare_data_variable
{
  aterm v;

  compare_data_variable(data_variable v_)
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
  return find_if(l, compare_data_variable(v)) != aterm();
}

pbes_expression pbes_expression_rewrite_and_simplify(
                   pbes_expression p, 
                   Rewriter *rewriter)
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
    pbes_expression left = pbes_expression_rewrite_and_simplify(lhs(p),rewriter);
    if (is_false(left))
    { result = false_();
    }
    else
    { pbes_expression right = pbes_expression_rewrite_and_simplify(rhs(p),rewriter);
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
    pbes_expression left = pbes_expression_rewrite_and_simplify(lhs(p),rewriter);
    if (is_true(left))
    { result = true_();
    }
    else 
    { pbes_expression right = pbes_expression_rewrite_and_simplify(rhs(p),rewriter);
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
    pbes_expression expr = pbes_expression_rewrite_and_simplify(quant_expr(p),rewriter);
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
    pbes_expression expr = pbes_expression_rewrite_and_simplify(quant_expr(p),rewriter);
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
    data_expression_list parameters = rewriter->rewriteList(propvar.parameters());
    result = pbes_expression(propositional_variable_instantiation(name, parameters));
  }
  else
  { // p is a data_expression
    data_expression d = rewriter->rewrite(p);
    if (is_true(d))
      result = true_();
    else if (is_false(d))
      result = false_();
    else
      result = val(d);
  }
  
  return result;
}



pbes_expression pbes_expression_substitute_and_rewrite(
                   const pbes_expression &p, 
                   const data_specification &data, 
                   Rewriter *rewriter)
{ 
  pbes_expression result;
  
  if (is_and(p))
  { // p = and(left, right)
    //Rewrite left and right as far as possible
    pbes_expression left = pbes_expression_substitute_and_rewrite(lhs(p), 
                               data, rewriter);
    if (is_false(left))
    { result = false_();
    }
    else
    { pbes_expression right = pbes_expression_substitute_and_rewrite(rhs(p), 
                 data, rewriter);
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
                 data, rewriter);
    if (is_true(left))
    { result = true_();
    }
    else 
    { pbes_expression right = pbes_expression_substitute_and_rewrite(rhs(p), 
                 data, rewriter);
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
  { // p == forall(data_expression_list, pbes_expression)

    data_variable_list data_vars = quant_vars(p);
    
    pbes_expression expr = pbes_expression_substitute_and_rewrite(quant_expr(p), data, rewriter);

    // If no data_vars
    if (data_vars.empty())
    { 
      result = expr;
    }
    else
    { /* Replace the quantified variables of constructor sorts by constructors. 
         E.g. forall x:Nat.phi(x) is replaced by phi(0) && forall x':Nat.phi(x').
         Simplify the resulting expressions. */
      fresh_variable_generator variable_generator;
      unsigned int no_variables=0;
      variable_generator.set_context(expr);
      variable_generator.set_hint("x");
      data_variable_list new_data_vars;
      bool constructor_sorts_found=true;
      for( ; constructor_sorts_found ; )
      { constructor_sorts_found=false;
        for (data_variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
        { 
          if (occurs_in_var(expr,*i))
          { pbes_expression resulting_conjunction(pbes_expr::true_());
            if (!is_constructorsort(i->sort(),data))
            { /* The sort of variable i is not a constructor sort.  */
              new_data_vars=push_front(new_data_vars,*i);
            }
            else
            { function_list func=data.constructors(i->sort());
              for (function_list::iterator f=func.begin() ; f!=func.end(); f++)
              { 
                sort_list domain_sorts=(f->sort()).domain_sorts();
                for( sort_list::iterator s=domain_sorts.begin() ;
                     s!=domain_sorts.end() ; s++ )
                { variable_generator.set_sort(*s);
                  constructor_sorts_found=constructor_sorts_found || is_constructorsort(*s,data);
                  data_variable new_data_variable=variable_generator();
                  no_variables++;
                  if ((no_variables % 10)==0)
                  { std::cerr << "Used " << no_variables << " variables when eliminating universal quantifier\n";
                    std::cerr << "Vars: " << pp(data_vars) << "\nExpression: " << pp(expr) << std::endl;
                  }
                  new_data_vars=push_front(new_data_vars,new_data_variable);
                }
                pbes_expression d(gsMakeDataApplList(*f,reverse(new_data_vars)));
                rewriter->setSubstitution(*i,rewriter->toRewriteFormat(d));
                resulting_conjunction=(pbes_expr::is_true(resulting_conjunction)?
                                           pbes_expression_substitute_and_rewrite(expr,data,rewriter):
                                           pbes_expr::and_(
                                              resulting_conjunction,
                                              pbes_expression_substitute_and_rewrite(expr,data,rewriter)));
                rewriter->clearSubstitution(*i);
              }
            }
            expr=resulting_conjunction;
          }
        }
        data_vars=new_data_vars;
        new_data_vars=data_variable_list();
      }

      if (!new_data_vars.empty())
      { 
        std::cerr << "Cannot eliminate universal quantifiers of variables " << pp(new_data_vars) << " in " << pp(p) << std::endl;
        gsErrorMsg("Aborting\n");
        exit(1);
      }
    }
    result=expr;
  }
  else if (is_exists(p))
  { // p = exists(data_expression_list, pbes_expression)
    // std::cerr << "Eliminate exists: " << pp(p) << std::endl;
    data_variable_list data_vars = quant_vars(p);
    
    pbes_expression expr = pbes_expression_substitute_and_rewrite(quant_expr(p), data, rewriter);

    // If no data_vars
    if (data_vars.empty())
      result = expr;
    else
    { /* Replace the quantified variables of constructor sorts by constructors. 
         E.g. forall x:Nat.phi(x) is replaced by phi(0) && forall x':Nat.phi(x').
         Simplify the resulting expressions. */
      fresh_variable_generator variable_generator;
      unsigned int no_variables=0;
      variable_generator.set_context(expr);
      variable_generator.set_hint("x");
      data_variable_list new_data_vars;
      bool constructor_sorts_found=true;
      for( ; constructor_sorts_found ; )
      { constructor_sorts_found=false;
        for (data_variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
        { 
          if (occurs_in_var(expr,*i))
          { pbes_expression resulting_disjunction(pbes_expr::false_());
            if (!is_constructorsort(i->sort(),data))
            { /* The sort of variable i is not a constructor sort.  */
              new_data_vars=push_front(new_data_vars,*i);
            }
            else
            { function_list func=data.constructors(i->sort());
              for (function_list::iterator f=func.begin() ; f!=func.end(); f++)
              { sort_list domain_sorts=(f->sort()).domain_sorts();
                for( sort_list::iterator s=domain_sorts.begin() ;
                     s!=domain_sorts.end() ; s++ )
                { variable_generator.set_sort(*s);
                  constructor_sorts_found=constructor_sorts_found || is_constructorsort(*s,data);
                  data_variable new_data_variable=variable_generator();
                  no_variables++;
                  if ((no_variables % 10)==0)
                  { std::cerr << "Used " << no_variables << " variables when eliminating existential quantifier\n";
                    std::cerr << "Vars: " << pp(data_vars) << "\nExpression: " << pp(expr) << std::endl;
                  }
                  new_data_vars=push_front(new_data_vars,new_data_variable);
                }
                pbes_expression d(gsMakeDataApplList(*f,reverse(new_data_vars)));
                rewriter->setSubstitution(*i,rewriter->toRewriteFormat(d));
                resulting_disjunction=(pbes_expr::is_false(resulting_disjunction)?
                                          pbes_expression_substitute_and_rewrite(expr,data,rewriter):
                                          pbes_expr::or_(
                                              resulting_disjunction,
                                              pbes_expression_substitute_and_rewrite(expr,data,rewriter)));
                rewriter->clearSubstitution(*i);
              }
            }
            expr=resulting_disjunction;
          }
        }
        data_vars=new_data_vars;
        new_data_vars=data_variable_list();
      }

      if (!new_data_vars.empty())
      { 
        std::cerr << "Cannot eliminate existential quantifiers of variables " << pp(new_data_vars) << " in " << pp(p) << std::endl;
        gsErrorMsg("Aborting\n");
        exit(1);
      }
    }
    result=expr;

  }
  else if (is_propositional_variable_instantiation(p))
  { // p is a propositional variable
    propositional_variable_instantiation propvar = p;
    identifier_string name = propvar.name();
    data_expression_list parameters = rewriter->rewriteList(propvar.parameters());
    result = pbes_expression(propositional_variable_instantiation(name, parameters));
  }
  else
  { // p is a data_expression
    data_expression d = rewriter->rewrite(p);
    // std::cerr << "Rewrite: " << pp(p) << "\nResult: " <<  pp(d) << "\n  " << d << std::endl;
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
  
  // std::cerr << "Hier: " << pp(result) << std::endl;
  return result;
}

