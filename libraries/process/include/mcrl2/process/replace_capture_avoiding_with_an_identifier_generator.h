// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/replace_capture_avoiding_with_an_identifier_generator.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_REPLACE_CAPTURE_AVOIDING_WITH_AN_IDENTIFIER_GENERATOR_H
#define MCRL2_PROCESS_REPLACE_CAPTURE_AVOIDING_WITH_AN_IDENTIFIER_GENERATOR_H

#include "mcrl2/data/replace_capture_avoiding_with_an_identifier_generator.h"
#include "mcrl2/process/add_binding.h"
#include "mcrl2/process/builder.h"

namespace mcrl2::process
{

namespace detail {

// Below the definitions are given for capture avoiding subsitution witn an identifier generator.
template <template <class> class Builder, class Derived, class Substitution, class IdentifierGenerator>
struct add_capture_avoiding_replacement_with_an_identifier_generator: public data::detail::add_capture_avoiding_replacement_with_an_identifier_generator<Builder, Derived, Substitution, IdentifierGenerator>
{
  using super = data::detail::add_capture_avoiding_replacement_with_an_identifier_generator<Builder,
      Derived,
      Substitution,
      IdentifierGenerator>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;
  using super::update_sigma;

  data::assignment_list::const_iterator find_variable(const data::assignment_list& a, const data::variable& v) const
  {
    for (data::assignment_list::const_iterator i = a.begin(); i != a.end(); ++i)
    {
      if (i->lhs() == v)
      {
        return i;
      }
    }
    return a.end();
  }

  add_capture_avoiding_replacement_with_an_identifier_generator(Substitution& sigma, IdentifierGenerator& id_generator)
    : super(sigma, id_generator)
  { }

  template <class T>
  void apply(T& result, const process::process_instance_assignment& x)
  {
    static_cast<Derived&>(*this).enter(x);
    const data::assignment_list& a = x.assignments();
    std::vector<data::assignment> v;

    for (const data::variable& variable: x.identifier().variables())
    {
      const data::assignment_list::const_iterator k = find_variable(a, variable);
      if (k == a.end())
      {
        data::data_expression e;
        apply(e, variable);
        if (e != variable)
        {
          v.emplace_back(variable, e);
        }
      }
      else
      {
        data::data_expression rhs;
        apply(rhs, k->rhs());
        v.emplace_back(k->lhs(), rhs);
      }
    }
    process::make_process_instance_assignment(result, x.identifier(), data::assignment_list(v.begin(), v.end()));
    static_cast<Derived&>(*this).leave(x);
  }

  template <class T>
  void apply(T& result, const sum& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    process_expression body;
    apply(body, x.operand());
    make_sum(result, v, body);
    update_sigma.pop(v);
  }

  template <class T>
  void apply(T& result, const stochastic_operator& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    process_expression body;
    apply(body, x.operand());
    data::data_expression dist;
    apply(dist, x.distribution());
    make_stochastic_operator(result, v, dist, body);
    update_sigma.pop(v);
  }
};

template <template <class> class Builder, template <template <class> class, class, class, class> class Binder, class Substitution, class IdentifierGenerator>
struct replace_capture_avoiding_variables__with_an_identifier_generator_builder: public Binder<Builder, replace_capture_avoiding_variables__with_an_identifier_generator_builder<Builder, Binder, Substitution, IdentifierGenerator>, Substitution, IdentifierGenerator>
{
  using super = Binder<Builder,
      replace_capture_avoiding_variables__with_an_identifier_generator_builder<Builder,
          Binder,
          Substitution,
          IdentifierGenerator>,
      Substitution,
      IdentifierGenerator>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  replace_capture_avoiding_variables__with_an_identifier_generator_builder(Substitution& sigma, IdentifierGenerator& id_generator)
    : super(sigma, id_generator)
  { }
};

template <template <class> class Builder, template <template <class> class, class, class, class> class Binder, class Substitution, class IdentifierGenerator>
replace_capture_avoiding_variables__with_an_identifier_generator_builder<Builder, Binder, Substitution, IdentifierGenerator>
apply_replace_capture_avoiding_variables__with_an_identifier_generator_builder(Substitution& sigma, IdentifierGenerator& id_generator)
{
  return replace_capture_avoiding_variables__with_an_identifier_generator_builder<Builder, Binder, Substitution, IdentifierGenerator>(sigma, id_generator);
}

} // namespace detail

//--- start generated process replace_capture_avoiding_with_identifier_generator code ---//
/// \\brief Applies sigma as a capture avoiding substitution to x using an identifier generator.
/// \\details This substitution function is much faster than replace_variables_capture_avoiding, but
///          it requires an identifier generator that generates strings for fresh variables. These
///          strings must be unique in the sense that they have not been used for other variables.
/// \\param x The object to which the subsitution is applied.
/// \\param sigma A mutable substitution of which it can efficiently be checked whether a variable occurs in its
///              right hand side. The class maintain_variables_in_rhs is useful for this purpose.
/// \\param id_generator A generator that generates unique strings, not yet used as variable names.

template <typename T, typename Substitution, typename IdentifierGenerator>
void replace_variables_capture_avoiding_with_an_identifier_generator(T& x,
                       Substitution& sigma,
                       IdentifierGenerator& id_generator
                      )
  requires (!std::is_base_of_v<atermpp::aterm, T>)
{
  data::detail::apply_replace_capture_avoiding_variables_builder_with_an_identifier_generator<process::data_expression_builder, process::detail::add_capture_avoiding_replacement_with_an_identifier_generator>(sigma, id_generator).update(x);
}

/// \\brief Applies sigma as a capture avoiding substitution to x using an identifier generator..
/// \\details This substitution function is much faster than replace_variables_capture_avoiding, but
///          it requires an identifier generator that generates strings for fresh variables. These
///          strings must be unique in the sense that they have not been used for other variables.
/// \\param x The object to which the substiution is applied.
/// \\param sigma A mutable substitution of which it can efficiently be checked whether a variable occurs in its
///              right hand side. The class maintain_variables_in_rhs is useful for this purpose.
/// \\param id_generator A generator that generates unique strings, not yet used as variable names.
/// \\return The result is the term x to which sigma has been applied.
template <typename T, typename Substitution, typename IdentifierGenerator>
T replace_variables_capture_avoiding_with_an_identifier_generator(const T& x,
                    Substitution& sigma,
                    IdentifierGenerator& id_generator
                   )
  requires std::is_base_of_v<atermpp::aterm, T>
{
  T result;
  data::detail::apply_replace_capture_avoiding_variables_builder_with_an_identifier_generator<process::data_expression_builder, process::detail::add_capture_avoiding_replacement_with_an_identifier_generator>(sigma, id_generator).apply(result, x);
  return result;
}
//--- end generated process replace_capture_avoiding_with_identifier_generator code ---//

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_REPLACE_CAPTURE_AVOIDING_WITH_AN_IDENTIFIER_GENERATOR_H
