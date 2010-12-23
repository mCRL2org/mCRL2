// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file builder_test2.cpp
/// \brief Test for expression builder.

#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/process/process_specification.h"

using namespace mcrl2;

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

  template <typename Derived>
  class process_sort_normalization_builder: public builder<Derived>
  {
    protected:
      const data::data_specification& m_data_spec;

    public:
      typedef builder<Derived> super;
  
      using super::enter;
      using super::leave;
      using super::operator();

      process_sort_normalization_builder(const data::data_specification& data_spec)
        : m_data_spec(data_spec)
      {}

      // This one is fine with Visual C++
      data::data_expression operator()(const data::variable& x)
      {
        return data::data_expression();
      }
      
      // This one is also fine with Visual C++
      data::sort_expression operator()(const data::function_sort& x)
      {
        return data::sort_expression();
      }

      // But this one gives a "similar conversion" error with Visual C++
      data::sort_expression operator()(const data::structured_sort& x)
      {
        return data::sort_expression();
      }

      lps::action_label operator()(const lps::action_label& x)
      {
        static_cast<Derived&>(*this).enter(x);  
        lps::action_label result = lps::action_label(x.name(), static_cast<Derived&>(*this)(x.sorts()));
        static_cast<Derived&>(*this).leave(x);
        return result;
      }

      void operator()(process::process_specification& x)
      {
        x.action_labels() = static_cast<Derived&>(*this)(x.action_labels());
      }
  };          

void test_normalize_sorts()
{
  process::process_specification x;
  apply_builder_arg1<process_sort_normalization_builder, data::data_specification> builder(x.data());
  builder(x);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test_normalize_sorts();

  return EXIT_SUCCESS;
}
