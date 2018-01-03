// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/function_sort.h
/// \brief The class function_sort.

#ifndef MCRL2_DATA_FUNCTION_SORT_H
#define MCRL2_DATA_FUNCTION_SORT_H

#include <iostream>

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/sort_expression.h"

namespace mcrl2
{

namespace data
{

//--- start generated class function_sort ---//
/// \brief A function sort
class function_sort: public sort_expression
{
  public:
    /// \brief Default constructor.
    function_sort()
      : sort_expression(core::detail::default_values::SortArrow)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit function_sort(const atermpp::aterm& term)
      : sort_expression(term)
    {
      assert(core::detail::check_term_SortArrow(*this));
    }

    /// \brief Constructor.
    function_sort(const sort_expression_list& domain, const sort_expression& codomain)
      : sort_expression(atermpp::aterm_appl(core::detail::function_symbol_SortArrow(), domain, codomain))
    {}

    /// \brief Constructor.
    template <typename Container>
    function_sort(const Container& domain, const sort_expression& codomain, typename atermpp::enable_if_container<Container, sort_expression>::type* = nullptr)
      : sort_expression(atermpp::aterm_appl(core::detail::function_symbol_SortArrow(), sort_expression_list(domain.begin(), domain.end()), codomain))
    {}

    const sort_expression_list& domain() const
    {
      return atermpp::down_cast<sort_expression_list>((*this)[0]);
    }

    const sort_expression& codomain() const
    {
      return atermpp::down_cast<sort_expression>((*this)[1]);
    }
};

// prototype declaration
std::string pp(const function_sort& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const function_sort& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(function_sort& t1, function_sort& t2)
{
  t1.swap(t2);
}
//--- end generated class function_sort ---//

/// \brief list of function sorts
typedef atermpp::term_list<function_sort> function_sort_list;
/// \brief vector of function sorts
typedef std::vector<function_sort> function_sort_vector;

/// \brief Convenience constructor for function sort with domain size 1
///
/// \param[in] dom1 The first sort of the domain.
/// \param[in] codomain The codomain of the sort.
/// \post *this represents dom1 -> codomain
inline function_sort make_function_sort(const sort_expression& dom1,
                                        const sort_expression& codomain)
{
  return function_sort({ dom1 }, codomain);
}

/// \brief Convenience constructor for function sort with domain size 2
///
/// \param[in] dom1 The first sort of the domain.
/// \param[in] dom2 The second sort of the domain.
/// \param[in] codomain The codomain of the sort.
/// \post *this represents dom1 # dom2 -> codomain
inline function_sort make_function_sort(const sort_expression& dom1,
                                        const sort_expression& dom2,
                                        const sort_expression& codomain)
{
  return function_sort({ dom1, dom2 }, codomain);
}

/// \brief Convenience constructor for function sort with domain size 3
///
/// \param[in] dom1 The first sort of the domain.
/// \param[in] dom2 The second sort of the domain.
/// \param[in] dom3 The third sort of the domain.
/// \param[in] codomain The codomain of the sort.
/// \post *this represents dom1 # dom2 # dom3 -> codomain
inline function_sort make_function_sort(const sort_expression& dom1,
                                        const sort_expression& dom2,
                                        const sort_expression& dom3,
                                        const sort_expression& codomain)
{
  return function_sort({ dom1, dom2, dom3 }, codomain);
}

/// \brief Convenience constructor for function sort with domain size 4
///
/// \param[in] dom1 The first sort of the domain.
/// \param[in] dom2 The second sort of the domain.
/// \param[in] dom3 The third sort of the domain.
/// \param[in] dom4 The fourth sort of the domain.
/// \param[in] codomain The codomain of the sort.
/// \post *this represents dom1 # dom2 # dom3 # dom4 -> codomain
inline function_sort make_function_sort(const sort_expression& dom1,
                                        const sort_expression& dom2,
                                        const sort_expression& dom3,
                                        const sort_expression& dom4,
                                        const sort_expression& codomain)
{
  return function_sort({ dom1, dom2, dom3, dom4 }, codomain);
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FUNCTION_SORT_H

