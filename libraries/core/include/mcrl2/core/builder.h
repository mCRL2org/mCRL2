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

#include "boost/utility/enable_if.hpp"
#include "boost/type_traits/is_base_of.hpp"

#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/atermpp/convert.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/identifier_string.h"

namespace mcrl2 {

namespace core {

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
    
    // default update
    template <typename T>
    T& update_copy(T& x,
              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
             )
    {
      static_cast<Derived&>(*this)(x);
      return x;
    }
  
    // ATerm update
    template <typename T>
    T update_copy(const T& x,
             typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
            )
    {
      return static_cast<Derived&>(*this)(x);
    }

    // default update
    template <typename T>
    void update_inplace(T& x,
              typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
             )
    {
      static_cast<Derived&>(*this)(x);
    }
  
    // ATerm update
    template <typename T>
    void update_inplace(T& x,
             typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
            )
    {
      x = static_cast<Derived&>(*this)(x);
    }

    // ATerm traversal
    template <typename T>
    T operator()(const T& x,
                 typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
                )
    {
      return x;
    }  
  
    // ATerm list traversal
    template <typename T>
    atermpp::term_list<T> operator()(const atermpp::term_list<T>& x,
                                     typename atermpp::detail::enable_if_container<T>::type* = 0  // why is this needed???
                                    )
    {
      atermpp::vector<T> result;
      for (typename atermpp::term_list<T>::const_iterator i = x.begin(); i != x.end(); ++i)
      {
        result.push_back(update_copy(*i));
      }
      return atermpp::convert<atermpp::term_list<T> >(result);
    }
  
    // Container traversal
    template <typename T>
    void operator()(T& x,
                    typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0,
                    typename atermpp::detail::enable_if_container<T>::type* = 0
                   )
    {
      for (typename T::iterator i = x.begin(); i != x.end(); ++i)
      {
        update_inplace(*i);
      }
    }
  
    // ATerm set traversal
    template <typename T>
    void operator()(atermpp::set<T>& x)
    {
      atermpp::set<T> result;
      for (typename atermpp::set<T>::const_iterator i = x.begin(); i != x.end(); ++i)
      {
        result.insert(update_copy(*i));
      }
      std::swap(x, result);
    }
  };

  // apply a builder without additional template arguments
  template <template <class> class Builder>
  struct apply_builder: public Builder<apply_builder<Builder> >
  {
    typedef Builder<apply_builder<Builder> > super;

    using super::enter;
    using super::leave;
    using super::operator();

//#ifdef BOOST_MSVC
    // ATerm list traversal
    template <typename T>
    atermpp::term_list<T> operator()(const atermpp::term_list<T>& x)
    {
      atermpp::vector<T> result;
      for (typename atermpp::term_list<T>::const_iterator i = x.begin(); i != x.end(); ++i)
      {
        result.push_back(update_copy(*i));
      }
      return atermpp::convert<atermpp::term_list<T> >(result);
    }

    // Container traversal
    template <typename T>
    void operator()(T& x,
                    typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0,
                    typename atermpp::detail::enable_if_container<T>::type* = 0
                   )
    {
      for (typename T::iterator i = x.begin(); i != x.end(); ++i)
      {
        update_inplace(*i);
      }
    }

    // ATerm set traversal
    template <typename T>
    void operator()(atermpp::set<T>& x)
    {
      atermpp::set<T> result;
      for (typename atermpp::set<T>::const_iterator i = x.begin(); i != x.end(); ++i)
      {
        result.insert(update_copy(*i));
      }
      std::swap(x, result);
    }
//#endif
  };

  // apply a builder with one additional template argument
  template <template <class> class Builder, class Arg1>
  struct apply_builder_arg1: public Builder<apply_builder_arg1<Builder, Arg1> >
  {
    typedef Builder<apply_builder_arg1<Builder, Arg1> > super;   
    using super::enter;
    using super::leave;
    using super::operator();

    apply_builder_arg1(const Arg1& arg1):
      super(arg1)
    {}
  
//#ifdef BOOST_MSVC
    // ATerm list traversal
    template <typename T>
    atermpp::term_list<T> operator()(const atermpp::term_list<T>& x)
    {
      atermpp::vector<T> result;
      for (typename atermpp::term_list<T>::const_iterator i = x.begin(); i != x.end(); ++i)
      {
        result.push_back(update_copy(*i));
      }
      return atermpp::convert<atermpp::term_list<T> >(result);
    }

    // Container traversal
    template <typename T>
    void operator()(T& x,
                    typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0,
                    typename atermpp::detail::enable_if_container<T>::type* = 0
                   )
    {
      for (typename T::iterator i = x.begin(); i != x.end(); ++i)
      {
        update_inplace(*i);
      }
    }

    // ATerm set traversal
    template <typename T>
    void operator()(atermpp::set<T>& x)
    {
      atermpp::set<T> result;
      for (typename atermpp::set<T>::const_iterator i = x.begin(); i != x.end(); ++i)
      {
        result.insert(update_copy(*i));
      }
      std::swap(x, result);
    }
//#endif
  };

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_BUILDER_H
