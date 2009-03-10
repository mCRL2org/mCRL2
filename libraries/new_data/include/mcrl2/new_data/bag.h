#ifndef MCRL2_NEW_DATA_BAG_H
#define MCRL2_NEW_DATA_BAG_H

#include "mcrl2/new_data/basic_sort.h"
#include "mcrl2/new_data/function_sort.h"
#include "mcrl2/new_data/function_symbol.h"
#include "mcrl2/new_data/application.h"
#include "mcrl2/new_data/data_equation.h"
#include "mcrl2/new_data/detail/container_utility.h"
#include "mcrl2/new_data/standard.h"
#include "mcrl2/new_data/lambda.h"
#include "mcrl2/new_data/forall.h"
#include "mcrl2/new_data/container_sort.h"
#include "mcrl2/new_data/bool.h"
#include "mcrl2/new_data/nat.h"
#include "mcrl2/new_data/set.h"

namespace mcrl2 {

  namespace new_data {

    /// \brief Namespace for system defined sort bag
    namespace sort_bag {

      /// \brief Constructor for sort expression Bag(s)
      /// \param s A sort expression
      /// \return Sort expression bag(s)
      inline
      container_sort bag(const sort_expression& s)
      {
        //static container_sort bag("bag", s);
        container_sort bag("bag", s);
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
          return static_cast<const container_sort&>(e).container_name() == "bag";
        }
        return false;
      }

      /// \brief Give all system defined constructors for bag
      /// \return All system defined constructors for bag
      inline
      function_symbol_vector bag_generate_constructors_code()
      {
        function_symbol_vector result;

        return result;
      }
      /// \brief Constructor for function symbol @bag
      /// \return Function symbol bag_comprehension
      inline
      function_symbol bag_comprehension(const sort_expression& s)
      {
        //static function_symbol bag_comprehension("@bag", function_sort(function_sort(s, sort_nat::nat()), bag(s)));
        function_symbol bag_comprehension("@bag", function_sort(function_sort(s, sort_nat::nat()), bag(s)));
        return bag_comprehension;
      }

      /// \brief Recogniser for function @bag
      /// \param e A data expression
      /// \return true iff e is the function symbol matching @bag
      inline
      bool is_bag_comprehension_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@bag";
        }
        return false;
      }

      ///\brief Application of function symbol @bag
      ///\return Application of @bag to a number of arguments
      inline
      application bag_comprehension(const sort_expression& s, const data_expression& arg0)
      {
        return application(bag_comprehension(s),arg0);
      }

      ///\brief Recogniser for application of @bag
      ///\param e A data expression
      ///\return true iff e is an application of function symbol bag_comprehension to a
      ///     number of arguments
      inline
      bool is_bag_comprehension_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_bag_comprehension_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol {}
      /// \return Function symbol emptybag
      inline
      function_symbol emptybag(const sort_expression& s)
      {
        //static function_symbol emptybag("{}", bag(s));
        function_symbol emptybag("{}", bag(s));
        return emptybag;
      }

      /// \brief Recogniser for function {}
      /// \param e A data expression
      /// \return true iff e is the function symbol matching {}
      inline
      bool is_emptybag_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "{}";
        }
        return false;
      }

      /// \brief Constructor for function symbol count
      /// \return Function symbol count
      inline
      function_symbol count(const sort_expression& s)
      {
        //static function_symbol count("count", function_sort(s, bag(s), sort_nat::nat()));
        function_symbol count("count", function_sort(s, bag(s), sort_nat::nat()));
        return count;
      }

      /// \brief Recogniser for function count
      /// \param e A data expression
      /// \return true iff e is the function symbol matching count
      inline
      bool is_count_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "count";
        }
        return false;
      }

      ///\brief Application of function symbol count
      ///\return Application of count to a number of arguments
      inline
      application count(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(count(s),arg0, arg1);
      }

      ///\brief Recogniser for application of count
      ///\param e A data expression
      ///\return true iff e is an application of function symbol count to a
      ///     number of arguments
      inline
      bool is_count_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_count_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol in
      /// \return Function symbol bagin
      inline
      function_symbol bagin(const sort_expression& s)
      {
        //static function_symbol bagin("in", function_sort(s, bag(s), sort_bool_::bool_()));
        function_symbol bagin("in", function_sort(s, bag(s), sort_bool_::bool_()));
        return bagin;
      }

      /// \brief Recogniser for function in
      /// \param e A data expression
      /// \return true iff e is the function symbol matching in
      inline
      bool is_bagin_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "in";
        }
        return false;
      }

      ///\brief Application of function symbol in
      ///\return Application of in to a number of arguments
      inline
      application bagin(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(bagin(s),arg0, arg1);
      }

      ///\brief Recogniser for application of in
      ///\param e A data expression
      ///\return true iff e is an application of function symbol bagin to a
      ///     number of arguments
      inline
      bool is_bagin_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_bagin_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol +
      /// \return Function symbol bagunion_
      inline
      function_symbol bagunion_(const sort_expression& s)
      {
        //static function_symbol bagunion_("+", function_sort(bag(s), bag(s), bag(s)));
        function_symbol bagunion_("+", function_sort(bag(s), bag(s), bag(s)));
        return bagunion_;
      }

      /// \brief Recogniser for function +
      /// \param e A data expression
      /// \return true iff e is the function symbol matching +
      inline
      bool is_bagunion__function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "+";
        }
        return false;
      }

      ///\brief Application of function symbol +
      ///\return Application of + to a number of arguments
      inline
      application bagunion_(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(bagunion_(s),arg0, arg1);
      }

      ///\brief Recogniser for application of +
      ///\param e A data expression
      ///\return true iff e is an application of function symbol bagunion_ to a
      ///     number of arguments
      inline
      bool is_bagunion__application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_bagunion__function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol -
      /// \return Function symbol bagdifference
      inline
      function_symbol bagdifference(const sort_expression& s)
      {
        //static function_symbol bagdifference("-", function_sort(bag(s), bag(s), bag(s)));
        function_symbol bagdifference("-", function_sort(bag(s), bag(s), bag(s)));
        return bagdifference;
      }

      /// \brief Recogniser for function -
      /// \param e A data expression
      /// \return true iff e is the function symbol matching -
      inline
      bool is_bagdifference_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "-";
        }
        return false;
      }

      ///\brief Application of function symbol -
      ///\return Application of - to a number of arguments
      inline
      application bagdifference(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(bagdifference(s),arg0, arg1);
      }

      ///\brief Recogniser for application of -
      ///\param e A data expression
      ///\return true iff e is an application of function symbol bagdifference to a
      ///     number of arguments
      inline
      bool is_bagdifference_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_bagdifference_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol *
      /// \return Function symbol bagintersection
      inline
      function_symbol bagintersection(const sort_expression& s)
      {
        //static function_symbol bagintersection("*", function_sort(bag(s), bag(s), bag(s)));
        function_symbol bagintersection("*", function_sort(bag(s), bag(s), bag(s)));
        return bagintersection;
      }

      /// \brief Recogniser for function *
      /// \param e A data expression
      /// \return true iff e is the function symbol matching *
      inline
      bool is_bagintersection_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "*";
        }
        return false;
      }

      ///\brief Application of function symbol *
      ///\return Application of * to a number of arguments
      inline
      application bagintersection(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(bagintersection(s),arg0, arg1);
      }

      ///\brief Recogniser for application of *
      ///\param e A data expression
      ///\return true iff e is an application of function symbol bagintersection to a
      ///     number of arguments
      inline
      bool is_bagintersection_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_bagintersection_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol Bag2Set
      /// \return Function symbol bag2set
      inline
      function_symbol bag2set(const sort_expression& s)
      {
        //static function_symbol bag2set("Bag2Set", function_sort(bag(s), sort_set_::set_(s)));
        function_symbol bag2set("Bag2Set", function_sort(bag(s), sort_set_::set_(s)));
        return bag2set;
      }

      /// \brief Recogniser for function Bag2Set
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Bag2Set
      inline
      bool is_bag2set_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "Bag2Set";
        }
        return false;
      }

      ///\brief Application of function symbol Bag2Set
      ///\return Application of Bag2Set to a number of arguments
      inline
      application bag2set(const sort_expression& s, const data_expression& arg0)
      {
        return application(bag2set(s),arg0);
      }

      ///\brief Recogniser for application of Bag2Set
      ///\param e A data expression
      ///\return true iff e is an application of function symbol bag2set to a
      ///     number of arguments
      inline
      bool is_bag2set_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_bag2set_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol Set2Bag
      /// \return Function symbol set2bag
      inline
      function_symbol set2bag(const sort_expression& s)
      {
        //static function_symbol set2bag("Set2Bag", function_sort(sort_set_::set_(s), bag(s)));
        function_symbol set2bag("Set2Bag", function_sort(sort_set_::set_(s), bag(s)));
        return set2bag;
      }

      /// \brief Recogniser for function Set2Bag
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Set2Bag
      inline
      bool is_set2bag_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "Set2Bag";
        }
        return false;
      }

      ///\brief Application of function symbol Set2Bag
      ///\return Application of Set2Bag to a number of arguments
      inline
      application set2bag(const sort_expression& s, const data_expression& arg0)
      {
        return application(set2bag(s),arg0);
      }

      ///\brief Recogniser for application of Set2Bag
      ///\param e A data expression
      ///\return true iff e is an application of function symbol set2bag to a
      ///     number of arguments
      inline
      bool is_set2bag_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_set2bag_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Give all system defined mappings for bag
      /// \return All system defined mappings for bag
      inline
      function_symbol_vector bag_generate_functions_code(const sort_expression& s)
      {
        function_symbol_vector result;
        result.push_back(bag_comprehension(s));
        result.push_back(emptybag(s));
        result.push_back(count(s));
        result.push_back(bagin(s));
        result.push_back(bagunion_(s));
        result.push_back(bagdifference(s));
        result.push_back(bagintersection(s));
        result.push_back(bag2set(s));
        result.push_back(set2bag(s));
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
        if (is_count_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_bagin_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_bagunion__application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_bagdifference_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_bagintersection_application(e))
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
        if (is_count_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }
        if (is_bagin_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }
        if (is_bagunion__application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }
        if (is_bagdifference_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }
        if (is_bagintersection_application(e))
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
        if (is_bag_comprehension_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_bag2set_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_set2bag_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      /// \brief Give all system defined equations for bag
      /// \param s A sort expression
      /// \return All system defined equations for sort bag
      inline
      data_equation_vector bag_generate_equations_code(const sort_expression& s)
      {
        variable vd("d",s);
        variable vf("f",function_sort(s, sort_nat::nat()));
        variable vg("g",function_sort(s, sort_nat::nat()));
        variable vs("s",bag(s));
        variable vt("t",bag(s));
        variable vu("u",sort_set_::set_(s));
        variable vx("x",s);
        variable vy("y",s);
        variable vm("m",sort_nat::nat());
        variable vn("n",sort_nat::nat());

        data_equation_vector result;
        result.push_back(data_equation(make_vector(vg, vf), equal_to(bag_comprehension(s, vf), bag_comprehension(s, vg)), equal_to(vf, vg)));
        result.push_back(data_equation(variable_list(), emptybag(s), bag_comprehension(s, lambda(make_vector(vx), sort_nat::c0()))));
        result.push_back(data_equation(make_vector(vf, vd), count(s, vd, bag_comprehension(s, vf)), vf(vd)));
        result.push_back(data_equation(make_vector(vs, vd), bagin(s, vd, vs), greater(count(s, vd, vs), sort_nat::c0())));
        result.push_back(data_equation(make_vector(vg, vf, vx), less_equal(bag_comprehension(s, vf), bag_comprehension(s, vg)), forall(make_vector(vx), less_equal(vf(vx), vg(vx)))));
        result.push_back(data_equation(make_vector(vt, vs), less(vs, vt), sort_bool_::and_(less_equal(vs, vt), not_equal_to(vs, vt))));
        result.push_back(data_equation(make_vector(vg, vf, vx), bagunion_(s, bag_comprehension(s, vf), bag_comprehension(s, vg)), bag_comprehension(s, lambda(make_vector(vx), bagunion_(s, vf(vx), vg(vx))))));
        result.push_back(data_equation(make_vector(vy, vg, vm, vf, vn), bagdifference(s, bag_comprehension(s, vf), bag_comprehension(s, vg)), bag_comprehension(s, lambda(make_vector(vy), lambda(make_vector(vm), lambda(make_vector(vn), if_(greater(vm, vn), sort_nat::gtesubt(vm, vn), sort_nat::c0())))(vf(vy))(vg(vy))))));
        result.push_back(data_equation(make_vector(vg, vf, vx), bagintersection(s, bag_comprehension(s, vf), bag_comprehension(s, vg)), bag_comprehension(s, lambda(make_vector(vx), sort_nat::minimum(vf(vx), vg(vx))))));
        result.push_back(data_equation(make_vector(vx, vs), bag2set(s, vs), sort_set_::set_comprehension(s, lambda(make_vector(vx), bagin(s, vx, vs)))));
        result.push_back(data_equation(make_vector(vx, vu), set2bag(s, vu), bag_comprehension(s, lambda(make_vector(vx), if_(bagin(s, vx, vu), sort_nat::cnat(sort_pos::c1()), sort_nat::c0())))));
        return result;
      }

    } // namespace sort_bag

  } // namespace new_data

} // namespace mcrl2

#endif // MCRL2_NEW_DATA_BAG_H
