// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite.h

#ifndef __LIBREWRITE_H
#define __LIBREWRITE_H

#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/data/substitutions.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/set_identifier_generator.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

/**
 * \brief Rewriter interface class.
 *
 * This is the interface class for the rewriters. To create a specific
 * rewriter, use createRewriter.
 *
 * Simple use of the rewriter would be as follow (with t a term in the mCRL2
 * internal format):
 *
 * \code
 *   Rewriter *r = createRewriter(equations);
 *   t = r->rewrite(t);
 *   delete r;
 * \endcode
 **/
class Rewriter
{
  public:
    data::set_identifier_generator generator;  //name for variables.

    typedef mutable_indexed_substitution<data::variable, std::vector< data::data_expression > > substitution_type;

    used_data_equation_selector data_equation_selector;
    /**
     * \brief Constructor. Do not use directly; use createRewriter()
     *        function instead.
     * \sa createRewriter()
     **/
    Rewriter(const data_specification& data_spec, const used_data_equation_selector& eq_selector):
          data_equation_selector(eq_selector),
          m_data_specification_for_enumeration(data_spec)
    {
      generator.add_identifiers(data::find_identifiers(data_spec.equations()));
      generator.add_identifiers(data::find_identifiers(data_spec.sorts()));
      generator.add_identifiers(data::find_identifiers(data_spec.constructors()));
      generator.add_identifiers(data::find_identifiers(data_spec.mappings()));

    }

    /** \brief Destructor. */
    virtual ~Rewriter()
    {
    }

    /**
     * \brief Get rewriter strategy that is used.
     * \return Used rewriter strategy.
     **/
    virtual rewrite_strategy getStrategy() = 0;

    /**
     * \brief Rewrite an mCRL2 data term.
     * \param Term The term to be rewritten. This term should be a data_term
     * \return The normal form of Term.
     **/

    virtual data_expression rewrite(const data_expression& term, substitution_type& sigma) = 0;

    /**
     * \brief Rewrite a list of mCRL2 data terms.
     * \param Terms The list of terms to be rewritten. These terms
     *              should be in the internal mCRL2 format.
     * \return The list Terms where each element is replaced by its
     *         normal form.
     **/
    virtual data_expression_list rewrite_list(const data_expression_list& Terms, substitution_type& sigma);

  public:
  /* The functions below are public, because they are used in the compiling jitty rewriter */
    data_expression existential_quantifier_enumeration(
         const abstraction& termInInnerFormat,
         substitution_type& sigma);
    data_expression existential_quantifier_enumeration(
         const variable_list& vl,
         const data_expression& t1,
         const bool t1_is_normal_form,
         substitution_type& sigma);

    data_expression universal_quantifier_enumeration(
         const abstraction& termInInnerFormat,
         substitution_type& sigma);
    data_expression universal_quantifier_enumeration(
         const variable_list& vl,
         const data_expression& t1,
         const bool t1_is_normal_form,
         substitution_type& sigma);

    // Rewrite a where expression where the subdataexpressions are in internal format.
    // It yields a term without a where expression.
    data_expression rewrite_where(
                      const where_clause& term,
                      substitution_type& sigma);

    // Rewrite an expression with a lambda as outermost symbol. The expression is in internal format.
    // Bound variables are replaced by new variables to avoid a clash with variables in the right hand sides
    // of sigma.

    abstraction rewrite_single_lambda(
                      const variable_list& vl,
                      const data_expression& body,
                      const bool body_in_normal_form,
                      substitution_type& sigma);

    /// Rewrite t, assuming that the headsymbol of t, which can be nested, is a lambda term.
    data_expression rewrite_lambda_application(
                      const data_expression& t,
                      substitution_type& sigma);

    data_expression rewrite_lambda_application(
                      const abstraction& lambda_term,
                      const data_expression& body,
                      substitution_type& sigma);


  protected:

    const mcrl2::data::data_specification m_data_specification_for_enumeration;
    data_expression quantifier_enumeration(
         const data_expression& termInInnerFormat,
         substitution_type& sigma);

};

/**
 * \brief Create a rewriter.
 * \param DataSpec A data specification.
 * \param Strategy The rewrite strategy to be used by the rewriter.
 * \return A (pointer to a) rewriter that uses the data specification DataSpec
 *         and strategy Strategy to rewrite.
 **/
Rewriter* createRewriter(
             const data_specification& DataSpec,
             const used_data_equation_selector& equations_selector,
             const rewrite_strategy Strategy = jitty);

/**
 * \brief Check that an mCRL2 data equation is a valid rewrite rule. If not, an runtime_error is thrown indicating the problem.
 * \param DataEqn The mCRL2 data equation to be checked.
 * \throw std::runtime_error containing a reason why DataEqn is not a valid rewrite rule.
 **/
void CheckRewriteRule(const data_equation& dataeqn);

/**
 * \brief Check whether or not an mCRL2 data equation is a valid rewrite rule.
 * \param DataEqn The mCRL2 data equation to be checked.
 * \return Whether or not DataEqn is a valid rewrite rule.
 **/
bool isValidRewriteRule(const data_equation& dataeqn);


inline size_t getArity(const data::function_symbol& op)
{
  // This function calculates the cumulated length of all
  // potential function arguments.
  sort_expression sort = op.sort();
  size_t arity = 0;

  while (is_function_sort(sort))
  {
    const function_sort fsort(sort);
    sort_expression_list sort_dom = fsort.domain();
    arity += sort_dom.size();
    sort = fsort.codomain();
  }
  return arity;
} 

}
}
}

#endif
