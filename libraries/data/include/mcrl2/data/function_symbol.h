// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/function_symbol.h
/// \brief The class function symbol.

#ifndef MCRL2_DATA_FUNCTION_SYMBOL_H
#define MCRL2_DATA_FUNCTION_SYMBOL_H

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/sort_expression.h"

#ifdef MCRL2_USE_INDEX_TRAITS
#include "mcrl2/core/index_traits.h"
#endif

namespace mcrl2
{

namespace data
{

//--- start generated class function_symbol ---//
/// \brief A function symbol
class function_symbol: public data_expression
{
  public:
    /// \brief Default constructor.
    function_symbol()
      : data_expression(core::detail::constructOpId())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit function_symbol(const atermpp::aterm& term)
      : data_expression(term)
    {
      assert(core::detail::check_term_OpId(*this));
    }

    /// \brief Constructor.
    function_symbol(const core::identifier_string& name, const sort_expression& sort)
      : data_expression(core::detail::gsMakeOpId(name, sort))
    {}

    /// \brief Constructor.
    function_symbol(const std::string& name, const sort_expression& sort)
      : data_expression(core::detail::gsMakeOpId(core::identifier_string(name), sort))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>((*this)[0]);
    }

    const sort_expression& sort() const
    {
      return atermpp::aterm_cast<const sort_expression>((*this)[1]);
    }
};

/// \brief list of function_symbols
typedef atermpp::term_list<function_symbol> function_symbol_list;

/// \brief vector of function_symbols
typedef std::vector<function_symbol>    function_symbol_vector;

/// \brief swap overload
inline void swap(function_symbol& t1, function_symbol& t2)
{
  t1.swap(t2);
}

//--- end generated class function_symbol ---//

#ifdef MCRL2_USE_INDEX_TRAITS

inline
void on_create_function_symbol(const atermpp::aterm& t)
{
  core::index_traits<function_symbol>::insert(static_cast<const function_symbol&>(t));
}

inline
void on_delete_function_symbol(const atermpp::aterm& t)
{
  core::index_traits<function_symbol>::erase(static_cast<const function_symbol&>(t));
}

inline
void register_function_symbol_hooks()
{
  add_creation_hook(core::detail::function_symbol_OpId(), on_create_function_symbol);
  add_deletion_hook(core::detail::function_symbol_OpId(), on_delete_function_symbol);
}

#endif // MCRL2_USE_INDEX_TRAITS

// template function overloads
std::string pp(const function_symbol& x);
std::string pp(const function_symbol_list& x);
std::string pp(const function_symbol_vector& x);
std::set<data::variable> find_all_variables(const data::function_symbol& x);

} // namespace data

} // namespace mcrl2

#ifdef MCRL2_USE_INDEX_TRAITS

namespace std {

template<>
struct hash<mcrl2::data::function_symbol>
{
  size_t operator()(const mcrl2::data::function_symbol & x) const
  {
    return mcrl2::core::hash_value(x.name(), x.sort());
  }
};

}
#endif

#endif // MCRL2_DATA_FUNCTION_SYMBOL_H

