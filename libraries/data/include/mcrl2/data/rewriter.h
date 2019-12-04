// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/rewriter.h
/// \brief The class rewriter.

#ifndef MCRL2_DATA_REWRITER_H
#define MCRL2_DATA_REWRITER_H

#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/expression_traits.h"

namespace mcrl2
{

namespace data
{

/// \brief Rewriter class for the mCRL2 Library. It only works for terms of type data_expression
/// and data_expression_with_variables.
template < typename Term >
class basic_rewriter
{
  public:
    /// \brief The type for the substitution that is used internally.
    typedef data::mutable_indexed_substitution<> substitution_type;

  protected:
    /// \brief The wrapped Rewriter.
    std::shared_ptr<detail::Rewriter> m_rewriter;

  public:

    /// \brief The type for expressions manipulated by the rewriter.
    typedef Term term_type;

    /// \brief The rewrite strategies of the rewriter.
    typedef rewrite_strategy strategy;

  protected:

    /// \brief Constructor.
    /// \param[in] r A rewriter
    explicit basic_rewriter(const std::shared_ptr<detail::Rewriter>& r) :
      m_rewriter(r)
    {}

    /// \brief Copy Constructor
    basic_rewriter(const basic_rewriter& other)=default;

    /// \brief Assignment operator
    basic_rewriter& operator=(const basic_rewriter& other)=default;

    /// \brief Constructor.
    /// \param[in] d A data specification
    /// \param[in] s A rewriter strategy.
    explicit basic_rewriter(const data_specification& d, const strategy s = jitty) :
      m_rewriter(detail::createRewriter(d, used_data_equation_selector(d), static_cast< rewrite_strategy >(s)))
    { }

    /// \brief Constructor.
    basic_rewriter(const data_specification& d, const used_data_equation_selector& equation_selector, const strategy s = jitty) :
      m_rewriter(detail::createRewriter(d, equation_selector, static_cast< rewrite_strategy >(s)))
    {}

};

/// \brief Rewriter that operates on data expressions.
//
/// \attention As long as normalisation of sorts remains necessary, the data
/// specification object used for construction *must* exist during the
/// lifetime of the rewriter object.

class rewriter: public basic_rewriter<data_expression>
{
  protected:
    // cache the empty substitution, since it is expensive to construct
    static substitution_type& empty_substitution()
    {
      static substitution_type result;
      assert(result.empty());    // This static substitution should always become empty again after use.
      return result;
    }

    /// \brief Default specification used if no specification is specified at construction
    static const data_specification& default_specification()
    {
      static data_specification specification;
      return specification;
    }

#ifdef MCRL2_COUNT_DATA_REWRITE_CALLS
    mutable std::size_t rewrite_calls = 0;
#endif

  public:
    typedef basic_rewriter<data_expression>::substitution_type substitution_type;

    /// \brief Constructor.
    /// \param[in] r a rewriter.
    rewriter(const rewriter& r) = default;

    /// \brief Constructor.
    /// \param[in] d A data specification
    /// \param[in] s A rewriter strategy.
    explicit rewriter(const data_specification& d = rewriter::default_specification(), const strategy s = jitty) :
      basic_rewriter<data_expression>(d, s)
    { }

    /// \brief Constructor.
    /// \param[in] d A data specification
    /// \param[in] selector A component that selects the equations that are converted to rewrite rules
    /// \param[in] s A rewriter strategy.
    template < typename EquationSelector >
    rewriter(const data_specification& d, const EquationSelector& selector, const strategy s = jitty) :
      basic_rewriter<data_expression>(d, selector, s)
    {
    }

    /// \brief Rewrites a data expression.
    /// \param[in] d A data expression
    /// \return The normal form of d.
    data_expression operator()(const data_expression& d) const
    {
      return (*this)(d, empty_substitution());
    }

    /// \brief Rewrites the data expression d, and on the fly applies a substitution function
    /// to data variables.
    /// \param[in] d A data expression
    /// \param[in] sigma A substitution function
    /// \return The normal form of the term.
    template <typename SubstitutionFunction>
    data_expression operator()(const data_expression& d, const SubstitutionFunction& sigma) const
    {
      substitution_type sigma_with_iterator;
      std::set<variable> free_variables = data::find_free_variables(d);
      for(const variable& free_variable: free_variables)
      {
        sigma_with_iterator[free_variable] = sigma(free_variable);
      }
      return (*this)(d, sigma_with_iterator);
    }

    /// \brief Rewrites the data expression d, and on the fly applies a substitution function
    /// to data variables.
    /// \param[in] d A data expression
    /// \param[in] sigma A substitution function
    /// \return The normal form of the term.
    //  Added bij JFG, to avoid the use of find_free_variables in the function operator() with
    //  an arbitrary SubstitionFunction, as this is prohibitively costly.

    data_expression operator()(const data_expression& d, substitution_type& sigma) const
    {
#ifdef MCRL2_COUNT_DATA_REWRITE_CALLS
      rewrite_calls++;
#endif
# ifdef MCRL2_PRINT_REWRITE_STEPS
      mCRL2log(log::debug) << "REWRITE " << d << "\n";
      data_expression result(m_rewriter->rewrite(d,sigma));
      mCRL2log(log::debug) << " ------------> " << result << std::endl;
      return result;
#else
      return m_rewriter->rewrite(d,sigma);
#endif
    }

    ~rewriter()
    {
#ifdef MCRL2_COUNT_DATA_REWRITE_CALLS
      std::cout << "number of data rewrite calls: " << rewrite_calls << std::endl;
#endif
    }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REWRITER_H
