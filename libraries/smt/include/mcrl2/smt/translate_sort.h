// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_TRANSLATE_SORT_H
#define MCRL2_SMT_TRANSLATE_SORT_H

#include "mcrl2/data/traverser.h"
#include "mcrl2/smt/utilities.h"

namespace mcrl2::smt
{
namespace detail
{

static inline
const std::map<data::structured_sort, std::string>& empty_name_map()
{
  static std::map<data::structured_sort, std::string> result;
  return result;
}

template <template <class> class Traverser, class OutputStream>
struct translate_sort_expression_traverser: public Traverser<translate_sort_expression_traverser<Traverser, OutputStream> >
{
  using super = Traverser<translate_sort_expression_traverser<Traverser, OutputStream>>;
  using super::enter;
  using super::leave;
  using super::apply;

  OutputStream& out;
  const native_translations& m_native;
  const std::map<data::structured_sort, std::string>& m_struct_names;

  translate_sort_expression_traverser(OutputStream& out_, const native_translations& nt, const std::map<data::structured_sort, std::string>& snm)
    : out(out_)
    , m_native(nt)
    , m_struct_names(snm)
  {}

  void apply(const data::basic_sort& s)
  {
    auto find_result = m_native.sorts.find(s);
    if(find_result != m_native.sorts.end())
    {
      out << find_result->second;
      return;
    }

    out << translate_identifier(s.name());
  }

  void apply(const data::container_sort& s)
  {
    out << "(" << pp(s.container_name()) << " ";
    super::apply(s.element_sort());
    out << ")";
  }

  void apply(const data::structured_sort& s)
  {
    auto find_result = m_struct_names.find(s);
    if(find_result != m_struct_names.end())
    {
      out << find_result->second;
    }
    else
    {
      throw translation_error("Cannot translate structured sort " + pp(s));
    }
  }

  void apply(const data::function_sort& s)
  {
    throw translation_error("Cannot translate function sort " + pp(s));
  }
};

template <template <class> class Traverser, class OutputStream>
translate_sort_expression_traverser<Traverser, OutputStream>
make_translate_sort_expression_traverser(OutputStream& out, const native_translations& nt,
  const std::map<data::structured_sort, std::string>& snm)
{
  return translate_sort_expression_traverser<Traverser, OutputStream>(out, nt, snm);
}

} // namespace detail

template <typename T, typename OutputStream>
void translate_sort_expression(const T& x, OutputStream& o, const native_translations& nt,
  const std::map<data::structured_sort, std::string>& snm = detail::empty_name_map())
{
  detail::make_translate_sort_expression_traverser<data::sort_expression_traverser>(o, nt, snm).apply(x);
}

} // namespace mcrl2::smt

#endif
