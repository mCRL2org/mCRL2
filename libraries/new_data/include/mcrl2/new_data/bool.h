// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/bool_.h
/// \brief The standard sort bool_.
/// \detail This file was generated from the data sort specification
///          mcrl2/new_data/build/bool.spec.

#ifndef MCRL2_NEW_DATA_BOOL__H
#define MCRL2_NEW_DATA_BOOL__H

#include "mcrl2/new_data/basic_sort.h"
#include "mcrl2/new_data/function_sort.h"
#include "mcrl2/new_data/function_symbol.h"
#include "mcrl2/new_data/application.h"
#include "mcrl2/new_data/data_equation.h"
#include "mcrl2/new_data/detail/container_utility.h"
#include "mcrl2/new_data/standard.h"
#include "mcrl2/new_data/data_specification.h"

namespace mcrl2 {

  namespace new_data {

    /// \brief Namespace for system defined sort bool_
    namespace sort_bool_ {

      /// \brief Constructor for sort expression Bool
      /// \return Sort expression Bool
      inline
      basic_sort bool_()
      {
        static basic_sort bool_("Bool");
        return bool_;
      }

      /// \brief Recogniser for sort expression Bool
      /// \param e A sort expression
      /// \return true iff e == bool_()
      inline
      bool is_bool_(const sort_expression& e)
      {
        if (e.is_basic_sort())
        {
          return static_cast<const basic_sort&>(e) == bool_();
        }
        return false;
      }

      /// \brief Constructor for function symbol true
      /// \return Function symbol true_
      inline
      function_symbol true_()
      {
        function_symbol true_("true", bool_());
        return true_;
      }

      /// \brief Recogniser for function true
      /// \param e A data expression
      /// \return true iff e is the function symbol matching true
      inline
      bool is_true__function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "true";
        }
        return false;
      }

      /// \brief Constructor for function symbol false
      /// \return Function symbol false_
      inline
      function_symbol false_()
      {
        function_symbol false_("false", bool_());
        return false_;
      }

      /// \brief Recogniser for function false
      /// \param e A data expression
      /// \return true iff e is the function symbol matching false
      inline
      bool is_false__function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "false";
        }
        return false;
      }

      /// \brief Give all system defined constructors for bool_
      /// \return All system defined constructors for bool_
      inline
      function_symbol_vector bool__generate_constructors_code()
      {
        function_symbol_vector result;
        result.push_back(true_());
        result.push_back(false_());

        return result;
      }
      /// \brief Constructor for function symbol !
      /// \return Function symbol not_
      inline
      function_symbol not_()
      {
        function_symbol not_("!", function_sort(bool_(), bool_()));
        return not_;
      }

      /// \brief Recogniser for function !
      /// \param e A data expression
      /// \return true iff e is the function symbol matching !
      inline
      bool is_not__function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "!";
        }
        return false;
      }

      /// \brief Application of function symbol !
      /// \param arg0 A data expression
      /// \return Application of ! to a number of arguments
      inline
      application not_(const data_expression& arg0)
      {
        return application(not_(),arg0);
      }

      /// \brief Recogniser for application of !
      /// \param e A data expression
      /// \return true iff e is an application of function symbol not_ to a
      ///     number of arguments
      inline
      bool is_not__application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_not__function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol &&
      /// \return Function symbol and_
      inline
      function_symbol and_()
      {
        function_symbol and_("&&", function_sort(bool_(), bool_(), bool_()));
        return and_;
      }

      /// \brief Recogniser for function &&
      /// \param e A data expression
      /// \return true iff e is the function symbol matching &&
      inline
      bool is_and__function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "&&";
        }
        return false;
      }

      /// \brief Application of function symbol &&
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of && to a number of arguments
      inline
      application and_(const data_expression& arg0, const data_expression& arg1)
      {
        return application(and_(),arg0, arg1);
      }

      /// \brief Recogniser for application of &&
      /// \param e A data expression
      /// \return true iff e is an application of function symbol and_ to a
      ///     number of arguments
      inline
      bool is_and__application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_and__function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol ||
      /// \return Function symbol or_
      inline
      function_symbol or_()
      {
        function_symbol or_("||", function_sort(bool_(), bool_(), bool_()));
        return or_;
      }

      /// \brief Recogniser for function ||
      /// \param e A data expression
      /// \return true iff e is the function symbol matching ||
      inline
      bool is_or__function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "||";
        }
        return false;
      }

      /// \brief Application of function symbol ||
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of || to a number of arguments
      inline
      application or_(const data_expression& arg0, const data_expression& arg1)
      {
        return application(or_(),arg0, arg1);
      }

      /// \brief Recogniser for application of ||
      /// \param e A data expression
      /// \return true iff e is an application of function symbol or_ to a
      ///     number of arguments
      inline
      bool is_or__application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_or__function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol =>
      /// \return Function symbol implies
      inline
      function_symbol implies()
      {
        function_symbol implies("=>", function_sort(bool_(), bool_(), bool_()));
        return implies;
      }

      /// \brief Recogniser for function =>
      /// \param e A data expression
      /// \return true iff e is the function symbol matching =>
      inline
      bool is_implies_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "=>";
        }
        return false;
      }

      /// \brief Application of function symbol =>
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of => to a number of arguments
      inline
      application implies(const data_expression& arg0, const data_expression& arg1)
      {
        return application(implies(),arg0, arg1);
      }

      /// \brief Recogniser for application of =>
      /// \param e A data expression
      /// \return true iff e is an application of function symbol implies to a
      ///     number of arguments
      inline
      bool is_implies_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_implies_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Give all system defined mappings for bool_
      /// \return All system defined mappings for bool_
      inline
      function_symbol_vector bool__generate_functions_code()
      {
        function_symbol_vector result;
        result.push_back(not_());
        result.push_back(and_());
        result.push_back(or_());
        result.push_back(implies());
        return result;
      }
      ///\brief Function for projecting out argument
      ///        left from an application
      /// \param e A data expression
      /// \pre left is defined for e
      /// \return The argument of e that corresponds to left
      inline
      data_expression left(const data_expression& e)
      {
        if (is_and__application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_or__application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_implies_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
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
        if (is_and__application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }
        if (is_or__application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }
        if (is_implies_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
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
        if (is_not__application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      /// \brief Give all system defined equations for bool_
      /// \return All system defined equations for sort bool_
      inline
      data_equation_vector bool__generate_equations_code()
      {
        variable vb("b",bool_());

        data_equation_vector result;
        result.push_back(data_equation(variable_list(), not_(true_()), false_()));
        result.push_back(data_equation(variable_list(), not_(false_()), true_()));
        result.push_back(data_equation(make_vector(vb), not_(not_(vb)), vb));
        result.push_back(data_equation(make_vector(vb), and_(vb, true_()), vb));
        result.push_back(data_equation(make_vector(vb), and_(vb, false_()), false_()));
        result.push_back(data_equation(make_vector(vb), and_(true_(), vb), vb));
        result.push_back(data_equation(make_vector(vb), and_(false_(), vb), false_()));
        result.push_back(data_equation(make_vector(vb), or_(vb, true_()), true_()));
        result.push_back(data_equation(make_vector(vb), or_(vb, false_()), vb));
        result.push_back(data_equation(make_vector(vb), or_(true_(), vb), true_()));
        result.push_back(data_equation(make_vector(vb), or_(false_(), vb), vb));
        result.push_back(data_equation(make_vector(vb), implies(vb, true_()), true_()));
        result.push_back(data_equation(make_vector(vb), implies(vb, false_()), not_(vb)));
        result.push_back(data_equation(make_vector(vb), implies(true_(), vb), vb));
        result.push_back(data_equation(make_vector(vb), implies(false_(), vb), true_()));
        result.push_back(data_equation(make_vector(vb), equal_to(true_(), vb), vb));
        result.push_back(data_equation(make_vector(vb), equal_to(false_(), vb), not_(vb)));
        result.push_back(data_equation(make_vector(vb), equal_to(vb, true_()), vb));
        result.push_back(data_equation(make_vector(vb), equal_to(vb, false_()), not_(vb)));
        return result;
      }

      /// \brief Add sort, constructors, mappings and equations for bool_
      /// \param specification a specification
      inline
      void add_bool__to_specification(data_specification& specification)
      {
         specification.add_system_defined_sort(bool_());
         specification.add_system_defined_constructors(boost::make_iterator_range(bool__generate_constructors_code()));
         specification.add_system_defined_mappings(boost::make_iterator_range(bool__generate_functions_code()));
         specification.add_system_defined_equations(boost::make_iterator_range(bool__generate_equations_code()));

      }
    } // namespace sort_bool_

  } // namespace new_data

} // namespace mcrl2

#endif // MCRL2_NEW_DATA_BOOL__H
