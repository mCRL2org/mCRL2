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

#include "boost/utility.hpp"

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

      namespace detail {

        /// \brief Declaration for sort fbag as structured sort
        /// \param s A sort expression
        /// \return The structured sort representing fbag
        inline
        structured_sort fbag_struct(const sort_expression& s)
        {
          structured_sort_constructor_vector constructors;
          constructors.push_back(structured_sort_constructor("@fbag_empty", "empty"));
          constructors.push_back(structured_sort_constructor("@fbag_cons", atermpp::make_vector(structured_sort_constructor_argument("arg1", s), structured_sort_constructor_argument("arg2", sort_pos::pos()), structured_sort_constructor_argument("arg3", fbag(s))), "cons_"));
          return structured_sort(constructors);
        }

      } // namespace detail

      /// \brief Generate identifier \@fbag_empty
      /// \return Identifier \@fbag_empty
      inline
      core::identifier_string const& empty_name()
      {
        static core::identifier_string empty_name = core::identifier_string("@fbag_empty");
        return empty_name;
      }

      /// \brief Constructor for function symbol \@fbag_empty
      /// \param s A sort expression
      /// \return Function symbol empty
      inline
      function_symbol empty(const sort_expression& s)
      {
        function_symbol empty(empty_name(), fbag(s));
        return empty;
      }


      /// \brief Recogniser for function \@fbag_empty
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_empty
      inline
      bool is_empty_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == empty_name();
        }
        return false;
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
        return cons_(s)(arg0, arg1, arg2);
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

      /// \brief Give all system defined constructors for fbag
      /// \param s A sort expression
      /// \return All system defined constructors for fbag
      inline
      function_symbol_vector fbag_generate_constructors_code(const sort_expression& s)
      {
        function_symbol_vector result;
        function_symbol_vector fbag_constructors = detail::fbag_struct(s).constructor_functions(fbag(s));
        result.insert(result.end(), fbag_constructors.begin(), fbag_constructors.end());

        return result;
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
        return insert(s)(arg0, arg1, arg2);
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
        return cinsert(s)(arg0, arg1, arg2);
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

      /// \brief Generate identifier \@fbag_count
      /// \return Identifier \@fbag_count
      inline
      core::identifier_string const& count_name()
      {
        static core::identifier_string count_name = core::identifier_string("@fbag_count");
        return count_name;
      }

      /// \brief Constructor for function symbol \@fbag_count
      /// \param s A sort expression
      /// \return Function symbol count
      inline
      function_symbol count(const sort_expression& s)
      {
        function_symbol count(count_name(), make_function_sort(s, fbag(s), sort_nat::nat()));
        return count;
      }


      /// \brief Recogniser for function \@fbag_count
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_count
      inline
      bool is_count_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == count_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fbag_count
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@fbag_count to a number of arguments
      inline
      application count(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return count(s)(arg0, arg1);
      }

      /// \brief Recogniser for application of \@fbag_count
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

      /// \brief Generate identifier \@fbag_in
      /// \return Identifier \@fbag_in
      inline
      core::identifier_string const& in_name()
      {
        static core::identifier_string in_name = core::identifier_string("@fbag_in");
        return in_name;
      }

      /// \brief Constructor for function symbol \@fbag_in
      /// \param s A sort expression
      /// \return Function symbol in
      inline
      function_symbol in(const sort_expression& s)
      {
        function_symbol in(in_name(), make_function_sort(s, fbag(s), sort_bool::bool_()));
        return in;
      }


      /// \brief Recogniser for function \@fbag_in
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_in
      inline
      bool is_in_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == in_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@fbag_in
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@fbag_in to a number of arguments
      inline
      application in(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return in(s)(arg0, arg1);
      }

      /// \brief Recogniser for application of \@fbag_in
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
        return join(s)(arg0, arg1, arg2, arg3);
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
      core::identifier_string const& intersect_name()
      {
        static core::identifier_string intersect_name = core::identifier_string("@fbag_inter");
        return intersect_name;
      }

      /// \brief Constructor for function symbol \@fbag_inter
      /// \param s A sort expression
      /// \return Function symbol intersect
      inline
      function_symbol intersect(const sort_expression& s)
      {
        function_symbol intersect(intersect_name(), make_function_sort(make_function_sort(s, sort_nat::nat()), make_function_sort(s, sort_nat::nat()), fbag(s), fbag(s), fbag(s)));
        return intersect;
      }


      /// \brief Recogniser for function \@fbag_inter
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_inter
      inline
      bool is_intersect_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == intersect_name();
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
      application intersect(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return intersect(s)(arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@fbag_inter
      /// \param e A data expression
      /// \return true iff e is an application of function symbol intersect to a
      ///     number of arguments
      inline
      bool is_intersect_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_intersect_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@fbag_diff
      /// \return Identifier \@fbag_diff
      inline
      core::identifier_string const& difference_name()
      {
        static core::identifier_string difference_name = core::identifier_string("@fbag_diff");
        return difference_name;
      }

      /// \brief Constructor for function symbol \@fbag_diff
      /// \param s A sort expression
      /// \return Function symbol difference
      inline
      function_symbol difference(const sort_expression& s)
      {
        function_symbol difference(difference_name(), make_function_sort(make_function_sort(s, sort_nat::nat()), make_function_sort(s, sort_nat::nat()), fbag(s), fbag(s), fbag(s)));
        return difference;
      }


      /// \brief Recogniser for function \@fbag_diff
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_diff
      inline
      bool is_difference_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == difference_name();
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
      application difference(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return difference(s)(arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@fbag_diff
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
        return fbag2fset(s)(arg0, arg1);
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
        return fset2fbag(s)(arg0);
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

      /// \brief Give all system defined mappings for fbag
      /// \param s A sort expression
      /// \return All system defined mappings for fbag
      inline
      function_symbol_vector fbag_generate_functions_code(const sort_expression& s)
      {
        function_symbol_vector result;
        result.push_back(insert(s));
        result.push_back(cinsert(s));
        result.push_back(count(s));
        result.push_back(in(s));
        result.push_back(join(s));
        result.push_back(intersect(s));
        result.push_back(difference(s));
        result.push_back(fbag2fset(s));
        result.push_back(fset2fbag(s));
        function_symbol_vector fbag_mappings = detail::fbag_struct(s).comparison_functions(fbag(s));
        result.insert(result.end(), fbag_mappings.begin(), fbag_mappings.end());
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
        assert(is_count_application(e) || is_in_application(e) || is_fbag2fset_application(e));
        return *boost::next(atermpp::aterm_cast<const application >(e).begin(), 1);
      }

      ///\brief Function for projecting out argument
      ///        arg1 from an application
      /// \param e A data expression
      /// \pre arg1 is defined for e
      /// \return The argument of e that corresponds to arg1
      inline
      data_expression arg1(const data_expression& e)
      {
        assert(is_cons_application(e) || is_insert_application(e) || is_cinsert_application(e) || is_join_application(e) || is_intersect_application(e) || is_difference_application(e));
        return *boost::next(atermpp::aterm_cast<const application >(e).begin(), 0);
      }

      ///\brief Function for projecting out argument
      ///        arg2 from an application
      /// \param e A data expression
      /// \pre arg2 is defined for e
      /// \return The argument of e that corresponds to arg2
      inline
      data_expression arg2(const data_expression& e)
      {
        assert(is_cons_application(e) || is_insert_application(e) || is_cinsert_application(e) || is_join_application(e) || is_intersect_application(e) || is_difference_application(e));
        return *boost::next(atermpp::aterm_cast<const application >(e).begin(), 1);
      }

      ///\brief Function for projecting out argument
      ///        arg3 from an application
      /// \param e A data expression
      /// \pre arg3 is defined for e
      /// \return The argument of e that corresponds to arg3
      inline
      data_expression arg3(const data_expression& e)
      {
        assert(is_cons_application(e) || is_insert_application(e) || is_cinsert_application(e) || is_join_application(e) || is_intersect_application(e) || is_difference_application(e));
        return *boost::next(atermpp::aterm_cast<const application >(e).begin(), 2);
      }

      ///\brief Function for projecting out argument
      ///        arg4 from an application
      /// \param e A data expression
      /// \pre arg4 is defined for e
      /// \return The argument of e that corresponds to arg4
      inline
      data_expression arg4(const data_expression& e)
      {
        assert(is_join_application(e) || is_intersect_application(e) || is_difference_application(e));
        return *boost::next(atermpp::aterm_cast<const application >(e).begin(), 3);
      }

      ///\brief Function for projecting out argument
      ///        arg from an application
      /// \param e A data expression
      /// \pre arg is defined for e
      /// \return The argument of e that corresponds to arg
      inline
      data_expression arg(const data_expression& e)
      {
        assert(is_fset2fbag_application(e));
        return *boost::next(atermpp::aterm_cast<const application >(e).begin(), 0);
      }

      ///\brief Function for projecting out argument
      ///        left from an application
      /// \param e A data expression
      /// \pre left is defined for e
      /// \return The argument of e that corresponds to left
      inline
      data_expression left(const data_expression& e)
      {
        assert(is_count_application(e) || is_in_application(e) || is_fbag2fset_application(e));
        return *boost::next(atermpp::aterm_cast<const application >(e).begin(), 0);
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
        data_equation_vector fbag_equations = detail::fbag_struct(s).constructor_equations(fbag(s));
        result.insert(result.end(), fbag_equations.begin(), fbag_equations.end());
        fbag_equations = detail::fbag_struct(s).comparison_equations(fbag(s));
        result.insert(result.end(), fbag_equations.begin(), fbag_equations.end());
        result.push_back(data_equation(atermpp::make_vector(vd, vp), insert(s, vd, vp, empty(s)), cons_(s, vd, vp, empty(s))));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vp, vq), insert(s, vd, vp, cons_(s, vd, vq, vb)), cons_(s, vd, sort_nat::plus(vp, vq), vb)));
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
        result.push_back(data_equation(atermpp::make_vector(vf, vg), intersect(s, vf, vg, empty(s), empty(s)), empty(s)));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vf, vg, vp), intersect(s, vf, vg, cons_(s, vd, vp, vb), empty(s)), cinsert(s, vd, sort_nat::swap_zero_min(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), intersect(s, vf, vg, vb, empty(s)))));
        result.push_back(data_equation(atermpp::make_vector(vc, ve, vf, vg, vq), intersect(s, vf, vg, empty(s), cons_(s, ve, vq, vc)), cinsert(s, ve, sort_nat::swap_zero_min(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), intersect(s, vf, vg, empty(s), vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, vf, vg, vp, vq), intersect(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, vd, vq, vc)), cinsert(s, vd, sort_nat::swap_zero_min(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::cnat(vq)), intersect(s, vf, vg, vb, vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(vd, ve), intersect(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), cinsert(s, vd, sort_nat::swap_zero_min(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), intersect(s, vf, vg, vb, cons_(s, ve, vq, vc)))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(ve, vd), intersect(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), cinsert(s, ve, sort_nat::swap_zero_min(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), intersect(s, vf, vg, cons_(s, vd, vp, vb), vc))));
        result.push_back(data_equation(atermpp::make_vector(vf, vg), difference(s, vf, vg, empty(s), empty(s)), empty(s)));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vf, vg, vp), difference(s, vf, vg, cons_(s, vd, vp, vb), empty(s)), cinsert(s, vd, sort_nat::swap_zero_monus(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), difference(s, vf, vg, vb, empty(s)))));
        result.push_back(data_equation(atermpp::make_vector(vc, ve, vf, vg, vq), difference(s, vf, vg, empty(s), cons_(s, ve, vq, vc)), cinsert(s, ve, sort_nat::swap_zero_monus(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), difference(s, vf, vg, empty(s), vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, vf, vg, vp, vq), difference(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, vd, vq, vc)), cinsert(s, vd, sort_nat::swap_zero_monus(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::cnat(vq)), difference(s, vf, vg, vb, vc))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(vd, ve), difference(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), cinsert(s, vd, sort_nat::swap_zero_monus(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), difference(s, vf, vg, vb, cons_(s, ve, vq, vc)))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(ve, vd), difference(s, vf, vg, cons_(s, vd, vp, vb), cons_(s, ve, vq, vc)), cinsert(s, ve, sort_nat::swap_zero_monus(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), difference(s, vf, vg, cons_(s, vd, vp, vb), vc))));
        result.push_back(data_equation(atermpp::make_vector(vf), fbag2fset(s, vf, empty(s)), sort_fset::empty(s)));
        result.push_back(data_equation(atermpp::make_vector(vb, vd, vf, vp), fbag2fset(s, vf, cons_(s, vd, vp, vb)), sort_fset::cinsert(s, vd, equal_to(equal_to(vf(vd), sort_nat::cnat(vp)), greater(vf(vd), sort_nat::c0())), fbag2fset(s, vf, vb))));
        result.push_back(data_equation(variable_list(), fset2fbag(s, sort_fset::empty(s)), empty(s)));
        result.push_back(data_equation(atermpp::make_vector(vd, vs), fset2fbag(s, sort_fset::cons_(s, vd, vs)), cinsert(s, vd, sort_nat::cnat(sort_pos::c1()), fset2fbag(s, vs))));
        return result;
      }

    } // namespace sort_fbag

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FBAG_H
