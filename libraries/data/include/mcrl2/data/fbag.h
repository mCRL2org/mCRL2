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

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/detail/container_utility.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/data_specification.h"
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
        container_sort fbag("fbag", s);
        return fbag;
      }

      /// \brief Recogniser for sort expression FBag(s)
      /// \param e A sort expression
      /// \return true iff e is a container sort of which the name matches
      ///      fbag
      inline
      bool is_fbag(const sort_expression& e)
      {
        if (e.is_container_sort())
        {
          return static_cast< container_sort >(e).container_name() == "fbag";
        }
        return false;
      }

      namespace detail {

        /// \brief Declaration for sort fbag as structured sort
        /// \param s A sort expression
        /// \ret The structured sort representing fbag
        inline
        structured_sort fbag_struct(const sort_expression& s)
        {
          structured_sort_constructor_vector constructors;
          constructors.push_back(structured_sort_constructor("@fbag_empty", "fbag_empty"));
          constructors.push_back(structured_sort_constructor("@fbag_cons", make_vector(structured_sort_constructor_argument(s, "head"), structured_sort_constructor_argument(sort_pos::pos(), "headcount"), structured_sort_constructor_argument(fbag(s), "tail")), "fbag_cons"));
          return structured_sort(constructors);
        }

      } // namespace detail

      /// \brief Constructor for function symbol \@fbag_empty
      /// \param s A sort expression
      /// \return Function symbol fbag_empty
      inline
      function_symbol const& fbag_empty(const sort_expression& s)
      {
        static function_symbol fbag_empty = data::detail::initialise_static_expression(fbag_empty, function_symbol("@fbag_empty", fbag(s)));
        return fbag_empty;
      }

      /// \brief Recogniser for function \@fbag_empty
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_empty
      inline
      bool is_fbag_empty_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@fbag_empty";
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fbag_cons
      /// \param s A sort expression
      /// \return Function symbol fbag_cons
      inline
      function_symbol const& fbag_cons(const sort_expression& s)
      {
        static function_symbol fbag_cons = data::detail::initialise_static_expression(fbag_cons, function_symbol("@fbag_cons", function_sort(s, sort_pos::pos(), fbag(s), fbag(s))));
        return fbag_cons;
      }

      /// \brief Recogniser for function \@fbag_cons
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_cons
      inline
      bool is_fbag_cons_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@fbag_cons";
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
      application fbag_cons(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(fbag_cons(s),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@fbag_cons
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fbag_cons to a
      ///     number of arguments
      inline
      bool is_fbag_cons_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fbag_cons_function_symbol(static_cast< application >(e).head());
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
      /// \brief Constructor for function symbol \@fbag_insert
      /// \param s A sort expression
      /// \return Function symbol fbaginsert
      inline
      function_symbol const& fbaginsert(const sort_expression& s)
      {
        static function_symbol fbaginsert = data::detail::initialise_static_expression(fbaginsert, function_symbol("@fbag_insert", function_sort(s, sort_pos::pos(), fbag(s), fbag(s))));
        return fbaginsert;
      }

      /// \brief Recogniser for function \@fbag_insert
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_insert
      inline
      bool is_fbaginsert_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@fbag_insert";
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
      application fbaginsert(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(fbaginsert(s),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@fbag_insert
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fbaginsert to a
      ///     number of arguments
      inline
      bool is_fbaginsert_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fbaginsert_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fbag_cinsert
      /// \param s A sort expression
      /// \return Function symbol fbagcinsert
      inline
      function_symbol const& fbagcinsert(const sort_expression& s)
      {
        static function_symbol fbagcinsert = data::detail::initialise_static_expression(fbagcinsert, function_symbol("@fbag_cinsert", function_sort(s, sort_nat::nat(), fbag(s), fbag(s))));
        return fbagcinsert;
      }

      /// \brief Recogniser for function \@fbag_cinsert
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_cinsert
      inline
      bool is_fbagcinsert_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@fbag_cinsert";
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
      application fbagcinsert(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(fbagcinsert(s),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@fbag_cinsert
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fbagcinsert to a
      ///     number of arguments
      inline
      bool is_fbagcinsert_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fbagcinsert_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fbag_count
      /// \param s A sort expression
      /// \return Function symbol fbagcount
      inline
      function_symbol const& fbagcount(const sort_expression& s)
      {
        static function_symbol fbagcount = data::detail::initialise_static_expression(fbagcount, function_symbol("@fbag_count", function_sort(s, fbag(s), sort_nat::nat())));
        return fbagcount;
      }

      /// \brief Recogniser for function \@fbag_count
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_count
      inline
      bool is_fbagcount_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@fbag_count";
        }
        return false;
      }

      /// \brief Application of function symbol \@fbag_count
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@fbag_count to a number of arguments
      inline
      application fbagcount(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(fbagcount(s),arg0, arg1);
      }

      /// \brief Recogniser for application of \@fbag_count
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fbagcount to a
      ///     number of arguments
      inline
      bool is_fbagcount_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fbagcount_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fbag_in
      /// \param s A sort expression
      /// \return Function symbol fbagin
      inline
      function_symbol const& fbagin(const sort_expression& s)
      {
        static function_symbol fbagin = data::detail::initialise_static_expression(fbagin, function_symbol("@fbag_in", function_sort(s, fbag(s), sort_bool_::bool_())));
        return fbagin;
      }

      /// \brief Recogniser for function \@fbag_in
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_in
      inline
      bool is_fbagin_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@fbag_in";
        }
        return false;
      }

      /// \brief Application of function symbol \@fbag_in
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@fbag_in to a number of arguments
      inline
      application fbagin(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(fbagin(s),arg0, arg1);
      }

      /// \brief Recogniser for application of \@fbag_in
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fbagin to a
      ///     number of arguments
      inline
      bool is_fbagin_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fbagin_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fbag_lte
      /// \param s A sort expression
      /// \return Function symbol fbaglte
      inline
      function_symbol const& fbaglte(const sort_expression& s)
      {
        static function_symbol fbaglte = data::detail::initialise_static_expression(fbaglte, function_symbol("@fbag_lte", function_sort(function_sort(s, sort_nat::nat()), fbag(s), fbag(s), sort_bool_::bool_())));
        return fbaglte;
      }

      /// \brief Recogniser for function \@fbag_lte
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_lte
      inline
      bool is_fbaglte_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@fbag_lte";
        }
        return false;
      }

      /// \brief Application of function symbol \@fbag_lte
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@fbag_lte to a number of arguments
      inline
      application fbaglte(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(fbaglte(s),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@fbag_lte
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fbaglte to a
      ///     number of arguments
      inline
      bool is_fbaglte_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fbaglte_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fbag_join
      /// \param s A sort expression
      /// \return Function symbol fbagjoin
      inline
      function_symbol const& fbagjoin(const sort_expression& s)
      {
        static function_symbol fbagjoin = data::detail::initialise_static_expression(fbagjoin, function_symbol("@fbag_join", function_sort(function_sort(s, sort_nat::nat()), function_sort(s, sort_nat::nat()), fbag(s), fbag(s), fbag(s))));
        return fbagjoin;
      }

      /// \brief Recogniser for function \@fbag_join
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_join
      inline
      bool is_fbagjoin_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@fbag_join";
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
      application fbagjoin(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(fbagjoin(s),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@fbag_join
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fbagjoin to a
      ///     number of arguments
      inline
      bool is_fbagjoin_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fbagjoin_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fbag_inter
      /// \param s A sort expression
      /// \return Function symbol fbagintersect
      inline
      function_symbol const& fbagintersect(const sort_expression& s)
      {
        static function_symbol fbagintersect = data::detail::initialise_static_expression(fbagintersect, function_symbol("@fbag_inter", function_sort(function_sort(s, sort_nat::nat()), function_sort(s, sort_nat::nat()), fbag(s), fbag(s), fbag(s))));
        return fbagintersect;
      }

      /// \brief Recogniser for function \@fbag_inter
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_inter
      inline
      bool is_fbagintersect_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@fbag_inter";
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
      application fbagintersect(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(fbagintersect(s),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@fbag_inter
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fbagintersect to a
      ///     number of arguments
      inline
      bool is_fbagintersect_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fbagintersect_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fbag_diff
      /// \param s A sort expression
      /// \return Function symbol fbagdifference
      inline
      function_symbol const& fbagdifference(const sort_expression& s)
      {
        static function_symbol fbagdifference = data::detail::initialise_static_expression(fbagdifference, function_symbol("@fbag_diff", function_sort(function_sort(s, sort_nat::nat()), function_sort(s, sort_nat::nat()), fbag(s), fbag(s), fbag(s))));
        return fbagdifference;
      }

      /// \brief Recogniser for function \@fbag_diff
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag_diff
      inline
      bool is_fbagdifference_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@fbag_diff";
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
      application fbagdifference(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(fbagdifference(s),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@fbag_diff
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fbagdifference to a
      ///     number of arguments
      inline
      bool is_fbagdifference_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fbagdifference_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fbag2fset
      /// \param s A sort expression
      /// \return Function symbol fbag2fset
      inline
      function_symbol const& fbag2fset(const sort_expression& s)
      {
        static function_symbol fbag2fset = data::detail::initialise_static_expression(fbag2fset, function_symbol("@fbag2fset", function_sort(function_sort(s, sort_nat::nat()), fbag(s), sort_fset::fset(s))));
        return fbag2fset;
      }

      /// \brief Recogniser for function \@fbag2fset
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fbag2fset
      inline
      bool is_fbag2fset_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@fbag2fset";
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
        return application(fbag2fset(s),arg0, arg1);
      }

      /// \brief Recogniser for application of \@fbag2fset
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fbag2fset to a
      ///     number of arguments
      inline
      bool is_fbag2fset_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fbag2fset_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fset2fbag
      /// \param s A sort expression
      /// \return Function symbol fset2fbag
      inline
      function_symbol const& fset2fbag(const sort_expression& s)
      {
        static function_symbol fset2fbag = data::detail::initialise_static_expression(fset2fbag, function_symbol("@fset2fbag", function_sort(sort_fset::fset(s), fbag(s))));
        return fset2fbag;
      }

      /// \brief Recogniser for function \@fset2fbag
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset2fbag
      inline
      bool is_fset2fbag_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@fset2fbag";
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
        return application(fset2fbag(s),arg0);
      }

      /// \brief Recogniser for application of \@fset2fbag
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fset2fbag to a
      ///     number of arguments
      inline
      bool is_fset2fbag_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fset2fbag_function_symbol(static_cast< application >(e).head());
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
        result.push_back(fbaginsert(s));
        result.push_back(fbagcinsert(s));
        result.push_back(fbagcount(s));
        result.push_back(fbagin(s));
        result.push_back(fbaglte(s));
        result.push_back(fbagjoin(s));
        result.push_back(fbagintersect(s));
        result.push_back(fbagdifference(s));
        result.push_back(fbag2fset(s));
        result.push_back(fset2fbag(s));
        return result;
      }
      ///\brief Function for projecting out argument
      ///        head from an application
      /// \param e A data expression
      /// \pre head is defined for e
      /// \return The argument of e that corresponds to head
      inline
      data_expression head(const data_expression& e)
      {
        if (is_fbag_cons_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        right from an application
      /// \param e A data expression
      /// \pre right is defined for e
      /// \return The argument of e that corresponds to right
      inline
      data_expression right(const data_expression& e)
      {
        if (is_fbagcount_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_fbagin_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_fbag2fset_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        arg1 from an application
      /// \param e A data expression
      /// \pre arg1 is defined for e
      /// \return The argument of e that corresponds to arg1
      inline
      data_expression arg1(const data_expression& e)
      {
        if (is_fbaginsert_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_fbagcinsert_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_fbaglte_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_fbagjoin_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_fbagintersect_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_fbagdifference_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        arg2 from an application
      /// \param e A data expression
      /// \pre arg2 is defined for e
      /// \return The argument of e that corresponds to arg2
      inline
      data_expression arg2(const data_expression& e)
      {
        if (is_fbaginsert_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_fbagcinsert_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_fbaglte_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_fbagjoin_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_fbagintersect_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_fbagdifference_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        arg3 from an application
      /// \param e A data expression
      /// \pre arg3 is defined for e
      /// \return The argument of e that corresponds to arg3
      inline
      data_expression arg3(const data_expression& e)
      {
        if (is_fbaginsert_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
        if (is_fbagcinsert_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
        if (is_fbaglte_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
        if (is_fbagjoin_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
        if (is_fbagintersect_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
        if (is_fbagdifference_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        arg4 from an application
      /// \param e A data expression
      /// \pre arg4 is defined for e
      /// \return The argument of e that corresponds to arg4
      inline
      data_expression arg4(const data_expression& e)
      {
        if (is_fbagjoin_application(e))
        {
          return static_cast< application >(e).arguments()[3];
        }
        if (is_fbagintersect_application(e))
        {
          return static_cast< application >(e).arguments()[3];
        }
        if (is_fbagdifference_application(e))
        {
          return static_cast< application >(e).arguments()[3];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        tail from an application
      /// \param e A data expression
      /// \pre tail is defined for e
      /// \return The argument of e that corresponds to tail
      inline
      data_expression tail(const data_expression& e)
      {
        if (is_fbag_cons_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        arg from an application
      /// \param e A data expression
      /// \pre arg is defined for e
      /// \return The argument of e that corresponds to arg
      inline
      data_expression arg(const data_expression& e)
      {
        if (is_fset2fbag_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        headcount from an application
      /// \param e A data expression
      /// \pre headcount is defined for e
      /// \return The argument of e that corresponds to headcount
      inline
      data_expression headcount(const data_expression& e)
      {
        if (is_fbag_cons_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        left from an application
      /// \param e A data expression
      /// \pre left is defined for e
      /// \return The argument of e that corresponds to left
      inline
      data_expression left(const data_expression& e)
      {
        if (is_fbagcount_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_fbagin_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_fbag2fset_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
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
        variable vf("f",function_sort(s, sort_nat::nat()));
        variable vg("g",function_sort(s, sort_nat::nat()));

        data_equation_vector result;
        data_equation_vector fbag_equations = detail::fbag_struct(s).constructor_equations(fbag(s));
        result.insert(result.end(), fbag_equations.begin(), fbag_equations.end());
        result.push_back(data_equation(make_vector(vd, vp), fbaginsert(s, vd, vp, fbag_empty(s)), fbag_cons(s, vd, vp, fbag_empty(s))));
        result.push_back(data_equation(make_vector(vb, vd, vp, vq), fbaginsert(s, vd, vp, fbag_cons(s, vd, vq, vb)), fbag_cons(s, vd, sort_nat::plus(vp, vq), vb)));
        result.push_back(data_equation(make_vector(vb, vd, ve, vp, vq), less(vd, ve), fbaginsert(s, vd, vp, fbag_cons(s, ve, vq, vb)), fbag_cons(s, vd, vp, fbag_cons(s, ve, vq, vb))));
        result.push_back(data_equation(make_vector(vb, vd, ve, vp, vq), less(ve, vd), fbaginsert(s, vd, vp, fbag_cons(s, ve, vq, vb)), fbag_cons(s, ve, vq, fbaginsert(s, vd, vp, vb))));
        result.push_back(data_equation(make_vector(vb, vd), fbagcinsert(s, vd, sort_nat::c0(), vb), vb));
        result.push_back(data_equation(make_vector(vb, vd, vp), fbagcinsert(s, vd, sort_nat::cnat(vp), vb), fbaginsert(s, vd, vp, vb)));
        result.push_back(data_equation(make_vector(vd), fbagcount(s, vd, fbag_empty(s)), sort_nat::c0()));
        result.push_back(data_equation(make_vector(vb, vd, vp), fbagcount(s, vd, fbag_cons(s, vd, vp, vb)), sort_nat::cnat(vp)));
        result.push_back(data_equation(make_vector(vb, vd, ve, vp), less(vd, ve), fbagcount(s, vd, fbag_cons(s, ve, vp, vb)), sort_nat::c0()));
        result.push_back(data_equation(make_vector(vb, vd, ve, vp), less(ve, vd), fbagcount(s, vd, fbag_cons(s, ve, vp, vb)), fbagcount(s, vd, vb)));
        result.push_back(data_equation(make_vector(vb, vd), fbagin(s, vd, vb), greater(fbagcount(s, vd, vb), sort_nat::c0())));
        result.push_back(data_equation(make_vector(vf), fbaglte(s, vf, fbag_empty(s), fbag_empty(s)), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(vb, vd, vf, vp), fbaglte(s, vf, fbag_cons(s, vd, vp, vb), fbag_empty(s)), sort_bool_::and_(sort_nat::swap_zero_lte(vf(vd), sort_nat::cnat(vp), sort_nat::c0()), fbaglte(s, vf, vb, fbag_empty(s)))));
        result.push_back(data_equation(make_vector(vc, ve, vf, vq), fbaglte(s, vf, fbag_empty(s), fbag_cons(s, ve, vq, vc)), sort_bool_::and_(sort_nat::swap_zero_lte(vf(ve), sort_nat::c0(), sort_nat::cnat(vq)), fbaglte(s, vf, fbag_empty(s), vc))));
        result.push_back(data_equation(make_vector(vb, vc, vd, vf, vp, vq), fbaglte(s, vf, fbag_cons(s, vd, vp, vb), fbag_cons(s, vd, vq, vc)), sort_bool_::and_(sort_nat::swap_zero_lte(vf(vd), sort_nat::cnat(vp), sort_nat::cnat(vq)), fbaglte(s, vf, vb, vc))));
        result.push_back(data_equation(make_vector(vb, vc, vd, ve, vf, vp, vq), less(vd, ve), fbaglte(s, vf, fbag_cons(s, vd, vp, vb), fbag_cons(s, ve, vq, vc)), sort_bool_::and_(sort_nat::swap_zero_lte(vf(vd), sort_nat::cnat(vp), sort_nat::c0()), fbaglte(s, vf, vb, fbag_cons(s, ve, vq, vc)))));
        result.push_back(data_equation(make_vector(vb, vc, vd, ve, vf, vp, vq), less(ve, vd), fbaglte(s, vf, fbag_cons(s, vd, vp, vb), fbag_cons(s, ve, vq, vc)), sort_bool_::and_(sort_nat::swap_zero_lte(vf(ve), sort_nat::c0(), sort_nat::cnat(vq)), fbaglte(s, vf, fbag_cons(s, vd, vp, vb), vc))));
        result.push_back(data_equation(make_vector(vf, vg), fbagjoin(s, vf, vg, fbag_empty(s), fbag_empty(s)), fbag_empty(s)));
        result.push_back(data_equation(make_vector(vb, vd, vf, vg, vp), fbagjoin(s, vf, vg, fbag_cons(s, vd, vp, vb), fbag_empty(s)), fbagcinsert(s, vd, sort_nat::swap_zero_add(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), fbagjoin(s, vf, vg, vb, fbag_empty(s)))));
        result.push_back(data_equation(make_vector(vc, ve, vf, vg, vq), fbagjoin(s, vf, vg, fbag_empty(s), fbag_cons(s, ve, vq, vc)), fbagcinsert(s, ve, sort_nat::swap_zero_add(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), fbagjoin(s, vf, vg, fbag_empty(s), vc))));
        result.push_back(data_equation(make_vector(vb, vc, vd, vf, vg, vp, vq), fbagjoin(s, vf, vg, fbag_cons(s, vd, vp, vb), fbag_cons(s, vd, vq, vc)), fbagcinsert(s, vd, sort_nat::swap_zero_add(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::cnat(vq)), fbagjoin(s, vf, vg, vb, vc))));
        result.push_back(data_equation(make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(vd, ve), fbagjoin(s, vf, vg, fbag_cons(s, vd, vp, vb), fbag_cons(s, ve, vq, vc)), fbagcinsert(s, vd, sort_nat::swap_zero_add(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), fbagjoin(s, vf, vg, vb, fbag_cons(s, ve, vq, vc)))));
        result.push_back(data_equation(make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(ve, vd), fbagjoin(s, vf, vg, fbag_cons(s, vd, vp, vb), fbag_cons(s, ve, vq, vc)), fbagcinsert(s, ve, sort_nat::swap_zero_add(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), fbagjoin(s, vf, vg, fbag_cons(s, vd, vp, vb), vc))));
        result.push_back(data_equation(make_vector(vf, vg), fbagintersect(s, vf, vg, fbag_empty(s), fbag_empty(s)), fbag_empty(s)));
        result.push_back(data_equation(make_vector(vb, vd, vf, vg, vp), fbagintersect(s, vf, vg, fbag_cons(s, vd, vp, vb), fbag_empty(s)), fbagcinsert(s, vd, sort_nat::swap_zero_min(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), fbagintersect(s, vf, vg, vb, fbag_empty(s)))));
        result.push_back(data_equation(make_vector(vc, ve, vf, vg, vq), fbagintersect(s, vf, vg, fbag_empty(s), fbag_cons(s, ve, vq, vc)), fbagcinsert(s, ve, sort_nat::swap_zero_min(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), fbagintersect(s, vf, vg, fbag_empty(s), vc))));
        result.push_back(data_equation(make_vector(vb, vc, vd, vf, vg, vp, vq), fbagintersect(s, vf, vg, fbag_cons(s, vd, vp, vb), fbag_cons(s, vd, vq, vc)), fbagcinsert(s, vd, sort_nat::swap_zero_min(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::cnat(vq)), fbagintersect(s, vf, vg, vb, vc))));
        result.push_back(data_equation(make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(vd, ve), fbagintersect(s, vf, vg, fbag_cons(s, vd, vp, vb), fbag_cons(s, ve, vq, vc)), fbagcinsert(s, vd, sort_nat::swap_zero_min(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), fbagintersect(s, vf, vg, vb, fbag_cons(s, ve, vq, vc)))));
        result.push_back(data_equation(make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(ve, vd), fbagintersect(s, vf, vg, fbag_cons(s, vd, vp, vb), fbag_cons(s, ve, vq, vc)), fbagcinsert(s, ve, sort_nat::swap_zero_min(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), fbagintersect(s, vf, vg, fbag_cons(s, vd, vp, vb), vc))));
        result.push_back(data_equation(make_vector(vf, vg), fbagdifference(s, vf, vg, fbag_empty(s), fbag_empty(s)), fbag_empty(s)));
        result.push_back(data_equation(make_vector(vb, vd, vf, vg, vp), fbagdifference(s, vf, vg, fbag_cons(s, vd, vp, vb), fbag_empty(s)), fbagcinsert(s, vd, sort_nat::swap_zero_monus(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), fbagdifference(s, vf, vg, vb, fbag_empty(s)))));
        result.push_back(data_equation(make_vector(vc, ve, vf, vg, vq), fbagdifference(s, vf, vg, fbag_empty(s), fbag_cons(s, ve, vq, vc)), fbagcinsert(s, ve, sort_nat::swap_zero_monus(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), fbagdifference(s, vf, vg, fbag_empty(s), vc))));
        result.push_back(data_equation(make_vector(vb, vc, vd, vf, vg, vp, vq), fbagdifference(s, vf, vg, fbag_cons(s, vd, vp, vb), fbag_cons(s, vd, vq, vc)), fbagcinsert(s, vd, sort_nat::swap_zero_monus(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::cnat(vq)), fbagdifference(s, vf, vg, vb, vc))));
        result.push_back(data_equation(make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(vd, ve), fbagdifference(s, vf, vg, fbag_cons(s, vd, vp, vb), fbag_cons(s, ve, vq, vc)), fbagcinsert(s, vd, sort_nat::swap_zero_monus(vf(vd), vg(vd), sort_nat::cnat(vp), sort_nat::c0()), fbagdifference(s, vf, vg, vb, fbag_cons(s, ve, vq, vc)))));
        result.push_back(data_equation(make_vector(vb, vc, vd, ve, vf, vg, vp, vq), less(ve, vd), fbagdifference(s, vf, vg, fbag_cons(s, vd, vp, vb), fbag_cons(s, ve, vq, vc)), fbagcinsert(s, ve, sort_nat::swap_zero_monus(vf(ve), vg(ve), sort_nat::c0(), sort_nat::cnat(vq)), fbagdifference(s, vf, vg, fbag_cons(s, vd, vp, vb), vc))));
        result.push_back(data_equation(make_vector(vf), fbag2fset(s, vf, fbag_empty(s)), sort_fset::fset_empty(s)));
        result.push_back(data_equation(make_vector(vb, vd, vf, vp), fbag2fset(s, vf, fbag_cons(s, vd, vp, vb)), sort_fset::fsetcinsert(s, vd, equal_to(equal_to(vf(vd), sort_nat::cnat(vp)), greater(vf(vd), sort_nat::c0())), fbag2fset(s, vf, vb))));
        result.push_back(data_equation(variable_list(), fset2fbag(s, sort_fset::fset_empty(s)), fbag_empty(s)));
        result.push_back(data_equation(make_vector(vd, vs), fset2fbag(s, sort_fset::fset_cons(s, vd, vs)), fbagcinsert(s, vd, sort_nat::cnat(sort_pos::c1()), fset2fbag(s, vs))));
        return result;
      }

      /// \brief Add sort, constructors, mappings and equations for fbag
      /// \param specification a specification
      /// \param element the sort of elements stored by the container
      inline
      void add_fbag_to_specification(data_specification& specification, sort_expression const& element)
      {
         if (specification.constructors(sort_bool_::bool_()).empty())
         {
           sort_bool_::add_bool__to_specification(specification);
         }
         if (specification.constructors(sort_pos::pos()).empty())
         {
           sort_pos::add_pos_to_specification(specification);
         }
         if (specification.constructors(sort_fset::fset(element)).empty())
         {
           sort_fset::add_fset_to_specification(specification, element);
         }
         if (specification.constructors(sort_nat::nat()).empty())
         {
           sort_nat::add_nat_to_specification(specification);
         }
         specification.add_system_defined_sort(fbag(element));
         specification.add_system_defined_constructors(fbag_generate_constructors_code(element));
         specification.add_system_defined_mappings(fbag_generate_functions_code(element));
         specification.add_system_defined_equations(fbag_generate_equations_code(element));
      }
    } // namespace sort_fbag

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FBAG_H
