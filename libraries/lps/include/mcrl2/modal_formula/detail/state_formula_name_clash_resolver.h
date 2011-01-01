// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/state_formula_name_clash_resolver.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_DETAIL_STATE_FORMULA_NAME_CLASH_RESOLVER_H
#define MCRL2_MODAL_FORMULA_DETAIL_STATE_FORMULA_NAME_CLASH_RESOLVER_H

#include <set>
#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/identifier_generator.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/modal_formula/builder.h"
#include "mcrl2/modal_formula/state_formula.h"

namespace mcrl2 {

namespace state_formulas {

namespace detail {

  template <typename Derived>
  class state_formula_name_clash_resolver: public state_formulas::state_formula_builder<Derived>
  {
    public:
      typedef state_formulas::state_formula_builder<Derived> super;
  
      using super::enter;
      using super::leave;
      using super::operator();

      typedef atermpp::map<core::identifier_string, atermpp::vector<core::identifier_string> > name_map;

      /// \brief The stack of names.
      name_map m_names;

      /// \brief Generator for fresh variable names.
      core::number_postfix_generator m_generator;

      /// \brief Pops the name of the stack
      void pop(const core::identifier_string& name)
      {
        //std::cout << "<pop>" << name << std::endl;
        //std::cout << "<m_names[" << name << "] =" << core::detail::print_list(m_names[name]) << std::endl;
        m_names[name].pop_back();
      }
      
      /// \brief Pushes name on the stack.
      void push(const core::identifier_string& name)
      {
        //std::cout << "<push>" << name << std::endl;
        atermpp::vector<core::identifier_string>& names = m_names[name];
        if (names.empty())
        {
          names.push_back(name);
        }
        else
        {
          names.push_back(m_generator(std::string(name) + "_"));
        }
        //std::cout << "<m_names[" << name << "] =" << core::detail::print_list(m_names[name]) << std::endl;
      }    

      void enter(const mu& x)
      {
        push(x.name());
      }
  
      void leave(const mu& x)
      {
        pop(x.name());
      }
  
      void enter(const nu& x)
      {
        push(x.name());
      }
  
      void leave(const nu& x)
      {
        pop(x.name());
      }

      // Rename variable
      state_formula operator()(const mu& x)
      {
        enter(x);
        state_formula result = mu(m_names[x.name()].back(), x.assignments(), (*this)(x.operand()));
        leave(x);
        return result;
      }
 
      // Rename variable
      state_formula operator()(const nu& x)
      {
        enter(x);
        state_formula result = nu(m_names[x.name()].back(), x.assignments(), (*this)(x.operand()));
        leave(x);
        return result;
      }

      // Rename variable
      state_formula operator()(const variable& x)
      {
        return variable(m_names[x.name()].back(), x.arguments());
      } 
  };

  /// \brief Resolves name clashes in state formula f
  inline
  state_formula resolve_name_clashes(const state_formula& f)
  {
    core::apply_builder<state_formula_name_clash_resolver> builder;
    return builder(f);
  }

} // namespace detail

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_DETAIL_STATE_FORMULA_NAME_CLASH_RESOLVER_H
