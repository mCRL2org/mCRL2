#ifndef MCRL2_DATA_BOOL__H
#define MCRL2_DATA_BOOL__H

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/utility.h"

namespace mcrl2 {

  namespace data {

    namespace sort_bool_ {

      // Sort expression Bool
      inline
      basic_sort bool_()
      {
        static basic_sort bool_("Bool");
        return bool_;
      }

      // Recogniser for sort expression Bool
      inline
      bool is_bool_(const sort_expression& e)
      {
        if (e.is_basic_sort())
        {
          return static_cast<const basic_sort&>(e) == bool_();
        }
        return false;
      }

      // Function symbol true
      inline
      function_symbol true_()
      {
        static function_symbol true_("true", bool_());
        return true_;
      }

      // Recogniser for true
      inline
      bool is_true__function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == true_();
        }
        return false;
      }

      // Application of true
      inline
      application true_(const data_expression& arg0)
      {
        assert(is_bool_(arg0.sort()));
        
        return application(true_(),arg0);
      }

      // Recogniser for application of true
      inline
      bool is_true__application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_true__function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol false
      inline
      function_symbol false_()
      {
        static function_symbol false_("false", bool_());
        return false_;
      }

      // Recogniser for false
      inline
      bool is_false__function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == false_();
        }
        return false;
      }

      // Application of false
      inline
      application false_(const data_expression& arg0)
      {
        assert(is_bool_(arg0.sort()));
        
        return application(false_(),arg0);
      }

      // Recogniser for application of false
      inline
      bool is_false__application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_false__function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol ==
      inline
      function_symbol equal_to()
      {
        static function_symbol equal_to("==", function_sort(bool_(), bool_(), bool_()));
        return equal_to;
      }

      // Recogniser for ==
      inline
      bool is_equal_to_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == equal_to();
        }
        return false;
      }

      // Application of ==
      inline
      application equal_to(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_bool_(arg0.sort()));
        assert(is_bool_(arg1.sort()));
        
        return application(equal_to(),arg0, arg1);
      }

      // Recogniser for application of ==
      inline
      bool is_equal_to_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_equal_to_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol !=
      inline
      function_symbol not_equal_to()
      {
        static function_symbol not_equal_to("!=", function_sort(bool_(), bool_(), bool_()));
        return not_equal_to;
      }

      // Recogniser for !=
      inline
      bool is_not_equal_to_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == not_equal_to();
        }
        return false;
      }

      // Application of !=
      inline
      application not_equal_to(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_bool_(arg0.sort()));
        assert(is_bool_(arg1.sort()));
        
        return application(not_equal_to(),arg0, arg1);
      }

      // Recogniser for application of !=
      inline
      bool is_not_equal_to_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_not_equal_to_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol if
      inline
      function_symbol if_()
      {
        static function_symbol if_("if", function_sort(bool_(), bool_(), bool_(), bool_()));
        return if_;
      }

      // Recogniser for if
      inline
      bool is_if__function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == if_();
        }
        return false;
      }

      // Application of if
      inline
      application if_(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        assert(is_bool_(arg0.sort()));
        assert(is_bool_(arg1.sort()));
        assert(is_bool_(arg2.sort()));
        
        return application(if_(),arg0, arg1, arg2);
      }

      // Recogniser for application of if
      inline
      bool is_if__application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_if__function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol !
      inline
      function_symbol not_()
      {
        static function_symbol not_("!", function_sort(bool_(), bool_()));
        return not_;
      }

      // Recogniser for !
      inline
      bool is_not__function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == not_();
        }
        return false;
      }

      // Application of !
      inline
      application not_(const data_expression& arg0)
      {
        assert(is_bool_(arg0.sort()));
        
        return application(not_(),arg0);
      }

      // Recogniser for application of !
      inline
      bool is_not__application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_not__function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol &&
      inline
      function_symbol and_()
      {
        static function_symbol and_("&&", function_sort(bool_(), bool_(), bool_()));
        return and_;
      }

      // Recogniser for &&
      inline
      bool is_and__function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == and_();
        }
        return false;
      }

      // Application of &&
      inline
      application and_(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_bool_(arg0.sort()));
        assert(is_bool_(arg1.sort()));
        
        return application(and_(),arg0, arg1);
      }

      // Recogniser for application of &&
      inline
      bool is_and__application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_and__function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol ||
      inline
      function_symbol or_()
      {
        static function_symbol or_("||", function_sort(bool_(), bool_(), bool_()));
        return or_;
      }

      // Recogniser for ||
      inline
      bool is_or__function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == or_();
        }
        return false;
      }

      // Application of ||
      inline
      application or_(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_bool_(arg0.sort()));
        assert(is_bool_(arg1.sort()));
        
        return application(or_(),arg0, arg1);
      }

      // Recogniser for application of ||
      inline
      bool is_or__application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_or__function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol =>
      inline
      function_symbol implies()
      {
        static function_symbol implies("=>", function_sort(bool_(), bool_(), bool_()));
        return implies;
      }

      // Recogniser for =>
      inline
      bool is_implies_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == implies();
        }
        return false;
      }

      // Application of =>
      inline
      application implies(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_bool_(arg0.sort()));
        assert(is_bool_(arg1.sort()));
        
        return application(implies(),arg0, arg1);
      }

      // Recogniser for application of =>
      inline
      bool is_implies_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_implies_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function for projecting out right
      inline
      data_expression right(const data_expression& e)
      {
        assert(is_equal_to_application(e) || is_not_equal_to_application(e) || is_and__application(e) || is_or__application(e) || is_implies_application(e));
        
        if (is_equal_to_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_not_equal_to_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

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

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out ifcase
      inline
      data_expression ifcase(const data_expression& e)
      {
        assert(is_if__application(e));
        
        if (is_if__application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out arg
      inline
      data_expression arg(const data_expression& e)
      {
        assert(is_not__application(e));
        
        if (is_not__application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out left
      inline
      data_expression left(const data_expression& e)
      {
        assert(is_equal_to_application(e) || is_not_equal_to_application(e) || is_and__application(e) || is_or__application(e) || is_implies_application(e));
        
        if (is_equal_to_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_not_equal_to_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

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

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out condition
      inline
      data_expression condition(const data_expression& e)
      {
        assert(is_if__application(e));
        
        if (is_if__application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out elsecase
      inline
      data_expression elsecase(const data_expression& e)
      {
        assert(is_if__application(e));
        
        if (is_if__application(e))
        {
          return static_cast<const application&>(e).arguments()[2];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Give all system defined equations for bool_
      inline
      data_equation_list bool__generate_equations_code()
      {
        data_equation_list result;
        result.push_back(data_equation(make_vector(variable("b", bool_())), equal_to(variable("b", bool_()), variable("b", bool_())), true_()));
        result.push_back(data_equation(make_vector(variable("b", bool_())), equal_to(true_(), variable("b", bool_())), variable("b", bool_())));
        result.push_back(data_equation(make_vector(variable("b", bool_())), equal_to(false_(), variable("b", bool_())), not_(variable("b", bool_()))));
        result.push_back(data_equation(make_vector(variable("b", bool_())), equal_to(variable("b", bool_()), true_()), variable("b", bool_())));
        result.push_back(data_equation(make_vector(variable("b", bool_())), equal_to(variable("b", bool_()), false_()), not_(variable("b", bool_()))));
        result.push_back(data_equation(make_vector(variable("c", bool_()), variable("b", bool_())), not_equal_to(variable("b", bool_()), variable("c", bool_())), not_(equal_to(variable("b", bool_()), variable("c", bool_())))));
        result.push_back(data_equation(make_vector(variable("c", bool_()), variable("b", bool_())), if_(true_(), variable("b", bool_()), variable("c", bool_())), variable("b", bool_())));
        result.push_back(data_equation(make_vector(variable("c", bool_()), variable("b", bool_())), if_(false_(), variable("b", bool_()), variable("c", bool_())), variable("c", bool_())));
        result.push_back(data_equation(make_vector(variable("c", bool_()), variable("b", bool_())), if_(variable("b", bool_()), variable("c", bool_()), variable("c", bool_())), variable("c", bool_())));
        result.push_back(data_equation(variable_list(), not_(true_()), false_()));
        result.push_back(data_equation(variable_list(), not_(false_()), true_()));
        result.push_back(data_equation(make_vector(variable("b", bool_())), not_(not_(variable("b", bool_()))), variable("b", bool_())));
        result.push_back(data_equation(make_vector(variable("b", bool_())), and_(variable("b", bool_()), true_()), variable("b", bool_())));
        result.push_back(data_equation(make_vector(variable("b", bool_())), and_(variable("b", bool_()), false_()), false_()));
        result.push_back(data_equation(make_vector(variable("b", bool_())), and_(true_(), variable("b", bool_())), variable("b", bool_())));
        result.push_back(data_equation(make_vector(variable("b", bool_())), and_(false_(), variable("b", bool_())), false_()));
        result.push_back(data_equation(make_vector(variable("b", bool_())), or_(variable("b", bool_()), true_()), true_()));
        result.push_back(data_equation(make_vector(variable("b", bool_())), or_(variable("b", bool_()), false_()), variable("b", bool_())));
        result.push_back(data_equation(make_vector(variable("b", bool_())), or_(true_(), variable("b", bool_())), true_()));
        result.push_back(data_equation(make_vector(variable("b", bool_())), or_(false_(), variable("b", bool_())), false_()));
        result.push_back(data_equation(make_vector(variable("b", bool_())), implies(variable("b", bool_()), true_()), true_()));
        result.push_back(data_equation(make_vector(variable("b", bool_())), implies(variable("b", bool_()), false_()), not_(variable("b", bool_()))));
        result.push_back(data_equation(make_vector(variable("b", bool_())), implies(true_(), variable("b", bool_())), variable("b", bool_())));
        result.push_back(data_equation(make_vector(variable("b", bool_())), implies(false_(), variable("b", bool_())), true_()));

        return result;
      }

    } // namespace bool_
  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_BOOL__H
