// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/fset.h
/// \brief The standard sort fset.
///
/// This file was generated from the data sort specification
/// mcrl2/data/build/fset.spec.

#ifndef MCRL2_DATA_FSET_H
#define MCRL2_DATA_FSET_H

#include "boost/utility.hpp"

#include "mcrl2/utilities/workarounds.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/container_sort.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/bool.h"

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort fset
    namespace sort_fset {

      /// \brief Constructor for sort expression FSet(S)
      /// \param s A sort expression
      /// \return Sort expression fset(s)
      inline
      container_sort fset(const sort_expression& s)
      {
        container_sort fset(fset_container(), s);
        return fset;
      }

      /// \brief Recogniser for sort expression FSet(s)
      /// \param e A sort expression
      /// \return true iff e is a container sort of which the name matches
      ///      fset
      inline
      bool is_fset(const sort_expression& e)
      {
        if (is_container_sort(e))
        {
          return container_sort(e).container_name() == fset_container();
        }
        return false;
      }

      namespace detail {

        /// \brief Declaration for sort fset as structured sort
        /// \param s A sort expression
        /// \ret The structured sort representing fset
        inline
        structured_sort fset_struct(const sort_expression& s)
        {
          structured_sort_constructor_vector constructors;
          constructors.push_back(structured_sort_constructor("@fset_empty", "empty"));
          constructors.push_back(structured_sort_constructor("@fset_cons", atermpp::make_vector(structured_sort_constructor_argument("left", s), structured_sort_constructor_argument("right", fset(s))), "cons_"));
          return structured_sort(constructors);
        }

      } // namespace detail

      /// \brief Generate identifier \@fset_empty
      /// \return Identifier \@fset_empty
      inline
      core::identifier_string const& empty_name()
      {
        static core::identifier_string empty_name = core::detail::initialise_static_expression(empty_name, core::identifier_string("@fset_empty"));
        return empty_name;
      }

      /// \brief Constructor for function symbol \@fset_empty
      /// \param s A sort expression
      /// \return Function symbol empty
      inline
      function_symbol empty(const sort_expression& s)
      {
        function_symbol empty(empty_name(), fset(s));
        return empty;
      }


      /// \brief Recogniser for function \@fset_empty
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_empty
      inline
      bool is_empty_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == empty_name();
        }
        return false;
      }

      /// \brief Generate identifier \@fset_cons
      /// \return Identifier \@fset_cons
      inline
      core::identifier_string const& cons_name()
      {
        static core::identifier_string cons_name = core::detail::initialise_static_expression(cons_name, core::identifier_string("@fset_cons"));
        return cons_name;
      }

      /// \brief Constructor for function symbol \@fset_cons
      /// \param s A sort expression
      /// \return Function symbol cons_
      inline
      function_symbol cons_(const sort_expression& s)
      {
        function_symbol cons_(cons_name(), make_function_sort(s, fset(s), fset(s)));
        return cons_;
      }


      /// \brief Recogniser for function \@fset_cons
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_cons
      inline
      bool is_cons_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == cons_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fset_cons
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@fset_cons to a number of arguments
      inline
      application cons_(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return cons_(s)(arg0, arg1);
      }

      /// \brief Recogniser for application of \@fset_cons
      /// \param e A data expression
      /// \return true iff e is an application of function symbol cons_ to a
      ///     number of arguments
      inline
      bool is_cons_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_cons_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Give all system defined constructors for fset
      /// \param s A sort expression
      /// \return All system defined constructors for fset
      inline
      function_symbol_vector fset_generate_constructors_code(const sort_expression& s)
      {
        function_symbol_vector result;
        function_symbol_vector fset_constructors = detail::fset_struct(s).constructor_functions(fset(s));
        result.insert(result.end(), fset_constructors.begin(), fset_constructors.end());

        return result;
      }
      /// \brief Generate identifier \@fset_insert
      /// \return Identifier \@fset_insert
      inline
      core::identifier_string const& insert_name()
      {
        static core::identifier_string insert_name = core::detail::initialise_static_expression(insert_name, core::identifier_string("@fset_insert"));
        return insert_name;
      }

      /// \brief Constructor for function symbol \@fset_insert
      /// \param s A sort expression
      /// \return Function symbol insert
      inline
      function_symbol insert(const sort_expression& s)
      {
        function_symbol insert(insert_name(), make_function_sort(s, fset(s), fset(s)));
        return insert;
      }


      /// \brief Recogniser for function \@fset_insert
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_insert
      inline
      bool is_insert_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == insert_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fset_insert
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@fset_insert to a number of arguments
      inline
      application insert(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return insert(s)(arg0, arg1);
      }

      /// \brief Recogniser for application of \@fset_insert
      /// \param e A data expression
      /// \return true iff e is an application of function symbol insert to a
      ///     number of arguments
      inline
      bool is_insert_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_insert_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@fset_cinsert
      /// \return Identifier \@fset_cinsert
      inline
      core::identifier_string const& cinsert_name()
      {
        static core::identifier_string cinsert_name = core::detail::initialise_static_expression(cinsert_name, core::identifier_string("@fset_cinsert"));
        return cinsert_name;
      }

      /// \brief Constructor for function symbol \@fset_cinsert
      /// \param s A sort expression
      /// \return Function symbol cinsert
      inline
      function_symbol cinsert(const sort_expression& s)
      {
        function_symbol cinsert(cinsert_name(), make_function_sort(s, sort_bool::bool_(), fset(s), fset(s)));
        return cinsert;
      }


      /// \brief Recogniser for function \@fset_cinsert
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_cinsert
      inline
      bool is_cinsert_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == cinsert_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fset_cinsert
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@fset_cinsert to a number of arguments
      inline
      application cinsert(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return cinsert(s)(arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@fset_cinsert
      /// \param e A data expression
      /// \return true iff e is an application of function symbol cinsert to a
      ///     number of arguments
      inline
      bool is_cinsert_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_cinsert_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@fset_in
      /// \return Identifier \@fset_in
      inline
      core::identifier_string const& in_name()
      {
        static core::identifier_string in_name = core::detail::initialise_static_expression(in_name, core::identifier_string("@fset_in"));
        return in_name;
      }

      /// \brief Constructor for function symbol \@fset_in
      /// \param s A sort expression
      /// \return Function symbol in
      inline
      function_symbol in(const sort_expression& s)
      {
        function_symbol in(in_name(), make_function_sort(s, fset(s), sort_bool::bool_()));
        return in;
      }


      /// \brief Recogniser for function \@fset_in
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_in
      inline
      bool is_in_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == in_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fset_in
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@fset_in to a number of arguments
      inline
      application in(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return in(s)(arg0, arg1);
      }

      /// \brief Recogniser for application of \@fset_in
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

      /// \brief Generate identifier \@fset_union
      /// \return Identifier \@fset_union
      inline
      core::identifier_string const& union_name()
      {
        static core::identifier_string union_name = core::detail::initialise_static_expression(union_name, core::identifier_string("@fset_union"));
        return union_name;
      }

      /// \brief Constructor for function symbol \@fset_union
      /// \param s A sort expression
      /// \return Function symbol union_
      inline
      function_symbol union_(const sort_expression& s)
      {
        function_symbol union_(union_name(), make_function_sort(make_function_sort(s, sort_bool::bool_()), make_function_sort(s, sort_bool::bool_()), fset(s), fset(s), fset(s)));
        return union_;
      }


      /// \brief Recogniser for function \@fset_union
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_union
      inline
      bool is_union_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == union_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fset_union
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of \@fset_union to a number of arguments
      inline
      application union_(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return union_(s)(arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@fset_union
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

      /// \brief Generate identifier \@fset_inter
      /// \return Identifier \@fset_inter
      inline
      core::identifier_string const& intersection_name()
      {
        static core::identifier_string intersection_name = core::detail::initialise_static_expression(intersection_name, core::identifier_string("@fset_inter"));
        return intersection_name;
      }

      /// \brief Constructor for function symbol \@fset_inter
      /// \param s A sort expression
      /// \return Function symbol intersection
      inline
      function_symbol intersection(const sort_expression& s)
      {
        function_symbol intersection(intersection_name(), make_function_sort(make_function_sort(s, sort_bool::bool_()), make_function_sort(s, sort_bool::bool_()), fset(s), fset(s), fset(s)));
        return intersection;
      }


      /// \brief Recogniser for function \@fset_inter
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_inter
      inline
      bool is_intersection_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == intersection_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fset_inter
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of \@fset_inter to a number of arguments
      inline
      application intersection(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return intersection(s)(arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@fset_inter
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

      /// \brief Generate identifier \@fset_diff
      /// \return Identifier \@fset_diff
      inline
      core::identifier_string const& difference_name()
      {
        static core::identifier_string difference_name = core::detail::initialise_static_expression(difference_name, core::identifier_string("@fset_diff"));
        return difference_name;
      }

      /// \brief Constructor for function symbol \@fset_diff
      /// \param s A sort expression
      /// \return Function symbol difference
      inline
      function_symbol difference(const sort_expression& s)
      {
        function_symbol difference(difference_name(), make_function_sort(fset(s), fset(s), fset(s)));
        return difference;
      }


      /// \brief Recogniser for function \@fset_diff
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_diff
      inline
      bool is_difference_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == difference_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fset_diff
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@fset_diff to a number of arguments
      inline
      application difference(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return difference(s)(arg0, arg1);
      }

      /// \brief Recogniser for application of \@fset_diff
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

      /// \brief Give all system defined mappings for fset
      /// \param s A sort expression
      /// \return All system defined mappings for fset
      inline
      function_symbol_vector fset_generate_functions_code(const sort_expression& s)
      {
        function_symbol_vector result;
        result.push_back(insert(s));
        result.push_back(cinsert(s));
        result.push_back(in(s));
        result.push_back(union_(s));
        result.push_back(intersection(s));
        result.push_back(difference(s));
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
        assert(is_cons_application(e) || is_insert_application(e) || is_in_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 1);
      }

      ///\brief Function for projecting out argument
      ///        arg1 from an application
      /// \param e A data expression
      /// \pre arg1 is defined for e
      /// \return The argument of e that corresponds to arg1
      inline
      data_expression arg1(const data_expression& e)
      {
        assert(is_cinsert_application(e) || is_union_application(e) || is_intersection_application(e) || is_difference_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 0);
      }

      ///\brief Function for projecting out argument
      ///        arg2 from an application
      /// \param e A data expression
      /// \pre arg2 is defined for e
      /// \return The argument of e that corresponds to arg2
      inline
      data_expression arg2(const data_expression& e)
      {
        assert(is_cinsert_application(e) || is_union_application(e) || is_intersection_application(e) || is_difference_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 1);
      }

      ///\brief Function for projecting out argument
      ///        arg3 from an application
      /// \param e A data expression
      /// \pre arg3 is defined for e
      /// \return The argument of e that corresponds to arg3
      inline
      data_expression arg3(const data_expression& e)
      {
        assert(is_cinsert_application(e) || is_union_application(e) || is_intersection_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 2);
      }

      ///\brief Function for projecting out argument
      ///        arg4 from an application
      /// \param e A data expression
      /// \pre arg4 is defined for e
      /// \return The argument of e that corresponds to arg4
      inline
      data_expression arg4(const data_expression& e)
      {
        assert(is_union_application(e) || is_intersection_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 3);
      }

      ///\brief Function for projecting out argument
      ///        left from an application
      /// \param e A data expression
      /// \pre left is defined for e
      /// \return The argument of e that corresponds to left
      inline
      data_expression left(const data_expression& e)
      {
        assert(is_cons_application(e) || is_insert_application(e) || is_in_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 0);
      }

      /// \brief Give all system defined equations for fset
      /// \param s A sort expression
      /// \return All system defined equations for sort fset
      inline
      data_equation_vector fset_generate_equations_code(const sort_expression& s)
      {
        variable vd("d",s);
        variable ve("e",s);
        variable vf("f",make_function_sort(s, sort_bool::bool_()));
        variable vg("g",make_function_sort(s, sort_bool::bool_()));
        variable vs("s",fset(s));
        variable vt("t",fset(s));

        data_equation_vector result;
        data_equation_vector fset_equations = detail::fset_struct(s).constructor_equations(fset(s));
        result.insert(result.end(), fset_equations.begin(), fset_equations.end());
        result.push_back(data_equation(atermpp::make_vector(vd), insert(s, vd, empty(s)), cons_(s, vd, empty(s))));
        result.push_back(data_equation(atermpp::make_vector(vd, vs), insert(s, vd, cons_(s, vd, vs)), cons_(s, vd, vs)));
        result.push_back(data_equation(atermpp::make_vector(vd, ve, vs), less(vd, ve), insert(s, vd, cons_(s, ve, vs)), cons_(s, vd, cons_(s, ve, vs))));
        result.push_back(data_equation(atermpp::make_vector(vd, ve, vs), less(ve, vd), insert(s, vd, cons_(s, ve, vs)), cons_(s, ve, insert(s, vd, vs))));
        result.push_back(data_equation(atermpp::make_vector(vd, vs), cinsert(s, vd, sort_bool::false_(), vs), vs));
        result.push_back(data_equation(atermpp::make_vector(vd, vs), cinsert(s, vd, sort_bool::true_(), vs), insert(s, vd, vs)));
        result.push_back(data_equation(atermpp::make_vector(vd), in(s, vd, empty(s)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vd, ve, vs), in(s, vd, cons_(s, ve, vs)), sort_bool::or_(equal_to(vd, ve), in(s, vd, vs))));
        result.push_back(data_equation(atermpp::make_vector(vf, vg), union_(s, vf, vg, empty(s), empty(s)), empty(s)));
        result.push_back(data_equation(atermpp::make_vector(vd, vf, vg, vs), union_(s, vf, vg, cons_(s, vd, vs), empty(s)), cinsert(s, vd, sort_bool::not_(vg(vd)), union_(s, vf, vg, vs, empty(s)))));
        result.push_back(data_equation(atermpp::make_vector(ve, vf, vg, vt), union_(s, vf, vg, empty(s), cons_(s, ve, vt)), cinsert(s, ve, sort_bool::not_(vf(ve)), union_(s, vf, vg, empty(s), vt))));
        result.push_back(data_equation(atermpp::make_vector(vd, vf, vg, vs, vt), union_(s, vf, vg, cons_(s, vd, vs), cons_(s, vd, vt)), cinsert(s, vd, equal_to(vf(vd), vg(vd)), union_(s, vf, vg, vs, vt))));
        result.push_back(data_equation(atermpp::make_vector(vd, ve, vf, vg, vs, vt), less(vd, ve), union_(s, vf, vg, cons_(s, vd, vs), cons_(s, ve, vt)), cinsert(s, vd, sort_bool::not_(vg(vd)), union_(s, vf, vg, vs, cons_(s, ve, vt)))));
        result.push_back(data_equation(atermpp::make_vector(vd, ve, vf, vg, vs, vt), less(ve, vd), union_(s, vf, vg, cons_(s, vd, vs), cons_(s, ve, vt)), cinsert(s, ve, sort_bool::not_(vf(ve)), union_(s, vf, vg, cons_(s, vd, vs), vt))));
        result.push_back(data_equation(atermpp::make_vector(vf, vg), intersection(s, vf, vg, empty(s), empty(s)), empty(s)));
        result.push_back(data_equation(atermpp::make_vector(vd, vf, vg, vs), intersection(s, vf, vg, cons_(s, vd, vs), empty(s)), cinsert(s, vd, vg(vd), intersection(s, vf, vg, vs, empty(s)))));
        result.push_back(data_equation(atermpp::make_vector(ve, vf, vg, vt), intersection(s, vf, vg, empty(s), cons_(s, ve, vt)), cinsert(s, ve, vf(ve), intersection(s, vf, vg, empty(s), vt))));
        result.push_back(data_equation(atermpp::make_vector(vd, vf, vg, vs, vt), intersection(s, vf, vg, cons_(s, vd, vs), cons_(s, vd, vt)), cinsert(s, vd, equal_to(vf(vd), vg(vd)), intersection(s, vf, vg, vs, vt))));
        result.push_back(data_equation(atermpp::make_vector(vd, ve, vf, vg, vs, vt), less(vd, ve), intersection(s, vf, vg, cons_(s, vd, vs), cons_(s, ve, vt)), cinsert(s, vd, vg(vd), intersection(s, vf, vg, vs, cons_(s, ve, vt)))));
        result.push_back(data_equation(atermpp::make_vector(vd, ve, vf, vg, vs, vt), less(ve, vd), intersection(s, vf, vg, cons_(s, vd, vs), cons_(s, ve, vt)), cinsert(s, ve, vf(ve), intersection(s, vf, vg, cons_(s, vd, vs), vt))));
        result.push_back(data_equation(atermpp::make_vector(vs), difference(s, vs, empty(s)), vs));
        result.push_back(data_equation(atermpp::make_vector(vt), difference(s, empty(s), vt), vt));
        result.push_back(data_equation(atermpp::make_vector(vd, vs, vt), difference(s, cons_(s, vd, vs), cons_(s, vd, vt)), difference(s, vs, vt)));
        result.push_back(data_equation(atermpp::make_vector(vd, ve, vs, vt), less(vd, ve), difference(s, cons_(s, vd, vs), cons_(s, ve, vt)), cons_(s, vd, difference(s, vs, cons_(s, ve, vt)))));
        result.push_back(data_equation(atermpp::make_vector(vd, ve, vs, vt), less(ve, vd), difference(s, cons_(s, vd, vs), cons_(s, ve, vt)), cons_(s, ve, difference(s, cons_(s, vd, vs), vt))));
        result.push_back(data_equation(variable_list(), equal_to(empty(s), empty(s)), sort_bool::true_()));
        result.push_back(data_equation(atermpp::make_vector(ve, vs), equal_to(empty(s), cons_(s, ve, vs)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(ve, vs), equal_to(cons_(s, ve, vs), empty(s)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vd, ve, vs, vt), equal_to(cons_(s, ve, vt), cons_(s, vd, vs)), sort_bool::and_(equal_to(ve, vd), equal_to(vt, vs))));
        result.push_back(data_equation(variable_list(), less(empty(s), empty(s)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(ve, vs), less(empty(s), cons_(s, ve, vs)), sort_bool::true_()));
        result.push_back(data_equation(atermpp::make_vector(ve, vs), less(cons_(s, ve, vs), empty(s)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vd, ve, vs, vt), less(cons_(s, ve, vt), cons_(s, vd, vs)), sort_bool::or_(less(ve, vd), sort_bool::and_(equal_to(ve, vd), less(vt, vs)))));
        result.push_back(data_equation(variable_list(), less_equal(empty(s), empty(s)), sort_bool::true_()));
        result.push_back(data_equation(atermpp::make_vector(ve, vs), less_equal(empty(s), cons_(s, ve, vs)), sort_bool::true_()));
        result.push_back(data_equation(atermpp::make_vector(ve, vs), less_equal(cons_(s, ve, vs), empty(s)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vd, ve, vs, vt), less_equal(cons_(s, ve, vt), cons_(s, vd, vs)), sort_bool::or_(less(ve, vd), sort_bool::and_(equal_to(ve, vd), less_equal(vt, vs)))));
        return result;
      }

    } // namespace sort_fset

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FSET_H
