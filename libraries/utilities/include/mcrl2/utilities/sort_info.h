// Author(s): Luc Engelen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/mcrl2/utilities/sort_info.h
/// \brief Add your file description here.

// Interface to classes Sort_Info
// file: sort_info.h

#ifndef SORT_INFO_H
#define SORT_INFO_H

#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/core/struct.h"

using namespace mcrl2::utilities;

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
      f_sort_id_int = gsMakeSortIdInt();
      f_sort_id_nat = gsMakeSortIdNat();
      f_sort_id_pos = gsMakeSortIdPos();
      f_sort_id_real = gsMakeSortIdReal();
      f_sort_id_bool = gsMakeSortIdBool();
    }

    /// \brief Indicates whether or not a sort expression is a base type.
    inline bool is_sort_id(ATermAppl a_sort_expression) {
      return gsIsSortId(a_sort_expression);
    }

    /// \brief Indicates whether or not the main operator of a sort expression is an arrow product.
    inline bool is_sort_arrow_prod(ATermAppl a_sort_expression) {
      return gsIsSortArrow(a_sort_expression);
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
        gsATermAppl2String(ATAgetArgument(a_sort_expression, 0)), "List@", 5) == 0;
    }

    /// \brief Indicates whether or not a sort expression represents the sort Bool.
    inline bool returns_bool(ATermAppl a_sort_expression) {
      return gsGetSortExprResult(a_sort_expression) == f_sort_id_bool;
    }

    /// \brief Returns the ID of the sort.
    inline char* get_sort_id(ATermAppl a_sort_expression) {
      return gsATermAppl2String(ATAgetArgument(a_sort_expression, 0));
    }

    /// \brief Returns the domain of a sort expression.
    inline ATermList get_domain(ATermAppl a_sort_expression) {
      return gsGetSortExprDomain(a_sort_expression);
    }

    /// \brief Returns the range of a sort expression.
    inline ATermAppl get_range(ATermAppl a_sort_expression) {
      return gsGetSortExprResult(a_sort_expression);
    }

    /// \brief Returns the number of arguments accepted by a function with this type;
    inline int get_number_of_arguments(ATermAppl a_sort_expression) {
      if (is_sort_arrow_prod(a_sort_expression)) {
        return ATgetLength(ATLgetArgument(a_sort_expression, 0));
      }

      return 0;
    }

    /// \brief Returns the type of the argument with the given number.
    inline ATermAppl get_type_of_argument(ATermAppl a_sort_expression, int a_number) {
      return ATAelementAt(get_domain(a_sort_expression), a_number);
    }

    /// \brief Returns the result type of a sort expression.
    inline ATermAppl get_result_type(ATermAppl a_sort_expression) {
      return ATAgetArgument(a_sort_expression, 1);
    }
};

#endif
