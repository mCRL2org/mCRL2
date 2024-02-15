// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/data/data_io.h"

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/data_specification.h"

using namespace mcrl2;
using namespace mcrl2::data;

static
atermpp::aterm remove_index_impl(const atermpp::aterm& x)
{
  if (x.function() == core::detail::function_symbol_OpId())
  {
    return atermpp::aterm(core::detail::function_symbol_OpIdNoIndex(), x.begin(), --x.end());
  }
  return x;
}

static
atermpp::aterm add_index_impl(const atermpp::aterm& x)
{
  if (x.function() == core::detail::function_symbol_DataVarIdNoIndex())  // Obsolete. Remove in say 2025. 
  {
    const data::variable& y = reinterpret_cast<const data::variable&>(x);
    return variable(y.name(), y.sort()); 
  }
  else if (x.function() == core::detail::function_symbol_OpIdNoIndex())
  {
    const data::function_symbol& y = reinterpret_cast<const data::function_symbol&>(x);
    return function_symbol(y.name(), y.sort()); 
  }
  return x;
}

atermpp::aterm detail::data_specification_to_aterm(const data_specification& s)
{
  return atermpp::aterm(core::detail::function_symbol_DataSpec(),
           atermpp::aterm(core::detail::function_symbol_SortSpec(), atermpp::aterm_list(s.user_defined_sorts().begin(),s.user_defined_sorts().end()) +
                              atermpp::aterm_list(s.user_defined_aliases().begin(),s.user_defined_aliases().end())),
           atermpp::aterm(core::detail::function_symbol_ConsSpec(), atermpp::aterm_list(s.user_defined_constructors().begin(),s.user_defined_constructors().end())),
           atermpp::aterm(core::detail::function_symbol_MapSpec(), atermpp::aterm_list(s.user_defined_mappings().begin(),s.user_defined_mappings().end())),
           atermpp::aterm(core::detail::function_symbol_DataEqnSpec(), atermpp::aterm_list(s.user_defined_equations().begin(),s.user_defined_equations().end())));
}

inline
atermpp::aterm add_index(const atermpp::aterm& x)
{
  return atermpp::bottom_up_replace(x, add_index_impl);
}

inline
atermpp::aterm remove_index(const atermpp::aterm& x)
{
  return atermpp::bottom_up_replace(x, remove_index_impl);
}

atermpp::aterm_istream& data::operator>>(atermpp::aterm_istream& stream, data_specification& spec)
{
  atermpp::aterm_stream_state state(stream);
  stream >> add_index_impl;

  basic_sort_vector sorts;
  alias_vector aliases;
  function_symbol_vector constructors;
  function_symbol_vector user_defined_mappings;
  data_equation_vector user_defined_equations;

  stream >> sorts;
  stream >> aliases;
  stream >> constructors;
  stream >> user_defined_mappings;
  stream >> user_defined_equations;

  // Store the given information in a new data specification (to ignore existing elements of spec).
  spec = data_specification(sorts, aliases, constructors, user_defined_mappings, user_defined_equations);

  return stream;
}

atermpp::aterm_ostream& data::operator<<(atermpp::aterm_ostream& stream, const data_specification& spec)
{
  atermpp::aterm_stream_state state(stream);
  stream << remove_index_impl;

  stream << spec.user_defined_sorts();
  stream << spec.user_defined_aliases();
  stream << spec.user_defined_constructors();
  stream << spec.user_defined_mappings();
  stream << spec.user_defined_equations();
  return stream;
}
