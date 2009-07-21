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

#include "mcrl2/exception.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/detail/container_utility.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/data_specification.h"
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
        container_sort set_(container_sort::set_(), s);
        return set_;
      }

      /// \brief Recogniser for sort expression Set(s)
      /// \param e A sort expression
      /// \return true iff e is a container sort of which the name matches
      ///      set_
      inline
      bool is_set(const sort_expression& e)
      {
        if (e.is_container_sort())
        {
          return static_cast< container_sort >(e).container_type() == container_sort::set_();
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

        static_cast< void >(result); // suppress unused variable warnings

        return result;
      }
      /// \brief Generate identifier \@set
      /// \return Identifier \@set
      inline
      core::identifier_string const& setconstructor_name()
      {
        static core::identifier_string setconstructor_name = data::detail::initialise_static_expression(setconstructor_name, core::identifier_string("@set"));
        return setconstructor_name;
      }

      /// \brief Constructor for function symbol \@set
      /// \param s A sort expression
      /// \return Function symbol setconstructor
      inline
      function_symbol setconstructor(const sort_expression& s)
      {
        function_symbol setconstructor(setconstructor_name(), function_sort(function_sort(s, sort_bool::bool_()), sort_fset::fset(s), set_(s)));
        return setconstructor;
      }


      /// \brief Recogniser for function \@set
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@set
      inline
      bool is_setconstructor_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == setconstructor_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@set
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@set to a number of arguments
      inline
      application setconstructor(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(setconstructor(s),arg0, arg1);
      }

      /// \brief Recogniser for application of \@set
      /// \param e A data expression
      /// \return true iff e is an application of function symbol setconstructor to a
      ///     number of arguments
      inline
      bool is_setconstructor_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setconstructor_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier {}
      /// \return Identifier {}
      inline
      core::identifier_string const& emptyset_name()
      {
        static core::identifier_string emptyset_name = data::detail::initialise_static_expression(emptyset_name, core::identifier_string("{}"));
        return emptyset_name;
      }

      /// \brief Constructor for function symbol {}
      /// \param s A sort expression
      /// \return Function symbol emptyset
      inline
      function_symbol emptyset(const sort_expression& s)
      {
        function_symbol emptyset(emptyset_name(), set_(s));
        return emptyset;
      }


      /// \brief Recogniser for function {}
      /// \param e A data expression
      /// \return true iff e is the function symbol matching {}
      inline
      bool is_emptyset_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == emptyset_name();
        }
        return false;
      }

      /// \brief Generate identifier \@setfset
      /// \return Identifier \@setfset
      inline
      core::identifier_string const& setfset_name()
      {
        static core::identifier_string setfset_name = data::detail::initialise_static_expression(setfset_name, core::identifier_string("@setfset"));
        return setfset_name;
      }

      /// \brief Constructor for function symbol \@setfset
      /// \param s A sort expression
      /// \return Function symbol setfset
      inline
      function_symbol setfset(const sort_expression& s)
      {
        function_symbol setfset(setfset_name(), function_sort(sort_fset::fset(s), set_(s)));
        return setfset;
      }


      /// \brief Recogniser for function \@setfset
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@setfset
      inline
      bool is_setfset_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == setfset_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@setfset
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@setfset to a number of arguments
      inline
      application setfset(const sort_expression& s, const data_expression& arg0)
      {
        return application(setfset(s),arg0);
      }

      /// \brief Recogniser for application of \@setfset
      /// \param e A data expression
      /// \return true iff e is an application of function symbol setfset to a
      ///     number of arguments
      inline
      bool is_setfset_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setfset_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@setcomp
      /// \return Identifier \@setcomp
      inline
      core::identifier_string const& setcomprehension_name()
      {
        static core::identifier_string setcomprehension_name = data::detail::initialise_static_expression(setcomprehension_name, core::identifier_string("@setcomp"));
        return setcomprehension_name;
      }

      /// \brief Constructor for function symbol \@setcomp
      /// \param s A sort expression
      /// \return Function symbol setcomprehension
      inline
      function_symbol setcomprehension(const sort_expression& s)
      {
        function_symbol setcomprehension(setcomprehension_name(), function_sort(function_sort(s, sort_bool::bool_()), set_(s)));
        return setcomprehension;
      }


      /// \brief Recogniser for function \@setcomp
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@setcomp
      inline
      bool is_setcomprehension_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == setcomprehension_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@setcomp
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of \@setcomp to a number of arguments
      inline
      application setcomprehension(const sort_expression& s, const data_expression& arg0)
      {
        return application(setcomprehension(s),arg0);
      }

      /// \brief Recogniser for application of \@setcomp
      /// \param e A data expression
      /// \return true iff e is an application of function symbol setcomprehension to a
      ///     number of arguments
      inline
      bool is_setcomprehension_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setcomprehension_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier in
      /// \return Identifier in
      inline
      core::identifier_string const& setin_name()
      {
        static core::identifier_string setin_name = data::detail::initialise_static_expression(setin_name, core::identifier_string("in"));
        return setin_name;
      }

      /// \brief Constructor for function symbol in
      /// \param s A sort expression
      /// \return Function symbol setin
      inline
      function_symbol setin(const sort_expression& s)
      {
        function_symbol setin(setin_name(), function_sort(s, set_(s), sort_bool::bool_()));
        return setin;
      }


      /// \brief Recogniser for function in
      /// \param e A data expression
      /// \return true iff e is the function symbol matching in
      inline
      bool is_setin_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == setin_name();
        }
        return false;
      }

      /// \brief Application of function symbol in
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of in to a number of arguments
      inline
      application setin(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(setin(s),arg0, arg1);
      }

      /// \brief Recogniser for application of in
      /// \param e A data expression
      /// \return true iff e is an application of function symbol setin to a
      ///     number of arguments
      inline
      bool is_setin_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setin_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier !
      /// \return Identifier !
      inline
      core::identifier_string const& setcomplement_name()
      {
        static core::identifier_string setcomplement_name = data::detail::initialise_static_expression(setcomplement_name, core::identifier_string("!"));
        return setcomplement_name;
      }

      /// \brief Constructor for function symbol !
      /// \param s A sort expression
      /// \return Function symbol setcomplement
      inline
      function_symbol setcomplement(const sort_expression& s)
      {
        function_symbol setcomplement(setcomplement_name(), function_sort(set_(s), set_(s)));
        return setcomplement;
      }


      /// \brief Recogniser for function !
      /// \param e A data expression
      /// \return true iff e is the function symbol matching !
      inline
      bool is_setcomplement_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == setcomplement_name();
        }
        return false;
      }

      /// \brief Application of function symbol !
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of ! to a number of arguments
      inline
      application setcomplement(const sort_expression& s, const data_expression& arg0)
      {
        return application(setcomplement(s),arg0);
      }

      /// \brief Recogniser for application of !
      /// \param e A data expression
      /// \return true iff e is an application of function symbol setcomplement to a
      ///     number of arguments
      inline
      bool is_setcomplement_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setcomplement_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier +
      /// \return Identifier +
      inline
      core::identifier_string const& setunion_name()
      {
        static core::identifier_string setunion_name = data::detail::initialise_static_expression(setunion_name, core::identifier_string("+"));
        return setunion_name;
      }

      /// \brief Constructor for function symbol +
      /// \param s A sort expression
      /// \return Function symbol setunion_
      inline
      function_symbol setunion_(const sort_expression& s)
      {
        function_symbol setunion_(setunion_name(), function_sort(set_(s), set_(s), set_(s)));
        return setunion_;
      }


      /// \brief Recogniser for function +
      /// \param e A data expression
      /// \return true iff e is the function symbol matching +
      inline
      bool is_setunion_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == setunion_name();
        }
        return false;
      }

      /// \brief Application of function symbol +
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of + to a number of arguments
      inline
      application setunion_(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(setunion_(s),arg0, arg1);
      }

      /// \brief Recogniser for application of +
      /// \param e A data expression
      /// \return true iff e is an application of function symbol setunion_ to a
      ///     number of arguments
      inline
      bool is_setunion_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setunion_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier *
      /// \return Identifier *
      inline
      core::identifier_string const& setintersection_name()
      {
        static core::identifier_string setintersection_name = data::detail::initialise_static_expression(setintersection_name, core::identifier_string("*"));
        return setintersection_name;
      }

      /// \brief Constructor for function symbol *
      /// \param s A sort expression
      /// \return Function symbol setintersection
      inline
      function_symbol setintersection(const sort_expression& s)
      {
        function_symbol setintersection(setintersection_name(), function_sort(set_(s), set_(s), set_(s)));
        return setintersection;
      }


      /// \brief Recogniser for function *
      /// \param e A data expression
      /// \return true iff e is the function symbol matching *
      inline
      bool is_setintersection_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == setintersection_name();
        }
        return false;
      }

      /// \brief Application of function symbol *
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of * to a number of arguments
      inline
      application setintersection(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(setintersection(s),arg0, arg1);
      }

      /// \brief Recogniser for application of *
      /// \param e A data expression
      /// \return true iff e is an application of function symbol setintersection to a
      ///     number of arguments
      inline
      bool is_setintersection_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setintersection_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier -
      /// \return Identifier -
      inline
      core::identifier_string const& setdifference_name()
      {
        static core::identifier_string setdifference_name = data::detail::initialise_static_expression(setdifference_name, core::identifier_string("-"));
        return setdifference_name;
      }

      /// \brief Constructor for function symbol -
      /// \param s A sort expression
      /// \return Function symbol setdifference
      inline
      function_symbol setdifference(const sort_expression& s)
      {
        function_symbol setdifference(setdifference_name(), function_sort(set_(s), set_(s), set_(s)));
        return setdifference;
      }


      /// \brief Recogniser for function -
      /// \param e A data expression
      /// \return true iff e is the function symbol matching -
      inline
      bool is_setdifference_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == setdifference_name();
        }
        return false;
      }

      /// \brief Application of function symbol -
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of - to a number of arguments
      inline
      application setdifference(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(setdifference(s),arg0, arg1);
      }

      /// \brief Recogniser for application of -
      /// \param e A data expression
      /// \return true iff e is an application of function symbol setdifference to a
      ///     number of arguments
      inline
      bool is_setdifference_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setdifference_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@false_
      /// \return Identifier \@false_
      inline
      core::identifier_string const& false_function_name()
      {
        static core::identifier_string false_function_name = data::detail::initialise_static_expression(false_function_name, core::identifier_string("@false_"));
        return false_function_name;
      }

      /// \brief Constructor for function symbol \@false_
      /// \param s A sort expression
      /// \return Function symbol false_function
      inline
      function_symbol false_function(const sort_expression& s)
      {
        function_symbol false_function(false_function_name(), function_sort(s, sort_bool::bool_()));
        return false_function;
      }


      /// \brief Recogniser for function \@false_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@false_
      inline
      bool is_false_function_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == false_function_name();
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
        return application(false_function(s),arg0);
      }

      /// \brief Recogniser for application of \@false_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol false_function to a
      ///     number of arguments
      inline
      bool is_false_function_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_false_function_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@true_
      /// \return Identifier \@true_
      inline
      core::identifier_string const& true_function_name()
      {
        static core::identifier_string true_function_name = data::detail::initialise_static_expression(true_function_name, core::identifier_string("@true_"));
        return true_function_name;
      }

      /// \brief Constructor for function symbol \@true_
      /// \param s A sort expression
      /// \return Function symbol true_function
      inline
      function_symbol true_function(const sort_expression& s)
      {
        function_symbol true_function(true_function_name(), function_sort(s, sort_bool::bool_()));
        return true_function;
      }


      /// \brief Recogniser for function \@true_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@true_
      inline
      bool is_true_function_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == true_function_name();
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
        return application(true_function(s),arg0);
      }

      /// \brief Recogniser for application of \@true_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol true_function to a
      ///     number of arguments
      inline
      bool is_true_function_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_true_function_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@not_
      /// \return Identifier \@not_
      inline
      core::identifier_string const& not_function_name()
      {
        static core::identifier_string not_function_name = data::detail::initialise_static_expression(not_function_name, core::identifier_string("@not_"));
        return not_function_name;
      }

      /// \brief Constructor for function symbol \@not_
      /// \param s A sort expression
      /// \return Function symbol not_function
      inline
      function_symbol not_function(const sort_expression& s)
      {
        function_symbol not_function(not_function_name(), function_sort(function_sort(s, sort_bool::bool_()), function_sort(s, sort_bool::bool_())));
        return not_function;
      }


      /// \brief Recogniser for function \@not_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@not_
      inline
      bool is_not_function_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == not_function_name();
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
        return application(not_function(s),arg0);
      }

      /// \brief Recogniser for application of \@not_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol not_function to a
      ///     number of arguments
      inline
      bool is_not_function_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_not_function_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@and_
      /// \return Identifier \@and_
      inline
      core::identifier_string const& and_function_name()
      {
        static core::identifier_string and_function_name = data::detail::initialise_static_expression(and_function_name, core::identifier_string("@and_"));
        return and_function_name;
      }

      /// \brief Constructor for function symbol \@and_
      /// \param s A sort expression
      /// \return Function symbol and_function
      inline
      function_symbol and_function(const sort_expression& s)
      {
        function_symbol and_function(and_function_name(), function_sort(function_sort(s, sort_bool::bool_()), function_sort(s, sort_bool::bool_()), function_sort(s, sort_bool::bool_())));
        return and_function;
      }


      /// \brief Recogniser for function \@and_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@and_
      inline
      bool is_and_function_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == and_function_name();
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
        return application(and_function(s),arg0, arg1);
      }

      /// \brief Recogniser for application of \@and_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol and_function to a
      ///     number of arguments
      inline
      bool is_and_function_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_and_function_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@or_
      /// \return Identifier \@or_
      inline
      core::identifier_string const& or_function_name()
      {
        static core::identifier_string or_function_name = data::detail::initialise_static_expression(or_function_name, core::identifier_string("@or_"));
        return or_function_name;
      }

      /// \brief Constructor for function symbol \@or_
      /// \param s A sort expression
      /// \return Function symbol or_function
      inline
      function_symbol or_function(const sort_expression& s)
      {
        function_symbol or_function(or_function_name(), function_sort(function_sort(s, sort_bool::bool_()), function_sort(s, sort_bool::bool_()), function_sort(s, sort_bool::bool_())));
        return or_function;
      }


      /// \brief Recogniser for function \@or_
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@or_
      inline
      bool is_or_function_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == or_function_name();
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
        return application(or_function(s),arg0, arg1);
      }

      /// \brief Recogniser for application of \@or_
      /// \param e A data expression
      /// \return true iff e is an application of function symbol or_function to a
      ///     number of arguments
      inline
      bool is_or_function_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_or_function_function_symbol(static_cast< application >(e).head());
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
        result.push_back(setconstructor(s));
        result.push_back(emptyset(s));
        result.push_back(setfset(s));
        result.push_back(setcomprehension(s));
        result.push_back(setin(s));
        result.push_back(setcomplement(s));
        result.push_back(setunion_(s));
        result.push_back(setintersection(s));
        result.push_back(setdifference(s));
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
        assert(is_setconstructor_application(e) || is_setin_application(e) || is_setunion_application(e) || is_setintersection_application(e) || is_setdifference_application(e) || is_and_function_application(e) || is_or_function_application(e));
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
        assert(is_setfset_application(e) || is_setcomprehension_application(e) || is_setcomplement_application(e) || is_false_function_application(e) || is_true_function_application(e) || is_not_function_application(e));
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
        assert(is_setconstructor_application(e) || is_setin_application(e) || is_setunion_application(e) || is_setintersection_application(e) || is_setdifference_application(e) || is_and_function_application(e) || is_or_function_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 0);
      }

      /// \brief Give all system defined equations for set_
      /// \param s A sort expression
      /// \return All system defined equations for sort set_
      inline
      data_equation_vector set_generate_equations_code(const sort_expression& s)
      {
        variable ve("e",s);
        variable vd("d",s);
        variable vs("s",sort_fset::fset(s));
        variable vt("t",sort_fset::fset(s));
        variable vf("f",function_sort(s, sort_bool::bool_()));
        variable vg("g",function_sort(s, sort_bool::bool_()));
        variable vx("x",set_(s));
        variable vy("y",set_(s));
        variable vc("c",s);

        data_equation_vector result;
        result.push_back(data_equation(variable_list(), emptyset(s), setconstructor(s, false_function(s), sort_fset::fset_empty(s))));
        result.push_back(data_equation(make_vector(vs), setfset(s, vs), setconstructor(s, false_function(s), vs)));
        result.push_back(data_equation(make_vector(vf), setcomprehension(s, vf), setconstructor(s, vf, sort_fset::fset_empty(s))));
        result.push_back(data_equation(make_vector(ve, vf, vs), setin(s, ve, setconstructor(s, vf, vs)), not_equal_to(vf(ve), sort_fset::fsetin(s, ve, vs))));
        result.push_back(data_equation(make_vector(vf, vg, vs, vt), equal_to(vf, vg), equal_to(setconstructor(s, vf, vs), setconstructor(s, vg, vt)), equal_to(vs, vt)));
        result.push_back(data_equation(make_vector(vf, vg, vs, vt), not_equal_to(vf, vg), equal_to(setconstructor(s, vf, vs), setconstructor(s, vg, vt)), forall(make_vector(vc), equal_to(setin(s, vc, setconstructor(s, vf, vs)), setin(s, vc, setconstructor(s, vg, vt))))));
        result.push_back(data_equation(make_vector(vx, vy), less(vx, vy), sort_bool::and_(less_equal(vx, vy), not_equal_to(vx, vy))));
        result.push_back(data_equation(make_vector(vf, vg, vs, vt), equal_to(vf, vg), less_equal(setconstructor(s, vf, vs), setconstructor(s, vg, vt)), sort_fset::fsetlte(s, vf, vs, vt)));
        result.push_back(data_equation(make_vector(vf, vg, vs, vt), not_equal_to(vf, vg), less_equal(setconstructor(s, vf, vs), setconstructor(s, vg, vt)), forall(make_vector(vc), sort_bool::implies(setin(s, vc, setconstructor(s, vf, vs)), setin(s, vc, setconstructor(s, vg, vt))))));
        result.push_back(data_equation(make_vector(vf, vs), setcomplement(s, setconstructor(s, vf, vs)), setconstructor(s, not_function(s, vf), vs)));
        result.push_back(data_equation(make_vector(vf, vg, vs, vt), setunion_(s, setconstructor(s, vf, vs), setconstructor(s, vg, vt)), setconstructor(s, or_function(s, vf, vg), sort_fset::fsetunion(s, vf, vg, vs, vt))));
        result.push_back(data_equation(make_vector(vf, vg, vs, vt), setintersection(s, setconstructor(s, vf, vs), setconstructor(s, vg, vt)), setconstructor(s, and_function(s, vf, vg), sort_fset::fsetintersection(s, vf, vg, vs, vt))));
        result.push_back(data_equation(make_vector(vx, vy), setdifference(s, vx, vy), setintersection(s, vx, setcomplement(s, vy))));
        result.push_back(data_equation(make_vector(ve), false_function(s, ve), sort_bool::false_()));
        result.push_back(data_equation(make_vector(ve), true_function(s, ve), sort_bool::true_()));
        result.push_back(data_equation(variable_list(), equal_to(false_function(s), true_function(s)), sort_bool::false_()));
        result.push_back(data_equation(variable_list(), equal_to(true_function(s), false_function(s)), sort_bool::false_()));
        result.push_back(data_equation(make_vector(ve, vf), not_function(s, vf)(ve), sort_bool::not_(vf(ve))));
        result.push_back(data_equation(variable_list(), not_function(s, false_function(s)), true_function(s)));
        result.push_back(data_equation(variable_list(), not_function(s, true_function(s)), false_function(s)));
        result.push_back(data_equation(make_vector(ve, vf, vg), and_function(s, vf, vg)(ve), sort_bool::and_(vf(ve), vg(ve))));
        result.push_back(data_equation(make_vector(vf), and_function(s, vf, vf), vf));
        result.push_back(data_equation(make_vector(vf), and_function(s, vf, false_function(s)), false_function(s)));
        result.push_back(data_equation(make_vector(vf), and_function(s, false_function(s), vf), false_function(s)));
        result.push_back(data_equation(make_vector(vf), and_function(s, vf, true_function(s)), vf));
        result.push_back(data_equation(make_vector(vf), and_function(s, true_function(s), vf), vf));
        result.push_back(data_equation(make_vector(ve, vf, vg), or_function(s, vf, vg)(ve), sort_bool::or_(vf(ve), vg(ve))));
        result.push_back(data_equation(make_vector(vf), or_function(s, vf, vf), vf));
        result.push_back(data_equation(make_vector(vf), or_function(s, vf, false_function(s)), vf));
        result.push_back(data_equation(make_vector(vf), or_function(s, false_function(s), vf), vf));
        result.push_back(data_equation(make_vector(vf), or_function(s, vf, true_function(s)), true_function(s)));
        result.push_back(data_equation(make_vector(vf), or_function(s, true_function(s), vf), true_function(s)));
        return result;
      }

      /// \brief Add sort, constructors, mappings and equations for set_
      /// \param specification a specification
      /// \param element the sort of elements stored by the container
      inline
      void add_set_to_specification(data_specification& specification, sort_expression const& element)
      {
         if (specification.constructors(sort_bool::bool_()).empty())
         {
           sort_bool::add_bool_to_specification(specification);
         }
         if (specification.constructors(sort_fset::fset(element)).empty())
         {
           sort_fset::add_fset_to_specification(specification, element);
         }
         specification.add_system_defined_sort(set_(element));
         specification.add_system_defined_constructors(set_generate_constructors_code(element));
         specification.add_system_defined_mappings(set_generate_functions_code(element));
         specification.add_system_defined_equations(set_generate_equations_code(element));
      }
    } // namespace sort_set_

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SET_H
