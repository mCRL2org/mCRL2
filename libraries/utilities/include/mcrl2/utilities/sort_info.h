// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/sort_info.h
/// \brief Interface to classes Sort_Info

#ifndef SORT_INFO_H
#define SORT_INFO_H

#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/detail/struct.h"

/// \brief Class that provides information on sort expressions.
class Sort_Info {

  private:
    /// \brief ATermAppl representing the sort Int.
    ATermAppl f_sort_id_int;

    /// \brief ATermAppl representing the sort Nat.
    ATermAppl f_sort_id_nat;

    /// \brief ATermAppl representing the sort Pos.
    ATermAppl f_sort_id_pos;

    /// \brief ATermAppl representing the sort Real.
    ATermAppl f_sort_id_real;

    /// \brief ATermAppl representing the sort Bool.
    ATermAppl f_sort_id_bool;

  public:

    /// \brief Constructor that initializes the fields of the class.
    inline Sort_Info() {
      f_sort_id_int = mcrl2::core::detail::gsMakeSortIdInt();
      f_sort_id_nat = mcrl2::core::detail::gsMakeSortIdNat();
      f_sort_id_pos = mcrl2::core::detail::gsMakeSortIdPos();
      f_sort_id_real = mcrl2::core::detail::gsMakeSortIdReal();
      f_sort_id_bool = mcrl2::core::detail::gsMakeSortIdBool();
    }

    /// \brief Indicates whether or not a sort expression is a base type.
    inline bool is_sort_id(ATermAppl a_sort_expression) {
      return mcrl2::core::detail::gsIsSortId(a_sort_expression);
    }

    /// \brief Indicates whether or not the main operator of a sort expression is an arrow product.
    inline bool is_sort_arrow_prod(ATermAppl a_sort_expression) {
      return mcrl2::core::detail::gsIsSortArrow(a_sort_expression);
    }

    /// \brief Indicates whether or not a sort expression represents the sort Int.
    inline bool is_sort_int(ATermAppl a_sort_expression) {
      return a_sort_expression == f_sort_id_int;
    }

    /// \brief Indicates whether or not a sort expression represents the sort Nat.
    inline bool is_sort_nat(ATermAppl a_sort_expression) {
      return a_sort_expression == f_sort_id_nat;
    }

    /// \brief Indicates whether or not a sort expression represents the sort Pos.
    inline bool is_sort_pos(ATermAppl a_sort_expression) {
      return a_sort_expression == f_sort_id_pos;
    }

    /// \brief Indicates whether or not a sort expression represents the sort Real.
    inline bool is_sort_real(ATermAppl a_sort_expression) {
      return a_sort_expression == f_sort_id_real;
    }

    /// \brief Indicates whether or not a sort expression represents the sort Bool.
    inline bool is_sort_bool(ATermAppl a_sort_expression) {
      return a_sort_expression == f_sort_id_bool;
    }

    /// \brief Indicates whether or not a sort expression represents the sort List.
    inline bool is_sort_list(ATermAppl a_sort_expression) {
      return strncmp(
        mcrl2::core::detail::gsATermAppl2String(mcrl2::core::ATAgetArgument(a_sort_expression, 0)), "List@", 5) == 0;
    }

    /// \brief Indicates whether or not a sort expression represents the sort Bool.
    inline bool returns_bool(ATermAppl a_sort_expression) {
      return mcrl2::core::detail::gsGetSortExprResult(a_sort_expression) == f_sort_id_bool;
    }

    /// \brief Returns the ID of the sort.
    inline char* get_sort_id(ATermAppl a_sort_expression) {
      return mcrl2::core::detail::gsATermAppl2String(mcrl2::core::ATAgetArgument(a_sort_expression, 0));
    }

    /// \brief Returns the domain of a sort expression.
    inline ATermList get_domain(ATermAppl a_sort_expression) {
      return mcrl2::core::detail::gsGetSortExprDomain(a_sort_expression);
    }

    /// \brief Returns the range of a sort expression.
    inline ATermAppl get_range(ATermAppl a_sort_expression) {
      return mcrl2::core::detail::gsGetSortExprResult(a_sort_expression);
    }

    /// \brief Returns the number of arguments accepted by a function with this type;
    inline int get_number_of_arguments(ATermAppl a_sort_expression) {
      if (is_sort_arrow_prod(a_sort_expression)) {
        return ATgetLength(mcrl2::core::ATLgetArgument(a_sort_expression, 0));
      }

      return 0;
    }

    /// \brief Returns the type of the argument with the given number.
    inline ATermAppl get_type_of_argument(ATermAppl a_sort_expression, int a_number) {
      return mcrl2::core::ATAelementAt(get_domain(a_sort_expression), a_number);
    }

    /// \brief Returns the result type of a sort expression.
    inline ATermAppl get_result_type(ATermAppl a_sort_expression) {
      return mcrl2::core::ATAgetArgument(a_sort_expression, 1);
    }
};

#endif
