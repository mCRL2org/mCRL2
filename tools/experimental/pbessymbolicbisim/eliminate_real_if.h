// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file eliminate_real_if.h


#ifndef MCRL2_PBESSYMBOLICBISIM_ELIMINATE_REAL_IF_H
#define MCRL2_PBESSYMBOLICBISIM_ELIMINATE_REAL_IF_H

namespace mcrl2::data
{

namespace detail
{

template <template <class> class Builder>
struct eliminate_real_if_builder: public Builder<eliminate_real_if_builder<Builder> >
{
  using super = Builder<eliminate_real_if_builder<Builder>>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  void apply(application& result, const application& x)
  {
    application y;
    super::apply(y, x);
    if (is_if_application(y))
    {
      result = y;
      return;
    }
    const data_expression& f = y.head();
    std::list<data_expression> arguments(y.begin(), y.end());
    for(auto i = arguments.begin(); i != arguments.end(); ++i)
    {
      if(i->sort() == sort_real::real_() && is_if_application(*i))
      {
        const data_expression condition = atermpp::down_cast<application>(*i)[0];
        const data_expression then_part = atermpp::down_cast<application>(*i)[1];
        const data_expression else_part = atermpp::down_cast<application>(*i)[2];
        *i = then_part;
        const data_expression left = application(f, arguments.begin(), arguments.end());
        *i = else_part;
        const data_expression right = application(f, arguments.begin(), arguments.end());
        make_if_(result, condition, [&](data_expression& r){ apply(r, left); }, 
                                    [&](data_expression& r){ apply(r, right); });
        return;
      }
    }
    result = y;
  }
};

template <template <class> class Builder>
eliminate_real_if_builder<Builder>
make_eliminate_real_if_builder()
{
  return eliminate_real_if_builder<Builder>();
}

} // namespace detail

template <typename T>
void eliminate_real_if(T& x, std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  data::detail::make_eliminate_real_if_builder<data::data_expression_builder>().update(x);
}

template <typename T>
T eliminate_real_if(const T x, std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result; 
  data::detail::make_eliminate_real_if_builder<data::data_expression_builder>().apply(result, x);
  return result;
}

} // namespace mcrl2
// namespace data

#endif // MCRL2_PBESSYMBOLICBISIM_ELIMINATE_REAL_IF_H
