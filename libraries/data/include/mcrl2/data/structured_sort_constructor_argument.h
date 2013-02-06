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
#include "mcrl2/atermpp/aterm_access.h"
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

namespace detail
{

//--- start generated class structured_sort_constructor_argument_base ---//
//--- end generated class structured_sort_constructor_argument_base ---//

/// \brief An argument of a constructor of a structured sort
class structured_sort_constructor_argument_base: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    structured_sort_constructor_argument_base()
      : atermpp::aterm_appl(core::detail::constructStructProj())
    {}

    /// \brief Constructor.
    /// \param term A term
    structured_sort_constructor_argument_base(const atermpp::aterm_appl& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_StructProj(*this));
    }

    /// \brief Constructor.
    structured_sort_constructor_argument_base(const core::identifier_string& name, const sort_expression& sort)
      : atermpp::aterm_appl(core::detail::gsMakeStructProj(name, sort))
    {}

    /// \brief Constructor.
    structured_sort_constructor_argument_base(const std::string& name, const sort_expression& sort)
      : atermpp::aterm_appl(core::detail::gsMakeStructProj(core::identifier_string(name), sort))
    {}

    core::identifier_string name() const
    {
      return atermpp::aterm_cast<core::identifier_string>(atermpp::arg1(*this));
    }

    sort_expression sort() const
    {
      return atermpp::aterm_cast<sort_expression>(atermpp::arg2(*this));
    }
};

} // namespace detail

/// \brief Special identifier string that is used to specify the absence of an identifier
inline
static core::identifier_string const& no_identifier()
{
  static core::identifier_string dummy;

  return dummy;
}

/// \cond INTERNAL_DOCS
namespace detail
{
/// \brief Convert a string to an identifier, or no_identifier() in case of the empty string
inline
static core::identifier_string make_identifier(std::string const& name)
{
  return (name.empty()) ? no_identifier() : core::identifier_string(name);
}

inline
static core::identifier_string make_identifier(atermpp::aterm_appl const& a)
{
  return (a == core::detail::gsMakeNil()) ? no_identifier() : core::identifier_string(a);
}
}
/// \endcond

/// \brief Argument of a structured sort constructor.
///
/// This comprises an optional name and a mandatory sort.
class structured_sort_constructor_argument: public detail::structured_sort_constructor_argument_base
{
  protected:

    typedef detail::structured_sort_constructor_argument_base super;

    atermpp::aterm_appl make_argument(const sort_expression& sort, const core::identifier_string& name = no_identifier())
    {
      return core::detail::gsMakeStructProj((name == no_identifier()) ?
                                            atermpp::aterm_appl(core::detail::gsMakeNil()) : atermpp::aterm_appl(name), sort);
    }

  public:
    /// \overload
    structured_sort_constructor_argument()
      : detail::structured_sort_constructor_argument_base()
    {}

    /// \overload
    structured_sort_constructor_argument(const aterm &term)
      : detail::structured_sort_constructor_argument_base(atermpp::aterm_appl(term))
    {}

    /// \brief Constructor
    ///
    /// \param[in] name The name of the argument.
    /// \param[in] sort The sort of the argument.
    structured_sort_constructor_argument(const core::identifier_string& name, const sort_expression& sort)
      : detail::structured_sort_constructor_argument_base(make_argument(sort, name))
    {}

    /// \brief Constructor
    ///
    /// \param[in] sort The sort of the argument.
    structured_sort_constructor_argument(const sort_expression& sort)
      : detail::structured_sort_constructor_argument_base(make_argument(sort, no_identifier()))
    {}

    /// \brief Constructor
    ///
    /// \param[in] name The name of the argument.
    /// \param[in] sort The sort of the argument.
    /// The default name, the empty string, signifies that there is no name.
    structured_sort_constructor_argument(const std::string& name, const sort_expression& sort)
      : detail::structured_sort_constructor_argument_base(make_argument(sort, detail::make_identifier(name)))
    {}

    /// \brief Constructor
    ///
    /// \overload to work around problem that MSVC reinterprets char* or char[] as core::identifier_string
    template < size_t S >
    structured_sort_constructor_argument(const char(&name)[S], const sort_expression& sort)
      : detail::structured_sort_constructor_argument_base(make_argument(sort, detail::make_identifier(name)))
    {}

    /// \brief Get the name of the constructor argument
    /// \note This returns no_identifier() if no name is available, and needs
    ///       to be treated as a special case in some user code.
    core::identifier_string name() const
    {
      return detail::make_identifier(super::name());
    }

}; // class structured_sort_constructor_argument

/// \brief List of structured_sort_constructor_argument
typedef atermpp::term_list< structured_sort_constructor_argument > structured_sort_constructor_argument_list;
/// \brief Vector of structured_sort_constructor_argument
typedef std::vector< structured_sort_constructor_argument >    structured_sort_constructor_argument_vector;

} // namespace data

} // namespace mcrl2

namespace std {
template <>
inline void swap(mcrl2::data::structured_sort_constructor_argument& t1, mcrl2::data::structured_sort_constructor_argument& t2)
{
  t1.swap(t2);
}
} // namespace std

#endif // MCRL2_DATA_STRUCTURED_SORT_CONSTUCTOR_ARGUMENT_H

