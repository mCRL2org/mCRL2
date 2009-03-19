#ifndef MCRL2_NEW_DATA_SET__H
#define MCRL2_NEW_DATA_SET__H

#include "mcrl2/new_data/basic_sort.h"
#include "mcrl2/new_data/function_sort.h"
#include "mcrl2/new_data/function_symbol.h"
#include "mcrl2/new_data/application.h"
#include "mcrl2/new_data/data_equation.h"
#include "mcrl2/new_data/detail/container_utility.h"
#include "mcrl2/new_data/standard.h"
#include "mcrl2/new_data/data_specification.h"
#include "mcrl2/new_data/lambda.h"
#include "mcrl2/new_data/forall.h"
#include "mcrl2/new_data/container_sort.h"
#include "mcrl2/new_data/bool.h"

namespace mcrl2 {

  namespace new_data {

    /// \brief Namespace for system defined sort set_
    namespace sort_set_ {

      /// \brief Constructor for sort expression Set(S)
      /// \param s A sort expression
      /// \return Sort expression set_(s)
      inline
      container_sort set_(const sort_expression& s)
      {
        container_sort set_("set_", s);
        return set_;
      }

      /// \brief Recogniser for sort expression Set(s)
      /// \param e A sort expression
      /// \return true iff e is a container sort of which the name matches
      ///      set_
      inline
      bool is_set_(const sort_expression& e)
      {
        if (e.is_container_sort())
        {
          return static_cast<const container_sort&>(e).container_name() == "set_";
        }
        return false;
      }

      /// \brief Give all system defined constructors for set_
      /// \return All system defined constructors for set_
      inline
      function_symbol_vector set__generate_constructors_code()
      {
        function_symbol_vector result;

        return result;
      }
      /// \brief Constructor for function symbol \@set
      /// \return Function symbol set_comprehension
      inline
      function_symbol set_comprehension(const sort_expression& s)
      {
        function_symbol set_comprehension("@set", function_sort(function_sort(s, sort_bool_::bool_()), set_(s)));
        return set_comprehension;
      }

      /// \brief Recogniser for function \@set
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@set
      inline
      bool is_set_comprehension_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@set";
        }
        return false;
      }

      ///\brief Application of function symbol \@set
      ///\return Application of \@set to a number of arguments
      inline
      application set_comprehension(const sort_expression& s, const data_expression& arg0)
      {
        return application(set_comprehension(s),arg0);
      }

      ///\brief Recogniser for application of \@set
      ///\param e A data expression
      ///\return true iff e is an application of function symbol set_comprehension to a
      ///     number of arguments
      inline
      bool is_set_comprehension_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_set_comprehension_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol {}
      /// \return Function symbol emptyset
      inline
      function_symbol emptyset(const sort_expression& s)
      {
        function_symbol emptyset("{}", set_(s));
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
          return static_cast<const function_symbol&>(e).name() == "{}";
        }
        return false;
      }

      /// \brief Constructor for function symbol in
      /// \return Function symbol setin
      inline
      function_symbol setin(const sort_expression& s)
      {
        function_symbol setin("in", function_sort(s, set_(s), sort_bool_::bool_()));
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
          return static_cast<const function_symbol&>(e).name() == "in";
        }
        return false;
      }

      ///\brief Application of function symbol in
      ///\return Application of in to a number of arguments
      inline
      application setin(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(setin(s),arg0, arg1);
      }

      ///\brief Recogniser for application of in
      ///\param e A data expression
      ///\return true iff e is an application of function symbol setin to a
      ///     number of arguments
      inline
      bool is_setin_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setin_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol +
      /// \return Function symbol setunion_
      inline
      function_symbol setunion_(const sort_expression& s)
      {
        function_symbol setunion_("+", function_sort(set_(s), set_(s), set_(s)));
        return setunion_;
      }

      /// \brief Recogniser for function +
      /// \param e A data expression
      /// \return true iff e is the function symbol matching +
      inline
      bool is_setunion__function_symbol(const data_expression& e)
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
      application setunion_(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(setunion_(s),arg0, arg1);
      }

      ///\brief Recogniser for application of +
      ///\param e A data expression
      ///\return true iff e is an application of function symbol setunion_ to a
      ///     number of arguments
      inline
      bool is_setunion__application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setunion__function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol -
      /// \return Function symbol setdifference
      inline
      function_symbol setdifference(const sort_expression& s)
      {
        function_symbol setdifference("-", function_sort(set_(s), set_(s), set_(s)));
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
          return static_cast<const function_symbol&>(e).name() == "-";
        }
        return false;
      }

      ///\brief Application of function symbol -
      ///\return Application of - to a number of arguments
      inline
      application setdifference(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(setdifference(s),arg0, arg1);
      }

      ///\brief Recogniser for application of -
      ///\param e A data expression
      ///\return true iff e is an application of function symbol setdifference to a
      ///     number of arguments
      inline
      bool is_setdifference_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setdifference_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol *
      /// \return Function symbol setintersection
      inline
      function_symbol setintersection(const sort_expression& s)
      {
        function_symbol setintersection("*", function_sort(set_(s), set_(s), set_(s)));
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
          return static_cast<const function_symbol&>(e).name() == "*";
        }
        return false;
      }

      ///\brief Application of function symbol *
      ///\return Application of * to a number of arguments
      inline
      application setintersection(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(setintersection(s),arg0, arg1);
      }

      ///\brief Recogniser for application of *
      ///\param e A data expression
      ///\return true iff e is an application of function symbol setintersection to a
      ///     number of arguments
      inline
      bool is_setintersection_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setintersection_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol !
      /// \return Function symbol setcomplement
      inline
      function_symbol setcomplement(const sort_expression& s)
      {
        function_symbol setcomplement("!", function_sort(set_(s), set_(s)));
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
          return static_cast<const function_symbol&>(e).name() == "!";
        }
        return false;
      }

      ///\brief Application of function symbol !
      ///\return Application of ! to a number of arguments
      inline
      application setcomplement(const sort_expression& s, const data_expression& arg0)
      {
        return application(setcomplement(s),arg0);
      }

      ///\brief Recogniser for application of !
      ///\param e A data expression
      ///\return true iff e is an application of function symbol setcomplement to a
      ///     number of arguments
      inline
      bool is_setcomplement_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setcomplement_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Give all system defined mappings for set_
      /// \param s A sort expression
      /// \return All system defined mappings for set_
      inline
      function_symbol_vector set__generate_functions_code(const sort_expression& s)
      {
        function_symbol_vector result;
        result.push_back(set_comprehension(s));
        result.push_back(emptyset(s));
        result.push_back(setin(s));
        result.push_back(setunion_(s));
        result.push_back(setdifference(s));
        result.push_back(setintersection(s));
        result.push_back(setcomplement(s));
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
        if (is_setin_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_setunion__application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_setdifference_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_setintersection_application(e))
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
        if (is_setin_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }
        if (is_setunion__application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }
        if (is_setdifference_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }
        if (is_setintersection_application(e))
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
        if (is_set_comprehension_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_setcomplement_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      /// \brief Give all system defined equations for set_
      /// \param s A sort expression
      /// \return All system defined equations for sort set_
      inline
      data_equation_vector set__generate_equations_code(const sort_expression& s)
      {
        variable vd("d",s);
        variable vs("s",set_(s));
        variable vt("t",set_(s));
        variable vf("f",function_sort(s, sort_bool_::bool_()));
        variable vg("g",function_sort(s, sort_bool_::bool_()));
        variable vx("x",s);

        data_equation_vector result;
        result.push_back(data_equation(make_vector(vf, vg), equal_to(set_comprehension(s, vf), set_comprehension(s, vg)), equal_to(vf, vg)));
        result.push_back(data_equation(variable_list(), emptyset(s), set_comprehension(s, lambda(make_vector(vx), sort_bool_::false_()))));
        result.push_back(data_equation(make_vector(vd, vf), setin(s, vd, set_comprehension(s, vf)), vf(vd)));
        result.push_back(data_equation(make_vector(vf, vg, vx), less_equal(set_comprehension(s, vf), set_comprehension(s, vg)), forall(make_vector(vx), sort_bool_::implies(vf(vx), vg(vx)))));
        result.push_back(data_equation(make_vector(vs, vt), less(vs, vt), sort_bool_::and_(less_equal(vs, vt), not_equal_to(vs, vt))));
        result.push_back(data_equation(make_vector(vf, vg, vx), setunion_(s, set_comprehension(s, vf), set_comprehension(s, vg)), set_comprehension(s, lambda(make_vector(vx), sort_bool_::or_(vf(vx), vg(vx))))));
        result.push_back(data_equation(make_vector(vs, vt), setdifference(s, vs, vt), setintersection(s, vs, setcomplement(s, vt))));
        result.push_back(data_equation(make_vector(vf, vg, vx), setintersection(s, set_comprehension(s, vf), set_comprehension(s, vg)), set_comprehension(s, lambda(make_vector(vx), sort_bool_::and_(vf(vx), vg(vx))))));
        result.push_back(data_equation(make_vector(vf, vx), setcomplement(s, set_comprehension(s, vf)), set_comprehension(s, lambda(make_vector(vx), setcomplement(s, vf(vx))))));
        return result;
      }

      /// \brief Add sort, constructors, mappings and equations for set_
      /// \param specification A specification
      /// \param the sort of elements stored by the container
      inline
      void add_set__to_specification(data_specification& specification, sort_expression const& element)
      {
         specification.add_system_defined_sort(set_(element));
         specification.add_system_defined_constructors(boost::make_iterator_range(set__generate_constructors_code()));
         specification.add_system_defined_mappings(boost::make_iterator_range(set__generate_functions_code(element)));
         specification.add_system_defined_equations(boost::make_iterator_range(set__generate_equations_code(element)));
      }
    } // namespace sort_set_

  } // namespace new_data

} // namespace mcrl2

#endif // MCRL2_NEW_DATA_SET__H
