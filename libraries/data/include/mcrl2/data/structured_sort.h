// Author(s): Jeroen Keiren, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/structured_sort.h
/// \brief The class structured_sort.

#ifndef MCRL2_DATA_STRUCTURED_SORT_H
#define MCRL2_DATA_STRUCTURED_SORT_H

#include <string>
#include <iterator>
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/structured_sort_constructor.h"
#include "mcrl2/data/variable.h"

namespace mcrl2
{

namespace data
{

/// \cond INTERNAL_DOCS
// declare for friendship
namespace sort_fset
{
function_symbol_vector fset_generate_constructors_code(const sort_expression&);
data_equation_vector fset_generate_equations_code(const sort_expression&);
}

// declare for friendship
namespace sort_fbag
{
function_symbol_vector fbag_generate_constructors_code(const sort_expression&);
data_equation_vector fbag_generate_equations_code(const sort_expression&);
}
/// \endcond

/// \brief structured sort.
///
/// A structured sort is a sort with the following structure:
///  struct c1(pr1,1:S1,1, ..., pr1,k1:S1,k1)?is_c1
///       | ...
///       | cn(prn,1:Sn,1, ..., prn,kn:Sn,kn)?is_cn
/// in this:
/// * c1, ..., cn are called constructors.
/// * pri,j are the projection functions (or constructor arguments).
/// * is_ci are the recognisers.
//--- start generated class structured_sort ---//
/// \brief A structured sort
class structured_sort: public sort_expression
{
  public:
    /// \brief Default constructor.
    structured_sort()
      : sort_expression(core::detail::constructSortStruct())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit structured_sort(const atermpp::aterm& term)
      : sort_expression(term)
    {
      assert(core::detail::check_term_SortStruct(*this));
    }

    /// \brief Constructor.
    structured_sort(const structured_sort_constructor_list& constructors)
      : sort_expression(core::detail::gsMakeSortStruct(constructors))
    {}

    /// \brief Constructor.
    template <typename Container>
    structured_sort(const Container& constructors, typename atermpp::detail::enable_if_container<Container, structured_sort_constructor>::type* = 0)
      : sort_expression(core::detail::gsMakeSortStruct(structured_sort_constructor_list(constructors.begin(), constructors.end())))
    {}

    const structured_sort_constructor_list& constructors() const
    {
      return atermpp::aterm_cast<const structured_sort_constructor_list>(atermpp::list_arg1(*this));
    }
//--- start user section structured_sort ---//

    friend class data_specification;

  private:

    friend function_symbol_vector sort_fset::fset_generate_constructors_code(const sort_expression&);
    friend data_equation_vector sort_fset::fset_generate_equations_code(const sort_expression&);
    friend function_symbol_vector sort_fbag::fbag_generate_constructors_code(const sort_expression&);
    friend data_equation_vector sort_fbag::fbag_generate_equations_code(const sort_expression&);

    static bool has_recogniser(structured_sort_constructor const& s)
    {
      return !s.recogniser().empty();
    }

    function_symbol_vector constructor_functions(const sort_expression& s) const
    {
      function_symbol_vector result;
      for (structured_sort_constructor_list::const_iterator i = constructors().begin(); i != constructors().end(); ++i)
      {
        result.push_back(i->constructor_function(s));
      }
      return result;
    }

    function_symbol_vector projection_functions(const sort_expression& s) const
    {
      function_symbol_vector result;
      for (structured_sort_constructor_list::const_iterator i = constructors().begin(); i != constructors().end(); ++i)
      {
        function_symbol_vector projections(i->projection_functions(s));

        for (function_symbol_vector::const_iterator j = projections.begin(); j != projections.end(); ++j)
        {
          result.push_back(*j);
        }
      }
      return result;
    }

    function_symbol_vector recogniser_functions(const sort_expression& s) const
    {
      function_symbol_vector result;
      for (structured_sort_constructor_list::const_iterator i = constructors().begin(); i != constructors().end(); ++i)
      {
        if (i->recogniser() != core::empty_identifier_string())
        {
          result.push_back(i->recogniser_function(s));
        }
      }
      return result;
    }

    data_equation_vector constructor_equations(const sort_expression& s) const
    {
      data_equation_vector result;

      structured_sort_constructor_vector cl(boost::copy_range< structured_sort_constructor_vector >(constructors()));

      for (structured_sort_constructor_vector::const_iterator i = cl.begin(); i != cl.end(); ++i)
      {
        for (structured_sort_constructor_vector::const_iterator j = cl.begin(); j != cl.end(); ++j)
        {
          data_expression right_equal         = (i == j) ? sort_bool::true_() : sort_bool::false_();
          data_expression right_smaller       = (i < j)  ? sort_bool::true_() : sort_bool::false_();
          data_expression right_smaller_equal = (i <= j) ? sort_bool::true_() : sort_bool::false_();

          if (i->arguments().empty() && j->arguments().empty())
          {
            data_expression operand_left  = i->constructor_function(s);
            data_expression operand_right = j->constructor_function(s);

            result.push_back(data_equation(equal_to(operand_left, operand_right), right_equal));
            result.push_back(data_equation(less(operand_left, operand_right), right_smaller));
            result.push_back(data_equation(less_equal(operand_left, operand_right), right_smaller_equal));
          }
          else   // at least one constructor takes arguments
          {
            data_expression operand_left;
            data_expression operand_right;

            set_identifier_generator generator;

            // Create variables for equation
            std::vector< variable > variables;

            if (i->arguments().empty())
            {
              operand_left = i->constructor_function(s);
            }
            else
            {
              structured_sort_constructor_argument_list i_arguments = i->arguments();
              for (structured_sort_constructor_argument_list::const_iterator k = i_arguments.begin(); k != i_arguments.end(); ++k)
              {
                variables.push_back(variable(generator("v"), k->sort()));
              }

              // create first operand of ==, < or <=
              operand_left = application(i->constructor_function(s), variables);
            }

            if (j->arguments().empty())
            {
              operand_right = j->constructor_function(s);
            }
            else
            {
              structured_sort_constructor_argument_list j_arguments = j->arguments();
              for (structured_sort_constructor_argument_list::const_iterator k = j_arguments.begin(); k != j_arguments.end(); ++k)
              {
                variables.push_back(variable(generator("v"), k->sort()));
              }

              // create second operand of ==, < or <=
              operand_right = application(j->constructor_function(s),
                                          boost::make_iterator_range(boost::next(variables.begin(), boost::distance(i->arguments())), variables.end()));
            }

            if (i == j)
            {
              // constructors are the same
              variable_vector::const_reverse_iterator k(variables.rbegin() + boost::distance(i->arguments()));
              variable_vector::const_reverse_iterator l(variables.rbegin());
              variable_vector::const_reverse_iterator end(variables.rend());

              right_equal         = equal_to(*k, *l);
              right_smaller       = less(*k, *l);
              right_smaller_equal = less_equal(*k, *l);

              for (++l, ++k; k != end; ++l, ++k)
              {
                // Constructors have one or more arguments:
                // - rhs for c(x0,...,xn) == c(y0,..,yn):
                //     x0 == y0 && ... && xn == yn
                right_equal         = sort_bool::and_(equal_to(*k, *l), right_equal);
                // - rhs for c(x0,...,xn) < c(y0,..,yn):
                //     x0 < y0                                                     , when n = 0
                //     x0 < y0 || (x0 == y0 && x1 < y1)                            , when n = 1
                //     x0 < y0 || (x0 == y0 && (x1 < y1 || (x1 == y1 && x2 < y2))) , when n = 2
                //     etcetera
                right_smaller       = sort_bool::or_(less(*k, *l),
                                                     sort_bool::and_(equal_to(*k, *l), right_smaller));
                // - rhs for c(x0,...,xn) <= c(y0,..,yn):
                //     x0 <= y0                                                    , when n = 0
                //     x0 < y0 || (x0 == y0 && x1 <= y1)                           , when n = 1
                //     x0 < y0 || (x0 == y0 && (x1 < y1 || (x1 == y1 && x2 <= y2))), when n = 2
                //     etcetera
                right_smaller_equal = sort_bool::or_(less(*k, *l),
                                                     sort_bool::and_(equal_to(*k, *l), right_smaller_equal));
              }
            }

            result.push_back(data_equation(variables,
                                           equal_to(operand_left, operand_right), right_equal));
            result.push_back(data_equation(variables,
                                           less(operand_left, operand_right), right_smaller));
            result.push_back(data_equation(variables,
                                           less_equal(operand_left, operand_right), right_smaller_equal));
          }
        }
      }

      return result;
    }

    data_equation_vector projection_equations(const sort_expression& s) const
    {
      data_equation_vector result;

      for (structured_sort_constructor_list::const_iterator i = constructors().begin(); i != constructors().end(); ++i)
      {
        if (!i->arguments().empty())
        {
          structured_sort_constructor_argument_list arguments(i->arguments());

          set_identifier_generator generator;

          std::vector< variable > variables;

          // Create variables for equation
          for (structured_sort_constructor_argument_list::const_iterator j(arguments.begin()); j != arguments.end(); ++j)
          {
            variables.push_back(variable(generator("v"), j->sort()));
          }

          std::vector< variable >::const_iterator v = variables.begin();

          for (structured_sort_constructor_argument_list::const_iterator j(arguments.begin()); j != arguments.end(); ++j, ++v)
          {
            if (j->name() != core::empty_identifier_string())
            {
              application lhs(function_symbol(j->name(), make_function_sort(s, j->sort()))
                              (application(i->constructor_function(s), variables)));

              result.push_back(data_equation(variables, lhs, *v));
            }
          }
        }
      }

      return result;
    }

    data_equation_vector recogniser_equations(const sort_expression& s) const
    {
      data_equation_vector result;

      structured_sort_constructor_list cl(constructors());

      for (structured_sort_constructor_list::const_iterator i = cl.begin(); i != cl.end(); ++i)
      {
        for (structured_sort_constructor_list::const_iterator j = cl.begin(); j != cl.end(); ++j)
        {
          if (j->recogniser() != core::empty_identifier_string())
          {
            data_expression right = (*i == *j) ? sort_bool::true_() : sort_bool::false_();

            if (i->arguments().empty())
            {
              result.push_back(data_equation(j->recogniser_function(s)(i->constructor_function(s)), right));
            }
            else
            {

              set_identifier_generator generator;

              structured_sort_constructor_argument_list arguments(i->arguments());

              // Create variables for equation
              variable_vector variables;

              for (structured_sort_constructor_argument_list::const_iterator k(arguments.begin()); k != arguments.end(); ++k)
              {
                variables.push_back(variable(generator("v"), k->sort()));
              }

              result.push_back(data_equation(variables, j->recogniser_function(s)(
                                               application(i->constructor_function(s), variables)), right));
            }
          }
        }
      }

      return result;
    }

public:
    /// \brief Returns the constructor functions of this sort, such that the
    ///        result can be used by the rewriter
    function_symbol_vector constructor_functions() const
    {
      return constructor_functions(*this);
    }

    /// \brief Returns the projection functions of this sort, such that the
    ///        result can be used by the rewriter
    function_symbol_vector projection_functions() const
    {
      return projection_functions(*this);
    }


    /// \brief Returns the recogniser functions of this sort, such that the
    ///        result can be used by the rewriter
    function_symbol_vector recogniser_functions() const
    {
      return recogniser_functions(*this);
    }

    /// \brief Returns the equations for ==, < and <= for this sort, such that the
    ///        result can be used by the rewriter
    //         internally
    data_equation_vector constructor_equations() const
    {
      return constructor_equations(*this);
    }
    /// \brief Generate equations for the projection functions of this sort
    /// \return A vector of equations for the projection functions of this sort.
    data_equation_vector projection_equations() const
    {
      return projection_equations(*this);
    }

    /// \brief Generate equations for the recognisers of this sort, assuming
    ///        that this sort is referred to with s.
    /// \return A vector of equations for the recognisers of this sort.
    data_equation_vector recogniser_equations() const
    {
      return recogniser_equations(*this);
    }
//--- end user section structured_sort ---//
};

/// \brief list of structured_sorts
typedef atermpp::term_list<structured_sort> structured_sort_list;

/// \brief vector of structured_sorts
typedef std::vector<structured_sort>    structured_sort_vector;

//--- end generated class structured_sort ---//

} // namespace data

} // namespace mcrl2

namespace std {
//--- start generated swap functions ---//
template <>
inline void swap(mcrl2::data::structured_sort& t1, mcrl2::data::structured_sort& t2)
{
  t1.swap(t2);
}
//--- end generated swap functions ---//
} // namespace std

#endif // MCRL2_DATA_SORT_EXPRESSION_H

