// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file builder_test.cpp
/// \brief Test for expression builder.

#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/process/process_specification.h"

using namespace mcrl2;

template <typename Derived>
struct builder
{
  // default update
  template <typename T>
  T& update(T& x)
  {
    static_cast<Derived&>(*this)(x);
    return x;
  }

  // ATerm update
  template <typename T>
  T update(const T& x,
           typename boost::enable_if<typename boost::is_base_of<atermpp::aterm_base, T>::type>::type* = 0
          )
  {
    return static_cast<Derived&>(*this)(x);
    //return x;
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
      result.push_back(update(*i));
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
      update(*i);
    }
  }

  // ATerm set traversal
  template <typename T>
  void operator()(atermpp::set<T>& x)
  {
    atermpp::set<T> result;
    for (typename atermpp::set<T>::const_iterator i = x.begin(); i != x.end(); ++i)
    {
      result.insert(update(*i));
    }
    std::swap(x, result);
  }
};

// apply a builder without additional template arguments
template <template <class> class Builder>
struct apply_builder: public Builder<apply_builder<Builder> >
{
  typedef Builder<apply_builder<Builder> > super;
  using super::operator();
};

template <typename Derived>
struct builder1: public builder<Derived>
{
  typedef builder<Derived> super;
  using super::operator();

  lps::action_label operator()(const lps::action_label& x)
  {
    lps::action_label result = lps::action_label(x.name(), static_cast<Derived&>(*this)(x.sorts()));
    return result;
  }

  process::process_equation operator()(const process::process_equation& x)
  {
    process::process_equation result;
    return result;
  }

  process::process_expression operator()(const process::process_expression& x)
  {
    process::process_expression result;
    return result;
  }

  void operator()(process::process_specification& x)
  {
    x.action_labels() = static_cast<Derived&>(*this)(x.action_labels());
    static_cast<Derived&>(*this)(x.equations());
    x.init() = static_cast<Derived&>(*this)(x.init());
  }
};

void test_builder()
{
  apply_builder<builder1> f;

  process::process_specification x;
  f(x);
  
  lps::action_label y1(core::identifier_string("a"), data::sort_expression_list());
  lps::action_label y2 = f(y1);

  lps::action_label_list v1;
  lps::action_label_list v2 = f(v1);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test_builder();

  return EXIT_SUCCESS;
}
