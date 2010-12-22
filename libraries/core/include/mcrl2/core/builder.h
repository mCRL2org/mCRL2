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
  class builder
  {
    public:

      // Enter object
      template <typename Expression>
      void enter(Expression const&)
      {}

      // Leave object
      template <typename Expression>
      void leave(Expression const&)
      {}

      // Traverse ATerm object. Since ATerms are read-only, it returns the computed result.
      template <typename Expression>
      Expression operator()(const Expression& x,
                            typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, Expression>::type>::type* = 0
                           )
      {
        return x;
      }

      // Traverse object that is not a container or an ATerm
      template <typename Expression>
      void operator()(Expression& x,
                      typename atermpp::detail::disable_if_container<Expression>::type* = 0,
                      typename boost::disable_if<typename boost::is_base_of<atermpp::aterm_base, Expression>::type >::type* = 0
                     )
      {
        // skip
      }

      // Traverse non-constant container
      template <typename Container>
      void operator()(Container& container, typename atermpp::detail::enable_if_container<Container>::type* = 0)
      {
        for (typename Container::iterator i = container.begin(); i != container.end(); ++i)
        {
          static_cast<Derived&>(*this)(*i);
        }
      }

      // Traverse constant container
      template <typename Container>
      Container operator()(const Container& container, typename atermpp::detail::enable_if_container<Container>::type* = 0)
      {
        throw std::runtime_error("Please supply an overload for traversing this container");
        return container;
      }

      // Traverse non-constant aterm list
      template <typename T>
      atermpp::term_list<T> operator()(atermpp::term_list<T>& x)
      {
        atermpp::vector<T> result;
        for (typename atermpp::term_list<T>::const_iterator i = x.begin(); i != x.end(); ++i)
        {
          result.push_back(static_cast<Derived&>(*this)(*i));
        }
        return atermpp::convert<atermpp::term_list<T> >(result);
      }

      // Traverse constant aterm list
      template <typename T>
      atermpp::term_list<T> operator()(const atermpp::term_list<T>& x)
      {
        atermpp::vector<T> result;
        for (typename atermpp::term_list<T>::const_iterator i = x.begin(); i != x.end(); ++i)
        {
          result.push_back(static_cast<Derived&>(*this)(*i));
        }
        return atermpp::convert<atermpp::term_list<T> >(result);
      }

      // Traverse aterm set
      template <typename T>
      void operator()(atermpp::set<T>& x)
      {
        atermpp::set<T> result;
        for (typename atermpp::set<T>::const_iterator i = x.begin(); i != x.end(); ++i)
        {
          result.insert(static_cast<Derived&>(*this)(*i));
        }
        std::swap(x, result);
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
      using super::operator();
  };

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
  };

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_BUILDER_H
