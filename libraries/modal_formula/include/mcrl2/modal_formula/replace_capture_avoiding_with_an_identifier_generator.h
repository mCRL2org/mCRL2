// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/replace_capture_avoiding_with_an_identifier_generator.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_REPLACE_CAPTURE_AVOIDING_WITH_AN_IDENTIFIER_GENERATOR_H
#define MCRL2_MODAL_FORMULA_REPLACE_CAPTURE_AVOIDING_WITH_AN_IDENTIFIER_GENERATOR_H

#include "mcrl2/lps/replace_capture_avoiding_with_an_identifier_generator.h"
#include "mcrl2/modal_formula/add_binding.h"
#include "mcrl2/modal_formula/builder.h"

namespace mcrl2 {

namespace action_formulas {

namespace detail {

// Below the functions to do a capture avoiding replacement with an identifier generator are given.
template <template <class> class Builder, class Derived, class Substitution, class IdentifierGenerator>
struct add_capture_avoiding_replacement_with_an_identifier_generator: public lps::detail::add_capture_avoiding_replacement_with_an_identifier_generator<Builder, Derived, Substitution, IdentifierGenerator>
{
  typedef lps::detail::add_capture_avoiding_replacement_with_an_identifier_generator<Builder, Derived, Substitution, IdentifierGenerator> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
  using super::update_sigma;

  template <class T>
  void apply(T& result, const forall& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    action_formula body;
    apply(body, x.body());
    make_forall(result, v, body);
    update_sigma.pop(v);
  }

  template <class T>
  void apply(T& result, const exists& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    action_formula body;
    apply(body, x.body());
    make_exists(result, v, body);
    update_sigma.pop(v);
  }

  add_capture_avoiding_replacement_with_an_identifier_generator(Substitution& sigma, IdentifierGenerator& id_generator)
    : super(sigma, id_generator)
  { }
};

} // namespace detail

//--- start generated action_formulas replace_capture_avoiding_with_identifier_generator code ---//
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
  data::detail::apply_replace_capture_avoiding_variables_builder_with_an_identifier_generator<action_formulas::data_expression_builder, action_formulas::detail::add_capture_avoiding_replacement_with_an_identifier_generator>(sigma, id_generator).update(x);
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
  data::detail::apply_replace_capture_avoiding_variables_builder_with_an_identifier_generator<action_formulas::data_expression_builder, action_formulas::detail::add_capture_avoiding_replacement_with_an_identifier_generator>(sigma, id_generator).apply(result, x);
  return result;
}
//--- end generated action_formulas replace_capture_avoiding_with_identifier_generator code ---//

} // namespace action_formulas

} // namespace mcrl2

namespace mcrl2 {

namespace regular_formulas {

namespace detail {

// Below the code is provided for a capture avoiding replacement with an identifier generator.
template <template <class> class Builder, class Derived, class Substitution, class IdentifierGenerator>
struct add_capture_avoiding_replacement_with_an_identifier_generator: public action_formulas::detail::add_capture_avoiding_replacement_with_an_identifier_generator<Builder, Derived, Substitution, IdentifierGenerator>
{
  typedef action_formulas::detail::add_capture_avoiding_replacement_with_an_identifier_generator<Builder, Derived, Substitution, IdentifierGenerator> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
  using super::update_sigma;

  add_capture_avoiding_replacement_with_an_identifier_generator(Substitution& sigma, IdentifierGenerator& id_generator)
    : super(sigma, id_generator)
  { }
};

template <template <class> class Builder, class Substitution, class IdentifierGenerator>
add_capture_avoiding_replacement_with_an_identifier_generator<Builder, class Derived, Substitution, IdentifierGenerator>
make_add_capture_avoiding_replacement_with_an_identifier_generator(Substitution& sigma, IdentifierGenerator& id_generator)
{
  return add_capture_avoiding_replacement_with_an_identifier_generator<Builder, Derived, Substitution, IdentifierGenerator>(sigma, id_generator);
}

} // namespace detail

//--- start generated regular_formulas replace_capture_avoiding_with_identifier_generator code ---//
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
  data::detail::apply_replace_capture_avoiding_variables_builder_with_an_identifier_generator<regular_formulas::data_expression_builder, regular_formulas::detail::add_capture_avoiding_replacement_with_an_identifier_generator>(sigma, id_generator).update(x);
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
  data::detail::apply_replace_capture_avoiding_variables_builder_with_an_identifier_generator<regular_formulas::data_expression_builder, regular_formulas::detail::add_capture_avoiding_replacement_with_an_identifier_generator>(sigma, id_generator).apply(result, x);
  return result;
}
//--- end generated regular_formulas replace_capture_avoiding_with_identifier_generator code ---//

} // namespace regular_formulas

} // namespace mcrl2

namespace mcrl2 {

namespace state_formulas {

namespace detail {

// Below the code for add capture avoiding replacment with an identifier generator is provided.

template <template <class> class Builder, class Derived, class Substitution, class IdentifierGenerator>
struct add_capture_avoiding_replacement_with_an_identifier_generator: public data::detail::add_capture_avoiding_replacement_with_an_identifier_generator<Builder, Derived, Substitution, IdentifierGenerator>
{
  typedef data::detail::add_capture_avoiding_replacement_with_an_identifier_generator<Builder, Derived, Substitution, IdentifierGenerator> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
  using super::sigma;
  using super::update_sigma;

  state_formula operator()(const forall& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    state_formula result = forall(v, (*this)(x.body()));
    update_sigma.pop(v);
    return result;
  }

  state_formula operator()(const exists& x)
  {
    data::variable_list v = update_sigma.push(x.variables());
    state_formula result = exists(v, (*this)(x.body()));
    update_sigma.pop(v);
    return result;
  }

  add_capture_avoiding_replacement_with_an_identifier_generator(Substitution& sigma, IdentifierGenerator& id_generator)
    : super(sigma, id_generator)
  { }
};

template <template <class> class Builder, class Substitution, class IdentifierGenerator>
add_capture_avoiding_replacement_with_an_identifier_generator<Builder, class Derived, Substitution, IdentifierGenerator>
make_add_capture_avoiding_replacement_with_an_identifier_generator(Substitution& sigma, IdentifierGenerator& id_generator)
{
  return add_capture_avoiding_replacement_with_an_identifier_generator<Builder, Derived, Substitution, IdentifierGenerator>(sigma, id_generator);
}

} // namespace detail

//--- start generated state_formulas replace_capture_avoiding_with_identifier_generator code ---//
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
  data::detail::apply_replace_capture_avoiding_variables_builder_with_an_identifier_generator<state_formulas::data_expression_builder, state_formulas::detail::add_capture_avoiding_replacement_with_an_identifier_generator>(sigma, id_generator).update(x);
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
  data::detail::apply_replace_capture_avoiding_variables_builder_with_an_identifier_generator<state_formulas::data_expression_builder, state_formulas::detail::add_capture_avoiding_replacement_with_an_identifier_generator>(sigma, id_generator).apply(result, x);
  return result;
}
//--- end generated state_formulas replace_capture_avoiding_with_identifier_generator code ---//

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_REPLACE_CAPTURE_AVOIDING_WITH_AN_IDENTIFIER_GENERATOR_H
