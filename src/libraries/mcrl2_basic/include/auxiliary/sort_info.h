// Interface to classes Sort_Info
// file: sort_info.h

#ifndef SORT_INFO_H
#define SORT_INFO_H

#include "aterm2.h"

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
    Sort_Info();

    /// \brief Indicates whether or not a sort expression is a base type.
    bool is_sort_id(ATermAppl a_sort_expression);

    /// \brief Indicates whether or not the main operator of a sort expression is an arrow.
    bool is_sort_arrow(ATermAppl a_sort_expression);

    /// \brief Indicates whether or not a sort expression represents the sort Int.
    bool is_sort_int(ATermAppl a_sort_expression);

    /// \brief Indicates whether or not a sort expression represents the sort Nat.
    bool is_sort_nat(ATermAppl a_sort_expression);

    /// \brief Indicates whether or not a sort expression represents the sort Pos.
    bool is_sort_pos(ATermAppl a_sort_expression);

    /// \brief Indicates whether or not a sort expression represents the sort Real.
    bool is_sort_real(ATermAppl a_sort_expression);

    /// \brief Indicates whether or not a sort expression represents the sort Bool.
    bool is_sort_bool(ATermAppl a_sort_expression);

    /// \brief Indicates whether or not a sort expression represents the sort List.
    bool is_sort_list(ATermAppl a_sort_expression);

    /// \brief Indicates whether or not a sort expression represents the sort Bool.
    bool returns_bool(ATermAppl a_sort_expression);

    /// \brief Returns the ID of the sort.
    char* get_sort_id(ATermAppl a_sort_expression);

    /// \brief Returns the domain of a sort expression.
    ATermAppl get_domain(ATermAppl a_sort_expression);

    /// \brief Returns the range of a sort expression.
    ATermAppl get_range(ATermAppl a_sort_expression);

    /// \brief Returns the number of arguments accepted by a function with this type;
    int get_number_of_arguments(ATermAppl a_sort_expression);

    /// \brief Returns the type of the argument with the given number.
    ATermAppl get_type_of_argument(ATermAppl a_sort_expression, int a_number);

    /// \brief Returns the result type of a sort expression.
    ATermAppl get_result_type(ATermAppl a_sort_expression);
};

#endif
