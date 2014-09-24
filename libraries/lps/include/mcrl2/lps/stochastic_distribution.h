// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/stochastic_distribution.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_STOCHASTIC_DISTRIBUTION_H
#define MCRL2_LPS_STOCHASTIC_DISTRIBUTION_H

namespace mcrl2 {

namespace lps {

//--- start generated class stochastic_distribution ---//
/// \brief A stochastic distribution
class stochastic_distribution: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    stochastic_distribution()
      : atermpp::aterm_appl(core::detail::default_values::Distribution)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit stochastic_distribution(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_Distribution(*this));
    }

    /// \brief Constructor.
    stochastic_distribution(const data::data_expression& distribution, const data::variable_list& variables)
      : atermpp::aterm_appl(core::detail::function_symbol_Distribution(), distribution, variables)
    {}

    const data::data_expression& distribution() const
    {
      return atermpp::down_cast<data::data_expression>((*this)[0]);
    }

    const data::variable_list& variables() const
    {
      return atermpp::down_cast<data::variable_list>((*this)[1]);
    }
//--- start user section stochastic_distribution ---//
    /// \brief Returns true if the distribution is undefined, i.e. it contains no valid distribution.
    /// This is encoded by an empty list of variables.
    bool is_undefined() const
    {
      return variables().empty();
    }
//--- end user section stochastic_distribution ---//
};

/// \brief list of stochastic_distributions
typedef atermpp::term_list<stochastic_distribution> stochastic_distribution_list;

/// \brief vector of stochastic_distributions
typedef std::vector<stochastic_distribution>    stochastic_distribution_vector;

/// \brief Test for a stochastic_distribution expression
/// \param x A term
/// \return True if \a x is a stochastic_distribution expression
inline
bool is_stochastic_distribution(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::Distribution;
}

// prototype declaration
std::string pp(const stochastic_distribution& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const stochastic_distribution& x)
{
  return out << lps::pp(x);
}

/// \brief swap overload
inline void swap(stochastic_distribution& t1, stochastic_distribution& t2)
{
  t1.swap(t2);
}
//--- end generated class stochastic_distribution ---//

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_STOCHASTIC_DISTRIBUTION_H
