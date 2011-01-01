// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transform_test.cpp
/// \brief Add your file description here.

#include <functional>
#include <boost/test/minimal.hpp>
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/parse.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

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

inline
data::variable nat(std::string name)
{
  return data::variable(core::identifier_string(name), data::sort_nat::nat());
}

inline
data::variable pos(std::string name)
{
  return data::variable(core::identifier_string(name), data::sort_pos::pos());
}

inline
data::variable bool_(std::string name)
{
  return data::variable(core::identifier_string(name), data::sort_bool::bool_());
}

struct f: public std::unary_function<data::variable, data::data_expression>
{
  data::data_expression operator()(const data::variable& x)
  {
    if (x.name() == core::identifier_string("m"))
    {
      return data::variable(core::identifier_string("n"), x.sort());
    }
    return x;
  }
};

void test_transform()
{
  const std::string VARSPEC =
    "datavar         \n"
    "  m: Nat;       \n"
    "  n: Nat;       \n"
    "                \n"
    "predvar         \n"
    "  X: Bool, Pos; \n"
    "  Y: Nat;       \n"
    ;

  pbes_expression x = parse_pbes_expression("X(true, 2) && Y(n+1) && Y(m)", VARSPEC);
  pbes_expression y = parse_pbes_expression("X(true, 2) && Y(n+1) && Y(n)", VARSPEC);
  pbes_expression z = make_update_apply_builder<pbes_system::data_expression_builder>(f())(x);

  BOOST_CHECK(y == z);

  core::garbage_collect(); 
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_transform();

  return 0;
}
