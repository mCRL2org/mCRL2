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

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/enumerator_variable_limit.h"


#define MAX_VARS_INIT   1000
#define MAX_VARS_FACTOR 5

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
    data_expression_list m_solution;                   // A list containing a solution of a condition.
    data_expression m_evaluated_condition;             // The condition after substituting the solution.

  public:

    // Constructor.
    ss_solution(const data_expression_list& solution, const data_expression evaluated_condition) :
      m_solution(solution),
      m_evaluated_condition(evaluated_condition)
    {} 
   
    data_expression evaluated_condition() const
    { 
      return m_evaluated_condition;
    }

    data_expression_list solution() const
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
    data_expression_list m_vals;                       // Data expressions that are to be substituted
                                                       // in the variables in m_substituted_vars.
    data_expression m_expr;                            // data_expression to which internal variables
                                                       // must adhere.

  public:
    // Default constructor
    fs_expr()
    {}

    // Constructor
    fs_expr(
        const variable_list& vars, 
        const variable_list& substituted_vars, 
        const data_expression_list& vals, 
        const data_expression& expr):
       m_vars(vars), m_substituted_vars(substituted_vars),m_vals(vals), m_expr(expr)
    {
    }

    const variable_list& vars() const
    {
      return m_vars;
    }

    const variable_list& substituted_vars() const
    {
      return m_substituted_vars;
    }

    const data_expression_list& vals() const
    {
      return m_vals;
    }

    const data_expression& expr() const
    {
      return m_expr;
    }
};

/**
 * \brief Class for enumerating solutions to boolean expressions.
 **/

class EnumeratorSolutionsStandard 
{
  public:
    typedef Rewriter::substitution_type substitution_type;

  protected:

    const mcrl2::data::data_specification& m_data_spec;
    Rewriter* m_rewr_obj;

//    detail::EnumeratorStandard *m_enclosing_enumerator;
/*    data_expression desired_truth_value;    // We search for solutions for the condition enum_expr that are not
    data_expression forbidden_truth_value;  // equal to the forbidden truth value, and if the output matches the
                                                // desired truth value, then the variable solution_is_exact is set. */

    variable_list enum_vars;                    // The variables over which a solution is searched.
    data_expression enum_expr;              // Condition to be satisfied.
    substitution_type& enum_sigma;

    std::deque < fs_expr> fs_stack;
    std::vector< ss_solution > ss_stack;

    size_t used_vars;
    size_t max_vars;
    size_t m_max_internal_variables;

    substitution_type& default_sigma()
    {
      static substitution_type default_sigma;
      return default_sigma;
    }

    data_specification& default_data_spec()
    {
      static data_specification default_data_spec;
      return default_data_spec;
    }

  public:

    /// \brief Default constructor
    EnumeratorSolutionsStandard():
       m_data_spec(default_data_spec()),
       enum_sigma(default_sigma()),
       m_max_internal_variables(0)
    {
    } 

    /// \brief Constructor. Generate solutions for the variables in Vars that satisfy Expr.
    /// If not equal_to_false is set all solutions are generated that make Expr not equal to false.
    /// Otherwise all solutions are generated that make Expr not equal to true. The enumerator
    /// uses internal variables. By setting max_internal_variables to any value larger than
    /// 0 the number of these variables can be bound, guaranteeing termination (provided the
    /// rewriter terminates). If set to 0, there is no bound, but the enumerator will issue 
    /// warnings.
    /// \param[in] vars The list of variables for which solutions are being sought.
    /// \param[in] expr The expression which determine the solutions. This expr is assumed to be
    ///                 in rewrite normal form. 
    /// \param[in] not_equal_to_false Determine whether the condition is supposed to be true, or
    ///                               whether it should become false. If the condition must be
    ///                               true, all solutions that make the condition not equal to
    ///                               false are enumerated. 
    /// \param[in] enclosing_enumerator Pointer to the enclosing enumerator.
    /// \param[in] max_internal_variables Maximal number of internal variables that will be used
    ///                                   when generating solutions. If 0 there is no limit, and
    ///                                   the enumerator may not terminate.
    //  \param[in] expr_is_normal_form A boolean that indicates whether expr is in normal form. If
    //                                 not, the first action of the enumerator is to rewrite it in
    //                                 normal form, but sometimes this is known, and in such 
    //                                 a case rewriting can be avoided.

    EnumeratorSolutionsStandard(
                   const variable_list& vars, 
                   const data_expression& expr, 
                   substitution_type& sigma,
                   const bool not_equal_to_false, 
                   const mcrl2::data::data_specification& data_spec,
                   Rewriter* rewr_obj,
                   const size_t max_internal_variables=0,
                   const bool expr_is_normal_form=false) :
      m_data_spec(data_spec),
      m_rewr_obj(rewr_obj),
      enum_vars(vars),
      enum_expr(expr),
      enum_sigma(sigma),
      used_vars(0),
      max_vars(MAX_VARS_INIT),
      m_max_internal_variables(max_internal_variables)
    { 
      
      reset(not_equal_to_false,expr_is_normal_form);
    }

    /// Standard destructor.
    ~EnumeratorSolutionsStandard()
    {
    }
 
   /**
    * \brief Get next solution as a data_expression_list if available.
    * \param[out] evaluated_condition This optional parameter is used to return the
    *             condition in which solution is substituted. 
    * \param[out] solution Place to store the solutions.
    *             The data_expression_list solution contains solutions for the variables
    *             in the same order as the variable list Vars.
    * \param[out] solution_possible. This boolean indicates whether it was possible to
    *             generate a solution. If there is a variable of a sort without a constructor
    *             sort, it is not possible to generate solutions. Similarly, it can be
    *             that the maximum number of solutions has been reached. In this case the variable
    *             solution_possible is false, and the function returns false. 
    *             This variable should be true when calling next. If it is initially false, 
    *             or if a variant of next is used
    *             without this parameter, an mcrl2::runtime_error exception is thrown if no solutions exist, or
    *             if the maximum number of internal variables is reached.
    * \param[in]  max_internal_variables The maximum number of variables to be 
    *             used internally when generating solutions. If set to 0 an unbounded number
    *             of variables are used, and warnings are printed to warn for potentially
    *             unbounded loops.
    * \return Whether or not a solution was found and stored in
    *         solution. If false, there are no more solutions to be found. 
    *
    **/

    bool next(data_expression& evaluated_condition,
              data_expression_list& solution, 
              bool& solution_possible);

  /** \brief Get next solution as a data_expression_list.
   **/
    bool next(data_expression_list& solution);

  /** \brief Get next solution as a data_expression_list.
   **/
    bool next(data_expression& evaluated_condition,
              data_expression_list& solution);

  /** \brief Get next solution as a data_expression_list.
   **/
    bool next(data_expression_list& solution, 
              bool& solution_possible);


  private:
    void reset(const bool not_equal_to_false, const bool expr_is_normal_form); 

    bool find_equality(const data_expression& T, 
                            const mcrl2::data::variable_list& vars, 
                            mcrl2::data::variable& v, 
                            data_expression& e); 

    void EliminateVars(fs_expr& e);

    data_expression build_solution_single(
                 const data_expression& t,
                 variable_list substituted_vars,
                 data_expression_list exprs) const;

    data_expression_list build_solution(
                 const variable_list& vars,
                 const variable_list& substituted_vars,
                 const data_expression_list& exprs) const;

    data_expression_list build_solution2(
                 const variable_list& vars,
                 const variable_list& substituted_vars,
                 const data_expression_list& exprs) const;
    data_expression build_solution_aux(
                 const data_expression& t,
                 const variable_list& substituted_vars,
                 const data_expression_list& exprs) const;
    data_expression add_negations(
                 const data_expression& condition,
                 const data_expression_list& negation_term_list,
                 const bool negated) const;
    void push_on_fs_stack_and_split_or(
                 std::deque < fs_expr>& fs_stack,
                 const variable_list& var_list,
                 const variable_list& substituted_vars,
                 const data_expression_list& substitution_terms,
                 const data_expression& condition,
                 const data_expression_list& negated_term_list,
                 const bool negated) const;
    void push_on_fs_stack_and_split_or_without_rewriting(
                 std::deque < fs_expr>& fs_stack,
                 const variable_list& var_list,
                 const variable_list& substituted_vars,
                 const data_expression_list& substitution_terms,
                 const data_expression& condition,
                 const data_expression_list& negated_term_list,
                 const bool negated) const;
    data_expression_list negate(
                 const data_expression_list& l) const;
};
}
}
} // namespace atermpp
/// \endcond

#endif
