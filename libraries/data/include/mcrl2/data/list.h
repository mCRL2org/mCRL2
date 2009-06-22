// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/list.h
/// \brief The standard sort list.
///
/// This file was generated from the data sort specification
/// mcrl2/data/build/list.spec.

#ifndef MCRL2_DATA_LIST_H
#define MCRL2_DATA_LIST_H

#include "mcrl2/exception.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/detail/container_utility.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/container_sort.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort list
    namespace sort_list {

      /// \brief Constructor for sort expression List(S)
      /// \param s A sort expression
      /// \return Sort expression list(s)
      inline
      container_sort list(const sort_expression& s)
      {
        container_sort list(container_sort::list(), s);
        return list;
      }

      /// \brief Recogniser for sort expression List(s)
      /// \param e A sort expression
      /// \return true iff e is a container sort of which the name matches
      ///      list
      inline
      bool is_list(const sort_expression& e)
      {
        if (e.is_container_sort())
        {
          return static_cast< container_sort >(e).container_type() == container_sort::list();
        }
        return false;
      }

      /// \brief Generate identifier []
      /// \return Identifier []
      inline
      core::identifier_string const& nil_name()
      {
        static core::identifier_string nil_name = data::detail::initialise_static_expression(nil_name, core::identifier_string("[]"));
        return nil_name;
      }

      /// \brief Constructor for function symbol []
      /// \param s A sort expression
      /// \return Function symbol nil
      inline
      function_symbol nil(const sort_expression& s)
      {
        function_symbol nil(nil_name(), list(s));
        return nil;
      }


      /// \brief Recogniser for function []
      /// \param e A data expression
      /// \return true iff e is the function symbol matching []
      inline
      bool is_nil_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == nil_name();
        }
        return false;
      }

      /// \brief Generate identifier |>
      /// \return Identifier |>
      inline
      core::identifier_string const& cons_name()
      {
        static core::identifier_string cons_name = data::detail::initialise_static_expression(cons_name, core::identifier_string("|>"));
        return cons_name;
      }

      /// \brief Constructor for function symbol |>
      /// \param s A sort expression
      /// \return Function symbol cons_
      inline
      function_symbol cons_(const sort_expression& s)
      {
        function_symbol cons_(cons_name(), function_sort(s, list(s), list(s)));
        return cons_;
      }


      /// \brief Recogniser for function |>
      /// \param e A data expression
      /// \return true iff e is the function symbol matching |>
      inline
      bool is_cons_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == cons_name();
        }
        return false;
      }

      /// \brief Application of function symbol |>
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of |> to a number of arguments
      inline
      application cons_(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(cons_(s),arg0, arg1);
      }

      /// \brief Recogniser for application of |>
      /// \param e A data expression
      /// \return true iff e is an application of function symbol cons_ to a
      ///     number of arguments
      inline
      bool is_cons_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_cons_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Give all system defined constructors for list
      /// \param s A sort expression
      /// \return All system defined constructors for list
      inline
      function_symbol_vector list_generate_constructors_code(const sort_expression& s)
      {
        function_symbol_vector result;
        result.push_back(nil(s));
        result.push_back(cons_(s));

        return result;
      }
      /// \brief Generate identifier in
      /// \return Identifier in
      inline
      core::identifier_string const& in_name()
      {
        static core::identifier_string in_name = data::detail::initialise_static_expression(in_name, core::identifier_string("in"));
        return in_name;
      }

      /// \brief Constructor for function symbol in
      /// \param s A sort expression
      /// \return Function symbol in
      inline
      function_symbol in(const sort_expression& s)
      {
        function_symbol in(in_name(), function_sort(s, list(s), sort_bool::bool_()));
        return in;
      }


      /// \brief Recogniser for function in
      /// \param e A data expression
      /// \return true iff e is the function symbol matching in
      inline
      bool is_in_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == in_name();
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
        return application(in(s),arg0, arg1);
      }

      /// \brief Recogniser for application of in
      /// \param e A data expression
      /// \return true iff e is an application of function symbol in to a
      ///     number of arguments
      inline
      bool is_in_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_in_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier #
      /// \return Identifier #
      inline
      core::identifier_string const& count_name()
      {
        static core::identifier_string count_name = data::detail::initialise_static_expression(count_name, core::identifier_string("#"));
        return count_name;
      }

      /// \brief Constructor for function symbol #
      /// \param s A sort expression
      /// \return Function symbol count
      inline
      function_symbol count(const sort_expression& s)
      {
        function_symbol count(count_name(), function_sort(list(s), sort_nat::nat()));
        return count;
      }


      /// \brief Recogniser for function #
      /// \param e A data expression
      /// \return true iff e is the function symbol matching #
      inline
      bool is_count_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == count_name();
        }
        return false;
      }

      /// \brief Application of function symbol #
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of # to a number of arguments
      inline
      application count(const sort_expression& s, const data_expression& arg0)
      {
        return application(count(s),arg0);
      }

      /// \brief Recogniser for application of #
      /// \param e A data expression
      /// \return true iff e is an application of function symbol count to a
      ///     number of arguments
      inline
      bool is_count_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_count_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier <|
      /// \return Identifier <|
      inline
      core::identifier_string const& snoc_name()
      {
        static core::identifier_string snoc_name = data::detail::initialise_static_expression(snoc_name, core::identifier_string("<|"));
        return snoc_name;
      }

      /// \brief Constructor for function symbol <|
      /// \param s A sort expression
      /// \return Function symbol snoc
      inline
      function_symbol snoc(const sort_expression& s)
      {
        function_symbol snoc(snoc_name(), function_sort(list(s), s, list(s)));
        return snoc;
      }


      /// \brief Recogniser for function <|
      /// \param e A data expression
      /// \return true iff e is the function symbol matching <|
      inline
      bool is_snoc_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == snoc_name();
        }
        return false;
      }

      /// \brief Application of function symbol <|
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of <| to a number of arguments
      inline
      application snoc(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(snoc(s),arg0, arg1);
      }

      /// \brief Recogniser for application of <|
      /// \param e A data expression
      /// \return true iff e is an application of function symbol snoc to a
      ///     number of arguments
      inline
      bool is_snoc_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_snoc_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier ++
      /// \return Identifier ++
      inline
      core::identifier_string const& concat_name()
      {
        static core::identifier_string concat_name = data::detail::initialise_static_expression(concat_name, core::identifier_string("++"));
        return concat_name;
      }

      /// \brief Constructor for function symbol ++
      /// \param s A sort expression
      /// \return Function symbol concat
      inline
      function_symbol concat(const sort_expression& s)
      {
        function_symbol concat(concat_name(), function_sort(list(s), list(s), list(s)));
        return concat;
      }


      /// \brief Recogniser for function ++
      /// \param e A data expression
      /// \return true iff e is the function symbol matching ++
      inline
      bool is_concat_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == concat_name();
        }
        return false;
      }

      /// \brief Application of function symbol ++
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of ++ to a number of arguments
      inline
      application concat(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(concat(s),arg0, arg1);
      }

      /// \brief Recogniser for application of ++
      /// \param e A data expression
      /// \return true iff e is an application of function symbol concat to a
      ///     number of arguments
      inline
      bool is_concat_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_concat_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier .
      /// \return Identifier .
      inline
      core::identifier_string const& element_at_name()
      {
        static core::identifier_string element_at_name = data::detail::initialise_static_expression(element_at_name, core::identifier_string("."));
        return element_at_name;
      }

      /// \brief Constructor for function symbol .
      /// \param s A sort expression
      /// \return Function symbol element_at
      inline
      function_symbol element_at(const sort_expression& s)
      {
        function_symbol element_at(element_at_name(), function_sort(list(s), sort_nat::nat(), s));
        return element_at;
      }


      /// \brief Recogniser for function .
      /// \param e A data expression
      /// \return true iff e is the function symbol matching .
      inline
      bool is_element_at_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == element_at_name();
        }
        return false;
      }

      /// \brief Application of function symbol .
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of . to a number of arguments
      inline
      application element_at(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(element_at(s),arg0, arg1);
      }

      /// \brief Recogniser for application of .
      /// \param e A data expression
      /// \return true iff e is an application of function symbol element_at to a
      ///     number of arguments
      inline
      bool is_element_at_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_element_at_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier head
      /// \return Identifier head
      inline
      core::identifier_string const& head_name()
      {
        static core::identifier_string head_name = data::detail::initialise_static_expression(head_name, core::identifier_string("head"));
        return head_name;
      }

      /// \brief Constructor for function symbol head
      /// \param s A sort expression
      /// \return Function symbol head
      inline
      function_symbol head(const sort_expression& s)
      {
        function_symbol head(head_name(), function_sort(list(s), s));
        return head;
      }


      /// \brief Recogniser for function head
      /// \param e A data expression
      /// \return true iff e is the function symbol matching head
      inline
      bool is_head_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == head_name();
        }
        return false;
      }

      /// \brief Application of function symbol head
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of head to a number of arguments
      inline
      application head(const sort_expression& s, const data_expression& arg0)
      {
        return application(head(s),arg0);
      }

      /// \brief Recogniser for application of head
      /// \param e A data expression
      /// \return true iff e is an application of function symbol head to a
      ///     number of arguments
      inline
      bool is_head_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_head_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier tail
      /// \return Identifier tail
      inline
      core::identifier_string const& tail_name()
      {
        static core::identifier_string tail_name = data::detail::initialise_static_expression(tail_name, core::identifier_string("tail"));
        return tail_name;
      }

      /// \brief Constructor for function symbol tail
      /// \param s A sort expression
      /// \return Function symbol tail
      inline
      function_symbol tail(const sort_expression& s)
      {
        function_symbol tail(tail_name(), function_sort(list(s), list(s)));
        return tail;
      }


      /// \brief Recogniser for function tail
      /// \param e A data expression
      /// \return true iff e is the function symbol matching tail
      inline
      bool is_tail_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == tail_name();
        }
        return false;
      }

      /// \brief Application of function symbol tail
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of tail to a number of arguments
      inline
      application tail(const sort_expression& s, const data_expression& arg0)
      {
        return application(tail(s),arg0);
      }

      /// \brief Recogniser for application of tail
      /// \param e A data expression
      /// \return true iff e is an application of function symbol tail to a
      ///     number of arguments
      inline
      bool is_tail_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_tail_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier rhead
      /// \return Identifier rhead
      inline
      core::identifier_string const& rhead_name()
      {
        static core::identifier_string rhead_name = data::detail::initialise_static_expression(rhead_name, core::identifier_string("rhead"));
        return rhead_name;
      }

      /// \brief Constructor for function symbol rhead
      /// \param s A sort expression
      /// \return Function symbol rhead
      inline
      function_symbol rhead(const sort_expression& s)
      {
        function_symbol rhead(rhead_name(), function_sort(list(s), s));
        return rhead;
      }


      /// \brief Recogniser for function rhead
      /// \param e A data expression
      /// \return true iff e is the function symbol matching rhead
      inline
      bool is_rhead_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == rhead_name();
        }
        return false;
      }

      /// \brief Application of function symbol rhead
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of rhead to a number of arguments
      inline
      application rhead(const sort_expression& s, const data_expression& arg0)
      {
        return application(rhead(s),arg0);
      }

      /// \brief Recogniser for application of rhead
      /// \param e A data expression
      /// \return true iff e is an application of function symbol rhead to a
      ///     number of arguments
      inline
      bool is_rhead_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_rhead_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier rtail
      /// \return Identifier rtail
      inline
      core::identifier_string const& rtail_name()
      {
        static core::identifier_string rtail_name = data::detail::initialise_static_expression(rtail_name, core::identifier_string("rtail"));
        return rtail_name;
      }

      /// \brief Constructor for function symbol rtail
      /// \param s A sort expression
      /// \return Function symbol rtail
      inline
      function_symbol rtail(const sort_expression& s)
      {
        function_symbol rtail(rtail_name(), function_sort(list(s), list(s)));
        return rtail;
      }


      /// \brief Recogniser for function rtail
      /// \param e A data expression
      /// \return true iff e is the function symbol matching rtail
      inline
      bool is_rtail_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == rtail_name();
        }
        return false;
      }

      /// \brief Application of function symbol rtail
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \return Application of rtail to a number of arguments
      inline
      application rtail(const sort_expression& s, const data_expression& arg0)
      {
        return application(rtail(s),arg0);
      }

      /// \brief Recogniser for application of rtail
      /// \param e A data expression
      /// \return true iff e is an application of function symbol rtail to a
      ///     number of arguments
      inline
      bool is_rtail_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_rtail_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Give all system defined mappings for list
      /// \param s A sort expression
      /// \return All system defined mappings for list
      inline
      function_symbol_vector list_generate_functions_code(const sort_expression& s)
      {
        function_symbol_vector result;
        result.push_back(in(s));
        result.push_back(count(s));
        result.push_back(snoc(s));
        result.push_back(concat(s));
        result.push_back(element_at(s));
        result.push_back(head(s));
        result.push_back(tail(s));
        result.push_back(rhead(s));
        result.push_back(rtail(s));
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
        assert(is_cons_application(e));
        return static_cast< application >(e).arguments()[0];
      }

      ///\brief Function for projecting out argument
      ///        right from an application
      /// \param e A data expression
      /// \pre right is defined for e
      /// \return The argument of e that corresponds to right
      inline
      data_expression right(const data_expression& e)
      {
        assert(is_concat_application(e));
        return static_cast< application >(e).arguments()[1];
      }

      ///\brief Function for projecting out argument
      ///        arg1 from an application
      /// \param e A data expression
      /// \pre arg1 is defined for e
      /// \return The argument of e that corresponds to arg1
      inline
      data_expression arg1(const data_expression& e)
      {
        assert(is_in_application(e));
        return static_cast< application >(e).arguments()[0];
      }

      ///\brief Function for projecting out argument
      ///        arg2 from an application
      /// \param e A data expression
      /// \pre arg2 is defined for e
      /// \return The argument of e that corresponds to arg2
      inline
      data_expression arg2(const data_expression& e)
      {
        assert(is_in_application(e));
        return static_cast< application >(e).arguments()[1];
      }

      ///\brief Function for projecting out argument
      ///        list from an application
      /// \param e A data expression
      /// \pre list is defined for e
      /// \return The argument of e that corresponds to list
      inline
      data_expression list(const data_expression& e)
      {
        assert(is_count_application(e) || is_element_at_application(e) || is_head_application(e) || is_tail_application(e) || is_rhead_application(e) || is_rtail_application(e));
        return static_cast< application >(e).arguments()[0];
      }

      ///\brief Function for projecting out argument
      ///        tail from an application
      /// \param e A data expression
      /// \pre tail is defined for e
      /// \return The argument of e that corresponds to tail
      inline
      data_expression tail(const data_expression& e)
      {
        assert(is_cons_application(e));
        return static_cast< application >(e).arguments()[1];
      }

      ///\brief Function for projecting out argument
      ///        rhead from an application
      /// \param e A data expression
      /// \pre rhead is defined for e
      /// \return The argument of e that corresponds to rhead
      inline
      data_expression rhead(const data_expression& e)
      {
        assert(is_snoc_application(e));
        return static_cast< application >(e).arguments()[1];
      }

      ///\brief Function for projecting out argument
      ///        position from an application
      /// \param e A data expression
      /// \pre position is defined for e
      /// \return The argument of e that corresponds to position
      inline
      data_expression position(const data_expression& e)
      {
        assert(is_element_at_application(e));
        return static_cast< application >(e).arguments()[1];
      }

      ///\brief Function for projecting out argument
      ///        rtail from an application
      /// \param e A data expression
      /// \pre rtail is defined for e
      /// \return The argument of e that corresponds to rtail
      inline
      data_expression rtail(const data_expression& e)
      {
        assert(is_snoc_application(e));
        return static_cast< application >(e).arguments()[0];
      }

      ///\brief Function for projecting out argument
      ///        left from an application
      /// \param e A data expression
      /// \pre left is defined for e
      /// \return The argument of e that corresponds to left
      inline
      data_expression left(const data_expression& e)
      {
        assert(is_concat_application(e));
        return static_cast< application >(e).arguments()[0];
      }

      /// \brief Give all system defined equations for list
      /// \param s A sort expression
      /// \return All system defined equations for sort list
      inline
      data_equation_vector list_generate_equations_code(const sort_expression& s)
      {
        variable vd("d",s);
        variable ve("e",s);
        variable vs("s",list(s));
        variable vt("t",list(s));
        variable vp("p",sort_pos::pos());

        data_equation_vector result;
        result.push_back(data_equation(make_vector(vd, vs), equal_to(nil(s), cons_(s, vd, vs)), sort_bool::false_()));
        result.push_back(data_equation(make_vector(vd, vs), equal_to(cons_(s, vd, vs), nil(s)), sort_bool::false_()));
        result.push_back(data_equation(make_vector(vd, ve, vs, vt), equal_to(cons_(s, vd, vs), cons_(s, ve, vt)), sort_bool::and_(equal_to(vd, ve), equal_to(vs, vt))));
        result.push_back(data_equation(make_vector(vd), in(s, vd, nil(s)), sort_bool::false_()));
        result.push_back(data_equation(make_vector(vd, ve, vs), in(s, vd, cons_(s, ve, vs)), sort_bool::or_(equal_to(vd, ve), in(s, vd, vs))));
        result.push_back(data_equation(variable_list(), count(s, nil(s)), sort_nat::c0()));
        result.push_back(data_equation(make_vector(vd, vs), count(s, cons_(s, vd, vs)), sort_nat::cnat(sort_nat::succ(count(s, vs)))));
        result.push_back(data_equation(make_vector(vd), snoc(s, nil(s), vd), cons_(s, vd, nil(s))));
        result.push_back(data_equation(make_vector(vd, ve, vs), snoc(s, cons_(s, vd, vs), ve), cons_(s, vd, snoc(s, vs, ve))));
        result.push_back(data_equation(make_vector(vs), concat(s, nil(s), vs), vs));
        result.push_back(data_equation(make_vector(vd, vs, vt), concat(s, cons_(s, vd, vs), vt), cons_(s, vd, concat(s, vs, vt))));
        result.push_back(data_equation(make_vector(vs), concat(s, vs, nil(s)), vs));
        result.push_back(data_equation(make_vector(vd, vs), element_at(s, cons_(s, vd, vs), sort_nat::c0()), vd));
        result.push_back(data_equation(make_vector(vd, vp, vs), element_at(s, cons_(s, vd, vs), sort_nat::cnat(vp)), element_at(s, vs, sort_nat::pred(vp))));
        result.push_back(data_equation(make_vector(vd, vs), head(s, cons_(s, vd, vs)), vd));
        result.push_back(data_equation(make_vector(vd, vs), tail(s, cons_(s, vd, vs)), vs));
        result.push_back(data_equation(make_vector(vd), rhead(s, cons_(s, vd, nil(s))), vd));
        result.push_back(data_equation(make_vector(vd, ve, vs), rhead(s, cons_(s, vd, cons_(s, ve, vs))), rhead(s, cons_(s, ve, vs))));
        result.push_back(data_equation(make_vector(vd), rtail(s, cons_(s, vd, nil(s))), nil(s)));
        result.push_back(data_equation(make_vector(vd, ve, vs), rtail(s, cons_(s, vd, cons_(s, ve, vs))), cons_(s, vd, rtail(s, cons_(s, ve, vs)))));
        return result;
      }

      /// \brief Add sort, constructors, mappings and equations for list
      /// \param specification a specification
      /// \param element the sort of elements stored by the container
      inline
      void add_list_to_specification(data_specification& specification, sort_expression const& element)
      {
         if (specification.constructors(sort_nat::nat()).empty())
         {
           sort_nat::add_nat_to_specification(specification);
         }
         if (specification.constructors(sort_bool::bool_()).empty())
         {
           sort_bool::add_bool_to_specification(specification);
         }
         if (specification.constructors(sort_pos::pos()).empty())
         {
           sort_pos::add_pos_to_specification(specification);
         }
         specification.add_system_defined_sort(list(element));
         specification.add_system_defined_constructors(list_generate_constructors_code(element));
         specification.add_system_defined_mappings(list_generate_functions_code(element));
         specification.add_system_defined_equations(list_generate_equations_code(element));
      }
    } // namespace sort_list

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_LIST_H
