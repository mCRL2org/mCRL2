// Author(s): Alexander van Dam
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bes.h
/// \brief Add your file description here.

#ifndef BES_H
#define BES_H

#include "atermpp/aterm.h"
#include "atermpp/aterm_traits.h"
#include "atermpp/aterm_access.h"
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_int.h"
#include "atermpp/table.h"
#include "mcrl2/pbes/pbes.h"

namespace bes 
{
  using atermpp::aterm_int;
  using atermpp::aterm;
  using atermpp::aterm_appl;
  using atermpp::arg1;
  using atermpp::arg2;
  using lps::fixpoint_symbol;


// a bes variable_type is an unsigned long. 

  typedef unsigned long variable_type; /* valid values start at 1 */

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


// below we define functions to construct bes_expressions using aterms.
//

  class bes_expression: public aterm
  {

    public:
      bes_expression()
        : aterm()
      {}

      bes_expression(aterm_appl term)
          : aterm(term)
      {}

      // bes_expression(atermpp::term_appl<atermpp::aterm> &term)
      //    : aterm_appl(term)
      // {}

      // allow assignment from aterms
      bes_expression& operator=(const atermpp::aterm &t)
      {
        m_term = t;
        return *this;
      }
  };

  static bes_expression initBEStrue(bes_expression &BEStrue)
  { BEStrue = ATmakeAppl0(ATmakeAFun("BESTrue", 0, ATfalse));
    ATprotect(reinterpret_cast<ATerm*>(&BEStrue));
    return BEStrue;
  }

  static bes_expression initBESfalse(bes_expression &BESfalse)
  { BESfalse = ATmakeAppl0(ATmakeAFun("BESFalse", 0, ATfalse));
    ATprotect(reinterpret_cast<ATerm*>(&BESfalse));
    return BESfalse;
  }

  static AFun initAFunBESAnd(AFun& f)
  { f = ATmakeAFun("BESAnd", 2, ATfalse);
    ATprotectAFun(f);
    return f;
  }

  static inline AFun AFunBESAnd()
  { static AFun BESAnd = initAFunBESAnd(BESAnd);
    return BESAnd;
  }

  static AFun initAFunBESOr(AFun& f)
  { f = ATmakeAFun("BESOr", 2, ATfalse);
    ATprotectAFun(f);
    return f;
  }

  static inline AFun AFunBESOr()
  { static AFun BESOr = initAFunBESOr(BESOr);
    return BESOr;
  }

  static AFun initAFunBESIf(AFun& f)
  { f = ATmakeAFun("BESIf", 3, ATfalse);
    ATprotectAFun(f);
    return f;
  }

  static inline AFun AFunBESIf()
  { static AFun BESIf = initAFunBESIf(BESIf);
    return BESIf;
  }

  /* inline bes_expression false_()
  { static bes_expression BESfalse=initBESfalse(BESfalse);
    return BESfalse;
  }

  inline bes_expression true_()
  { static bes_expression BEStrue=initBEStrue(BEStrue);
    return BEStrue;
  } */

  bes_expression BESfalse;
  inline bes_expression false_()
  { return (BESfalse?BESfalse:BESfalse=initBESfalse(BESfalse));
  }

  bes_expression BEStrue;
  inline bes_expression true_()
  {
    return (BEStrue?BEStrue:BEStrue=initBEStrue(BEStrue));
  }

  inline bes_expression and_(bes_expression b1,bes_expression b2)
  { return bes_expression(
               ATmakeAppl2(AFunBESAnd(),
                           (aterm)(b1),
                           (aterm)(b2)));
  }

  inline bes_expression or_(bes_expression b1,bes_expression b2)
  { return bes_expression(
               ATmakeAppl2(AFunBESOr(),
                           (aterm)(b1),
                           (aterm)(b2)));
  }

  inline bes_expression if_(bes_expression b1,bes_expression b2,bes_expression b3)
  { return bes_expression(
               ATmakeAppl3(AFunBESIf(),
                           (aterm)(b1),
                           (aterm)(b2),
                           (aterm)(b3)));
  }

  static inline bes_expression ifAUX_(bes_expression b1,bes_expression b2,bes_expression b3)
  { if (b2==b3)
    { return b2;
    }
    return if_(b1,b2,b3);
  }

  inline bes_expression variable(variable_type n)
  { return bes_expression((aterm)aterm_int(n));
  }

  inline bool is_false(bes_expression b)
  { // ATfprintf(stderr,"HIER--- %t %t\n",(ATerm)b,(ATerm)false_());
    return b==false_();
  }

  inline bool is_true(bes_expression b)
  { return b==true_();
  }

  inline bool is_and(bes_expression b)
  { return ATgetAFun((_ATerm*)b)==AFunBESAnd();
  }

  inline bool is_or(bes_expression b)
  { return ATgetAFun((_ATerm*)b)==AFunBESOr();
  }

  inline bool is_if(bes_expression b)
  { return ATgetAFun((_ATerm*)b)==AFunBESIf();
  }

  inline bes_expression lhs(bes_expression b)
  { assert(is_and(b) || is_or(b));
    return bes_expression(aterm_appl(b)(0));
  }

  inline bes_expression rhs(bes_expression b)
  { assert(is_and(b) || is_or(b));
    return bes_expression(aterm_appl(b)(1));
  }

  inline bool is_variable(bes_expression b)
  { return b.type()==AT_INT;
  }

  inline bes_expression condition(bes_expression b)
  { // ATfprintf(stderr,"condition %t\n",(ATerm)b);
    assert(is_if(b));
    return bes_expression(aterm_appl(b)(0));
  }

  inline bes_expression then_branch(bes_expression b)
  { assert(is_if(b));
    return bes_expression(aterm_appl(b)(1));
  }

  inline bes_expression else_branch(bes_expression b)
  { // ATfprintf(stderr,"else %t\n",(ATerm)b);
    assert(is_if(b));
    return bes_expression(aterm_appl(b)(2));
  }

  inline variable_type get_variable(bes_expression b)
  { assert(is_variable(b));
    return ((aterm_int)b).value();
  }

  static bes_expression BDDif_rec(bes_expression b1, bes_expression b2, bes_expression b3,atermpp::table &hashtable);


  bes_expression BDDif(bes_expression b1, bes_expression b2, bes_expression b3)
  { 
    
    static atermpp::table hashtable(100,75);
    // ATfprintf(stderr,"BDD b1 %t\n",(ATerm)b1);
    // ATfprintf(stderr,"BDD b2 %t\n",(ATerm)b2);
    // ATfprintf(stderr,"BDD b3 %t\n",(ATerm)b3);

    bes_expression b=BDDif_rec(b1,b2,b3,hashtable);
    // hashtable.reset(); This costs a lot of time, currently.
    // ATfprintf(stderr,"Result b %t\n",(ATerm)b);
    return b;
  }

  static bes_expression BDDif_rec(bes_expression b1, bes_expression b2, bes_expression b3,atermpp::table &hashtable)
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
    { // ATfprintf(stderr,"HIER\n");
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
      // ATfprintf(stderr,"HIER1\n");
      if (condition(b1)==condition(b2))
      { 
        // ATfprintf(stderr,"HIER2\n");
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
        // ATfprintf(stderr,"HIER3\n");
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

    /* ATfprintf(stderr,"BDD b1 %t\n",(ATerm)b1);
    ATfprintf(stderr,"BDD b2 %t\n",(ATerm)b2);
    ATfprintf(stderr,"BDD b3 %t\n",(ATerm)b3); 
    ATfprintf(stderr,"BDD result %t\n\n",(ATerm)result); */
    hashtable.put(b1b2b3,result);
    return result;
  }


  class equations
  { 
    private:
      // initial size is the initial size of 
      // the vector holding equations;
      std::vector<unsigned long> control_info;
      atermpp::vector<bes_expression> right_hand_sides;

    public:
      unsigned long max_rank;

      equations()
        : control_info(0),
          right_hand_sides(0),
          max_rank(0)
      {}

      void add_equation(variable_type v, 
                        fixpoint_symbol sigma,
                        unsigned long rank,
                        bes_expression rhs)
      { assert(rank>0);  // rank must be positive.
        assert(v>0);     // variables are represented by numbers >0.
        // std::cerr << "Add equation " << v << std::endl;
        if (v>=control_info.size())
        { control_info.resize(v+1,0);
          right_hand_sides.resize(v+1,true_());
        }
        // the vector at position v is now guaranteed to exist.
  
        // if the control info is 0, the value at variable
        // is not initialized.

        control_info[v]=(sigma.is_nu()?0:1)+2*(rank+1);
        right_hand_sides[v]=rhs;
        if (rank>max_rank)
        { max_rank=rank;
        }
      }

      inline unsigned long nr_of_equations()
      { return control_info.size()-1; /* there is no equation at position 0 */
      }

      inline fixpoint_symbol get_fixpoint_symbol(variable_type v)
      {
        assert(v>0); /* variable indices start at 1. 0 is used as an indicator of error */
        assert(v<=nr_of_equations());
        assert(control_info[v]>0);

        return (((control_info[v] % 2)==0) ? fixpoint_symbol::nu() : fixpoint_symbol::mu());
      }


      inline unsigned long get_rank(variable_type v)
      { // std::cerr << v << "-!-" << nr_of_equations() << std::endl;
        assert(v>0);
        assert(v<=nr_of_equations());
        assert(control_info[v]>0);
  
        return (control_info[v] / 2)-1;
      }

      inline void set_rhs(variable_type v,bes_expression b)
      {
        assert(v>0);
        assert(v<=nr_of_equations());
        // ATfprintf(stderr,"set rhs of equation %d to %t\n",v,(ATerm)b);
        right_hand_sides[v]=b;
      }

      inline bes_expression get_rhs(variable_type v)
      { // std::cerr << v << "--" << nr_of_equations() << std::endl;
        assert(v<=nr_of_equations());
        assert(control_info[v]>0);

        return right_hand_sides[v];
      }

  };
} // namespace bes.

namespace atermpp
{
  template<>
    struct aterm_traits<bes::bes_expression>
    {
      typedef ATermAppl aterm_type;
      static void protect(bes::bes_expression t)   { t.protect(); }
      static void unprotect(bes::bes_expression t) { t.unprotect(); }
      static void mark(bes::bes_expression t)      { t.mark(); }
      static ATerm term(bes::bes_expression t)     { return t.term(); }
      static ATerm* ptr(bes::bes_expression& t)    { return &t.term(); }
    };
} // namespace atermpp

#endif
