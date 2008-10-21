#ifndef MCRL2_DATA_LIST_H
#define MCRL2_DATA_LIST_H

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/container_sort.h"


namespace mcrl2 {

  namespace data {

    namespace sort_list {

      // Sort expression List(s)
      inline
      container_sort list(const sort_expression& s)
      {
        static container_sort list("list", s);
        return list;
      }

      // Recogniser for sort expression List(s)
      inline
      bool is_list(const sort_expression& e)
      {
        if (e.is_container_sort())
        {
          return static_cast<const container_sort&>(e).container_name() == "list";
        }
        return false;
      }

      // Function symbol []
      inline
      function_symbol nil(const sort_expression& s)
      {
        static function_symbol nil("[]", sort_list::list(s));
        return nil;
      }

      // Recogniser for []
      inline
      bool is_nil_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "[]";
        }
        return false;
      }

      // Function symbol |>
      inline
      function_symbol cons_(const sort_expression& s)
      {
        static function_symbol cons_("|>", function_sort(s, sort_list::list(s), sort_list::list(s)));
        return cons_;
      }

      // Recogniser for |>
      inline
      bool is_cons__function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "|>";
        }
        return false;
      }

      // Application of |>
      inline
      application cons_(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        assert(arg0.sort() == s);
        assert(sort_list::is_list(arg1.sort()));
        
        return application(cons_(s),arg0, arg1);
      }

      // Recogniser for application of |>
      inline
      bool is_cons__application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_cons__function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol in
      inline
      function_symbol in(const sort_expression& s)
      {
        static function_symbol in("in", function_sort(s, sort_list::list(s), sort_bool_::bool_()));
        return in;
      }

      // Recogniser for in
      inline
      bool is_in_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "in";
        }
        return false;
      }

      // Application of in
      inline
      application in(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        assert(arg0.sort() == s);
        assert(sort_list::is_list(arg1.sort()));
        
        return application(in(s),arg0, arg1);
      }

      // Recogniser for application of in
      inline
      bool is_in_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_in_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol cnt
      inline
      function_symbol count(const sort_expression& s)
      {
        static function_symbol count("cnt", function_sort(sort_list::list(s), sort_nat::nat()));
        return count;
      }

      // Recogniser for cnt
      inline
      bool is_count_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "cnt";
        }
        return false;
      }

      // Application of cnt
      inline
      application count(const sort_expression& s, const data_expression& arg0)
      {
        assert(sort_list::is_list(arg0.sort()));
        
        return application(count(s),arg0);
      }

      // Recogniser for application of cnt
      inline
      bool is_count_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_count_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol <|
      inline
      function_symbol snoc(const sort_expression& s)
      {
        static function_symbol snoc("<|", function_sort(sort_list::list(s), s, sort_list::list(s)));
        return snoc;
      }

      // Recogniser for <|
      inline
      bool is_snoc_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "<|";
        }
        return false;
      }

      // Application of <|
      inline
      application snoc(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        assert(sort_list::is_list(arg0.sort()));
        assert(arg1.sort() == s);
        
        return application(snoc(s),arg0, arg1);
      }

      // Recogniser for application of <|
      inline
      bool is_snoc_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_snoc_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol ++
      inline
      function_symbol concat(const sort_expression& s)
      {
        static function_symbol concat("++", function_sort(sort_list::list(s), sort_list::list(s), sort_list::list(s)));
        return concat;
      }

      // Recogniser for ++
      inline
      bool is_concat_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "++";
        }
        return false;
      }

      // Application of ++
      inline
      application concat(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        assert(sort_list::is_list(arg0.sort()));
        assert(sort_list::is_list(arg1.sort()));
        
        return application(concat(s),arg0, arg1);
      }

      // Recogniser for application of ++
      inline
      bool is_concat_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_concat_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol .
      inline
      function_symbol element_at(const sort_expression& s)
      {
        static function_symbol element_at(".", function_sort(sort_list::list(s), sort_nat::nat(), s));
        return element_at;
      }

      // Recogniser for .
      inline
      bool is_element_at_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == ".";
        }
        return false;
      }

      // Application of .
      inline
      application element_at(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        assert(sort_list::is_list(arg0.sort()));
        assert(sort_nat::is_nat(arg1.sort()));
        
        return application(element_at(s),arg0, arg1);
      }

      // Recogniser for application of .
      inline
      bool is_element_at_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_element_at_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol head
      inline
      function_symbol head(const sort_expression& s)
      {
        static function_symbol head("head", function_sort(sort_list::list(s), s));
        return head;
      }

      // Recogniser for head
      inline
      bool is_head_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "head";
        }
        return false;
      }

      // Application of head
      inline
      application head(const sort_expression& s, const data_expression& arg0)
      {
        assert(sort_list::is_list(arg0.sort()));
        
        return application(head(s),arg0);
      }

      // Recogniser for application of head
      inline
      bool is_head_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_head_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol tail
      inline
      function_symbol tail(const sort_expression& s)
      {
        static function_symbol tail("tail", function_sort(sort_list::list(s), sort_list::list(s)));
        return tail;
      }

      // Recogniser for tail
      inline
      bool is_tail_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "tail";
        }
        return false;
      }

      // Application of tail
      inline
      application tail(const sort_expression& s, const data_expression& arg0)
      {
        assert(sort_list::is_list(arg0.sort()));
        
        return application(tail(s),arg0);
      }

      // Recogniser for application of tail
      inline
      bool is_tail_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_tail_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol rhead
      inline
      function_symbol rhead(const sort_expression& s)
      {
        static function_symbol rhead("rhead", function_sort(sort_list::list(s), s));
        return rhead;
      }

      // Recogniser for rhead
      inline
      bool is_rhead_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "rhead";
        }
        return false;
      }

      // Application of rhead
      inline
      application rhead(const sort_expression& s, const data_expression& arg0)
      {
        assert(sort_list::is_list(arg0.sort()));
        
        return application(rhead(s),arg0);
      }

      // Recogniser for application of rhead
      inline
      bool is_rhead_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_rhead_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol rtail
      inline
      function_symbol rtail(const sort_expression& s)
      {
        static function_symbol rtail("rtail", function_sort(sort_list::list(s), sort_list::list(s)));
        return rtail;
      }

      // Recogniser for rtail
      inline
      bool is_rtail_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "rtail";
        }
        return false;
      }

      // Application of rtail
      inline
      application rtail(const sort_expression& s, const data_expression& arg0)
      {
        assert(sort_list::is_list(arg0.sort()));
        
        return application(rtail(s),arg0);
      }

      // Recogniser for application of rtail
      inline
      bool is_rtail_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_rtail_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function for projecting out head
      inline
      data_expression head(const data_expression& e)
      {
        assert(is_cons__application(e));
        
        if (is_cons__application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out right
      inline
      data_expression right(const data_expression& e)
      {
        assert(is_concat_application(e));
        
        if (is_concat_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out arg1
      inline
      data_expression arg1(const data_expression& e)
      {
        assert(is_in_application(e));
        
        if (is_in_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out arg2
      inline
      data_expression arg2(const data_expression& e)
      {
        assert(is_in_application(e));
        
        if (is_in_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out list
      inline
      data_expression list(const data_expression& e)
      {
        assert(is_count_application(e) || is_element_at_application(e) || is_head_application(e) || is_tail_application(e) || is_rhead_application(e) || is_rtail_application(e));
        
        if (is_count_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_element_at_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_head_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_tail_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_rhead_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_rtail_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out tail
      inline
      data_expression tail(const data_expression& e)
      {
        assert(is_cons__application(e));
        
        if (is_cons__application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out rhead
      inline
      data_expression rhead(const data_expression& e)
      {
        assert(is_snoc_application(e));
        
        if (is_snoc_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out position
      inline
      data_expression position(const data_expression& e)
      {
        assert(is_element_at_application(e));
        
        if (is_element_at_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out rtail
      inline
      data_expression rtail(const data_expression& e)
      {
        assert(is_snoc_application(e));
        
        if (is_snoc_application(e))
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
        assert(is_concat_application(e));
        
        if (is_concat_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Give all system defined equations for List
      inline
      data_equation_list list_generate_equations_code(const sort_expression& s)
      {
        data_equation_list result;
        result.push_back(data_equation(make_vector(variable("s", sort_list::list(s)), variable("d", s)), sort_bool_::true_(), sort_bool_::equal_to(sort_list::nil(), sort_list::cons_(s, variable("d", s), variable("s", sort_list::list(s)))), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("s", sort_list::list(s)), variable("d", s)), sort_bool_::true_(), sort_bool_::equal_to(sort_list::cons_(s, variable("d", s), variable("s", sort_list::list(s))), sort_list::nil()), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("e", s), variable("d", s), variable("s", sort_list::list(s)), variable("t", sort_list::list(s))), sort_bool_::true_(), sort_bool_::equal_to(sort_list::cons_(s, variable("d", s), variable("s", sort_list::list(s))), sort_list::cons_(s, variable("e", s), variable("t", sort_list::list(s)))), sort_bool_::and_(sort_bool_::equal_to(variable("d", s), variable("e", s)), sort_bool_::equal_to(variable("s", sort_list::list(s)), variable("t", sort_list::list(s))))));
        result.push_back(data_equation(make_vector(variable("d", s)), sort_bool_::true_(), sort_list::in(s, variable("d", s), sort_list::nil()), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("s", sort_list::list(s)), variable("e", s), variable("d", s)), sort_bool_::true_(), sort_list::in(s, variable("d", s), sort_list::cons_(s, variable("e", s), variable("s", sort_list::list(s)))), sort_bool_::or_(sort_bool_::equal_to(variable("d", s), variable("e", s)), sort_list::in(s, variable("d", s), variable("s", sort_list::list(s))))));
        result.push_back(data_equation(variable_list(), sort_bool_::true_(), sort_list::count(s, sort_list::nil()), sort_nat::c0()));
        result.push_back(data_equation(make_vector(variable("s", sort_list::list(s)), variable("d", s)), sort_bool_::true_(), sort_list::count(s, sort_list::cons_(s, variable("d", s), variable("s", sort_list::list(s)))), sort_nat::cnat(sort_pos::succ(sort_list::count(s, variable("s", sort_list::list(s)))))));
        result.push_back(data_equation(make_vector(variable("d", s)), sort_bool_::true_(), sort_list::snoc(s, sort_list::nil(), variable("d", s)), sort_list::cons_(s, variable("d", s), sort_list::nil())));
        result.push_back(data_equation(make_vector(variable("s", sort_list::list(s)), variable("e", s), variable("d", s)), sort_bool_::true_(), sort_list::snoc(s, sort_list::cons_(s, variable("d", s), variable("s", sort_list::list(s))), variable("e", s)), sort_list::cons_(s, variable("d", s), sort_list::snoc(s, variable("s", sort_list::list(s)), variable("e", s)))));
        result.push_back(data_equation(make_vector(variable("s", sort_list::list(s))), sort_bool_::true_(), sort_list::concat(s, sort_list::nil(), variable("s", sort_list::list(s))), variable("s", sort_list::list(s))));
        result.push_back(data_equation(make_vector(variable("s", sort_list::list(s)), variable("d", s), variable("t", sort_list::list(s))), sort_bool_::true_(), sort_list::concat(s, sort_list::cons_(s, variable("d", s), variable("s", sort_list::list(s))), variable("t", sort_list::list(s))), sort_list::cons_(s, variable("d", s), sort_list::concat(s, variable("s", sort_list::list(s)), variable("t", sort_list::list(s))))));
        result.push_back(data_equation(make_vector(variable("s", sort_list::list(s))), sort_bool_::true_(), sort_list::concat(s, variable("s", sort_list::list(s)), sort_list::nil()), variable("s", sort_list::list(s))));
        result.push_back(data_equation(make_vector(variable("s", sort_list::list(s)), variable("d", s)), sort_bool_::true_(), sort_list::element_at(s, sort_list::cons_(s, variable("d", s), variable("s", sort_list::list(s))), sort_nat::c0()), variable("d", s)));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("s", sort_list::list(s)), variable("d", s)), sort_bool_::true_(), sort_list::element_at(s, sort_list::cons_(s, variable("d", s), variable("s", sort_list::list(s))), sort_nat::cnat(variable("p", sort_pos::pos()))), sort_list::element_at(s, variable("s", sort_list::list(s)), sort_nat::pred(variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("s", sort_list::list(s)), variable("d", s)), sort_bool_::true_(), sort_list::head(s, sort_list::cons_(s, variable("d", s), variable("s", sort_list::list(s)))), variable("d", s)));
        result.push_back(data_equation(make_vector(variable("s", sort_list::list(s)), variable("d", s)), sort_bool_::true_(), sort_list::tail(s, sort_list::cons_(s, variable("d", s), variable("s", sort_list::list(s)))), variable("s", sort_list::list(s))));
        result.push_back(data_equation(make_vector(variable("d", s)), sort_bool_::true_(), sort_list::rhead(s, sort_list::cons_(s, variable("d", s), sort_list::nil())), variable("d", s)));
        result.push_back(data_equation(make_vector(variable("s", sort_list::list(s)), variable("e", s), variable("d", s)), sort_bool_::true_(), sort_list::rhead(s, sort_list::cons_(s, variable("d", s), sort_list::cons_(s, variable("e", s), variable("s", sort_list::list(s))))), sort_list::rhead(s, sort_list::cons_(s, variable("e", s), variable("s", sort_list::list(s))))));
        result.push_back(data_equation(make_vector(variable("d", s)), sort_bool_::true_(), sort_list::rtail(s, sort_list::cons_(s, variable("d", s), sort_list::nil())), sort_list::nil()));
        result.push_back(data_equation(make_vector(variable("s", sort_list::list(s)), variable("e", s), variable("d", s)), sort_bool_::true_(), sort_list::rtail(s, sort_list::cons_(s, variable("d", s), sort_list::cons_(s, variable("e", s), variable("s", sort_list::list(s))))), sort_list::cons_(s, variable("d", s), sort_list::rtail(s, sort_list::cons_(s, variable("e", s), variable("s", sort_list::list(s)))))));

        return result;
      }

    } // namespace list
  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_LIST_H
