// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/fbag.h
/// \brief The standard sort fbag.
///
/// This file was generated from the data sort specification
/// mcrl2/data/build/fbag.spec.

#ifndef MCRL2_DATA_FBAG_H
#define MCRL2_DATA_FBAG_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/container_sort.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/fset.h"

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort fbag
    namespace sort_fbag {

      /// \brief Constructor for sort expression FBag(S)
      /// \param s A sort expression
      /// \return Sort expression fbag(s)
      inline
      container_sort fbag(const sort_expression& s)
      {
        container_sort fbag(fbag_container(), s);
        return fbag;
      }

      /// \brief Recogniser for sort expression FBag(s)
      /// \param e A sort expression
      /// \return true iff e is a container sort of which the name matches
      ///      fbag
      inline
      bool is_fbag(const sort_expression& e)
      {
        if (is_container_sort(e))
        {
          return container_sort(e).container_name() == fbag_container();
        }
        return false;
      }


      /// \brief Generate identifier {:}
      /// \return Identifier {:}
      inline
      core::identifier_string const& empty_name()
      {
        static core::identifier_string empty_name = core::identifier_string("{:}");
        return empty_name;
      }

      /// \brief Constructor for function symbol {:}
      /// \param s A sort expression
      /// \return Function symbol empty
      inline
      function_symbol empty(const sort_expression& s)
      {
        function_symbol empty(empty_name(), fbag(s));
        return empty;
      }

      /// \brief Recogniser for function {:}
      /// \param e A data expression
      /// \return true iff e is the function symbol matching {:}
      inline
      bool is_empty_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == empty_name();
        }
        return false;
      }

      /// \brief Generate identifier \@fbag_insert
      /// \return Identifier \@fbag_insert
      inline
      core::identifier_string const& insert_name()
      {
        static core::identifier_string insert_name = core::identifier_string("@fbag_insert");
        return insert_name;
      }

      /// \brief Constructor for function symbol \@fbag_insert
      /// \param s A sort expression
      /// \return Function symbol insert
      inline
      function_symbol insert(const sort_expression& s)
      {
        function_symbol insert(insert_name(), make_function_sort(s, sort_pos::pos(), fbag(s), fbag(s)));
        return insert;
      }

      /// \brief Recogniser for function \@fbag_insert
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_insert
      inline
      bool is_insert_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == insert_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fbag_insert
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@fbag_insert to a number of arguments
      inline
      application insert(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return sort_fbag::insert(s)(arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@fbag_insert
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
      /// \brief Give all system defined constructors for fbag
      /// \param s A sort expression
      /// \return All system defined constructors for fbag
      inline
      function_symbol_vector fbag_generate_constructors_code(const sort_expression& s)
      {
        function_symbol_vector result;
        result.push_back(sort_fbag::empty(s));
        result.push_back(sort_fbag::insert(s));

        return result;
      }

      /// \brief Generate identifier \@fbag_cons
      /// \return Identifier \@fbag_cons
      inline
      core::identifier_string const& cons_name()
      {
        static core::identifier_string cons_name = core::identifier_string("@fbag_cons");
        return cons_name;
      }

      /// \brief Constructor for function symbol \@fbag_cons
      /// \param s A sort expression
      /// \return Function symbol cons_
      inline
      function_symbol cons_(const sort_expression& s)
      {
        function_symbol cons_(cons_name(), make_function_sort(s, sort_pos::pos(), fbag(s), fbag(s)));
        return cons_;
      }

      /// \brief Recogniser for function \@fbag_cons
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_cons
      inline
      bool is_cons_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == cons_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fbag_cons
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@fbag_cons to a number of arguments
      inline
      application cons_(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return sort_fbag::cons_(s)(arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@fbag_cons
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

      /// \brief Generate identifier \@fbag_cinsert
      /// \return Identifier \@fbag_cinsert
      inline
      core::identifier_string const& cinsert_name()
      {
        static core::identifier_string cinsert_name = core::identifier_string("@fbag_cinsert");
        return cinsert_name;
      }

      /// \brief Constructor for function symbol \@fbag_cinsert
      /// \param s A sort expression
      /// \return Function symbol cinsert
      inline
      function_symbol cinsert(const sort_expression& s)
      {
        function_symbol cinsert(cinsert_name(), make_function_sort(s, sort_nat::nat(), fbag(s), fbag(s)));
        return cinsert;
      }

      /// \brief Recogniser for function \@fbag_cinsert
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_cinsert
      inline
      bool is_cinsert_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == cinsert_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fbag_cinsert
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@fbag_cinsert to a number of arguments
      inline
      application cinsert(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return sort_fbag::cinsert(s)(arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@fbag_cinsert
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

      /// \brief Generate identifier count
      /// \return Identifier count
      inline
      core::identifier_string const& count_name()
      {
        static core::identifier_string count_name = core::identifier_string("count");
        return count_name;
      }

      /// \brief Constructor for function symbol count
      /// \param s A sort expression
      /// \return Function symbol count
      inline
      function_symbol count(const sort_expression& s)
      {
        function_symbol count(count_name(), make_function_sort(s, fbag(s), sort_nat::nat()));
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
          return function_symbol(e).name() == count_name();
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
        return sort_fbag::count(s)(arg0, arg1);
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

      /// \brief Constructor for function symbol in
      /// \param s A sort expression
      /// \return Function symbol in
      inline
      function_symbol in(const sort_expression& s)
      {
        function_symbol in(in_name(), make_function_sort(s, fbag(s), sort_bool::bool_()));
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
        return sort_fbag::in(s)(arg0, arg1);
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

      /// \brief Generate identifier \@fbag_join
      /// \return Identifier \@fbag_join
      inline
      core::identifier_string const& join_name()
      {
        static core::identifier_string join_name = core::identifier_string("@fbag_join");
        return join_name;
      }

      /// \brief Constructor for function symbol \@fbag_join
      /// \param s A sort expression
      /// \return Function symbol join
      inline
      function_symbol join(const sort_expression& s)
      {
        function_symbol join(join_name(), make_function_sort(make_function_sort(s, sort_nat::nat()), make_function_sort(s, sort_nat::nat()), fbag(s), fbag(s), fbag(s)));
        return join;
      }

      /// \brief Recogniser for function \@fbag_join
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_join
      inline
      bool is_join_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == join_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fbag_join
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of \@fbag_join to a number of arguments
      inline
      application join(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return sort_fbag::join(s)(arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@fbag_join
      /// \param e A data expression
      /// \return true iff e is an application of function symbol join to a
      ///     number of arguments
      inline
      bool is_join_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_join_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@fbag_inter
      /// \return Identifier \@fbag_inter
      inline
      core::identifier_string const& fbag_intersect_name()
      {
        static core::identifier_string fbag_intersect_name = core::identifier_string("@fbag_inter");
        return fbag_intersect_name;
      }

      /// \brief Constructor for function symbol \@fbag_inter
      /// \param s A sort expression
      /// \return Function symbol fbag_intersect
      inline
      function_symbol fbag_intersect(const sort_expression& s)
      {
        function_symbol fbag_intersect(fbag_intersect_name(), make_function_sort(make_function_sort(s, sort_nat::nat()), make_function_sort(s, sort_nat::nat()), fbag(s), fbag(s), fbag(s)));
        return fbag_intersect;
      }

      /// \brief Recogniser for function \@fbag_inter
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_inter
      inline
      bool is_fbag_intersect_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == fbag_intersect_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fbag_inter
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of \@fbag_inter to a number of arguments
      inline
      application fbag_intersect(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return sort_fbag::fbag_intersect(s)(arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@fbag_inter
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fbag_intersect to a
      ///     number of arguments
      inline
      bool is_fbag_intersect_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_fbag_intersect_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@fbag_diff
      /// \return Identifier \@fbag_diff
      inline
      core::identifier_string const& fbag_difference_name()
      {
        static core::identifier_string fbag_difference_name = core::identifier_string("@fbag_diff");
        return fbag_difference_name;
      }

      /// \brief Constructor for function symbol \@fbag_diff
      /// \param s A sort expression
      /// \return Function symbol fbag_difference
      inline
      function_symbol fbag_difference(const sort_expression& s)
      {
        function_symbol fbag_difference(fbag_difference_name(), make_function_sort(make_function_sort(s, sort_nat::nat()), make_function_sort(s, sort_nat::nat()), fbag(s), fbag(s), fbag(s)));
        return fbag_difference;
      }

      /// \brief Recogniser for function \@fbag_diff
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_diff
      inline
      bool is_fbag_difference_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == fbag_difference_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fbag_diff
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of \@fbag_diff to a number of arguments
      inline
      application fbag_difference(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return sort_fbag::fbag_difference(s)(arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@fbag_diff
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fbag_difference to a
      ///     number of arguments
      inline
      bool is_fbag_difference_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_fbag_difference_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@fbag2fset
      /// \return Identifier \@fbag2fset
      inline
      core::identifier_string const& fbag2fset_name()
      {
        static core::identifier_string fbag2fset_name = core::identifier_string("@fbag2fset");
        return fbag2fset_name;
      }

      /// \brief Constructor for function symbol \@fbag2fset
      /// \param s A sort expression
      /// \return Function symbol fbag2fset
      inline
      function_symbol fbag2fset(const sort_expression& s)
      {
        function_symbol fbag2fset(fbag2fset_name(), make_function_sort(make_function_sort(s, sort_nat::nat()), fbag(s), sort_fset::fset(s)));
        return fbag2fset;
      }

      /// \brief Recogniser for function \@fbag2fset
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag2fset
      inline
      bool is_fbag2fset_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == fbag2fset_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fbag2fset
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@fbag2fset to a number of arguments
      inline
      application fbag2fset(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return sort_fbag::fbag2fset(s)(arg0, arg1);
      }

      /// \brief Recogniser for application of \@fbag2fset
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fbag2fset to a
      ///     number of arguments
      inline
      bool is_fbag2fset_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_fbag2fset_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@fset2fbag
      /// \return Identifier \@fset2fbag
      inline
      core::identifier_string const& fset2fbag_name()
      {
        static core::identifier_string fset2fbag_name = core::identifier_string("@fset2fbag");
        return fset2fbag_name;
      }

      /// \brief Constructor for function symbol \@fset2fbag
      /// \param s A sort expression
      /// \return Function symbol fset2fbag
      inline
      function_symbol fset2fbag(const sort_expression& s)
      {
        function_symbol fset2fbag(fset2fbag_name(), make_function_sort(sort_fset::fset(s), fbag(s)));
        return fset2fbag;
      }

      /// \brief Recogniser for function \@fset2fbag
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset2fbag
      inline
      bool is_fset2fbag_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == fset2fbag_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fset2fbag
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@fset2fbag to a number of arguments
      inline
      application fset2fbag(const sort_expression& s, const data_expression& arg0)
      {
        return sort_fbag::fset2fbag(s)(arg0);
      }

      /// \brief Recogniser for application of \@fset2fbag
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fset2fbag to a
      ///     number of arguments
      inline
      bool is_fset2fbag_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_fset2fbag_function_symbol(application(e).head());
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
        function_symbol union_(union_name(), make_function_sort(fbag(s), fbag(s), fbag(s)));
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
        return sort_fbag::union_(s)(arg0, arg1);
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
        function_symbol intersection(intersection_name(), make_function_sort(fbag(s), fbag(s), fbag(s)));
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
        return sort_fbag::intersection(s)(arg0, arg1);
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
        function_symbol difference(difference_name(), make_function_sort(fbag(s), fbag(s), fbag(s)));
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
        return sort_fbag::difference(s)(arg0, arg1);
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

      /// \brief Generate identifier #
      /// \return Identifier #
      inline
      core::identifier_string const& count_all_name()
      {
        static core::identifier_string count_all_name = core::identifier_string("#");
        return count_all_name;
      }

      /// \brief Constructor for function symbol #
      /// \param s A sort expression
      /// \return Function symbol count_all
      inline
      function_symbol count_all(const sort_expression& s)
      {
        function_symbol count_all(count_all_name(), make_function_sort(fbag(s), sort_nat::nat()));
        return count_all;
      }

      /// \brief Recogniser for function #
      /// \param e A data expression
      /// \return true iff e is the function symbol matching #
      inline
      bool is_count_all_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == count_all_name();
        }
        return false;
      }

      /// \brief Application of function symbol #
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of # to a number of arguments
      inline
      application count_all(const sort_expression& s, const data_expression& arg0)
      {
        return sort_fbag::count_all(s)(arg0);
      }

      /// \brief Recogniser for application of #
      /// \param e A data expression
      /// \return true iff e is an application of function symbol count_all to a
      ///     number of arguments
      inline
      bool is_count_all_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_count_all_function_symbol(application(e).head());
        }
        return false;
      }
      /// \brief Give all system defined mappings for fbag
      /// \param s A sort expression
      /// \return All system defined mappings for fbag
      inline
      function_symbol_vector fbag_generate_functions_code(const sort_expression& s)
      {
        function_symbol_vector result;
        result.push_back(sort_fbag::cons_(s));
        result.push_back(sort_fbag::cinsert(s));
        result.push_back(sort_fbag::count(s));
        result.push_back(sort_fbag::in(s));
        result.push_back(sort_fbag::join(s));
        result.push_back(sort_fbag::fbag_intersect(s));
        result.push_back(sort_fbag::fbag_difference(s));
        result.push_back(sort_fbag::fbag2fset(s));
        result.push_back(sort_fbag::fset2fbag(s));
        result.push_back(sort_fbag::union_(s));
        result.push_back(sort_fbag::intersection(s));
        result.push_back(sort_fbag::difference(s));
        result.push_back(sort_fbag::count_all(s));
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
        assert(is_count_application(e) || is_in_application(e) || is_fbag2fset_application(e) || is_union_application(e) || is_intersection_application(e) || is_difference_application(e));
        return atermpp::down_cast<const application >(e)[1];
      }

      ///\brief Function for projecting out argument
      ///        arg1 from an application
      /// \param e A data expression
      /// \pre arg1 is defined for e
      /// \return The argument of e that corresponds to arg1
      inline
      data_expression arg1(const data_expression& e)
      {
        assert(is_insert_application(e) || is_cons_application(e) || is_cinsert_application(e) || is_join_application(e) || is_fbag_intersect_application(e) || is_fbag_difference_application(e));
        return atermpp::down_cast<const application >(e)[0];
      }

      ///\brief Function for projecting out argument
      ///        arg2 from an application
      /// \param e A data expression
      /// \pre arg2 is defined for e
      /// \return The argument of e that corresponds to arg2
      inline
      data_expression arg2(const data_expression& e)
      {
        assert(is_insert_application(e) || is_cons_application(e) || is_cinsert_application(e) || is_join_application(e) || is_fbag_intersect_application(e) || is_fbag_difference_application(e));
        return atermpp::down_cast<const application >(e)[1];
      }

      ///\brief Function for projecting out argument
      ///        arg3 from an application
      /// \param e A data expression
      /// \pre arg3 is defined for e
      /// \return The argument of e that corresponds to arg3
      inline
      data_expression arg3(const data_expression& e)
      {
        assert(is_insert_application(e) || is_cons_application(e) || is_cinsert_application(e) || is_join_application(e) || is_fbag_intersect_application(e) || is_fbag_difference_application(e));
        return atermpp::down_cast<const application >(e)[2];
      }

      ///\brief Function for projecting out argument
      ///        arg4 from an application
      /// \param e A data expression
      /// \pre arg4 is defined for e
      /// \return The argument of e that corresponds to arg4
      inline
      data_expression arg4(const data_expression& e)
      {
        assert(is_join_application(e) || is_fbag_intersect_application(e) || is_fbag_difference_application(e));
        return atermpp::down_cast<const application >(e)[3];
      }

      ///\brief Function for projecting out argument
      ///        arg from an application
      /// \param e A data expression
      /// \pre arg is defined for e
      /// \return The argument of e that corresponds to arg
      inline
      data_expression arg(const data_expression& e)
      {
        assert(is_fset2fbag_application(e) || is_count_all_application(e));
        return atermpp::down_cast<const application >(e)[0];
      }

      ///\brief Function for projecting out argument
      ///        left from an application
      /// \param e A data expression
      /// \pre left is defined for e
      /// \return The argument of e that corresponds to left
      inline
      data_expression left(const data_expression& e)
      {
        assert(is_count_application(e) || is_in_application(e) || is_fbag2fset_application(e) || is_union_application(e) || is_intersection_application(e) || is_difference_application(e));
        return atermpp::down_cast<const application >(e)[0];
      }

      /// \brief Give all system defined equations for fbag
      /// \param s A sort expression
      /// \return All system defined equations for sort fbag
      inline
      data_equation_vector fbag_generate_equations_code(const sort_expression& s)
      {
        variable vd("d",s);
        variable ve("e",s);
        variable vp("p",sort_pos::pos());
        variable vq("q",sort_pos::pos());
        variable vb("b",fbag(s));
        variable vc("c",fbag(s));
        variable vs("s",sort_fset::fset(s));
        variable vf("f",make_function_sort(s, sort_nat::nat()));
        variable vg("g",make_function_sort(s, sort_nat::nat()));

        data_equation_vector result;
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vp), equal_to(cons_(s, vd, vp, vb), empty(s)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vp), equal_to(empty(s), cons_(s, vd, vp, vb)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vp, vq), equal_to(cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), sort_bool::and_(equal_to(vp, vq), sort_bool::and_(equal_to(vd, ve), equal_to(vb, vc)))));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vp), less_equal(cons_(s, vd, vp, vb), empty(s)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vp), less_equal(empty(s), cons_(s, vd, vp, vb)), sort_bool::true_()));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vp, vq), less_equal(cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), if_(less(vd, ve), sort_bool::false_(), if_(equal_to(vd, ve), sort_bool::and_(less_equal(vp, vq), less_equal(vb, vc)), less_equal(cons_(s, vd, vp, vb), vc)))));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vp), less(cons_(s, vd, vp, vb), empty(s)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vp), less(empty(s), cons_(s, vd, vp, vb)), sort_bool::true_()));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vp, vq), less(cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), if_(less(vd, ve), sort_bool::false_(), if_(equal_to(vd, ve), sort_bool::or_(sort_bool::and_(equal_to(vp, vq), less(vb, vc)), sort_bool::and_(less(vp, vq), less_equal(vb, vc))), less_equal(cons_(s, vd, vp, vb), vc)))));
        result.push_back(data_equation(atermpp::make_vector(vd, vp), insert(s, vd, vp, empty(s)), cons_(s, vd, vp, empty(s))));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vp, vq), insert(s, vd, vp, cons_(s, vd, vq, vb)), cons_(s, vd, sort_pos::add_with_carry(sort_bool::false_(), vp, vq), vb)));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, ve, vp, vq), less(vd, ve), insert(s, vd, vp, cons_(s, ve, vq, vb)), cons_(s, vd, vp, cons_(s, ve, vq, vb))));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, ve, vp, vq), less(ve, vd), insert(s, vd, vp, cons_(s, ve, vq, vb)), cons_(s, ve, vq, insert(s, vd, vp, vb))));
        result.push_back(data_equation(atermpp::make_vector(vb, vd), cinsert(s, vd, sort_nat::c0(), vb), vb));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vp), cinsert(s, vd, sort_nat::cnat(vp), vb), insert(s, vd, vp, vb)));
        result.push_back(data_equation(atermpp::make_vector(vd), count(s, vd, empty(s)), sort_nat::c0()));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vp), count(s, vd, cons_(s, vd, vp, vb)), sort_nat::cnat(vp)));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, ve, vp), less(vd, ve), count(s, vd, cons_(s, ve, vp, vb)), sort_nat::c0()));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, ve, vp), less(ve, vd), count(s, vd, cons_(s, ve, vp, vb)), count(s, vd, vb)));
        result.push_back(data_equation(atermpp::make_vector(vb, vd), in(s, vd, vb), greater(count(s, vd, vb), sort_nat::c0())));
        result.push_back(data_equation(atermpp::make_vector(vf, vg), join(s, vf, vg, empty(s), empty(s)), empty(s)));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vf, vg, vp), join(s, vf, vg, cons_(s, vd, vp, vb), empty(s)), cinsert(s, vd, sort_nat::swap_zero_add(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), join(s, vf, vg, vb, empty(s)))));
        result.push_back(data_equation(atermpp::make_vector(vc, ve, vf, vg, vq), join(s, vf, vg, empty(s), cons_(s, ve, vq, vc)), cinsert(s, ve, sort_nat::swap_zero_add(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), join(s, vf, vg, empty(s), vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, vf, vg, vp, vq), join(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, vd, vq, vc)), cinsert(s, vd, sort_nat::swap_zero_add(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::cnat(vq)), join(s, vf, vg, vb, vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(vd, ve), join(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), cinsert(s, vd, sort_nat::swap_zero_add(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), join(s, vf, vg, vb, cons_(s, ve, vq, vc)))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(ve, vd), join(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), cinsert(s, ve, sort_nat::swap_zero_add(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), join(s, vf, vg, cons_(s, vd, vp, vb), vc))));
        result.push_back(data_equation(atermpp::make_vector(vf, vg), fbag_intersect(s, vf, vg, empty(s), empty(s)), empty(s)));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vf, vg, vp), fbag_intersect(s, vf, vg, cons_(s, vd, vp, vb), empty(s)), cinsert(s, vd, sort_nat::swap_zero_min(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), fbag_intersect(s, vf, vg, vb, empty(s)))));
        result.push_back(data_equation(atermpp::make_vector(vc, ve, vf, vg, vq), fbag_intersect(s, vf, vg, empty(s), cons_(s, ve, vq, vc)), cinsert(s, ve, sort_nat::swap_zero_min(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), fbag_intersect(s, vf, vg, empty(s), vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, vf, vg, vp, vq), fbag_intersect(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, vd, vq, vc)), cinsert(s, vd, sort_nat::swap_zero_min(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::cnat(vq)), fbag_intersect(s, vf, vg, vb, vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(vd, ve), fbag_intersect(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), cinsert(s, vd, sort_nat::swap_zero_min(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), fbag_intersect(s, vf, vg, vb, cons_(s, ve, vq, vc)))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(ve, vd), fbag_intersect(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), cinsert(s, ve, sort_nat::swap_zero_min(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), fbag_intersect(s, vf, vg, cons_(s, vd, vp, vb), vc))));
        result.push_back(data_equation(atermpp::make_vector(vf, vg), fbag_difference(s, vf, vg, empty(s), empty(s)), empty(s)));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vf, vg, vp), fbag_difference(s, vf, vg, cons_(s, vd, vp, vb), empty(s)), cinsert(s, vd, sort_nat::swap_zero_monus(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), fbag_difference(s, vf, vg, vb, empty(s)))));
        result.push_back(data_equation(atermpp::make_vector(vc, ve, vf, vg, vq), fbag_difference(s, vf, vg, empty(s), cons_(s, ve, vq, vc)), cinsert(s, ve, sort_nat::swap_zero_monus(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), fbag_difference(s, vf, vg, empty(s), vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, vf, vg, vp, vq), fbag_difference(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, vd, vq, vc)), cinsert(s, vd, sort_nat::swap_zero_monus(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::cnat(vq)), fbag_difference(s, vf, vg, vb, vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(vd, ve), fbag_difference(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), cinsert(s, vd, sort_nat::swap_zero_monus(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), fbag_difference(s, vf, vg, vb, cons_(s, ve, vq, vc)))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(ve, vd), fbag_difference(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), cinsert(s, ve, sort_nat::swap_zero_monus(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), fbag_difference(s, vf, vg, cons_(s, vd, vp, vb), vc))));
        result.push_back(data_equation(atermpp::make_vector(vf), fbag2fset(s, vf, empty(s)), sort_fset::empty(s)));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vf, vp), fbag2fset(s, vf, cons_(s, vd, vp, vb)), sort_fset::cinsert(s, vd, equal_to(equal_to(vf(vd), sort_nat::cnat(vp)), greater(vf(vd), sort_nat::c0())), fbag2fset(s, vf, vb))));
        result.push_back(data_equation(variable_list(), fset2fbag(s, sort_fset::empty(s)), empty(s)));
        result.push_back(data_equation(atermpp::make_vector(vd, vs), fset2fbag(s, sort_fset::cons_(s, vd, vs)), cinsert(s, vd, sort_nat::cnat(sort_pos::c1()), fset2fbag(s, vs))));
        result.push_back(data_equation(atermpp::make_vector(vb), difference(s, vb, empty(s)), vb));
        result.push_back(data_equation(atermpp::make_vector(vc), difference(s, empty(s), vc), empty(s)));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, vp), difference(s, cons_(s, vd, vp, vb), cons_(s, vd, vp, vc)), difference(s, vb, vc)));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, vp, vq), less(vp, vq), difference(s, cons_(s, vd, vp, vb), cons_(s, vd, vq, vc)), difference(s, vb, vc)));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, vp, vq), less(vq, vp), difference(s, cons_(s, vd, vp, vb), cons_(s, vd, vq, vc)), cons_(s, vd, sort_nat::nat2pos(sort_nat::gte_subtract_with_borrow(sort_bool::false_(), vp, vq)), difference(s, vb, vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vp, vq), less(vd, ve), difference(s, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), cons_(s, vd, vp, difference(s, vb, cons_(s, ve, vq, vc)))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vp, vq), less(ve, vd), difference(s, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), cons_(s, ve, vq, difference(s, cons_(s, vd, vp, vb), vc))));
        result.push_back(data_equation(atermpp::make_vector(vb), union_(s, vb, empty(s)), vb));
        result.push_back(data_equation(atermpp::make_vector(vc), union_(s, empty(s), vc), vc));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, vp, vq), union_(s, cons_(s, vd, vp, vb), cons_(s, vd, vq, vc)), cons_(s, vd, union_(s, vp, vq), union_(s, vb, vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vp, vq), less(vd, ve), union_(s, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), cons_(s, vd, vp, union_(s, vb, cons_(s, ve, vq, vc)))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vp, vq), less(ve, vd), union_(s, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), cons_(s, ve, vq, union_(s, cons_(s, vd, vp, vb), vc))));
        result.push_back(data_equation(atermpp::make_vector(vb), intersection(s, vb, empty(s)), empty(s)));
        result.push_back(data_equation(atermpp::make_vector(vc), intersection(s, empty(s), vc), empty(s)));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, vp, vq), intersection(s, cons_(s, vd, vp, vb), cons_(s, vd, vq, vc)), cons_(s, vd, sort_nat::minimum(vp, vq), intersection(s, vb, vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vp, vq), less(vd, ve), intersection(s, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), intersection(s, vb, cons_(s, ve, vq, vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vp, vq), less(ve, vd), intersection(s, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), intersection(s, cons_(s, vd, vp, vb), vc)));
        result.push_back(data_equation(variable_list(), count_all(s, empty(s)), sort_nat::c0()));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vp), count_all(s, cons_(s, vd, vp, vb)), union_(s, vp, count_all(s, vb))));
        return result;
      }

    } // namespace sort_fbag

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FBAG_H
