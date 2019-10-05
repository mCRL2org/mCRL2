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
#include "mcrl2/data/function_symbol.h"

using namespace mcrl2;
using namespace mcrl2::data;

atermpp::aterm_appl remove_index_impl(const atermpp::aterm_appl& x)
{
  if (x.function() == core::detail::function_symbol_DataVarId())
  {
    return atermpp::aterm_appl(core::detail::function_symbol_DataVarIdNoIndex(), x.begin(), --x.end());
  }
  else if (x.function() == core::detail::function_symbol_OpId())
  {
    return atermpp::aterm_appl(core::detail::function_symbol_OpIdNoIndex(), x.begin(), --x.end());
  }
  return x;
}

atermpp::aterm_appl add_index_impl(const atermpp::aterm_appl& x)
{
  if (x.function() == core::detail::function_symbol_DataVarIdNoIndex())
  {
    const data::variable& y = atermpp::down_cast<const data::variable>(x);
    std::size_t index = core::index_traits<data::variable, data::variable_key_type, 2>::insert(std::make_pair(y.name(), y.sort()));
    return atermpp::aterm_appl(core::detail::function_symbol_DataVarId(), x[0], x[1], atermpp::aterm_int(index));
  }
  else if (x.function() == core::detail::function_symbol_OpIdNoIndex())
  {
    const data::function_symbol& y = atermpp::down_cast<const data::function_symbol>(x);
    std::size_t index = core::index_traits<data::function_symbol, data::function_symbol_key_type, 2>::insert(std::make_pair(y.name(), y.sort()));
    return atermpp::aterm_appl(core::detail::function_symbol_OpId(), x[0], x[1], atermpp::aterm_int(index));
  }
  return x;
}

atermpp::aterm_appl detail::data_specification_to_aterm(const data_specification& s)
{
  return atermpp::aterm_appl(core::detail::function_symbol_DataSpec(),
           atermpp::aterm_appl(core::detail::function_symbol_SortSpec(), atermpp::aterm_list(s.user_defined_sorts().begin(),s.user_defined_sorts().end()) +
                              atermpp::aterm_list(s.user_defined_aliases().begin(),s.user_defined_aliases().end())),
           atermpp::aterm_appl(core::detail::function_symbol_ConsSpec(), atermpp::aterm_list(s.user_defined_constructors().begin(),s.user_defined_constructors().end())),
           atermpp::aterm_appl(core::detail::function_symbol_MapSpec(), atermpp::aterm_list(s.user_defined_mappings().begin(),s.user_defined_mappings().end())),
           atermpp::aterm_appl(core::detail::function_symbol_DataEqnSpec(), atermpp::aterm_list(s.user_defined_equations().begin(),s.user_defined_equations().end())));
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
  atermpp::aterm t = stream.get();

  if (!t.type_is_appl() || !is_data_specification(atermpp::down_cast<const atermpp::aterm_appl>(t)))
  {
    throw mcrl2::runtime_error("Input stream does not contain a data specification");
  }

  spec = data_specification(atermpp::down_cast<atermpp::aterm_appl>(t));
  return stream;
}

atermpp::aterm_ostream& data::operator<<(atermpp::aterm_ostream& stream, const data_specification& spec)
{
  stream << remove_index_impl << detail::data_specification_to_aterm(spec);
  return stream;
}
