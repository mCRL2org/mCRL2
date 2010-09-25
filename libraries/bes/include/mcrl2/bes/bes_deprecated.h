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


#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/atermpp/table.h"
#include "mcrl2/core/messaging.h"

// #include "mcrl2/data/data_expression.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/map_substitution.h"


#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/fixpoint_symbol.h"
#include "mcrl2/pbes/utility.h"

#define RELEVANCE_MASK 1
#define FIXPOINT_MASK 2
#define RANK_SHIFT 2


namespace bes
{

// a bes variable_type is an unsigned long.

  typedef unsigned long variable_type; /* valid values start at 1 */

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
                        APPROXIMATION } reason;


  /// \brief Strategies for the generation of a BES from a PBES
  enum transformation_strategy {
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


  /* Declare a protected PAIR symbol */
  inline AFun initAFunPair(AFun& f)
  { f = ATmakeAFun("PAIR", 2, ATfalse);
    ATprotectAFun(f);
    return f;
  }

  inline AFun PAIR()
  { static AFun PAIR = initAFunPair(PAIR);
    return PAIR;
  }

  static ATermAppl apply_pair_symbol(ATermAppl t1, ATermAppl t2)
  {
    return ATmakeAppl2(PAIR(),(ATerm)t1,(ATerm)t2);
  }

  static bool is_pair(ATerm t)
  { return ATgetAFun(t)==PAIR();
  }

  static unsigned int largest_power_of_2_smaller_than(int i)
  { unsigned int j=1;
    i=i>>1;
    while (i>0)
    { i=i>>1;
      j=j*2;
    };
    return j;
  }

  static void assign_variables_in_tree(
                        ATerm t,
                        mcrl2::data::variable_list::iterator &var_iter,
                        mcrl2::pbes_system::detail::legacy_pbes_data_rewriter &rewriter,
                        const bool opt_precompile_pbes)
  { if (is_pair(t))
    { assign_variables_in_tree(ATgetArgument(t,0),var_iter,rewriter,opt_precompile_pbes);
      assign_variables_in_tree(ATgetArgument(t,1),var_iter,rewriter,opt_precompile_pbes);
    }
    else
    {
      if (opt_precompile_pbes)
      { rewriter.setSubstitutionInternal(*var_iter,t);
      }
      else
      { rewriter.setSubstitution(*var_iter,(ATermAppl)t);
      }
      var_iter++;
    }
  }

  // static ATermAppl store_as_tree(pbes_expression p)
  static ATermAppl store_as_tree(mcrl2::pbes_system::propositional_variable_instantiation p)
  /* We assume p is a propositional_variable_instantiation of the form B(x1,...,xn). If p has less than 3
   * arguments p is returned. Otherwise a term of the form B(pair(pair(...pair(x1,x2),...)))) is
   * returned, which is a balanced tree flushed to the right. For each input the resulting
   * tree is unique.
   */
  {
    mcrl2::data::data_expression_list args=p.parameters();

    if ( args.size() ==0 )
    return p.name();

    unsigned int n=largest_power_of_2_smaller_than(args.size());

    atermpp::vector<ATermAppl> tree_store(n);

    /* put the arguments in the intermediate tree_store. The last elements are stored as
     * pairs, such that the args.size() elements are stored in n positions. */
    unsigned int i=0;
    for(mcrl2::data::data_expression_list::const_iterator t=args.begin() ; t!=args.end(); t++)
    { if (i<2*n-args.size())
      { tree_store[i]= (*t);
        i++;
      }
      else
      { ATermAppl t1(*t);
        t++;
        ATermAppl t2(*t);
        tree_store[i]= apply_pair_symbol(t1,t2);
        i++;
      }
    }

    while (n>1)
    { n=n>>1; // n=n/2;
      for (unsigned int i=0; i<n; i++)
      {
        tree_store[i] = apply_pair_symbol(tree_store[2*i],tree_store[2*i+1]);
      }
    }
    return apply_pair_symbol(p.name(),(ATermAppl)tree_store[0]);
  }

  /* class pbes2bool_rewriter: public mcrl2::data::rewriter
  { // This class is introduced to get access the the 
    // rewriter made by Muck internally in the rewriter shell made by
    // Wieger.
    public:
      mcrl2::data::detail::Rewriter *get_internal_rewriter() 
      { return (mcrl2::data::detail::Rewriter *)(&m_rewriter);
      }

      pbes2bool_rewriter(mcrl2::data::rewriter &r)
      { m_rewriter=r.m_rewriter;
      } 
  }; */

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
      { return v;
      }

      void set_variable(variable_type w)
      { v=w;
      }

      reason get_reason(void)
      { return r;
      }

      void set_reason(reason s)
      { r=s;
      }

      std::string print_reason(void)
      { switch (r)
        { case UNKNOWN:                    return "Unknown     ";
          case MU_CYCLE:                   return "Mu Cycle    ";
          case NU_CYCLE:                   return "Nu Cycle    ";
          case SET_TO_FALSE:               return "Set:false   ";
          case SET_TO_TRUE:                return "Set:true    ";
          case FORWARD_SUBSTITUTION_FALSE: return "FSubst:false";
          case FORWARD_SUBSTITUTION_TRUE:  return "FSubst:true ";
          case SUBSTITUTION_FALSE:         return "Subst:false ";
          case SUBSTITUTION_TRUE:          return "Subst:true  ";
          case APPROXIMATION_FALSE:        return "Appr:false  ";
          case APPROXIMATION_TRUE:         return "Appr:true   ";
          case APPROXIMATION:              return "Approxim    ";
          default: return "ERROR UNKNOWN CASE";
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

      bes_expression(atermpp::aterm_appl term)
          : atermpp::aterm(term)
      {}

      // bes_expression(atermpp::term_appl<atermpp::aterm> &term)
      //    : atermpp::aterm_appl(term)
      // {}

      // allow assignment from aterms
      bes_expression& operator=(const atermpp::aterm &t)
      {
        m_term = t;
        return *this;
      }

      atermpp::aterm aterm() const
      { return m_term;
      }
  };

  inline
  void use_hashtables(void)
  { bes_global_variables<int>::opt_use_hashtables=true;
  }

  inline AFun initAFunBESAnd(AFun& f)
  { f = ATmakeAFun("BESAnd", 2, ATfalse);
    ATprotectAFun(f);
    return f;
  }

  inline AFun AFunBESAnd()
  { static AFun BESAnd = initAFunBESAnd(BESAnd);
    return BESAnd;
  }

  inline AFun initAFunBESOr(AFun& f)
  { f = ATmakeAFun("BESOr", 2, ATfalse);
    ATprotectAFun(f);
    return f;
  }

  inline AFun AFunBESOr()
  { static AFun BESOr = initAFunBESOr(BESOr);
    return BESOr;
  }

  inline AFun initAFunBESIf(AFun& f)
  { f = ATmakeAFun("BESIf", 3, ATfalse);
    ATprotectAFun(f);
    return f;
  }

  inline AFun AFunBESIf()
  { static AFun BESIf = initAFunBESIf(BESIf);
    return BESIf;
  }

  // BESFalse
  inline
  AFun initAFunBESFalse(AFun& f)
  {
    f = ATmakeAFun("BESFalse", 0, ATfalse);
    ATprotectAFun(f);
    return f;
  }

  inline
  AFun gsAFunBESFalse()
  {
    static AFun AFunBESFalse = initAFunBESFalse(AFunBESFalse);
    return AFunBESFalse;
  }

  inline
  bool gsIsBESFalse(ATermAppl Term)
  {
    return ATgetAFun(Term) == gsAFunBESFalse();
  }

  // BESTrue
  inline
  AFun initAFunBESTrue(AFun& f)
  {
    f = ATmakeAFun("BESTrue", 0, ATfalse);
    ATprotectAFun(f);
    return f;
  }

  inline
  AFun gsAFunBESTrue()
  {
    static AFun AFunBESTrue = initAFunBESTrue(AFunBESTrue);
    return AFunBESTrue;
  }

  inline
  bool gsIsBESTrue(ATermAppl Term)
  {
    return ATgetAFun(Term) == gsAFunBESTrue();
  }

  // BESDummy
  inline
  AFun initAFunBESDummy(AFun& f)
  {
    f = ATmakeAFun("BESDummy", 0, ATfalse);
    ATprotectAFun(f);
    return f;
  }

  inline
  AFun gsAFunBESDummy()
  {
    static AFun AFunBESDummy = initAFunBESDummy(AFunBESDummy);
    return AFunBESDummy;
  }

  inline
  bool gsIsBESDummy(ATermAppl Term)
  {
    return ATgetAFun(Term) == gsAFunBESDummy();
  }

  inline
  ATermAppl gsMakeBESFalse()
  {
    return ATmakeAppl0(gsAFunBESFalse());
  }

  inline
  ATermAppl gsMakeBESTrue()
  {
    return ATmakeAppl0(gsAFunBESTrue());
  }

  inline
  ATermAppl gsMakeBESDummy()
  {
    return ATmakeAppl0(gsAFunBESDummy());
  }

  /// \brief Returns the expression true
  inline
  bes_expression true_()
  {
    return bes_expression(gsMakeBESTrue());
  }

  /// \brief Returns the expression false
  inline
  bes_expression false_()
  {
    return bes_expression(gsMakeBESFalse());
  }

  /// \brief Returns the expression dummy (???)
  inline
  bes_expression dummy()
  {
    return bes_expression(gsMakeBESDummy());
  }

  inline bes_expression and_(bes_expression b1,bes_expression b2)
  { return bes_expression(
               ATmakeAppl2(AFunBESAnd(),
                           (atermpp::aterm)(b1),
                           (atermpp::aterm)(b2)));
  }

  inline bes_expression and_optimized(bes_expression b1,bes_expression b2)
  { if (b1==true_())
    { return b2;
    }
    if (b2==true_())
    { return b1;
    }
    if (b1==false_())
    { return false_();
    }
    if (b2==false_())
    { return false_();
    }
    if (b1==b2)
    { return b1;
    }
    return and_(b1,b2);
  }

  inline bes_expression or_(bes_expression b1,bes_expression b2)
  { return bes_expression(
               ATmakeAppl2(AFunBESOr(),
                           (atermpp::aterm)(b1),
                           (atermpp::aterm)(b2)));
  }

  inline bes_expression or_optimized(bes_expression b1,bes_expression b2)
  { if (b1==true_())
    { return true_();
    }
    if (b2==true_())
    { return true_();
    }
    if (b1==false_())
    { return b2;
    }
    if (b2==false_())
    { return b1;
    }
    if (b1==b2)
    { return b1;
    }
    return or_(b1,b2);
  }


  inline bool is_variable(bes_expression b)
  { return b.type()==AT_INT;
  }

  inline bes_expression if_(bes_expression b1,bes_expression b2,bes_expression b3)
  {
    return bes_expression(
               ATmakeAppl3(AFunBESIf(),
                           (atermpp::aterm)(b1),
                           (atermpp::aterm)(b2),
                           (atermpp::aterm)(b3)));
  }

  inline bes_expression ifAUX_(bes_expression b1,bes_expression b2,bes_expression b3)
  { if (b2==b3)
    { return b2;
    }
    return if_(b1,b2,b3);
  }

  inline bes_expression variable(const variable_type &n)
  { return bes_expression((atermpp::aterm)atermpp::aterm_int(n));
  }

  inline bool is_false(const bes_expression &b)
  {
    return b==false_();
  }

  inline bool is_true(const bes_expression &b)
  { return b==true_();
  }

  inline bool is_dummy(const bes_expression &b)
  { return b==dummy();
  }

  inline bool is_and(const bes_expression &b)
  { return ATgetAFun((_ATerm*)b)==AFunBESAnd();
  }

  inline bool is_or(const bes_expression &b)
  { return ATgetAFun((_ATerm*)b)==AFunBESOr();
  }

  inline bool is_if(const bes_expression &b)
  { return ATgetAFun((_ATerm*)b)==AFunBESIf();
  }

  inline bes_expression lhs(const bes_expression &b)
  { assert(is_and(b) || is_or(b));
    return bes_expression(atermpp::aterm_appl((const atermpp::aterm&)b)(0));
  }

  inline bes_expression rhs(const bes_expression &b)
  { assert(is_and(b) || is_or(b));
    return bes_expression(atermpp::aterm_appl((const atermpp::aterm&)b)(1));
  }

  inline bes_expression condition(const bes_expression &b)
  {
    assert(is_if(b));
    return bes_expression(atermpp::aterm_appl((const atermpp::aterm&)b)(0));
  }

  inline bes_expression then_branch(const bes_expression &b)
  { assert(is_if(b));
    return bes_expression(atermpp::aterm_appl((const atermpp::aterm&)b)(1));
  }

  inline bes_expression else_branch(const bes_expression &b)
  {
    assert(is_if(b));
    return bes_expression(atermpp::aterm_appl((const atermpp::aterm&)b)(2));
  }

  inline variable_type get_variable(const bes_expression &b)
  { assert(is_variable(b));
    return ((atermpp::aterm_int)(const atermpp::aterm&)b).value();
  }

  inline bes_expression substitute_true_false_rec(
                      bes_expression b,
                      const variable_type v,
                      const bes_expression b_subst,
                      atermpp::table &hashtable,
                      std::deque < counter_example > &counter_example_queue=bes_global_variables<int>::COUNTER_EXAMPLE_NULL_QUEUE)
  { assert(is_true(b_subst)||is_false(b_subst));

    if (is_true(b)||is_false(b)||is_dummy(b))
    { return b;
    }

    bes_expression result;

    if (bes_global_variables<int>::opt_use_hashtables)
    { result=hashtable.get(b);
      if (result!=NULL)
      { return result;
      }
    }

    if (is_if(b))
    { if (v==get_variable(condition(b)))
      {
        if (is_true(b_subst))
        { result=then_branch(b);
          if (&counter_example_queue!=&bes_global_variables<int>::COUNTER_EXAMPLE_NULL_QUEUE)
          { counter_example_queue.push_front(counter_example(v,SUBSTITUTION_TRUE));
          }
        }
        else
        { assert(is_false(b_subst));
          if (&counter_example_queue!=&bes_global_variables<int>::COUNTER_EXAMPLE_NULL_QUEUE)
          { counter_example_queue.push_front(counter_example(v,SUBSTITUTION_FALSE));
          }
          result=else_branch(b);
        }
      }
      else
      { result=ifAUX_(condition(b),
                     substitute_true_false_rec(then_branch(b),v,b_subst,hashtable,counter_example_queue),
                     substitute_true_false_rec(else_branch(b),v,b_subst,hashtable,counter_example_queue));
      }
    }
    else if (is_variable(b))
    { if (v==get_variable(b))
      { result=b_subst;
        if (is_true(b_subst))
        {
          if (&counter_example_queue!=&bes_global_variables<int>::COUNTER_EXAMPLE_NULL_QUEUE)
          { counter_example_queue.push_front(counter_example(v,SUBSTITUTION_TRUE));
          }
        }
        else
        { assert(is_false(b_subst));
          if (&counter_example_queue!=&bes_global_variables<int>::COUNTER_EXAMPLE_NULL_QUEUE)
          { counter_example_queue.push_front(counter_example(v,SUBSTITUTION_FALSE));
          }
        }
      }
      else
      { result=b;
      }
    }
    else if (is_and(b))
    {
      bes_expression b1=substitute_true_false_rec(lhs(b),v,b_subst,hashtable,counter_example_queue);
      if (is_false(b1))
      { result=false_();
      }
      else
      { bes_expression b2=substitute_true_false_rec(rhs(b),v,b_subst,hashtable,counter_example_queue);
        if (is_false(b2))
        { result=false_();
        }
        else if (is_true(b1))
        { result=b2;
        }
        else if (is_true(b2))
        { result=b1;
        }
        else
        { result=and_(b1,b2);
        }
      }
    }
    else if (is_or(b))
    {
      bes_expression b1=substitute_true_false_rec(lhs(b),v,b_subst,hashtable,counter_example_queue);
      if (is_true(b1))
      { result=true_();
      }
      else
      { bes_expression b2=substitute_true_false_rec(rhs(b),v,b_subst,hashtable,counter_example_queue);
        if (is_true(b2))
        { result=true_();
        }
        else if (is_false(b1))
        { result=b2;
        }
        else if (is_false(b2))
        { result=b1;
        }
        else
        { result=or_(b1,b2);
        }
      }
    }

    if (bes_global_variables<int>::opt_use_hashtables)
    { hashtable.put(b,result);
    }
    return result;
  }

  inline
  bes_expression substitute_true_false(
                      bes_expression b,
                      const variable_type v,
                      const bes_expression b_subst,
                      std::deque < counter_example > &counter_example_queue=bes_global_variables<int>::COUNTER_EXAMPLE_NULL_QUEUE)
  { assert(is_true(b_subst)||is_false(b_subst));

    if (is_true(b)||is_false(b)||is_dummy(b))
    { return b;
    }

    static atermpp::table hashtable1(10,50);

    bes_expression result=substitute_true_false_rec(b,v,b_subst,hashtable1,counter_example_queue);

    if (bes_global_variables<int>::opt_use_hashtables)
    { hashtable1.reset();
    }
    return result;
  }

  inline
  bes_expression BDDif_rec(
                             bes_expression b1,
                             bes_expression b2,
                             bes_expression b3,
                             atermpp::table &hashtable);


  inline bes_expression BDDif(bes_expression b1, bes_expression b2, bes_expression b3)
  {

    static atermpp::table hashtable(100,75);
    static unsigned int hashtable_reset_counter=0;

    bes_expression b=BDDif_rec(b1,b2,b3,hashtable);
    hashtable_reset_counter++;
    if (hashtable_reset_counter==1000) // resetting of the hashtable is quite expensive.
    { hashtable.reset();
      hashtable_reset_counter=0;
    }
    return b;
  }

  inline bes_expression BDDif_rec(bes_expression b1, bes_expression b2, bes_expression b3,atermpp::table &hashtable)
  { /* Assume that b1, b2 and b3 are ordered BDDs. Return an
       ordered BDD */

    /* Check hash table */

    bes_expression b1b2b3;
    bes_expression result;

    if (is_true(b1))
    { return b2;
    }
    else if (is_false(b1))
    { return b3;
    }
    /* b1 has an if_then_else shape */
    else if (b2==b3)
    { return b2;
    }

    if (is_true(b2))
    { /* hence not is_true(b3) */
      if (is_false(b3))
      { return b1;
      }
      else
      {
        b1b2b3=if_(b1,b2,b3);
        bes_expression b(hashtable.get(b1b2b3));
        if (b!=NULL)
        { return b;
        }

        /* b3 contains an if then else expression, and b2=true */
        if (condition(b1)==condition(b3))
        { result=ifAUX_(condition(b1),
                        BDDif_rec(then_branch(b1),b2,then_branch(b3),hashtable),
                        BDDif_rec(else_branch(b1),b2,else_branch(b3),hashtable));
        }
        else if (get_variable(condition(b1))<get_variable(condition(b3)))
        { result=ifAUX_(condition(b1),
                        BDDif_rec(then_branch(b1),b2,b3,hashtable),
                        BDDif_rec(else_branch(b1),b2,b3,hashtable));
        }
        else
        { result=ifAUX_(condition(b3),
                        BDDif_rec(b1,b2,then_branch(b3),hashtable),
                        BDDif_rec(b1,b2,else_branch(b3),hashtable));
        }
      }
    }
    else if (is_false(b2))
    { /* hence not is_false(b3) */
      b1b2b3=if_(b1,b2,b3);
      bes_expression b(hashtable.get(b1b2b3));
      if (b!=NULL)
      { return b;
      }

      if (is_true(b3))
      { result=ifAUX_(condition(b1),
                     BDDif_rec(then_branch(b1),b2,b3,hashtable),
                     BDDif_rec(else_branch(b1),b2,b3,hashtable));
      }
      else
      { /* b3 contains an if then else expression, and b2=true */
        if (condition(b1)==condition(b3))
        { result=ifAUX_(condition(b1),
                        BDDif_rec(then_branch(b1),b2,then_branch(b3),hashtable),
                        BDDif_rec(else_branch(b1),b2,else_branch(b3),hashtable));
        }
        else if (get_variable(condition(b1))<get_variable(condition(b3)))
        { result=ifAUX_(condition(b1),
                        BDDif_rec(then_branch(b1),b2,b3,hashtable),
                        BDDif_rec(else_branch(b1),b2,b3,hashtable));
        }
        else
        { result=ifAUX_(condition(b3),
                        BDDif_rec(b1,b2,then_branch(b3),hashtable),
                        BDDif_rec(b1,b2,else_branch(b3),hashtable));
        }
      }
    }
    else if (is_true(b3)||is_false(b3))
    {
      if (condition(b1)==condition(b2))
      { result=ifAUX_(condition(b1),
                      BDDif_rec(then_branch(b1),then_branch(b2),b3,hashtable),
                      BDDif_rec(else_branch(b1),else_branch(b2),b3,hashtable));
      }
      else if (get_variable(condition(b1))<get_variable(condition(b2)))
      { result=ifAUX_(condition(b1),
                      BDDif_rec(then_branch(b1),b2,b3,hashtable),
                      BDDif_rec(else_branch(b1),b2,b3,hashtable));
      }
      else
      { result=ifAUX_(condition(b2),
                      BDDif_rec(b1,then_branch(b2),b3,hashtable),
                      BDDif_rec(b1,else_branch(b2),b3,hashtable));
      }
    }
    else
    { /* None of b1, b2 and b3 is true or false, all have the if then else shape */
      if (condition(b1)==condition(b2))
      {
        if (condition(b2)==condition(b3))
        { /* all conditions are equal */
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

    hashtable.put(b1b2b3,result);
    return result;
  }

  static bes_expression toBDD_rec(bes_expression b1,atermpp::table &hashtable)
  {
    if ((b1==true_()) || (b1==false_()))
    {return b1;
    }

    bes_expression result;
    bes_expression b(hashtable.get(b1));
    if (b!=NULL)
    { return b;
    }

    if (is_variable(b1))
    { result=if_(b1,true_(),false_());
    }
    else if (is_and(b1))
    { result=BDDif_rec(toBDD_rec(lhs(b1),hashtable),toBDD_rec(rhs(b1),hashtable),false_(),hashtable);
    }
    else if (is_or(b1))
    { result=BDDif_rec(toBDD_rec(lhs(b1),hashtable),true_(),toBDD_rec(rhs(b1),hashtable),hashtable);
    }
    else if (is_if(b1))
    { result=BDDif_rec(toBDD_rec(condition(b1),hashtable),
                       toBDD_rec(then_branch(b1),hashtable),
                       toBDD_rec(else_branch(b1),hashtable),
                       hashtable);
    }
    else
    { std::cerr << "Unexpected expression\n";
      assert(0);
    }

    hashtable.put(b1,result);
    return result;
  }


  inline bes_expression toBDD(bes_expression b)
  {
    static atermpp::table hashtable(100,75);
    return toBDD_rec(b,hashtable);
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

      std::vector<unsigned long> control_info;
      atermpp::vector<bes_expression> right_hand_sides;
      bool variable_occurrences_are_stored;
      std::vector< std::set <variable_type> > variable_occurrence_sets;
      atermpp::indexed_set variable_relevance_indexed_set;
      bool count_variable_relevance;
      std::vector < std::deque < counter_example> > data_to_construct_counter_example;
      bool construct_counter_example;
      atermpp::indexed_set variable_index;  //Used for constructing counter examples
      mcrl2::pbes_system::detail::legacy_pbes_data_rewriter Mucks_rewriter;
      const bool opt_precompile_pbes;
      const bool internal_opt_store_as_tree;

    protected:
      inline void check_vector_sizes(const variable_type v)
      { if (v>nr_of_variables())
        { control_info.resize(v+1,0);
          right_hand_sides.resize(v+1,dummy());
          if (variable_occurrences_are_stored)
          { variable_occurrence_sets.resize(v+1,std::set<variable_type>());
          }
         if (construct_counter_example)
          { data_to_construct_counter_example.resize(v+1,std::deque<counter_example>());
          }
        }
      }

    public:
      unsigned long max_rank;

      inline unsigned long nr_of_variables() const
      { return control_info.size()-1; /* there is no equation at position 0 */
      }

      inline mcrl2::pbes_system::fixpoint_symbol get_fixpoint_symbol(variable_type v)
      {
        assert(v>0); /* variable indices start at 1. 0 is used as an indicator of error */
        assert(v<=nr_of_variables());
        assert(control_info[v]>0);

        return (((control_info[v] & FIXPOINT_MASK) ==0) ? mcrl2::pbes_system::fixpoint_symbol::nu() : mcrl2::pbes_system::fixpoint_symbol::mu());
      }


      inline unsigned long get_rank(variable_type v)
      {
        assert(v>0);
        check_vector_sizes(v);

        return (control_info[v] >> RANK_SHIFT)-1;
      }

      void add_equation(variable_type v,
                        mcrl2::pbes_system::fixpoint_symbol sigma,
                        unsigned long rank,
                        bes_expression rhs,
                        std::deque <variable_type> &todo=bes_global_variables<int>::TODO_NULL_QUEUE)
      { assert(rank>0);  // rank must be positive.
        assert(v>0);     // variables are represented by numbers >0.

        check_vector_sizes(v);
        // the vector at position v is now guaranteed to exist.

        // if the control info is 0, the value at variable
        // is not initialized.

        control_info[v]=1+(((sigma.is_nu()?0:1)+((rank+1)<<1))<<1);
        right_hand_sides[v]=rhs;
        if (rank>max_rank)
        { max_rank=rank;
        }
        if (variable_occurrences_are_stored)
        { add_variables_to_occurrence_sets(v,rhs);
        }
        if (count_variable_relevance)
        { set_variable_relevance_rec(rhs,todo);
        }
      }

      inline void set_rhs(variable_type v,
                          bes_expression b,
                          variable_type v_except=0,
                          std::deque <variable_type> &todo=bes_global_variables<int>::TODO_NULL_QUEUE)
      { /* set the right hand side of v to b. Update the variable occurrences
           of v in the variables occurrence sets of variables occurring in b, but
           do not update the variable occurrence sets of v_except */
        assert(v>0);
        assert(v<=nr_of_variables());

        control_info[v]=control_info[v]|RELEVANCE_MASK;  // make this variable relevant.
        bes_expression old_rhs=right_hand_sides[v];
        if ((variable_occurrences_are_stored)&& (old_rhs!=b))
        { remove_variables_from_occurrence_sets(v,old_rhs,v_except);
          add_variables_to_occurrence_sets(v,b);
          right_hand_sides[v]=b;
        }
        if (count_variable_relevance)
        { set_variable_relevance_rec(b,todo);
        }

      }

      inline bes_expression get_rhs(variable_type v)
      {
        if (v>nr_of_variables())
        {
          // fprintf(stderr,"ACCESSING A VARIABLE THAT DOES NOT EXIST\n");
          return dummy();
        }
        else
        {
          return right_hand_sides[v];
        }
      }

      void clear_variable_occurrence_set(variable_type v)
      { assert(variable_occurrences_are_stored);
        check_vector_sizes(v);
        variable_occurrence_sets[v].clear();
      }

      std::set< variable_type >::iterator
            variable_occurrence_set_begin(variable_type v)
      { assert(variable_occurrences_are_stored);
        assert(v>0);
        check_vector_sizes(v);
        return variable_occurrence_sets[v].begin();
      }

      std::set< variable_type >::iterator
            variable_occurrence_set_end(variable_type v)
      { assert(variable_occurrences_are_stored);
        assert(v>0);
        check_vector_sizes(v);
        return variable_occurrence_sets[v].end();
      }

      void add_variables_to_occurrence_sets(
                      variable_type v,
                      bes_expression b,
                      const bool use_indexed_set,
                      atermpp::indexed_set &indexed_set)
      {
        assert(v>0);
        assert(variable_occurrences_are_stored);

        if (is_true(b)||is_false(b)||is_dummy(b))
        { return;
        }

        if (use_indexed_set)
        { if (!(indexed_set.put(b)).second)  // b is already in the set.
          return;
        }
        if (is_if(b))
        { assert(get_variable(condition(b))>0);
          variable_type w=get_variable(condition(b));
          check_vector_sizes(w);
          variable_occurrence_sets[w].insert(v);

          add_variables_to_occurrence_sets(v,then_branch(b),use_indexed_set,indexed_set);
          add_variables_to_occurrence_sets(v,else_branch(b),use_indexed_set,indexed_set);
          return;
        }

        if (is_variable(b))
        { variable_type w=get_variable(b);
          check_vector_sizes(w);
          variable_occurrence_sets[w].insert(v);
          return;
        }

        if (is_and(b)||is_or(b))
        {
          add_variables_to_occurrence_sets(v,lhs(b),use_indexed_set,indexed_set);
          add_variables_to_occurrence_sets(v,rhs(b),use_indexed_set,indexed_set);
          return;
        }
        assert(0); // do not expect other term formats.

      }

      void add_variables_to_occurrence_sets(
                      variable_type v,
                      bes_expression b)
      { static atermpp::indexed_set indexed_set1(10,50);

        add_variables_to_occurrence_sets(v,b,true,indexed_set1);
        // add_variables_to_occurrence_sets(v,b,bes_global_variables<int>::opt_use_hashtables,indexed_set1);
        // if (bes_global_variables<int>::opt_use_hashtables)
        { indexed_set1.reset();
        }
      }

      void remove_variables_from_occurrence_sets(
                     const variable_type v,
                     bes_expression b,
                     const variable_type v_except,
                     const bool use_indexed_set,
                     atermpp::indexed_set &indexed_set)
      {
        assert(v>0);
        assert(variable_occurrences_are_stored);
        check_vector_sizes(v);
        if (is_true(b)||is_false(b)||is_dummy(b))
        { return;
        }

        if (use_indexed_set)
        { if (!(indexed_set.put(b)).second)  // b is already in the set.
          return;
        }

        if (is_if(b))
        { variable_type w=get_variable(condition(b));
          assert(w<=nr_of_variables());
          if (w!=v_except)
          { variable_occurrence_sets[w].erase(v);
          }
          // Using hash tables this can be made more efficient, by employing
          // sharing of the atermpp::aterm representing b.
          remove_variables_from_occurrence_sets(v,then_branch(b),v_except,use_indexed_set,indexed_set);
          remove_variables_from_occurrence_sets(v,else_branch(b),v_except,use_indexed_set,indexed_set);
          return;
        }
        else if (is_variable(b))
        { variable_type w=get_variable(b);
          assert(w<=nr_of_variables());
          if (w!=v_except)
          { variable_occurrence_sets[w].erase(v);
          }
          return;
        }
        else if (is_or(b)||is_and(b))
        {
          remove_variables_from_occurrence_sets(v,lhs(b),v_except,use_indexed_set,indexed_set);
          remove_variables_from_occurrence_sets(v,rhs(b),v_except,use_indexed_set,indexed_set);
          return;
        }
      }

      void remove_variables_from_occurrence_sets(
                     const variable_type v,
                     bes_expression b,
                     const variable_type v_except)
      {
        static atermpp::indexed_set indexed_set2(10,50);

        remove_variables_from_occurrence_sets(v,b,v_except,true,indexed_set2);
        // remove_variables_from_occurrence_sets(v,b,v_except,bes_global_variables<int>::opt_use_hashtables,indexed_set2);
        // if (bes_global_variables<int>::opt_use_hashtables)
        { indexed_set2.reset();
        }
      }


      void store_variable_occurrences(void)
      {
        if (variable_occurrences_are_stored)
        { return;
        }
        assert(variable_occurrence_sets.size()==1);
        variable_occurrences_are_stored=true;
        variable_occurrence_sets.resize(nr_of_variables()+1,std::set<variable_type>());
        for(variable_type v=nr_of_variables(); v>0; v--)
        {
          bes_expression b=get_rhs(v);
          if (b!=dummy())
          { add_variables_to_occurrence_sets(v,get_rhs(v));
          }
        }
      }


      void reset_variable_relevance(void)
      {
        for(std::vector <unsigned long>::iterator v=control_info.begin() ;
                 v!=control_info.end() ;
                 v++)
        { *v =(*v)& ~RELEVANCE_MASK;  // reset the relevance bit to 0
        }
      }


      void set_variable_relevance_rec(
                    bes_expression b,
                    std::deque <variable_type> &todo=bes_global_variables<int>::TODO_NULL_QUEUE)
      {
        assert(count_variable_relevance);
        if (is_true(b)||is_false(b)||is_dummy(b))
        { return;
        }

        if (bes_global_variables<int>::opt_use_hashtables)
        { if (!(variable_relevance_indexed_set.put(b)).second)
          { /* The relevance for the variables in this term has already been set */
            return;
          }
        }

        if (is_variable(b))
        { variable_type v=get_variable(b);
          assert(v>0);
          check_vector_sizes(v);
          if (!is_relevant(v))
          { control_info[v]=control_info[v]|RELEVANCE_MASK;  // Make relevant
            if (get_rhs(v)==dummy()) // v is relevant an unprocessed. Put in on the todo stack.
            {
              if (&todo!=&bes_global_variables<int>::TODO_NULL_QUEUE)
              {
                todo.push_back(v);
              }
              return;
            }
            else
            { set_variable_relevance_rec(get_rhs(v),todo);
              return;
            }
          }
          return;
        }

        if (is_if(b))
        { set_variable_relevance_rec(condition(b),todo);
          set_variable_relevance_rec(then_branch(b),todo);
          set_variable_relevance_rec(else_branch(b),todo);
          return;
        }

        if (is_and(b)||is_or(b))
        { set_variable_relevance_rec(lhs(b),todo);
          set_variable_relevance_rec(rhs(b),todo);
          return;
        }

        assert(0); // do not expect other term formats.
      }

      void refresh_relevances(std::deque <variable_type> &todo=bes_global_variables<int>::TODO_NULL_QUEUE)
      { if (count_variable_relevance)
        { reset_variable_relevance();
          if (bes_global_variables<int>::opt_use_hashtables)
          { variable_relevance_indexed_set.reset();
          }
          if (&todo!=&bes_global_variables<int>::TODO_NULL_QUEUE)
          { todo.clear();
          }
          set_variable_relevance_rec(variable(1),bes_global_variables<int>::TODO_NULL_QUEUE);
          if (&todo!=&bes_global_variables<int>::TODO_NULL_QUEUE)
          { // We add the variables to the todo queue separately,
            // to guarantee that lower numbered variables occur earlier in
            // the queue. This guarantees shorter counter examples.
            for(unsigned long v=1; v<=nr_of_variables(); v++)
            { if ((get_rhs(v)==dummy()) && is_relevant(v))
              { todo.push_back(v);
              }
            }
          }
        }
      }

      void count_variable_relevance_on(void)
      {
        assert(!count_variable_relevance);
        count_variable_relevance=true;
        refresh_relevances();
      }

      bool is_relevant(const variable_type v)
      {
        assert(0<v);
        check_vector_sizes(v);
        /* all variables are relevant if relevancy is not maintained */
        return (!count_variable_relevance || (control_info[v] & RELEVANCE_MASK));
      }

      void construct_counter_example_on(void)
      {
        assert(!construct_counter_example);
        construct_counter_example=true;
        data_to_construct_counter_example.resize(nr_of_variables()+1,std::deque<counter_example>());
      }

      std::deque <counter_example>
                   &counter_example_queue(variable_type v)
      { assert(construct_counter_example);
        check_vector_sizes(v);
        return data_to_construct_counter_example[v];
      }

      std::deque<counter_example>::iterator dummy_deque_iterator;

      std::deque<counter_example>::iterator counter_example_begin(variable_type v)
      { assert(construct_counter_example);
        if (v>nr_of_variables())
        { return dummy_deque_iterator;
        }
        return data_to_construct_counter_example[v].begin();
      }

      std::deque<counter_example>::iterator counter_example_end(variable_type v)
      { assert(construct_counter_example);
        if (v>nr_of_variables())
        { return dummy_deque_iterator;
        }
        return data_to_construct_counter_example[v].end();
      }

      // The following function indicates whether there is a mu/nu loop.
      bool find_mu_nu_loop_rec(
             bes_expression b,
             variable_type v,
             unsigned long rankv, // rank of v may not have been stored yet.
             std::map < variable_type,bool > &visited_variables,
             bool is_mu)
      {
        if (is_false(b) || is_true(b) || is_dummy(b))
        {
          return false;
        }

        if (is_variable(b))
        { variable_type w=get_variable(b);
          if (w==v)
          { 
            return true;
          }
          if (get_rank(w)!=rankv)
          {
            return false;
          }
          if (visited_variables.find(w)!=visited_variables.end())                                            
          { return visited_variables[w];
          }

          visited_variables.insert(std::make_pair(w,false));
          bool result;
          result=find_mu_nu_loop_rec(get_rhs(w),v,rankv,visited_variables,is_mu);
          visited_variables.insert(std::make_pair(w,result));
          return result;
        }

        if (is_mu)
        { if (is_and(b))
          { return find_mu_nu_loop_rec(lhs(b),v,rankv,visited_variables,is_mu) ||
                        find_mu_nu_loop_rec(rhs(b),v,rankv,visited_variables,is_mu);
          }

          if (is_or(b))
          { return find_mu_nu_loop_rec(lhs(b),v,rankv,visited_variables,is_mu) &&
                          find_mu_nu_loop_rec(rhs(b),v,rankv,visited_variables,is_mu);
          }
        }
        else
        { if (is_and(b))
          { return find_mu_nu_loop_rec(lhs(b),v,rankv,visited_variables,is_mu) &&
                          find_mu_nu_loop_rec(rhs(b),v,rankv,visited_variables,is_mu);
          }

          if (is_or(b))
          { return find_mu_nu_loop_rec(lhs(b),v,rankv,visited_variables,is_mu) ||
                          find_mu_nu_loop_rec(rhs(b),v,rankv,visited_variables,is_mu);
          }
        }

        if (is_if(b))
        {
          bool r=find_mu_nu_loop_rec(condition(b),v,rankv,visited_variables,is_mu);
          if (r)
          { if (is_mu)
            { return find_mu_nu_loop_rec(else_branch(b),v,rankv,visited_variables,is_mu);
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
             bes_expression b,
             variable_type v,
             unsigned long rankv)
      { std::map < variable_type, bool > visited_variables;
        bool result=find_mu_nu_loop_rec(b,v,rankv,visited_variables,true);
        return result;
      }

      bool find_nu_loop(
             bes_expression b,
             variable_type v,
             unsigned long rankv)
      { std::map < variable_type, bool > visited_variables;
        return find_mu_nu_loop_rec(b,v,rankv,visited_variables,false);
      }

////////////////////////////  Functions to generate a bes out of a pbes  /////////////////////////


      ///\brief returns a pbes expression which has been rewritten into internal rewrite format.
      ///\details This function is used to translate a term into internal rewrite format, which is
      /// required to get good performance out of the rewriter by Muck van Weerdenburg. This code
      /// should be removed once the new rewriters are getting enough performance. Currently, however,
      /// the performance of the rewriter with the nice interface is so bad that it cannot be used
      /// for actual verification.
      ///
      /// This function simplifies all data expressions in p by applying the rewriter to it.
      /// Data expressions that are true or false are translated to the pbes expressions true and false.
      /// Quantified variables that do not occur in the body are removed.
      /// Conjunctions and disjunctions of which one of the arguments is true or false are simplified.
      /// If the opt_precompile_pbes is set, the data expressions in the resulting
      /// pbes expressions are translated into the internal format belonging to the rewriter. The
      /// advantage of this is that the rewriter does not have to translate the data expressions
      /// to internal format the next time the rewriter is applied to it. This is for instance useful
      /// in the tool pbes2bool (or pbes2bes) where pbes expressions must iteratively be rewritten.
      
      inline mcrl2::pbes_system::pbes_expression pbes_expression_rewrite_and_simplify(
                         mcrl2::pbes_system::pbes_expression p)
      {
        using namespace mcrl2;
        using namespace mcrl2::pbes_system;
        using namespace mcrl2::pbes_system::pbes_expr;
        using namespace mcrl2::pbes_system::accessors;

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
          pbes_expression l = pbes_expression_rewrite_and_simplify(left(p));
          if (is_pbes_false(l))
          { result = pbes_expr::false_();
          }
          else
          { pbes_expression rt = pbes_expression_rewrite_and_simplify(right(p));
            //Options for left and right
            if (is_pbes_false(rt))
            { result = pbes_expr::false_();
            }
            else if (is_pbes_true(l))
            { result = rt;
            }
            else if (is_pbes_true(rt))
            { result = l;
            }
            else result = pbes_expr::and_(l,rt);
          }
        }
        else if (is_pbes_or(p))
        { // p = or(left, right)
          //Rewrite left and right as far as possible
          pbes_expression l = pbes_expression_rewrite_and_simplify(left(p));
          if (is_pbes_true(l))
          { result = pbes_expr::true_();
          }
          else
          { pbes_expression rt = pbes_expression_rewrite_and_simplify(right(p));
            if (is_pbes_true(rt))
            { result = pbes_expr::true_();
            }
            else if (is_pbes_false(l))
            { result = rt;
            }
            else if (is_pbes_false(rt))
            { result = l;
            }
            else result = pbes_expr::or_(l,rt);
          }
        }
        else if (is_pbes_forall(p))
        { // p = forall(data::data_expression_list, pbes_expression)
          data::variable_list data_vars = var(p);
          pbes_expression expr = pbes_expression_rewrite_and_simplify(arg(p));
          //Remove data_vars which do not occur in expr
          data::variable_list occurred_data_vars;
          for (data::variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
          {
            if (detail::occurs_in_varL(expr, *i)) // The var occurs in expr
            {
              occurred_data_vars = push_front(occurred_data_vars, *i);
            }
          }
      
          // If no data_vars
          if (occurred_data_vars.empty())
          { result = expr;
          }
          else
          { result=pbes_expr::forall(occurred_data_vars,expr);
          }
        }
        else if (is_pbes_exists(p))
        { // p = exists(data::data_expression_list, pbes_expression)
          data::variable_list data_vars = var(p);
          pbes_expression expr = pbes_expression_rewrite_and_simplify(arg(p));
          //Remove data_vars which does not occur in expr
          data::variable_list occurred_data_vars;
          for (data::variable_list::iterator i = data_vars.begin(); i != data_vars.end(); i++)
          {
            if (detail::occurs_in_varL(expr, *i)) // The var occurs in expr
            {
               occurred_data_vars = atermpp::push_front(occurred_data_vars, *i);
            }
          }
      
          //If no data_vars remaining
          if (occurred_data_vars.empty())
            result = expr;
          else
          { result=pbes_expr::exists(occurred_data_vars,expr);
          }
        }
        else if (is_propositional_variable_instantiation(p))
        { // p is a propositional variable
          propositional_variable_instantiation propvar = p;
          core::identifier_string name = propvar.name();
          data::data_expression_list parameters;
          if (opt_precompile_pbes)
          { 
            data::data_expression_list current_parameters(propvar.parameters());
            for( data::data_expression_list::const_iterator l=current_parameters.begin();
                 l != current_parameters.end(); ++l)
            {
              parameters = atermpp::push_front(parameters, data::data_expression(Mucks_rewriter.rewriteInternal(
                                       Mucks_rewriter.translate(*l))));  
            }
            parameters = atermpp::reverse(parameters); 
          }
          else
          { parameters=Mucks_rewriter.rewriteList(propvar.parameters());
          }
          result = pbes_expression(propositional_variable_instantiation(name, parameters));
        }
        else
        { // p is a data::data_expression
      
          if (opt_precompile_pbes)
          {
            data::data_expression d = (data::data_expression)Mucks_rewriter.rewriteInternal(Mucks_rewriter.translate(p));
            if (detail::is_true_in_internal_rewrite_format(d,Mucks_rewriter))
            { result = pbes_expr::true_();
            }
            else if (detail::is_false_in_internal_rewrite_format(d,Mucks_rewriter))
            { result = pbes_expr::false_();
            }
            else
            { result = d;
            } 
          }
          else
          {
            data::data_expression d(Mucks_rewriter.rewrite(p));
            if (d == data::sort_bool::true_())
            { result = pbes_expr::true_();
            }
            else if (d == data::sort_bool::false_())
            { result = pbes_expr::false_();
            }
            else
            { result = d;
            }
          }
        }
      
        return result;
      }


    //function add_propositional_variable_instantiations_to_indexed_set
    //and translate to pbes expression to a bes_expression in BDD format.
    bes_expression add_propositional_variable_instantiations_to_indexed_set_and_translate(
                       const mcrl2::pbes_system::pbes_expression p,
                       atermpp::indexed_set &variable_index,
                       unsigned long &nr_of_generated_variables,
                       const bool to_bdd,
                       const transformation_strategy strategy,
                       const bool construct_counter_example,
                       const variable_type current_variable)
    { using namespace mcrl2::pbes_system;
      if (is_propositional_variable_instantiation(p))
      {
        std::pair<unsigned long,bool> pr=variable_index.put((internal_opt_store_as_tree)?
                               mcrl2::pbes_system::pbes_expression(store_as_tree(p)):p);

        if (pr.second) /* p is added to the indexed set, so it is a new variable */
        { nr_of_generated_variables++;
          if (to_bdd)
          { return if_(variable(pr.first),true_(),false_());
          }
          else
          { return variable(pr.first);
          }
        }
        else
        {
          if (strategy>lazy)
          { bes_expression b=get_rhs(pr.first);
            if (is_true(b) )
            {
              if (construct_counter_example)
              { counter_example_queue(current_variable).
                       push_front(counter_example(pr.first,FORWARD_SUBSTITUTION_TRUE));
              }
              return b;
            }
            if (is_false(b))
            {
              if (construct_counter_example)
              { counter_example_queue(current_variable).
                       push_front(counter_example(pr.first,FORWARD_SUBSTITUTION_FALSE));
              }
              return b;
            }
          }
          if (to_bdd)
          { return if_(variable(pr.first),true_(),false_());
          }
          else
          { return variable(pr.first);
          }
        }
      }
      else if (mcrl2::pbes_system::pbes_expr::is_and(p))
      { bes_expression b1=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                                accessors::left(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                                construct_counter_example,current_variable);
        if (is_false(b1))
        { return b1;
        }
        bes_expression b2=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                                accessors::right(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                                construct_counter_example,current_variable);
        if (is_false(b2))
        { return b2;
        }
        if (is_true(b1))
        { return b2;
        }
        if (is_true(b2))
        { return b1;
        }
        if (to_bdd)
        { return BDDif(b1,b2,false_());
        }
        else
        { return and_(b1,b2);
        }
      }
      else if (pbes_expr::is_or(p))
      {
        bes_expression b1=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                                accessors::left(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                                construct_counter_example,current_variable);
        if (is_true(b1))
        { return b1;
        }

        bes_expression b2=add_propositional_variable_instantiations_to_indexed_set_and_translate(
                                accessors::right(p),variable_index,nr_of_generated_variables,to_bdd,strategy,
                                construct_counter_example,current_variable);
        if (is_true(b2))
        { return b2;
        }
        if (is_false(b1))
        { return b2;
        }
        if (is_false(b2))
        { return b1;
        }

        if (to_bdd)
        { return BDDif(b1,true_(),b2);
        }
        else
        { return or_(b1,b2);
        }
      }
      else if (pbes_expr::is_true(p))
      { return true_();
      }
      else if (pbes_expr::is_false(p))
      { return false_();
      }
      
      if (opt_precompile_pbes)
      { 
        throw mcrl2::runtime_error("Unexpected expression. Most likely because expression fails to rewrite to true or false: " +
                     pp(Mucks_rewriter.fromRewriteFormat((ATerm)(ATermAppl)p)) + "\n");
      }
      else
      { 
        throw mcrl2::runtime_error("Unexpected expression. Most likely because expression fails to rewrite to true or false: " + 
                     pp(p) + "\n");
      }
      return false_();
    }

    /// \brief Calculate a BES from a given PBES
    /// \detail

    /// Global variables
    ///  atermpp::indexed_set variable_index(10000, 50);
    ///  bes_equations;
    // template < typename Container, typename PbesRewriter >
    template < typename Container >
    boolean_equation_system(
        const typename mcrl2::pbes_system::pbes<Container>& pbes_spec,
        mcrl2::data::rewriter &data_rewriter,
        const transformation_strategy opt_strategy=lazy,
        const bool opt_store_as_tree=false,
        const bool opt_construct_counter_example=false,
        const bool opt_use_hashtables=false):
          control_info(1),
          right_hand_sides(1),
          variable_occurrences_are_stored(false),
          variable_occurrence_sets(1),
          variable_relevance_indexed_set(10,50),
          count_variable_relevance(false),
          data_to_construct_counter_example(1),
          construct_counter_example(false),
          Mucks_rewriter(data_rewriter),
#ifdef NDEBUG  // Only in non-debug mode we want highest performance.
          opt_precompile_pbes(true),
          internal_opt_store_as_tree(opt_store_as_tree),
#else
          opt_precompile_pbes(false),
          internal_opt_store_as_tree(false),
#endif
          max_rank(0)
    { using namespace mcrl2::data;
      using namespace mcrl2::pbes_system;
      assert(pbes_spec.is_well_typed());
      assert(pbes_spec.is_closed());

      // Verbose msg: doing naive algorithm

      // Declare all constructors and mappings to the rewriter to prevent unnecessary compilation.
      // This can be removed if the jittyc or innerc compilers are not in use anymore.
      for(data_specification::constructors_const_range c=pbes_spec.data().constructors(); !c.empty() ; c.advance_begin(1))
      { Mucks_rewriter.translate(c.front());
      }
      
      for(data_specification::constructors_const_range c=pbes_spec.data().mappings(); !c.empty() ; c.advance_begin(1))
      { Mucks_rewriter.translate(c.front());
      }
      
      // Variables in which the result is stored
      propositional_variable_instantiation new_initial_state;

      // atermpp::indexed_set variable_index(10000, 50);
      // In order to generate a counterexample, this must also be known outside
      // this procedure.
      variable_index.put(true_());
      variable_index.put(false_()); /* Put first two dummy terms that
                                           gets index 0 and 1 in the indexed set, to
                                           take care that the first variable gets an index 2, to
                                           make space for a first equation of the shape X1=X2. */

      /* The following list contains that variables that need to be explored.
         This list is only relevant if opt_strategy>=on_the_fly,
         as in the other case the variables to be investigated are those
         with indices between nre_of_processed_variables and nr_of_generated
         variables. */
      std::deque < variable_type> todo;
      if (opt_strategy>=on_the_fly)
      { todo.push_front(2);
      }
      // Data rewriter
#ifndef NDEBUG  
      if (internal_opt_store_as_tree)
      { std::cerr << "Warning. Do not store pbes variables in a tree structure in a debug build of pbes2bool\n";
      }
#endif
      pbes_expression p=// pbes_rewriter(pbes_spec.initial_state());
                         pbes_expression_rewrite_and_simplify(
                                pbes_spec.initial_state());   
      variable_index.put((internal_opt_store_as_tree)?pbes_expression(store_as_tree(p)):p);

      if (opt_strategy>=on_the_fly)
      { store_variable_occurrences();
        count_variable_relevance_on();
      }

      if (opt_construct_counter_example)
      { construct_counter_example_on();
      }

      // Needed hashtables
      Container eqsys = pbes_spec.equations();
      atermpp::table pbes_equations(2*eqsys.size(), 50);   // (propvarname, pbes_equation)

      // Vector with the order of the variable names used for sorting the result

      atermpp::table variable_rank(2*eqsys.size(),50);

      // Fill the pbes_equations table
      if (mcrl2::core::gsVerbose)
      { std::cerr << "Retrieving pbes_equations from equation system...\n";
      }

      assert(eqsys.size()>0); // There should be at least one equation
      fixpoint_symbol current_fixpoint_symbol=eqsys.begin()->symbol();

      unsigned long rank=1;

      for (typename Container::iterator eqi = eqsys.begin(); eqi != eqsys.end(); eqi++)
      {
        pbes_equations.put(
                eqi->variable().name(),
                pbes_equation_to_aterm(
                             pbes_equation(
                                    eqi->symbol(),
                                    eqi->variable(),
                                    pbes_expression_rewrite_and_simplify(
                                             eqi->formula()
                                             ))));
        // Rewriting terms here can lead to non termination, in 
        // case the quantifier-all rewriter is used. This kind of rewriting
        // should be done outside this method. 
        //            pbes_rewriter(eqi->formula())));
        if (eqi->symbol()!=current_fixpoint_symbol)
        { current_fixpoint_symbol=eqi->symbol();
          rank=rank+1;
        }
        variable_rank.put(eqi->variable().name(),atermpp::aterm_int(rank));
      }

      unsigned long relevance_counter=0;
      unsigned long relevance_counter_limit=100;
      #define RELEVANCE_DIVIDE_FACTOR 100

      if (mcrl2::core::gsVerbose)
      { std::cerr << "Computing a BES from the PBES....\n";
      }
      // Set the first BES equation X1=X2
      add_equation(
                    1,
                    eqsys.begin()->symbol(),
                    1,
                    variable(2));

      // Variables used in whole function
      unsigned long nr_of_processed_variables = 1;
      unsigned long nr_of_generated_variables = 2;

      // As long as there are states to be explored
      while ((opt_strategy>=on_the_fly)
                 ?todo.size()>0
                 :(nr_of_processed_variables < nr_of_generated_variables))
      {
        variable_type variable_to_be_processed;
        if (opt_strategy>=on_the_fly)
        { variable_to_be_processed=todo.front();
          todo.pop_front();
        }
        else
        { variable_to_be_processed=nr_of_processed_variables+1;
        }

        if (is_relevant(variable_to_be_processed))
               // If v is not relevant, it does not need to be investigated.
        {

          pbes_equation current_pbeq;
          // atermpp::map<mcrl2::data::variable, mcrl2::data::data_expression_with_variables > sigma;

          // Add the required substitutions
          if (internal_opt_store_as_tree)
          {  // The current varable instantiation is stored as a tree, and this tree must be unfolded.
             ATerm t=variable_index.get(variable_to_be_processed);
             if (!is_pair(t))
             { // Then t is the name of the current_variable_instantiation, and it has
               // no arguments.

               current_pbeq = pbes_equation(pbes_equations.get(t));
               assert(current_pbeq.variable().parameters().size()==0);
             }
             else
             { // t is a pair, with a name as its left hand side.
               current_pbeq = pbes_equation(pbes_equations.get(ATgetArgument(t,0)));
               // the right hand side of t are the parameters, in a tree structure.

               t=ATgetArgument(t,1);
               variable_list::iterator iter=current_pbeq.variable().parameters().begin();
               assign_variables_in_tree(t,iter,Mucks_rewriter,opt_precompile_pbes);
             }

          }
          else // The current variable instantiation is a propositional_variable_instantiation
          {
            propositional_variable_instantiation current_variable_instantiation =
                propositional_variable_instantiation(variable_index.get(variable_to_be_processed));

            current_pbeq = pbes_equation(pbes_equations.get(current_variable_instantiation.name()));
            assert(current_pbeq!=pbes_equation());  // If this fails, a pbes variable is used in
                                                    // a right hand side, and not in the left hand side
                                                    // of an equation.

            data_expression_list::iterator elist=current_variable_instantiation.parameters().begin();

            // Rewriter *data_rewriter=pbes_rewriter.get_rewriter();
            for(variable_list::iterator vlist=current_pbeq.variable().parameters().begin() ;
                   vlist!=current_pbeq.variable().parameters().end() ; vlist++)
            {
              assert(elist!=current_variable_instantiation.parameters().end());
              if (opt_precompile_pbes)
              { Mucks_rewriter.setSubstitutionInternal(*vlist,(atermpp::aterm)*elist);
              }
              else
              {
                Mucks_rewriter.setSubstitution(*vlist,*elist);
              }

              // sigma[*vlist]=*elist;
              elist++;
            }
            assert(elist==current_variable_instantiation.parameters().end());
          }

          pbes_expression new_pbes_expression =
                // pbes_rewriter(current_pbeq.formula(),make_map_substitution_adapter(sigma)); This is the code if the
                // rewriters work with an acceptable performance.
                        mcrl2::pbes_system::detail::pbes_expression_substitute_and_rewrite
                                    ( current_pbeq.formula(),
                                      pbes_spec.data(),
                                      Mucks_rewriter,
                                      opt_precompile_pbes
                                    );

          bes_expression new_bes_expression=
               add_propositional_variable_instantiations_to_indexed_set_and_translate(
                            new_pbes_expression,
                            variable_index,
                            nr_of_generated_variables,
                            opt_use_hashtables,
                            opt_strategy,
                            opt_construct_counter_example,
                            variable_to_be_processed);

          /* No need to clear up sigma, as it was locally declared. */
          /* Rewriter *data_rewriter=pbes_rewriter.get_rewriter(); */
          for(variable_list::iterator vlist=current_pbeq.variable().parameters().begin() ;
                   vlist!=current_pbeq.variable().parameters().end() ; vlist++)
          { Mucks_rewriter.clearSubstitution(*vlist);
          }  


          if (opt_strategy>=on_the_fly_with_fixed_points)
          { // find a variable in the new_bes_expression from which `variable' to be
            // processed is reachable. If so, new_bes_expression can be set to
            // true or false.

            if (current_pbeq.symbol()==fixpoint_symbol::mu())
            {
              if (find_mu_loop(
                                   new_bes_expression,
                                   variable_to_be_processed,
                                   atermpp::aterm_int(variable_rank.get(current_pbeq.variable().name())).value()))
              { new_bes_expression=false_();
                if (opt_construct_counter_example)
                { counter_example_queue(variable_to_be_processed).
                       push_back(counter_example(variable_to_be_processed,MU_CYCLE));
                }
              }
            }
            else
            {
              if (find_nu_loop(
                                   new_bes_expression,
                                   variable_to_be_processed,
                                   atermpp::aterm_int(variable_rank.get(current_pbeq.variable().name())).value()))
              { new_bes_expression=true_();
                if (opt_construct_counter_example)
                { counter_example_queue(variable_to_be_processed).
                       push_back(counter_example(variable_to_be_processed,NU_CYCLE));
                }
              }
            }
          }

          if ((opt_strategy>=on_the_fly))
          {
            add_equation(
                    variable_to_be_processed,
                    current_pbeq.symbol(),
                    atermpp::aterm_int(variable_rank.get(current_pbeq.variable().name())).value(),
                    new_bes_expression,
                    todo);

            /* So now and then (after doing as many operations on the size of bes_equations,
               the relevances of variables must be reset, to avoid investigating irrelevant
               variables. There is an invariant in the system that all variables reachable
               from the initial variable 1, are always relevant. Furthermore, relevant
               variables that need to be investigated are always in the todo list */
            relevance_counter++;
            if (relevance_counter>=relevance_counter_limit)
            { relevance_counter_limit=nr_of_variables()/RELEVANCE_DIVIDE_FACTOR;
              relevance_counter=0;
              refresh_relevances(todo);
            }
          }
          else
          { add_equation(
                    variable_to_be_processed,
                    current_pbeq.symbol(),
                    atermpp::aterm_int(variable_rank.get(current_pbeq.variable().name())).value(),
                    new_bes_expression);
          }

          if (opt_strategy>=on_the_fly)
          {
            if (is_true(new_bes_expression)||is_false(new_bes_expression))
            {
              // new_bes_expression is true or false and opt_strategy is on the fly or higher.
              // This means we must optimize the y substituting true/false for this variable
              // everywhere. For this we use the occurrence set.

              std::set <variable_type> to_set_to_true_or_false;
              to_set_to_true_or_false.insert(variable_to_be_processed);
              for( ; !to_set_to_true_or_false.empty() ; )
              {
                variable_type w=(*to_set_to_true_or_false.begin());

                // Take the lowest element for substitution, to generate
                // short counterexample.

                // gsVerboseMsg("------------------ %d\n",(unsigned long)w);
                to_set_to_true_or_false.erase(w);
                for( std::set <variable_type>::iterator
                          v=variable_occurrence_set_begin(w);
                          v!=variable_occurrence_set_end(w);
                          v++)
                {

                  bes_expression b=get_rhs(*v);
                  if (opt_construct_counter_example)
                  { b=substitute_true_false(b,w,get_rhs(w),
                                             counter_example_queue(*v));
                  }
                  else
                  { b=substitute_true_false(b,w,get_rhs(w));
                  }

                  if (is_true(b)||is_false(b))
                  { to_set_to_true_or_false.insert(*v);
                  }
                  relevance_counter++;
                  set_rhs(*v,b,w);
                }
                clear_variable_occurrence_set(w);
              }
            }
          }
        }
        nr_of_processed_variables++;
//Due to internal consistency checks, pbes2bool is slow in non NDEBUG mode.
//More messages are therefore required.
#ifdef NDEBUG
        if (nr_of_processed_variables % 1000 == 0)
#else
        if (nr_of_processed_variables % 10 == 0)
#endif
        {
          if (mcrl2::core::gsVerbose)
          {
            std::cerr << "Processed " << nr_of_processed_variables <<
                          " and generated " << nr_of_generated_variables <<
                               " boolean variables\n";
          }
        }
      }
      refresh_relevances();
    }


  private:

    void print_tree_rec(const char c,
                        ATerm t,
                        std::ostream &f)
    { using namespace mcrl2::data;
      if (is_pair(t))
      { print_tree_rec(c,ATgetArgument(t,0),f);
        print_tree_rec(',',ATgetArgument(t,1),f);
      }
      else
      { if (opt_precompile_pbes)
        { data_expression t1(Mucks_rewriter.fromRewriteFormat((ATerm)t));
          f << c << pp(t1);
        }
        else
        { data_expression t1(t);
          f << c << pp(t1);
        }
      }
    }


    void print_counter_example_rec(bes::variable_type current_var,
                                   std::string indent,
                                   std::vector<bool> &already_printed,
                                   std::ostream &f) 
    { using namespace mcrl2::data;
      using namespace mcrl2::pbes_system;
      if (internal_opt_store_as_tree)
      { ATerm t=variable_index.get(current_var);
        if (!is_pair(t))
        { f << ATgetName(ATgetAFun(t));
        }
        else
        { f << ATgetName(ATgetAFun(ATgetArgument(t,0)));
          print_tree_rec('(',ATgetArgument(t,1),f);
          f << ")";
        }
      }
      else
      {
        propositional_variable_instantiation X(variable_index.get(current_var));

        data_expression_list tl=X.parameters();
        std::string s=X.name();
        f << s;
        data_expression_list::iterator t;
        for(t=tl.begin(); t!=tl.end(); ++t)
        { f << ((t==tl.begin())?"(":",");
          if (opt_precompile_pbes)
          { ATermAppl term=*t;
            f << pp(Mucks_rewriter.fromRewriteFormat((ATerm)term));
          }
          else
          { f << pp(*t);
          }
        }
        f << ((t==tl.begin())?"":")"); // No closing bracket if there are tl.begin()==tl.end()
      }

      if (already_printed[current_var])
      { f << "*\n";
      }
      else
      { f << "\n";
        already_printed[current_var]=true;

        for(std::deque < bes::counter_example>::iterator walker=counter_example_begin(current_var);
            walker!=counter_example_end(current_var) ; walker++)
        {
          f << indent << (*walker).get_variable() << ": " << (*walker).print_reason() << "  " ;
          print_counter_example_rec((*walker).get_variable(),indent+"  ",
                                already_printed,
                                f);
        }
      }
    }

  public:
    void print_counter_example(const std::string filename) 
    { 
      std::ofstream f;
      std::vector <bool> already_printed(nr_of_variables()+1,false);
      if (filename.empty())
      { // Print the counterexample to cout.
        std::cout << "Below the justification for this outcome is listed\n1: ";
        print_counter_example_rec(2,"  ",already_printed,std::cout);
      }
      if (f!=NULL)
      {
        try
        {
          std::ofstream f(filename.c_str());
          f << "Below the justification for this outcome is listed\n1: ";
          print_counter_example_rec(2,"  ",already_printed,f);
          f.close();
        }
        catch (std::exception& e)
        { std::cerr << "Fail to write counterexample to " << filename <<
                   "(" << e.what() << ")\n";
        }
      }
    }


  };


} // namespace bes.

  namespace atermpp
  {
    template<>
    struct aterm_traits<bes::bes_expression>
    {
      typedef ATermAppl aterm_type;
      static void protect(bes::bes_expression t)   { t.aterm().protect(); }   // protect the term against garbage collection
      static void unprotect(bes::bes_expression t) { t.aterm().unprotect(); } // undo the protection against garbage collection
      static void mark(bes::bes_expression t)      { t.aterm().mark(); }      // mark the term for not being garbage collected
                                                                    // when it is inside a protected container
      static ATerm term(bes::bes_expression t)     { return t.term(); }  // return the ATerm corresponding to t
      static ATerm* ptr(bes::bes_expression& t)    { return &t.term(); } // return the address of the ATerm corresponding to t
    };
  } // namespace atermpp


namespace bes
{

template <class Container>
void save_bes_in_pbes_format(
                          const std::string &outfilename,
                          boolean_equation_system &bes_equations,
                          const typename mcrl2::pbes_system::pbes<Container> &p);
void save_bes_in_cwi_format(const std::string &outfilename,boolean_equation_system &bes_equations);
void save_bes_in_vasy_format(const std::string &outfilename,boolean_equation_system bes_equations);


static void save_rhs_in_cwi_form(std::ostream &outputfile,
                                     bes_expression p,
                                     boolean_equation_system &bes_equations);

static void save_rhs_in_vasy_form(std::ostream &outputfile,
                                     bes_expression p,
                                     std::vector<unsigned long> &variable_index,
                                     const unsigned long current_rank,
                                     boolean_equation_system &bes_equations);


//function save_bes_in_vasy_format
//--------------------------------

typedef enum { both, and_form, or_form} expression_sort;

static bes_expression translate_equation_for_vasy(const unsigned long i,
                                        const bes_expression b,
                                        const expression_sort s,
                                        boolean_equation_system &bes_equations)
{
  if (is_true(b))
  { return b;
  }
  else if (is_false(b))
  { return b;
  }
  else if (is_and(b))
  {
    if (s==or_form)
    { /* make a new equation B=b, and return B */
      variable_type v=bes_equations.nr_of_variables()+1;
      bes_equations.add_equation(v,
                                 bes_equations.get_fixpoint_symbol(i),
                                 bes_equations.get_rank(i),
                                 b);
      return variable(v);
    }
    else
    {
      bes_expression b1=translate_equation_for_vasy(i,lhs(b),and_form,bes_equations);
      bes_expression b2=translate_equation_for_vasy(i,rhs(b),and_form,bes_equations);
      return and_(b1,b2);
    }
  }
  else if (is_or(b))
  {
    if (s==and_form)
    { /* make a new equation B=b, and return B */
      variable_type v=bes_equations.nr_of_variables()+1;
      bes_equations.add_equation(v,
                                 bes_equations.get_fixpoint_symbol(i),
                                 bes_equations.get_rank(i),
                                 b);
      return variable(v);
    }
    else
    {
      bes_expression b1=translate_equation_for_vasy(i,lhs(b),or_form,bes_equations);
      bes_expression b2=translate_equation_for_vasy(i,rhs(b),or_form,bes_equations);
      return or_(b1,b2);
    }
  }
  else if (is_variable(b))
  {
    return b;
  }
  else if (is_if(b))
  { //BESIF(x,y,z) is equivalent to (y & (x|z)) provided the expression is monotonic.
    return translate_equation_for_vasy(i,
                    and_optimized(then_branch(b),
                         or_optimized(condition(b),else_branch(b))),s,bes_equations);
    /* const bes_expression y=then_branch(b);
    const bes_expression z=else_branch(b);
    if (is_true(y))
    { if (is_false(z))
      { return translate_equation_for_vasy(i,condition(b),s,bes_equations);
      }
      else
      { return translate_equation_for_vasy(i,or_(condition(b),z),s,bes_equations);
      }
    }
    else if (is_false(z))
    { // not is_true(y)
      { return translate_equation_for_vasy(i,and_(condition(b),y),s,bes_equations);
      }
    }
    else
    { gsErrorMsg("The generated equation system is not a monotonic BES. It cannot be saved in VASY-format.\n");
      e x i t(1);
    } */
  }
  else
  {
    throw mcrl2::runtime_error("The generated equation system is not a BES. It cannot be saved in VASY-format.\n");
  }
  return b;
}

/// \brief Save the bes in the format in use by the VASY group, INRIA, Grenoble
/// \detail Save the BES in blocks with equal rank. 
/// \param string The name of the output file
/// \parar bes_equations The bes equations to bes saved.
void save_bes_in_vasy_format(const std::string &outfilename,bes::boolean_equation_system bes_equations)
{ using namespace mcrl2::pbes_system;
  if (mcrl2::core::gsVerbose)
  { std::cerr << "Converting result to VASY-format...\n";
  }
  // Use an indexed set to keep track of the variables and their vasy-representations

  /* First translate the right hand sides of the equations such that they only
     contain only conjunctions of disjunctions. Note that dynamically new
     equations are added during the translation process in "translate_equation_for_vasy"
     that must also be translated. */

  for(unsigned long i=1; i<=bes_equations.nr_of_variables() ; i++)
  {
    bes_equations.set_rhs(i,translate_equation_for_vasy(i,bes_equations.get_rhs(i),both,bes_equations));
  }

  /* Second give a consecutive index to each variable of a particular rank */

  std::vector<unsigned long> variable_index(bes_equations.nr_of_variables()+1);
  for(unsigned long r=1 ; r<=bes_equations.max_rank ; r++)
  { unsigned long index=0;
    for(unsigned long i=1; i<=bes_equations.nr_of_variables() ; i++)
    { if (bes_equations.get_rank(i)==r)
      {
        variable_index[i]=index;
        index++;
      }
    }
  }

  /* Third save the equations in the forms of blocks of equal rank */

  std::ofstream outputfile;
  if (outfilename!="")
  { outputfile.open(outfilename.c_str(), std::ios::trunc);
    if (!outputfile.is_open())
    { throw mcrl2::runtime_error("Could not save BES to " + outfilename + "\n");
    }
  }

  for(unsigned long r=1 ; r<=bes_equations.max_rank ; r++)
  { bool first=true;
    for(unsigned long i=1; i<=bes_equations.nr_of_variables() ; i++)
    { if (bes_equations.is_relevant(i) && (bes_equations.get_rank(i)==r))
      { if (first)
        { ((outfilename=="")?std::cout:outputfile) <<
             "block " <<
             ((bes_equations.get_fixpoint_symbol(i)==fixpoint_symbol::mu()) ? "mu  B" : "nu B") <<
             r-1 <<
             " is " << std::endl;
           first=false;
        }
        ((outfilename=="")?std::cout:outputfile) << "  X" << variable_index[i] << " = ";
        save_rhs_in_vasy_form(((outfilename=="")?std::cout:outputfile),
                                 bes_equations.get_rhs(i),
                                 variable_index,
                                 r,
                                 bes_equations);
        ((outfilename=="")?std::cout:outputfile) << std::endl;
      }
    }
    ((outfilename=="")?std::cout:outputfile) << "end block" << std::endl << std::endl;
  }

  outputfile.close();
}

//function save_rhs_in_vasy_form
//---------------------------
static void save_rhs_in_vasy_form(std::ostream &outputfile,
                                     bes_expression b,
                                     std::vector<unsigned long> &variable_index,
                                     const unsigned long current_rank,
                                     bes::boolean_equation_system &bes_equations)
{
  if (is_true(b))
  { outputfile << "true";
  }
  else if (is_false(b))
  { outputfile << "false";
  }
  else if (is_and(b))
  {
    //BESAnd(a,b) => a and b
    save_rhs_in_vasy_form(outputfile,lhs(b),variable_index,current_rank,bes_equations);
    outputfile << " and ";
    save_rhs_in_vasy_form(outputfile,rhs(b),variable_index,current_rank,bes_equations);
  }
  else if (is_or(b))
  {
    //BESOr(a,b) => a or b
    save_rhs_in_vasy_form(outputfile,lhs(b),variable_index,current_rank,bes_equations);
    outputfile << " or ";
    save_rhs_in_vasy_form(outputfile,rhs(b),variable_index,current_rank,bes_equations);
  }
  else if (is_variable(b))
  {
    // PropVar => <Int>
    outputfile << "X" << variable_index[get_variable(b)];
    if (bes_equations.get_rank(get_variable(b))!=current_rank)
    { outputfile << "_" << bes_equations.get_rank(get_variable(b))-1;
    }
  }
  else if (is_if(b))
  { //BESIF(x,y,z) is equivalent to (y & (x|z)) provided the expression is monotonic.
    save_rhs_in_vasy_form(outputfile,
                          and_optimized(then_branch(b),
                            or_optimized(condition(b),else_branch(b))),
                          variable_index,
                          current_rank,
                          bes_equations);
  }
  else
  {
    throw std::runtime_error("The generated equation system is not a BES. It cannot be saved in VASY-format.\n");
  }
  return;
}

//function save_bes_in_cwi_format
//--------------------------------
/// \brief Save the bes in the format in use by CWI, Amsterdam
/// \detail The BES equations are saved using rules of the form
///         min Xi = expression or max Xi = expression, where i is an index and
///         expression of the synax E = (E&E) | (E|E) | Xj | T | F.  The equations
///         are saved with increasing rank. Variable X1 is the initial variable.
/// \param string The name of the output file
/// \param bes_equations The bes equations to bes saved.
void save_bes_in_cwi_format(const std::string &outfilename,boolean_equation_system &bes_equations)
{ using namespace mcrl2::pbes_system;
  if (mcrl2::core::gsVerbose)
  { std::cerr << "Converting result to CWI-format...\n";
  }
  // Use an indexed set to keep track of the variables and their cwi-representations

  std::ofstream outputfile;
  if (outfilename!="")
  { outputfile.open(outfilename.c_str(), std::ios::trunc);
    if (!outputfile.is_open())
    { throw mcrl2::runtime_error("Could not save BES to " + outfilename + "\n");
    }
  }

  for(unsigned long r=1 ; r<=bes_equations.max_rank ; r++)
  { for(unsigned long i=1; i<=bes_equations.nr_of_variables() ; i++)
    {
      if (bes_equations.is_relevant(i) && (bes_equations.get_rank(i)==r) )
      { ((outfilename=="")?std::cout:outputfile) <<
              ((bes_equations.get_fixpoint_symbol(i)==fixpoint_symbol::mu()) ? "min X" : "max X") << i << "=";
        save_rhs_in_cwi_form(((outfilename=="")?std::cout:outputfile),bes_equations.get_rhs(i),bes_equations);
        ((outfilename=="")?std::cout:outputfile) << std::endl;
      }
    }
  }

  outputfile.close();
}

//function save_rhs_in_cwi
//---------------------------
static void save_rhs_in_cwi_form(std::ostream &outputfile,
                                 bes_expression b,
                                 boolean_equation_system &bes_equations)
{
  if (is_true(b))
  { outputfile << "T";
  }
  else if (is_false(b))
  { outputfile << "F";
  }
  else if (is_and(b))
  {
    //BESAnd(a,b) => (a & b)
    outputfile << "(";
    save_rhs_in_cwi_form(outputfile,lhs(b),bes_equations);
    outputfile << "&";
    save_rhs_in_cwi_form(outputfile,rhs(b),bes_equations);
    outputfile << ")";
  }
  else if (is_or(b))
  {
    //BESOr(a,b) => (a | b)
    outputfile << "(";
    save_rhs_in_cwi_form(outputfile,lhs(b),bes_equations);
    outputfile << "|";
    save_rhs_in_cwi_form(outputfile,rhs(b),bes_equations);
    outputfile << ")";
  }
  else if (is_variable(b))
  {
    // PropVar => <Int>
    outputfile << "X" << get_variable(b);
  }
  else if (is_if(b))
  { //BESIF(x,y,z) is equivalent to (y & (x|z)) provided the expression is monotonic.
    save_rhs_in_cwi_form(outputfile,and_optimized(then_branch(b),
                                    or_optimized(condition(b),else_branch(b))),bes_equations);
  }
  else
  {
    throw mcrl2::runtime_error("The generated equation system is not a BES. It cannot be saved in CWI-format.\n");
  }
  return;
}

static mcrl2::pbes_system::pbes_expression generate_rhs_as_formula(bes_expression b);

//function save_bes_in_pbes_format
//--------------------------------
/// \brief Save the bes as a PBES without parameters
/// \detail The BES equations are saved as a PBES file in ATerm format with name
///         outfilename. All datatypes are taken from the pbes p.
/// \param string The name of the output file
/// \param bes_equations The bes equations to bes saved.
/// \param p A PBES from which the datatypes are taken for the PBES that is saved.

template <class Container>
static void save_bes_in_pbes_format(
                   const std::string &outfilename,
                   boolean_equation_system &bes_equations,
                   const typename mcrl2::pbes_system::pbes<Container> &p)
{ using namespace mcrl2::pbes_system;
  using namespace mcrl2::data;
  if (mcrl2::core::gsVerbose)
  { std::cerr << "Converting result to PBES-format...\n";
  }
  // Use an indexed set to keep track of the variables and their pbes-representations

  atermpp::vector < pbes_equation > eqns;
  for(unsigned long r=1 ; r<=bes_equations.max_rank ; r++)
  { for(unsigned long i=1; i<=bes_equations.nr_of_variables() ; i++)
    {
      if (bes_equations.is_relevant(i) && (bes_equations.get_rank(i)==r) )
      {  pbes_expression pbe=generate_rhs_as_formula(bes_equations.get_rhs(i));
         std::stringstream variable_name;
         variable_name << "X" << i;
         eqns.push_back(
               pbes_equation(
                  bes_equations.get_fixpoint_symbol(i),
                  propositional_variable(variable_name.str()),
                  generate_rhs_as_formula(bes_equations.get_rhs(i))));
      }
    }
  }

  pbes<> p1(p.data(),eqns,atermpp::set<mcrl2::data::variable>(),
                             propositional_variable_instantiation("X1"));
  p1.save(outfilename);
}

//function save_rhs_in_pbes
//---------------------------
static mcrl2::pbes_system::pbes_expression generate_rhs_as_formula(bes_expression b)
{ using namespace mcrl2::pbes_system;
  if (is_true(b))
  { return mcrl2::pbes_system::pbes_expr::true_();
  }
  else if (is_false(b))
  { return pbes_expr::false_();
  }
  else if (is_and(b))
  { return pbes_expr::and_(generate_rhs_as_formula(lhs(b)),
                generate_rhs_as_formula(rhs(b)));
  }
  else if (is_or(b))
  { return pbes_expr::or_(generate_rhs_as_formula(lhs(b)),
               generate_rhs_as_formula(rhs(b)));
  }
  else if (is_variable(b))
  { std::stringstream converter;
    converter << "X" << get_variable(b);
    return propositional_variable_instantiation(converter.str());
  }
  else if (is_if(b))
  { //BESIF(x,y,z) is equivalent to (y & (x|z)) provided the expression is monotonic.
    return generate_rhs_as_formula(and_optimized(then_branch(b),
                                   or_optimized(condition(b),else_branch(b))));
  }
  else
  {
    throw mcrl2::runtime_error("The generated equation system is not a BES. It cannot be saved in CWI-format.\n");
  }
  return pbes_expr::true_();
}

/// From here there are routines to solve a BES.

/* substitute boolean equation expression */
static bes_expression substitute_rank(
                bes_expression b,
                const unsigned long current_rank,
                const atermpp::vector<bes_expression> &approximation,
                bes::boolean_equation_system &bes_equations,
                const bool use_hashtable,
                atermpp::table &hashtable,
                bool store_counter_example=false,
                bes::variable_type current_variable=0)
{ /* substitute variables with rank larger and equal
     than current_rank with their approximations. */

  if (bes::is_true(b)||bes::is_false(b)||bes::is_dummy(b))
  { return b;
  }

  bes_expression result;
  if (use_hashtable)
  { result=hashtable.get(b);
    if (result!=NULL)
    { return result;
    }
  }

  if (is_variable(b))
  { bes::variable_type v=bes::get_variable(b);
    if (bes_equations.get_rank(v)==current_rank)
    {
      result=approximation[v];
      if (store_counter_example)
      { if (bes::is_true(result))
        { bes_equations.counter_example_queue(current_variable).
                   push_front(bes::counter_example(v,bes::APPROXIMATION_TRUE));
        }
        else if (bes::is_false(result))
        { bes_equations.counter_example_queue(current_variable).
                   push_front(bes::counter_example(v,bes::APPROXIMATION_FALSE));
        }
        else
        { bes_equations.counter_example_queue(current_variable).
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
  { bes_expression b1=substitute_rank(lhs(b),current_rank,approximation,bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
    if (is_false(b1))
    { result=b1;
    }
    else
    { bes_expression b2=substitute_rank(rhs(b),current_rank,approximation,bes_equations,
                                          use_hashtable,hashtable,store_counter_example,current_variable);
      if (is_false(b2))
      { result=b2;
      }
      else if (is_true(b1))
      { result=b2;
      }
      else if (is_true(b2))
      { result=b1;
      }
      else
      { result=and_(b1,b2);
      }
    }
  }
  else if (is_or(b))
  { bes_expression b1=substitute_rank(lhs(b),current_rank,approximation,bes_equations,use_hashtable,
                                          hashtable,store_counter_example,current_variable);
    if (is_true(b1))
    { result=b1;
    }
    else
    { bes_expression b2=substitute_rank(rhs(b),current_rank,approximation,bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
      if (is_true(b2))
      { result=b2;
      }
      else if (is_false(b1))
      { result=b2;
      }
      else if (is_false(b2))
      { result=b1;
      }
      else
      { result=or_(b1,b2);
      }
    }
  }


  else if (is_if(b))
  {
    bes::variable_type v=bes::get_variable(condition(b));
    if (bes_equations.get_rank(v)==current_rank)
    {
      if (bes::is_true(approximation[v]))
      { result=substitute_rank(then_branch(b),current_rank,approximation,bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
        if (store_counter_example)
        { bes_equations.counter_example_queue(current_variable).
                     push_front(bes::counter_example(v,bes::APPROXIMATION_TRUE));
        }
      }
      else if (bes::is_false(approximation[v]))
      { result=substitute_rank(else_branch(b),current_rank,approximation,bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
        if (store_counter_example)
        { bes_equations.counter_example_queue(current_variable).
                     push_front(bes::counter_example(v,bes::APPROXIMATION_FALSE));
        }
      }
      else
      { bes_expression b1=substitute_rank(then_branch(b),current_rank,approximation,
                                          bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
        bes_expression b2=substitute_rank(else_branch(b),current_rank,approximation,
                                          bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
        result=BDDif(approximation[v],b1,b2);
      }
    }
    else
    { /* the condition is not equal to v */
      bes_expression b1=substitute_rank(then_branch(b),current_rank,approximation,
                                        bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
      bes_expression b2=substitute_rank(else_branch(b),current_rank,approximation,
                                        bes_equations,use_hashtable,hashtable,store_counter_example,current_variable);
      if ((b1==then_branch(b)) && (b2==else_branch(b)))
      { result=b;
      }
      else
      { result=BDDif(bes::if_(condition(b),bes::true_(),bes::false_()),b1,b2);
      }
    }
  }
  else
  {
    assert(0);  // expect an if, variable, and, or, or a true or a false here.
  }

  if (use_hashtable)
  { hashtable.put(b,result);
  }
  return result;
}

/* substitute boolean equation expression */

static bes_expression evaluate_bex(
                bes_expression b,
                const atermpp::vector<bes_expression> &approximation,
                const unsigned long rank,
                bes::boolean_equation_system &bes_equations,
                const bool use_hashtable,
                atermpp::table &hashtable,
                const bool construct_counter_example,
                const bes::variable_type current_variable)
{ /* substitute the approximation for variables in b, given
     by approximation, for all those variables that have a
     rank higher or equal to the variable rank; */

  if (bes::is_true(b)||bes::is_false(b))
  { return b;
  }

  bes_expression result;
  if (use_hashtable)
  { result=hashtable.get(b);
    if (result!=NULL)
    return result;
  }

  if (is_variable(b))
  { const bes::variable_type v=bes::get_variable(b);
    if (bes_equations.get_rank(v)>=rank)
    {
      result=approximation[v];
      if (construct_counter_example)
      { if (bes::is_true(result))
        { bes_equations.counter_example_queue(current_variable).
                   push_front(bes::counter_example(v,bes::APPROXIMATION_TRUE));
        }
        else if (bes::is_false(result))
        { bes_equations.counter_example_queue(current_variable).
                   push_front(bes::counter_example(v,bes::APPROXIMATION_FALSE));
        }
        else
        { bes_equations.counter_example_queue(current_variable).
                   push_front(bes::counter_example(v,bes::APPROXIMATION));
        }
      }
    }
    else
    { /* the condition has lower rank than the variable rank,
         leave it untouched */
      result=b;
    }
  }
  else if (is_and(b))
  { bes_expression b1=evaluate_bex(lhs(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
    if (is_false(b1))
    { result=b1;
    }
    else
    { bes_expression b2=evaluate_bex(rhs(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      if (is_false(b2))
      { result=b2;
      }
      else if (is_true(b1))
      { result=b2;
      }
      else if (is_true(b2))
      { result=b1;
      }
      else
      { result=and_(b1,b2);
      }
    }
  }
  else if (is_or(b))
  { bes_expression b1=evaluate_bex(lhs(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
    if (is_true(b1))
    { result=b1;
    }
    else
    { bes_expression b2=evaluate_bex(rhs(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      if (is_true(b2))
      { result=b2;
      }
      else if (is_false(b1))
      { result=b2;
      }
      else if (is_false(b2))
      { result=b1;
      }
      else
      { result=or_(b1,b2);
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
    { /* the condition has lower rank than the variable rank,
         leave it untouched */
      bes_expression b1=evaluate_bex(then_branch(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      bes_expression b2=evaluate_bex(else_branch(b),approximation,rank,bes_equations,use_hashtable,hashtable,construct_counter_example,current_variable);
      if ((b1==then_branch(b)) && (b2==else_branch(b)))
      { result=b;
      }
      else
      { result=BDDif(bes::if_(condition(b),bes::true_(),bes::false_()),b1,b2);
      }
    }
  }
  else
  { assert(0); //expect an if, true or false, and, variable or or;
  }

  if (use_hashtable)
  { hashtable.put(b,result);
  }
  return result;
}

bool solve_bes(bes::boolean_equation_system &bes_equations,
               const bool opt_use_hashtables,
               const bool opt_construct_counter_example)
{ using namespace mcrl2::core;
  using namespace std;
  using namespace mcrl2::pbes_system;

  if (mcrl2::core::gsVerbose)
  { std::cerr << "Solving a BES with " << bes_equations.nr_of_variables() <<
                                  " equations.\n";
  }
  atermpp::vector<bes_expression> approximation(bes_equations.nr_of_variables()+1);

  atermpp::table bex_hashtable(10,5);

  /* Set the approximation to its initial value */
  for(bes::variable_type v=bes_equations.nr_of_variables(); v>0; v--)
  {
    bes_expression b=bes_equations.get_rhs(v);
    if (b!=bes::dummy())
    { if (bes::is_true(bes_equations.get_rhs(v)))
      { approximation[v]=bes::true_();
      }
      else if (bes::is_false(bes_equations.get_rhs(v)))
      { approximation[v]=bes::false_();
      }
      else if (bes_equations.get_fixpoint_symbol(v)==fixpoint_symbol::mu())
      { approximation[v]=bes::false_();
      }
      else
      { approximation[v]=bes::true_();
      }
    }
  }

  bes_equations.store_variable_occurrences();

  for(unsigned long current_rank=bes_equations.max_rank;
      current_rank>0 ; current_rank--)
  {
    if (gsVerbose)
    { std::cerr << "Solve equations of rank " << current_rank << ".\n";
    }

    /* Calculate the stable solution for the current rank */

    set <bes::variable_type> todo;

    for(bes::variable_type v=bes_equations.nr_of_variables(); v>0; v--)
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
          { bex_hashtable.reset();  /* we change the approximation, so the
                                       hashtable becomes invalid */
          }
          if (opt_construct_counter_example)
          { if (opt_use_hashtables)
            { bex_hashtable.reset();  /* We want to construct a counter example, which is
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
          { approximation[v]=t;
          }
          todo.insert(v);
        }
      }
    }

    for( ; todo.size()>0 ; )
    { set<bes::variable_type>::iterator w= todo.begin();
      bes::variable_type w_value=*w;
      todo.erase(w);

      for(set <bes::variable_type>::iterator
         u=bes_equations.variable_occurrence_set_begin(w_value);
         u!=bes_equations.variable_occurrence_set_end(w_value);
         u++)
      {
        if (bes_equations.is_relevant(*u) && (bes_equations.get_rank(*u)==current_rank))
        { bes_expression t=evaluate_bex(
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
            { bex_hashtable.reset();  /* we change approximation, so the
                                         hashtable becomes invalid */
            }
            if (opt_construct_counter_example)
            { if (opt_use_hashtables)
              { bex_hashtable.reset();  /* We want to construct a counter example, which is
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
            { approximation[*u]=t;
            }
            todo.insert(*u);
          }
        }
      }
    }

    /* substitute the stable solution for the current rank in all other
       equations. */


    if (opt_use_hashtables)
    { bex_hashtable.reset();
    }

    for(bes::variable_type v=bes_equations.nr_of_variables(); v>0; v--)
    { if (bes_equations.is_relevant(v))
      {
        if (bes_equations.get_rank(v)==current_rank)
        {
          if (opt_construct_counter_example)
          { bes_equations.set_rhs(
                         v,
                         substitute_rank(
                                   bes_equations.get_rhs(v),
                                   current_rank,
                                   approximation,
                                   bes_equations,
                                   opt_use_hashtables,
                                   bex_hashtable,
                                   true,v));
          }
          else
          { bes_equations.set_rhs(v,approximation[v]);
          }
        }
        else
        { bes_equations.set_rhs(
                         v,
                         substitute_rank(
                                   bes_equations.get_rhs(v),
                                   current_rank,
                                   approximation,
                                   bes_equations,
                                   opt_use_hashtables,
                                   bex_hashtable,
                                   opt_construct_counter_example,v));
        }
      }
    }
    if (opt_use_hashtables)
    { bex_hashtable.reset();
    }

  }
  assert(bes::is_true(approximation[1])||
         bes::is_false(approximation[1]));
  return bes::is_true(approximation[1]);  /* 1 is the index of the initial variable */
}




} // end namespace bes

#endif // BES_DEPRECATED_H
