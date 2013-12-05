// Author(s): Jeroen Keiren, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/structured_sort_constructor_argument.h
/// \brief The class structured_sort_constructor_arguments.

#ifndef MCRL2_DATA_STRUCTURED_SORT_CONSTRUCTOR_ARGUMENT_H
#define MCRL2_DATA_STRUCTURED_SORT_CONSTRUCTOR_ARGUMENT_H

#include <string>
#include <iterator>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/sort_expression.h"

namespace mcrl2
{

namespace data
{

//--- start generated class structured_sort_constructor_argument ---//
/// \brief An argument of a constructor of a structured sort
class structured_sort_constructor_argument: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    structured_sort_constructor_argument()
      : atermpp::aterm_appl(core::detail::constructStructProj())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit structured_sort_constructor_argument(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_StructProj(*this));
    }

    /// \brief Constructor.
    structured_sort_constructor_argument(const core::identifier_string& name, const sort_expression& sort)
      : atermpp::aterm_appl(core::detail::function_symbol_StructProj(), name, sort)
    {}

    /// \brief Constructor.
    structured_sort_constructor_argument(const std::string& name, const sort_expression& sort)
      : atermpp::aterm_appl(core::detail::function_symbol_StructProj(), core::identifier_string(name), sort)
    {}

    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>((*this)[0]);
    }

    const sort_expression& sort() const
    {
      return atermpp::aterm_cast<const sort_expression>((*this)[1]);
    }
//--- start user section structured_sort_constructor_argument ---//
    /// \brief Constructor
    ///
    /// \param[in] sort The sort of the argument.
    structured_sort_constructor_argument(const sort_expression& sort)
      : atermpp::aterm_appl(core::detail::gsMakeStructProj(core::empty_identifier_string(), sort))
    {}

    /// \brief Constructor.
    ///
    /// \overload to work around problem that MSVC reinterprets char* or char[] as core::identifier_string
    template < size_t S >
    structured_sort_constructor_argument(const char(&name)[S], const sort_expression& sort)
      : atermpp::aterm_appl(core::detail::gsMakeStructProj(core::identifier_string(name), sort))
    {}
//--- end user section structured_sort_constructor_argument ---//
};

/// \brief list of structured_sort_constructor_arguments
typedef atermpp::term_list<structured_sort_constructor_argument> structured_sort_constructor_argument_list;

/// \brief vector of structured_sort_constructor_arguments
typedef std::vector<structured_sort_constructor_argument>    structured_sort_constructor_argument_vector;

/// \brief Test for a structured_sort_constructor_argument expression
/// \param x A term
/// \return True if \a x is a structured_sort_constructor_argument expression
inline
bool is_structured_sort_constructor_argument(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsStructProj(x);
}

// prototype declaration
std::string pp(const structured_sort_constructor_argument& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const structured_sort_constructor_argument& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(structured_sort_constructor_argument& t1, structured_sort_constructor_argument& t2)
{
  t1.swap(t2);
}
//--- end generated class structured_sort_constructor_argument ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_STRUCTURED_SORT_CONSTUCTOR_ARGUMENT_H

