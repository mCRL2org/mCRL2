// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/rewriter.h
/// \brief The class rewriter.

#ifndef MCRL2_DATA_REWRITER_H
#define MCRL2_DATA_REWRITER_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "boost/shared_ptr.hpp"
#include "boost/type_traits/add_reference.hpp"

#include "mcrl2/exception.h"
#include "mcrl2/data/expression_traits.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/data_expression_with_variables.h"
#include "mcrl2/data/detail/data_expression_with_variables_traits.h"
#include "mcrl2/data/detail/rewrite_conversion_helper.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/find.h"

namespace mcrl2
{

namespace data
{

/// \brief Rewriter class for the mCRL2 Library. It only works for terms of type data_expression
/// and data_expression_with_variables.
template < typename Term >
class basic_rewriter
{
  protected:
    /// \brief The wrapped Rewriter.
    boost::shared_ptr<detail::Rewriter> m_rewriter;

  public:

    /// \brief The term type of the rewriter.
    typedef Term term_type;

    /// \brief The type for expressions manipulated by the rewriter.
    typedef Term expression_type;

    /// \brief The strategy of the rewriter.
    enum strategy
    {
// Disable inner and innerp rewriters due to different internal format. As they are hardly used,
// this saves on maintenance.
      jitty                      = detail::GS_REWR_JITTY   ,  /** \brief JITty */
#ifdef MCRL2_JITTYC_AVAILABLE
      jitty_compiling            = detail::GS_REWR_JITTYC  ,  /** \brief Compiling JITty */
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
      jitty_prover               = detail::GS_REWR_JITTY_P ,  /** \brief JITty + Prover */
      jitty_compiling_prover     = detail::GS_REWR_JITTYC_P   /** \brief Compiling JITty + Prover*/
#else
      jitty_prover               = detail::GS_REWR_JITTY_P    /** \brief JITty + Prover */
#endif
    };

  protected:

    /// \brief Constructor.
    /// \param[in] r A rewriter
    basic_rewriter(const boost::shared_ptr<detail::Rewriter> & r) :
      m_rewriter(r)
    {}

    /// \brief Copy Constructor
    basic_rewriter(const basic_rewriter &other) :
      m_rewriter(other.m_rewriter)
    {}

    /// \brief Constructor.
    basic_rewriter(const data_specification & d, const strategy s = jitty, const bool add_rewrite_rules=true) :
      m_rewriter(detail::createRewriter(d, static_cast< detail::RewriteStrategy >(s),add_rewrite_rules))
    {}

  public:

    /// \brief Adds an equation to the rewrite rules.
    /// \param[in] eq The equation that is added.
    /// \return Returns true if the operation succeeded.
    bool add_rule(const data_equation& eq)
    {
      return m_rewriter->addRewriteRule(eq);
    }

    /// \brief Removes an equation from the rewrite rules.
    /// \param[in] eq The equation that is removed.
    void remove_rule(const data_equation& eq)
    {
      m_rewriter->removeRewriteRule(eq);
    }

};

/// \brief Rewriter class for the mCRL2 Library. It only works for terms of type data_expression
/// and data_expression_with_variables.
template <>
class basic_rewriter< data_expression > : public basic_rewriter< atermpp::aterm >
{
    template < typename CompatibleExpression >
    friend class basic_rewriter;
    friend class enumerator;

  public:
    /// \brief The term type of the rewriter.
    typedef data_expression                                     expression_type;
    /// \brief The variable type of the rewriter.
    typedef core::term_traits< expression_type >::variable_type variable_type;

  protected:

    /// \brief for data implementation/reconstruction
    mutable boost::shared_ptr< detail::rewrite_conversion_helper >   m_conversion_helper;

  protected:

    /// \brief Copy constructor for conversion between derived types
    template < typename CompatibleExpression >
    basic_rewriter(const basic_rewriter< CompatibleExpression > & other) :
      basic_rewriter< atermpp::aterm >(other),
      m_conversion_helper(other.m_conversion_helper)
    { }

    /// \brief Performs data implementation before rewriting (should become obsolete)
    /// \param[in] expression an expression.
    template < typename Expression >
    Expression implement(Expression const& expression) const
    {
      return m_conversion_helper->implement(expression);
    }

    /// \brief Performs data reconstruction after rewriting (should become obsolete)
    data_expression reconstruct(atermpp::aterm_appl const& expression) const
    {
      return m_conversion_helper->lazy_reconstruct(expression);
    }

  public:

    /// \brief Constructor.
    /// \param[in] r A rewriter
    basic_rewriter(const basic_rewriter & other) :
      basic_rewriter< atermpp::aterm >(other),
      m_conversion_helper(other.m_conversion_helper)
    { }

    /// \brief Constructor.
    /// \param[in] d A data specification
    /// \param[in] s A rewriter strategy.
    basic_rewriter(const data_specification& d, const strategy s = jitty) :
      basic_rewriter< atermpp::aterm >(d,s,false),
      m_conversion_helper(new detail::rewrite_conversion_helper(d, *m_rewriter))
    { }

    /// \brief Constructor.
    /// \param[in] d A data specification
    /// \param[in] s A rewriter strategy.
    /// \param[in] selsctor A component that selects the equations that are converted to rewrite rules
    template < typename EquationSelector >
    basic_rewriter(const data_specification& d, EquationSelector const& selector, const strategy s = jitty) :
      basic_rewriter< atermpp::aterm >(d,s,false),
      m_conversion_helper(new detail::rewrite_conversion_helper(d, *m_rewriter, selector))
    { }

    /// \brief Adds an equation to the rewrite rules.
    /// \param[in] equation The equation that is added.
    /// \return Returns true if the operation succeeded.
    bool add_rule(const data_equation& equation)
    {
      return m_rewriter->addRewriteRule(const_cast< basic_rewriter const* >(this)->implement(equation));
    }
};

/// \brief Rewriter that operates on data expressions.
//
/// \attention As long as normalisation of sorts remains necessary, the data
/// specification object used for construction *must* exist during the
/// lifetime of the rewriter object.

class rewriter: public basic_rewriter<data_expression>
{
  public:
    /// \brief Constructor.
    /// \param[in] r a rewriter.
    rewriter(const rewriter& r) :
      basic_rewriter<data_expression>(r)
    { }

    /// \brief Constructor.
    /// \param[in] d A data specification
    /// \param[in] s A rewriter strategy.
    rewriter(const data_specification& d = rewriter::default_specification(), const strategy s = jitty) :
      basic_rewriter<data_expression>(d, s)
    { }

    /// \brief Constructor.
    /// \param[in] d A data specification
    /// \param[in] selsctor A component that selects the equations that are converted to rewrite rules
    /// \param[in] s A rewriter strategy.
    template < typename EquationSelector >
    rewriter(const data_specification& d, const EquationSelector& selector, const strategy s = jitty) :
      basic_rewriter<data_expression>(d, selector, s)
    {
    }

    /// \brief Default specification used if no specification is specified at construction
    /// \param[in] d A data specification
    static data_specification& default_specification()
    {
      static data_specification specification;
      return specification;
    }

    /// \brief Rewrites a data expression.
    /// \param[in] d A data expression
    /// \return The normal form of d.
    data_expression operator()(const data_expression& d) const
    {
      mutable_map_substitution<> sigma;
#ifdef MCRL2_PRINT_REWRITE_STEPS
      std::cerr << "REWRITE: " << d;
#endif 
      data_expression result(reconstruct(m_rewriter->rewrite(implement(d),sigma)));

#ifdef MCRL2_PRINT_REWRITE_STEPS
      std::cerr << " ------------> " << result << std::endl;
#endif 
      return result;
    }

    /// \brief Rewrites the data expression d, and on the fly applies a substitution function
    /// to data variables.
    /// \param[in] d A data expression
    /// \param[in] sigma A substitution function
    /// \return The normal form of the term.
    template <typename SubstitutionFunction>
    data_expression operator()(const data_expression& d, const SubstitutionFunction& sigma) const
    {
# ifdef MCRL2_PRINT_REWRITE_STEPS
      std::cerr << "REWRITE " << d << "\n";
#endif
      mutable_map_substitution<> fresh_sigma; // Temporary hack to make an empty subsitution.
      data_expression result(reconstruct(m_rewriter->rewrite(implement(data::replace_free_variables(d, sigma)),fresh_sigma)));

std::cerr << "Check whether the code fragment below without implement works. Todo for JFG\n";
      // The code fragment below is much more efficient, provided the substitued values are already in 
      // normal form. In that case, the implement is also not required.
      // Unfortunately, as substitutions do not always have a const_iterator on board, the
      // code does not work.
      /* for(typename SubstitutionFunction::const_iterator i=sigma.begin(); i!=sigma.end(); ++i)
      {
        fresh_sigma[i->first]=implement(i->second);
      }

      data_expression result(reconstruct(m_rewriter->rewrite(implement(d),fresh_sigma)));
      */
# ifdef MCRL2_PRINT_REWRITE_STEPS
      std::cerr << " ------------> " << result << std::endl;
#endif
      return result;
    }
};

/// \brief Rewriter that operates on data expressions.
class rewriter_with_variables: public basic_rewriter<data_expression>
{
    /// \brief The term type of the rewriter.
    typedef data_expression term_type;

    /// \brief The variable type of the rewriter.
    typedef core::term_traits< data_expression_with_variables >::variable_type variable_type;

  public:

    /// \brief Constructor. The Rewriter object that is used internally will be shared with \p r.
    /// \param[in] r A data rewriter
    rewriter_with_variables(const basic_rewriter< data_expression >& r) :
      basic_rewriter<data_expression>(r)
    {}

    /// \brief Constructor.
    /// \param[in] d A data specification
    /// \param[in] s A rewriter strategy.
    rewriter_with_variables(const data_specification& d = rewriter::default_specification(), const strategy s = jitty) :
      basic_rewriter<data_expression>(d, s)
    { }

    /// \brief Constructor.
    /// \param[in] d A data specification
    /// \param[in] selsctor A component that selects the equations that are converted to rewrite rules
    /// \param[in] s A rewriter strategy.
    template < typename EquationSelector >
    rewriter_with_variables(const data_specification& d, const EquationSelector & selector, const strategy s = jitty) :
      basic_rewriter<data_expression>(d, selector, s)
    { }


    /// \brief Rewrites a data expression.
    /// \param[in] d The term to be rewritten.
    /// \return The normal form of d.
    data_expression_with_variables operator()(const data_expression_with_variables& d) const
    {
      mutable_map_substitution<atermpp::map < variable,data_expression> > sigma; 
      data_expression t = reconstruct(m_rewriter->rewrite(implement(d),sigma));
      data_expression_with_variables result(t, find_free_variables(t));
#ifdef MCRL2_PRINT_REWRITE_STEPS
      std::cerr << "REWRITE " << d << " ------------> " << result << std::endl;
#endif
      return result;
    }

    /// \brief Rewrites the data expression d, and on the fly applies a substitution function
    /// to data variables.
    /// \param[in] d A term.
    /// \param[in] sigma A substitution function
    /// \return The normal form of the term.
    template <typename SubstitutionFunction>
    data_expression_with_variables operator()(const data_expression_with_variables& d, const SubstitutionFunction& sigma) const
    {
      data_expression t = this->operator()(replace_free_variables(static_cast< const data_expression& >(d), sigma));
      data_expression_with_variables result(t, find_free_variables(t));
#ifdef MCRL2_PRINT_REWRITE_STEPS
      std::cerr << "REWRITE " << d << " ------------> " << result << std::endl;
#endif
      return result;
    }
};


/// \brief Function object for converting a data rewriter into a rewriter that
/// applies a substitution.
template <typename DataRewriter, typename Substitution>
struct rewriter_adapter
{
  const DataRewriter& R_;
  const Substitution& sigma_;

  rewriter_adapter(const DataRewriter& R, const Substitution& sigma)
    : R_(R), sigma_(sigma)
  {}

  data::data_expression operator()(const data::data_expression& t) const
  {
    return R_(t, sigma_);
  }
};

/// \brief standard conversion from stream to rewrite strategy
inline std::istream& operator>>(std::istream& is, data::rewriter::strategy& s)
{
  char strategy[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  using namespace mcrl2::data::detail;

  is.readsome(strategy, 9);

  size_t new_s = static_cast< size_t >(RewriteStrategyFromString(strategy));

  s = static_cast< data::rewriter::strategy >(new_s);

  if (static_cast< size_t >(new_s) == static_cast< size_t >(GS_REWR_INVALID))
  {
    is.setstate(std::ios_base::failbit);
  }

  return is;
}

/// \brief standard conversion from rewrite strategy to stream
inline std::ostream& operator<<(std::ostream& os, data::rewriter::strategy& s)
{
  static char const* strategies[] =
  {
    "jitty",
#ifdef MCRL2_JITTYC_AVAILABLE
    "jittyc",
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
    "jittyp"
#else
    "jittyp",
    "jittypc"
#endif
  };

  os << strategies[s];

  return os;
}

/// \brief Pretty prints a rewrite strategy
/// \param[in] s A rewrite strategy.
inline std::string pp(const mcrl2::data::basic_rewriter< mcrl2::data::data_expression >::strategy s)
{
  switch (s)
  {
    case mcrl2::data::basic_rewriter< mcrl2::data::data_expression >::jitty:
      return "jitty";
      break;
#ifdef MCRL2_JITTYC_AVAILABLE
    case mcrl2::data::basic_rewriter< mcrl2::data::data_expression >::jitty_compiling:
      return "jittyc";
      break;
#endif
    case mcrl2::data::basic_rewriter< mcrl2::data::data_expression >::jitty_prover:
      return "jittyp";
      break;
#ifdef MCRL2_JITTYC_AVAILABLE
    case mcrl2::data::basic_rewriter< mcrl2::data::data_expression >::jitty_compiling_prover:
      return "jittycp";
      break;
#endif
    default:
      return "unknown";
  }
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REWRITER_H
