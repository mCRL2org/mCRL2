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
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/core/down_cast.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/utilities/exception.h"

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

  // aterm update
  template <typename T>
  void update(T& x,
              typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
             )
  {
    msg("aterm update");
    x = static_cast<Derived&>(*this)(x);
  }

  // non-aterm update
  template <typename T>
  void update(T& x,
              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
             )
  {
    msg("non-aterm update");
    static_cast<Derived&>(*this)(x);
  }

  // aterm update copy
  template <typename T>
  T update_copy(const T& x,
                typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
               )
  {
    msg("aterm update copy");
    return core::static_down_cast<const T&>(static_cast<Derived&>(*this)(x));
  }

  // non-aterm update copy
  template <typename T>
  T& update_copy(T& x,
                 typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                )
  {
    msg("non-aterm update copy");
    static_cast<Derived&>(*this)(x);
    return x;
  }

  // non-container visit
  template <typename T>
  void visit(T&,
             typename atermpp::detail::disable_if_container<T>::type* = 0
            )
  {
    msg("non-container visit");
    throw mcrl2::runtime_error("unknown type encountered in builder function!");
  }

  // container visit
  template <typename T>
  void visit(T& x,
             typename atermpp::detail::enable_if_container<T>::type* = 0
            )
  {
    msg("container visit");
    for (typename T::iterator i = x.begin(); i != x.end(); ++i)
    {
      update(*i);
    }
  }

  // aterm set visit
  template <typename T>
  void visit(std::set<T>& x)
  {
    msg("aterm set visit");
    std::set<T> result;
    for (typename std::set<T>::const_iterator i = x.begin(); i != x.end(); ++i)
    {
      result.insert(update_copy(*i));
    }
    using std::swap;
    swap(x, result);
  }

  // non-container visit_copy
  template <typename T>
  T visit_copy(const T& x)
  {
    msg("non-container visit_copy");
    throw mcrl2::runtime_error("unknown type encountered in builder function!");
    return x;
  }

  // term_list visit copy
  template <typename T>
  atermpp::term_list<T> visit_copy(const atermpp::term_list<T>& x)
  {
    msg("term_list visit_copy");
    std::vector<T> result;
    for (typename atermpp::term_list<T>::const_iterator i = x.begin(); i != x.end(); ++i)
    {
      result.push_back(core::static_down_cast<const T&>(static_cast<Derived&>(*this)(*i)));
    }
    return atermpp::term_list<T>(result.begin(),result.end());
  }

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#else
  // aterm traversal
  template <typename T>
  T operator()(const T& x,
               typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
              )
  {
    msg("aterm traversal");
    return visit_copy(x);
  }

  // non-aterm traversal
  template <typename T>
  void operator()(T& x,
                  typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0
                 )
  {
    msg("non aterm traversal");
    visit(x);
  }
#endif
};


// apply a builder without additional template arguments
template <template <class> class Builder>
class apply_builder: public Builder<apply_builder<Builder> >
{
    typedef Builder<apply_builder<Builder> > super;

  public:

    using super::enter;
    using super::leave;
    using super::operator();

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
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
    using super::operator();

    apply_builder_arg1(const Arg1& arg1):
      super(arg1)
    {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
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
  using super::operator();

  typedef typename Function::result_type result_type;
  typedef typename Function::argument_type argument_type;

  Function f_;

  result_type operator()(const argument_type& x)
  {
    return f_(x);
  }

  update_apply_builder(Function f)
    : f_(f)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class> class Builder, class Function>
update_apply_builder<Builder, Function>
make_update_apply_builder(Function f)
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
    using super::operator();

    typedef typename Function::result_type result_type;
    typedef typename Function::argument_type argument_type;

    Function f_;

    result_type operator()(const argument_type& x)
    {
      return f_(x);
    }

    update_apply_builder_arg1(Function f, const Arg1& arg1):
      super(arg1),
      f_(f)
    {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif
};

template <template <class> class Builder, class Function, class Arg1>
update_apply_builder_arg1<Builder, Function, Arg1>
make_update_apply_builder_arg1(Function f)
{
  return update_apply_builder_arg1<Builder, Function, Arg1>(f);
}

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_BUILDER_H
