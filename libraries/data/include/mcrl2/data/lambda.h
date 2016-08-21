// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/lambda.h
/// \brief The class lambda.

#ifndef MCRL2_DATA_LAMBDA_H
#define MCRL2_DATA_LAMBDA_H

#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/application.h"

namespace mcrl2
{

namespace data
{

/// \brief function symbol.
///
class lambda: public abstraction
{
  public:

    /// Constructor.
    ///
    /// \param[in] d A data expression.
    /// \pre d is a lambda abstraction.
    lambda(const data_expression& d)
      : abstraction(d)
    {
      assert(is_abstraction(d));
      assert(static_cast<abstraction>(d).binding_operator() == lambda_binder());
    }

    /// Constructor.
    ///
    /// \param[in] variable A nonempty list of binding variables.
    /// \param[in] body The body of the lambda abstraction.
    /// \pre variables is not empty.
    lambda(const variable& variable,
           const data_expression& body)
      : abstraction(lambda_binder(), variable_list({ variable }), body)
    {
    }

    /// Constructor.
    ///
    /// \param[in] variables A nonempty list of binding variables (objects of type variable).
    /// \param[in] body The body of the lambda abstraction.
    /// \pre variables is not empty.
    template < typename Container >
    lambda(const Container& variables,
           const data_expression& body,
           typename atermpp::enable_if_container< Container, variable >::type* = nullptr)
      : abstraction(lambda_binder(), variables, body)
    {
      assert(!variables.empty());
    }

}; // class lambda

//--- start generated class lambda ---//
// prototype declaration
std::string pp(const lambda& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const lambda& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(lambda& t1, lambda& t2)
{
  t1.swap(t2);
}
//--- end generated class lambda ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_LAMBDA_H

