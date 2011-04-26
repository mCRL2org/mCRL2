// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/enum/standard.h

#ifndef _ENUM_STANDARD_H
#define _ENUM_STANDARD_H

#include <aterm2.h>
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/enum/enumerator.h"

/// \cond INTERNAL_DOCS
namespace mcrl2
{
namespace data
{
namespace detail
{

class fs_expr
{
  protected:
    variable_list m_vars;
    atermpp::term_list< atermpp::aterm_appl > m_vals;  // data_expression_list in internal format.
    atermpp::aterm_appl m_expr;  // data_expression in internal format.

  public:
    // Default constructor
    fs_expr()
    {}

    // Constructor
    fs_expr(const variable_list &vars, const atermpp::term_list< atermpp::aterm_appl > &vals, const atermpp::aterm_appl &expr):
       m_vars(vars), m_vals(vals), m_expr(expr)
    {}

    variable_list vars() const
    {
      return m_vars;
    }

    atermpp::term_list< atermpp::aterm_appl > vals() const
    {
      return m_vals;
    }

    atermpp::aterm_appl expr() const
    {
      return m_expr;
    }
};

class EnumeratorSolutionsStandard;

typedef struct
{
  Rewriter* rewr_obj;

  atermpp::map<ATermAppl, ATermList> constructors;
  atermpp::aterm_appl rewr_true, rewr_false;

  int* max_vars;

  ATerm opidAnd;
  atermpp::set< ATerm > eqs;
  AFun tupAFun;

} enumstd_info;

class EnumeratorStandard // : public Enumerator
{
  public:
    EnumeratorStandard(mcrl2::data::data_specification const& data_spec, Rewriter* r, bool clean_up_rewriter = false);
    ~EnumeratorStandard();

    EnumeratorSolutionsStandard* findSolutions(ATermList vars, ATerm expr, bool true_only, EnumeratorSolutionsStandard* old = NULL);

    Rewriter* getRewriter();
    enumstd_info& getInfo()
    {
      return info;
    }

  private:
    bool clean_up_rewr_obj;

    enumstd_info info;

    int max_vars;
};

class EnumeratorSolutionsStandard // : public EnumeratorSolutions
{
  public:
    EnumeratorSolutionsStandard(enumstd_info& Info) : info(Info)
    {
      enum_vars.protect();
      enum_expr.protect();
    }

    EnumeratorSolutionsStandard(const EnumeratorSolutionsStandard & other);



    EnumeratorSolutionsStandard(
                   const variable_list &Vars, 
                   const atermpp::aterm_appl &Expr, 
                   const bool not_equal_to_false, 
                   enumstd_info& Info) :
      m_not_equal_to_false(not_equal_to_false),
      used_vars(0)
    { 
      info = Info; 
      enum_vars.protect();
      enum_expr.protect();

      reset(Vars,Expr,m_not_equal_to_false);
    }


    EnumeratorSolutionsStandard():
      m_not_equal_to_false(true),
      used_vars(0)
    {
      enum_vars.protect();
      enum_expr.protect();
    }


    ~EnumeratorSolutionsStandard();

    bool next(ATermList* solution);

    void reset(const variable_list &Vars, const atermpp::aterm_appl &Expr, const bool netf);

    // bool FindInnerCEquality(ATerm t, ATermList vars, ATerm* v, ATerm* e);
    bool FindInnerCEquality(const atermpp::aterm_appl T, 
                            const mcrl2::data::variable_list vars, 
                            mcrl2::data::variable &v, 
                            atermpp::aterm_appl &e);
    ATerm build_solution_aux_innerc(ATerm t, ATermList substs);
    // ATerm build_solution_aux_inner3(ATerm t, ATermList substs);
    // bool FindInner3Equality(ATerm t, ATermList vars, ATerm* v, ATerm* e);
  private:
    enumstd_info info;

    variable_list enum_vars;
    atermpp::aterm_appl enum_expr; // Expression in internal format.

    bool m_not_equal_to_false;

    int used_vars;

    atermpp::vector < fs_expr> fs_stack;
    // int fs_stack_size;
    // int fs_stack_pos;

    atermpp::vector<ATermList> ss_stack;
    // int ss_stack_size;
    // int ss_stack_pos;

    // void fs_reset();
    // void fs_push(ATermList vars, ATermList vals, ATerm expr);
    // void fs_pop(fs_expr* e = NULL);

    // void ss_reset();
    // void ss_push(ATermList s);
    // ATermList ss_pop();

    void EliminateVars(fs_expr &e);
    // bool IsInner3Eq(ATerm a);
    bool IsInnerCEq(ATermAppl a);
    bool FindInnerCEquality_aux(ATerm t);
    ATerm build_solution_single(ATerm t, ATermList substs);
    ATermList build_solution2(ATermList vars, ATermList substs);
    ATermList build_solution(ATermList vars, ATermList substs);
};
}
}
}

namespace atermpp
{
template<>
struct aterm_traits<mcrl2::data::detail::fs_expr>
{
  static void protect(const mcrl2::data::detail::fs_expr& t)
  {
    assert(0); // This is not being used. This is to check this.
    t.vars().protect();
    t.vals().protect();
    t.expr().protect();
  }
  static void unprotect(const mcrl2::data::detail::fs_expr& t)
  {
    assert(0); // This is not being used. This is to check this.
    t.vars().unprotect();
    t.vals().unprotect();
    t.expr().unprotect();
  }
  static void mark(const mcrl2::data::detail::fs_expr& t)
  {
    t.vars().mark();
    t.vals().mark();
    t.expr().mark();
  }
};
} // namespace atermpp
/// \endcond

#endif
