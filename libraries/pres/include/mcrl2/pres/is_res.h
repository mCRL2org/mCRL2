// Author(s): Jan Friso Groote. Based on pbes/is_bes.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pres/include/mcrl2/pres/is_res.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_IS_BES_H
#define MCRL2_PRES_IS_BES_H

#include "mcrl2/data/consistency.h"
#include "mcrl2/pres/traverser.h"

namespace mcrl2 {

namespace pres_system {

/// \cond INTERNAL_DOCS
/// \brief Visitor for checking if a pres object is in BES form.
struct is_res_traverser: public pres_expression_traverser<is_res_traverser>
{
  typedef pres_expression_traverser<is_res_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result;
  std::string error_message;

  std::string get_error_message() const
  {
    assert(result==false);
    return error_message;
  }

  is_res_traverser()
    : result(true)
  {}

  void enter(const infimum& x)
  {
    result = false;
    if (error_message.empty())
    {
      error_message="Infimum not allowed in RES: " + pp(x);
    }
  }

  void enter(const supremum& x)
  {
    result = false;
    if (error_message.empty())
    {
      error_message="Supremum not allowed in RES: " + pp(x);
    }
  }

  void enter(const sum& x)
  {
    result = false;
    if (error_message.empty())
    {
      error_message="Supremum not allowed in RES: " + pp(x);
    }
  }

  void enter(const data::data_expression& x)
  {
    if (x != data::true_() && x != data::false_() && x.sort()!=data::sort_real::real_())
    {
      result = false;
      if (error_message.empty())
      {
        error_message="Expression in a RES can only be true, false or a real number: " + pp(x);
      }
    }
  }

  void enter(const propositional_variable_instantiation& x)
  {
    if (result)
    {
      result = x.parameters().empty();
      if (error_message.empty())
      {
        error_message="A propositional variable in a RES cannot have arguments: " + pp(x);
      }
    }
  }

  void enter(const pres_equation& x)
  {
    if (result)
    {
      result = x.variable().parameters().empty();
      if (error_message.empty())
      {
        error_message="The defined variable in a RES equation cannot have arguments: " + pres_system::pp(x.variable());
      }
    }
  }
};
/// \endcond

/// \brief Returns true if a PRES object is in BES form.
/// \param x a PRES object
template <typename T>
bool is_res(const T& x)
{
  is_res_traverser f;
  f.apply(x);
  return f.result;
}

/// \brief Returns true if a PRES object is in BES form.
/// \param x a PRES object
template <typename T>
bool is_res(const T& x, std::string& error_message)
{
  is_res_traverser f;
  f.apply(x);
  error_message=f.error_message;
  return f.result;
}

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_IS_BES_H
