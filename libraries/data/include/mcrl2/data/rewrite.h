// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data/include/mcrl2/data/rewrite.h
/// \brief add your file description here.

#ifndef DATA_INCLUDE_MCRL2_DATA_REWRITE_H
#define DATA_INCLUDE_MCRL2_DATA_REWRITE_H

#include "mcrl2/data/builder.h"

namespace mcrl2 {

namespace data {

namespace detail
{

template <template <class> class Builder, class Rewriter>
struct rewrite_data_expressions_builder: public Builder<rewrite_data_expressions_builder<Builder, Rewriter> >
{
  typedef Builder<rewrite_data_expressions_builder<Builder, Rewriter> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  Rewriter R;

  rewrite_data_expressions_builder(Rewriter R_)
    : R(R_)
  {}

  data_expression operator()(const data_expression& x)
  {
    return R(x);
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class> class Builder, class Rewriter>
rewrite_data_expressions_builder<Builder, Rewriter>
make_rewrite_data_expressions_builder(Rewriter R)
{
  return rewrite_data_expressions_builder<Builder, Rewriter>(R);
}

template <template <class> class Builder, class Rewriter, class Substitution>
struct rewrite_data_expressions_with_substitution_builder: public Builder<rewrite_data_expressions_with_substitution_builder<Builder, Rewriter, Substitution> >
{
  typedef Builder<rewrite_data_expressions_with_substitution_builder<Builder, Rewriter, Substitution> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  Rewriter R;
  Substitution sigma;

  rewrite_data_expressions_with_substitution_builder(Rewriter R_, Substitution sigma_)
    : R(R_),
      sigma(sigma_)
  {}

  data_expression operator()(const data_expression& x)
  {
    return R(x, sigma);
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class> class Builder, class Rewriter, class Substitution>
rewrite_data_expressions_with_substitution_builder<Builder, Rewriter, Substitution>
make_rewrite_data_expressions_with_substitution_builder(Rewriter R, Substitution sigma)
{
  return rewrite_data_expressions_with_substitution_builder<Builder, Rewriter, Substitution>(R, sigma);
}
/// \endcond

} // namespace detail

//--- start generated data rewrite code ---//
/// \brief Rewrites all embedded expressions in an object x
/// \param x an object containing expressions
/// \param R a rewriter
template <typename T, typename Rewriter>
void rewrite(T& x,
             Rewriter R,
             typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = 0
            )
{
  data::detail::make_rewrite_data_expressions_builder<data::data_expression_builder>(R)(x);
}

/// \brief Rewrites all embedded expressions in an object x
/// \param x an object containing expressions
/// \param R a rewriter
/// \return the rewrite result
template <typename T, typename Rewriter>
T rewrite(const T& x,
          Rewriter R,
          typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = 0
         )
{
  return core::static_down_cast<const T&>(data::detail::make_rewrite_data_expressions_builder<data::data_expression_builder>(R)(x));
}

/// \brief Rewrites all embedded expressions in an object x, and applies a substitution to variables on the fly
/// \param x an object containing expressions
/// \param R a rewriter
/// \param sigma a substitution
template <typename T, typename Rewriter, typename Substitution>
void rewrite(T& x,
             Rewriter R,
             const Substitution& sigma,
             typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = 0
            )
{
  data::detail::make_rewrite_data_expressions_with_substitution_builder<data::data_expression_builder>(R, sigma)(x);
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
          typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = 0
         )
{
  return core::static_down_cast<const T&>(data::detail::make_rewrite_data_expressions_with_substitution_builder<data::data_expression_builder>(R, sigma)(x));
}
//--- end generated data rewrite code ---//

} // namespace data

} // namespace mcrl2

#endif // DATA_INCLUDE_MCRL2_DATA_REWRITE_H
