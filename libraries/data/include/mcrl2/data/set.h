// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/set.h
/// \brief The standard sort set_.
///
/// This file was generated from the data sort specification
/// mcrl2/data/build/set.spec.

#ifndef MCRL2_DATA_SET_H
#define MCRL2_DATA_SET_H

#include "boost/utility.hpp"

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
#include "mcrl2/data/bool.h"
#include "mcrl2/data/fset.h"

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort set_
    namespace sort_set {

      /// \brief Constructor for sort expression Set(S)
      /// \param s A sort expression
      /// \return Sort expression set_(s)
      inline
      container_sort set_(const sort_expression& s)
      {
        container_sort set_(set_container(), s);
        return set_;
      }

      /// \brief Recogniser for sort expression Set(s)
      /// \param e A sort expression
      /// \return true iff e is a container sort of which the name matches
      ///      set_
      inline
      bool is_set(const sort_expression& e)
      {
        if (is_container_sort(e))
        {
          return container_sort(e).container_name() == set_container();
        }
        return false;
      }

      /// \brief Generate identifier \@set
      /// \return Identifier \@set
      inline
      core::identifier_string const& constructor_name()
      {
        static core::identifier_string constructor_name = core::identifier_string("@set");
        return constructor_name;
      }

      /// \brief Constructor for function symbol \@set
      /// \param s A sort expression
      /// \return Function symbol constructor
      inline
      function_symbol constructor(const sort_expression& s)
      {
        function_symbol constructor(constructor_name(), make_function_sort(make_function_sort(s, sort_bool::bool_()), sort_fset::fset(s), set_(s)));
        return constructor;
      }


      /// \brief Recogniser for function \@set
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@set
      inline
      bool is_constructor_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == constructor_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@set
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@set to a number of arguments
      inline
      application constructor(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return constructor(s)(arg0, arg1);
      }

      /// \brief Recogniser for application of \@set
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

      /// \brief Give all system defined constructors for set_
      /// \param s A sort expression
      /// \return All system defined constructors for set_
      inline
      function_symbol_vector set_generate_constructors_code(const sort_expression& s)
      {
        function_symbol_vector result;
        result.push_back(constructor(s));

        return result;
      }
      /// \brief Generate identifier {}
      /// \return Identifier {}
      inline
      core::identifier_string const& empty_name()
      {
        static core::identifier_string empty_name = core::identifier_string("{}");
        return empty_name;
      }

      /// \brief Constructor for function symbol {}
      /// \param s A sort expression
      /// \return Function symbol empty
      inline
      function_symbol empty(const sort_expression& s)
      {
        function_symbol empty(empty_name(), set_(s));
        return empty;
      }


      /// \brief Recogniser for function {}
      /// \param e A data expression
      /// \return true iff e is the function symbol matching {}
      inline
      bool is_empty_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == empty_name();
        }
        return false;
      }

      /// \brief Generate identifier \@setfset
      /// \return Identifier \@setfset
      inline
      core::identifier_string const& set_fset_name()
      {
        static core::identifier_string set_fset_name = core::identifier_string("@setfset");
        return set_fset_name;
      }

      /// \brief Constructor for function symbol \@setfset
      /// \param s A sort expression
      /// \return Function symbol set_fset
      inline
      function_symbol set_fset(const sort_expression& s)
      {
        function_symbol set_fset(set_fset_name(), make_function_sort(sort_fset::fset(s), set_(s)));
        return set_fset;
      }


      /// \brief Recogniser for function \@setfset
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@setfset
      inline
      bool is_set_fset_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == set_fset_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@setfset
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@setfset to a number of arguments
      inline
      application set_fset(const sort_expression& s, const data_expression& arg0)
      {
        return set_fset(s)(arg0);
      }

      /// \brief Recogniser for application of \@setfset
      /// \param e A data expression
      /// \return true iff e is an application of function symbol set_fset to a
      ///     number of arguments
      inline
      bool is_set_fset_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_set_fset_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@setcomp
      /// \return Identifier \@setcomp
      inline
      core::identifier_string const& set_comprehension_name()
      {
        static core::identifier_string set_comprehension_name = core::identifier_string("@setcomp");
        return set_comprehension_name;
      }

      /// \brief Constructor for function symbol \@setcomp
      /// \param s A sort expression
      /// \return Function symbol set_comprehension
      inline
      function_symbol set_comprehension(const sort_expression& s)
      {
        function_symbol set_comprehension(set_comprehension_name(), make_function_sort(make_function_sort(s, sort_bool::bool_()), set_(s)));
        return set_comprehension;
      }


      /// \brief Recogniser for function \@setcomp
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@setcomp
      inline
      bool is_set_comprehension_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == set_comprehension_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@setcomp
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@setcomp to a number of arguments
      inline
      application set_comprehension(const sort_expression& s, const data_expression& arg0)
      {
        return set_comprehension(s)(arg0);
      }

      /// \brief Recogniser for application of \@setcomp
      /// \param e A data expression
      /// \return true iff e is an application of function symbol set_comprehension to a
      ///     number of arguments
      inline
      bool is_set_comprehension_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_set_comprehension_function_symbol(application(e).head());
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

      /// \brief Constructor for function symbol in
      /// \param s A sort expression
      /// \return Function symbol in
      inline
      function_symbol in(const sort_expression& s)
      {
        function_symbol in(in_name(), make_function_sort(s, set_(s), sort_bool::bool_()));
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
          return function_symbol(e).name() == in_name();
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
        return in(s)(arg0, arg1);
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

      /// \brief Generate identifier !
      /// \return Identifier !
      inline
      core::identifier_string const& complement_name()
      {
        static core::identifier_string complement_name = core::identifier_string("!");
        return complement_name;
      }

      /// \brief Constructor for function symbol !
      /// \param s A sort expression
      /// \return Function symbol complement
      inline
      function_symbol complement(const sort_expression& s)
      {
        function_symbol complement(complement_name(), make_function_sort(set_(s), set_(s)));
        return complement;
      }


      /// \brief Recogniser for function !
      /// \param e A data expression
      /// \return true iff e is the function symbol matching !
      inline
      bool is_complement_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == complement_name();
        }
        return false;
      }

      /// \brief Application of function symbol !
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of ! to a number of arguments
      inline
      application complement(const sort_expression& s, const data_expression& arg0)
      {
        return complement(s)(arg0);
      }

      /// \brief Recogniser for application of !
      /// \param e A data expression
      /// \return true iff e is an application of function symbol complement to a
      ///     number of arguments
      inline
      bool is_complement_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_complement_function_symbol(application(e).head());
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

      /// \brief Constructor for function symbol +
      /// \param s A sort expression
      /// \return Function symbol union_
      inline
      function_symbol union_(const sort_expression& s)
      {
        function_symbol union_(union_name(), make_function_sort(set_(s), set_(s), set_(s)));
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
          return function_symbol(e).name() == union_name();
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
        return union_(s)(arg0, arg1);
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

      /// \brief Constructor for function symbol *
      /// \param s A sort expression
      /// \return Function symbol intersection
      inline
      function_symbol intersection(const sort_expression& s)
      {
        function_symbol intersection(intersection_name(), make_function_sort(set_(s), set_(s), set_(s)));
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
          return function_symbol(e).name() == intersection_name();
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
        return intersection(s)(arg0, arg1);
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

      /// \brief Constructor for function symbol -
      /// \param s A sort expression
      /// \return Function symbol difference
      inline
      function_symbol difference(const sort_expression& s)
      {
        function_symbol difference(difference_name(), make_function_sort(set_(s), set_(s), set_(s)));
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
          return function_symbol(e).name() == difference_name();
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
        return difference(s)(arg0, arg1);
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

      /// \brief Generate identifier \@false_
      /// \return Identifier \@false_
      inline
      core::identifier_string const& false_function_name()
      {
        static core::identifier_string false_function_name = core::identifier_string("@false_");
        return false_function_name;
      }

      /// \brief Constructor for function symbol \@false_
      /// \param s A sort expression
      /// \return Function symbol false_function
      inline
      function_symbol false_function(const sort_expression& s)
      {
        function_symbol false_function(false_function_name(), make_function_sort(s, sort_bool::bool_()));
        return false_function;
      }


      /// \brief Recogniser for function \@false_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@false_
      inline
      bool is_false_function_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == false_function_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@false_
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@false_ to a number of arguments
      inline
      application false_function(const sort_expression& s, const data_expression& arg0)
      {
        return false_function(s)(arg0);
      }

      /// \brief Recogniser for application of \@false_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol false_function to a
      ///     number of arguments
      inline
      bool is_false_function_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_false_function_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@true_
      /// \return Identifier \@true_
      inline
      core::identifier_string const& true_function_name()
      {
        static core::identifier_string true_function_name = core::identifier_string("@true_");
        return true_function_name;
      }

      /// \brief Constructor for function symbol \@true_
      /// \param s A sort expression
      /// \return Function symbol true_function
      inline
      function_symbol true_function(const sort_expression& s)
      {
        function_symbol true_function(true_function_name(), make_function_sort(s, sort_bool::bool_()));
        return true_function;
      }


      /// \brief Recogniser for function \@true_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@true_
      inline
      bool is_true_function_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == true_function_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@true_
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@true_ to a number of arguments
      inline
      application true_function(const sort_expression& s, const data_expression& arg0)
      {
        return true_function(s)(arg0);
      }

      /// \brief Recogniser for application of \@true_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol true_function to a
      ///     number of arguments
      inline
      bool is_true_function_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_true_function_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@not_
      /// \return Identifier \@not_
      inline
      core::identifier_string const& not_function_name()
      {
        static core::identifier_string not_function_name = core::identifier_string("@not_");
        return not_function_name;
      }

      /// \brief Constructor for function symbol \@not_
      /// \param s A sort expression
      /// \return Function symbol not_function
      inline
      function_symbol not_function(const sort_expression& s)
      {
        function_symbol not_function(not_function_name(), make_function_sort(make_function_sort(s, sort_bool::bool_()), make_function_sort(s, sort_bool::bool_())));
        return not_function;
      }


      /// \brief Recogniser for function \@not_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@not_
      inline
      bool is_not_function_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == not_function_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@not_
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@not_ to a number of arguments
      inline
      application not_function(const sort_expression& s, const data_expression& arg0)
      {
        return not_function(s)(arg0);
      }

      /// \brief Recogniser for application of \@not_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol not_function to a
      ///     number of arguments
      inline
      bool is_not_function_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_not_function_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@and_
      /// \return Identifier \@and_
      inline
      core::identifier_string const& and_function_name()
      {
        static core::identifier_string and_function_name = core::identifier_string("@and_");
        return and_function_name;
      }

      /// \brief Constructor for function symbol \@and_
      /// \param s A sort expression
      /// \return Function symbol and_function
      inline
      function_symbol and_function(const sort_expression& s)
      {
        function_symbol and_function(and_function_name(), make_function_sort(make_function_sort(s, sort_bool::bool_()), make_function_sort(s, sort_bool::bool_()), make_function_sort(s, sort_bool::bool_())));
        return and_function;
      }


      /// \brief Recogniser for function \@and_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@and_
      inline
      bool is_and_function_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == and_function_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@and_
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@and_ to a number of arguments
      inline
      application and_function(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return and_function(s)(arg0, arg1);
      }

      /// \brief Recogniser for application of \@and_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol and_function to a
      ///     number of arguments
      inline
      bool is_and_function_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_and_function_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@or_
      /// \return Identifier \@or_
      inline
      core::identifier_string const& or_function_name()
      {
        static core::identifier_string or_function_name = core::identifier_string("@or_");
        return or_function_name;
      }

      /// \brief Constructor for function symbol \@or_
      /// \param s A sort expression
      /// \return Function symbol or_function
      inline
      function_symbol or_function(const sort_expression& s)
      {
        function_symbol or_function(or_function_name(), make_function_sort(make_function_sort(s, sort_bool::bool_()), make_function_sort(s, sort_bool::bool_()), make_function_sort(s, sort_bool::bool_())));
        return or_function;
      }


      /// \brief Recogniser for function \@or_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@or_
      inline
      bool is_or_function_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == or_function_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@or_
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@or_ to a number of arguments
      inline
      application or_function(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return or_function(s)(arg0, arg1);
      }

      /// \brief Recogniser for application of \@or_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol or_function to a
      ///     number of arguments
      inline
      bool is_or_function_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_or_function_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Give all system defined mappings for set_
      /// \param s A sort expression
      /// \return All system defined mappings for set_
      inline
      function_symbol_vector set_generate_functions_code(const sort_expression& s)
      {
        function_symbol_vector result;
        result.push_back(empty(s));
        result.push_back(set_fset(s));
        result.push_back(set_comprehension(s));
        result.push_back(in(s));
        result.push_back(complement(s));
        result.push_back(union_(s));
        result.push_back(intersection(s));
        result.push_back(difference(s));
        result.push_back(false_function(s));
        result.push_back(true_function(s));
        result.push_back(not_function(s));
        result.push_back(and_function(s));
        result.push_back(or_function(s));
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
        assert(is_constructor_application(e) || is_in_application(e) || is_union_application(e) || is_intersection_application(e) || is_difference_application(e) || is_and_function_application(e) || is_or_function_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 1);
      }

      ///\brief Function for projecting out argument
      ///        arg from an application
      /// \param e A data expression
      /// \pre arg is defined for e
      /// \return The argument of e that corresponds to arg
      inline
      data_expression arg(const data_expression& e)
      {
        assert(is_set_fset_application(e) || is_set_comprehension_application(e) || is_complement_application(e) || is_false_function_application(e) || is_true_function_application(e) || is_not_function_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 0);
      }

      ///\brief Function for projecting out argument
      ///        left from an application
      /// \param e A data expression
      /// \pre left is defined for e
      /// \return The argument of e that corresponds to left
      inline
      data_expression left(const data_expression& e)
      {
        assert(is_constructor_application(e) || is_in_application(e) || is_union_application(e) || is_intersection_application(e) || is_difference_application(e) || is_and_function_application(e) || is_or_function_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 0);
      }

      /// \brief Give all system defined equations for set_
      /// \param s A sort expression
      /// \return All system defined equations for sort set_
      inline
      data_equation_vector set_generate_equations_code(const sort_expression& s)
      {
        variable ve("e",s);
        variable vs("s",sort_fset::fset(s));
        variable vt("t",sort_fset::fset(s));
        variable vf("f",make_function_sort(s, sort_bool::bool_()));
        variable vg("g",make_function_sort(s, sort_bool::bool_()));
        variable vx("x",set_(s));
        variable vy("y",set_(s));
        variable vc("c",s);

        data_equation_vector result;
        result.push_back(data_equation(variable_list(), empty(s), constructor(s, false_function(s), sort_fset::empty(s))));
        result.push_back(data_equation(atermpp::make_vector(vs), set_fset(s, vs), constructor(s, false_function(s), vs)));
        result.push_back(data_equation(atermpp::make_vector(vf), set_comprehension(s, vf), constructor(s, vf, sort_fset::empty(s))));
        result.push_back(data_equation(atermpp::make_vector(ve, vf, vs), in(s, ve, constructor(s, vf, vs)), not_equal_to(vf(ve), sort_fset::in(s, ve, vs))));
        result.push_back(data_equation(atermpp::make_vector(vf, vg, vs, vt), equal_to(constructor(s, vf, vs), constructor(s, vg, vt)), forall(atermpp::make_vector(vc), not_equal_to(equal_to(vf(vc), vg(vc)), sort_fset::in(s, vc, sort_fset::difference(s, vs, vt))))));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), less(vx, vy), sort_bool::and_(less_equal(vx, vy), not_equal_to(vx, vy))));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), less_equal(vx, vy), equal_to(intersection(s, vx, vy), vx)));
        result.push_back(data_equation(atermpp::make_vector(vf, vs), complement(s, constructor(s, vf, vs)), constructor(s, not_function(s, vf), vs)));
        result.push_back(data_equation(atermpp::make_vector(vf, vg, vs, vt), union_(s, constructor(s, vf, vs), constructor(s, vg, vt)), constructor(s, or_function(s, vf, vg), sort_fset::union_(s, vf, vg, vs, vt))));
        result.push_back(data_equation(atermpp::make_vector(vf, vg, vs, vt), intersection(s, constructor(s, vf, vs), constructor(s, vg, vt)), constructor(s, and_function(s, vf, vg), sort_fset::intersection(s, vf, vg, vs, vt))));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), difference(s, vx, vy), intersection(s, vx, complement(s, vy))));
        result.push_back(data_equation(atermpp::make_vector(ve), false_function(s, ve), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(ve), true_function(s, ve), sort_bool::true_()));
        result.push_back(data_equation(variable_list(), equal_to(false_function(s), true_function(s)), sort_bool::false_()));
        result.push_back(data_equation(variable_list(), equal_to(true_function(s), false_function(s)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(ve, vf), not_function(s, vf)(ve), sort_bool::not_(vf(ve))));
        result.push_back(data_equation(variable_list(), not_function(s, false_function(s)), true_function(s)));
        result.push_back(data_equation(variable_list(), not_function(s, true_function(s)), false_function(s)));
        result.push_back(data_equation(atermpp::make_vector(ve, vf, vg), and_function(s, vf, vg)(ve), sort_bool::and_(vf(ve), vg(ve))));
        result.push_back(data_equation(atermpp::make_vector(vf), and_function(s, vf, vf), vf));
        result.push_back(data_equation(atermpp::make_vector(vf), and_function(s, vf, false_function(s)), false_function(s)));
        result.push_back(data_equation(atermpp::make_vector(vf), and_function(s, false_function(s), vf), false_function(s)));
        result.push_back(data_equation(atermpp::make_vector(vf), and_function(s, vf, true_function(s)), vf));
        result.push_back(data_equation(atermpp::make_vector(vf), and_function(s, true_function(s), vf), vf));
        result.push_back(data_equation(atermpp::make_vector(ve, vf, vg), or_function(s, vf, vg)(ve), sort_bool::or_(vf(ve), vg(ve))));
        result.push_back(data_equation(atermpp::make_vector(vf), or_function(s, vf, vf), vf));
        result.push_back(data_equation(atermpp::make_vector(vf), or_function(s, vf, false_function(s)), vf));
        result.push_back(data_equation(atermpp::make_vector(vf), or_function(s, false_function(s), vf), vf));
        result.push_back(data_equation(atermpp::make_vector(vf), or_function(s, vf, true_function(s)), true_function(s)));
        result.push_back(data_equation(atermpp::make_vector(vf), or_function(s, true_function(s), vf), true_function(s)));
        return result;
      }

    } // namespace sort_set_

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SET_H
