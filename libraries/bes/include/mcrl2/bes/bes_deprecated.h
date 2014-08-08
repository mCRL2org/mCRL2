// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bes_deprecated.h
/// \brief An ad hoc bes class for pbes2bool. It is intended
///        to be replaced by more general bes library components
///        This file is primary used by pbes2bool.

#ifndef BES_DEPRECATED_H
#define BES_DEPRECATED_H

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <deque>
#include <map>
#include <time.h>

#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/atermpp/aterm_balanced_tree.h"
#include "mcrl2/utilities/logger.h"

#include "mcrl2/core/print.h"

#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/data_specification.h"

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/io.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/fixpoint_symbol.h"

#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/one_point_rule_rewriter.h"

#define RELEVANCE_MASK 1
#define FIXPOINT_MASK 2
#define RANK_SHIFT 2

// TODO:
namespace bes
{

// a bes variable_type is an size_t.

typedef size_t variable_type; /* valid values start at 1 */

typedef enum reason { UNKNOWN,
                      MU_CYCLE,
                      NU_CYCLE,
                      SET_TO_FALSE,
                      SET_TO_TRUE,
                      FORWARD_SUBSTITUTION_FALSE,
                      FORWARD_SUBSTITUTION_TRUE,
                      SUBSTITUTION_FALSE,
                      SUBSTITUTION_TRUE,
                      APPROXIMATION_FALSE,
                      APPROXIMATION_TRUE,
                      APPROXIMATION
                    } reason;


/// \brief Strategies for the generation of a BES from a PBES
enum transformation_strategy
{
  lazy,          // generate equations but do not optimize on the fly
  optimize,      // optimize by substituting true and false for already
  // investigated variables in a rhs, while generating this rhs.
  on_the_fly,    // make a distinction between variables that occur somewhere,
  // and variables that do not occur somewhere. When generating
  // a rhs, optimize this rhs as in "optimize". If the rhs is
  // equal to T or F, substitute this value throughout the
  // equation system, and maintain which variables become unused
  // by doing so, as these do not have to be investigated further.
  // E.g. if a rhs is  X1 && X2, X1 does not occur elsewhere and
  // X2 turns out to be equal to false, then X1 is moved to the
  // set of irrelevant variables, and not investigated further.
  on_the_fly_with_fixed_points
  // Do the same as with on the fly, but for each generated variable
  // in the rhs, investigate whether this variable lies on a loop
  // such that depending on its fixed point, it can be set to true
  // or false. Due to the breadth first nature of the main algorithm
  // the existence of such loops must be investigated separately
  // for each variable, which can take a lot of time.
};

inline
transformation_strategy parse_transformation_strategy(const std::string& s)
{
  if (s == "0") return lazy;
  else if (s == "1") return optimize;
  else if (s == "2") return on_the_fly;
  else if (s == "3") return on_the_fly_with_fixed_points;
  else throw mcrl2::runtime_error("unknown transformation strategy " + s);
}

inline
std::string print_transformation_strategy(const transformation_strategy s)
{
  switch(s)
  {
    case lazy: return "0";
    case optimize: return "1";
    case on_the_fly: return "2";
    case on_the_fly_with_fixed_points: return "3";
  }
  throw mcrl2::runtime_error("unknown transformation strategy");
}

inline
std::istream& operator>>(std::istream& is, transformation_strategy& strategy)
{
  try
  {
    std::string s;
    is >> s;
    strategy = parse_transformation_strategy(s);
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

inline
std::ostream& operator<<(std::ostream& os, const transformation_strategy s)
{
  os << print_transformation_strategy(s);
  return os;
}

inline
std::string description(const transformation_strategy s)
{
  switch(s)
  {
    case lazy: return "Compute all boolean equations which can be reached"
        " from the initial state, without optimization."
        " This is is the most data efficient"
        " option per generated equation.";
    case optimize: return "Optimize by immediately substituting the right"
        " hand sides for already investigated variables"
        " that are true or false when generating an"
        " expression. This is as memory efficient as 0.";
    case on_the_fly: return "In addition to 1, also substitute variables that"
        " are true or false into an already generated right"
        " hand side. This can mean that certain variables"
        " become unreachable (e.g. X0 in X0 and X1, when X1"
        " becomes false, assuming X0 does not occur"
        " elsewhere. It will be maintained which variables"
        " have become unreachable as these do not have to be"
        " investigated. Depending on the PBES, this can"
        " reduce the size of the generated BES substantially"
        " but requires a larger memory footprint.";
    case on_the_fly_with_fixed_points: return "In addition to 2, investigate for generated"
        " variables whether they occur on a loop, such that"
        " they can be set to true or false, depending on the"
        " fixed point symbol. This can increase the time"
        " needed to generate an equation substantially.";
  }
  throw mcrl2::runtime_error("unknown transformation strategy");
}


/// \brief BES variable remove level when generating a BES from a PBES.
enum remove_level
{
  none,   // Do not remove bes variables.
  some,   // Remove bes variables that are not used, and of which
          // the rhs of its equation is not equal to true and false.
  all     // Remove all bes variables whenever they are not used in
          // any other equation.
};

inline
remove_level parse_remove_level(const std::string& s)
{
  if (s == "none") return none;
  else if (s == "some") return some;
  else if (s == "all") return all;
  else throw mcrl2::runtime_error("unknown bes variables remove level " + s);
}

inline
std::string print_remove_level(const remove_level s)
{
  switch(s)
  {
    case none: return "none";
    case some: return "some";
    case all: return "all";
  }
  throw mcrl2::runtime_error("unknown remove_level");
}

inline
std::istream& operator>>(std::istream& is, remove_level& level)
{
  try
  {
    std::string s;
    is >> s;
    level = parse_remove_level(s);
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

inline
std::ostream& operator<<(std::ostream& os, const remove_level s)
{
  os << print_remove_level(s);
  return os;
}

inline
std::string description(const remove_level s)
{
  switch(s)
  {
    case none: return "do not remove generated bes variables. This can lead to excessive"
        " usage of memory.";
    case some: return "remove generated bes variables that are not used, except if"
        " the right hand side of its equation is true or false. The rhss of variables"
        " must have to be recalculated, if encountered again, which is quite normal.";
    case all: return "remove every bes variable that is not used anymore in any equation."
        " This is quite memory efficient, but it can be very time consuming as the rhss of removed bes"
        " variables may have to be recalculated quite often.";
  }
  throw mcrl2::runtime_error("unknown remove level");
}

/// \brief Search strategy when generating a BES from a PBES.
enum search_strategy
{
  breadth_first, // Generate the rhs of the last generated BES variable last.
  depth_first,   // Generate the rhs of the last generated BES variable first.
  breadth_first_short,
  depth_first_short
};

inline
search_strategy parse_search_strategy(const std::string& s)
{
  if (s == "breadth-first") return breadth_first;
  else if (s == "b") return breadth_first_short;
  else if (s == "depth-first") return depth_first;
  else if (s == "d") return depth_first_short;
  else throw mcrl2::runtime_error("unknown search strategy " + s);
}

inline
std::string print_search_strategy(const search_strategy s)
{
  switch(s)
  {
    case breadth_first: return "breadth-first";
    case depth_first: return "depth-first";
    case breadth_first_short: return "b";
    case depth_first_short: return "d";
  }
  throw mcrl2::runtime_error("unknown search strategy");
}

inline
std::istream& operator>>(std::istream& is, search_strategy& strategy)
{
  try
  {
    std::string s;
    is >> s;
    strategy = parse_search_strategy(s);
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

inline
std::ostream& operator<<(std::ostream& os, const search_strategy s)
{
  os << print_search_strategy(s);
  return os;
}

inline
std::string description(const search_strategy s)
{
  switch(s)
  {
    case breadth_first: return "Compute the right hand side of the boolean variables"
        " in a first come first served basis. This is comparable with a breadth-first search."
        " This is good for generating counter examples. ";
    case depth_first: return "Compute the right hand side of a boolean variables where "
        " the last generated variable is investigated first. This corresponds to a depth-first "
        " search. This can substantially outperform breadth-first search when the validity of a"
        " formula is determined after a larger depths. ";
    case breadth_first_short: return "Short hand for breadth-first.";
    case depth_first_short: return "Short hand for depth-first.";
  }
  throw mcrl2::runtime_error("unknown search strategy");
}

// The class below contains a pbes_instantiation consisting of an identifier_string and the parameters,
// where the parameters are stored as a tree.

class propositional_variable_instantiation_as_tree
{
  protected:
    mcrl2::core::identifier_string m_name;
    atermpp::term_balanced_tree<mcrl2::data::data_expression> m_arguments_tree;

  public:

    /// \brief Default constructor
    propositional_variable_instantiation_as_tree()
    {}

    propositional_variable_instantiation_as_tree(const mcrl2::pbes_system::propositional_variable_instantiation& p)
     : m_name(p.name()), 
       m_arguments_tree(atermpp::term_balanced_tree<mcrl2::data::data_expression>(p.parameters().begin(),p.parameters().size()))
    {}


    propositional_variable_instantiation_as_tree(const mcrl2::core::identifier_string& t1,
                                                 const mcrl2::data::data_expression_list& t2)
     : m_name(t1), 
       m_arguments_tree(t2)
    {}

    const mcrl2::core::identifier_string& name() const
    {
      return m_name;
    }

    const atermpp::term_balanced_tree<mcrl2::data::data_expression>& arguments() const
    {
      return m_arguments_tree;
    }

    inline
    void assign_variables_in_tree(
         mcrl2::data::variable_list::iterator var_iter,
         mcrl2::data::rewriter::substitution_type& sigma) const
    {
      for(atermpp::term_balanced_tree<mcrl2::data::data_expression>::iterator i=m_arguments_tree.begin(); i!=m_arguments_tree.end(); ++i,++var_iter)
      {
        sigma[*var_iter]= *i;
      }
    }
  
    bool defined() const
    {
      return m_name!=mcrl2::core::identifier_string();
    }
   
    bool operator==(const propositional_variable_instantiation_as_tree& other) const
    {
      return m_name==other.m_name && m_arguments_tree==other.m_arguments_tree;
    }

};

/// \brief Streaming operator for a propositional_variable_instantiation_as_tree.
inline
std::ostream& operator<<(std::ostream& os, const propositional_variable_instantiation_as_tree s)
{
  os << s.name();
  if (!s.arguments().empty())
  { 
    os << "(" << s.arguments() << ")" << "\n";
  }
  return os;
}

class counter_example
{
  private:
    bes::variable_type v;
    reason r;

  public:
    counter_example()
      : v(0),
        r(UNKNOWN)
    {}

    counter_example(variable_type w, reason s)
      : v(w),
        r(s)
    {
    }

    variable_type get_variable(void)
    {
      return v;
    }

    void set_variable(variable_type w)
    {
      v=w;
    }

    reason get_reason(void)
    {
      return r;
    }

    void set_reason(reason s)
    {
      r=s;
    }

    std::string print_reason(void)
    {
      switch (r)
      {
        case UNKNOWN:
          return "Unknown     ";
        case MU_CYCLE:
          return "Mu Cycle    ";
        case NU_CYCLE:
          return "Nu Cycle    ";
        case SET_TO_FALSE:
          return "Set:false   ";
        case SET_TO_TRUE:
          return "Set:true    ";
        case FORWARD_SUBSTITUTION_FALSE:
          return "FSubst:false";
        case FORWARD_SUBSTITUTION_TRUE:
          return "FSubst:true ";
        case SUBSTITUTION_FALSE:
          return "Subst:false ";
        case SUBSTITUTION_TRUE:
          return "Subst:true  ";
        case APPROXIMATION_FALSE:
          return "Appr:false  ";
        case APPROXIMATION_TRUE:
          return "Appr:true   ";
        case APPROXIMATION:
          return "Approxim    ";
        default:
          return "ERROR UNKNOWN CASE";
      }
    }

    bool reason_indicates_that_formula_evaluated_to_given_truth_value(const bool truth_value)
    {
      switch (r)
      {
        case UNKNOWN:
          return true; // This is not known. True is the safer answer.
        case MU_CYCLE:
          return !truth_value;
        case NU_CYCLE:
          return truth_value;
        case SET_TO_FALSE:
          return !truth_value;
        case SET_TO_TRUE:
          return truth_value;
        case FORWARD_SUBSTITUTION_FALSE:
          return !truth_value;
        case FORWARD_SUBSTITUTION_TRUE:
          return truth_value;
        case SUBSTITUTION_FALSE:
          return !truth_value;
        case SUBSTITUTION_TRUE:
          return truth_value;
        case APPROXIMATION_FALSE:
          return !truth_value;
        case APPROXIMATION_TRUE:
          return truth_value;
        case APPROXIMATION:
          return true; // This is not known. True is the safer answer.
        default:
          return "ERROR UNKNOWN CASE";
      }
    }

};

// wrap global variables in a class, to enable multiple inclusion of this header file
// note, T is only a dummy

template <class T>
struct bes_global_variables
{
  static std::deque<counter_example> COUNTER_EXAMPLE_NULL_QUEUE;
  static std::deque<variable_type> TODO_NULL_QUEUE;
  static bool opt_use_hashtables;
};

template <class T>
bool bes_global_variables<T>::opt_use_hashtables = false;

template <class T>
std::deque<counter_example> bes_global_variables<T>::COUNTER_EXAMPLE_NULL_QUEUE;

template <class T>
std::deque<variable_type> bes_global_variables<T>::TODO_NULL_QUEUE;


///////////////////////////////////////////////////////////////////////////////
// bes_expression
/// \brief bes expression.
///
// <BESExpr>       ::=
//                  | BESTrue
//                  | BESFalse
//                  | And(<BESExpr>, <BESExpr>)
//                  | Or(<BESExpr>, <BESExpr>)
//                  | If(<BESExpr>,<BESExpr>,<BESExpr>)
//                  | <variable>
//
// Moreover, there is a dummy bes expression, that is used
// as default expression.


// below we define functions to construct bes_expressions using aterms.
//

class bes_expression: public atermpp::aterm
{
  public:
    bes_expression()
      : atermpp::aterm()
    {}

    bes_expression(const atermpp::aterm& term)
      : atermpp::aterm(term)
    {}

    // allow assignment from aterms
    bes_expression& operator=(const bes_expression& t)
    {
      copy_term(t);
      return *this;
    }
};

inline
void use_hashtables(void)
{
  bes_global_variables<size_t>::opt_use_hashtables=true;
}

inline const atermpp::function_symbol& AFunBESAnd()
{
  static atermpp::function_symbol BESAnd("BESAnd", 2);
  return BESAnd;
}

inline const atermpp::function_symbol& AFunBESOr()
{
  static atermpp::function_symbol BESOr("BESOr", 2);
  return BESOr;
}

inline const atermpp::function_symbol& AFunBESIf()
{
  static atermpp::function_symbol BESIf("BESIf", 3);
  return BESIf;
}

// BESFalse
inline
const atermpp::function_symbol& gsAFunBESFalse()
{
  static const atermpp::function_symbol AFunBESFalse("BESFalse", 0);
  return AFunBESFalse;
}

inline
bool gsIsBESFalse(atermpp::aterm_appl Term)
{
  return Term.function() == gsAFunBESFalse();
}

// BESTrue
inline
const atermpp::function_symbol& gsAFunBESTrue()
{
  static const atermpp::function_symbol AFunBESTrue("BESTrue", 0);
  return AFunBESTrue;
}

inline
bool gsIsBESTrue(atermpp::aterm_appl Term)
{
  return Term.function() == gsAFunBESTrue();
}

// BESDummy
inline
const atermpp::function_symbol& gsAFunBESDummy()
{
  static const atermpp::function_symbol AFunBESDummy("BESDummy", 0);
  return AFunBESDummy;
}

inline
bool gsIsBESDummy(atermpp::aterm_appl Term)
{
  return Term.function() == gsAFunBESDummy();
}

inline
const atermpp::aterm_appl& gsMakeBESFalse()
{
  static const atermpp::aterm_appl t(gsAFunBESFalse());
  return t;
}

inline
const atermpp::aterm_appl& gsMakeBESTrue()
{
  static const atermpp::aterm_appl t(gsAFunBESTrue());
  return t;
}

inline
const atermpp::aterm_appl& gsMakeBESDummy()
{
  static const atermpp::aterm_appl t(gsAFunBESDummy());
  return t;
}

/// \brief Returns the expression true
inline
const bes_expression& true_()
{
  return atermpp::down_cast<const bes_expression>(gsMakeBESTrue());
}

/// \brief Returns the expression false
inline
const bes_expression& false_()
{
  return atermpp::down_cast<const bes_expression>(gsMakeBESFalse());
}

/// \brief Returns the expression dummy (???)
inline
const bes_expression& dummy()
{
  return atermpp::down_cast<const bes_expression>(gsMakeBESDummy());
}

inline bes_expression and_(const bes_expression& b1, const bes_expression& b2)
{
  return atermpp::down_cast<bes_expression>(atermpp::aterm_appl(AFunBESAnd(), b1, b2));
}

inline bes_expression and_optimized(const bes_expression& b1, const bes_expression& b2)
{
  if (b1==true_())
  {
    return b2;
  }
  if (b2==true_())
  {
    return b1;
  }
  if (b1==false_())
  {
    return false_();
  }
  if (b2==false_())
  {
    return false_();
  }
  if (b1==b2)
  {
    return b1;
  }
  return and_(b1,b2);
}

inline bes_expression or_(const bes_expression& b1, const bes_expression& b2)
{
  return atermpp::down_cast<const bes_expression>(atermpp::aterm_appl(AFunBESOr(), b1, b2));
}

inline bes_expression or_optimized(const bes_expression& b1, const bes_expression& b2)
{
  if (b1==true_())
  {
    return true_();
  }
  if (b2==true_())
  {
    return true_();
  }
  if (b1==false_())
  {
    return b2;
  }
  if (b2==false_())
  {
    return b1;
  }
  if (b1==b2)
  {
    return b1;
  }
  return or_(b1,b2);
}


inline bool is_variable(const bes_expression& b)
{
  return b.type_is_int();
}

inline bes_expression if_(const bes_expression& b1, const bes_expression& b2, const bes_expression& b3)
{
  return atermpp::down_cast<bes_expression>(atermpp::aterm_appl(AFunBESIf(), b1, b2,b3));
}

inline bes_expression ifAUX_(const bes_expression& b1, const bes_expression& b2, const bes_expression& b3)
{
  if (b2==b3)
  {
    return b2;
  }
  return if_(b1,b2,b3);
}

inline bes_expression variable(const variable_type& n)
{
  return atermpp::down_cast<const bes_expression>(atermpp::aterm_int(n));
}

inline bool is_false(const bes_expression& b)
{
  return atermpp::down_cast<const atermpp::aterm_appl>(b).function()==gsAFunBESFalse();
}

inline bool is_true(const bes_expression& b)
{
  return atermpp::down_cast<const atermpp::aterm_appl>(b).function()==gsAFunBESTrue();
}

inline bool is_dummy(const bes_expression& b)
{
  return atermpp::down_cast<const atermpp::aterm_appl>(b).function()==gsAFunBESDummy();
}

inline bool is_and(const bes_expression& b)
{
  using namespace atermpp;
  return atermpp::down_cast<const aterm_appl>(b).function()==AFunBESAnd();
}

inline bool is_or(const bes_expression& b)
{
  using namespace atermpp;
  return atermpp::down_cast<const aterm_appl>(b).function()==AFunBESOr();
}

inline bool is_if(const bes_expression& b)
{
  using namespace atermpp;
  return atermpp::down_cast<const aterm_appl>(b).function()==AFunBESIf();
}

inline const bes_expression& lhs(const bes_expression& b)
{
  assert(is_and(b) || is_or(b));
  return atermpp::down_cast<const bes_expression>(atermpp::down_cast<const atermpp::aterm_appl>(b)[0]);
}

inline const bes_expression& rhs(const bes_expression& b)
{
  assert(is_and(b) || is_or(b));
  return atermpp::down_cast<const bes_expression>(atermpp::down_cast<const atermpp::aterm_appl>(b)[1]);
}

inline const bes_expression& condition(const bes_expression& b)
{
  assert(is_if(b));
  return atermpp::down_cast<const bes_expression>(atermpp::down_cast<const atermpp::aterm_appl>(b)[0]);
}

inline const bes_expression& then_branch(const bes_expression& b)
{
  assert(is_if(b));
  return atermpp::down_cast<const bes_expression>(atermpp::down_cast<const atermpp::aterm_appl>(b)[1]);
}

inline const bes_expression& else_branch(const bes_expression& b)
{
  assert(is_if(b));
  return atermpp::down_cast<const bes_expression>(atermpp::down_cast<const atermpp::aterm_appl>(b)[2]);
}

inline variable_type get_variable(const bes_expression& b)
{
  assert(is_variable(b));
  return ((atermpp::aterm_int)(const atermpp::aterm&)b).value();
}

inline bes_expression substitute_true_false_rec(
  const bes_expression& b,
  const variable_type v,
  const bes_expression b_subst,
  std::map < bes_expression, bes_expression > hashtable,
  std::deque < counter_example >& counter_example_queue=bes_global_variables<size_t>::COUNTER_EXAMPLE_NULL_QUEUE)
{
  assert(is_true(b_subst)||is_false(b_subst));

  if (is_true(b)||is_false(b)||is_dummy(b))
  {
    return b;
  }

  bes_expression result;

  if (bes_global_variables<size_t>::opt_use_hashtables)
  {
    result=hashtable[b];
    if (result==bes_expression())
    {
      return result;
    }
  }

  if (is_if(b))
  {
    if (v==get_variable(condition(b)))
    {
      if (is_true(b_subst))
      {
        result=then_branch(b);
        if (&counter_example_queue!=&bes_global_variables<size_t>::COUNTER_EXAMPLE_NULL_QUEUE)
        {
          counter_example_queue.push_front(counter_example(v,SUBSTITUTION_TRUE));
        }
      }
      else
      {
        assert(is_false(b_subst));
        if (&counter_example_queue!=&bes_global_variables<size_t>::COUNTER_EXAMPLE_NULL_QUEUE)
        {
          counter_example_queue.push_front(counter_example(v,SUBSTITUTION_FALSE));
        }
        result=else_branch(b);
      }
    }
    else
    {
      result=ifAUX_(condition(b),
                    substitute_true_false_rec(then_branch(b),v,b_subst,hashtable,counter_example_queue),
                    substitute_true_false_rec(else_branch(b),v,b_subst,hashtable,counter_example_queue));
    }
  }
  else if (is_variable(b))
  {
    if (v==get_variable(b))
    {
      result=b_subst;
      if (is_true(b_subst))
      {
        if (&counter_example_queue!=&bes_global_variables<size_t>::COUNTER_EXAMPLE_NULL_QUEUE)
        {
          counter_example_queue.push_front(counter_example(v,SUBSTITUTION_TRUE));
        }
      }
      else
      {
        assert(is_false(b_subst));
        if (&counter_example_queue!=&bes_global_variables<size_t>::COUNTER_EXAMPLE_NULL_QUEUE)
        {
          counter_example_queue.push_front(counter_example(v,SUBSTITUTION_FALSE));
        }
      }
    }
    else
    {
      result=b;
    }
  }
  else if (is_and(b))
  {
    bes_expression b1=substitute_true_false_rec(lhs(b),v,b_subst,hashtable,counter_example_queue);
    if (is_false(b1))
    {
      result=false_();
    }
    else
    {
      bes_expression b2=substitute_true_false_rec(rhs(b),v,b_subst,hashtable,counter_example_queue);
      if (is_false(b2))
      {
        result=false_();
      }
      else if (is_true(b1))
      {
        result=b2;
      }
      else if (is_true(b2))
      {
        result=b1;
      }
      else if (b1==b2)
      {
        result=b1;
      }
      else
      {
        result=and_(b1,b2);
      }
    }
  }
  else if (is_or(b))
  {
    bes_expression b1=substitute_true_false_rec(lhs(b),v,b_subst,hashtable,counter_example_queue);
    if (is_true(b1))
    {
      result=true_();
    }
    else
    {
      bes_expression b2=substitute_true_false_rec(rhs(b),v,b_subst,hashtable,counter_example_queue);
      if (is_true(b2))
      {
        result=true_();
      }
      else if (is_false(b1))
      {
        result=b2;
      }
      else if (is_false(b2))
      {
        result=b1;
      }
      else if (b1==b2)
      {
        result=b1;
      }
      else
      {
        result=or_(b1,b2);
      }
    }
  }

  if (bes_global_variables<size_t>::opt_use_hashtables)
  {
    hashtable[b]=result;
  }
  return result;
}

inline
bes_expression substitute_true_false(
  const bes_expression& b,
  const variable_type v,
  const bes_expression b_subst,
  std::deque < counter_example >& counter_example_queue=bes_global_variables<size_t>::COUNTER_EXAMPLE_NULL_QUEUE)
{
  assert(is_true(b_subst)||is_false(b_subst));

  if (is_true(b)||is_false(b)||is_dummy(b))
  {
    return b;
  }

  std::map < bes_expression, bes_expression > hashtable1;

  bes_expression result=substitute_true_false_rec(b,v,b_subst,hashtable1,counter_example_queue);

  if (bes_global_variables<size_t>::opt_use_hashtables)
  {
    hashtable1.clear();
  }
  return result;
}

inline
bes_expression BDDif_rec(
  const bes_expression& b1,
  const bes_expression& b2,
  const bes_expression& b3,
  std::map < bes_expression, bes_expression >& hashtable);


inline bes_expression BDDif(const bes_expression& b1, const bes_expression& b2, const bes_expression& b3)
{

  std::map < bes_expression, bes_expression > hashtable;
  static size_t hashtable_reset_counter=0;

  bes_expression b=BDDif_rec(b1,b2,b3,hashtable);
  hashtable_reset_counter++;
  if (hashtable_reset_counter==1000) // resetting of the hashtable is quite expensive.
  {
    hashtable.clear();
    hashtable_reset_counter=0;
  }
  return b;
}

inline bes_expression BDDif_rec(const bes_expression& b1, const bes_expression& b2, const bes_expression& b3, std::map< bes_expression, bes_expression >& hashtable)
{
  /* Assume that b1, b2 and b3 are ordered BDDs. Return an
     ordered BDD */

  /* Check hash table */

  bes_expression b1b2b3;
  bes_expression result;

  if (is_true(b1))
  {
    return b2;
  }
  else if (is_false(b1))
  {
    return b3;
  }
  /* b1 has an if_then_else shape */
  else if (b2==b3)
  {
    return b2;
  }

  if (is_true(b2))
  {
    /* hence not is_true(b3) */
    if (is_false(b3))
    {
      return b1;
    }
    else
    {
      b1b2b3=if_(b1,b2,b3);
      bes_expression b(hashtable[b1b2b3]);
      if (b!=bes_expression())
      {
        return b;
      }

      /* b3 contains an if then else expression, and b2=true */
      if (condition(b1)==condition(b3))
      {
        result=ifAUX_(condition(b1),
                      BDDif_rec(then_branch(b1),b2,then_branch(b3),hashtable),
                      BDDif_rec(else_branch(b1),b2,else_branch(b3),hashtable));
      }
      else if (get_variable(condition(b1))<get_variable(condition(b3)))
      {
        result=ifAUX_(condition(b1),
                      BDDif_rec(then_branch(b1),b2,b3,hashtable),
                      BDDif_rec(else_branch(b1),b2,b3,hashtable));
      }
      else
      {
        result=ifAUX_(condition(b3),
                      BDDif_rec(b1,b2,then_branch(b3),hashtable),
                      BDDif_rec(b1,b2,else_branch(b3),hashtable));
      }
    }
  }
  else if (is_false(b2))
  {
    /* hence not is_false(b3) */
    b1b2b3=if_(b1,b2,b3);
    bes_expression b(hashtable[b1b2b3]);
    if (b!=bes_expression())
    {
      return b;
    }

    if (is_true(b3))
    {
      result=ifAUX_(condition(b1),
                    BDDif_rec(then_branch(b1),b2,b3,hashtable),
                    BDDif_rec(else_branch(b1),b2,b3,hashtable));
    }
    else
    {
      /* b3 contains an if then else expression, and b2=true */
      if (condition(b1)==condition(b3))
      {
        result=ifAUX_(condition(b1),
                      BDDif_rec(then_branch(b1),b2,then_branch(b3),hashtable),
                      BDDif_rec(else_branch(b1),b2,else_branch(b3),hashtable));
      }
      else if (get_variable(condition(b1))<get_variable(condition(b3)))
      {
        result=ifAUX_(condition(b1),
                      BDDif_rec(then_branch(b1),b2,b3,hashtable),
                      BDDif_rec(else_branch(b1),b2,b3,hashtable));
      }
      else
      {
        result=ifAUX_(condition(b3),
                      BDDif_rec(b1,b2,then_branch(b3),hashtable),
                      BDDif_rec(b1,b2,else_branch(b3),hashtable));
      }
    }
  }
  else if (is_true(b3)||is_false(b3))
  {
    if (condition(b1)==condition(b2))
    {
      result=ifAUX_(condition(b1),
                    BDDif_rec(then_branch(b1),then_branch(b2),b3,hashtable),
                    BDDif_rec(else_branch(b1),else_branch(b2),b3,hashtable));
    }
    else if (get_variable(condition(b1))<get_variable(condition(b2)))
    {
      result=ifAUX_(condition(b1),
                    BDDif_rec(then_branch(b1),b2,b3,hashtable),
                    BDDif_rec(else_branch(b1),b2,b3,hashtable));
    }
    else
    {
      result=ifAUX_(condition(b2),
                    BDDif_rec(b1,then_branch(b2),b3,hashtable),
                    BDDif_rec(b1,else_branch(b2),b3,hashtable));
    }
  }
  else
  {
    /* None of b1, b2 and b3 is true or false, all have the if then else shape */
    if (condition(b1)==condition(b2))
    {
      if (condition(b2)==condition(b3))
      {
        /* all conditions are equal */
        result=ifAUX_(condition(b1),
                      BDDif_rec(then_branch(b1),then_branch(b2),then_branch(b3),hashtable),
                      BDDif_rec(else_branch(b1),else_branch(b2),else_branch(b3),hashtable));
      }
      else if (get_variable(condition(b1))<get_variable(condition(b3)))
      {
        result=ifAUX_(condition(b1),
                      BDDif_rec(then_branch(b1),then_branch(b2),b3,hashtable),
                      BDDif_rec(else_branch(b1),else_branch(b2),b3,hashtable));
      }
      else
      {
        result=ifAUX_(condition(b3),
                      BDDif_rec(b1,b2,then_branch(b3),hashtable),
                      BDDif_rec(b1,b2,else_branch(b3),hashtable));
      }
    }
    else if (get_variable(condition(b1))<get_variable(condition(b2)))
    {
      if (condition(b1)==condition(b3))
      {
        result=ifAUX_(condition(b1),
                      BDDif_rec(then_branch(b1),b2,then_branch(b3),hashtable),
                      BDDif_rec(else_branch(b1),b2,else_branch(b3),hashtable));

      }
      else if (get_variable(condition(b1))<get_variable(condition(b3)))
      {
        result=ifAUX_(condition(b1),
                      BDDif_rec(then_branch(b1),b2,b3,hashtable),
                      BDDif_rec(else_branch(b1),b2,b3,hashtable));
      }
      else if (condition(b2)==condition(b3))
      {
        result=ifAUX_(condition(b2),
                      BDDif_rec(b1,then_branch(b2),then_branch(b3),hashtable),
                      BDDif_rec(b1,else_branch(b2),else_branch(b3),hashtable));
      }
      else if (get_variable(condition(b2))<get_variable(condition(b3)))
      {
        result=ifAUX_(condition(b2),
                      BDDif_rec(b1,then_branch(b2),b3,hashtable),
                      BDDif_rec(b1,else_branch(b2),b3,hashtable));
      }
      else
      {
        result=ifAUX_(condition(b3),
                      BDDif_rec(b1,b2,then_branch(b3),hashtable),
                      BDDif_rec(b1,b2,else_branch(b3),hashtable));
      }
    }
  }

  /* Add (if_(b1,b2,b3),result) to hashtable. */

  hashtable[b1b2b3]=result;
  return result;
}

static bes_expression toBDD_rec(const bes_expression& b1,std::map< bes_expression, bes_expression>& hashtable)
{
  if ((b1==true_()) || (b1==false_()))
  {
    return b1;
  }

  bes_expression result;
  bes_expression b(hashtable[b1]);
  if (b!=bes_expression())
  {
    return b;
  }

  if (is_variable(b1))
  {
    result=if_(b1,true_(),false_());
  }
  else if (is_and(b1))
  {
    result=BDDif_rec(toBDD_rec(lhs(b1),hashtable),toBDD_rec(rhs(b1),hashtable),false_(),hashtable);
  }
  else if (is_or(b1))
  {
    result=BDDif_rec(toBDD_rec(lhs(b1),hashtable),true_(),toBDD_rec(rhs(b1),hashtable),hashtable);
  }
  else if (is_if(b1))
  {
    result=BDDif_rec(toBDD_rec(condition(b1),hashtable),
                     toBDD_rec(then_branch(b1),hashtable),
                     toBDD_rec(else_branch(b1),hashtable),
                     hashtable);
  }
  else
  {
    mCRL2log(mcrl2::log::error) << "Unexpected expression" << std::endl;
    assert(0);
  }

  hashtable[b1]=result;
  return result;
}


inline bes_expression toBDD(const bes_expression& b)
{
  std::map <bes_expression, bes_expression > hashtable;
  return toBDD_rec(b,hashtable);
}

// The function below orders the variables in quantified expressions, such that they are enumerated faster.
// Once this is a feature of the enumerator this code could be removed.

inline mcrl2::pbes_system::pbes_expression pbes_expression_order_quantified_variables(
              const mcrl2::pbes_system::pbes_expression& p, const mcrl2::data::data_specification& data_spec)
{
  using namespace mcrl2;
  using namespace mcrl2::pbes_system;
  using namespace mcrl2::pbes_system::pbes_expr;
  using namespace mcrl2::pbes_system::accessors;

  if (is_pbes_and(p))
  {
    return pbes_expr::and_(pbes_expression_order_quantified_variables(left(p),data_spec),pbes_expression_order_quantified_variables(right(p),data_spec));
  }
  else if (is_pbes_or(p))
  {
    return pbes_expr::or_(pbes_expression_order_quantified_variables(left(p),data_spec),pbes_expression_order_quantified_variables(right(p),data_spec));
  }
  else if (is_pbes_imp(p))
  {
    return pbes_expr::imp(pbes_expression_order_quantified_variables(left(p),data_spec),pbes_expression_order_quantified_variables(right(p),data_spec));
  }
  else if (is_pbes_not(p))
  {
    return pbes_expr::not_(pbes_expression_order_quantified_variables(arg(p),data_spec));
  }
  else if (is_pbes_forall(p))
  {
    const pbes_expression expr = pbes_expression_order_quantified_variables(arg(p),data_spec);
    return pbes_expr::forall(mcrl2::data::order_variables_to_optimise_enumeration(var(p),data_spec),expr);
  }
  else if (is_pbes_exists(p))
  {
    const pbes_expression expr = pbes_expression_order_quantified_variables(arg(p),data_spec);
    return pbes_expr::exists(mcrl2::data::order_variables_to_optimise_enumeration(var(p),data_spec),expr);
  }
  else 
  {
    return p;
  }
}

class boolean_equation_system
{
  private:
    // initial size is the initial size of
    // the vector holding equations;
    // the first position of the vectors (i.e. position 0)
    // is not used.
    //
    // The first bit it is indicated whether this
    // equation is relevant. It is 0 if not relevant.
    // It is 1 if relevant.
    // In the second bit it is indicated whether
    // the equation is nu (0) or a mu (1), the rest of the
    // bits indicate the rank+1 of this equation.
    // The value 0 for control_info indicates a wrong value.

    std::vector<size_t> control_info;
    std::vector<bes_expression> right_hand_sides;
    bool variable_occurrences_are_stored;
    std::vector< std::set <variable_type> > variable_occurrence_sets;
    bool count_variable_relevance;
    std::vector < std::deque < counter_example> > data_to_construct_counter_example;
    bool construct_counter_example;
    remove_level remove_unused_bes_variables;
    atermpp::indexed_set< propositional_variable_instantiation_as_tree > variable_index;  //Used for constructing counter examples
    typedef mcrl2::data::rewriter::substitution_type substitution_type;

  protected:
    inline void check_vector_sizes(const variable_type v)
    {
      if (v>nr_of_variables())
      {
        control_info.resize(v+1,0);
        right_hand_sides.resize(v+1,dummy());
        if (variable_occurrences_are_stored)
        {
          variable_occurrence_sets.resize(v+1,std::set<variable_type>());
        }
        if (construct_counter_example)
        {
          data_to_construct_counter_example.resize(v+1,std::deque<counter_example>());
        }
      }
    }

  public:
    size_t max_rank;

    inline size_t nr_of_variables() const
    {
      return control_info.size()-1; /* there is no equation at position 0 */
    }

    inline mcrl2::pbes_system::fixpoint_symbol get_fixpoint_symbol(variable_type v)
    {
      assert(v>0); /* variable indices start at 1. 0 is used as an indicator of error */
      assert(v<=nr_of_variables());
      assert(control_info[v]>0);

      return (((control_info[v] & FIXPOINT_MASK) ==0) ? mcrl2::pbes_system::fixpoint_symbol::nu() : mcrl2::pbes_system::fixpoint_symbol::mu());
    }


    inline size_t get_rank(variable_type v)
    {
      assert(v>0);
      check_vector_sizes(v);

      return (control_info[v] >> RANK_SHIFT)-1;
    }

    void add_equation(variable_type v,
                      mcrl2::pbes_system::fixpoint_symbol sigma,
                      size_t rank,
                      const bes_expression& rhs,
                      const size_t maximal_todo_size,
                      size_t& queue_put_count_extra,
                      const bool approximate_true,
                      std::deque <variable_type>& todo=bes_global_variables<size_t>::TODO_NULL_QUEUE)
    {
      assert(rank>0);  // rank must be positive.
      assert(v>0);     // variables are represented by numbers >0.

      check_vector_sizes(v);
      // the vector at position v is now guaranteed to exist.

      // if the control info is 0, the value at variable
      // is not initialized.

      control_info[v]=1+(((sigma.is_nu()?0:1)+((rank+1)<<1))<<1);
      right_hand_sides[v]=rhs;
      if (rank>max_rank)
      {
        max_rank=rank;
      }
      if (variable_occurrences_are_stored)
      {
        add_variables_to_occurrence_sets(v,rhs);
      }
      if (count_variable_relevance)
      {
        set_variable_relevance_rec(rhs,maximal_todo_size,queue_put_count_extra,approximate_true,todo);
      }
    }

    inline void set_rhs(variable_type v,
                        const bes_expression& b,
                        const size_t maximal_todo_size,
                        size_t& queue_put_count_extra,
                        const bool approximate_true,
                        variable_type v_except=0,
                        std::deque <variable_type>& todo=bes_global_variables<size_t>::TODO_NULL_QUEUE)
    {
      /* set the right hand side of v to b. Update the variable occurrences
         of v in the variables occurrence sets of variables occurring in b, but
         do not update the variable occurrence sets of v_except */
      assert(v>0);
      assert(v<=nr_of_variables());

      control_info[v]=control_info[v]|RELEVANCE_MASK;  // make this variable relevant.
      bes_expression old_rhs=right_hand_sides[v];
      if ((variable_occurrences_are_stored) && (old_rhs!=b))
      {
        remove_variables_from_occurrence_sets(v,old_rhs,v_except);
        add_variables_to_occurrence_sets(v,b);
        right_hand_sides[v]=b;
      }
      if (count_variable_relevance)
      {
        set_variable_relevance_rec(b,maximal_todo_size,queue_put_count_extra,approximate_true,todo);
      }

    }

    inline bes_expression get_rhs(variable_type v)
    {
      if (v>nr_of_variables())
      {
        return dummy();
      }
      else
      {
        return right_hand_sides[v];
      }
    }

    void clear_variable_occurrence_set(variable_type v)
    {
      assert(variable_occurrences_are_stored);
      check_vector_sizes(v);
      variable_occurrence_sets[v].clear();
    }

    std::set< variable_type >::iterator
    variable_occurrence_set_begin(variable_type v)
    {
      assert(variable_occurrences_are_stored);
      assert(v>0);
      check_vector_sizes(v);
      return variable_occurrence_sets[v].begin();
    }

    std::set< variable_type >::iterator
    variable_occurrence_set_end(variable_type v)
    {
      assert(variable_occurrences_are_stored);
      assert(v>0);
      check_vector_sizes(v);
      return variable_occurrence_sets[v].end();
    }

    void add_variables_to_occurrence_sets(
      variable_type v,
      const bes_expression& b)
    {
      assert(v>0);
      assert(variable_occurrences_are_stored);

      if (is_true(b)||is_false(b)||is_dummy(b))
      {
        return;
      }

      if (is_if(b))
      {
        assert(get_variable(condition(b))>0);
        variable_type w=get_variable(condition(b));
        check_vector_sizes(w);
        variable_occurrence_sets[w].insert(v);

        add_variables_to_occurrence_sets(v,then_branch(b));
        add_variables_to_occurrence_sets(v,else_branch(b));
        return;
      }

      if (is_variable(b))
      {
        variable_type w=get_variable(b);
        check_vector_sizes(w);
        variable_occurrence_sets[w].insert(v);
        return;
      }

      if (is_and(b)||is_or(b))
      {
        add_variables_to_occurrence_sets(v,lhs(b));
        add_variables_to_occurrence_sets(v,rhs(b));
        return;
      }
      assert(0); // do not expect other term formats.

    }

    void remove_variables_from_occurrence_sets(
      const variable_type v,
      const bes_expression& b,
      const variable_type v_except)
    {
      assert(v>0);
      assert(variable_occurrences_are_stored);
      check_vector_sizes(v);
      if (is_true(b)||is_false(b)||is_dummy(b))
      {
        return;
      }

      if (is_if(b))
      {
        variable_type w=get_variable(condition(b));
        assert(w<=nr_of_variables());
        if (w!=v_except)
        {
          variable_occurrence_sets[w].erase(v);
        }
        // Using hash tables this can be made more efficient, by employing
        // sharing of the atermpp::aterm representing b.
        remove_variables_from_occurrence_sets(v,then_branch(b),v_except);
        remove_variables_from_occurrence_sets(v,else_branch(b),v_except);
        return;
      }
      else if (is_variable(b))
      {
        variable_type w=get_variable(b);
        assert(w<=nr_of_variables());
        if (w!=v_except)
        {
          variable_occurrence_sets[w].erase(v);
        }
        return;
      }
      else if (is_or(b)||is_and(b))
      {
        remove_variables_from_occurrence_sets(v,lhs(b),v_except);
        remove_variables_from_occurrence_sets(v,rhs(b),v_except);
        return;
      }
    }

    void store_variable_occurrences(void)
    {
      if (variable_occurrences_are_stored)
      {
        return;
      }
      assert(variable_occurrence_sets.size()==1);
      variable_occurrences_are_stored=true;
      variable_occurrence_sets.resize(nr_of_variables()+1,std::set<variable_type>());
      for (variable_type v=nr_of_variables(); v>0; v--)
      {
        bes_expression b=get_rhs(v);
        if (b!=dummy())
        {
          add_variables_to_occurrence_sets(v,get_rhs(v));
        }
      }
    }


    void reset_variable_relevance(void)
    {
      for (std::vector <size_t>::iterator v=control_info.begin() ;
           v!=control_info.end() ;
           v++)
      {
        *v =(*v)& ~RELEVANCE_MASK;  // reset the relevance bit to 0
      }
    }

    // Insert the element v in the queue. If the queue exceeds its maximal size
    // remove a random element. Return true if the element is successfully inserted
    // in the queue.
    void insert_element_in_queue(std::deque<size_t>& todo, 
                                 size_t v, 
                                 const size_t maximal_todo_size,
                                 size_t& queue_put_count_extra,
                                 const bool approximate_true)
    {
      if (todo.size()<maximal_todo_size)
      {
        todo.push_back(v);
      }
      else
      {
        /* Insert this state such that each state has equal probability to remain
           in the queue. See libraries/lts/include/mcrl2/lts/detail/queue.h or the article
           on high_way search for an explanation */

        ++queue_put_count_extra;
        if ((rand() % (todo.size() + queue_put_count_extra)) < todo.size())
        {
          // Replace a random existing element in the queue by this one.
          size_t pos = rand() % todo.size();
          size_t old_bes_variable = todo[pos];
          todo[pos] = v;
          add_equation(old_bes_variable,
                       mcrl2::pbes_system::fixpoint_symbol::mu(),
                       1,
                       approximate_true?bes::false_():bes::true_(),
                       maximal_todo_size,queue_put_count_extra,approximate_true,
                       todo);//Counterexample TODO??.
        }
        // Do not add the element v to the todo queue, and set its rhs to an approximate value.
        add_equation(v,
                     mcrl2::pbes_system::fixpoint_symbol::mu(),
                     1,
                     approximate_true?bes::false_():bes::true_(),
                     maximal_todo_size,queue_put_count_extra,approximate_true,
                     todo);//Counterexample TODO??.
      }
    }



    void set_variable_relevance_rec(
                        const bes_expression& b,
                        const size_t maximal_todo_size,
                        size_t& queue_put_count_extra,
                        const bool approximate_true,
                        std::deque <variable_type>& todo=bes_global_variables<size_t>::TODO_NULL_QUEUE)
    {
      assert(count_variable_relevance);
      if (is_true(b)||is_false(b)||is_dummy(b))
      {
        return;
      }

      if (is_variable(b))
      {
        variable_type v=get_variable(b);
        assert(v>0);
        check_vector_sizes(v);
        if (!is_relevant(v))
        {
          control_info[v]=control_info[v]|RELEVANCE_MASK;  // Make relevant
          if (get_rhs(v)==dummy()) // v is relevant and unprocessed. Put in on the todo stack.
          {
            if (&todo!=&bes_global_variables<size_t>::TODO_NULL_QUEUE)
            {
              insert_element_in_queue(todo,v,maximal_todo_size,queue_put_count_extra,approximate_true);
            }
            return;
          }
          else
          {
            set_variable_relevance_rec(get_rhs(v),maximal_todo_size,queue_put_count_extra,approximate_true,todo);
            return;
          }
        }
        return;
      }

      if (is_if(b))
      {
        set_variable_relevance_rec(condition(b),maximal_todo_size,queue_put_count_extra,approximate_true,todo);
        set_variable_relevance_rec(then_branch(b),maximal_todo_size,queue_put_count_extra,approximate_true,todo);
        set_variable_relevance_rec(else_branch(b),maximal_todo_size,queue_put_count_extra,approximate_true,todo);
        return;
      }

      assert(is_and(b)||is_or(b));
      set_variable_relevance_rec(lhs(b),maximal_todo_size,queue_put_count_extra,approximate_true,todo);
      set_variable_relevance_rec(rhs(b),maximal_todo_size,queue_put_count_extra,approximate_true,todo);
      return;
    }

    void refresh_relevances(const size_t maximal_todo_size,
                            size_t& queue_put_count_extra,
                            const bool approximate_true,
                            std::deque <variable_type>& todo=bes_global_variables<size_t>::TODO_NULL_QUEUE)
    {
      if (count_variable_relevance)
      {
        reset_variable_relevance();
        if (&todo!=&bes_global_variables<size_t>::TODO_NULL_QUEUE)
        {
          todo.clear();
        }
        set_variable_relevance_rec(variable(1),maximal_todo_size,queue_put_count_extra,approximate_true,bes_global_variables<size_t>::TODO_NULL_QUEUE);
        if (&todo!=&bes_global_variables<size_t>::TODO_NULL_QUEUE)
        {
          // We add the variables to the todo queue separately,
          // to guarantee that lower numbered variables occur earlier in
          // the queue. This guarantees shorter counter examples.
          for (size_t v=1; v<=nr_of_variables(); v++)
          {
            if ((get_rhs(v)==dummy()) && is_relevant(v))
            {
              insert_element_in_queue(todo,v,maximal_todo_size,queue_put_count_extra,approximate_true);
            }
            else if (remove_unused_bes_variables!=none && 
                     !construct_counter_example && 
                     variable_index.defined(v) &&
                     !is_relevant(v) &&
                     ( remove_unused_bes_variables==all ||
                       ( !is_true(get_rhs(v)) && 
                         !is_false(get_rhs(v))
                     ) )
                    )
            {
              set_rhs(v,dummy(),maximal_todo_size,queue_put_count_extra,approximate_true); // Clear the rhs, to save space. This comes at the
                                  // cost that the rhs must be recalculated later.
              control_info[v] = control_info[v] & ~RELEVANCE_MASK; // Reset relevance, which is set in set_rhs.
              variable_index.erase(variable_index.get(v));
              assert(!variable_index.defined(v));
            }
          }
        }
      }
    }

    void count_variable_relevance_on(const size_t maximal_todo_size,                      
                                     size_t& queue_put_count_extra,
                                     const bool approximate_true)
    {
      assert(!count_variable_relevance);
      count_variable_relevance=true;
      refresh_relevances(maximal_todo_size,queue_put_count_extra,approximate_true);
    }

    bool is_relevant(const variable_type v)
    {
      assert(0<v);
      check_vector_sizes(v);
      if (count_variable_relevance)
      {
        return control_info[v] & RELEVANCE_MASK;
      }

      /* all variables are relevant if relevancy is not maintained */
      return true;
    }

    void construct_counter_example_on(void)
    {
      assert(!construct_counter_example);
      construct_counter_example=true;
      data_to_construct_counter_example.resize(nr_of_variables()+1,std::deque<counter_example>());
    }

    std::deque <counter_example>& counter_example_queue(variable_type v)
    {
      assert(construct_counter_example);
      check_vector_sizes(v);
      return data_to_construct_counter_example[v];
    }

    std::deque<counter_example>::iterator dummy_deque_iterator;

    std::deque<counter_example>::iterator counter_example_begin(variable_type v)
    {
      assert(construct_counter_example);
      if (v>nr_of_variables())
      {
        return dummy_deque_iterator;
      }
      return data_to_construct_counter_example[v].begin();
    }

    std::deque<counter_example>::iterator counter_example_end(variable_type v)
    {
      assert(construct_counter_example);
      if (v>nr_of_variables())
      {
        return dummy_deque_iterator;
      }
      return data_to_construct_counter_example[v].end();
    }

    // The following function indicates whether there is a mu/nu loop.
    bool find_mu_nu_loop_rec(
      const bes_expression& b,
      variable_type v,
      size_t rankv, // rank of v may not have been stored yet.
      std::map < variable_type, bool >& visited_variables,
      bool is_mu)
    {
      if (is_false(b) || is_true(b) || is_dummy(b))
      {
        return false;
      }

      if (is_variable(b))
      {
        variable_type w=get_variable(b);
        if (w==v)
        {
          return true;
        }
        if (get_rank(w)!=rankv)
        {
          return false;
        }
        if (visited_variables.find(w)!=visited_variables.end())
        {
          return visited_variables[w];
        }

        visited_variables[w]=false;
        bool result;
        result=find_mu_nu_loop_rec(get_rhs(w),v,rankv,visited_variables,is_mu);
        visited_variables[w]=result;
        return result;
      }

      if (is_mu)
      {
        if (is_and(b))
        {
          return find_mu_nu_loop_rec(lhs(b),v,rankv,visited_variables,is_mu) ||
                 find_mu_nu_loop_rec(rhs(b),v,rankv,visited_variables,is_mu);
        }

        if (is_or(b))
        {
          return find_mu_nu_loop_rec(lhs(b),v,rankv,visited_variables,is_mu) &&
                 find_mu_nu_loop_rec(rhs(b),v,rankv,visited_variables,is_mu);
        }
      }
      else
      {
        if (is_and(b))
        {
          return find_mu_nu_loop_rec(lhs(b),v,rankv,visited_variables,is_mu) &&
                 find_mu_nu_loop_rec(rhs(b),v,rankv,visited_variables,is_mu);
        }

        if (is_or(b))
        {
          return find_mu_nu_loop_rec(lhs(b),v,rankv,visited_variables,is_mu) ||
                 find_mu_nu_loop_rec(rhs(b),v,rankv,visited_variables,is_mu);
        }
      }

      if (is_if(b))
      {
        bool r=find_mu_nu_loop_rec(condition(b),v,rankv,visited_variables,is_mu);
        if (r)
        {
          if (is_mu)
          {
            return find_mu_nu_loop_rec(else_branch(b),v,rankv,visited_variables,is_mu);
          }
          return find_mu_nu_loop_rec(then_branch(b),v,rankv,visited_variables,is_mu);
        }
        return false;
      }

      assert(0); // One should not end up here.
      return false;
    }

    /* delivers true if a loop from variables in b to v is detected. */
    bool find_mu_loop(
      const bes_expression& b,
      variable_type v,
      size_t rankv)
    {
      std::map < variable_type, bool > visited_variables; // Intentionally a map, and not a set.
      bool result=find_mu_nu_loop_rec(b,v,rankv,visited_variables,true);
      return result;
    }

    bool find_nu_loop(
      const bes_expression& b,
      variable_type v,
      size_t rankv)
    {
      std::map < variable_type, bool > visited_variables; // Intentionally a map, and not a set.
      return find_mu_nu_loop_rec(b,v,rankv,visited_variables,false); 
    }

////////////////////////////  Functions to generate a bes out of a pbes  /////////////////////////


    //function add_propositional_variable_instantiations_to_indexed_set
    //and translate to pbes expression to a bes_expression in BDD format.
    bes_expression add_propositional_variable_instantiations_to_indexed_set_and_translate(
      const mcrl2::pbes_system::pbes_expression& p,
      atermpp::indexed_set<propositional_variable_instantiation_as_tree>& variable_index,
      size_t& nr_of_generated_variables,
      const bool to_bdd,
      const transformation_strategy strategy,
      const bool construct_counter_example,
      const variable_type current_variable)
    {
      using namespace mcrl2::pbes_system;
      if (is_propositional_variable_instantiation(p))
      {
        const propositional_variable_instantiation& p1 = atermpp::down_cast<propositional_variable_instantiation>(p);
        std::pair<size_t,bool> pr=variable_index.put(propositional_variable_instantiation_as_tree(p1));
        if (pr.second) /* p is added to the indexed set, so it is a new variable */
        {
          nr_of_generated_variables++;
          if (to_bdd)
          {
            return if_(variable(pr.first),true_(),false_());
          }
          else
          {
            return variable(pr.first);
          }
        }
        else
        {
          if (strategy>lazy)
          {
            bes_expression b=get_rhs(pr.first);
            if (is_true(b))
            {
              if (construct_counter_example)
              {
                counter_example_queue(current_variable).
                push_front(counter_example(pr.first,FORWARD_SUBSTITUTION_TRUE));
              }
              return b;
            }
            if (is_false(b))
            {
              if (construct_counter_example)
              {
                counter_example_queue(current_variable).
                push_front(counter_example(pr.first,FORWARD_SUBSTITUTION_FALSE));
              }
              return b;
            }
          }
          if (to_bdd)
          {
            return if_(variable(pr.first),true_(),false_());
          }
          else
          {
            return variable(pr.first);
          }
        }
      }
      else if (mcrl2::pbes_system::is_and(p))
      {
        bes_expression b1=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                            accessors::left(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                            construct_counter_example,current_variable);
        if (is_false(b1))
        {
          return b1;
        }
        bes_expression b2=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                            accessors::right(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                            construct_counter_example,current_variable);
        if (is_false(b2))
        {
          return b2;
        }
        if (is_true(b1))
        {
          return b2;
        }
        if (is_true(b2))
        {
          return b1;
        }
        if (b1==b2)
        {
          return b1;
        }
        if (to_bdd)
        {
          return BDDif(b1,b2,false_());
        }
        else
        {
          return and_(b1,b2);
        }
      }
      else if (mcrl2::pbes_system::is_or(p))
      {
        bes_expression b1=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                            accessors::left(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                            construct_counter_example,current_variable);
        if (is_true(b1))
        {
          return b1;
        }

        bes_expression b2=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                            accessors::right(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                            construct_counter_example,current_variable);
        if (is_true(b2))
        {
          return b2;
        }
        if (is_false(b1))
        {
          return b2;
        }
        if (is_false(b2))
        {
          return b1;
        }
        if (b1==b2)
        {
          return b1;
        }

        if (to_bdd)
        {
          return BDDif(b1,true_(),b2);
        }
        else
        {
          return or_(b1,b2);
        }
      }
      else if (mcrl2::pbes_system::is_imp(p))
      {
        bes_expression b1=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                            accessors::left(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                            construct_counter_example,current_variable);
        if (is_false(b1))
        {
          return true_();
        }

        bes_expression b2=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                            accessors::right(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                            construct_counter_example,current_variable);
        if (is_true(b2))
        {
          return b2;
        }
        if (is_true(b1))
        {
          return b2;
        }
        if (is_false(b2))
        {
          return not_(b1);
        }
        if (b1==b2)
        {
          return true_();
        }

        if (to_bdd)
        {
          return BDDif(b1,b2,true_());
        }
        else
        {
          return imp(pbes_expression(b1), pbes_expression(b2));
        }
      }
      else if (mcrl2::pbes_system::is_not(p))
      {
        bes_expression b1=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                            atermpp::down_cast<mcrl2::pbes_system::not_>(p).operand(),variable_index,nr_of_generated_variables,to_bdd,strategy,
                            construct_counter_example,current_variable);
        return BDDif(b1,false_(),true_());
      }
      else if (is_true(p)||p==mcrl2::data::sort_bool::true_())
      {
        return true_();
      }
      else if (is_false(p)||p==mcrl2::data::sort_bool::false_())
      {
        return false_();
      }
      throw mcrl2::runtime_error("Unexpected expression. Most likely because expression fails to rewrite to true or false: " +
                                   mcrl2::pbes_system::pp(p));
      return false_();
    }

    /// \brief Calculate a BES from a given PBES
    /// \details
    /// Global variables
    ///  atermpp::indexed_set variable_index(10000, 50);
    ///  bes_equations;
    // template < typename Container, typename PbesRewriter >
    boolean_equation_system(
      const mcrl2::pbes_system::pbes& pbes_spec,
      mcrl2::data::rewriter& data_rewriter,
      const transformation_strategy opt_transformation_strategy=lazy,
      search_strategy opt_search_strategy=breadth_first,
      const bool opt_construct_counter_example=false,
      const remove_level opt_remove_unused_bes_variables=none,
      const bool opt_use_hashtables=false,
      const size_t maximal_todo_size=atermpp::npos,
      const bool approximate_true=true):
      control_info(1),
      right_hand_sides(1),
      variable_occurrences_are_stored(false),
      variable_occurrence_sets(1),
      count_variable_relevance(false),
      data_to_construct_counter_example(1),
      construct_counter_example(false),
      remove_unused_bes_variables(opt_remove_unused_bes_variables),
      max_rank(0)
    {
      if (opt_search_strategy==breadth_first_short) opt_search_strategy=breadth_first;
      if (opt_search_strategy==depth_first_short) opt_search_strategy=depth_first;
      const bool enumerate_infinite_sorts=true;
      mcrl2::pbes_system::enumerate_quantifiers_rewriter pbes_quantifier_eliminating_rewriter(data_rewriter, pbes_spec.data(), enumerate_infinite_sorts);

      mcrl2::pbes_system::simplify_quantifiers_data_rewriter<mcrl2::data::rewriter> pbes_simplify_rewriter(data_rewriter); 

      mcrl2::pbes_system::one_point_rule_rewriter pbes_one_point_rule_rewriter; 


      using namespace mcrl2::data;
      using namespace mcrl2::pbes_system;
      assert(pbes_spec.is_well_typed());
      assert(pbes_spec.is_closed());

      // Verbose msg: doing naive algorithm

      // Declare all constructors and mappings to the rewriter to prevent unnecessary compilation.
      // This can be removed if the jittyc compiler is not in use anymore.
      std::set < mcrl2::data::variable > vset=mcrl2::pbes_system::find_all_variables(pbes_spec);
      std::set < mcrl2::data::variable > vfset=mcrl2::pbes_system::find_free_variables(pbes_spec);
      std::set < mcrl2::data::variable > diff_set;
      std::set_difference(vfset.begin(),vfset.end(),vset.begin(),vset.end(),std::inserter(diff_set,diff_set.begin()));

      // Declare two variable substitutions for use in the rewriters. Only one is necessary if precompilation
      // can be switched off.
      substitution_type sigma;

      std::set < sort_expression > bounded_sorts;
      for(std::set < mcrl2::data::variable > :: const_iterator i=diff_set.begin(); i!=diff_set.end(); ++i)
      {
        bounded_sorts.insert(i->sort());
      }

      // Variables in which the result is stored
      propositional_variable_instantiation new_initial_state;

      // atermpp::indexed_set variable_index(10000, 50);
      // In order to generate a counterexample, this must also be known outside
      // this procedure.
      // variable_index.put(true_());
      variable_index.put(propositional_variable_instantiation_as_tree(mcrl2::core::identifier_string("Initial_pbes1"),data_expression_list()));
      variable_index.put(propositional_variable_instantiation_as_tree(mcrl2::core::identifier_string("Initial_pbes2"),data_expression_list()));
      
                                        /* Put first two dummy terms that
                                           gets index 0 and 1 in the indexed set, to
                                           take care that the first variable gets an index 2, to
                                           make space for a first equation of the shape X1=X2. */

      /* The following list contains that variables that need to be explored.
         This list is only relevant if opt_transformation_strategy>=on_the_fly 
         or opt_search_strategy==depth_first,
         as in the other case the variables to be investigated are those
         with indices between nre_of_processed_variables and nr_of_generated
         variables. */
      std::deque < variable_type> todo;
      size_t queue_put_count_extra=0;
      if (opt_transformation_strategy>=on_the_fly || opt_search_strategy==depth_first || maximal_todo_size!=atermpp::npos)
      {
        insert_element_in_queue(todo,2,maximal_todo_size,queue_put_count_extra,approximate_true);
      }
      // Data rewriter
      pbes_expression p=pbes_expression_order_quantified_variables(pbes_one_point_rule_rewriter(pbes_simplify_rewriter(pbes_spec.initial_state())),pbes_spec.data());

      const propositional_variable_instantiation& p1 = atermpp::down_cast<propositional_variable_instantiation>(p);
      variable_index.put(propositional_variable_instantiation_as_tree(p1));

      if (opt_transformation_strategy>=on_the_fly || opt_search_strategy==depth_first || maximal_todo_size!=atermpp::npos)
      {
        store_variable_occurrences();
        count_variable_relevance_on(maximal_todo_size,queue_put_count_extra,approximate_true);
      }

      if (opt_construct_counter_example)
      {
        construct_counter_example_on();
      }

      // Needed hashtables
      std::vector<pbes_equation> eqsys = pbes_spec.equations();
      std::map< mcrl2::core::identifier_string, pbes_equation > pbes_equations;

      // Vector with the order of the variable names used for sorting the result

      std::map < mcrl2::core::identifier_string, size_t> variable_rank;

      // Fill the pbes_equations table
      mCRL2log(mcrl2::log::verbose) << "Retrieving pbes_equations from equation system..." << std::endl;

      assert(eqsys.size()>0); // There should be at least one equation
      fixpoint_symbol current_fixpoint_symbol=eqsys.begin()->symbol();

      size_t rank=1;

      for (auto eqi = eqsys.begin(); eqi != eqsys.end(); eqi++)
      {
        // Rewriting terms here can lead to non termination, in
        // case the quantifier-all rewriter is used. Therefore,
        // only simpler rewrite steps are used here.

        pbes_equations[eqi->variable().name()]=
                  pbes_equation(eqi->symbol(), eqi->variable(), 
                    pbes_expression_order_quantified_variables(
                       pbes_one_point_rule_rewriter(
                                 pbes_simplify_rewriter(eqi->formula(),sigma)),
                       pbes_spec.data())); 
        
        if (eqi->symbol()!=current_fixpoint_symbol)
        {
          current_fixpoint_symbol=eqi->symbol();
          rank=rank+1;
        }
        variable_rank[eqi->variable().name()]=rank;
      }

      size_t relevance_counter=0;
      size_t relevance_counter_limit=100;
#define RELEVANCE_DIVIDE_FACTOR 2

      mCRL2log(mcrl2::log::verbose) << "Computing a BES from the PBES...." << std::endl;

      // Set the first BES equation X1=X2
      add_equation(
        1,
        eqsys.begin()->symbol(),
        1,
        variable(2),
        maximal_todo_size,queue_put_count_extra,approximate_true);

      // Variables used in whole function
      size_t nr_of_processed_variables = 1;
      size_t nr_of_generated_variables = 2;
      time_t last_log_time = time(NULL) - 1;

      // As long as there are states to be explored
      while ((opt_transformation_strategy>=on_the_fly || opt_search_strategy==depth_first || maximal_todo_size!=atermpp::npos)
             ?todo.size()>0
             :(nr_of_processed_variables < nr_of_generated_variables))
      {
        variable_type variable_to_be_processed;
        if (opt_transformation_strategy>=on_the_fly || opt_search_strategy==depth_first || maximal_todo_size!=atermpp::npos)
        {
          if (opt_search_strategy==breadth_first)
          { 
            // Do a breadth-first search
            variable_to_be_processed=todo.front();
            todo.pop_front();
          }
          else 
          {
            // Do a depth-first search
            variable_to_be_processed=todo.back();
            todo.pop_back();
          }
        }
        else
        {
          variable_to_be_processed=nr_of_processed_variables+1;
        }

        if (is_relevant(variable_to_be_processed))
          // If v is not relevant, it does not need to be investigated.
        {

          pbes_equation current_pbeq;

          // Add the required substitutions
          // The current variable instantiation is stored as a tree, and this tree must be unfolded.
          const propositional_variable_instantiation_as_tree& t=variable_index.get(variable_to_be_processed);
          current_pbeq = pbes_equations[t.name()];
          variable_list::iterator iter=current_pbeq.variable().parameters().begin();
          t.assign_variables_in_tree(iter,sigma);

          bes_expression new_bes_expression;
          try
          {
            pbes_expression new_pbes_expression=pbes_quantifier_eliminating_rewriter(current_pbeq.formula(),sigma);
            new_bes_expression=
              add_propositional_variable_instantiations_to_indexed_set_and_translate(
                new_pbes_expression,
                variable_index,
                nr_of_generated_variables,
                opt_use_hashtables,
                opt_transformation_strategy,
                opt_construct_counter_example,
                variable_to_be_processed);
          }
          catch (mcrl2::runtime_error& e)
          {
            const propositional_variable_instantiation_as_tree& t=variable_index.get(variable_to_be_processed);
            propositional_variable_instantiation prop_var(t.name(),data_expression_list(t.arguments().begin(), t.arguments().end()));
            throw mcrl2::runtime_error(std::string(e.what()) + "\nError occurred when investigating " +
                  mcrl2::pbes_system::pp(prop_var));
          }

          for (variable_list::iterator vlist=current_pbeq.variable().parameters().begin() ;
               vlist!=current_pbeq.variable().parameters().end() ; vlist++)
          {
            sigma[*vlist]=data_expression(*vlist);
          }


          if (opt_transformation_strategy>=on_the_fly_with_fixed_points)
          {
            // find a variable in the new_bes_expression from which `variable' to be
            // processed is reachable. If so, new_bes_expression can be set to
            // true or false.

            if (current_pbeq.symbol()==fixpoint_symbol::mu())
            {
              if (find_mu_loop(
                    new_bes_expression,
                    variable_to_be_processed,
                    variable_rank[current_pbeq.variable().name()]))
              {
                new_bes_expression=false_();
                if (opt_construct_counter_example)
                {
                  counter_example_queue(variable_to_be_processed).
                  push_back(counter_example(variable_to_be_processed,MU_CYCLE));
                }
              }
            }
            else
            {
              if (find_nu_loop(
                    new_bes_expression,
                    variable_to_be_processed,
                    variable_rank[current_pbeq.variable().name()]))
              {
                new_bes_expression=true_();
                if (opt_construct_counter_example)
                {
                  counter_example_queue(variable_to_be_processed).
                  push_back(counter_example(variable_to_be_processed,NU_CYCLE));
                }
              }
            }
          }

          if (opt_transformation_strategy>=on_the_fly || opt_search_strategy==depth_first || maximal_todo_size!=atermpp::npos)
          {
            add_equation(
              variable_to_be_processed,
              current_pbeq.symbol(),
              variable_rank[current_pbeq.variable().name()],
              new_bes_expression,
              maximal_todo_size,queue_put_count_extra,approximate_true,
              todo);

            /* So now and then (after doing as many operations on the size of bes_equations,
               the relevances of variables must be reset, to avoid investigating irrelevant
               variables. There is an invariant in the system that all variables reachable
               from the initial variable 1, are always relevant. Furthermore, relevant
               variables that need to be investigated are always in the todo list */
            relevance_counter++;
          }
          else
          {
            add_equation(
              variable_to_be_processed,
              current_pbeq.symbol(),
              variable_rank[current_pbeq.variable().name()],
              new_bes_expression,
              maximal_todo_size,queue_put_count_extra,approximate_true);
          }

          if (opt_transformation_strategy>=on_the_fly)
          {
            if (is_true(new_bes_expression)||is_false(new_bes_expression))
            {
              // new_bes_expression is true or false and opt_transformation_strategy is on the fly or higher.
              // This means we must optimize the y substituting true/false for this variable
              // everywhere. For this we use the occurrence set.

              std::set <variable_type> to_set_to_true_or_false;
              to_set_to_true_or_false.insert(variable_to_be_processed);
              for (; !to_set_to_true_or_false.empty() ;)
              {
                variable_type w=(*to_set_to_true_or_false.begin());

                // Take the lowest element for substitution, to generate
                // short counterexample.

                to_set_to_true_or_false.erase(w);
                for (std::set <variable_type>::iterator
                     v=variable_occurrence_set_begin(w);
                     v!=variable_occurrence_set_end(w);
                     v++)
                {

                  bes_expression b=get_rhs(*v);
                  if (opt_construct_counter_example)
                  {
                    b=substitute_true_false(b,w,get_rhs(w),
                                            counter_example_queue(*v));
                  }
                  else
                  {
                    b=substitute_true_false(b,w,get_rhs(w));
                  }

                  if (is_true(b)||is_false(b))
                  {
                    to_set_to_true_or_false.insert(*v);
                  }
                  set_rhs(*v,b,maximal_todo_size,queue_put_count_extra,approximate_true,w);
                }
                relevance_counter++;
                clear_variable_occurrence_set(w);
              }
            }
          }
        }
        if (relevance_counter>=relevance_counter_limit)
        {
          relevance_counter_limit=nr_of_variables()/RELEVANCE_DIVIDE_FACTOR;
          relevance_counter=0;
          refresh_relevances(maximal_todo_size,queue_put_count_extra,approximate_true,todo);
        }
        nr_of_processed_variables++;
        time_t new_log_time=0;
        if (time(&new_log_time) > last_log_time)
        {
          last_log_time = new_log_time;
          mCRL2log(mcrl2::log::status) << "Processed " << nr_of_processed_variables <<
                        " and generated " << nr_of_generated_variables <<
                        " boolean variables with a todo buffer of size ";
          if (opt_transformation_strategy>=on_the_fly || opt_search_strategy==depth_first || maximal_todo_size!=atermpp::npos)
          {
            mCRL2log(mcrl2::log::status) << todo.size() << " and " << variable_index.size() << " stored bes variables";;
          }
          else
          {
            mCRL2log(mcrl2::log::status) << nr_of_generated_variables-nr_of_processed_variables;
          }
          mCRL2log(mcrl2::log::status) << ".     " << std::endl;
        }
      }
      refresh_relevances(maximal_todo_size,queue_put_count_extra,approximate_true);
    }


  private:

    void print_counter_example_rec(bes::variable_type current_var,
                                   std::string indent,
                                   std::vector<bool>& already_printed,
                                   std::ostream& f,
                                   const bool formula_is_valid)
    {
      using namespace mcrl2::data;
      using namespace mcrl2::pbes_system;
      using namespace atermpp;
      const propositional_variable_instantiation_as_tree& t=variable_index.get(current_var);
      f << t.name();
      if (!t.arguments().empty())
      {
        f << "("; 
        for(atermpp::term_balanced_tree<mcrl2::data::data_expression>::iterator i=t.arguments().begin(); i!=t.arguments().end(); ++i)
        {
          f << (i==t.arguments().begin()?"":",") << *i;
         
        }
        f << ")";
      }
     
      if (already_printed[current_var])
      {
        f << "*\n";
      }
      else
      {
        f << "\n";
        already_printed[current_var]=true;

        for (std::deque < bes::counter_example>::iterator walker=counter_example_begin(current_var);
             walker!=counter_example_end(current_var) ; walker++)
        {
          if ((*walker).reason_indicates_that_formula_evaluated_to_given_truth_value(formula_is_valid))
          {
            f << indent << (*walker).get_variable() << ": " << (*walker).print_reason() << "  " ;
            print_counter_example_rec((*walker).get_variable(),indent+"  ",
                                      already_printed,
                                      f,
                                      formula_is_valid);
          }
        }
      }
    }

  public:
    
    void print_counter_example(const std::string filename, const bool formula_is_valid)
    {
      std::vector <bool> already_printed(nr_of_variables()+1,false);
      if (filename.empty())
      {
        // Print the counterexample to cout.
        std::cout << "Below the justification for this outcome is listed\n1: ";
        print_counter_example_rec(2,"  ",already_printed,std::cout,formula_is_valid);
      }
      try
      {
        std::ofstream f(filename.c_str());
        f << "Below the justification for this outcome is listed\n1: ";
        print_counter_example_rec(2,"  ",already_printed,f,formula_is_valid);
        f.close();
      }
      catch (std::exception& e)
      {
        mCRL2log(mcrl2::log::warning) << "Fail to write counterexample to " << filename <<
                  "(" << e.what() << ")" << std::endl;
      }
    }
};


} // namespace bes.


namespace std
{
/// Template specialisation for 
template<>
struct hash<bes::propositional_variable_instantiation_as_tree >
{
  std::size_t operator()(const bes::propositional_variable_instantiation_as_tree& t) const
  {
    size_t seed=std::hash<atermpp::aterm>()(t.name());
    return std::hash<atermpp::aterm>()(t.arguments()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
};


} // namespace std


namespace bes
{

inline void save_bes_in_pbes_format(
  const std::string& outfilename,
  boolean_equation_system& bes_equations,
  const mcrl2::pbes_system::pbes& p);
inline void save_bes_in_cwi_format(const std::string& outfilename,boolean_equation_system& bes_equations);

//function generate_rhs_as_bes_formula
//---------------------------
static mcrl2::bes::boolean_expression generate_rhs_as_bes_formula(const bes_expression& b)
{
  using namespace mcrl2::pbes_system;
  if (is_true(b))
  {
    return mcrl2::bes::true_();
  }
  else if (is_false(b))
  {
    return mcrl2::bes::false_();
  }
  else if (is_and(b))
  {
    return mcrl2::bes::and_(generate_rhs_as_bes_formula(lhs(b)),
                            generate_rhs_as_bes_formula(rhs(b)));
  }
  else if (is_or(b))
  {
    return mcrl2::bes::or_(generate_rhs_as_bes_formula(lhs(b)),
                           generate_rhs_as_bes_formula(rhs(b)));
  }
  else if (is_variable(b))
  {
    std::stringstream converter;
    converter << "X" << get_variable(b);
    return mcrl2::bes::boolean_variable(converter.str());
  }
  else if (is_if(b))
  {
    //BESIF(x,y,z) is equivalent to (y & (x|z)) provided the expression is monotonic.
    return generate_rhs_as_bes_formula(and_optimized(then_branch(b),
                                       or_optimized(condition(b),else_branch(b))));
  }
  else
  {
    throw mcrl2::runtime_error("The generated equation system is not a BES. It cannot be saved in CWI-format.\n");
  }
  return mcrl2::bes::true_();
}

inline
mcrl2::bes::boolean_equation_system convert_to_bes(boolean_equation_system& bes_equations)
{
  using namespace mcrl2::pbes_system;
  using namespace mcrl2::bes;
  mCRL2log(mcrl2::log::verbose) << "Converting result to BES-format..." << std::endl;
  // Use an indexed set to keep track of the variables and their bes-representations

  std::vector < boolean_equation > eqns;
  for (size_t r=1 ; r<=bes_equations.max_rank ; ++r)
  {
    for (size_t i=1; i<=bes_equations.nr_of_variables() ; ++i)
    {
      if (bes_equations.is_relevant(i) && (bes_equations.get_rank(i)==r))
      {
        mcrl2::bes::boolean_expression be=generate_rhs_as_bes_formula(bes_equations.get_rhs(i));
        std::stringstream variable_name;
        variable_name << "X" << i;
        eqns.push_back(
          boolean_equation(
            bes_equations.get_fixpoint_symbol(i),
            mcrl2::bes::boolean_variable(variable_name.str()),
            be));
      }
    }
  }

  mcrl2::bes::boolean_equation_system result(eqns,
      boolean_variable("X1"));

  return result;
}

/// From here there are routines to solve a BES.

/* substitute boolean equation expression */
static bes_expression substitute_rank(
  const bes_expression& b,
  const size_t current_rank,
  const std::vector<bes_expression>& approximation,
  bes::boolean_equation_system& bes_equations,
  const bool use_hashtable,
  std::map <bes_expression, bes_expression >& hashtable,
  bool store_counter_example=false,
  bes::variable_type current_variable=0)
{
  /* substitute variables with rank larger and equal
     than current_rank with their approximations. */

  if (bes::is_true(b)||bes::is_false(b)||bes::is_dummy(b))
  {
    return b;
  }

  bes_expression result;
  if (use_hashtable)
  {
    result=hashtable[b];
    if (result!=bes_expression())
    {
      return result;
    }
  }

  if (is_variable(b))
  {
    bes::variable_type v=bes::get_variable(b);
    if (bes_equations.get_rank(v)==current_rank)
    {
      result=approximation[v];
      if (store_counter_example)
      {
        if (bes::is_true(result))
        {
          bes_equations.counter_example_queue(current_variable).
          push_front(bes::counter_example(v,bes::APPROXIMATION_TRUE));
        }
        else if (bes::is_false(result))
        {
          bes_equations.counter_example_queue(current_variable).
          push_front(bes::counter_example(v,bes::APPROXIMATION_FALSE));
        }
        else
        {
          bes_equations.counter_example_queue(current_variable).
          push_front(bes::counter_example(v,bes::APPROXIMATION));
        }
      }
    }
    else
    {
      result=b;
    }
  }

  else if (is_and(b))
  {
    bes_expression b1=substitute_rank(lhs(b),current_rank,approximation,bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
    if (is_false(b1))
    {
      result=b1;
    }
    else
    {
      bes_expression b2=substitute_rank(rhs(b),current_rank,approximation,bes_equations,
                                        use_hashtable,hashtable,store_counter_example,current_variable);
      if (is_false(b2))
      {
        result=b2;
      }
      else if (is_true(b1))
      {
        result=b2;
      }
      else if (is_true(b2))
      {
        result=b1;
      }
      else if (b1==b2)
      {
        result=b1;
      }
      else
      {
        result=and_(b1,b2);
      }
    }
  }
  else if (is_or(b))
  {
    bes_expression b1=substitute_rank(lhs(b),current_rank,approximation,bes_equations,use_hashtable,
                                      hashtable,store_counter_example,current_variable);
    if (is_true(b1))
    {
      result=b1;
    }
    else
    {
      bes_expression b2=substitute_rank(rhs(b),current_rank,approximation,bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
      if (is_true(b2))
      {
        result=b2;
      }
      else if (is_false(b1))
      {
        result=b2;
      }
      else if (is_false(b2))
      {
        result=b1;
      }
      else if (b1==b2)
      {
        result=b1;
      }
      else
      {
        result=or_(b1,b2);
      }
    }
  }


  else if (is_if(b))
  {
    bes::variable_type v=bes::get_variable(condition(b));
    if (bes_equations.get_rank(v)==current_rank)
    {
      if (bes::is_true(approximation[v]))
      {
        result=substitute_rank(then_branch(b),current_rank,approximation,bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
        if (store_counter_example)
        {
          bes_equations.counter_example_queue(current_variable).
          push_front(bes::counter_example(v,bes::APPROXIMATION_TRUE));
        }
      }
      else if (bes::is_false(approximation[v]))
      {
        result=substitute_rank(else_branch(b),current_rank,approximation,bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
        if (store_counter_example)
        {
          bes_equations.counter_example_queue(current_variable).
          push_front(bes::counter_example(v,bes::APPROXIMATION_FALSE));
        }
      }
      else
      {
        bes_expression b1=substitute_rank(then_branch(b),current_rank,approximation,
                                          bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
        bes_expression b2=substitute_rank(else_branch(b),current_rank,approximation,
                                          bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
        result=BDDif(approximation[v],b1,b2);
      }
    }
    else
    {
      /* the condition is not equal to v */
      bes_expression b1=substitute_rank(then_branch(b),current_rank,approximation,
                                        bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
      bes_expression b2=substitute_rank(else_branch(b),current_rank,approximation,
                                        bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
      if ((b1==then_branch(b)) && (b2==else_branch(b)))
      {
        result=b;
      }
      else
      {
        result=BDDif(bes::if_(condition(b),bes::true_(),bes::false_()),b1,b2);
      }
    }
  }
  else
  {
    assert(0);  // expect an if, variable, and, or, or a true or a false here.
  }

  if (use_hashtable)
  {
    hashtable[b]=result;
  }
  return result;
}

/* substitute boolean equation expression */

static bes_expression evaluate_bex(
  const bes_expression& b,
  const std::vector<bes_expression>& approximation,
  const size_t rank,
  bes::boolean_equation_system& bes_equations,
  const bool use_hashtable,
  std::map<bes_expression, bes_expression>& hashtable,
  const bool construct_counter_example,
  const bes::variable_type current_variable)
{
  /* substitute the approximation for variables in b, given
     by approximation, for all those variables that have a
     rank higher or equal to the variable rank; */

  if (bes::is_true(b)||bes::is_false(b))
  {
    return b;
  }

  bes_expression result;
  if (use_hashtable)
  {
    result=hashtable[b];
    if (result!=bes_expression())
    {
      return result;
    }
  }
  if (is_variable(b))
  {
    const bes::variable_type v=bes::get_variable(b);
    if (bes_equations.get_rank(v)>=rank)
    {
      result=approximation[v];
      if (construct_counter_example)
      {
        if (bes::is_true(result))
        {
          bes_equations.counter_example_queue(current_variable).
          push_front(bes::counter_example(v,bes::APPROXIMATION_TRUE));
        }
        else if (bes::is_false(result))
        {
          bes_equations.counter_example_queue(current_variable).
          push_front(bes::counter_example(v,bes::APPROXIMATION_FALSE));
        }
        else
        {
          bes_equations.counter_example_queue(current_variable).
          push_front(bes::counter_example(v,bes::APPROXIMATION));
        }
      }
    }
    else
    {
      /* the condition has lower rank than the variable rank,
         leave it untouched */
      result=b;
    }
  }
  else if (is_and(b))
  {
    bes_expression b1=evaluate_bex(lhs(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
    if (is_false(b1))
    {
      result=b1;
    }
    else
    {
      bes_expression b2=evaluate_bex(rhs(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      if (is_false(b2))
      {
        result=b2;
      }
      else if (is_true(b1))
      {
        result=b2;
      }
      else if (is_true(b2))
      {
        result=b1;
      }
      else if (b1==b2)
      {
        result=b1;
      }
      else
      {
        result=and_(b1,b2);
      }
    }
  }
  else if (is_or(b))
  {
    bes_expression b1=evaluate_bex(lhs(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
    if (is_true(b1))
    {
      result=b1;
    }
    else
    {
      bes_expression b2=evaluate_bex(rhs(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      if (is_true(b2))
      {
        result=b2;
      }
      else if (is_false(b1))
      {
        result=b2;
      }
      else if (is_false(b2))
      {
        result=b1;
      }
      else if (b1==b2)
      {
        result=b1;
      }
      else
      {
        result=or_(b1,b2);
      }
    }
  }
  else if (is_if(b))
  {
    bes::variable_type v=bes::get_variable(condition(b));
    if (bes_equations.get_rank(v)>=rank)
    {
      bes_expression b1=evaluate_bex(then_branch(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      bes_expression b2=evaluate_bex(else_branch(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      result=BDDif(approximation[v],b1,b2);
    }
    else
    {
      /* the condition has lower rank than the variable rank,
         leave it untouched */
      bes_expression b1=evaluate_bex(then_branch(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      bes_expression b2=evaluate_bex(else_branch(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      if ((b1==then_branch(b)) && (b2==else_branch(b)))
      {
        result=b;
      }
      else
      {
        result=BDDif(bes::if_(condition(b),bes::true_(),bes::false_()),b1,b2);
      }
    }
  }
  else
  {
    assert(0); //expect an if, true or false, and, variable or or;
  }

  if (use_hashtable)
  {
    hashtable[b]=result;
  }
  return result;
}

inline
bool solve_bes(bes::boolean_equation_system& bes_equations,
               const bool opt_use_hashtables,
               const bool opt_construct_counter_example)
{
  using namespace mcrl2::core;
  using namespace std;
  using namespace mcrl2::pbes_system;

  mCRL2log(mcrl2::log::verbose) << "Solving a BES with " << bes_equations.nr_of_variables() <<
              " equations." << std::endl;

  const size_t maximal_todo_size=atermpp::npos;
  size_t queue_put_count_extra=0;
  const bool approximate_true=true;
  std::vector<bes_expression> approximation(bes_equations.nr_of_variables()+1);

  std::map<bes_expression, bes_expression> bex_hashtable;

  /* Set the approximation to its initial value */
  for (bes::variable_type v=bes_equations.nr_of_variables(); v>0; v--)
  {
    bes_expression b=bes_equations.get_rhs(v);
    if (b!=bes::dummy())
    {
      if (bes::is_true(bes_equations.get_rhs(v)))
      {
        approximation[v]=bes::true_();
      }
      else if (bes::is_false(bes_equations.get_rhs(v)))
      {
        approximation[v]=bes::false_();
      }
      else if (bes_equations.get_fixpoint_symbol(v)==fixpoint_symbol::mu())
      {
        approximation[v]=bes::false_();
      }
      else
      {
        approximation[v]=bes::true_();
      }
    }
  }

  bes_equations.store_variable_occurrences();

  for (size_t current_rank=bes_equations.max_rank;
       current_rank>0 ; current_rank--)
  {
    mCRL2log(mcrl2::log::verbose) << "Solve equations of rank " << current_rank << "." << std::endl;

    /* Calculate the stable solution for the current rank */

    set <bes::variable_type> todo;

    for (bes::variable_type v=bes_equations.nr_of_variables(); v>0; v--)
    {
      if (bes_equations.is_relevant(v) && (bes_equations.get_rank(v)==current_rank))
      {
        bes_expression t=evaluate_bex(
                           bes_equations.get_rhs(v),
                           approximation,
                           current_rank,
                           bes_equations,
                           opt_use_hashtables,
                           bex_hashtable,
                           false,
                           v);

        if (toBDD(t)!=toBDD(approximation[v]))
        {
          if (opt_use_hashtables)
          {
            bex_hashtable.clear();  /* we change the approximation, so the
                                       hashtable becomes invalid */
          }
          if (opt_construct_counter_example)
          {
            if (opt_use_hashtables)
            {
              bex_hashtable.clear();  /* We want to construct a counter example, which is
                                         not traceable when results in the hashtable are used */
            }
            approximation[v]=evaluate_bex(
                               bes_equations.get_rhs(v),
                               approximation,
                               current_rank,
                               bes_equations,
                               opt_use_hashtables,
                               bex_hashtable,
                               true,
                               v);
          }
          else
          {
            approximation[v]=t;
          }
          todo.insert(v);
        }
      }
    }

    for (; todo.size()>0 ;)
    {
      set<bes::variable_type>::iterator w= todo.begin();
      bes::variable_type w_value=*w;
      todo.erase(w);

      for (set <bes::variable_type>::iterator
           u=bes_equations.variable_occurrence_set_begin(w_value);
           u!=bes_equations.variable_occurrence_set_end(w_value);
           u++)
      {
        if (bes_equations.is_relevant(*u) && (bes_equations.get_rank(*u)==current_rank))
        {
          bes_expression t=evaluate_bex(
                             bes_equations.get_rhs(*u),
                             approximation,
                             current_rank,
                             bes_equations,
                             opt_use_hashtables,
                             bex_hashtable,
                             false,
                             *u);

          if (toBDD(t)!=toBDD(approximation[*u]))
          {
            if (opt_use_hashtables)
            {
              bex_hashtable.clear();  /* we change approximation, so the
                                         hashtable becomes invalid */
            }
            if (opt_construct_counter_example)
            {
              if (opt_use_hashtables)
              {
                bex_hashtable.clear();  /* We want to construct a counter example, which is
                                           not traceable when results in the hashtable are used */
              }
              approximation[*u]=evaluate_bex(
                                  bes_equations.get_rhs(*u),
                                  approximation,
                                  current_rank,
                                  bes_equations,
                                  opt_use_hashtables,
                                  bex_hashtable,
                                  true,
                                  *u);
            }
            else
            {
              approximation[*u]=t;
            }
            todo.insert(*u);
          }
        }
      }
    }

    /* substitute the stable solution for the current rank in all other
       equations. */


    if (opt_use_hashtables)
    {
      bex_hashtable.clear();
    }

    for (bes::variable_type v=bes_equations.nr_of_variables(); v>0; v--)
    {
      if (bes_equations.is_relevant(v))
      {
        if (bes_equations.get_rank(v)==current_rank)
        {
          if (opt_construct_counter_example)
          {
            bes_equations.set_rhs(
              v,
              substitute_rank(
                bes_equations.get_rhs(v),
                current_rank,
                approximation,
                bes_equations,
                opt_use_hashtables,
                bex_hashtable,
                true,v),
                maximal_todo_size,queue_put_count_extra,approximate_true);
          }
          else
          {
            bes_equations.set_rhs(v,approximation[v],maximal_todo_size,queue_put_count_extra,approximate_true);
          }
        }
        else
        {
          bes_equations.set_rhs(
            v,
            substitute_rank(
              bes_equations.get_rhs(v),
              current_rank,
              approximation,
              bes_equations,
              opt_use_hashtables,
              bex_hashtable,
              opt_construct_counter_example,v),
              maximal_todo_size,queue_put_count_extra,approximate_true);
        }
      }
    }
    if (opt_use_hashtables)
    {
      bex_hashtable.clear();
    }

  }
  assert(bes::is_true(approximation[1])||
         bes::is_false(approximation[1]));
  return bes::is_true(approximation[1]);  /* 1 is the index of the initial variable */
}




} // end namespace bes

#endif // BES_DEPRECATED_H
