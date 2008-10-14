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
          return static_cast<const container_sort&>(e).name() == "list";
        }
        return false;
      }

      // Function symbol []
      inline
      function_symbol nil(const sort_expression& s)
      {
        static function_symbol nil("[]", list(s));
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
        static function_symbol cons_("|>", function_sort(s, list(s), list(s)));
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
        assert(is_list(arg1.sort()));
        
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
        static function_symbol in("in", function_sort(s, list(s), bool_()));
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
        assert(is_list(arg1.sort()));
        
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
        static function_symbol count("cnt", function_sort(list(s), nat()));
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
        assert(is_list(arg0.sort()));
        
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
        static function_symbol snoc("<|", function_sort(list(s), s, list(s)));
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
        assert(is_list(arg0.sort()));
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
        static function_symbol concat("++", function_sort(list(s), list(s), list(s)));
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
        assert(is_list(arg0.sort()));
        assert(is_list(arg1.sort()));
        
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
        static function_symbol element_at(".", function_sort(list(s), nat(), s));
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
        assert(is_list(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
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
        static function_symbol head("head", function_sort(list(s), s));
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
        assert(is_list(arg0.sort()));
        
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
        static function_symbol tail("tail", function_sort(list(s), list(s)));
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
        assert(is_list(arg0.sort()));
        
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
        static function_symbol rhead("rhead", function_sort(list(s), s));
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
        assert(is_list(arg0.sort()));
        
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
        static function_symbol rtail("rtail", function_sort(list(s), list(s)));
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
        assert(is_list(arg0.sort()));
        
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
        result.push_back(data_equation(make_vector(variable("s", list(s)), variable("d", s)), true_(), equal_to(nil(), cons_(variable("d", s), variable("s", list(s)))), false_()));
        result.push_back(data_equation(make_vector(variable("s", list(s)), variable("d", s)), true_(), equal_to(cons_(variable("d", s), variable("s", list(s))), nil()), false_()));
        result.push_back(data_equation(make_vector(variable("e", s), variable("d", s), variable("s", list(s)), variable("t", list(s))), true_(), equal_to(cons_(variable("d", s), variable("s", list(s))), cons_(variable("e", s), variable("t", list(s)))), and_(equal_to(variable("d", s), variable("e", s)), equal_to(variable("s", list(s)), variable("t", list(s))))));
        result.push_back(data_equation(make_vector(variable("d", s)), true_(), in(variable("d", s), nil()), false_()));
        result.push_back(data_equation(make_vector(variable("s", list(s)), variable("e", s), variable("d", s)), true_(), in(variable("d", s), cons_(variable("e", s), variable("s", list(s)))), or_(equal_to(variable("d", s), variable("e", s)), in(variable("d", s), variable("s", list(s))))));
        result.push_back(data_equation(variable_list(), true_(), count(nil()), c0()));
        result.push_back(data_equation(make_vector(variable("s", list(s)), variable("d", s)), true_(), count(cons_(variable("d", s), variable("s", list(s)))), cnat(succ(count(variable("s", list(s)))))));
        result.push_back(data_equation(make_vector(variable("d", s)), true_(), snoc(nil(), variable("d", s)), cons_(variable("d", s), nil())));
        result.push_back(data_equation(make_vector(variable("s", list(s)), variable("e", s), variable("d", s)), true_(), snoc(cons_(variable("d", s), variable("s", list(s))), variable("e", s)), cons_(variable("d", s), snoc(variable("s", list(s)), variable("e", s)))));
        result.push_back(data_equation(make_vector(variable("s", list(s))), true_(), concat(nil(), variable("s", list(s))), variable("s", list(s))));
        result.push_back(data_equation(make_vector(variable("s", list(s)), variable("d", s), variable("t", list(s))), true_(), concat(cons_(variable("d", s), variable("s", list(s))), variable("t", list(s))), cons_(variable("d", s), concat(variable("s", list(s)), variable("t", list(s))))));
        result.push_back(data_equation(make_vector(variable("s", list(s))), true_(), concat(variable("s", list(s)), nil()), variable("s", list(s))));
        result.push_back(data_equation(make_vector(variable("s", list(s)), variable("d", s)), true_(), element_at(cons_(variable("d", s), variable("s", list(s))), c0()), variable("d", s)));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("s", list(s)), variable("d", s)), true_(), element_at(cons_(variable("d", s), variable("s", list(s))), cnat(variable("p", pos()))), element_at(variable("s", list(s)), pred(variable("p", pos())))));
        result.push_back(data_equation(make_vector(variable("s", list(s)), variable("d", s)), true_(), head(cons_(variable("d", s), variable("s", list(s)))), variable("d", s)));
        result.push_back(data_equation(make_vector(variable("s", list(s)), variable("d", s)), true_(), tail(cons_(variable("d", s), variable("s", list(s)))), variable("s", list(s))));
        result.push_back(data_equation(make_vector(variable("d", s)), true_(), rhead(cons_(variable("d", s), nil())), variable("d", s)));
        result.push_back(data_equation(make_vector(variable("s", list(s)), variable("e", s), variable("d", s)), true_(), rhead(cons_(variable("d", s), cons_(variable("e", s), variable("s", list(s))))), rhead(cons_(variable("e", s), variable("s", list(s))))));
        result.push_back(data_equation(make_vector(variable("d", s)), true_(), rtail(cons_(variable("d", s), nil())), nil()));
        result.push_back(data_equation(make_vector(variable("s", list(s)), variable("e", s), variable("d", s)), true_(), rtail(cons_(variable("d", s), cons_(variable("e", s), variable("s", list(s))))), cons_(variable("d", s), rtail(cons_(variable("e", s), variable("s", list(s)))))));

        return result;
      }

    } // namespace list
  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_LIST_H
