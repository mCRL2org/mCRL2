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

#include "boost/utility.hpp"

#include "mcrl2/exception.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/detail/container_utility.h"
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
        container_sort bag(container_sort::bag(), s);
        return bag;
      }

      /// \brief Recogniser for sort expression Bag(s)
      /// \param e A sort expression
      /// \return true iff e is a container sort of which the name matches
      ///      bag
      inline
      bool is_bag(const sort_expression& e)
      {
        if (e.is_container_sort())
        {
          return static_cast< container_sort >(e).container_type() == container_sort::bag();
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
        static_cast< void >(s); // suppress unused variable warnings
        return result;
      }
      /// \brief Generate identifier \@bag
      /// \return Identifier \@bag
      inline
      core::identifier_string const& bagconstructor_name()
      {
        static core::identifier_string bagconstructor_name = data::detail::initialise_static_expression(bagconstructor_name, core::identifier_string("@bag"));
        return bagconstructor_name;
      }

      /// \brief Constructor for function symbol \@bag
      /// \param s A sort expression
      /// \return Function symbol bagconstructor
      inline
      function_symbol bagconstructor(const sort_expression& s)
      {
        function_symbol bagconstructor(bagconstructor_name(), function_sort(function_sort(s, sort_nat::nat()), sort_fbag::fbag(s), bag(s)));
        return bagconstructor;
      }


      /// \brief Recogniser for function \@bag
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@bag
      inline
      bool is_bagconstructor_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == bagconstructor_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@bag
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@bag to a number of arguments
      inline
      application bagconstructor(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(bagconstructor(s),arg0, arg1);
      }

      /// \brief Recogniser for application of \@bag
      /// \param e A data expression
      /// \return true iff e is an application of function symbol bagconstructor to a
      ///     number of arguments
      inline
      bool is_bagconstructor_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_bagconstructor_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier {}
      /// \return Identifier {}
      inline
      core::identifier_string const& emptybag_name()
      {
        static core::identifier_string emptybag_name = data::detail::initialise_static_expression(emptybag_name, core::identifier_string("{}"));
        return emptybag_name;
      }

      /// \brief Constructor for function symbol {}
      /// \param s A sort expression
      /// \return Function symbol emptybag
      inline
      function_symbol emptybag(const sort_expression& s)
      {
        function_symbol emptybag(emptybag_name(), bag(s));
        return emptybag;
      }


      /// \brief Recogniser for function {}
      /// \param e A data expression
      /// \return true iff e is the function symbol matching {}
      inline
      bool is_emptybag_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == emptybag_name();
        }
        return false;
      }

      /// \brief Generate identifier \@bagfbag
      /// \return Identifier \@bagfbag
      inline
      core::identifier_string const& bagfbag_name()
      {
        static core::identifier_string bagfbag_name = data::detail::initialise_static_expression(bagfbag_name, core::identifier_string("@bagfbag"));
        return bagfbag_name;
      }

      /// \brief Constructor for function symbol \@bagfbag
      /// \param s A sort expression
      /// \return Function symbol bagfbag
      inline
      function_symbol bagfbag(const sort_expression& s)
      {
        function_symbol bagfbag(bagfbag_name(), function_sort(sort_fbag::fbag(s), bag(s)));
        return bagfbag;
      }


      /// \brief Recogniser for function \@bagfbag
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@bagfbag
      inline
      bool is_bagfbag_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == bagfbag_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@bagfbag
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@bagfbag to a number of arguments
      inline
      application bagfbag(const sort_expression& s, const data_expression& arg0)
      {
        return application(bagfbag(s),arg0);
      }

      /// \brief Recogniser for application of \@bagfbag
      /// \param e A data expression
      /// \return true iff e is an application of function symbol bagfbag to a
      ///     number of arguments
      inline
      bool is_bagfbag_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_bagfbag_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@bagcomp
      /// \return Identifier \@bagcomp
      inline
      core::identifier_string const& bagcomprehension_name()
      {
        static core::identifier_string bagcomprehension_name = data::detail::initialise_static_expression(bagcomprehension_name, core::identifier_string("@bagcomp"));
        return bagcomprehension_name;
      }

      /// \brief Constructor for function symbol \@bagcomp
      /// \param s A sort expression
      /// \return Function symbol bagcomprehension
      inline
      function_symbol bagcomprehension(const sort_expression& s)
      {
        function_symbol bagcomprehension(bagcomprehension_name(), function_sort(function_sort(s, sort_nat::nat()), bag(s)));
        return bagcomprehension;
      }


      /// \brief Recogniser for function \@bagcomp
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@bagcomp
      inline
      bool is_bagcomprehension_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == bagcomprehension_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@bagcomp
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@bagcomp to a number of arguments
      inline
      application bagcomprehension(const sort_expression& s, const data_expression& arg0)
      {
        return application(bagcomprehension(s),arg0);
      }

      /// \brief Recogniser for application of \@bagcomp
      /// \param e A data expression
      /// \return true iff e is an application of function symbol bagcomprehension to a
      ///     number of arguments
      inline
      bool is_bagcomprehension_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_bagcomprehension_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier count
      /// \return Identifier count
      inline
      core::identifier_string const& bagcount_name()
      {
        static core::identifier_string bagcount_name = data::detail::initialise_static_expression(bagcount_name, core::identifier_string("count"));
        return bagcount_name;
      }

      /// \brief Constructor for function symbol count
      /// \param s A sort expression
      /// \return Function symbol bagcount
      inline
      function_symbol bagcount(const sort_expression& s)
      {
        function_symbol bagcount(bagcount_name(), function_sort(s, bag(s), sort_nat::nat()));
        return bagcount;
      }


      /// \brief Recogniser for function count
      /// \param e A data expression
      /// \return true iff e is the function symbol matching count
      inline
      bool is_bagcount_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == bagcount_name();
        }
        return false;
      }

      /// \brief Application of function symbol count
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of count to a number of arguments
      inline
      application bagcount(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(bagcount(s),arg0, arg1);
      }

      /// \brief Recogniser for application of count
      /// \param e A data expression
      /// \return true iff e is an application of function symbol bagcount to a
      ///     number of arguments
      inline
      bool is_bagcount_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_bagcount_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier in
      /// \return Identifier in
      inline
      core::identifier_string const& bagin_name()
      {
        static core::identifier_string bagin_name = data::detail::initialise_static_expression(bagin_name, core::identifier_string("in"));
        return bagin_name;
      }

      /// \brief Constructor for function symbol in
      /// \param s A sort expression
      /// \return Function symbol bagin
      inline
      function_symbol bagin(const sort_expression& s)
      {
        function_symbol bagin(bagin_name(), function_sort(s, bag(s), sort_bool::bool_()));
        return bagin;
      }


      /// \brief Recogniser for function in
      /// \param e A data expression
      /// \return true iff e is the function symbol matching in
      inline
      bool is_bagin_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == bagin_name();
        }
        return false;
      }

      /// \brief Application of function symbol in
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of in to a number of arguments
      inline
      application bagin(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(bagin(s),arg0, arg1);
      }

      /// \brief Recogniser for application of in
      /// \param e A data expression
      /// \return true iff e is an application of function symbol bagin to a
      ///     number of arguments
      inline
      bool is_bagin_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_bagin_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier +
      /// \return Identifier +
      inline
      core::identifier_string const& bagjoin_name()
      {
        static core::identifier_string bagjoin_name = data::detail::initialise_static_expression(bagjoin_name, core::identifier_string("+"));
        return bagjoin_name;
      }

      /// \brief Constructor for function symbol +
      /// \param s A sort expression
      /// \return Function symbol bagjoin
      inline
      function_symbol bagjoin(const sort_expression& s)
      {
        function_symbol bagjoin(bagjoin_name(), function_sort(bag(s), bag(s), bag(s)));
        return bagjoin;
      }


      /// \brief Recogniser for function +
      /// \param e A data expression
      /// \return true iff e is the function symbol matching +
      inline
      bool is_bagjoin_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == bagjoin_name();
        }
        return false;
      }

      /// \brief Application of function symbol +
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of + to a number of arguments
      inline
      application bagjoin(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(bagjoin(s),arg0, arg1);
      }

      /// \brief Recogniser for application of +
      /// \param e A data expression
      /// \return true iff e is an application of function symbol bagjoin to a
      ///     number of arguments
      inline
      bool is_bagjoin_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_bagjoin_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier *
      /// \return Identifier *
      inline
      core::identifier_string const& bagintersect_name()
      {
        static core::identifier_string bagintersect_name = data::detail::initialise_static_expression(bagintersect_name, core::identifier_string("*"));
        return bagintersect_name;
      }

      /// \brief Constructor for function symbol *
      /// \param s A sort expression
      /// \return Function symbol bagintersect
      inline
      function_symbol bagintersect(const sort_expression& s)
      {
        function_symbol bagintersect(bagintersect_name(), function_sort(bag(s), bag(s), bag(s)));
        return bagintersect;
      }


      /// \brief Recogniser for function *
      /// \param e A data expression
      /// \return true iff e is the function symbol matching *
      inline
      bool is_bagintersect_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == bagintersect_name();
        }
        return false;
      }

      /// \brief Application of function symbol *
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of * to a number of arguments
      inline
      application bagintersect(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(bagintersect(s),arg0, arg1);
      }

      /// \brief Recogniser for application of *
      /// \param e A data expression
      /// \return true iff e is an application of function symbol bagintersect to a
      ///     number of arguments
      inline
      bool is_bagintersect_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_bagintersect_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier -
      /// \return Identifier -
      inline
      core::identifier_string const& bagdifference_name()
      {
        static core::identifier_string bagdifference_name = data::detail::initialise_static_expression(bagdifference_name, core::identifier_string("-"));
        return bagdifference_name;
      }

      /// \brief Constructor for function symbol -
      /// \param s A sort expression
      /// \return Function symbol bagdifference
      inline
      function_symbol bagdifference(const sort_expression& s)
      {
        function_symbol bagdifference(bagdifference_name(), function_sort(bag(s), bag(s), bag(s)));
        return bagdifference;
      }


      /// \brief Recogniser for function -
      /// \param e A data expression
      /// \return true iff e is the function symbol matching -
      inline
      bool is_bagdifference_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == bagdifference_name();
        }
        return false;
      }

      /// \brief Application of function symbol -
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of - to a number of arguments
      inline
      application bagdifference(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(bagdifference(s),arg0, arg1);
      }

      /// \brief Recogniser for application of -
      /// \param e A data expression
      /// \return true iff e is an application of function symbol bagdifference to a
      ///     number of arguments
      inline
      bool is_bagdifference_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_bagdifference_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier Bag2Set
      /// \return Identifier Bag2Set
      inline
      core::identifier_string const& bag2set_name()
      {
        static core::identifier_string bag2set_name = data::detail::initialise_static_expression(bag2set_name, core::identifier_string("Bag2Set"));
        return bag2set_name;
      }

      /// \brief Constructor for function symbol Bag2Set
      /// \param s A sort expression
      /// \return Function symbol bag2set
      inline
      function_symbol bag2set(const sort_expression& s)
      {
        function_symbol bag2set(bag2set_name(), function_sort(bag(s), sort_set::set_(s)));
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
        return application(bag2set(s),arg0);
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
          return is_bag2set_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier Set2Bag
      /// \return Identifier Set2Bag
      inline
      core::identifier_string const& set2bag_name()
      {
        static core::identifier_string set2bag_name = data::detail::initialise_static_expression(set2bag_name, core::identifier_string("Set2Bag"));
        return set2bag_name;
      }

      /// \brief Constructor for function symbol Set2Bag
      /// \param s A sort expression
      /// \return Function symbol set2bag
      inline
      function_symbol set2bag(const sort_expression& s)
      {
        function_symbol set2bag(set2bag_name(), function_sort(sort_set::set_(s), bag(s)));
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
        return application(set2bag(s),arg0);
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
          return is_set2bag_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@zero_
      /// \return Identifier \@zero_
      inline
      core::identifier_string const& zero_function_name()
      {
        static core::identifier_string zero_function_name = data::detail::initialise_static_expression(zero_function_name, core::identifier_string("@zero_"));
        return zero_function_name;
      }

      /// \brief Constructor for function symbol \@zero_
      /// \param s A sort expression
      /// \return Function symbol zero_function
      inline
      function_symbol zero_function(const sort_expression& s)
      {
        function_symbol zero_function(zero_function_name(), function_sort(s, sort_nat::nat()));
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
        return application(zero_function(s),arg0);
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
          return is_zero_function_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@one_
      /// \return Identifier \@one_
      inline
      core::identifier_string const& one_function_name()
      {
        static core::identifier_string one_function_name = data::detail::initialise_static_expression(one_function_name, core::identifier_string("@one_"));
        return one_function_name;
      }

      /// \brief Constructor for function symbol \@one_
      /// \param s A sort expression
      /// \return Function symbol one_function
      inline
      function_symbol one_function(const sort_expression& s)
      {
        function_symbol one_function(one_function_name(), function_sort(s, sort_nat::nat()));
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
        return application(one_function(s),arg0);
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
          return is_one_function_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@add_
      /// \return Identifier \@add_
      inline
      core::identifier_string const& add_function_name()
      {
        static core::identifier_string add_function_name = data::detail::initialise_static_expression(add_function_name, core::identifier_string("@add_"));
        return add_function_name;
      }

      /// \brief Constructor for function symbol \@add_
      /// \param s A sort expression
      /// \return Function symbol add_function
      inline
      function_symbol add_function(const sort_expression& s)
      {
        function_symbol add_function(add_function_name(), function_sort(function_sort(s, sort_nat::nat()), function_sort(s, sort_nat::nat()), function_sort(s, sort_nat::nat())));
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
        return application(add_function(s),arg0, arg1);
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
          return is_add_function_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@min_
      /// \return Identifier \@min_
      inline
      core::identifier_string const& min_function_name()
      {
        static core::identifier_string min_function_name = data::detail::initialise_static_expression(min_function_name, core::identifier_string("@min_"));
        return min_function_name;
      }

      /// \brief Constructor for function symbol \@min_
      /// \param s A sort expression
      /// \return Function symbol min_function
      inline
      function_symbol min_function(const sort_expression& s)
      {
        function_symbol min_function(min_function_name(), function_sort(function_sort(s, sort_nat::nat()), function_sort(s, sort_nat::nat()), function_sort(s, sort_nat::nat())));
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
        return application(min_function(s),arg0, arg1);
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
          return is_min_function_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@monus_
      /// \return Identifier \@monus_
      inline
      core::identifier_string const& monus_function_name()
      {
        static core::identifier_string monus_function_name = data::detail::initialise_static_expression(monus_function_name, core::identifier_string("@monus_"));
        return monus_function_name;
      }

      /// \brief Constructor for function symbol \@monus_
      /// \param s A sort expression
      /// \return Function symbol monus_function
      inline
      function_symbol monus_function(const sort_expression& s)
      {
        function_symbol monus_function(monus_function_name(), function_sort(function_sort(s, sort_nat::nat()), function_sort(s, sort_nat::nat()), function_sort(s, sort_nat::nat())));
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
        return application(monus_function(s),arg0, arg1);
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
          return is_monus_function_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@Nat2Bool_
      /// \return Identifier \@Nat2Bool_
      inline
      core::identifier_string const& nat2bool_function_name()
      {
        static core::identifier_string nat2bool_function_name = data::detail::initialise_static_expression(nat2bool_function_name, core::identifier_string("@Nat2Bool_"));
        return nat2bool_function_name;
      }

      /// \brief Constructor for function symbol \@Nat2Bool_
      /// \param s A sort expression
      /// \return Function symbol nat2bool_function
      inline
      function_symbol nat2bool_function(const sort_expression& s)
      {
        function_symbol nat2bool_function(nat2bool_function_name(), function_sort(function_sort(s, sort_nat::nat()), function_sort(s, sort_bool::bool_())));
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
        return application(nat2bool_function(s),arg0);
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
          return is_nat2bool_function_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@Bool2Nat_
      /// \return Identifier \@Bool2Nat_
      inline
      core::identifier_string const& bool2nat_function_name()
      {
        static core::identifier_string bool2nat_function_name = data::detail::initialise_static_expression(bool2nat_function_name, core::identifier_string("@Bool2Nat_"));
        return bool2nat_function_name;
      }

      /// \brief Constructor for function symbol \@Bool2Nat_
      /// \param s A sort expression
      /// \return Function symbol bool2nat_function
      inline
      function_symbol bool2nat_function(const sort_expression& s)
      {
        function_symbol bool2nat_function(bool2nat_function_name(), function_sort(function_sort(s, sort_bool::bool_()), function_sort(s, sort_nat::nat())));
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
        return application(bool2nat_function(s),arg0);
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
          return is_bool2nat_function_function_symbol(static_cast< application >(e).head());
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
        result.push_back(bagconstructor(s));
        result.push_back(emptybag(s));
        result.push_back(bagfbag(s));
        result.push_back(bagcomprehension(s));
        result.push_back(bagcount(s));
        result.push_back(bagin(s));
        result.push_back(bagjoin(s));
        result.push_back(bagintersect(s));
        result.push_back(bagdifference(s));
        result.push_back(bag2set(s));
        result.push_back(set2bag(s));
        result.push_back(zero_function(s));
        result.push_back(one_function(s));
        result.push_back(add_function(s));
        result.push_back(min_function(s));
        result.push_back(monus_function(s));
        result.push_back(nat2bool_function(s));
        result.push_back(bool2nat_function(s));
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
        assert(is_bagconstructor_application(e) || is_bagcount_application(e) || is_bagin_application(e) || is_bagjoin_application(e) || is_bagintersect_application(e) || is_bagdifference_application(e) || is_add_function_application(e) || is_min_function_application(e) || is_monus_function_application(e));
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
        assert(is_bagfbag_application(e) || is_bagcomprehension_application(e) || is_bag2set_application(e) || is_set2bag_application(e) || is_zero_function_application(e) || is_one_function_application(e) || is_nat2bool_function_application(e) || is_bool2nat_function_application(e));
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
        assert(is_bagconstructor_application(e) || is_bagcount_application(e) || is_bagin_application(e) || is_bagjoin_application(e) || is_bagintersect_application(e) || is_bagdifference_application(e) || is_add_function_application(e) || is_min_function_application(e) || is_monus_function_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 0);
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
        variable vf("f",function_sort(s, sort_nat::nat()));
        variable vg("g",function_sort(s, sort_nat::nat()));
        variable vh("h",function_sort(s, sort_bool::bool_()));
        variable vs("s",sort_fset::fset(s));
        variable vx("x",bag(s));
        variable vy("y",bag(s));
        variable vd("d",s);

        data_equation_vector result;
        result.push_back(data_equation(variable_list(), emptybag(s), bagconstructor(s, zero_function(s), sort_fbag::fbag_empty(s))));
        result.push_back(data_equation(make_vector(vb), bagfbag(s, vb), bagconstructor(s, zero_function(s), vb)));
        result.push_back(data_equation(make_vector(vf), bagcomprehension(s, vf), bagconstructor(s, vf, sort_fbag::fbag_empty(s))));
        result.push_back(data_equation(make_vector(vb, ve, vf), bagcount(s, ve, bagconstructor(s, vf, vb)), sort_nat::swap_zero(vf(ve), sort_fbag::fbagcount(s, ve, vb))));
        result.push_back(data_equation(make_vector(ve, vx), bagin(s, ve, vx), greater(bagcount(s, ve, vx), sort_nat::c0())));
        result.push_back(data_equation(make_vector(vb, vc, vf, vg), equal_to(vf, vg), equal_to(bagconstructor(s, vf, vb), bagconstructor(s, vg, vc)), equal_to(vb, vc)));
        result.push_back(data_equation(make_vector(vb, vc, vf, vg), not_equal_to(vf, vg), equal_to(bagconstructor(s, vf, vb), bagconstructor(s, vg, vc)), forall(make_vector(vd), equal_to(bagcount(s, vd, bagconstructor(s, vf, vb)), bagcount(s, vd, bagconstructor(s, vg, vc))))));
        result.push_back(data_equation(make_vector(vx, vy), less(vx, vy), sort_bool::and_(less_equal(vx, vy), not_equal_to(vx, vy))));
        result.push_back(data_equation(make_vector(vb, vc, vf, vg), equal_to(vf, vg), less_equal(bagconstructor(s, vf, vb), bagconstructor(s, vg, vc)), sort_fbag::fbaglte(s, vf, vb, vc)));
        result.push_back(data_equation(make_vector(vb, vc, vf, vg), not_equal_to(vf, vg), less_equal(bagconstructor(s, vf, vb), bagconstructor(s, vg, vc)), forall(make_vector(vd), less_equal(bagcount(s, vd, bagconstructor(s, vf, vb)), bagcount(s, vd, bagconstructor(s, vg, vc))))));
        result.push_back(data_equation(make_vector(vb, vc, vf, vg), bagjoin(s, bagconstructor(s, vf, vb), bagconstructor(s, vg, vc)), bagconstructor(s, add_function(s, vf, vg), sort_fbag::fbagjoin(s, vf, vg, vb, vc))));
        result.push_back(data_equation(make_vector(vb, vc, vf, vg), bagintersect(s, bagconstructor(s, vf, vb), bagconstructor(s, vg, vc)), bagconstructor(s, min_function(s, vf, vg), sort_fbag::fbagintersect(s, vf, vg, vb, vc))));
        result.push_back(data_equation(make_vector(vb, vc, vf, vg), bagdifference(s, bagconstructor(s, vf, vb), bagconstructor(s, vg, vc)), bagconstructor(s, monus_function(s, vf, vg), sort_fbag::fbagdifference(s, vf, vg, vb, vc))));
        result.push_back(data_equation(make_vector(vb, vf), bag2set(s, bagconstructor(s, vf, vb)), sort_set::setconstructor(s, nat2bool_function(s, vf), sort_fbag::fbag2fset(s, vf, vb))));
        result.push_back(data_equation(make_vector(vh, vs), set2bag(s, sort_set::setconstructor(s, vh, vs)), bagconstructor(s, bool2nat_function(s, vh), sort_fbag::fset2fbag(s, vs))));
        result.push_back(data_equation(make_vector(ve), zero_function(s, ve), sort_nat::c0()));
        result.push_back(data_equation(make_vector(ve), one_function(s, ve), sort_nat::cnat(sort_pos::c1())));
        result.push_back(data_equation(variable_list(), equal_to(zero_function(s), one_function(s)), sort_bool::false_()));
        result.push_back(data_equation(variable_list(), equal_to(one_function(s), zero_function(s)), sort_bool::false_()));
        result.push_back(data_equation(make_vector(ve, vf, vg), add_function(s, vf, vg)(ve), sort_nat::plus(vf(ve), vg(ve))));
        result.push_back(data_equation(make_vector(vf), add_function(s, vf, zero_function(s)), vf));
        result.push_back(data_equation(make_vector(vf), add_function(s, zero_function(s), vf), vf));
        result.push_back(data_equation(make_vector(ve, vf, vg), min_function(s, vf, vg)(ve), sort_nat::minimum(vf(ve), vg(ve))));
        result.push_back(data_equation(make_vector(vf), min_function(s, vf, vf), vf));
        result.push_back(data_equation(make_vector(vf), min_function(s, vf, zero_function(s)), zero_function(s)));
        result.push_back(data_equation(make_vector(vf), min_function(s, zero_function(s), vf), zero_function(s)));
        result.push_back(data_equation(make_vector(ve, vf, vg), monus_function(s, vf, vg)(ve), sort_nat::monus(vf(ve), vg(ve))));
        result.push_back(data_equation(make_vector(vf), monus_function(s, vf, vf), zero_function(s)));
        result.push_back(data_equation(make_vector(vf), monus_function(s, vf, zero_function(s)), vf));
        result.push_back(data_equation(make_vector(vf), monus_function(s, zero_function(s), vf), zero_function(s)));
        result.push_back(data_equation(make_vector(ve, vf), nat2bool_function(s, vf)(ve), greater(vf(ve), sort_nat::c0())));
        result.push_back(data_equation(variable_list(), nat2bool_function(s, zero_function(s)), sort_set::false_function(s)));
        result.push_back(data_equation(variable_list(), nat2bool_function(s, one_function(s)), sort_set::true_function(s)));
        result.push_back(data_equation(make_vector(ve, vh), bool2nat_function(s, vh)(ve), if_(vh(ve), sort_nat::cnat(sort_pos::c1()), sort_nat::c0())));
        result.push_back(data_equation(variable_list(), bool2nat_function(s, sort_set::false_function(s)), zero_function(s)));
        result.push_back(data_equation(variable_list(), bool2nat_function(s, sort_set::true_function(s)), one_function(s)));
        return result;
      }

    } // namespace sort_bag

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_BAG_H
