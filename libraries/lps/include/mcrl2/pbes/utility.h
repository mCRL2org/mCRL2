// Author(s): Wieger Wesselink
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
#include "mcrl2/basic/mucalculus.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/sort.h"
#include "atermpp/algorithm.h"
#include "dataimpl.h" // implement_data_data_expr

// JFG:
#include "librewrite.h"

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

/// Converts a pbes expression to a data expression. Note that the specification
/// spec may change as a result of this.
inline
data_expression pbes2data(const pbes_expression& p, specification& spec)
{
  using namespace pbes_expr;
  namespace d = lps::data_expr;
  namespace s = lps::sort_expr;
  
  if (is_data(p)) {
    return val(p);
  } else if (is_true(p)) {
    return d::true_();
  } else if(is_false(p)) {
    return d::false_();
  } else if (is_and(p)) {
    return d::and_(pbes2data(lhs(p), spec), pbes2data(rhs(p), spec));
  } else if (is_or(p)) {
    return d::or_(pbes2data(lhs(p), spec), pbes2data(rhs(p), spec));
  } else if (is_forall(p)) {
      aterm_appl x = gsMakeBinder(gsMakeForall(), quant_vars(p), pbes2data(quant_expr(p), spec));
      return implement_data_data_expr(x, spec);
  } else if (is_exists(p)) {
      aterm_appl x = gsMakeBinder(gsMakeExists(), quant_vars(p), pbes2data(quant_expr(p), spec));
      return implement_data_data_expr(x, spec);
  } else if (is_propositional_variable_instantiation(p)) {
    identifier_string vname = var_name(p);
    data_expression_list parameters = var_val(p);
    sort_list sorts = apply(parameters, gsGetSort);
    // In order to use gsMakeSortArrow sorts must be non-empty
    // else an extra case should be added to just make vsort == s::bool_();
    assert(!sorts.empty());
    lps::sort vsort = gsMakeSortArrow(sorts, s::bool_());
    data_variable v(gsMakeDataVarId(vname, vsort));
    return gsMakeDataApplList(v, parameters);
  }
  throw std::runtime_error(std::string("pbes2data error: unknown pbes_variable_instantiation ") + p.to_string());
  return data_expression(); // to prevent compiler warnings
}

/// Converts a data expression to a pbes expression.
inline
pbes_expression data2pbes(data_expression q)
{
  namespace d = lps::data_expr;
  namespace p = lps::pbes_expr;
  namespace s = lps::sort_expr;

  data_expression head = q.head();
  data_expression_list arguments = q.arguments();
  if (is_data_variable(head))
  {
    return propositional_variable_instantiation(data_variable(head).name(), arguments);
  }
  else // head must be an operation id
  {
    assert(is_function(head));
    if (d::is_true(head)) {
      return p::true_();
    } else if (d::is_false(head)) {
      return p::false_();
    } else if (d::is_and(head)) {
      return p::and_(data2pbes(arg1(q)), data2pbes(arg2(q)));
    } else if (d::is_or(head)) {
      return p::and_(data2pbes(arg1(q)), data2pbes(arg2(q)));
    }
    throw std::runtime_error(std::string("data2pbes error: unknown data_expression ") + q.to_string());
    return pbes_expression();
  }
  throw std::runtime_error(std::string("data2pbes error: unknown data_expression ") + q.to_string());
  return pbes_expression();
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

} // namespace lps

#endif // MCRL2_PBES_UTILITY_H
