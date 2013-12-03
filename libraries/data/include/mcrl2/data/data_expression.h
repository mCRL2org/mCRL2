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

#include "mcrl2/atermpp/detail/utility.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/detail/struct_core.h" // for gsIsDataExpr
#include "mcrl2/core/hash.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/untyped_sort.h"
#include "mcrl2/data/container_sort.h"
#include "mcrl2/data/is_simple_substitution.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2
{

namespace data
{

/// \brief Returns true if the term t is an abstraction
inline bool is_abstraction(const atermpp::aterm_appl &p)
{
  return core::detail::gsIsBinder(p);
}

/// \brief Returns true if the term t is a lambda abstraction
inline bool is_lambda(const atermpp::aterm_appl &p)
{
  return core::detail::gsIsBinder(p) &&
         core::detail::gsIsLambda(atermpp::aterm_cast<const atermpp::aterm_appl>(p[0]));
}

/// \brief Returns true if the term t is a universal quantification
inline bool is_forall(const atermpp::aterm_appl &p)
{
  return core::detail::gsIsBinder(p) &&
         core::detail::gsIsForall(atermpp::aterm_cast<const atermpp::aterm_appl>(p[0]));
}

/// \brief Returns true if the term t is an existential quantification
inline bool is_exists(const atermpp::aterm_appl &p)
{
  return core::detail::gsIsBinder(p) &&
         core::detail::gsIsExists(atermpp::aterm_cast<const atermpp::aterm_appl>(p[0]));
}

/// \brief Returns true if the term t is a set comprehension
inline bool is_set_comprehension(const atermpp::aterm_appl &p)
{
  return core::detail::gsIsBinder(p) &&
         core::detail::gsIsSetComp(atermpp::aterm_cast<const atermpp::aterm_appl>(p[0]));
}

/// \brief Returns true if the term t is a bag comprehension
inline bool is_bag_comprehension(const atermpp::aterm_appl &p)
{
  return core::detail::gsIsBinder(p) &&
         core::detail::gsIsBagComp(atermpp::aterm_cast<const atermpp::aterm_appl>(p[0]));
}

/// \brief Returns true if the term t is a set/bag comprehension.
inline bool is_untyped_set_or_bag_comprehension(const atermpp::aterm_appl &p)
{
  return core::detail::gsIsBinder(p) &&
         core::detail::gsIsUntypedSetBagComp(atermpp::aterm_cast<const atermpp::aterm_appl>(p[0]));
}

/// \brief Returns true if the term t is a function symbol
// inline bool is_function_symbol(const atermpp::aterm_appl &p)
//{
//  // return core::detail::gsIsOpId(p);
//  return p==mcrl2::data::function_symbol::function_symbol_OpId_;
//}

/// \brief Returns true if the term t is a variable
inline bool is_variable(const atermpp::aterm &p)
{
  return core::detail::gsIsDataVarId(atermpp::aterm_cast<const atermpp::aterm_appl>(p));
}

/// \brief Returns true if the term t is an application
inline bool is_application(const atermpp::aterm_appl &p)
{
  return core::detail::gsIsDataAppl(p);
}

/// \brief Returns true if the term t is a where clause
inline bool is_where_clause(const atermpp::aterm_appl &p)
{
  return core::detail::gsIsWhr(p);
}

/// \brief Returns true if the term t is an identifier
inline bool is_untyped_identifier(const atermpp::aterm_appl &p)
{
  return core::detail::gsIsUntypedIdentifier(p);
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

//--- start generated class data_expression ---//
/// \brief A data expression
class data_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    data_expression()
      : atermpp::aterm_appl(core::detail::constructDataExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit data_expression(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_DataExpr(*this));
    }
//--- start user section data_expression ---//
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
    
    sort_expression sort() const;
    /* {
      using namespace atermpp;
      // This implementation is currently done in this class, because there
      // is no elegant solution of distributing the implementation of the
      // derived classes (as we need to support requesting the sort of a
      // data_expression we do need to provide an implementation here).
      if (is_variable(*this))
      {
        return aterm_cast<sort_expression>((*this)[1]);
      }
      else if (is_function_symbol(*this))
      {
        return aterm_cast<sort_expression>((*this)[1]);
      }
      else if (is_abstraction(*this))
      {
        if (is_forall(*this) || is_exists(*this))
        {
          // Workaround for the unavailability of sort_bool::bool_()
          // (because of cyclic dependencies).
          return aterm_cast<data_expression>((*this)[2]).sort();
        }
        else if (is_lambda(*this))
        {
          const atermpp::term_list<aterm_appl> &v_variables = atermpp::aterm_cast<atermpp::term_list<aterm_appl> >((*this)[1]);
          sort_expression_vector s;
          for (atermpp::term_list<aterm_appl>::const_iterator i = v_variables.begin() ; i != v_variables.end(); ++i)
          {
            s.push_back(aterm_cast<sort_expression>((*i)[1])); // Push the sort.
          }
          return function_sort(sort_expression_list(s.begin(),s.end()), aterm_cast<data_expression>((*this)[2]).sort());
        }
        else
        {
          assert(is_set_comprehension(*this) || is_bag_comprehension(*this) || is_untyped_set_or_bag_comprehension(*this));
          const atermpp::term_list<aterm_appl> &v_variables  = atermpp::aterm_cast<atermpp::term_list<aterm_appl> >((*this)[1]);
          assert(v_variables.size() == 1);

          if (is_bag_comprehension(*this))
          {
            return container_sort(bag_container(), aterm_cast<const sort_expression>(v_variables.front()[1]));
          }
          else // If it is not known whether the term is a set or a bag, it returns the type of a set, as there is
               // no setbag type. This can only occur for terms that are not propertly type checked.
          {
            return container_sort(set_container(), aterm_cast<sort_expression>(v_variables.front()[1]));
          }
        }
      }
      else if (is_application(*this))
      {
        const data_expression &head = atermpp::aterm_cast<const data_expression>((*this)[0]);
        sort_expression s(head.sort());
        if (is_function_sort(s))
        {
          const function_sort& fs = core::static_down_cast<const function_sort&>(s);
          return (fs.codomain());
        }
        return s;
      }
      else if (is_where_clause(*this))
      {
        return aterm_cast<data_expression>((*this)[0]).sort();
      }
      assert(is_untyped_identifier(*this)); // All cases have been deal with here, except this one.
      return untyped_sort();

    } */
//--- end user section data_expression ---//
};

/// \brief list of data_expressions
typedef atermpp::term_list<data_expression> data_expression_list;

/// \brief vector of data_expressions
typedef std::vector<data_expression>    data_expression_vector;

// prototype declaration
std::string pp(const data_expression& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const data_expression& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(data_expression& t1, data_expression& t2)
{
  t1.swap(t2);
}
//--- end generated class data_expression ---//

/// \brief Test for a data_expression expression
/// \param x A term
/// \return True if it is a data_expression expression
inline
bool is_data_expression(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsUntypedIdentifier(x) ||
         core::detail::gsIsDataVarId(x) ||
         core::detail::gsIsOpId(x) ||
         core::detail::gsIsDataAppl(x) ||
         core::detail::gsIsBinder(x) ||
         core::detail::gsIsWhr(x);
}

/// \brief Converts an container with data expressions to data_expression_list
/// \param r A range of data expressions.
/// \note This function uses implementation details of the iterator type
/// and hence is sometimes efficient than copying all elements of the list.
template < typename Container >
inline data_expression_list make_data_expression_list(Container const& r, typename atermpp::detail::enable_if_container< Container, data_expression >::type* = 0)
{
  return atermpp::convert< data_expression_list >(r);
}

class variable;

// template function overloads
std::string pp(const data_expression_list& x);
std::string pp(const data_expression_vector& x);
data::data_expression translate_user_notation(const data::data_expression& x);
std::set<data::sort_expression> find_sort_expressions(const data::data_expression& x);
std::set<data::variable> find_all_variables(const data::data_expression& x);
std::set<data::variable> find_all_variables(const data::data_expression_list& x);
std::set<data::variable> find_free_variables(const data::data_expression& x);
std::set<data::variable> find_free_variables(const data::data_expression_list& x);
bool search_variable(const data::data_expression& x, const data::variable& v);

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
  return application(*this, e);
}

/// \brief Apply data expression to two data expressions
inline
application data_expression::operator()(const data_expression& e1, const data_expression& e2) const
{
  return application(*this, e1, e2);
}

/// \brief Apply data expression to three data expressions
inline
application data_expression::operator()(const data_expression& e1, const data_expression& e2, const data_expression& e3) const
{
  return application(*this, e1, e2, e3);
}

/// \brief Apply data expression to four data expressions
inline
application data_expression::operator()(const data_expression& e1, const data_expression& e2, const data_expression& e3, const data_expression& e4) const
{
  return application(*this, e1, e2, e3, e4);
}

}
}

#endif // MCRL2_DATA_DATA_EXPRESSION_H

