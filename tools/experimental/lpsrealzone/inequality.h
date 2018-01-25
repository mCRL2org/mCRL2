// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/inequality.h
/// \brief The standard sort inequality.
///
/// This file was generated from the data sort specification
/// mcrl2/data/build/inequality.spec.

#ifndef MCRL2_DATA_INEQUALITY_H
#define MCRL2_DATA_INEQUALITY_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/bool.h"

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort inequality
    namespace sort_inequality {

      inline
      core::identifier_string const& inequality_name()
      {
        static core::identifier_string inequality_name = core::identifier_string("Inequality");
        return inequality_name;
      }

      /// \brief Constructor for sort expression Inequality
      /// \return Sort expression Inequality
      inline
      basic_sort const& inequality()
      {
        static basic_sort inequality = basic_sort(inequality_name());
        return inequality;
      }

      /// \brief Recogniser for sort expression Inequality
      /// \param e A sort expression
      /// \return true iff e == inequality()
      inline
      bool is_inequality(const sort_expression& e)
      {
        if (is_basic_sort(e))
        {
          return basic_sort(e) == inequality();
        }
        return false;
      }


      /// \brief Generate identifier less
      /// \return Identifier less
      inline
      core::identifier_string const& lt_name()
      {
        static core::identifier_string lt_name = core::identifier_string("less");
        return lt_name;
      }

      /// \brief Constructor for function symbol less
      
      /// \return Function symbol lt
      inline
      function_symbol const& lt()
      {
        static function_symbol lt(lt_name(), inequality());
        return lt;
      }

      /// \brief Recogniser for function less
      /// \param e A data expression
      /// \return true iff e is the function symbol matching less
      inline
      bool is_lt_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == lt();
        }
        return false;
      }

      /// \brief Generate identifier less_equal
      /// \return Identifier less_equal
      inline
      core::identifier_string const& le_name()
      {
        static core::identifier_string le_name = core::identifier_string("less_equal");
        return le_name;
      }

      /// \brief Constructor for function symbol less_equal
      
      /// \return Function symbol le
      inline
      function_symbol const& le()
      {
        static function_symbol le(le_name(), inequality());
        return le;
      }

      /// \brief Recogniser for function less_equal
      /// \param e A data expression
      /// \return true iff e is the function symbol matching less_equal
      inline
      bool is_le_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == le();
        }
        return false;
      }
      /// \brief Give all system defined constructors for inequality
      /// \return All system defined constructors for inequality
      inline
      function_symbol_vector inequality_generate_constructors_code()
      {
        function_symbol_vector result;
        result.push_back(sort_inequality::lt());
        result.push_back(sort_inequality::le());

        return result;
      }

      /// \brief Generate identifier id
      /// \return Identifier id
      inline
      core::identifier_string const& id_name()
      {
        static core::identifier_string id_name = core::identifier_string("id");
        return id_name;
      }

      /// \brief Constructor for function symbol id
      
      /// \return Function symbol id
      inline
      function_symbol const& id()
      {
        static function_symbol id(id_name(), inequality());
        return id;
      }

      /// \brief Recogniser for function id
      /// \param e A data expression
      /// \return true iff e is the function symbol matching id
      inline
      bool is_id_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == id();
        }
        return false;
      }
      /// \brief Give all system defined mappings for inequality
      /// \return All system defined mappings for inequality
      inline
      function_symbol_vector inequality_generate_functions_code()
      {
        function_symbol_vector result;
        result.push_back(sort_inequality::id());
        return result;
      }


      /// \brief Give all system defined equations for inequality
      /// \return All system defined equations for sort inequality
      inline
      data_equation_vector inequality_generate_equations_code()
      {
        variable vi("i",inequality());

        data_equation_vector result;
        result.push_back(data_equation(variable_list({vi}), equal_to(vi, vi), sort_bool::true_()));
        result.push_back(data_equation(variable_list(), equal_to(lt(), le()), sort_bool::false_()));
        result.push_back(data_equation(variable_list(), equal_to(le(), lt()), sort_bool::false_()));
        return result;
      }

    } // namespace sort_inequality

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_INEQUALITY_H
