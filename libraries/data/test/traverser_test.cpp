// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file traverser_test.cpp
/// \brief Traverser tests.

#include <iostream>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/detail/binding_aware_traverser.h"
#include "mcrl2/data/detail/sort_traverser.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::data;

class identity_traverser: public mcrl2::data::detail::traverser<identity_traverser>
{
  public:
    typedef mcrl2::data::detail::traverser<identity_traverser> super;
      
    using super::enter;
    using super::leave;
#if BOOST_MSVC
    // Workaround for malfunctioning MSVC 2008 overload resolution
    template <typename Container >
    void operator()(Container const& a)
    {
      super::operator()(a);
    }
#endif
};

class identity_sort_traverser: public mcrl2::data::detail::sort_traverser<identity_sort_traverser>
{
  public:
    typedef mcrl2::data::detail::traverser<identity_sort_traverser> super;
      
    using super::enter;
    using super::leave;
#if BOOST_MSVC
    // Workaround for malfunctioning MSVC 2008 overload resolution
    template <typename Container >
    void operator()(Container const& a)
    {
      super::operator()(a);
    }
#endif
};

class my_traverser: public mcrl2::data::detail::traverser<my_traverser>
{
  protected:
    unsigned int m_sort_count;

  public:
    typedef mcrl2::data::detail::traverser<my_traverser> super;
      
    using super::enter;
    using super::leave;
#if BOOST_MSVC
      // Workaround for malfunctioning MSVC 2008 overload resolution
      template <typename Container >
      void operator()(Container const& a)
      {
        super::operator()(a);
      }
#endif

    my_traverser() : m_sort_count(0)
    { }

    void enter(sort_expression const& s)
    {
      m_sort_count++;
    }
    
    unsigned int sort_count() const
    {
      return m_sort_count;
    }
};

class my_sort_traverser: public mcrl2::data::detail::sort_traverser<my_sort_traverser>
{
  protected:
    unsigned int m_sort_count;

  public:
    typedef mcrl2::data::detail::sort_traverser<my_sort_traverser> super;
      
    using super::enter;
    using super::leave;
#if BOOST_MSVC
      // Workaround for malfunctioning MSVC 2008 overload resolution
      template <typename Container >
      void operator()(Container const& a)
      {
        super::operator()(a);
      }
#endif

    my_sort_traverser() : m_sort_count(0)
    { }

    void enter(sort_expression const& s)
    {
      m_sort_count++;
    }
    
    unsigned int sort_count() const
    {
      return m_sort_count;
    }
};

template <typename Derived>
class custom_sort_traverser : public mcrl2::data::detail::traverser<Derived>
{
  public:
    typedef mcrl2::data::detail::traverser<Derived> super;

    using super::enter;
    using super::leave;
#if BOOST_MSVC
    // Workaround for malfunctioning MSVC 2008 overload resolution
    template <typename Container >
    void operator()(Container const& a)
    {
      super::operator()(a);
    }
#else
    using super::operator();
#endif

    void operator()(function_symbol const& e)
    {
      static_cast<Derived&>(*this).enter(static_cast< data_expression const& >(e));
      static_cast<Derived&>(*this).enter(e);
      static_cast<Derived&>(*this)(e.sort());
      static_cast<Derived&>(*this)(e.name());
      static_cast<Derived&>(*this).leave(e);
      static_cast<Derived&>(*this).leave(static_cast< data_expression const& >(e));
    }

    void operator()(variable const& e)
    {
      static_cast<Derived&>(*this).enter(static_cast< data_expression const& >(e));
      static_cast<Derived&>(*this).enter(e);
      static_cast<Derived&>(*this)(e.sort());
      static_cast<Derived&>(*this)(e.name());
      static_cast<Derived&>(*this).leave(e);
      static_cast<Derived&>(*this).leave(static_cast< data_expression const& >(e));
    }

    void operator()(basic_sort const& e)
    {
      static_cast<Derived&>(*this).enter(static_cast< sort_expression const& >(e));
      static_cast<Derived&>(*this).enter(e);
      static_cast<Derived&>(*this)(e.name());
      static_cast<Derived&>(*this).leave(e);
      static_cast<Derived&>(*this).leave(static_cast< sort_expression const& >(e));
    }

    void operator()(function_sort const& e)
    {
      static_cast<Derived&>(*this).enter(static_cast< sort_expression const& >(e));
      static_cast<Derived&>(*this).enter(e);

      static_cast<Derived&>(*this)(e.domain());
      static_cast<Derived&>(*this)(e.codomain());

      static_cast<Derived&>(*this).leave(e);
      static_cast<Derived&>(*this).leave(static_cast< sort_expression const& >(e));
    }

    void operator()(container_sort const& e)
    {
      static_cast<Derived&>(*this).enter(static_cast< sort_expression const& >(e));
      static_cast<Derived&>(*this).enter(e);

      static_cast<Derived&>(*this)(e.element_sort());

      static_cast<Derived&>(*this).leave(static_cast< sort_expression const& >(e));
      static_cast<Derived&>(*this).leave(e);
    }

    void operator()(structured_sort const& e)
    {
      static_cast<Derived&>(*this).enter(static_cast< sort_expression const& >(e));
      static_cast<Derived&>(*this).enter(e);

      for (structured_sort::constructors_const_range r(e.struct_constructors()); !r.empty(); r.advance_begin(1))
      {
        for (structured_sort_constructor::arguments_const_range j(r.front().arguments()); !j.empty(); j.advance_begin(1))
        {
          static_cast<Derived&>(*this)(j.front().sort());
        }
      }

      static_cast<Derived&>(*this).leave(e);
      static_cast<Derived&>(*this).leave(static_cast< sort_expression const& >(e));
    }

    void operator()(sort_expression const& e)
    {
      if (e.is_basic_sort())
      {
        static_cast<Derived&>(*this)(basic_sort(e));
      }
      else if (e.is_container_sort())
      {
        static_cast<Derived&>(*this)(container_sort(e));
      }
      else if (e.is_structured_sort())
      {
        static_cast<Derived&>(*this)(structured_sort(e));
      }
      else if (e.is_function_sort())
      {
        static_cast<Derived&>(*this)(function_sort(e));
      }
    }

    void operator()(alias const& e)
    {
      static_cast<Derived&>(*this).enter(e);

      static_cast<Derived&>(*this)(e.name());
      static_cast<Derived&>(*this)(e.reference());

      static_cast<Derived&>(*this).leave(e);
    }

    void operator()(assignment const& a)
    {
      static_cast< super& >(*this)(a);
    }

    void operator()(data_equation const& e)
    {
      static_cast< super& >(*this)(e);
    }

    void operator()(data_specification const& e)
    {
      static_cast<Derived&>(*this)(e.sorts());
      // static_cast<Derived&>(*this)(e.aliases());
      static_cast< super& >(*this)(e);
    }
};

class custom_sort_traverser1: public custom_sort_traverser<custom_sort_traverser1>
{
  protected:
    unsigned int m_sort_count;

  public:
    typedef custom_sort_traverser<custom_sort_traverser1> super;
      
    using super::enter;
    using super::leave;
#if BOOST_MSVC
      // Workaround for malfunctioning MSVC 2008 overload resolution
      template <typename Container >
      void operator()(Container const& a)
      {
        super::operator()(a);
      }
#endif
};

void test_traversers()
{
  data_expression x;
  std::string var_decl =
    "n: Pos;\n"
    ; 
  x = parse_data_expression("n < 10", var_decl); 
  
  identity_traverser t1;
  t1(x); 

  identity_sort_traverser t2;
  t2(x); 

  my_traverser t3;
  t3(x);
  BOOST_CHECK(t3.sort_count() == 0);

  my_sort_traverser t4;
  t4(x);
  BOOST_CHECK(t4.sort_count() > 0);
  core::garbage_collect();

  custom_sort_traverser1 t5;
  t5(x);
}

//inline
//data::variable nat(std::string name)
//{
//  return data::variable(core::identifier_string(name), data::sort_nat::nat());
//}
//
//void test_find()
//{
//  data::variable m = nat("m"); 
//  std::set<data::variable> v = data::find_variables(m);
//  //v = data::find_variables(s); // TODO: this doesn't compile!
//  BOOST_CHECK(v.find(m) != v.end());   
//
//  //--- find_sort_expressions ---//
//  //std::set<data::sort_expression> e = data::find_sort_expressions(m);
//  //BOOST_CHECK(std::find(e.begin(), e.end(), data::sort_nat::nat()) != e.end());
//  //BOOST_CHECK(std::find(e.begin(), e.end(), data::sort_pos::pos()) == e.end());
//}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);

  test_traversers();

  return EXIT_SUCCESS;
}
