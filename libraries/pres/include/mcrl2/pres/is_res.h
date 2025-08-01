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



namespace mcrl2::pres_system {

/// \cond INTERNAL_DOCS
/// \brief Visitor for checking if a pres object is in BES form.
struct is_res_traverser: public pres_expression_traverser<is_res_traverser>
{
  using super = pres_expression_traverser<is_res_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result=true;
  std::string error_message;

  std::string get_error_message() const
  {
    assert(result==false);
    return error_message;
  }

  is_res_traverser() = default;

  void enter(const infimum& x)
  {
    if (result)
    {
      result = false;
      error_message="Infimum not allowed in RES: " + pp(x);
    }
  }

  void enter(const supremum& x)
  {
    if (result)
    {
      result = false;
      error_message="Supremum not allowed in RES: " + pp(x);
    }
  }

  void enter(const sum& x)
  {
    if (result)
    { 
      result = false;
      error_message="Sum not allowed in RES: " + pp(x);
    }
  }

  void enter(const data::data_expression& x)
  {
    if (result && x != data::true_() && x != data::false_() && x.sort()!=data::sort_real::real_())
    {
      result = false;
      error_message="Expression in a RES can only be true, false or a real number: " + pp(x);
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
  if (!f.result)
  {
    error_message = f.get_error_message();
  }
  return f.result;
}

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_IS_BES_H
