// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/bag.h
/// \brief The standard sort bag.
///
/// This file was generated from the data sort specification
/// mcrl2/data/build/bag.spec.

#ifndef MCRL2_DATA_BAG_H
#define MCRL2_DATA_BAG_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/forall.h"
#include "mcrl2/data/container_sort.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/fbag.h"
#include "mcrl2/data/fset.h"
#include "mcrl2/data/set.h"

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort bag
    namespace sort_bag {

      /// \brief Constructor for sort expression Bag(S)
      /// \param s A sort expression
      /// \return Sort expression bag(s)
      inline
      container_sort bag(const sort_expression& s)
      {
        container_sort bag(bag_container(), s);
        return bag;
      }

      /// \brief Recogniser for sort expression Bag(s)
      /// \param e A sort expression
      /// \return true iff e is a container sort of which the name matches
      ///      bag
      inline
      bool is_bag(const sort_expression& e)
      {
        if (is_container_sort(e))
        {
          return container_sort(e).container_name() == bag_container();
        }
        return false;
      }


      /// \brief Generate identifier \@bag
      /// \return Identifier \@bag
      inline
      core::identifier_string const& constructor_name()
      {
        static core::identifier_string constructor_name = core::identifier_string("@bag");
        return constructor_name;
      }

      /// \brief Constructor for function symbol \@bag
      /// \param s A sort expression
      /// \return Function symbol constructor
      inline
      function_symbol constructor(const sort_expression& s)
      {
        function_symbol constructor(constructor_name(), make_function_sort(make_function_sort(s, sort_nat::nat()), sort_fbag::fbag(s), bag(s)));
        return constructor;
      }

      /// \brief Recogniser for function \@bag
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@bag
      inline
      bool is_constructor_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == constructor_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@bag
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@bag to a number of arguments
      inline
      application constructor(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return sort_bag::constructor(s)(arg0, arg1);
      }

      /// \brief Recogniser for application of \@bag
      /// \param e A data expression
      /// \return true iff e is an application of function symbol constructor to a
      ///     number of arguments
      inline
      bool is_constructor_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_constructor_function_symbol(application(e).head());
        }
        return false;
      }
      /// \brief Give all system defined constructors for bag
      /// \param s A sort expression
      /// \return All system defined constructors for bag
      inline
      function_symbol_vector bag_generate_constructors_code(const sort_expression& s)
      {
        function_symbol_vector result;
        result.push_back(sort_bag::constructor(s));

        return result;
      }

      /// \brief Generate identifier \@bagfbag
      /// \return Identifier \@bagfbag
      inline
      core::identifier_string const& bag_fbag_name()
      {
        static core::identifier_string bag_fbag_name = core::identifier_string("@bagfbag");
        return bag_fbag_name;
      }

      /// \brief Constructor for function symbol \@bagfbag
      /// \param s A sort expression
      /// \return Function symbol bag_fbag
      inline
      function_symbol bag_fbag(const sort_expression& s)
      {
        function_symbol bag_fbag(bag_fbag_name(), make_function_sort(sort_fbag::fbag(s), bag(s)));
        return bag_fbag;
      }

      /// \brief Recogniser for function \@bagfbag
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@bagfbag
      inline
      bool is_bag_fbag_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == bag_fbag_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@bagfbag
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@bagfbag to a number of arguments
      inline
      application bag_fbag(const sort_expression& s, const data_expression& arg0)
      {
        return sort_bag::bag_fbag(s)(arg0);
      }

      /// \brief Recogniser for application of \@bagfbag
      /// \param e A data expression
      /// \return true iff e is an application of function symbol bag_fbag to a
      ///     number of arguments
      inline
      bool is_bag_fbag_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_bag_fbag_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@bagcomp
      /// \return Identifier \@bagcomp
      inline
      core::identifier_string const& bag_comprehension_name()
      {
        static core::identifier_string bag_comprehension_name = core::identifier_string("@bagcomp");
        return bag_comprehension_name;
      }

      /// \brief Constructor for function symbol \@bagcomp
      /// \param s A sort expression
      /// \return Function symbol bag_comprehension
      inline
      function_symbol bag_comprehension(const sort_expression& s)
      {
        function_symbol bag_comprehension(bag_comprehension_name(), make_function_sort(make_function_sort(s, sort_nat::nat()), bag(s)));
        return bag_comprehension;
      }

      /// \brief Recogniser for function \@bagcomp
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@bagcomp
      inline
      bool is_bag_comprehension_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == bag_comprehension_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@bagcomp
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@bagcomp to a number of arguments
      inline
      application bag_comprehension(const sort_expression& s, const data_expression& arg0)
      {
        return sort_bag::bag_comprehension(s)(arg0);
      }

      /// \brief Recogniser for application of \@bagcomp
      /// \param e A data expression
      /// \return true iff e is an application of function symbol bag_comprehension to a
      ///     number of arguments
      inline
      bool is_bag_comprehension_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_bag_comprehension_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier count
      /// \return Identifier count
      inline
      core::identifier_string const& count_name()
      {
        static core::identifier_string count_name = core::identifier_string("count");
        return count_name;
      }

      ///\brief Constructor for function symbol count
      /// \param s A sort expression
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol count
      inline
      function_symbol count(const sort_expression& s, const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort(sort_nat::nat());
        function_symbol count(count_name(), make_function_sort(s0, s1, target_sort));
        return count;
      }

      /// \brief Recogniser for function count
      /// \param e A data expression
      /// \return true iff e is the function symbol matching count
      inline
      bool is_count_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == count_name();
        }
        return false;
      }

      /// \brief Application of function symbol count
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of count to a number of arguments
      inline
      application count(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return sort_bag::count(s, arg0.sort(), arg1.sort())(arg0, arg1);
      }

      /// \brief Recogniser for application of count
      /// \param e A data expression
      /// \return true iff e is an application of function symbol count to a
      ///     number of arguments
      inline
      bool is_count_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_count_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier in
      /// \return Identifier in
      inline
      core::identifier_string const& in_name()
      {
        static core::identifier_string in_name = core::identifier_string("in");
        return in_name;
      }

      ///\brief Constructor for function symbol in
      /// \param s A sort expression
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol in
      inline
      function_symbol in(const sort_expression& s, const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort(sort_bool::bool_());
        function_symbol in(in_name(), make_function_sort(s0, s1, target_sort));
        return in;
      }

      /// \brief Recogniser for function in
      /// \param e A data expression
      /// \return true iff e is the function symbol matching in
      inline
      bool is_in_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == in_name();
        }
        return false;
      }

      /// \brief Application of function symbol in
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of in to a number of arguments
      inline
      application in(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return sort_bag::in(s, arg0.sort(), arg1.sort())(arg0, arg1);
      }

      /// \brief Recogniser for application of in
      /// \param e A data expression
      /// \return true iff e is an application of function symbol in to a
      ///     number of arguments
      inline
      bool is_in_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_in_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier +
      /// \return Identifier +
      inline
      core::identifier_string const& union_name()
      {
        static core::identifier_string union_name = core::identifier_string("+");
        return union_name;
      }

      ///\brief Constructor for function symbol +
      /// \param s A sort expression
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol union_
      inline
      function_symbol union_(const sort_expression& s, const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == bag(s) && s1 == bag(s))
        {
          target_sort = bag(s);
        }
        else if (s0 == sort_set::set_(s) && s1 == sort_set::set_(s))
        {
          target_sort = sort_set::set_(s);
        }
        else if (s0 == sort_fset::fset(s) && s1 == sort_fset::fset(s))
        {
          target_sort = sort_fset::fset(s);
        }
        else if (s0 == sort_fbag::fbag(s) && s1 == sort_fbag::fbag(s))
        {
          target_sort = sort_fbag::fbag(s);
        }
        else
        {
          throw mcrl2::runtime_error("cannot compute target sort for union_ with domain sorts " + to_string(s0) + ", " + to_string(s1));
        }

        function_symbol union_(union_name(), make_function_sort(s0, s1, target_sort));
        return union_;
      }

      /// \brief Recogniser for function +
      /// \param e A data expression
      /// \return true iff e is the function symbol matching +
      inline
      bool is_union_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == union_name();
        }
        return false;
      }

      /// \brief Application of function symbol +
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of + to a number of arguments
      inline
      application union_(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return sort_bag::union_(s, arg0.sort(), arg1.sort())(arg0, arg1);
      }

      /// \brief Recogniser for application of +
      /// \param e A data expression
      /// \return true iff e is an application of function symbol union_ to a
      ///     number of arguments
      inline
      bool is_union_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_union_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier *
      /// \return Identifier *
      inline
      core::identifier_string const& intersection_name()
      {
        static core::identifier_string intersection_name = core::identifier_string("*");
        return intersection_name;
      }

      ///\brief Constructor for function symbol *
      /// \param s A sort expression
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol intersection
      inline
      function_symbol intersection(const sort_expression& s, const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == bag(s) && s1 == bag(s))
        {
          target_sort = bag(s);
        }
        else if (s0 == sort_set::set_(s) && s1 == sort_set::set_(s))
        {
          target_sort = sort_set::set_(s);
        }
        else if (s0 == sort_fset::fset(s) && s1 == sort_fset::fset(s))
        {
          target_sort = sort_fset::fset(s);
        }
        else if (s0 == sort_fbag::fbag(s) && s1 == sort_fbag::fbag(s))
        {
          target_sort = sort_fbag::fbag(s);
        }
        else
        {
          throw mcrl2::runtime_error("cannot compute target sort for intersection with domain sorts " + to_string(s0) + ", " + to_string(s1));
        }

        function_symbol intersection(intersection_name(), make_function_sort(s0, s1, target_sort));
        return intersection;
      }

      /// \brief Recogniser for function *
      /// \param e A data expression
      /// \return true iff e is the function symbol matching *
      inline
      bool is_intersection_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == intersection_name();
        }
        return false;
      }

      /// \brief Application of function symbol *
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of * to a number of arguments
      inline
      application intersection(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return sort_bag::intersection(s, arg0.sort(), arg1.sort())(arg0, arg1);
      }

      /// \brief Recogniser for application of *
      /// \param e A data expression
      /// \return true iff e is an application of function symbol intersection to a
      ///     number of arguments
      inline
      bool is_intersection_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_intersection_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier -
      /// \return Identifier -
      inline
      core::identifier_string const& difference_name()
      {
        static core::identifier_string difference_name = core::identifier_string("-");
        return difference_name;
      }

      ///\brief Constructor for function symbol -
      /// \param s A sort expression
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol difference
      inline
      function_symbol difference(const sort_expression& s, const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == bag(s) && s1 == bag(s))
        {
          target_sort = bag(s);
        }
        else if (s0 == sort_set::set_(s) && s1 == sort_set::set_(s))
        {
          target_sort = sort_set::set_(s);
        }
        else if (s0 == sort_fset::fset(s) && s1 == sort_fset::fset(s))
        {
          target_sort = sort_fset::fset(s);
        }
        else if (s0 == sort_fbag::fbag(s) && s1 == sort_fbag::fbag(s))
        {
          target_sort = sort_fbag::fbag(s);
        }
        else
        {
          throw mcrl2::runtime_error("cannot compute target sort for difference with domain sorts " + to_string(s0) + ", " + to_string(s1));
        }

        function_symbol difference(difference_name(), make_function_sort(s0, s1, target_sort));
        return difference;
      }

      /// \brief Recogniser for function -
      /// \param e A data expression
      /// \return true iff e is the function symbol matching -
      inline
      bool is_difference_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == difference_name();
        }
        return false;
      }

      /// \brief Application of function symbol -
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of - to a number of arguments
      inline
      application difference(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return sort_bag::difference(s, arg0.sort(), arg1.sort())(arg0, arg1);
      }

      /// \brief Recogniser for application of -
      /// \param e A data expression
      /// \return true iff e is an application of function symbol difference to a
      ///     number of arguments
      inline
      bool is_difference_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_difference_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier Bag2Set
      /// \return Identifier Bag2Set
      inline
      core::identifier_string const& bag2set_name()
      {
        static core::identifier_string bag2set_name = core::identifier_string("Bag2Set");
        return bag2set_name;
      }

      /// \brief Constructor for function symbol Bag2Set
      /// \param s A sort expression
      /// \return Function symbol bag2set
      inline
      function_symbol bag2set(const sort_expression& s)
      {
        function_symbol bag2set(bag2set_name(), make_function_sort(bag(s), sort_set::set_(s)));
        return bag2set;
      }

      /// \brief Recogniser for function Bag2Set
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Bag2Set
      inline
      bool is_bag2set_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == bag2set_name();
        }
        return false;
      }

      /// \brief Application of function symbol Bag2Set
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of Bag2Set to a number of arguments
      inline
      application bag2set(const sort_expression& s, const data_expression& arg0)
      {
        return sort_bag::bag2set(s)(arg0);
      }

      /// \brief Recogniser for application of Bag2Set
      /// \param e A data expression
      /// \return true iff e is an application of function symbol bag2set to a
      ///     number of arguments
      inline
      bool is_bag2set_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_bag2set_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier Set2Bag
      /// \return Identifier Set2Bag
      inline
      core::identifier_string const& set2bag_name()
      {
        static core::identifier_string set2bag_name = core::identifier_string("Set2Bag");
        return set2bag_name;
      }

      /// \brief Constructor for function symbol Set2Bag
      /// \param s A sort expression
      /// \return Function symbol set2bag
      inline
      function_symbol set2bag(const sort_expression& s)
      {
        function_symbol set2bag(set2bag_name(), make_function_sort(sort_set::set_(s), bag(s)));
        return set2bag;
      }

      /// \brief Recogniser for function Set2Bag
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Set2Bag
      inline
      bool is_set2bag_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == set2bag_name();
        }
        return false;
      }

      /// \brief Application of function symbol Set2Bag
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of Set2Bag to a number of arguments
      inline
      application set2bag(const sort_expression& s, const data_expression& arg0)
      {
        return sort_bag::set2bag(s)(arg0);
      }

      /// \brief Recogniser for application of Set2Bag
      /// \param e A data expression
      /// \return true iff e is an application of function symbol set2bag to a
      ///     number of arguments
      inline
      bool is_set2bag_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_set2bag_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@zero_
      /// \return Identifier \@zero_
      inline
      core::identifier_string const& zero_function_name()
      {
        static core::identifier_string zero_function_name = core::identifier_string("@zero_");
        return zero_function_name;
      }

      /// \brief Constructor for function symbol \@zero_
      /// \param s A sort expression
      /// \return Function symbol zero_function
      inline
      function_symbol zero_function(const sort_expression& s)
      {
        function_symbol zero_function(zero_function_name(), make_function_sort(s, sort_nat::nat()));
        return zero_function;
      }

      /// \brief Recogniser for function \@zero_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@zero_
      inline
      bool is_zero_function_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == zero_function_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@zero_
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@zero_ to a number of arguments
      inline
      application zero_function(const sort_expression& s, const data_expression& arg0)
      {
        return sort_bag::zero_function(s)(arg0);
      }

      /// \brief Recogniser for application of \@zero_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol zero_function to a
      ///     number of arguments
      inline
      bool is_zero_function_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_zero_function_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@one_
      /// \return Identifier \@one_
      inline
      core::identifier_string const& one_function_name()
      {
        static core::identifier_string one_function_name = core::identifier_string("@one_");
        return one_function_name;
      }

      /// \brief Constructor for function symbol \@one_
      /// \param s A sort expression
      /// \return Function symbol one_function
      inline
      function_symbol one_function(const sort_expression& s)
      {
        function_symbol one_function(one_function_name(), make_function_sort(s, sort_nat::nat()));
        return one_function;
      }

      /// \brief Recogniser for function \@one_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@one_
      inline
      bool is_one_function_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == one_function_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@one_
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@one_ to a number of arguments
      inline
      application one_function(const sort_expression& s, const data_expression& arg0)
      {
        return sort_bag::one_function(s)(arg0);
      }

      /// \brief Recogniser for application of \@one_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol one_function to a
      ///     number of arguments
      inline
      bool is_one_function_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_one_function_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@add_
      /// \return Identifier \@add_
      inline
      core::identifier_string const& add_function_name()
      {
        static core::identifier_string add_function_name = core::identifier_string("@add_");
        return add_function_name;
      }

      /// \brief Constructor for function symbol \@add_
      /// \param s A sort expression
      /// \return Function symbol add_function
      inline
      function_symbol add_function(const sort_expression& s)
      {
        function_symbol add_function(add_function_name(), make_function_sort(make_function_sort(s, sort_nat::nat()), make_function_sort(s, sort_nat::nat()), make_function_sort(s, sort_nat::nat())));
        return add_function;
      }

      /// \brief Recogniser for function \@add_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@add_
      inline
      bool is_add_function_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == add_function_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@add_
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@add_ to a number of arguments
      inline
      application add_function(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return sort_bag::add_function(s)(arg0, arg1);
      }

      /// \brief Recogniser for application of \@add_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol add_function to a
      ///     number of arguments
      inline
      bool is_add_function_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_add_function_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@min_
      /// \return Identifier \@min_
      inline
      core::identifier_string const& min_function_name()
      {
        static core::identifier_string min_function_name = core::identifier_string("@min_");
        return min_function_name;
      }

      /// \brief Constructor for function symbol \@min_
      /// \param s A sort expression
      /// \return Function symbol min_function
      inline
      function_symbol min_function(const sort_expression& s)
      {
        function_symbol min_function(min_function_name(), make_function_sort(make_function_sort(s, sort_nat::nat()), make_function_sort(s, sort_nat::nat()), make_function_sort(s, sort_nat::nat())));
        return min_function;
      }

      /// \brief Recogniser for function \@min_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@min_
      inline
      bool is_min_function_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == min_function_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@min_
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@min_ to a number of arguments
      inline
      application min_function(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return sort_bag::min_function(s)(arg0, arg1);
      }

      /// \brief Recogniser for application of \@min_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol min_function to a
      ///     number of arguments
      inline
      bool is_min_function_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_min_function_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@monus_
      /// \return Identifier \@monus_
      inline
      core::identifier_string const& monus_function_name()
      {
        static core::identifier_string monus_function_name = core::identifier_string("@monus_");
        return monus_function_name;
      }

      /// \brief Constructor for function symbol \@monus_
      /// \param s A sort expression
      /// \return Function symbol monus_function
      inline
      function_symbol monus_function(const sort_expression& s)
      {
        function_symbol monus_function(monus_function_name(), make_function_sort(make_function_sort(s, sort_nat::nat()), make_function_sort(s, sort_nat::nat()), make_function_sort(s, sort_nat::nat())));
        return monus_function;
      }

      /// \brief Recogniser for function \@monus_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@monus_
      inline
      bool is_monus_function_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == monus_function_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@monus_
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@monus_ to a number of arguments
      inline
      application monus_function(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return sort_bag::monus_function(s)(arg0, arg1);
      }

      /// \brief Recogniser for application of \@monus_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol monus_function to a
      ///     number of arguments
      inline
      bool is_monus_function_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_monus_function_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@Nat2Bool_
      /// \return Identifier \@Nat2Bool_
      inline
      core::identifier_string const& nat2bool_function_name()
      {
        static core::identifier_string nat2bool_function_name = core::identifier_string("@Nat2Bool_");
        return nat2bool_function_name;
      }

      /// \brief Constructor for function symbol \@Nat2Bool_
      /// \param s A sort expression
      /// \return Function symbol nat2bool_function
      inline
      function_symbol nat2bool_function(const sort_expression& s)
      {
        function_symbol nat2bool_function(nat2bool_function_name(), make_function_sort(make_function_sort(s, sort_nat::nat()), make_function_sort(s, sort_bool::bool_())));
        return nat2bool_function;
      }

      /// \brief Recogniser for function \@Nat2Bool_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@Nat2Bool_
      inline
      bool is_nat2bool_function_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == nat2bool_function_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@Nat2Bool_
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@Nat2Bool_ to a number of arguments
      inline
      application nat2bool_function(const sort_expression& s, const data_expression& arg0)
      {
        return sort_bag::nat2bool_function(s)(arg0);
      }

      /// \brief Recogniser for application of \@Nat2Bool_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol nat2bool_function to a
      ///     number of arguments
      inline
      bool is_nat2bool_function_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_nat2bool_function_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@Bool2Nat_
      /// \return Identifier \@Bool2Nat_
      inline
      core::identifier_string const& bool2nat_function_name()
      {
        static core::identifier_string bool2nat_function_name = core::identifier_string("@Bool2Nat_");
        return bool2nat_function_name;
      }

      /// \brief Constructor for function symbol \@Bool2Nat_
      /// \param s A sort expression
      /// \return Function symbol bool2nat_function
      inline
      function_symbol bool2nat_function(const sort_expression& s)
      {
        function_symbol bool2nat_function(bool2nat_function_name(), make_function_sort(make_function_sort(s, sort_bool::bool_()), make_function_sort(s, sort_nat::nat())));
        return bool2nat_function;
      }

      /// \brief Recogniser for function \@Bool2Nat_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@Bool2Nat_
      inline
      bool is_bool2nat_function_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == bool2nat_function_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@Bool2Nat_
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@Bool2Nat_ to a number of arguments
      inline
      application bool2nat_function(const sort_expression& s, const data_expression& arg0)
      {
        return sort_bag::bool2nat_function(s)(arg0);
      }

      /// \brief Recogniser for application of \@Bool2Nat_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol bool2nat_function to a
      ///     number of arguments
      inline
      bool is_bool2nat_function_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_bool2nat_function_function_symbol(application(e).head());
        }
        return false;
      }
      /// \brief Give all system defined mappings for bag
      /// \param s A sort expression
      /// \return All system defined mappings for bag
      inline
      function_symbol_vector bag_generate_functions_code(const sort_expression& s)
      {
        function_symbol_vector result;
        result.push_back(sort_bag::bag_fbag(s));
        result.push_back(sort_bag::bag_comprehension(s));
        result.push_back(sort_bag::count(s, s, bag(s)));
        result.push_back(sort_bag::in(s, s, bag(s)));
        result.push_back(sort_bag::union_(s, bag(s), bag(s)));
        result.push_back(sort_bag::intersection(s, bag(s), bag(s)));
        result.push_back(sort_bag::difference(s, bag(s), bag(s)));
        result.push_back(sort_bag::bag2set(s));
        result.push_back(sort_bag::set2bag(s));
        result.push_back(sort_bag::zero_function(s));
        result.push_back(sort_bag::one_function(s));
        result.push_back(sort_bag::add_function(s));
        result.push_back(sort_bag::min_function(s));
        result.push_back(sort_bag::monus_function(s));
        result.push_back(sort_bag::nat2bool_function(s));
        result.push_back(sort_bag::bool2nat_function(s));
        return result;
      }
      ///\brief Function for projecting out argument
      ///        right from an application
      /// \param e A data expression
      /// \pre right is defined for e
      /// \return The argument of e that corresponds to right
      inline
      data_expression right(const data_expression& e)
      {
        assert(is_constructor_application(e) || is_count_application(e) || is_in_application(e) || is_union_application(e) || is_intersection_application(e) || is_difference_application(e) || is_add_function_application(e) || is_min_function_application(e) || is_monus_function_application(e));
        return atermpp::aterm_cast<const application >(e)[1];
      }

      ///\brief Function for projecting out argument
      ///        arg from an application
      /// \param e A data expression
      /// \pre arg is defined for e
      /// \return The argument of e that corresponds to arg
      inline
      data_expression arg(const data_expression& e)
      {
        assert(is_bag_fbag_application(e) || is_bag_comprehension_application(e) || is_bag2set_application(e) || is_set2bag_application(e) || is_zero_function_application(e) || is_one_function_application(e) || is_nat2bool_function_application(e) || is_bool2nat_function_application(e));
        return atermpp::aterm_cast<const application >(e)[0];
      }

      ///\brief Function for projecting out argument
      ///        left from an application
      /// \param e A data expression
      /// \pre left is defined for e
      /// \return The argument of e that corresponds to left
      inline
      data_expression left(const data_expression& e)
      {
        assert(is_constructor_application(e) || is_count_application(e) || is_in_application(e) || is_union_application(e) || is_intersection_application(e) || is_difference_application(e) || is_add_function_application(e) || is_min_function_application(e) || is_monus_function_application(e));
        return atermpp::aterm_cast<const application >(e)[0];
      }

      /// \brief Give all system defined equations for bag
      /// \param s A sort expression
      /// \return All system defined equations for sort bag
      inline
      data_equation_vector bag_generate_equations_code(const sort_expression& s)
      {
        variable vb("b",sort_fbag::fbag(s));
        variable vc("c",sort_fbag::fbag(s));
        variable ve("e",s);
        variable vf("f",make_function_sort(s, sort_nat::nat()));
        variable vg("g",make_function_sort(s, sort_nat::nat()));
        variable vh("h",make_function_sort(s, sort_bool::bool_()));
        variable vs("s",sort_fset::fset(s));
        variable vx("x",bag(s));
        variable vy("y",bag(s));
        variable vd("d",s);

        data_equation_vector result;
        result.push_back(data_equation(atermpp::make_vector(vb), bag_fbag(s, vb), constructor(s, zero_function(s), vb)));
        result.push_back(data_equation(atermpp::make_vector(vf), sort_bag::bag_comprehension(s, vf), constructor(s, vf, sort_fbag::empty(s))));
        result.push_back(data_equation(atermpp::make_vector(vb, ve, vf), count(s, ve, constructor(s, vf, vb)), sort_nat::swap_zero(vf(ve), count(s, ve, vb))));
        result.push_back(data_equation(atermpp::make_vector(ve, vx), in(s, ve, vx), greater(count(s, ve, vx), sort_nat::c0())));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vf, vg), equal_to(constructor(s, vf, vb), constructor(s, vg, vc)), if_(equal_to(vf, vg), equal_to(vb, vc), forall(atermpp::make_vector(vd), equal_to(count(s, vd, constructor(s, vf, vb)), count(s, vd, constructor(s, vg, vc)))))));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), less(vx, vy), sort_bool::and_(less_equal(vx, vy), not_equal_to(vx, vy))));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), less_equal(vx, vy), equal_to(intersection(s, vx, vy), vx)));
        result.push_back(data_equation(atermpp::make_vector(vx), union_(s, vx, vx), vx));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), union_(s, vx, union_(s, vx, vy)), union_(s, vx, vy)));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), union_(s, vx, union_(s, vy, vx)), union_(s, vy, vx)));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), union_(s, union_(s, vx, vy), vx), union_(s, vx, vy)));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), union_(s, union_(s, vy, vx), vx), union_(s, vy, vx)));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vf, vg), union_(s, constructor(s, vf, vb), constructor(s, vg, vc)), constructor(s, add_function(s, vf, vg), sort_fbag::join(s, vf, vg, vb, vc))));
        result.push_back(data_equation(atermpp::make_vector(vx), intersection(s, vx, vx), vx));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), intersection(s, vx, intersection(s, vx, vy)), intersection(s, vx, vy)));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), intersection(s, vx, intersection(s, vy, vx)), intersection(s, vy, vx)));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), intersection(s, intersection(s, vx, vy), vx), intersection(s, vx, vy)));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), intersection(s, intersection(s, vy, vx), vx), intersection(s, vy, vx)));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vf, vg), intersection(s, constructor(s, vf, vb), constructor(s, vg, vc)), constructor(s, min_function(s, vf, vg), sort_fbag::fbag_intersect(s, vf, vg, vb, vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vf, vg), difference(s, constructor(s, vf, vb), constructor(s, vg, vc)), constructor(s, monus_function(s, vf, vg), sort_fbag::fbag_difference(s, vf, vg, vb, vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vf), bag2set(s, constructor(s, vf, vb)), sort_set::constructor(s, nat2bool_function(s, vf), sort_fbag::fbag2fset(s, vf, vb))));
        result.push_back(data_equation(atermpp::make_vector(vh, vs), set2bag(s, sort_set::constructor(s, vh, vs)), constructor(s, bool2nat_function(s, vh), sort_fbag::fset2fbag(s, vs))));
        result.push_back(data_equation(atermpp::make_vector(ve), zero_function(s, ve), sort_nat::c0()));
        result.push_back(data_equation(atermpp::make_vector(ve), one_function(s, ve), sort_nat::cnat(sort_pos::c1())));
        result.push_back(data_equation(variable_list(), equal_to(zero_function(s), one_function(s)), sort_bool::false_()));
        result.push_back(data_equation(variable_list(), equal_to(one_function(s), zero_function(s)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(ve, vf, vg), add_function(s, vf, vg)(ve), sort_nat::plus(vf(ve), vg(ve))));
        result.push_back(data_equation(atermpp::make_vector(vf), add_function(s, vf, zero_function(s)), vf));
        result.push_back(data_equation(atermpp::make_vector(vf), add_function(s, zero_function(s), vf), vf));
        result.push_back(data_equation(atermpp::make_vector(ve, vf, vg), min_function(s, vf, vg)(ve), sort_nat::minimum(vf(ve), vg(ve))));
        result.push_back(data_equation(atermpp::make_vector(vf), min_function(s, vf, vf), vf));
        result.push_back(data_equation(atermpp::make_vector(vf), min_function(s, vf, zero_function(s)), zero_function(s)));
        result.push_back(data_equation(atermpp::make_vector(vf), min_function(s, zero_function(s), vf), zero_function(s)));
        result.push_back(data_equation(atermpp::make_vector(ve, vf, vg), monus_function(s, vf, vg)(ve), sort_nat::monus(vf(ve), vg(ve))));
        result.push_back(data_equation(atermpp::make_vector(vf), monus_function(s, vf, vf), zero_function(s)));
        result.push_back(data_equation(atermpp::make_vector(vf), monus_function(s, vf, zero_function(s)), vf));
        result.push_back(data_equation(atermpp::make_vector(vf), monus_function(s, zero_function(s), vf), zero_function(s)));
        result.push_back(data_equation(atermpp::make_vector(ve, vf), nat2bool_function(s, vf)(ve), greater(vf(ve), sort_nat::c0())));
        result.push_back(data_equation(variable_list(), nat2bool_function(s, zero_function(s)), sort_set::false_function(s)));
        result.push_back(data_equation(variable_list(), nat2bool_function(s, one_function(s)), sort_set::true_function(s)));
        result.push_back(data_equation(atermpp::make_vector(ve, vh), bool2nat_function(s, vh)(ve), if_(vh(ve), sort_nat::cnat(sort_pos::c1()), sort_nat::c0())));
        result.push_back(data_equation(variable_list(), bool2nat_function(s, sort_set::false_function(s)), zero_function(s)));
        result.push_back(data_equation(variable_list(), bool2nat_function(s, sort_set::true_function(s)), one_function(s)));
        return result;
      }

    } // namespace sort_bag

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_BAG_H
