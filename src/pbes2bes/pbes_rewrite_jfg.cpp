#include "pbes_rewrite.h"
#include "lps/pbes_utility.h"
#include "atermpp/substitute.h"
#include "atermpp/indexed_set.h"

#include "sort_functions.h"

using namespace lps;
using namespace pbes_expr;

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
                       pbes_expression p,
                       data_specification data,
                       Rewriter *rewriter);

bool element_in_propvarinstlist(data_variable_list vars, std::set< propositional_variable_instantiation > pvilist)
{
  bool result = false;
   for (std::set< propositional_variable_instantiation >::iterator pvi = pvilist.begin(); pvi != pvilist.end(); pvi++)
  {
    for (data_variable_list::iterator dvl = vars.begin(); dvl != vars.end(); dvl++)
    {
      if (occurs_in(*dvl, *pvi))
      {
        result = true;
      }
    } 
  }
  return result;
}

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
bool occurs_in(aterm_appl l, data_variable v)
{
  return find_if(l, compare_data_variable(v)) != aterm();
}

pbes_expression_list get_all_possible_expressions(data_variable_list data_vars, pbes_expression pbexp, data_specification data)
{
  // Create a pbes_expression for each possible instantiations of the variables and put those in a list.
  atermpp::set< pbes_expression > set_result;
  set_result.insert(pbexp);
  for (data_variable_list::iterator vars = data_vars.begin(); vars != data_vars.end(); vars++)
  {
    atermpp::set< pbes_expression > intermediate;
    for (atermpp::set< pbes_expression >::iterator exp = set_result.begin(); exp != set_result.end(); exp++)
    {
      data_expression_list enumerations = enumerate_constructors(data.constructors(), vars->sort());
      for (data_expression_list::iterator enums = enumerations.begin(); enums != enumerations.end(); enums++)
      {
        pbes_expression toAdd = *exp;
        toAdd = toAdd.substitute(make_substitution(*vars, *enums));
        intermediate.insert(toAdd);
      }
    }
    set_result = intermediate;
  }
  pbes_expression_list result;
  for (atermpp::set< pbes_expression >::iterator exp = set_result.begin(); exp != set_result.end(); exp++)
  {
    result = push_front(result, *exp);
  }
  
  return reverse(result);
}

pbes_expression_list get_and_expressions(pbes_expression_list and_list, data_specification data, Rewriter *rewriter)
{
  // From a pbes_expression_list: Remove all expressions which are true.
  // Return a list with only one element False, if an element in the original list is false.
  pbes_expression_list result;
  bool is_pbes_false = false;
  
  pbes_expression_list::iterator it_enum = and_list.begin();
  while ((it_enum != and_list.end()) && (!is_pbes_false))
  {
    pbes_expression current = pbes_expression_substitute_and_rewrite(*it_enum, data, rewriter);
    if (is_false(current))
    {
      pbes_expression_list list_false;
      list_false = push_front(list_false, false_());
      result = list_false;
      is_pbes_false = true;
    }
    else if (!is_true(current))
      result = push_front(result, current);
    it_enum++;
  }
  return reverse(result);
}

pbes_expression_list get_or_expressions(
            pbes_expression_list or_list, 
            data_specification data, 
            Rewriter *rewriter)
{
  // From a pbes_expression_list: Remove all expressions which are false.
  // Return a list with only one element False, if an element in the original list is true.
  pbes_expression_list result;
  bool is_pbes_true = false;
  
  pbes_expression_list::iterator it_enum = or_list.begin();
  while ((it_enum != or_list.end()) && (!is_pbes_true))
  {
    pbes_expression current = pbes_expression_substitute_and_rewrite(*it_enum, data, rewriter);
    if (is_true(current))
    {
      pbes_expression_list list_true;
      list_true = push_front(list_true, true_());
      result = list_true;
      is_pbes_true = true;
    }
    else if (!is_false(current))
      result = push_front(result, current);
    it_enum++;
  }
  return reverse(result);
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
    if (left.is_false())
    { result = false_();
    }
    else
    { pbes_expression right = pbes_expression_rewrite_and_simplify(rhs(p),rewriter);
      //Options for left and right
      if (right.is_false())
      { result = false_();
      }
      else if (left.is_true())
      { result = right;
      }
      else if (right.is_true())
      { result = left;
      }
      else result = and_(left,right);
    }
  }
  else if (is_or(p))
  { // p = or(left, right)
    //Rewrite left and right as far as possible
    pbes_expression left = pbes_expression_rewrite_and_simplify(lhs(p),rewriter);
    if (left.is_true())
    { result = true_();
    }
    else 
    { pbes_expression right = pbes_expression_rewrite_and_simplify(rhs(p),rewriter);
      if (right.is_true())
      { result = true_();
      }
      else if (left.is_false())
      { result = right;
      }
      else if (right.is_false())
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
      if (occurs_in(expr, *i)) // The var occurs in expr
      { occurred_data_vars = push_back(occurred_data_vars, *i);
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
      if (occurs_in(expr, *i)) // The var occurs in expr
      { occurred_data_vars = push_back(occurred_data_vars, *i);
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
    if (d.is_true())
      result = true_();
    else if (d.is_false())
      result = false_();
    else
      result = val(d);
  }
  
  return result;
}



pbes_expression pbes_expression_substitute_and_rewrite(
                   pbes_expression p, 
                   data_specification data, 
                   Rewriter *rewriter)
{ 
  pbes_expression result;
  
  if (is_and(p))
  { // p = and(left, right)
    //Rewrite left and right as far as possible
    pbes_expression left = pbes_expression_substitute_and_rewrite(lhs(p), 
                               data, rewriter);
    if (left.is_false())
    { result = false_();
    }
    else
    { pbes_expression right = pbes_expression_substitute_and_rewrite(rhs(p), 
                 data, rewriter);
      //Options for left and right
      if (right.is_false())
      { result = false_();
      }
      else if (left.is_true())
      { result = right;
      }
      else if (right.is_true())
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
    if (left.is_true())
    { result = true_();
    }
    else 
    { pbes_expression right = pbes_expression_substitute_and_rewrite(rhs(p), 
                 data, rewriter);
      if (right.is_true())
      { result = true_();
      }
      else if (left.is_false())
      { result = right;
      }
      else if (right.is_false())
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
  { // p = forall(data_expression_list, pbes_expression)
    data_variable_list data_vars = quant_vars(p);
    pbes_expression expr = pbes_expression_substitute_and_rewrite(quant_expr(p), data, rewriter);
    //Remove data_vars which do not occur in expr
    data_variable_list occurred_data_vars;
    for (data_variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
    {
      if (occurs_in(expr, *i)) // The var occurs in expr
      { occurred_data_vars = push_back(occurred_data_vars, *i);
      }
    }

    // If no data_vars
    if (occurred_data_vars.empty())
      result = expr;
    else
    {
      data_vars = occurred_data_vars;
      //If the forall  has only finite data variables, make a conjunction out of it.
      if (check_finite_list(data.constructors(), get_sorts(data_vars)))
      {
        pbes_expression_list and_list = get_and_expressions(get_all_possible_expressions(data_vars, expr, data), data, rewriter);
        result = multi_and(and_list.begin(), and_list.end());
      }
      //Probably some advanced stuff is needed here to check finiteness...
      else
      { std::cerr << "HIER NIET" << pp(p) << std::endl;
        gsErrorMsg("The propositional_variable_instantiation cannot be replaced\n");
        gsErrorMsg("Aborting\n");
        exit(1);
      }
    }
  }
  else if (is_exists(p))
  { // p = exists(data_expression_list, pbes_expression)
    data_variable_list data_vars = quant_vars(p);
    pbes_expression expr = pbes_expression_substitute_and_rewrite(quant_expr(p), data, rewriter);
    //Remove data_vars which does not occur in expr
    data_variable_list occurred_data_vars;
    for (data_variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
    {
      if (occurs_in(expr, *i)) // The var occurs in expr
        occurred_data_vars = push_back(occurred_data_vars, *i);
    }
    
    //If no data_vars remaining
    if (occurred_data_vars.empty())
      result = expr;
    else
    {
      data_vars = occurred_data_vars;
      //If expression is true or false -> return it
      if (expr.is_true() || expr.is_false())
        result = expr;
      //If the exists  has only finite data variables, make a conjunction out of it.
      else if (check_finite_list(data.constructors(), get_sorts(data_vars)))
      {
        pbes_expression_list or_list = get_or_expressions(get_all_possible_expressions(data_vars, expr, data), data, rewriter);
        result = multi_or(or_list.begin(), or_list.end());
      }
      else 
      { std::cerr << "HIER NIET2" << pp(p) << std::endl;
        gsErrorMsg("The propositional_variable_instantiation cannot be replaced\n");
        gsErrorMsg("Aborting\n");
        exit(1);
      }
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
    if (d.is_true())
      result = true_();
    else if (d.is_false())
      result = false_();
    else
      result = val(d);
  }
  
  return result;
}

