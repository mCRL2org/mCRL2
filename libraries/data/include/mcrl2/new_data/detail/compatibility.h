// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/detail/compatibility.h
/// \brief Conversion utilities for transforming between terms in the old
//         datatype format and the new format.

#ifndef MCRL2_DATA_DETAIL_COMPATIBILITY_H
#define MCRL2_DATA_DETAIL_COMPATIBILITY_H

#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/new_data/sort_expression.h"
#include "mcrl2/new_data/function_symbol.h"
#include "mcrl2/new_data/data_equation.h"

namespace mcrl2 {

  namespace new_data {

    namespace detail {

      /// \brief Convert a sort specification in the old new_data format before new_data
      ///        implementation to a list of sort expressions in the new new_data format.
      /// \param sort_spec A sort specification in the old new_data format before
      ///                 new_data implementation.
      /// \return A list of sort expressions in the new new_data format, containing
      ///      exactly one equivalent for each sort in sort_spec.
      inline
      atermpp::set< sort_expression > aterm_sort_spec_to_sort_expression_set(const atermpp::aterm_appl& sort_spec)
      {
        assert(core::detail::gsIsSortSpec(sort_spec));

        atermpp::term_list<atermpp::aterm_appl> sl = list_arg1(sort_spec);
        return atermpp::set< sort_expression >(sl.begin(), sl.end());
      }

      /// \brief Convert a constructor specification in the old new_data format
      ///        before new_data implementation to a mapping of sort expressions to
      ///        constructor declarations in the new new_data format.
      /// \param cons_spec A constructor specification in the old new_data format
      ///        before new_data implementation.
      /// \return A mapping of sort expressions to the corresponding constructor
      ///      declarations in the new new_data format.
      inline
      atermpp::multimap<sort_expression, function_symbol> aterm_cons_spec_to_constructor_map(const atermpp::aterm_appl& cons_spec)
      {
        assert(core::detail::gsIsConsSpec(cons_spec));

        atermpp::multimap<sort_expression, function_symbol> constructors;

        for(atermpp::term_list_iterator<function_symbol> i(atermpp::list_arg1(cons_spec));
                                          i != atermpp::term_list_iterator<function_symbol>(); ++i)
        {
           constructors.insert(std::make_pair(i->sort().target_sort(), *i));
        }

        return constructors;
      }

      /// \brief Convert a map specification in the old new_data format before new_data
      ///        implementation to a list of function symbols in the new new_data format.
      /// \param map_spec A map specification in the old new_data format before
      ///                 new_data implementation.
      /// \return A list of function declaration in the new new_data format, containing
      ///      exactly one equivalent for each function in map_spec.
      inline
      atermpp::set< function_symbol > aterm_map_spec_to_function_set(const atermpp::aterm_appl& map_spec)
      {
        assert(core::detail::gsIsMapSpec(map_spec));

        atermpp::term_list<atermpp::aterm_appl> fl = atermpp::list_arg1(map_spec);
        return atermpp::set< function_symbol >(fl.begin(), fl.end());
      }

      /// \brief Convert an equation specification in the old new_data format before new_data
      ///        implementation to a list of new_data equations in the new new_data format.
      /// \param eqn_spec An equation specification in the old new_data format before
      ///                 new_data implementation.
      /// \return A list of new_data equations in the new new_data format, containing
      ///      exactly one equivalent for each equation in eqn_spec.
      inline
      atermpp::set< data_equation > aterm_data_eqn_spec_to_equation_set(const atermpp::aterm_appl& eqn_spec)
      {
        assert(core::detail::gsIsDataEqnSpec(eqn_spec));

        atermpp::term_list<atermpp::aterm_appl> el = atermpp::list_arg1(eqn_spec);
        return atermpp::set< data_equation >(el.begin(), el.end());
      }

    } // namespace detail

  } // namespace new_data

} // namespace mcrl2

#endif //MCRL2_DATA_DETAIL_COMPATIBILITY_H

