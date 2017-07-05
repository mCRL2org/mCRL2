// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps/include/mcrl2/modal_formula/rewrite.h
/// \brief add your file description here.

#ifndef LPS_INCLUDE_MCRL2_MODAL_FORMULA_REWRITE_H
#define LPS_INCLUDE_MCRL2_MODAL_FORMULA_REWRITE_H

#include "mcrl2/data/rewrite.h"
#include "mcrl2/modal_formula/builder.h"

namespace mcrl2 {

namespace action_formulas {

//--- start generated action_formulas rewrite code ---//
/// \brief Rewrites all embedded expressions in an object x
/// \param x an object containing expressions
/// \param R a rewriter
template <typename T, typename Rewriter>
void rewrite(T& x,
             Rewriter R,
             typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
            )
{
  data::detail::make_rewrite_data_expressions_builder<action_formulas::data_expression_builder>(R).update(x);
}

/// \brief Rewrites all embedded expressions in an object x
/// \param x an object containing expressions
/// \param R a rewriter
/// \return the rewrite result
template <typename T, typename Rewriter>
T rewrite(const T& x,
          Rewriter R,
          typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
         )
{
  return data::detail::make_rewrite_data_expressions_builder<action_formulas::data_expression_builder>(R).apply(x);
}

/// \brief Rewrites all embedded expressions in an object x, and applies a substitution to variables on the fly
/// \param x an object containing expressions
/// \param R a rewriter
/// \param sigma a substitution
template <typename T, typename Rewriter, typename Substitution>
void rewrite(T& x,
             Rewriter R,
             const Substitution& sigma,
             typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
            )
{
  data::detail::make_rewrite_data_expressions_with_substitution_builder<action_formulas::data_expression_builder>(R, sigma).update(x);
}

/// \brief Rewrites all embedded expressions in an object x, and applies a substitution to variables on the fly
/// \param x an object containing expressions
/// \param R a rewriter
/// \param sigma a substitution
/// \return the rewrite result
template <typename T, typename Rewriter, typename Substitution>
T rewrite(const T& x,
          Rewriter R,
          const Substitution& sigma,
          typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
         )
{
  return data::detail::make_rewrite_data_expressions_with_substitution_builder<action_formulas::data_expression_builder>(R, sigma).apply(x);
}
//--- end generated action_formulas rewrite code ---//

} // namespace action_formulas

namespace regular_formulas {

//--- start generated regular_formulas rewrite code ---//
/// \brief Rewrites all embedded expressions in an object x
/// \param x an object containing expressions
/// \param R a rewriter
template <typename T, typename Rewriter>
void rewrite(T& x,
             Rewriter R,
             typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
            )
{
  data::detail::make_rewrite_data_expressions_builder<regular_formulas::data_expression_builder>(R).update(x);
}

/// \brief Rewrites all embedded expressions in an object x
/// \param x an object containing expressions
/// \param R a rewriter
/// \return the rewrite result
template <typename T, typename Rewriter>
T rewrite(const T& x,
          Rewriter R,
          typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
         )
{
  return data::detail::make_rewrite_data_expressions_builder<regular_formulas::data_expression_builder>(R).apply(x);
}

/// \brief Rewrites all embedded expressions in an object x, and applies a substitution to variables on the fly
/// \param x an object containing expressions
/// \param R a rewriter
/// \param sigma a substitution
template <typename T, typename Rewriter, typename Substitution>
void rewrite(T& x,
             Rewriter R,
             const Substitution& sigma,
             typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
            )
{
  data::detail::make_rewrite_data_expressions_with_substitution_builder<regular_formulas::data_expression_builder>(R, sigma).update(x);
}

/// \brief Rewrites all embedded expressions in an object x, and applies a substitution to variables on the fly
/// \param x an object containing expressions
/// \param R a rewriter
/// \param sigma a substitution
/// \return the rewrite result
template <typename T, typename Rewriter, typename Substitution>
T rewrite(const T& x,
          Rewriter R,
          const Substitution& sigma,
          typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
         )
{
  return data::detail::make_rewrite_data_expressions_with_substitution_builder<regular_formulas::data_expression_builder>(R, sigma).apply(x);
}
//--- end generated regular_formulas rewrite code ---//

} // namespace regular_formulas

namespace state_formulas {

//--- start generated state_formulas rewrite code ---//
/// \brief Rewrites all embedded expressions in an object x
/// \param x an object containing expressions
/// \param R a rewriter
template <typename T, typename Rewriter>
void rewrite(T& x,
             Rewriter R,
             typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
            )
{
  data::detail::make_rewrite_data_expressions_builder<state_formulas::data_expression_builder>(R).update(x);
}

/// \brief Rewrites all embedded expressions in an object x
/// \param x an object containing expressions
/// \param R a rewriter
/// \return the rewrite result
template <typename T, typename Rewriter>
T rewrite(const T& x,
          Rewriter R,
          typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
         )
{
  return data::detail::make_rewrite_data_expressions_builder<state_formulas::data_expression_builder>(R).apply(x);
}

/// \brief Rewrites all embedded expressions in an object x, and applies a substitution to variables on the fly
/// \param x an object containing expressions
/// \param R a rewriter
/// \param sigma a substitution
template <typename T, typename Rewriter, typename Substitution>
void rewrite(T& x,
             Rewriter R,
             const Substitution& sigma,
             typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
            )
{
  data::detail::make_rewrite_data_expressions_with_substitution_builder<state_formulas::data_expression_builder>(R, sigma).update(x);
}

/// \brief Rewrites all embedded expressions in an object x, and applies a substitution to variables on the fly
/// \param x an object containing expressions
/// \param R a rewriter
/// \param sigma a substitution
/// \return the rewrite result
template <typename T, typename Rewriter, typename Substitution>
T rewrite(const T& x,
          Rewriter R,
          const Substitution& sigma,
          typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
         )
{
  return data::detail::make_rewrite_data_expressions_with_substitution_builder<state_formulas::data_expression_builder>(R, sigma).apply(x);
}
//--- end generated state_formulas rewrite code ---//

} // namespace state_formulas

} // namespace mcrl2

#endif // LPS_INCLUDE_MCRL2_MODAL_FORMULA_REWRITE_H
