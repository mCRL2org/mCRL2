// Author(s): Jeroen Keiren, Jeroen van der Wulp, Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/utility.h
/// \brief Provides utilities for working with lists.

#ifndef MCRL2_NEW_DATA_UTILITY_H
#define MCRL2_NEW_DATA_UTILITY_H

#include <algorithm>
#include <functional>
#include <iterator>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "boost/format.hpp"
#include <boost/assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <boost/type_traits/is_floating_point.hpp>

#include "mcrl2/new_data/bool.h"
#include "mcrl2/new_data/pos.h"
#include "mcrl2/new_data/nat.h"
#include "mcrl2/new_data/int.h"
#include "mcrl2/new_data/real.h"
#include "mcrl2/new_data/list.h"
#include "mcrl2/new_data/assignment.h"
#include "mcrl2/new_data/detail/data_utility.h"
#include "mcrl2/new_data/data_expression_utility.h"
#include "mcrl2/core/print.h"

namespace mcrl2 {

  namespace new_data {

    /// \cond INTERNAL_DOCS
    namespace detail {

      inline std::vector< char > string_to_vector_number(std::string const& s) {
        std::vector< char > result;

        result.reserve(s.size());

        for (std::string::const_iterator i = s.begin(); i != s.end(); ++i) {
          BOOST_ASSERT('0' <= *i && *i <= '9');

          result.push_back(*i - '0');
        }

        return result;
      }

      /// Type T is an unsigned type
      template< typename T >
      inline std::string as_decimal_string(T t) {
        if (t != 0) {
          std::string result;

          while (0 < t) {
            result.append(1, '0' + static_cast< char >(t % 10));

            t /= 10;
          }

          return std::string(result.rbegin(), result.rend());
        }

        return "0";
      }

      /// \brief Divides a number in decimal notation represented by an array by two
      /// \param[in,out] s the number
      /// A number d0 d1 ... dn is represented as s[0] s[1] ... s[n]
      inline void divide_by_two(std::vector< char >& n)
      {
        BOOST_ASSERT(0 < n.size());

        std::vector< char > result(n.size(), 0);

        std::vector< char >::const_iterator i = n.begin();
        std::vector< char >::iterator       j = result.begin();

        if (1 < *i) {
          *(j++) = *i / 2;
        }

        for (++i; i != n.end(); ++i, ++j)
        {
          //result[a] = 5*(n[b - 1] mod 2) + n[b] div 2   where result[a] = *j, n[b - 1] = *(i - 1) and n[b] = *i
          *j = 5 * (*(i - 1) % 2) + (*i / 2);
        }

        result.resize(j - result.begin());

        n.swap(result);
      }
    }
    /// \endcond

    namespace sort_list {
      /// \brief Constructs a list expression from a range of expressions
      /// Type I must be a model of the Forward Traversal Iterator concept;
      /// with value_type convertible to data_expression.
      /// \param[in] s the sort of list elements
      /// \param[in] begin iterator that marks the start of a range of elements of sort s
      /// \param[in] end the past-end iterator for a range of elements of sort s
      template < typename I >
      inline
      application list(const sort_expression& s, I const& begin, I const& end)
      {
        data_expression list_expression(nil(s));

        for (I i = begin; i != end; ++i) {
          assert(i->sort() == s);

          list_expression = sort_list::snoc(s, list_expression, *i);
        }

        return static_cast< application >(list_expression);
      }
    }

    namespace sort_bool_{
      /// \brief Constructs expression of type Bool from an integral type
      inline data_expression bool_(bool b) {
        return (b) ? sort_bool_::true_() : sort_bool_::false_();
      }
    }

    namespace sort_pos {
      /// \brief Constructs expression of type Bool from an integral type
      /// Type T is an unsigned integral type.
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type,
        typename boost::enable_if< typename boost::is_unsigned< T >::type, data_expression >::type >::type
      pos(const T t) {
        return data_expression(core::detail::gsMakeDataExprPos(
                const_cast< char* >(detail::as_decimal_string(t).c_str())));
      }

      /// \brief Constructs expression of type Pos from a string
      inline data_expression pos(std::string const& n) {
        using namespace mcrl2::core::detail;

        std::vector< char > number_as_vector(detail::string_to_vector_number(n));
        std::vector< bool > bits;

        bits.reserve(number_as_vector.size());

        while (number_as_vector[0] != 1 || 1 < number_as_vector.size()) { // number != 1
          bits.push_back((static_cast< int >(*number_as_vector.rbegin()) % 2 != 0));

          detail::divide_by_two(number_as_vector);
        }

        data_expression result(core::detail::gsMakeDataExprC1());

        for (std::vector< bool >::const_reverse_iterator i = bits.rbegin(); i != bits.rend(); ++i) {
          result = data_expression(gsMakeDataExprCDub(((*i) ? gsMakeDataExprTrue() : gsMakeDataExprFalse()), result));
        }

        return result;
      }
    }

    namespace sort_nat {

      /// \brief Constructs expression of type pos from an integral type
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type,
        typename boost::enable_if< typename boost::is_unsigned< T >::type, data_expression >::type >::type
      nat(T t) {
        return data_expression(core::detail::gsMakeDataExprNat(const_cast< char* >(detail::as_decimal_string(t).c_str())));
      }

      /// \brief Constructs expression of type Nat from a string
      inline data_expression nat(std::string const& n) {
        if (n == "0") {
          return data_expression(core::detail::gsMakeDataExprC0());
        }

        return data_expression(core::detail::gsMakeDataExprCNat(sort_pos::pos(n)));
      }
    }

    namespace sort_int_ {

      /// \brief Constructs expression of type pos from an integral type
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
      int_(T t) {
        std::string number(detail::as_decimal_string< typename boost::make_unsigned< T >::type >((0 <= t) ? t : -t));

        return data_expression(core::detail::gsMakeDataExprInt(const_cast< char* >((0 < t) ?
                        number.c_str() : std::string("-").append(number).c_str())));
      }

      /// \brief Constructs expression of type Int from a string
      /// \pre n is of the form (-[1...9][0...9]+)([0...9]+)
      inline data_expression int_(std::string const& n) {
        if (n[0] == '-') {
          return data_expression(core::detail::gsMakeDataExprCNeg(sort_pos::pos(n.substr(1))));
        }

        return data_expression(core::detail::gsMakeDataExprCInt(sort_nat::nat(n)));
      }
    }

    namespace sort_real_ {

      /// \brief Constructs expression of type pos from an integral type
      //template < typename T >
      //inline typename boost::enable_if< typename boost::is_floating_point< T >::type, data_expression >::type
      //real_(const T) {
      // TODO;
      //}

      /// \brief Constructs expression of type pos from an integral type
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
      real_(T t) {
        std::string number(detail::as_decimal_string< typename boost::make_unsigned< T >::type >((0 <= t) ? t : -t));

        return data_expression(core::detail::gsMakeDataExprReal(const_cast< char* >((0 <= t) ?
                        number.c_str() : std::string("-").append(number).c_str())));
      }

      /// \brief Constructs expression of type Real from a string
      /// \pre n is of the form (-[1...9][0...9]+)([0...9]+)
      inline data_expression real_(std::string const& s) {
        return data_expression(core::detail::gsMakeDataExprCReal(sort_int_::int_(s), core::detail::gsMakeDataExprC1()));
      }
    }

    /// \brief Construct numeric expression from a string representing a number in decimal notation
    /// \pre n is of the form [0...9]+
    inline data_expression number(sort_expression const& s, std::string const& n)
    {
      if (s == sort_pos::pos()) {
        return sort_pos::pos(n);
      }
      else if (s == sort_nat::nat()) {
        return sort_nat::nat(n);
      }
      else if (s == sort_int_::int_()) {
        return sort_int_::int_(n);
      }

      return sort_real_::real_(n);
    }

    /// \brief Returns true if the term t is equal to nil
    inline bool is_nil(atermpp::aterm_appl t)
    {
      return t == core::detail::gsMakeNil();
    }

    /// \brief Applies the assignment to t and returns the result.
    /// \param t A term
    /// \return The application of the assignment to the term.
    inline data_expression substitute(assignment const& c, data_expression const& e)
    {
      return atermpp::replace(e, atermpp::aterm(c.lhs()), atermpp::aterm(c.rhs()));
    }

    template < typename SubstitutionFunction >
    inline data_expression substitute(SubstitutionFunction& f, data_expression const& c)
    {
      return data_expression(f(c));
    }

    /// \brief Applies a substitution function to all elements of a container
    template < typename Container, typename SubstitutionFunction, typename OutputIterator >
    void substitute(SubstitutionFunction f, Container const& c, OutputIterator o)
    {
      for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i, ++o)
      {
        *o = f(*i);
      }
    }

    /// \brief Pretty prints the contents of a container
    template < typename Container >
    inline std::string pp(Container const& c)
    {
      std::string result;

      if (c.begin() != c.end())
      {
        result.append(mcrl2::core::pp(*c.begin()));

        for (typename Container::const_iterator i = ++(c.begin()); i != c.end(); ++i)
        {
          result.append(", ").append(mcrl2::core::pp(*i));
        }
      }

      return result;
    }

    /// \brief Returns a copy of t, but with a common postfix added to each variable name,
    /// and such that the new names do not appear in context.
    /// \param t A sequence of data variables
    /// \param context A set of strings
    /// \param postfix_format A string
    /// \return A sequence of variables with names that do not appear in \p context. The
    /// string \p postfix_format is used to generate new names. It should contain one
    /// occurrence of "%d", that will be replaced with an integer.
    inline
    variable_list fresh_variables(variable_list const& t, const std::set<std::string>& context, std::string postfix_format = "_%02d")
    {
      std::vector<std::string> ids = detail::variable_strings(t);
      std::string postfix;
      for (int i = 0; ; i++)
      {
        postfix = str(boost::format(postfix_format) % i);
        std::vector<std::string>::iterator j = ids.begin();
        for ( ; j != ids.end(); j++)
        {
          if (context.find(*j + postfix) != context.end())
            break;
        }
        if (j == ids.end()) // success!
          break;
      }
      variable_list result;
      for (variable_list::const_iterator k = t.begin(); k != t.end(); ++k)
      {
        core::identifier_string name(std::string(k->name()) + postfix);
        result.push_back(variable(name, k->sort()));
      }
      return result;
    }

    /// \brief Returns an identifier that doesn't appear in the set <tt>context</tt>
    /// \param context A set of strings
    /// \param hint A string
    /// \param id_creator A function that generates identifiers
    /// \return An identifier that doesn't appear in the set <tt>context</tt>
    template <typename IdentifierCreator>
    inline core::identifier_string fresh_identifier(const std::set<core::identifier_string>& context, const std::string& hint, IdentifierCreator id_creator = IdentifierCreator())
    {
      int index = 0;
      core::identifier_string s;
      do
      {
        s = core::identifier_string(id_creator(hint, index++));
      }
      while(context.find(s) != context.end());
      return s;
    }

    /// \brief Returns an identifier that doesn't appear in the term context
    /// \param context A term
    /// \param hint A string
    /// \param id_creator A function that generates identifiers
    /// \return An identifier that doesn't appear in the term context
    template <typename Term, class IdentifierCreator>
    core::identifier_string fresh_identifier(Term context, const std::string& hint, IdentifierCreator id_creator = IdentifierCreator())
    {
      return fresh_identifier(core::find_identifiers(context), hint, id_creator);
    }

    /// \brief Creates an identifier built from name and index.
    struct default_identifier_creator
    {
      /// \brief Constructor.
      /// \param name A string
      /// \param index A positive number.
      /// \return An identifier.
      std::string operator()(const std::string& name, int index) const
      {
        if (index <= 0)
          return name;
        return str(boost::format(name + "%02d") % index++);
      }
    };

    /// \brief Returns an identifier that doesn't appear in the term context
    /// \param context A term
    /// \param hint A string
    /// \return An identifier that doesn't appear in the term context
    template <typename Term>
    core::identifier_string fresh_identifier(const Term& context, const std::string& hint)
    {
      return fresh_identifier(context, hint, default_identifier_creator());
    }

    /// \brief Returns a variable that doesn't appear in context
    /// \param context A term
    /// \param s A sort expression
    /// \param hint A string
    /// \return A variable that doesn't appear in context
    template <typename Term>
    variable fresh_variable(Term context, sort_expression s, std::string hint)
    {
      core::identifier_string id = fresh_identifier(context, hint);
      return variable(id, s);
    }

  } // namespace new_data

} // namespace mcrl2

#endif //MCRL2_NEW_DATA_UTILITY_H

