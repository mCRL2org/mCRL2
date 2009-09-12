// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bes_common.h
/// This file implements common functionality missing in the official BES
/// library. Some of the implementations are workarounds for issues that
/// should be fixed in the library, some is missing functionality.

#ifndef MCRL2_BES_EXAMPLE_BES_COMMON_H
#define MCRL2_BES_EXAMPLE_BES_COMMON_H

#include <algorithm>
#include <boost/bind.hpp>

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/pbes/bes.h"
#include "mcrl2/core/detail/join.h"

namespace mcrl2 {

  namespace bes {

    /// FIXME: Workaround circumventing the problem of using find_all_if in
    /// combination with tr::is_variable
    inline
    bool is_variable(const atermpp::aterm_appl& t)
    {
      if(core::detail::check_rule_BooleanExpression(t))
      {
        return core::term_traits<boolean_expression>::is_variable(t);
      }
      else
      {
        return false;
      }
    }
    
    /// FIXME: Workaround circumventing the problem of using find_all_if in
    /// combination with tr::is_true
    inline
    bool is_true(const atermpp::aterm_appl& t)
    {
      if(core::detail::check_rule_BooleanExpression(t))
      {
        return core::term_traits<boolean_expression>::is_true(t);
      }
      else
      {
        return false;
      }
    }
    
    /// FIXME: Workaround circumventing the problem of using find_all_if in
    /// combination with tr::is_false
    inline
    bool is_false(const atermpp::aterm_appl& t)
    {
      if(core::detail::check_rule_BooleanExpression(t))
      {
        return core::term_traits<boolean_expression>::is_false(t);
      }
      else
      {
        return false;
      }
    }
    
    /// \brief Returns or applied to the sequence of boolean expressions [first, last)
    /// \param first Start of a sequence of boolean expressions
    /// \param last End of a sequence of boolean expressions
    /// \return Or applied to the sequence of boolean expressions [first, last)
    template <typename FwdIt>
    boolean_expression join_or(FwdIt first, FwdIt last)
    {
      typedef core::term_traits<boolean_expression> tr;
      return core::detail::join(first, last, tr::or_, tr::false_());
    }
    
    /// \brief Returns and applied to the sequence of boolean expressions [first, last)
    /// \param first Start of a sequence of boolean expressions
    /// \param last End of a sequence of boolean expressions
    /// \return And applied to the sequence of boolean expressions [first, last)
    template <typename FwdIt>
    boolean_expression join_and(FwdIt first, FwdIt last)
    {
      typedef core::term_traits<boolean_expression> tr;
      return core::detail::join(first, last, tr::and_, tr::true_());
    }
    
    /// \brief Splits a disjunction into a sequence of operands
    /// Given a pbes expression of the form p1 || p2 || .... || pn, this will yield a
    /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main
    /// function symbol.
    /// \param expr A PBES expression
    /// \return A sequence of operands
    inline
    atermpp::set<boolean_expression> split_or(const boolean_expression& expr)
    {
      typedef core::term_traits<boolean_expression> tr;
      atermpp::set<boolean_expression> result;
      core::detail::split(expr, std::insert_iterator<atermpp::set<boolean_expression> >(result, result.begin()), tr::is_or, tr::left, tr::right);
      return result;
    }
    
      /// \brief Splits a conjunction into a sequence of operands
      /// Given a pbes expression of the form p1 && p2 && .... && pn, this will yield a
      /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main
      /// function symbol.
      /// \param expr A PBES expression
      /// \return A sequence of operands
    inline
    atermpp::set<boolean_expression> split_and(const boolean_expression& expr)
    {
      typedef core::term_traits<boolean_expression> tr;
      atermpp::set<boolean_expression> result;
      core::detail::split(expr, std::insert_iterator<atermpp::set<boolean_expression> >(result, result.begin()), tr::is_and, tr::left, tr::right);
      return result;
    }

    /// \brief Determines whether a BES is in standard form
    /// \param bes a boolean equation system.
    /// \return true iff bes is in standard form.
    template <typename Container>
    inline
    bool is_standard_form(boolean_equation_system<Container> const& bes)
    {
      for(typename Container::const_iterator i = bes.equations().begin(); i != bes.equations().end(); ++i)
      {
        if(!is_standard_form(i->formula()))
        {
          return false;
        }
      }
      return true;
    }

    /// \brief Determines whether the constant true occurs in bes
    /// \param bes a boolean equation system.
    /// \return true iff the constant true occurs in bes.
    template <typename Container>
    inline
    bool has_true(boolean_equation_system<Container> const& bes)
    {
      for (typename Container::const_iterator i = bes.equations().begin(); i != bes.equations().end(); ++i)
      {
        if(atermpp::find_if(i->formula(), is_true))
        {
          return true;
        }
      }
      return false;
    }
    
    /// \brief Determines whether the constant false occurs in bes
    /// \param bes a boolean equation system.
    /// \return true iff the constant false occurs in bes.
    template <typename Container>
    inline
    bool has_false(boolean_equation_system<Container> const& bes)
    {
      for (typename Container::const_iterator i = bes.equations().begin(); i != bes.equations().end(); ++i)
      {
        if(atermpp::find_if(i->formula(), is_false))
        {
          return true;
        }
      }
      return false;
    }

  }
}

#endif

