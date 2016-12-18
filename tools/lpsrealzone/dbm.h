// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/dbm.h
/// \brief The standard sort dbm.
///
/// This file was generated from the data sort specification
/// mcrl2/data/build/dbm.spec.

#ifndef MCRL2_DATA_DBM_H
#define MCRL2_DATA_DBM_H

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
#include "mcrl2/data/real.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/list.h"
#include "inequality.h"
#include "bound.h"

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort dbm
    namespace sort_dbm {

      inline
      core::identifier_string const& dbm_name()
      {
        static core::identifier_string dbm_name = core::identifier_string("DBM");
        return dbm_name;
      }

      /// \brief Constructor for sort expression DBM
      /// \return Sort expression DBM
      inline
      basic_sort const& dbm()
      {
        static basic_sort dbm = basic_sort(dbm_name());
        return dbm;
      }

      /// \brief Recogniser for sort expression DBM
      /// \param e A sort expression
      /// \return true iff e == dbm()
      inline
      bool is_dbm(const sort_expression& e)
      {
        if (is_basic_sort(e))
        {
          return basic_sort(e) == dbm();
        }
        return false;
      }

      /// \brief Give all system defined constructors for dbm
      /// \return All system defined constructors for dbm
      inline
      function_symbol_vector dbm_generate_constructors_code()
      {
        function_symbol_vector result;
        return result;
      }

      /// \brief Generate identifier get
      /// \return Identifier get
      inline
      core::identifier_string const& get_name()
      {
        static core::identifier_string get_name = core::identifier_string("get");
        return get_name;
      }

      /// \brief Constructor for function symbol get
      
      /// \return Function symbol get
      inline
      function_symbol const& get()
      {
        static function_symbol get(get_name(), function_sort({ dbm(), sort_nat::nat(), sort_nat::nat() }, sort_bound::bound()));
        return get;
      }

      /// \brief Recogniser for function get
      /// \param e A data expression
      /// \return true iff e is the function symbol matching get
      inline
      bool is_get_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == get();
        }
        return false;
      }

      /// \brief Application of function symbol get
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of get to a number of arguments
      inline
      application get(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(sort_dbm::get(),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of get
      /// \param e A data expression
      /// \return true iff e is an application of function symbol get to a
      ///     number of arguments
      inline
      bool is_get_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_get_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier set
      /// \return Identifier set
      inline
      core::identifier_string const& set_name()
      {
        static core::identifier_string set_name = core::identifier_string("set");
        return set_name;
      }

      /// \brief Constructor for function symbol set
      
      /// \return Function symbol set
      inline
      function_symbol const& set()
      {
        static function_symbol set(set_name(), function_sort({ dbm(), sort_nat::nat(), sort_nat::nat(), sort_bound::bound() }, dbm()));
        return set;
      }

      /// \brief Recogniser for function set
      /// \param e A data expression
      /// \return true iff e is the function symbol matching set
      inline
      bool is_set_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == set();
        }
        return false;
      }

      /// \brief Application of function symbol set
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of set to a number of arguments
      inline
      application set(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(sort_dbm::set(),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of set
      /// \param e A data expression
      /// \return true iff e is an application of function symbol set to a
      ///     number of arguments
      inline
      bool is_set_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_set_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier set1
      /// \return Identifier set1
      inline
      core::identifier_string const& set1_name()
      {
        static core::identifier_string set1_name = core::identifier_string("set1");
        return set1_name;
      }

      /// \brief Constructor for function symbol set1
      
      /// \return Function symbol set1
      inline
      function_symbol const& set1()
      {
        static function_symbol set1(set1_name(), function_sort({ sort_list::list(sort_bound::bound()), sort_nat::nat(), sort_bound::bound() }, sort_list::list(sort_bound::bound())));
        return set1;
      }

      /// \brief Recogniser for function set1
      /// \param e A data expression
      /// \return true iff e is the function symbol matching set1
      inline
      bool is_set1_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == set1();
        }
        return false;
      }

      /// \brief Application of function symbol set1
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of set1 to a number of arguments
      inline
      application set1(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(sort_dbm::set1(),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of set1
      /// \param e A data expression
      /// \return true iff e is an application of function symbol set1 to a
      ///     number of arguments
      inline
      bool is_set1_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_set1_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier close
      /// \return Identifier close
      inline
      core::identifier_string const& close_name()
      {
        static core::identifier_string close_name = core::identifier_string("close");
        return close_name;
      }

      /// \brief Constructor for function symbol close
      
      /// \return Function symbol close
      inline
      function_symbol const& close()
      {
        static function_symbol close(close_name(), function_sort({ dbm() }, dbm()));
        return close;
      }

      /// \brief Recogniser for function close
      /// \param e A data expression
      /// \return true iff e is the function symbol matching close
      inline
      bool is_close_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == close();
        }
        return false;
      }

      /// \brief Application of function symbol close
      
      /// \param arg0 A data expression
      /// \return Application of close to a number of arguments
      inline
      application close(const data_expression& arg0)
      {
        return application(sort_dbm::close(),arg0);
      }

      /// \brief Recogniser for application of close
      /// \param e A data expression
      /// \return true iff e is an application of function symbol close to a
      ///     number of arguments
      inline
      bool is_close_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_close_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier close1
      /// \return Identifier close1
      inline
      core::identifier_string const& close1_name()
      {
        static core::identifier_string close1_name = core::identifier_string("close1");
        return close1_name;
      }

      /// \brief Constructor for function symbol close1
      
      /// \return Function symbol close1
      inline
      function_symbol const& close1()
      {
        static function_symbol close1(close1_name(), function_sort({ dbm(), sort_nat::nat() }, dbm()));
        return close1;
      }

      /// \brief Recogniser for function close1
      /// \param e A data expression
      /// \return true iff e is the function symbol matching close1
      inline
      bool is_close1_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == close1();
        }
        return false;
      }

      /// \brief Application of function symbol close1
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of close1 to a number of arguments
      inline
      application close1(const data_expression& arg0, const data_expression& arg1)
      {
        return application(sort_dbm::close1(),arg0, arg1);
      }

      /// \brief Recogniser for application of close1
      /// \param e A data expression
      /// \return true iff e is an application of function symbol close1 to a
      ///     number of arguments
      inline
      bool is_close1_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_close1_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier close2
      /// \return Identifier close2
      inline
      core::identifier_string const& close2_name()
      {
        static core::identifier_string close2_name = core::identifier_string("close2");
        return close2_name;
      }

      /// \brief Constructor for function symbol close2
      
      /// \return Function symbol close2
      inline
      function_symbol const& close2()
      {
        static function_symbol close2(close2_name(), function_sort({ dbm(), sort_nat::nat(), sort_nat::nat() }, dbm()));
        return close2;
      }

      /// \brief Recogniser for function close2
      /// \param e A data expression
      /// \return true iff e is the function symbol matching close2
      inline
      bool is_close2_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == close2();
        }
        return false;
      }

      /// \brief Application of function symbol close2
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of close2 to a number of arguments
      inline
      application close2(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(sort_dbm::close2(),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of close2
      /// \param e A data expression
      /// \return true iff e is an application of function symbol close2 to a
      ///     number of arguments
      inline
      bool is_close2_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_close2_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier close3
      /// \return Identifier close3
      inline
      core::identifier_string const& close3_name()
      {
        static core::identifier_string close3_name = core::identifier_string("close3");
        return close3_name;
      }

      /// \brief Constructor for function symbol close3
      
      /// \return Function symbol close3
      inline
      function_symbol const& close3()
      {
        static function_symbol close3(close3_name(), function_sort({ dbm(), sort_nat::nat(), sort_nat::nat(), sort_nat::nat() }, dbm()));
        return close3;
      }

      /// \brief Recogniser for function close3
      /// \param e A data expression
      /// \return true iff e is the function symbol matching close3
      inline
      bool is_close3_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == close3();
        }
        return false;
      }

      /// \brief Application of function symbol close3
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of close3 to a number of arguments
      inline
      application close3(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(sort_dbm::close3(),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of close3
      /// \param e A data expression
      /// \return true iff e is an application of function symbol close3 to a
      ///     number of arguments
      inline
      bool is_close3_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_close3_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier consistent
      /// \return Identifier consistent
      inline
      core::identifier_string const& consistent_name()
      {
        static core::identifier_string consistent_name = core::identifier_string("consistent");
        return consistent_name;
      }

      /// \brief Constructor for function symbol consistent
      
      /// \return Function symbol consistent
      inline
      function_symbol const& consistent()
      {
        static function_symbol consistent(consistent_name(), function_sort({ dbm() }, sort_bool::bool_()));
        return consistent;
      }

      /// \brief Recogniser for function consistent
      /// \param e A data expression
      /// \return true iff e is the function symbol matching consistent
      inline
      bool is_consistent_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == consistent();
        }
        return false;
      }

      /// \brief Application of function symbol consistent
      
      /// \param arg0 A data expression
      /// \return Application of consistent to a number of arguments
      inline
      application consistent(const data_expression& arg0)
      {
        return application(sort_dbm::consistent(),arg0);
      }

      /// \brief Recogniser for application of consistent
      /// \param e A data expression
      /// \return true iff e is an application of function symbol consistent to a
      ///     number of arguments
      inline
      bool is_consistent_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_consistent_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier and_d
      /// \return Identifier and_d
      inline
      core::identifier_string const& and_d_name()
      {
        static core::identifier_string and_d_name = core::identifier_string("and_d");
        return and_d_name;
      }

      /// \brief Constructor for function symbol and_d
      
      /// \return Function symbol and_d
      inline
      function_symbol const& and_d()
      {
        static function_symbol and_d(and_d_name(), function_sort({ dbm(), sort_nat::nat(), sort_nat::nat(), sort_bound::bound() }, dbm()));
        return and_d;
      }

      /// \brief Recogniser for function and_d
      /// \param e A data expression
      /// \return true iff e is the function symbol matching and_d
      inline
      bool is_and_d_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == and_d();
        }
        return false;
      }

      /// \brief Application of function symbol and_d
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of and_d to a number of arguments
      inline
      application and_d(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(sort_dbm::and_d(),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of and_d
      /// \param e A data expression
      /// \return true iff e is an application of function symbol and_d to a
      ///     number of arguments
      inline
      bool is_and_d_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_and_d_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier and_d1
      /// \return Identifier and_d1
      inline
      core::identifier_string const& and_d1_name()
      {
        static core::identifier_string and_d1_name = core::identifier_string("and_d1");
        return and_d1_name;
      }

      /// \brief Constructor for function symbol and_d1
      
      /// \return Function symbol and_d1
      inline
      function_symbol const& and_d1()
      {
        static function_symbol and_d1(and_d1_name(), function_sort({ dbm(), sort_nat::nat(), sort_nat::nat(), sort_nat::nat() }, dbm()));
        return and_d1;
      }

      /// \brief Recogniser for function and_d1
      /// \param e A data expression
      /// \return true iff e is the function symbol matching and_d1
      inline
      bool is_and_d1_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == and_d1();
        }
        return false;
      }

      /// \brief Application of function symbol and_d1
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of and_d1 to a number of arguments
      inline
      application and_d1(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(sort_dbm::and_d1(),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of and_d1
      /// \param e A data expression
      /// \return true iff e is an application of function symbol and_d1 to a
      ///     number of arguments
      inline
      bool is_and_d1_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_and_d1_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier and_d2
      /// \return Identifier and_d2
      inline
      core::identifier_string const& and_d2_name()
      {
        static core::identifier_string and_d2_name = core::identifier_string("and_d2");
        return and_d2_name;
      }

      /// \brief Constructor for function symbol and_d2
      
      /// \return Function symbol and_d2
      inline
      function_symbol const& and_d2()
      {
        static function_symbol and_d2(and_d2_name(), function_sort({ dbm(), sort_nat::nat(), sort_nat::nat(), sort_nat::nat(), sort_nat::nat() }, dbm()));
        return and_d2;
      }

      /// \brief Recogniser for function and_d2
      /// \param e A data expression
      /// \return true iff e is the function symbol matching and_d2
      inline
      bool is_and_d2_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == and_d2();
        }
        return false;
      }

      /// \brief Application of function symbol and_d2
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \param arg4 A data expression
      /// \return Application of and_d2 to a number of arguments
      inline
      application and_d2(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3, const data_expression& arg4)
      {
        return application(sort_dbm::and_d2(),arg0, arg1, arg2, arg3, arg4);
      }

      /// \brief Recogniser for application of and_d2
      /// \param e A data expression
      /// \return true iff e is an application of function symbol and_d2 to a
      ///     number of arguments
      inline
      bool is_and_d2_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_and_d2_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier and_not
      /// \return Identifier and_not
      inline
      core::identifier_string const& and_not_name()
      {
        static core::identifier_string and_not_name = core::identifier_string("and_not");
        return and_not_name;
      }

      /// \brief Constructor for function symbol and_not
      
      /// \return Function symbol and_not
      inline
      function_symbol const& and_not()
      {
        static function_symbol and_not(and_not_name(), function_sort({ dbm(), sort_nat::nat(), sort_nat::nat(), sort_bound::bound(), sort_bool::bool_() }, dbm()));
        return and_not;
      }

      /// \brief Recogniser for function and_not
      /// \param e A data expression
      /// \return true iff e is the function symbol matching and_not
      inline
      bool is_and_not_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == and_not();
        }
        return false;
      }

      /// \brief Application of function symbol and_not
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \param arg4 A data expression
      /// \return Application of and_not to a number of arguments
      inline
      application and_not(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3, const data_expression& arg4)
      {
        return application(sort_dbm::and_not(),arg0, arg1, arg2, arg3, arg4);
      }

      /// \brief Recogniser for application of and_not
      /// \param e A data expression
      /// \return true iff e is an application of function symbol and_not to a
      ///     number of arguments
      inline
      bool is_and_not_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_and_not_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier free
      /// \return Identifier free
      inline
      core::identifier_string const& free_name()
      {
        static core::identifier_string free_name = core::identifier_string("free");
        return free_name;
      }

      /// \brief Constructor for function symbol free
      
      /// \return Function symbol free
      inline
      function_symbol const& free()
      {
        static function_symbol free(free_name(), function_sort({ dbm(), sort_nat::nat() }, dbm()));
        return free;
      }

      /// \brief Recogniser for function free
      /// \param e A data expression
      /// \return true iff e is the function symbol matching free
      inline
      bool is_free_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == free();
        }
        return false;
      }

      /// \brief Application of function symbol free
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of free to a number of arguments
      inline
      application free(const data_expression& arg0, const data_expression& arg1)
      {
        return application(sort_dbm::free(),arg0, arg1);
      }

      /// \brief Recogniser for application of free
      /// \param e A data expression
      /// \return true iff e is an application of function symbol free to a
      ///     number of arguments
      inline
      bool is_free_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_free_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier free1
      /// \return Identifier free1
      inline
      core::identifier_string const& free1_name()
      {
        static core::identifier_string free1_name = core::identifier_string("free1");
        return free1_name;
      }

      /// \brief Constructor for function symbol free1
      
      /// \return Function symbol free1
      inline
      function_symbol const& free1()
      {
        static function_symbol free1(free1_name(), function_sort({ dbm(), sort_nat::nat(), sort_nat::nat() }, dbm()));
        return free1;
      }

      /// \brief Recogniser for function free1
      /// \param e A data expression
      /// \return true iff e is the function symbol matching free1
      inline
      bool is_free1_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == free1();
        }
        return false;
      }

      /// \brief Application of function symbol free1
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of free1 to a number of arguments
      inline
      application free1(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(sort_dbm::free1(),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of free1
      /// \param e A data expression
      /// \return true iff e is an application of function symbol free1 to a
      ///     number of arguments
      inline
      bool is_free1_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_free1_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier reset
      /// \return Identifier reset
      inline
      core::identifier_string const& reset_name()
      {
        static core::identifier_string reset_name = core::identifier_string("reset");
        return reset_name;
      }

      /// \brief Constructor for function symbol reset
      
      /// \return Function symbol reset
      inline
      function_symbol const& reset()
      {
        static function_symbol reset(reset_name(), function_sort({ dbm(), sort_nat::nat(), sort_real::real_() }, dbm()));
        return reset;
      }

      /// \brief Recogniser for function reset
      /// \param e A data expression
      /// \return true iff e is the function symbol matching reset
      inline
      bool is_reset_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == reset();
        }
        return false;
      }

      /// \brief Application of function symbol reset
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of reset to a number of arguments
      inline
      application reset(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(sort_dbm::reset(),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of reset
      /// \param e A data expression
      /// \return true iff e is an application of function symbol reset to a
      ///     number of arguments
      inline
      bool is_reset_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_reset_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier reset1
      /// \return Identifier reset1
      inline
      core::identifier_string const& reset1_name()
      {
        static core::identifier_string reset1_name = core::identifier_string("reset1");
        return reset1_name;
      }

      /// \brief Constructor for function symbol reset1
      
      /// \return Function symbol reset1
      inline
      function_symbol const& reset1()
      {
        static function_symbol reset1(reset1_name(), function_sort({ dbm(), sort_nat::nat(), sort_real::real_(), sort_nat::nat() }, dbm()));
        return reset1;
      }

      /// \brief Recogniser for function reset1
      /// \param e A data expression
      /// \return true iff e is the function symbol matching reset1
      inline
      bool is_reset1_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == reset1();
        }
        return false;
      }

      /// \brief Application of function symbol reset1
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of reset1 to a number of arguments
      inline
      application reset1(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(sort_dbm::reset1(),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of reset1
      /// \param e A data expression
      /// \return true iff e is an application of function symbol reset1 to a
      ///     number of arguments
      inline
      bool is_reset1_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_reset1_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier shift
      /// \return Identifier shift
      inline
      core::identifier_string const& shift_name()
      {
        static core::identifier_string shift_name = core::identifier_string("shift");
        return shift_name;
      }

      /// \brief Constructor for function symbol shift
      
      /// \return Function symbol shift
      inline
      function_symbol const& shift()
      {
        static function_symbol shift(shift_name(), function_sort({ dbm(), sort_nat::nat(), sort_real::real_() }, dbm()));
        return shift;
      }

      /// \brief Recogniser for function shift
      /// \param e A data expression
      /// \return true iff e is the function symbol matching shift
      inline
      bool is_shift_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == shift();
        }
        return false;
      }

      /// \brief Application of function symbol shift
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of shift to a number of arguments
      inline
      application shift(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(sort_dbm::shift(),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of shift
      /// \param e A data expression
      /// \return true iff e is an application of function symbol shift to a
      ///     number of arguments
      inline
      bool is_shift_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_shift_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier shift_range
      /// \return Identifier shift_range
      inline
      core::identifier_string const& shift_range_name()
      {
        static core::identifier_string shift_range_name = core::identifier_string("shift_range");
        return shift_range_name;
      }

      /// \brief Constructor for function symbol shift_range
      
      /// \return Function symbol shift_range
      inline
      function_symbol const& shift_range()
      {
        static function_symbol shift_range(shift_range_name(), function_sort({ dbm(), sort_nat::nat(), sort_real::real_(), sort_real::real_() }, dbm()));
        return shift_range;
      }

      /// \brief Recogniser for function shift_range
      /// \param e A data expression
      /// \return true iff e is the function symbol matching shift_range
      inline
      bool is_shift_range_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == shift_range();
        }
        return false;
      }

      /// \brief Application of function symbol shift_range
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of shift_range to a number of arguments
      inline
      application shift_range(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(sort_dbm::shift_range(),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of shift_range
      /// \param e A data expression
      /// \return true iff e is an application of function symbol shift_range to a
      ///     number of arguments
      inline
      bool is_shift_range_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_shift_range_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier shift1
      /// \return Identifier shift1
      inline
      core::identifier_string const& shift1_name()
      {
        static core::identifier_string shift1_name = core::identifier_string("shift1");
        return shift1_name;
      }

      /// \brief Constructor for function symbol shift1
      
      /// \return Function symbol shift1
      inline
      function_symbol const& shift1()
      {
        static function_symbol shift1(shift1_name(), function_sort({ dbm(), sort_nat::nat(), sort_real::real_(), sort_real::real_(), sort_nat::nat() }, dbm()));
        return shift1;
      }

      /// \brief Recogniser for function shift1
      /// \param e A data expression
      /// \return true iff e is the function symbol matching shift1
      inline
      bool is_shift1_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == shift1();
        }
        return false;
      }

      /// \brief Application of function symbol shift1
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \param arg4 A data expression
      /// \return Application of shift1 to a number of arguments
      inline
      application shift1(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3, const data_expression& arg4)
      {
        return application(sort_dbm::shift1(),arg0, arg1, arg2, arg3, arg4);
      }

      /// \brief Recogniser for application of shift1
      /// \param e A data expression
      /// \return true iff e is an application of function symbol shift1 to a
      ///     number of arguments
      inline
      bool is_shift1_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_shift1_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier copy
      /// \return Identifier copy
      inline
      core::identifier_string const& copy_name()
      {
        static core::identifier_string copy_name = core::identifier_string("copy");
        return copy_name;
      }

      /// \brief Constructor for function symbol copy
      
      /// \return Function symbol copy
      inline
      function_symbol const& copy()
      {
        static function_symbol copy(copy_name(), function_sort({ dbm(), sort_nat::nat(), sort_nat::nat() }, dbm()));
        return copy;
      }

      /// \brief Recogniser for function copy
      /// \param e A data expression
      /// \return true iff e is the function symbol matching copy
      inline
      bool is_copy_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == copy();
        }
        return false;
      }

      /// \brief Application of function symbol copy
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of copy to a number of arguments
      inline
      application copy(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(sort_dbm::copy(),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of copy
      /// \param e A data expression
      /// \return true iff e is an application of function symbol copy to a
      ///     number of arguments
      inline
      bool is_copy_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_copy_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier copy1
      /// \return Identifier copy1
      inline
      core::identifier_string const& copy1_name()
      {
        static core::identifier_string copy1_name = core::identifier_string("copy1");
        return copy1_name;
      }

      /// \brief Constructor for function symbol copy1
      
      /// \return Function symbol copy1
      inline
      function_symbol const& copy1()
      {
        static function_symbol copy1(copy1_name(), function_sort({ dbm(), sort_nat::nat(), sort_nat::nat(), sort_nat::nat() }, dbm()));
        return copy1;
      }

      /// \brief Recogniser for function copy1
      /// \param e A data expression
      /// \return true iff e is the function symbol matching copy1
      inline
      bool is_copy1_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == copy1();
        }
        return false;
      }

      /// \brief Application of function symbol copy1
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of copy1 to a number of arguments
      inline
      application copy1(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(sort_dbm::copy1(),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of copy1
      /// \param e A data expression
      /// \return true iff e is an application of function symbol copy1 to a
      ///     number of arguments
      inline
      bool is_copy1_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_copy1_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier split
      /// \return Identifier split
      inline
      core::identifier_string const& split_name()
      {
        static core::identifier_string split_name = core::identifier_string("split");
        return split_name;
      }

      /// \brief Constructor for function symbol split
      
      /// \return Function symbol split
      inline
      function_symbol const& split()
      {
        static function_symbol split(split_name(), function_sort({ dbm(), sort_nat::nat(), sort_nat::nat(), sort_bound::bound(), sort_bool::bool_() }, dbm()));
        return split;
      }

      /// \brief Recogniser for function split
      /// \param e A data expression
      /// \return true iff e is the function symbol matching split
      inline
      bool is_split_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == split();
        }
        return false;
      }

      /// \brief Application of function symbol split
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \param arg4 A data expression
      /// \return Application of split to a number of arguments
      inline
      application split(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3, const data_expression& arg4)
      {
        return application(sort_dbm::split(),arg0, arg1, arg2, arg3, arg4);
      }

      /// \brief Recogniser for application of split
      /// \param e A data expression
      /// \return true iff e is an application of function symbol split to a
      ///     number of arguments
      inline
      bool is_split_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_split_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier inconsistent
      /// \return Identifier inconsistent
      inline
      core::identifier_string const& inconsistent_name()
      {
        static core::identifier_string inconsistent_name = core::identifier_string("inconsistent");
        return inconsistent_name;
      }

      /// \brief Constructor for function symbol inconsistent
      
      /// \return Function symbol inconsistent
      inline
      function_symbol const& inconsistent()
      {
        static function_symbol inconsistent(inconsistent_name(), function_sort({ dbm() }, sort_bool::bool_()));
        return inconsistent;
      }

      /// \brief Recogniser for function inconsistent
      /// \param e A data expression
      /// \return true iff e is the function symbol matching inconsistent
      inline
      bool is_inconsistent_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == inconsistent();
        }
        return false;
      }

      /// \brief Application of function symbol inconsistent
      
      /// \param arg0 A data expression
      /// \return Application of inconsistent to a number of arguments
      inline
      application inconsistent(const data_expression& arg0)
      {
        return application(sort_dbm::inconsistent(),arg0);
      }

      /// \brief Recogniser for application of inconsistent
      /// \param e A data expression
      /// \return true iff e is an application of function symbol inconsistent to a
      ///     number of arguments
      inline
      bool is_inconsistent_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_inconsistent_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier N
      /// \return Identifier N
      inline
      core::identifier_string const& N_name()
      {
        static core::identifier_string N_name = core::identifier_string("N");
        return N_name;
      }

      /// \brief Constructor for function symbol N
      
      /// \return Function symbol N
      inline
      function_symbol const& N()
      {
        static function_symbol N(N_name(), sort_nat::nat());
        return N;
      }

      /// \brief Recogniser for function N
      /// \param e A data expression
      /// \return true iff e is the function symbol matching N
      inline
      bool is_N_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == N();
        }
        return false;
      }

      /// \brief Generate identifier dbm_inconsistent
      /// \return Identifier dbm_inconsistent
      inline
      core::identifier_string const& dbm_inconsistent_name()
      {
        static core::identifier_string dbm_inconsistent_name = core::identifier_string("dbm_inconsistent");
        return dbm_inconsistent_name;
      }

      /// \brief Constructor for function symbol dbm_inconsistent
      
      /// \return Function symbol dbm_inconsistent
      inline
      function_symbol const& dbm_inconsistent()
      {
        static function_symbol dbm_inconsistent(dbm_inconsistent_name(), dbm());
        return dbm_inconsistent;
      }

      /// \brief Recogniser for function dbm_inconsistent
      /// \param e A data expression
      /// \return true iff e is the function symbol matching dbm_inconsistent
      inline
      bool is_dbm_inconsistent_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == dbm_inconsistent();
        }
        return false;
      }

      /// \brief Generate identifier dbm
      /// \return Identifier dbm
      inline
      core::identifier_string const& dbm_empty_name()
      {
        static core::identifier_string dbm_empty_name = core::identifier_string("dbm");
        return dbm_empty_name;
      }

      /// \brief Constructor for function symbol dbm
      
      /// \return Function symbol dbm_empty
      inline
      function_symbol const& dbm_empty()
      {
        static function_symbol dbm_empty(dbm_empty_name(), dbm());
        return dbm_empty;
      }

      /// \brief Recogniser for function dbm
      /// \param e A data expression
      /// \return true iff e is the function symbol matching dbm
      inline
      bool is_dbm_empty_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == dbm_empty();
        }
        return false;
      }

      /// \brief Generate identifier dbm_zero
      /// \return Identifier dbm_zero
      inline
      core::identifier_string const& dbm_zero_name()
      {
        static core::identifier_string dbm_zero_name = core::identifier_string("dbm_zero");
        return dbm_zero_name;
      }

      /// \brief Constructor for function symbol dbm_zero
      
      /// \return Function symbol dbm_zero
      inline
      function_symbol const& dbm_zero()
      {
        static function_symbol dbm_zero(dbm_zero_name(), dbm());
        return dbm_zero;
      }

      /// \brief Recogniser for function dbm_zero
      /// \param e A data expression
      /// \return true iff e is the function symbol matching dbm_zero
      inline
      bool is_dbm_zero_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == dbm_zero();
        }
        return false;
      }

      /// \brief Generate identifier dbm_zero1
      /// \return Identifier dbm_zero1
      inline
      core::identifier_string const& dbm_zero1_name()
      {
        static core::identifier_string dbm_zero1_name = core::identifier_string("dbm_zero1");
        return dbm_zero1_name;
      }

      /// \brief Constructor for function symbol dbm_zero1
      
      /// \return Function symbol dbm_zero1
      inline
      function_symbol const& dbm_zero1()
      {
        static function_symbol dbm_zero1(dbm_zero1_name(), function_sort({ sort_nat::nat() }, dbm()));
        return dbm_zero1;
      }

      /// \brief Recogniser for function dbm_zero1
      /// \param e A data expression
      /// \return true iff e is the function symbol matching dbm_zero1
      inline
      bool is_dbm_zero1_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == dbm_zero1();
        }
        return false;
      }

      /// \brief Application of function symbol dbm_zero1
      
      /// \param arg0 A data expression
      /// \return Application of dbm_zero1 to a number of arguments
      inline
      application dbm_zero1(const data_expression& arg0)
      {
        return application(sort_dbm::dbm_zero1(),arg0);
      }

      /// \brief Recogniser for application of dbm_zero1
      /// \param e A data expression
      /// \return true iff e is an application of function symbol dbm_zero1 to a
      ///     number of arguments
      inline
      bool is_dbm_zero1_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_dbm_zero1_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier dbm_size
      /// \return Identifier dbm_size
      inline
      core::identifier_string const& dbm_size_name()
      {
        static core::identifier_string dbm_size_name = core::identifier_string("dbm_size");
        return dbm_size_name;
      }

      /// \brief Constructor for function symbol dbm_size
      
      /// \return Function symbol dbm_size
      inline
      function_symbol const& dbm_size()
      {
        static function_symbol dbm_size(dbm_size_name(), function_sort({ sort_nat::nat() }, dbm()));
        return dbm_size;
      }

      /// \brief Recogniser for function dbm_size
      /// \param e A data expression
      /// \return true iff e is the function symbol matching dbm_size
      inline
      bool is_dbm_size_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == dbm_size();
        }
        return false;
      }

      /// \brief Application of function symbol dbm_size
      
      /// \param arg0 A data expression
      /// \return Application of dbm_size to a number of arguments
      inline
      application dbm_size(const data_expression& arg0)
      {
        return application(sort_dbm::dbm_size(),arg0);
      }

      /// \brief Recogniser for application of dbm_size
      /// \param e A data expression
      /// \return true iff e is an application of function symbol dbm_size to a
      ///     number of arguments
      inline
      bool is_dbm_size_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_dbm_size_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier dbm_zero2
      /// \return Identifier dbm_zero2
      inline
      core::identifier_string const& dbm_zero2_name()
      {
        static core::identifier_string dbm_zero2_name = core::identifier_string("dbm_zero2");
        return dbm_zero2_name;
      }

      /// \brief Constructor for function symbol dbm_zero2
      
      /// \return Function symbol dbm_zero2
      inline
      function_symbol const& dbm_zero2()
      {
        static function_symbol dbm_zero2(dbm_zero2_name(), function_sort({ sort_nat::nat(), sort_nat::nat() }, sort_list::list(sort_bound::bound())));
        return dbm_zero2;
      }

      /// \brief Recogniser for function dbm_zero2
      /// \param e A data expression
      /// \return true iff e is the function symbol matching dbm_zero2
      inline
      bool is_dbm_zero2_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == dbm_zero2();
        }
        return false;
      }

      /// \brief Application of function symbol dbm_zero2
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of dbm_zero2 to a number of arguments
      inline
      application dbm_zero2(const data_expression& arg0, const data_expression& arg1)
      {
        return application(sort_dbm::dbm_zero2(),arg0, arg1);
      }

      /// \brief Recogniser for application of dbm_zero2
      /// \param e A data expression
      /// \return true iff e is an application of function symbol dbm_zero2 to a
      ///     number of arguments
      inline
      bool is_dbm_zero2_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_dbm_zero2_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier dbm_size1
      /// \return Identifier dbm_size1
      inline
      core::identifier_string const& dbm_size1_name()
      {
        static core::identifier_string dbm_size1_name = core::identifier_string("dbm_size1");
        return dbm_size1_name;
      }

      /// \brief Constructor for function symbol dbm_size1
      
      /// \return Function symbol dbm_size1
      inline
      function_symbol const& dbm_size1()
      {
        static function_symbol dbm_size1(dbm_size1_name(), function_sort({ sort_nat::nat(), sort_nat::nat() }, sort_list::list(sort_bound::bound())));
        return dbm_size1;
      }

      /// \brief Recogniser for function dbm_size1
      /// \param e A data expression
      /// \return true iff e is the function symbol matching dbm_size1
      inline
      bool is_dbm_size1_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == dbm_size1();
        }
        return false;
      }

      /// \brief Application of function symbol dbm_size1
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of dbm_size1 to a number of arguments
      inline
      application dbm_size1(const data_expression& arg0, const data_expression& arg1)
      {
        return application(sort_dbm::dbm_size1(),arg0, arg1);
      }

      /// \brief Recogniser for application of dbm_size1
      /// \param e A data expression
      /// \return true iff e is an application of function symbol dbm_size1 to a
      ///     number of arguments
      inline
      bool is_dbm_size1_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_dbm_size1_function_symbol(application(e).head());
        }
        return false;
      }
      /// \brief Give all system defined mappings for dbm
      /// \return All system defined mappings for dbm
      inline
      function_symbol_vector dbm_generate_functions_code()
      {
        function_symbol_vector result;
        result.push_back(sort_dbm::get());
        result.push_back(sort_dbm::set());
        result.push_back(sort_dbm::set1());
        result.push_back(sort_dbm::close());
        result.push_back(sort_dbm::close1());
        result.push_back(sort_dbm::close2());
        result.push_back(sort_dbm::close3());
        result.push_back(sort_dbm::consistent());
        result.push_back(sort_dbm::and_d());
        result.push_back(sort_dbm::and_d1());
        result.push_back(sort_dbm::and_d2());
        result.push_back(sort_dbm::and_not());
        result.push_back(sort_dbm::free());
        result.push_back(sort_dbm::free1());
        result.push_back(sort_dbm::reset());
        result.push_back(sort_dbm::reset1());
        result.push_back(sort_dbm::shift());
        result.push_back(sort_dbm::shift_range());
        result.push_back(sort_dbm::shift1());
        result.push_back(sort_dbm::copy());
        result.push_back(sort_dbm::copy1());
        result.push_back(sort_dbm::split());
        result.push_back(sort_dbm::inconsistent());
        result.push_back(sort_dbm::N());
        result.push_back(sort_dbm::dbm_inconsistent());
        result.push_back(sort_dbm::dbm_empty());
        result.push_back(sort_dbm::dbm_zero());
        result.push_back(sort_dbm::dbm_zero1());
        result.push_back(sort_dbm::dbm_size());
        result.push_back(sort_dbm::dbm_zero2());
        result.push_back(sort_dbm::dbm_size1());
        return result;
      }
      ///\brief Function for projecting out argument
      ///        upper from an application
      /// \param e A data expression
      /// \pre upper is defined for e
      /// \return The argument of e that corresponds to upper
      inline
      data_expression upper(const data_expression& e)
      {
        assert(is_shift_range_application(e) || is_shift1_application(e));
        return atermpp::down_cast<const application >(e)[3];
      }

      ///\brief Function for projecting out argument
      ///        lower from an application
      /// \param e A data expression
      /// \pre lower is defined for e
      /// \return The argument of e that corresponds to lower
      inline
      data_expression lower(const data_expression& e)
      {
        assert(is_shift_range_application(e) || is_shift1_application(e));
        return atermpp::down_cast<const application >(e)[2];
      }

      ///\brief Function for projecting out argument
      ///        b from an application
      /// \param e A data expression
      /// \pre b is defined for e
      /// \return The argument of e that corresponds to b
      inline
      data_expression b(const data_expression& e)
      {
        assert(is_and_not_application(e) || is_split_application(e));
        return atermpp::down_cast<const application >(e)[4];
      }

      ///\brief Function for projecting out argument
      ///        dbm from an application
      /// \param e A data expression
      /// \pre dbm is defined for e
      /// \return The argument of e that corresponds to dbm
      inline
      data_expression dbm(const data_expression& e)
      {
        assert(is_get_application(e) || is_set_application(e) || is_close_application(e) || is_close1_application(e) || is_close2_application(e) || is_close3_application(e) || is_consistent_application(e) || is_and_d_application(e) || is_and_d1_application(e) || is_and_d2_application(e) || is_and_not_application(e) || is_free_application(e) || is_free1_application(e) || is_reset_application(e) || is_reset1_application(e) || is_shift_application(e) || is_shift_range_application(e) || is_shift1_application(e) || is_copy_application(e) || is_copy1_application(e) || is_split_application(e) || is_inconsistent_application(e));
        return atermpp::down_cast<const application >(e)[0];
      }

      ///\brief Function for projecting out argument
      ///        i from an application
      /// \param e A data expression
      /// \pre i is defined for e
      /// \return The argument of e that corresponds to i
      inline
      data_expression i(const data_expression& e)
      {
        assert(is_get_application(e) || is_set_application(e) || is_set1_application(e) || is_close1_application(e) || is_close2_application(e) || is_close3_application(e) || is_and_d_application(e) || is_and_d1_application(e) || is_and_d2_application(e) || is_and_not_application(e) || is_free_application(e) || is_free1_application(e) || is_reset_application(e) || is_reset1_application(e) || is_shift_application(e) || is_shift_range_application(e) || is_shift1_application(e) || is_copy_application(e) || is_copy1_application(e) || is_split_application(e));
        return atermpp::down_cast<const application >(e)[1];
      }

      ///\brief Function for projecting out argument
      ///        k from an application
      /// \param e A data expression
      /// \pre k is defined for e
      /// \return The argument of e that corresponds to k
      inline
      data_expression k(const data_expression& e)
      {
        assert(is_close2_application(e) || is_close3_application(e) || is_and_d1_application(e) || is_and_d2_application(e) || is_free1_application(e) || is_reset1_application(e) || is_shift1_application(e) || is_copy1_application(e) || is_dbm_zero1_application(e) || is_dbm_size_application(e) || is_dbm_zero2_application(e) || is_dbm_size1_application(e));
        if (is_dbm_zero1_application(e) || is_dbm_size_application(e) || is_dbm_zero2_application(e) || is_dbm_size1_application(e))
        {
          return atermpp::down_cast<const application >(e)[0];
        }
        if (is_close2_application(e) || is_close3_application(e) || is_free1_application(e))
        {
          return atermpp::down_cast<const application >(e)[2];
        }
        if (is_and_d1_application(e) || is_and_d2_application(e) || is_reset1_application(e) || is_copy1_application(e))
        {
          return atermpp::down_cast<const application >(e)[3];
        }
        if (is_shift1_application(e))
        {
          return atermpp::down_cast<const application >(e)[4];
        }
        return data_expression();
      }

      ///\brief Function for projecting out argument
      ///        j from an application
      /// \param e A data expression
      /// \pre j is defined for e
      /// \return The argument of e that corresponds to j
      inline
      data_expression j(const data_expression& e)
      {
        assert(is_get_application(e) || is_set_application(e) || is_and_d_application(e) || is_and_d1_application(e) || is_and_d2_application(e) || is_and_not_application(e) || is_copy_application(e) || is_copy1_application(e) || is_split_application(e));
        return atermpp::down_cast<const application >(e)[2];
      }

      ///\brief Function for projecting out argument
      ///        bound from an application
      /// \param e A data expression
      /// \pre bound is defined for e
      /// \return The argument of e that corresponds to bound
      inline
      data_expression bound(const data_expression& e)
      {
        assert(is_set_application(e) || is_set1_application(e) || is_and_d_application(e) || is_and_not_application(e) || is_split_application(e));
        if (is_set1_application(e))
        {
          return atermpp::down_cast<const application >(e)[2];
        }
        if (is_set_application(e) || is_and_d_application(e) || is_and_not_application(e) || is_split_application(e))
        {
          return atermpp::down_cast<const application >(e)[3];
        }
        return data_expression();
      }

      ///\brief Function for projecting out argument
      ///        value from an application
      /// \param e A data expression
      /// \pre value is defined for e
      /// \return The argument of e that corresponds to value
      inline
      data_expression value(const data_expression& e)
      {
        assert(is_reset_application(e) || is_reset1_application(e) || is_shift_application(e));
        return atermpp::down_cast<const application >(e)[2];
      }

      ///\brief Function for projecting out argument
      ///        l from an application
      /// \param e A data expression
      /// \pre l is defined for e
      /// \return The argument of e that corresponds to l
      inline
      data_expression l(const data_expression& e)
      {
        assert(is_set1_application(e) || is_close3_application(e) || is_and_d2_application(e) || is_dbm_zero2_application(e) || is_dbm_size1_application(e));
        if (is_set1_application(e))
        {
          return atermpp::down_cast<const application >(e)[0];
        }
        if (is_dbm_zero2_application(e) || is_dbm_size1_application(e))
        {
          return atermpp::down_cast<const application >(e)[1];
        }
        if (is_close3_application(e))
        {
          return atermpp::down_cast<const application >(e)[3];
        }
        if (is_and_d2_application(e))
        {
          return atermpp::down_cast<const application >(e)[4];
        }
        return data_expression();
      }

      /// \brief Give all system defined equations for dbm
      /// \return All system defined equations for sort dbm
      inline
      data_equation_vector dbm_generate_equations_code()
      {
        variable vr1("r1",sort_real::real_());
        variable vr2("r2",sort_real::real_());
        variable vb1("b1",sort_bound::bound());
        variable vb2("b2",sort_bound::bound());
        variable vd("d",dbm());
        variable vl1("l1",sort_list::list(sort_bound::bound()));
        variable vl("l",sort_nat::nat());
        variable vk("k",sort_nat::nat());
        variable vi("i",sort_nat::nat());
        variable vj("j",sort_nat::nat());

        data_equation_vector result;
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vj), get(vd, vi, vj), sort_list::element_at(sort_bound::bound())(sort_list::element_at(sort_list::list(sort_bound::bound()))(vd, vi), vj)));
        result.push_back(data_equation(atermpp::make_vector(vb1, vb2, vl1), set1(sort_list::cons_(sort_bound::bound())(vb2, vl1), sort_nat::c0(), vb1), sort_list::cons_(sort_bound::bound())(vb1, vl1)));
        result.push_back(data_equation(atermpp::make_vector(vb1, vb2, vj, vl1), not_equal_to(vj, sort_nat::c0()), set1(sort_list::cons_(sort_bound::bound())(vb2, vl1), vj, vb1), sort_list::cons_(sort_bound::bound())(vb2, set1(vl1, sort_int::int2nat(sort_real::minus(vj, sort_nat::cnat(sort_pos::c1()))), vb1))));
        result.push_back(data_equation(atermpp::make_vector(vb1, vd, vj, vl1), set(sort_list::cons_(sort_list::list(sort_bound::bound()))(vl1, vd), sort_nat::c0(), vj, vb1), sort_list::cons_(sort_list::list(sort_bound::bound()))(set1(vl1, vj, vb1), vd)));
        result.push_back(data_equation(atermpp::make_vector(vb1, vd, vi, vj, vl1), not_equal_to(vi, sort_nat::c0()), set(sort_list::cons_(sort_list::list(sort_bound::bound()))(vl1, vd), vi, vj, vb1), sort_list::cons_(sort_list::list(sort_bound::bound()))(vl1, set(vd, sort_int::int2nat(sort_real::minus(vi, sort_nat::cnat(sort_pos::c1()))), vj, vb1))));
        result.push_back(data_equation(atermpp::make_vector(vd), close(vd), close1(vd, sort_nat::c0())));
        result.push_back(data_equation(atermpp::make_vector(vd, vk), equal_to(vk, N()), close1(vd, vk), vd));
        result.push_back(data_equation(atermpp::make_vector(vd, vk), less(vk, N()), close1(vd, vk), close1(close2(vd, vk, sort_nat::c0()), sort_real::plus(vk, sort_nat::cnat(sort_pos::c1())))));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vk), equal_to(vi, N()), close2(vd, vk, vi), vd));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vk), less(vi, N()), close2(vd, vk, vi), close2(close3(vd, vk, vi, sort_nat::c0()), vk, sort_real::plus(vi, sort_nat::cnat(sort_pos::c1())))));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vj, vk), equal_to(vj, N()), close3(vd, vk, vi, vj), vd));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vj, vk), less(vj, N()), close3(vd, vk, vi, vj), close3(set(vd, vi, vj, sort_bound::min_b(get(vd, vi, vj), sort_bound::add(get(vd, vi, vk), get(vd, vk, vj)))), vk, vi, sort_real::plus(vj, sort_nat::cnat(sort_pos::c1())))));
        result.push_back(data_equation(atermpp::make_vector(vb1, vd, vi, vj), sort_bound::lt(sort_bound::add(get(vd, vj, vi), vb1), sort_bound::cbound(sort_real::creal(sort_int::cint(sort_nat::c0()),sort_pos::c1()), sort_inequality::le())), and_d(vd, vi, vj, vb1), dbm_inconsistent()));
        result.push_back(data_equation(atermpp::make_vector(vb1, vd, vi, vj), sort_bool::and_(sort_bound::lt(vb1, get(vd, vi, vj)), sort_bool::not_(sort_bound::lt(sort_bound::add(get(vd, vj, vi), vb1), sort_bound::cbound(sort_real::creal(sort_int::cint(sort_nat::c0()),sort_pos::c1()), sort_inequality::le())))), and_d(vd, vi, vj, vb1), and_d1(set(vd, vi, vj, vb1), vi, vj, sort_nat::c0())));
        result.push_back(data_equation(atermpp::make_vector(vb1, vd, vi, vj), sort_bool::and_(sort_bool::not_(sort_bound::lt(vb1, get(vd, vi, vj))), sort_bool::not_(sort_bound::lt(sort_bound::add(get(vd, vj, vi), vb1), sort_bound::cbound(sort_real::creal(sort_int::cint(sort_nat::c0()),sort_pos::c1()), sort_inequality::le())))), and_d(vd, vi, vj, vb1), vd));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vj, vk), equal_to(vk, N()), and_d1(vd, vi, vj, vk), vd));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vj, vk), less(vk, N()), and_d1(vd, vi, vj, vk), and_d1(and_d2(vd, vi, vj, vk, sort_nat::c0()), vi, vj, sort_real::plus(vk, sort_nat::cnat(sort_pos::c1())))));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vj, vk, vl), equal_to(vl, N()), and_d2(vd, vi, vj, vk, vl), vd));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vj, vk, vl), less(vl, N()), and_d2(vd, vi, vj, vk, vl), and_d2(set(vd, vk, vl, sort_bound::min_b(sort_bound::min_b(get(vd, vk, vl), sort_bound::add(get(vd, vk, vi), get(vd, vi, vl))), sort_bound::add(get(vd, vk, vj), get(vd, vj, vl)))), vi, vj, vk, sort_real::plus(vl, sort_nat::cnat(sort_pos::c1())))));
        result.push_back(data_equation(atermpp::make_vector(vd, vk), free(vd, vk), free1(vd, vk, sort_nat::c0())));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vk), equal_to(vi, N()), free1(vd, vk, vi), vd));
        result.push_back(data_equation(atermpp::make_vector(vd, vi), less(vi, N()), free1(vd, vi, vi), free1(vd, vi, sort_real::plus(vi, sort_nat::cnat(sort_pos::c1())))));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vk), sort_bool::and_(less(vi, N()), not_equal_to(vi, vk)), free1(vd, vi, vk), free1(set(set(vd, vk, vi, sort_bound::inf()), vi, vk, sort_bound::inf()), vi, sort_real::plus(vk, sort_nat::cnat(sort_pos::c1())))));
        result.push_back(data_equation(atermpp::make_vector(vd, vk, vr1), reset(vd, vk, vr1), reset1(vd, vk, vr1, sort_nat::c0())));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vk, vr1), equal_to(vi, N()), reset1(vd, vk, vr1, vi), vd));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vk, vr1), less(vi, N()), reset1(vd, vk, vr1, vi), reset1(set(set(vd, vk, vi, sort_bound::add(sort_bound::cbound(vr1, sort_inequality::le()), get(vd, sort_nat::c0(), vi))), vi, vk, sort_bound::add(get(vd, vi, sort_nat::c0()), sort_bound::cbound(sort_real::negate(vr1), sort_inequality::le()))), vk, vr1, sort_real::plus(vi, sort_nat::cnat(sort_pos::c1())))));
        result.push_back(data_equation(atermpp::make_vector(vd, vk, vr1), shift(vd, vk, vr1), shift_range(vd, vk, vr1, vr1)));
        result.push_back(data_equation(atermpp::make_vector(vd, vk, vr1, vr2), shift_range(vd, vk, vr1, vr2), shift1(vd, vk, vr1, vr2, sort_nat::c0())));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vk, vr1, vr2), equal_to(vi, N()), shift1(vd, vk, vr1, vr2, vi), vd));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vr1, vr2), less(vi, N()), shift1(vd, vi, vr1, vr2, vi), shift1(vd, vi, vr1, vr2, sort_real::plus(vi, sort_nat::cnat(sort_pos::c1())))));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vk, vr1, vr2), sort_bool::and_(less(vi, N()), not_equal_to(vi, vk)), shift1(vd, vk, vr1, vr2, vi), shift1(set(set(vd, vk, vi, sort_bound::add(get(vd, vk, vi), sort_bound::cbound(vr2, sort_inequality::le()))), vi, vk, sort_bound::add(get(vd, vi, vk), sort_bound::cbound(sort_real::negate(vr1), sort_inequality::le()))), vk, vr1, vr2, sort_real::plus(vi, sort_nat::cnat(sort_pos::c1())))));
        result.push_back(data_equation(atermpp::make_vector(vd, vk, vl), copy(vd, vk, vl), copy1(vd, vk, vl, sort_nat::c0())));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vk, vl), equal_to(vi, N()), copy1(vd, vk, vl, vi), set(set(vd, vk, vl, sort_bound::cbound(sort_real::creal(sort_int::cint(sort_nat::c0()),sort_pos::c1()), sort_inequality::le())), vl, vk, sort_bound::cbound(sort_real::creal(sort_int::cint(sort_nat::c0()),sort_pos::c1()), sort_inequality::le()))));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vl), less(vi, N()), copy1(vd, vi, vl, vi), copy1(vd, vi, vl, sort_real::plus(vi, sort_nat::cnat(sort_pos::c1())))));
        result.push_back(data_equation(atermpp::make_vector(vd, vi, vk, vl), sort_bool::and_(less(vi, N()), not_equal_to(vi, vk)), copy1(vd, vk, vl, vi), copy1(set(set(vd, vk, vi, get(vd, vl, vi)), vi, vk, get(vd, vi, vl)), vk, vl, sort_real::plus(vi, sort_nat::cnat(sort_pos::c1())))));
        result.push_back(data_equation(atermpp::make_vector(vb1, vd, vi, vj), split(vd, vi, vj, vb1, sort_bool::true_()), and_d(vd, vi, vj, vb1)));
        result.push_back(data_equation(atermpp::make_vector(vb1, vd, vi, vj), split(vd, vi, vj, vb1, sort_bool::false_()), and_d(vd, vj, vi, sort_bound::not_b(vb1))));
        result.push_back(data_equation(atermpp::make_vector(vd), inconsistent(vd), equal_to(get(vd, sort_nat::c0(), sort_nat::c0()), sort_bound::cbound(sort_real::creal(sort_int::cneg(sort_pos::c1()),sort_pos::c1()), sort_inequality::lt()))));
        result.push_back(data_equation(variable_list(), dbm_inconsistent(), set(dbm_zero(), sort_nat::c0(), sort_nat::c0(), sort_bound::cbound(sort_real::creal(sort_int::cneg(sort_pos::c1()),sort_pos::c1()), sort_inequality::lt()))));
        result.push_back(data_equation(variable_list(), dbm_empty(), dbm_size(N())));
        result.push_back(data_equation(variable_list(), dbm_size(sort_nat::c0()), sort_list::empty(sort_list::list(sort_bound::bound()))));
        result.push_back(data_equation(atermpp::make_vector(vi), not_equal_to(vi, sort_nat::c0()), dbm_size(vi), sort_list::cons_(sort_list::list(sort_bound::bound()))(dbm_size1(vi, N()), dbm_size(sort_int::int2nat(sort_real::minus(vi, sort_nat::cnat(sort_pos::c1())))))));
        result.push_back(data_equation(atermpp::make_vector(vi), dbm_size1(vi, sort_nat::c0()), sort_list::empty(sort_bound::bound())));
        result.push_back(data_equation(atermpp::make_vector(vi, vj), sort_bool::and_(equal_to(vi, vj), not_equal_to(vj, sort_nat::c0())), dbm_size1(vi, vj), sort_list::cons_(sort_bound::bound())(sort_bound::cbound(sort_real::creal(sort_int::cint(sort_nat::c0()),sort_pos::c1()), sort_inequality::le()), dbm_size1(vi, sort_int::int2nat(sort_real::minus(vj, sort_nat::cnat(sort_pos::c1())))))));
        result.push_back(data_equation(atermpp::make_vector(vi, vj), sort_bool::and_(not_equal_to(vi, vj), not_equal_to(vj, sort_nat::c0())), dbm_size1(vi, vj), sort_list::cons_(sort_bound::bound())(sort_bound::inf(), dbm_size1(vi, sort_int::int2nat(sort_real::minus(vj, sort_nat::cnat(sort_pos::c1())))))));
        result.push_back(data_equation(variable_list(), dbm_zero(), dbm_zero1(N())));
        result.push_back(data_equation(variable_list(), dbm_zero1(sort_nat::c0()), sort_list::empty(sort_list::list(sort_bound::bound()))));
        result.push_back(data_equation(atermpp::make_vector(vi), not_equal_to(vi, sort_nat::c0()), dbm_zero1(vi), sort_list::cons_(sort_list::list(sort_bound::bound()))(dbm_zero2(vi, N()), dbm_zero1(sort_int::int2nat(sort_real::minus(vi, sort_nat::cnat(sort_pos::c1())))))));
        result.push_back(data_equation(atermpp::make_vector(vi), dbm_zero2(vi, sort_nat::c0()), sort_list::empty(sort_bound::bound())));
        result.push_back(data_equation(atermpp::make_vector(vi, vj), sort_bool::and_(not_equal_to(vj, sort_nat::c0()), sort_bool::or_(equal_to(vi, vj), sort_bool::or_(equal_to(vi, N()), equal_to(vj, N())))), dbm_zero2(vi, vj), sort_list::cons_(sort_bound::bound())(sort_bound::cbound(sort_real::creal(sort_int::cint(sort_nat::c0()),sort_pos::c1()), sort_inequality::le()), dbm_zero2(vi, sort_int::int2nat(sort_real::minus(vj, sort_nat::cnat(sort_pos::c1())))))));
        result.push_back(data_equation(atermpp::make_vector(vi, vj), sort_bool::and_(not_equal_to(vj, sort_nat::c0()), sort_bool::and_(not_equal_to(vi, vj), sort_bool::and_(not_equal_to(vi, N()), not_equal_to(vj, N())))), dbm_zero2(vi, vj), sort_list::cons_(sort_bound::bound())(sort_bound::inf(), dbm_zero2(vi, sort_int::int2nat(sort_real::minus(vj, sort_nat::cnat(sort_pos::c1())))))));
        return result;
      }

    } // namespace sort_dbm

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DBM_H
