// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_expression.h
/// \brief The class data_expression.

#ifndef MCRL2_DATA_DATA_EXPRESSION_H
#define MCRL2_DATA_DATA_EXPRESSION_H

#include "mcrl2/exception.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/detail/struct_core.h" // for gsIsDataExpr
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/unknown_sort.h"
#include "mcrl2/data/container_sort.h"

namespace mcrl2
{

namespace data
{

/// \brief Returns true if the term t is a data expression
/// \param t A term
/// \return True if the term is a data expression.
inline
bool is_data_expression(atermpp::aterm_appl t)
{
  return core::detail::gsIsDataExpr(t);
}

/// \brief Returns true if the term t is an abstraction
inline bool is_abstraction(atermpp::aterm_appl p)
{
  return core::detail::gsIsBinder(p);
}

/// \brief Returns true if the term t is a lambda abstraction
inline bool is_lambda(atermpp::aterm_appl p)
{
  return core::detail::gsIsBinder(p) &&
         core::detail::gsIsLambda(atermpp::arg1(p));
}

/// \brief Returns true if the term t is a universal quantification
inline bool is_forall(atermpp::aterm_appl p)
{
  return core::detail::gsIsBinder(p) &&
         core::detail::gsIsForall(atermpp::arg1(p));
}

/// \brief Returns true if the term t is an existential quantification
inline bool is_exists(atermpp::aterm_appl p)
{
  return core::detail::gsIsBinder(p) &&
         core::detail::gsIsExists(atermpp::arg1(p));
}

/// \brief Returns true if the term t is a set comprehension
inline bool is_set_comprehension(atermpp::aterm_appl p)
{
  return core::detail::gsIsBinder(p) &&
         core::detail::gsIsSetComp(atermpp::arg1(p));
}

/// \brief Returns true if the term t is a bag comprehension
inline bool is_bag_comprehension(atermpp::aterm_appl p)
{
  return core::detail::gsIsBinder(p) &&
         core::detail::gsIsBagComp(atermpp::arg1(p));
}

/// \brief Returns true if the term t is a function symbol
inline bool is_function_symbol(atermpp::aterm_appl p)
{
  return core::detail::gsIsOpId(p);
}

/// \brief Returns true if the term t is a variable
inline bool is_variable(const atermpp::aterm &p)
{
  return core::detail::gsIsDataVarId(atermpp::aterm_appl(p));
}

/// \brief Returns true if the term t is an application
inline bool is_application(atermpp::aterm_appl p)
{
  return core::detail::gsIsDataAppl(p);
}

/// \brief Returns true if the term t is a where clause
inline bool is_where_clause(atermpp::aterm_appl p)
{
  return core::detail::gsIsWhr(p);
}

/// \brief Returns true if the term t is an identifier
inline bool is_identifier(atermpp::aterm_appl p)
{
  return core::detail::gsIsId(p);
}

class application; // prototype

/// \brief data expression.
///
/// A data expression can be any of:
/// - variable
/// - function symbol
/// - application
/// - abstraction
/// - where clause
/// - set enumeration
/// - bag enumeration
class data_expression: public atermpp::aterm_appl
{
  public:

    /// \brief Default constructor.
    ///
    data_expression()
      : atermpp::aterm_appl(core::detail::constructOpId())
    {}

    /// \brief Constructor.
    ///
    /// \param[in] t a term adhering to the internal format.
    explicit data_expression(const atermpp::aterm& t)
      : atermpp::aterm_appl(t)
    {
      // As Nil is used to indicate a non existing time value
      // in a linear process, we allow the occurrence of a Nil
      // term as a data_expression. This is a workaround which
      // should be removed.
      assert(is_data_expression(atermpp::aterm_appl(t)) || core::detail::gsIsNil(atermpp::aterm_appl(t)));
    }
    
    /// \brief Constructor.
    ///
    /// \param[in] t a term adhering to the internal format.
    data_expression(const atermpp::aterm_appl& t)
      : atermpp::aterm_appl(t)
    {
      // As Nil is used to indicate a non existing time value
      // in a linear process, we allow the occurrence of a Nil
      // term as a data_expression. This is a workaround which
      // should be removed.
      assert(is_data_expression(atermpp::aterm_appl(t)) || core::detail::gsIsNil(atermpp::aterm_appl(t)));
    }

    application operator()(const data_expression& e) const;

    application operator()(const data_expression& e1,
                           const data_expression& e2) const;

    application operator()(const data_expression& e1,
                           const data_expression& e2,
                           const data_expression& e3) const;

    application operator()(const data_expression& e1,
                           const data_expression& e2,
                           const data_expression& e3,
                           const data_expression& e4) const;

    /// \brief Returns the sort of the data expression
    inline
    sort_expression sort() const
    {
      sort_expression result;

      // This implementation is currently done in this class, because there
      // is no elegant solution of distributing the implementation of the
      // derived classes (as we need to support requesting the sort of a
      // data_expression we do need to provide an implementation here).
      if (is_variable(*this))
      {
        result = atermpp::arg2(*this);
      }
      else if (is_function_symbol(*this))
      {
        result = atermpp::arg2(*this);
      }
      else if (is_abstraction(*this))
      {
        if (is_forall(*this) || is_exists(*this))
        {
          // Workaround for the unavailability of sort_bool::bool_()
          // (because of cyclic dependencies).
          result = data_expression(atermpp::arg3(*this)).sort();
        }
        else if (is_lambda(*this))
        {
          atermpp::term_list<data_expression> v_variables = atermpp::term_list<data_expression>(atermpp::list_arg2(*this));
          sort_expression_vector s;
          for (atermpp::term_list<data_expression>::const_iterator i = v_variables.begin() ; i != v_variables.end(); ++i)
          {
            s.push_back(i->sort());
          }
          result = function_sort(boost::make_iterator_range(s), data_expression(atermpp::arg3(*this)).sort());
        }
        else if (is_set_comprehension(*this) || is_bag_comprehension(*this))
        {
          atermpp::term_list<data_expression> v_variables = atermpp::term_list<data_expression> (atermpp::list_arg2(*this));
          if (v_variables.size() != 1)
          {
            throw mcrl2::runtime_error("Set or bag comprehension has multiple bound variables, but may only have 1 bound variable");
          }

          if (is_set_comprehension(*this))
          {
            result = container_sort(set_container(), v_variables.begin()->sort());
          }
          else
          {
            result = container_sort(bag_container(), v_variables.begin()->sort());
          }

        }
        else
        {
          throw mcrl2::runtime_error("Unexpected abstraction occurred");
        }
      }
      else if (is_application(*this))
      {
        data_expression head = data_expression(atermpp::arg1(*this));
        sort_expression s(head.sort());
        if (s == sort_expression())
        {
          result = s;
        }
        else if (is_function_sort(s))
        {
          result = atermpp::arg2(s);
        }
        else
        {
          throw mcrl2::runtime_error("Sort " + s.to_string() + " of " + atermpp::arg1(*this).to_string() + " is not a function sort.");
        }
      }
      else if (is_where_clause(*this))
      {
        result = data_expression(atermpp::arg1(*this)).sort();
      }
      else if (is_identifier(*this))
      {
        result = sort_expression();
      }
      else
      {
        throw mcrl2::runtime_error("Unexpected data expression " + this->to_string() + " occurred.");
      }

      return result;
    }

}; // class data_expression

/// \brief list of data expressions
///
typedef atermpp::term_list<data_expression> data_expression_list;

/// \brief vector of data expressions
///
typedef std::vector<data_expression> data_expression_vector;

/// \brief Converts an container with data expressions to data_expression_list
/// \param r A range of data expressions.
/// \note This function uses implementation details of the iterator type
/// and hence is sometimes efficient than copying all elements of the list.
template < typename Container >
inline data_expression_list make_data_expression_list(Container const& r, typename atermpp::detail::enable_if_container< Container, data_expression >::type* = 0)
{
  return atermpp::convert< data_expression_list >(r);
}

//--- start generated class data_expression ---//
//--- end generated class data_expression ---//

class variable;

// template function overloads
std::string pp(const data_expression& x);
std::string pp(const data_expression_list& x);
std::string pp(const data_expression_vector& x);
data::data_expression translate_user_notation(const data::data_expression& x);
std::set<data::sort_expression> find_sort_expressions(const data::data_expression& x);
std::set<data::variable> find_variables(const data::data_expression& x);
std::set<data::variable> find_variables(const data::data_expression_list& x);
std::set<data::variable> find_free_variables(const data::data_expression& x);
std::set<data::variable> find_free_variables(const data::data_expression_list& x);
bool search_variable(const data::data_expression& x, const data::variable& v);

// TODO: we have to put it somewhere...
std::string pp(const atermpp::aterm& x);
std::string pp(const atermpp::aterm_appl& x);

} // namespace data

} // namespace mcrl2

// The trick of including application.h only at this point is needed to break
// the circular dependencies between application and data_expression. This
// dependency was introduced to allow the application operator to be defined for
// all data expression types.
#ifndef MCRL2_DATA_APPLICATION_H
#include "mcrl2/data/application.h"
#endif

namespace mcrl2
{
namespace data
{

/// \brief Apply data expression to a data expression
inline
application data_expression::operator()(const data_expression& e) const
{
  return make_application(*this, e);
}

/// \brief Apply data expression to two data expressions
inline
application data_expression::operator()(const data_expression& e1, const data_expression& e2) const
{
  return make_application(*this, e1, e2);
}

/// \brief Apply data expression to three data expressions
inline
application data_expression::operator()(const data_expression& e1, const data_expression& e2, const data_expression& e3) const
{
  return make_application(*this, e1, e2, e3);
}

/// \brief Apply data expression to four data expressions
inline
application data_expression::operator()(const data_expression& e1, const data_expression& e2, const data_expression& e3, const data_expression& e4) const
{
  return make_application(*this, e1, e2, e3, e4);
}

}
}

#endif // MCRL2_DATA_DATA_EXPRESSION_H

