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

#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/deque.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/enum/enumerator.h"


/// \cond INTERNAL_DOCS
namespace mcrl2
{
namespace data
{
namespace detail
{

class ss_solution
{
  protected:
    atermpp::term_list< atermpp::aterm_appl > m_solution;  // A list containing the solution of a condition in internal format.
    bool m_solution_is_exact;                              // An indication whether the solution made the solution exactly false or true.

  public:

    // Constructor.
    ss_solution(const atermpp::term_list< atermpp::aterm_appl > &solution, const bool solution_is_exact) :
      m_solution(solution),
      m_solution_is_exact(solution_is_exact)
    {} 
   
    bool solution_is_exact() const
    { 
      return m_solution_is_exact;
    }

    atermpp::term_list< atermpp::aterm_appl > solution() const
    {
      return m_solution; 
    }
};

class fs_expr
{
  protected:
    variable_list m_vars;                              // The vars over which enumeration must take place.
    variable_list m_substituted_vars;                  // Variables for which a substitution exist. The substituted
                                                       // values are given in m_vals;
    atermpp::term_list< atermpp::aterm_appl > m_vals;  // Data expressions in internal format that are to be substituted
                                                       // in the variables in m_substituted_vars.
    atermpp::aterm_appl m_expr;                        // data_expression in internal format to which internal variables
                                                       // must adhere.

  public:
    // Default constructor
    fs_expr()
    {}

    // Constructor
    fs_expr(
        const variable_list &vars, 
        const variable_list &substituted_vars, 
        const atermpp::term_list< atermpp::aterm_appl > &vals, 
        const atermpp::aterm_appl &expr):
       m_vars(vars), m_substituted_vars(substituted_vars),m_vals(vals), m_expr(expr)
    {
    }

    variable_list vars() const
    {
      return m_vars;
    }

    variable_list substituted_vars() const
    {
      return m_substituted_vars;
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


class EnumeratorStandard // : public Enumerator
{

  private:
    bool clean_up_rewr_obj;

  
  public:
    const mcrl2::data::data_specification &m_data_spec;
    Rewriter* rewr_obj;
    atermpp::aterm_appl rewr_true, rewr_false;
  
    atermpp::aterm_int opidAnd;
    atermpp::set< atermpp::aterm_int > eqs;
  
    variable_list enum_vars;       // The variables over which a solution is searched.
    atermpp::aterm_appl enum_expr; // Condition to be satisfied in internal format.

    atermpp::deque < fs_expr> fs_stack;
    atermpp::vector< ss_solution > ss_stack;

    EnumeratorStandard(mcrl2::data::data_specification const& data_spec, Rewriter* r, bool clean_up_rewriter = false);
    ~EnumeratorStandard();

    /* EnumeratorSolutionsStandard* findSolutions(const variable_list vars, atermpp::aterm_appl expr, bool true_only, EnumeratorSolutionsStandard* old = NULL); */

    Rewriter* getRewriter() const
    {
      return rewr_obj;
    }
};

class EnumeratorSolutionsStandard // : public EnumeratorSolutions
{
  private:

    detail::EnumeratorStandard *m_enclosing_enumerator;

    // bool m_not_equal_to_false;
    atermpp::aterm_appl desired_truth_value;    // We search for solutions for the condition enum_expr that are not
    atermpp::aterm_appl forbidden_truth_value;  // equal to the forbidden truth value, and if the output matches the
                                                // desired truth value, then the variable solution_is_exact is set.

    size_t used_vars;
    size_t max_vars;


  public:

    /// Default constructor
    EnumeratorSolutionsStandard()
    {}

    /// Constructor
    EnumeratorSolutionsStandard(
                   const variable_list &Vars, 
                   const atermpp::aterm_appl &Expr, 
                   const bool not_equal_to_false, 
                   detail::EnumeratorStandard *enclosing_enumerator) :
      m_enclosing_enumerator(enclosing_enumerator),
      used_vars(0),
      max_vars(0)
    { 
      reset(Vars,Expr,not_equal_to_false);
    }

    ~EnumeratorSolutionsStandard()
    {}

    // returns a list of values for the variables given.
    bool next(atermpp::term_list<atermpp::aterm_appl> &solution);

    /// \details If not_equal_to_false is set to true, solution_is_exact returns whether the solution
    ///          made the condition true. If not_equal_to_false was set to false, it
    ///          indicates whether the last delivered solution made the solution true.
    ///          The complexity is constant.
    bool next(atermpp::term_list<atermpp::aterm_appl> &solution, bool &solution_is_exact);

    void reset(const variable_list &Vars, const atermpp::aterm_appl &Expr, const bool not_equal_to_false);

  private:
    bool FindInnerCEquality(const atermpp::aterm_appl T, 
                            const mcrl2::data::variable_list vars, 
                            mcrl2::data::variable &v, 
                            atermpp::aterm_appl &e);

    void EliminateVars(fs_expr &e);

    bool FindInnerCEquality_aux(const atermpp::aterm_appl t);

    atermpp::aterm_appl build_solution_single(
                 const atermpp::aterm_appl t,
                 const variable_list substituted_vars,
                 const atermpp::term_list < atermpp::aterm_appl> exprs) const;

    atermpp::term_list < atermpp::aterm_appl> build_solution(
                 const variable_list vars,
                 const variable_list substituted_vars,
                 const atermpp::term_list < atermpp::aterm_appl> exprs) const;

    atermpp::term_list < atermpp::aterm_appl> build_solution2(
                 const variable_list vars,
                 const variable_list substituted_vars,
                 const atermpp::term_list < atermpp::aterm_appl> exprs) const;
    atermpp::aterm_appl build_solution_aux_innerc(
                 const atermpp::aterm_appl t,
                 const variable_list substituted_vars,
                 const atermpp::term_list < atermpp::aterm_appl> exprs) const;
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
    t.substituted_vars().protect();
    t.vals().protect();
    t.expr().protect();
  }
  static void unprotect(const mcrl2::data::detail::fs_expr& t)
  {
    assert(0); // This is not being used. This is to check this.
    t.vars().unprotect();
    t.substituted_vars().unprotect();
    t.vals().unprotect();
    t.expr().unprotect();
  }
  static void mark(const mcrl2::data::detail::fs_expr& t)
  {
    t.vars().mark();
    t.substituted_vars().mark();
    t.vals().mark();
    t.expr().mark();
  }
};

template<>
struct aterm_traits<mcrl2::data::detail::ss_solution>
{
  static void protect(const mcrl2::data::detail::ss_solution& t)
  {
    assert(0); // This is not being used. This is to check this.
    t.solution().protect();
  }
  static void unprotect(const mcrl2::data::detail::ss_solution& t)
  {
    assert(0); // This is not being used. This is to check this.
    t.solution().unprotect();
  }
  static void mark(const mcrl2::data::detail::ss_solution& t)
  {
    t.solution().mark();
  }
};
} // namespace atermpp
/// \endcond

#endif
