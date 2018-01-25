// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/builder.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_BUILDER_H
#define MCRL2_CORE_BUILDER_H

#include <stdexcept>
#include <type_traits>

#include "mcrl2/utilities/exception.h"
#include "mcrl2/atermpp/type_traits.h"
#include "mcrl2/atermpp/aterm_list.h"

namespace mcrl2
{

namespace core
{

#ifdef MCRL2_DEBUG_EXPRESSION_BUILDER
inline void msg(const std::string& s)
{
  std::cout << "--- " << s << " ---" << std::endl;
}
#else
inline void msg(const std::string&)
{}
#endif

/**
 * \brief expression builder that visits all sub expressions
 *
 * Types:
 *  \arg Derived the type of a derived class, as per CRTP
 *
 **/
template <typename Derived>
struct builder
{
  // Enter object
  template <typename T>
  void enter(const T&)
  {}

  // Leave object
  template <typename T>
  void leave(const T&)
  {}

  template <typename T>
  void update(T& x, typename atermpp::disable_if_container<T>::type* = nullptr)
  {
    msg("non-container visit");
    x = static_cast<Derived*>(this)->apply(x);
  }

  // container visit
  template <typename T>
  void update(T& x, typename atermpp::enable_if_container<T>::type* = nullptr)
  {
    msg("container visit");
    for (auto& v: x)
    {
      static_cast<Derived*>(this)->update(v);
    }
  }

  // aterm set visit
  template <typename T>
  void update(std::set<T>& x)
  {
    msg("set visit");
    std::set<T> result;
    for (T v: x)
    {
      static_cast<Derived*>(this)->update(v);
      result.insert(v);
    }
    result.swap(x);
  }

  // term_list visit copy
  template <typename T>
  atermpp::term_list<T> apply(const atermpp::term_list<T>& x)
  {
    msg("term_list traversal");
    return atermpp::term_list<T>(x.begin(), x.end(), [&](const T& v) { return atermpp::vertical_cast<T>(static_cast<Derived*>(this)->apply(v)); } );
  }
};


// apply a builder without additional template arguments
template <template <class> class Builder>
class apply_builder: public Builder<apply_builder<Builder> >
{
    typedef Builder<apply_builder<Builder> > super;

  public:

    using super::enter;
    using super::leave;
    using super::apply;
    using super::update;
};

template <template <class> class Builder>
apply_builder<Builder>
make_apply_builder()
{
  return apply_builder<Builder>();
}

// apply a builder with one additional template argument
template <template <class> class Builder, class Arg1>
class apply_builder_arg1: public Builder<apply_builder_arg1<Builder, Arg1> >
{
    typedef Builder<apply_builder_arg1<Builder, Arg1> > super;

  public:
    using super::enter;
    using super::leave;
    using super::apply;
    using super::update;

    apply_builder_arg1(const Arg1& arg1):
      super(arg1)
    {}
};

template <template <class> class Builder, class Arg1>
apply_builder_arg1<Builder, Arg1>
make_apply_builder_arg1(const Arg1& arg1)
{
  return apply_builder_arg1<Builder, Arg1>(arg1);
}

// apply a builder without additional template arguments
template <template <class> class Builder, class Function>
struct update_apply_builder: public Builder<update_apply_builder<Builder, Function> >
{
  typedef Builder<update_apply_builder<Builder, Function> > super;

  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  typedef typename Function::result_type result_type;
  typedef typename Function::argument_type argument_type;

  const Function& f_;

  result_type apply(const argument_type& x)
  {
    return f_(x);
  }

  update_apply_builder(const Function& f)
    : f_(f)
  {}
};

template <template <class> class Builder, class Function>
update_apply_builder<Builder, Function>
make_update_apply_builder(const Function& f)
{
  return update_apply_builder<Builder, Function>(f);
}

// apply a builder with one additional template argument
template <template <class> class Builder, class Function, class Arg1>
class update_apply_builder_arg1: public Builder<update_apply_builder_arg1<Builder, Function, Arg1> >
{
  typedef Builder<update_apply_builder_arg1<Builder, Function, Arg1> > super;

  using super::enter;
  using super::leave;
	using super::apply;
  using super::update;

  typedef typename Function::result_type result_type;
  typedef typename Function::argument_type argument_type;

  const Function& f_;

  result_type apply(const argument_type& x)
  {
    return f_(x);
  }

  update_apply_builder_arg1(const Function& f, const Arg1& arg1):
    super(arg1),
    f_(f)
  {}
};

template <template <class> class Builder, class Function, class Arg1>
update_apply_builder_arg1<Builder, Function, Arg1>
make_update_apply_builder_arg1(const Function& f)
{
  return update_apply_builder_arg1<Builder, Function, Arg1>(f);
}

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_BUILDER_H
