#ifndef MCRL2_NEW_DATA_FSET_H
#define MCRL2_NEW_DATA_FSET_H

#include "mcrl2/new_data/basic_sort.h"
#include "mcrl2/new_data/function_sort.h"
#include "mcrl2/new_data/function_symbol.h"
#include "mcrl2/new_data/application.h"
#include "mcrl2/new_data/data_equation.h"
#include "mcrl2/new_data/detail/container_utility.h"
#include "mcrl2/new_data/standard.h"
#include "mcrl2/new_data/data_specification.h"
#include "mcrl2/new_data/container_sort.h"
#include "mcrl2/new_data/structured_sort.h"
#include "mcrl2/new_data/bool.h"

namespace mcrl2 {

  namespace new_data {

    /// \brief Namespace for system defined sort fset
    namespace sort_fset {

      /// \brief Constructor for sort expression FSet(S)
      /// \param s A sort expression
      /// \return Sort expression fset(s)
      inline
      container_sort fset(const sort_expression& s)
      {
        container_sort fset("fset", s);
        return fset;
      }

      /// \brief Recogniser for sort expression FSet(s)
      /// \param e A sort expression
      /// \return true iff e is a container sort of which the name matches
      ///      fset
      inline
      bool is_fset(const sort_expression& e)
      {
        if (e.is_container_sort())
        {
          return static_cast<const container_sort&>(e).container_name() == "fset";
        }
        return false;
      }

      namespace detail {

        /// \brief Declaration for sort fset as structured sort
        /// \param s A sort expression
        /// \ret The structured sort representing fset
        inline
        structured_sort fset_struct(const sort_expression& s)
        {
          structured_sort_constructor_vector constructors;
          constructors.push_back(structured_sort_constructor("@fset_empty", "fset_empty"));
          constructors.push_back(structured_sort_constructor("@fset_cons", boost::make_iterator_range(make_vector(structured_sort_constructor_argument("head", s), structured_sort_constructor_argument("tail", fset(s)))), "fset_cons"));
          return structured_sort(constructors);
        }

      } // namespace detail

      /// \brief Constructor for function symbol \@fset_empty
      /// \param s A sort expression
      /// \return Function symbol fset_empty
      inline
      function_symbol fset_empty(const sort_expression& s)
      {
        function_symbol fset_empty("@fset_empty", fset(s));
        return fset_empty;
      }

      /// \brief Recogniser for function \@fset_empty
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_empty
      inline
      bool is_fset_empty_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@fset_empty";
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fset_cons
      /// \param s A sort expression
      /// \return Function symbol fset_cons
      inline
      function_symbol fset_cons(const sort_expression& s)
      {
        function_symbol fset_cons("@fset_cons", function_sort(s, fset(s), fset(s)));
        return fset_cons;
      }

      /// \brief Recogniser for function \@fset_cons
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_cons
      inline
      bool is_fset_cons_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@fset_cons";
        }
        return false;
      }

      /// \brief Application of function symbol \@fset_cons
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@fset_cons to a number of arguments
      inline
      application fset_cons(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(fset_cons(s),arg0, arg1);
      }

      /// \brief Recogniser for application of \@fset_cons
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fset_cons to a
      ///     number of arguments
      inline
      bool is_fset_cons_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fset_cons_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Give all system defined constructors for fset
      /// \param s A sort expression
      /// \return All system defined constructors for fset
      inline
      function_symbol_vector fset_generate_constructors_code(const sort_expression& s)
      {
        function_symbol_vector result;
        function_symbol_vector fset_constructors = detail::fset_struct(s).constructor_functions(fset(s));
        result.insert(result.end(), fset_constructors.begin(), fset_constructors.end());

        return result;
      }
      /// \brief Constructor for function symbol \@fset_insert
      /// \param s A sort expression
      /// \return Function symbol fsetinsert
      inline
      function_symbol fsetinsert(const sort_expression& s)
      {
        function_symbol fsetinsert("@fset_insert", function_sort(s, fset(s), fset(s)));
        return fsetinsert;
      }

      /// \brief Recogniser for function \@fset_insert
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_insert
      inline
      bool is_fsetinsert_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@fset_insert";
        }
        return false;
      }

      /// \brief Application of function symbol \@fset_insert
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@fset_insert to a number of arguments
      inline
      application fsetinsert(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(fsetinsert(s),arg0, arg1);
      }

      /// \brief Recogniser for application of \@fset_insert
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fsetinsert to a
      ///     number of arguments
      inline
      bool is_fsetinsert_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fsetinsert_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fset_cinsert
      /// \param s A sort expression
      /// \return Function symbol fsetcinsert
      inline
      function_symbol fsetcinsert(const sort_expression& s)
      {
        function_symbol fsetcinsert("@fset_cinsert", function_sort(s, sort_bool_::bool_(), fset(s), fset(s)));
        return fsetcinsert;
      }

      /// \brief Recogniser for function \@fset_cinsert
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_cinsert
      inline
      bool is_fsetcinsert_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@fset_cinsert";
        }
        return false;
      }

      /// \brief Application of function symbol \@fset_cinsert
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@fset_cinsert to a number of arguments
      inline
      application fsetcinsert(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(fsetcinsert(s),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@fset_cinsert
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fsetcinsert to a
      ///     number of arguments
      inline
      bool is_fsetcinsert_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fsetcinsert_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fset_in
      /// \param s A sort expression
      /// \return Function symbol fsetin
      inline
      function_symbol fsetin(const sort_expression& s)
      {
        function_symbol fsetin("@fset_in", function_sort(s, fset(s), sort_bool_::bool_()));
        return fsetin;
      }

      /// \brief Recogniser for function \@fset_in
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_in
      inline
      bool is_fsetin_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@fset_in";
        }
        return false;
      }

      /// \brief Application of function symbol \@fset_in
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@fset_in to a number of arguments
      inline
      application fsetin(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        return application(fsetin(s),arg0, arg1);
      }

      /// \brief Recogniser for application of \@fset_in
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fsetin to a
      ///     number of arguments
      inline
      bool is_fsetin_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fsetin_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fset_lte
      /// \param s A sort expression
      /// \return Function symbol fsetlte
      inline
      function_symbol fsetlte(const sort_expression& s)
      {
        function_symbol fsetlte("@fset_lte", function_sort(function_sort(s, sort_bool_::bool_()), fset(s), fset(s), sort_bool_::bool_()));
        return fsetlte;
      }

      /// \brief Recogniser for function \@fset_lte
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_lte
      inline
      bool is_fsetlte_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@fset_lte";
        }
        return false;
      }

      /// \brief Application of function symbol \@fset_lte
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@fset_lte to a number of arguments
      inline
      application fsetlte(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(fsetlte(s),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@fset_lte
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fsetlte to a
      ///     number of arguments
      inline
      bool is_fsetlte_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fsetlte_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fset_union
      /// \param s A sort expression
      /// \return Function symbol fsetunion
      inline
      function_symbol fsetunion(const sort_expression& s)
      {
        function_symbol fsetunion("@fset_union", function_sort(function_sort(s, sort_bool_::bool_()), function_sort(s, sort_bool_::bool_()), fset(s), fset(s), fset(s)));
        return fsetunion;
      }

      /// \brief Recogniser for function \@fset_union
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_union
      inline
      bool is_fsetunion_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@fset_union";
        }
        return false;
      }

      /// \brief Application of function symbol \@fset_union
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of \@fset_union to a number of arguments
      inline
      application fsetunion(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(fsetunion(s),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@fset_union
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fsetunion to a
      ///     number of arguments
      inline
      bool is_fsetunion_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fsetunion_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@fset_inter
      /// \param s A sort expression
      /// \return Function symbol fsetintersection
      inline
      function_symbol fsetintersection(const sort_expression& s)
      {
        function_symbol fsetintersection("@fset_inter", function_sort(function_sort(s, sort_bool_::bool_()), function_sort(s, sort_bool_::bool_()), fset(s), fset(s), fset(s)));
        return fsetintersection;
      }

      /// \brief Recogniser for function \@fset_inter
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@fset_inter
      inline
      bool is_fsetintersection_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@fset_inter";
        }
        return false;
      }

      /// \brief Application of function symbol \@fset_inter
      /// \param s A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of \@fset_inter to a number of arguments
      inline
      application fsetintersection(const sort_expression& s, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(fsetintersection(s),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@fset_inter
      /// \param e A data expression
      /// \return true iff e is an application of function symbol fsetintersection to a
      ///     number of arguments
      inline
      bool is_fsetintersection_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_fsetintersection_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      /// \brief Give all system defined mappings for fset
      /// \param s A sort expression
      /// \return All system defined mappings for fset
      inline
      function_symbol_vector fset_generate_functions_code(const sort_expression& s)
      {
        function_symbol_vector result;
        result.push_back(fsetinsert(s));
        result.push_back(fsetcinsert(s));
        result.push_back(fsetin(s));
        result.push_back(fsetlte(s));
        result.push_back(fsetunion(s));
        result.push_back(fsetintersection(s));
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
        if (is_fset_cons_application(e))
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
        if (is_fsetinsert_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }
        if (is_fsetin_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
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
        if (is_fsetcinsert_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_fsetlte_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_fsetunion_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_fsetintersection_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
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
        if (is_fsetcinsert_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }
        if (is_fsetlte_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }
        if (is_fsetunion_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }
        if (is_fsetintersection_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
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
        if (is_fsetcinsert_application(e))
        {
          return static_cast<const application&>(e).arguments()[2];
        }
        if (is_fsetlte_application(e))
        {
          return static_cast<const application&>(e).arguments()[2];
        }
        if (is_fsetunion_application(e))
        {
          return static_cast<const application&>(e).arguments()[2];
        }
        if (is_fsetintersection_application(e))
        {
          return static_cast<const application&>(e).arguments()[2];
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
        if (is_fsetunion_application(e))
        {
          return static_cast<const application&>(e).arguments()[3];
        }
        if (is_fsetintersection_application(e))
        {
          return static_cast<const application&>(e).arguments()[3];
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
        if (is_fset_cons_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
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
        if (is_fsetinsert_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        if (is_fsetin_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      /// \brief Give all system defined equations for fset
      /// \param s A sort expression
      /// \return All system defined equations for sort fset
      inline
      data_equation_vector fset_generate_equations_code(const sort_expression& s)
      {
        variable vd("d",s);
        variable ve("e",s);
        variable vf("f",function_sort(s, sort_bool_::bool_()));
        variable vg("g",function_sort(s, sort_bool_::bool_()));
        variable vs("s",fset(s));
        variable vt("t",fset(s));

        data_equation_vector result;
        data_equation_vector fset_equations = detail::fset_struct(s).constructor_equations(fset(s));
        result.insert(result.end(), fset_equations.begin(), fset_equations.end());
        result.push_back(data_equation(make_vector(vd), fsetinsert(s, vd, fset_empty(s)), fset_cons(s, vd, fset_empty(s))));
        result.push_back(data_equation(make_vector(vd, vs), fsetinsert(s, vd, fset_cons(s, vd, vs)), fset_cons(s, vd, vs)));
        result.push_back(data_equation(make_vector(vd, ve, vs), less(vd, ve), fsetinsert(s, vd, fset_cons(s, ve, vs)), fset_cons(s, vd, fset_cons(s, ve, vs))));
        result.push_back(data_equation(make_vector(vd, ve, vs), less(ve, vd), fsetinsert(s, vd, fset_cons(s, ve, vs)), fset_cons(s, ve, fsetinsert(s, vd, vs))));
        result.push_back(data_equation(make_vector(vd, vs), fsetcinsert(s, vd, sort_bool_::false_(), vs), vs));
        result.push_back(data_equation(make_vector(vd, vs), fsetcinsert(s, vd, sort_bool_::true_(), vs), fsetinsert(s, vd, vs)));
        result.push_back(data_equation(make_vector(vd), fsetin(s, vd, fset_empty(s)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vd, vs), fsetin(s, vd, fset_cons(s, vd, vs)), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(vd, ve, vs), less(vd, ve), fsetin(s, vd, fset_cons(s, ve, vs)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vd, ve, vs), less(ve, vd), fsetin(s, vd, fset_cons(s, ve, vs)), fsetin(s, vd, vs)));
        result.push_back(data_equation(make_vector(vf), fsetlte(s, vf, fset_empty(s), fset_empty(s)), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(vd, vf, vs), fsetlte(s, vf, fset_cons(s, vd, vs), fset_empty(s)), sort_bool_::and_(vf(vd), fsetlte(s, vf, vs, fset_empty(s)))));
        result.push_back(data_equation(make_vector(ve, vf, vt), fsetlte(s, vf, fset_empty(s), fset_cons(s, ve, vt)), sort_bool_::and_(sort_bool_::not_(vf(ve)), fsetlte(s, vf, fset_empty(s), vt))));
        result.push_back(data_equation(make_vector(vd, vf, vs, vt), fsetlte(s, vf, fset_cons(s, vd, vs), fset_cons(s, vd, vt)), fsetlte(s, vf, vs, vt)));
        result.push_back(data_equation(make_vector(vd, ve, vf, vs, vt), less(vd, ve), fsetlte(s, vf, fset_cons(s, vd, vs), fset_cons(s, ve, vt)), sort_bool_::and_(vf(vd), fsetlte(s, vf, vs, fset_cons(s, ve, vt)))));
        result.push_back(data_equation(make_vector(vd, ve, vf, vs, vt), less(ve, vd), fsetlte(s, vf, fset_cons(s, vd, vs), fset_cons(s, ve, vt)), sort_bool_::and_(sort_bool_::not_(vf(ve)), fsetlte(s, vf, fset_cons(s, vd, vs), vt))));
        result.push_back(data_equation(make_vector(vf, vg), fsetunion(s, vf, vg, fset_empty(s), fset_empty(s)), fset_empty(s)));
        result.push_back(data_equation(make_vector(vd, vf, vg, vs), fsetunion(s, vf, vg, fset_cons(s, vd, vs), fset_empty(s)), fsetcinsert(s, vd, sort_bool_::not_(vg(vd)), fsetunion(s, vf, vg, vs, fset_empty(s)))));
        result.push_back(data_equation(make_vector(ve, vf, vg, vt), fsetunion(s, vf, vg, fset_empty(s), fset_cons(s, ve, vt)), fsetcinsert(s, ve, sort_bool_::not_(vf(ve)), fsetunion(s, vf, vg, fset_empty(s), vt))));
        result.push_back(data_equation(make_vector(vd, vf, vg, vs, vt), fsetunion(s, vf, vg, fset_cons(s, vd, vs), fset_cons(s, vd, vt)), fsetcinsert(s, vd, equal_to(vf(vd), vg(vd)), fsetunion(s, vf, vg, vs, vt))));
        result.push_back(data_equation(make_vector(vd, ve, vf, vg, vs, vt), less(vd, ve), fsetunion(s, vf, vg, fset_cons(s, vd, vs), fset_cons(s, ve, vt)), fsetcinsert(s, vd, sort_bool_::not_(vg(vd)), fsetunion(s, vf, vg, vs, fset_cons(s, ve, vt)))));
        result.push_back(data_equation(make_vector(vd, ve, vf, vg, vs, vt), less(ve, vd), fsetunion(s, vf, vg, fset_cons(s, vd, vs), fset_cons(s, ve, vt)), fsetcinsert(s, ve, sort_bool_::not_(vf(ve)), fsetunion(s, vf, vg, fset_cons(s, vd, vs), vt))));
        result.push_back(data_equation(make_vector(vf, vg), fsetintersection(s, vf, vg, fset_empty(s), fset_empty(s)), fset_empty(s)));
        result.push_back(data_equation(make_vector(vd, vf, vg, vs), fsetintersection(s, vf, vg, fset_cons(s, vd, vs), fset_empty(s)), fsetcinsert(s, vd, vg(vd), fsetintersection(s, vf, vg, vs, fset_empty(s)))));
        result.push_back(data_equation(make_vector(ve, vf, vg, vt), fsetintersection(s, vf, vg, fset_empty(s), fset_cons(s, ve, vt)), fsetcinsert(s, ve, vf(ve), fsetintersection(s, vf, vg, fset_empty(s), vt))));
        result.push_back(data_equation(make_vector(vd, vf, vg, vs, vt), fsetintersection(s, vf, vg, fset_cons(s, vd, vs), fset_cons(s, vd, vt)), fsetcinsert(s, vd, equal_to(vf(vd), vg(vd)), fsetintersection(s, vf, vg, vs, vt))));
        result.push_back(data_equation(make_vector(vd, ve, vf, vg, vs, vt), less(vd, ve), fsetintersection(s, vf, vg, fset_cons(s, vd, vs), fset_cons(s, ve, vt)), fsetcinsert(s, vd, vg(vd), fsetintersection(s, vf, vg, vs, fset_cons(s, ve, vt)))));
        result.push_back(data_equation(make_vector(vd, ve, vf, vg, vs, vt), less(ve, vd), fsetintersection(s, vf, vg, fset_cons(s, vd, vs), fset_cons(s, ve, vt)), fsetcinsert(s, ve, vf(ve), fsetintersection(s, vf, vg, fset_cons(s, vd, vs), vt))));
        result.push_back(data_equation(variable_list(), equal_to(fset_empty(s), fset_empty(s)), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(ve, vs), equal_to(fset_empty(s), fset_cons(s, ve, vs)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(ve, vs), equal_to(fset_cons(s, ve, vs), fset_empty(s)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vd, ve, vs, vt), equal_to(fset_cons(s, ve, vt), fset_cons(s, vd, vs)), sort_bool_::and_(equal_to(ve, vd), equal_to(vt, vs))));
        result.push_back(data_equation(variable_list(), less(fset_empty(s), fset_empty(s)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(ve, vs), less(fset_empty(s), fset_cons(s, ve, vs)), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(ve, vs), less(fset_cons(s, ve, vs), fset_empty(s)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vd, ve, vs, vt), less(fset_cons(s, ve, vt), fset_cons(s, vd, vs)), sort_bool_::or_(less(ve, vd), sort_bool_::and_(equal_to(ve, vd), less(vt, vs)))));
        result.push_back(data_equation(variable_list(), less_equal(fset_empty(s), fset_empty(s)), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(ve, vs), less_equal(fset_empty(s), fset_cons(s, ve, vs)), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(ve, vs), less_equal(fset_cons(s, ve, vs), fset_empty(s)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vd, ve, vs, vt), less_equal(fset_cons(s, ve, vt), fset_cons(s, vd, vs)), sort_bool_::or_(less(ve, vd), sort_bool_::and_(equal_to(ve, vd), less_equal(vt, vs)))));
        return result;
      }

      /// \brief Add sort, constructors, mappings and equations for fset
      /// \param specification a specification
      /// \param element the sort of elements stored by the container
      inline
      void add_fset_to_specification(data_specification& specification, sort_expression const& element)
      {
         specification.add_system_defined_sort(fset(element));
         specification.add_system_defined_constructors(boost::make_iterator_range(fset_generate_constructors_code(element)));
         specification.add_system_defined_mappings(boost::make_iterator_range(fset_generate_functions_code(element)));
         specification.add_system_defined_equations(boost::make_iterator_range(fset_generate_equations_code(element)));

         sort_bool_::add_bool__to_specification(specification);
      }
    } // namespace sort_fset

  } // namespace new_data

} // namespace mcrl2

#endif // MCRL2_NEW_DATA_FSET_H
