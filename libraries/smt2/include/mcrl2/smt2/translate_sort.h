// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_TRANSLATE_SORT_H
#define MCRL2_SMT_TRANSLATE_SORT_H

#include "mcrl2/data/sort_expression.h"
#include "mcrl2/smt2/native_translation.h"
#include "mcrl2/smt2/translation_error.h"

namespace mcrl2
{
namespace smt
{
namespace detail
{

template <template <class> class Traverser, class OutputStream>
struct translate_sort_expression_traverser: public Traverser<translate_sort_expression_traverser<Traverser, OutputStream> >
{
  typedef Traverser<translate_sort_expression_traverser<Traverser, OutputStream> > super;
  using super::enter;
  using super::leave;
  using super::apply;

  OutputStream& out;
  const native_translations& m_native;

  translate_sort_expression_traverser(OutputStream& out_, const native_translations& nt)
    : out(out_)
    , m_native(nt)
  {}

  void apply(const data::basic_sort& s)
  {
    auto find_result = m_native.sorts.find(s);
    if(find_result != m_native.sorts.end())
    {
      out << find_result->second;
      return;
    }

    out << s.name();
  }

  void apply(const data::container_sort& s)
  {
    out << "(" << pp(s.container_name()) << " (";
    super::apply(s.element_sort());
    out << "))";
  }

  void apply(const data::structured_sort& s)
  {
    throw translation_error("Cannot translate structured sort " + pp(s));
  }

  void apply(const data::function_sort& s)
  {
    throw translation_error("Cannot translate function sort " + pp(s));
  }
};

template <template <class> class Traverser, class OutputStream>
translate_sort_expression_traverser<Traverser, OutputStream>
make_translate_sort_expression_traverser(OutputStream& out, const native_translations& nt)
{
  return translate_sort_expression_traverser<Traverser, OutputStream>(out, nt);
}

} // namespace detail

template <typename T, typename OutputStream>
void translate_sort_expression(const T& x, OutputStream& o, const native_translations& nt)
{
  detail::make_translate_sort_expression_traverser<data::sort_expression_traverser>(o, nt).apply(x);
}

} // namespace smt
} // namespace mcrl2

#endif
