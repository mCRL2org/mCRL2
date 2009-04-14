// Author(s): Jeroen Keiren, Jeroen van der Wulp, Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/data_expression_utility.h
/// \brief Provides utilities for working with new_data expression.

#ifndef MCRL2_NEW_DATA_DATA_EXPRESSION_UTILITY_H
#define MCRL2_NEW_DATA_DATA_EXPRESSION_UTILITY_H

#include <string>
#include <vector>
#include <set>

#include "boost/format.hpp"
#include "boost/assert.hpp"
#include "boost/iterator/transform_iterator.hpp"
#include "boost/utility/enable_if.hpp"
#include "boost/type_traits/is_integral.hpp"
#include "boost/type_traits/is_unsigned.hpp"
#include "boost/type_traits/make_unsigned.hpp"
#include "boost/type_traits/is_floating_point.hpp"

#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/core/find.h"
#include "mcrl2/core/detail/join.h"

#include "mcrl2/new_data/data_expression.h"
#include "mcrl2/new_data/sort_expression.h"
#include "mcrl2/new_data/function_sort.h"
#include "mcrl2/new_data/application.h"
#include "mcrl2/new_data/variable.h"
#include "mcrl2/new_data/bool.h"
#include "mcrl2/new_data/pos.h"
#include "mcrl2/new_data/nat.h"
#include "mcrl2/new_data/int.h"
#include "mcrl2/new_data/real.h"
#include "mcrl2/new_data/list.h"
#include "mcrl2/new_data/set.h"
#include "mcrl2/new_data/bag.h"

namespace mcrl2 {

  namespace new_data {

    /// \cond INTERNAL_DOCS
    namespace detail {

      /// \brief Extracts the sort out of objects of type data expression (or derived type)
      template < typename Expression >
      struct sort_of : public std::unary_function< Expression const&, sort_expression > {
        sort_expression operator()(Expression const& d) const {
          return d.sort();
        }
      };

      /// \brief Applies a function to the objects of type data expression (or derived type)
      template < typename Expression >
      struct apply : public std::unary_function< Expression const&, Expression > {
        function_symbol const& m_function;

        Expression operator()(Expression const& d) const {
          return m_function(d);
        }

        apply(function_symbol const& f) : m_function(f) {
        }
      };

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
      inline void decimal_number_divide_by_two(std::vector< char >& number)
      {
        BOOST_ASSERT(0 < number.size());

        std::vector< char >           result(number.size(), 0);
        std::vector< char >::iterator j(result.begin());

        if (2 <= number[0]) {
          *(j++) = number[0] / 2;
        }

        for (std::vector< char >::const_iterator i = number.begin() + 1; i != number.end(); ++i, ++j) {
          // result[a] = 5*(number[b - 1] mod 2) + number[b] div 2   where result[a] = *j, number[b - 1] = *(i - 1)
          *j = 5 * (*(i - 1) % 2) + *i / 2;
        }

        result.resize(j - result.begin());

        number.swap(result);
      }

      /// \brief Multiplies a number in decimal notation represented by an array by two
      /// \param[in,out] s the number
      /// A number d0 d1 ... dn is represented as s[0] s[1] ... s[n]
      inline void decimal_number_multiply_by_two(std::vector< char >& number)
      {
        BOOST_ASSERT(0 < number.size());

        std::vector< char >           result(number.size() + 2, 0);
        std::vector< char >::iterator j(result.begin());

        if (5 <= number[0]) {
          *(j++) = number[0] / 5;
        }

        for (std::vector< char >::const_iterator i = number.begin() + 1; i != number.end(); ++i, ++j) {
          // result[a] = 5*(number[b - 1] mod 2) + number[b] div 2   where result[a] = *j and number[b - 1] = *(i - 1)
          *j = 2 * (*(i - 1) % 5) + *i / 5;
        }

        result.resize(j - result.begin());

        number.swap(result);
      }

      /// \brief Adds one to a number in decimal notation represented by an array by two
      /// \param[in,out] s the number
      /// A number d0 d1 ... dn is represented as s[0] s[1] ... s[n]
      inline void decimal_number_increment(std::vector< char >& number) {
        BOOST_ASSERT(0 < number.size());

        for (std::vector< char >::reverse_iterator i = number.rbegin(); i != number.rend(); ++i) {
          if (*i < 9) {
            ++(*i);

            return;
          }
          else {
            *i = 0;
          }
        }

        number.insert(number.begin(), 1);
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

    namespace sort_fset {
      /// \brief Constructs a finite set expression from a range of expressions
      /// Type I must be a model of the Forward Traversal Iterator concept;
      /// with value_type convertible to data_expression.
      /// \param[in] s the sort of list elements
      /// \param[in] begin iterator that marks the start of a range of elements of sort s
      /// \param[in] end the past-end iterator for a range of elements of sort s
      template < typename I >
      inline
      application fset(const sort_expression& s, I const& begin, I const& end)
      {
        data_expression fset_expression(sort_fset::fset_empty(s));

        for (I i = begin; i != end; ++i) {
          assert(i->sort() == s);

          fset_expression = sort_fset::fsetinsert(s, *i, fset_expression);
        }

        return static_cast< application >(fset_expression);
      }
    }

    namespace sort_fbag {
      /// \brief Constructs a finite bag expression from a range of expressions
      /// Type I must be a model of the Forward Traversal Iterator concept;
      /// with value_type convertible to data_expression.
      /// \param[in] s the sort of list elements
      /// \param[in] begin iterator that marks the start of a range of elements of sort s
      /// \param[in] end the past-end iterator for a range of elements of sort s
      template < typename I >
      inline
      application fbag(const sort_expression& s, I const& begin, I const& end)
      {
        data_expression fbag_expression(sort_fbag::fbag_empty(s));

        for (I i = begin; i != end; ++i) {
          assert(i->sort() == s);

          fbag_expression = sort_fbag::fbaginsert(s, *i, fbag_expression, 1);
        }

        return static_cast< application >(fbag_expression);
      }
    }

    namespace sort_bool_{
      /// \brief Constructs expression of type Bool from an integral type
      /// \param b A Boolean
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
        return data_expression(core::detail::gsMakeOpId(
                atermpp::aterm_string(detail::as_decimal_string(t).c_str()), sort_pos::pos()));
      }

      /// \brief Constructs expression of type Pos from a string
      /// \param n A string
      inline data_expression pos(std::string const& n) {
        using namespace mcrl2::core::detail;

//// Temporary measure
//        std::vector< char > number_as_vector(detail::string_to_vector_number(n));
//        std::vector< bool > bits;
//
//        bits.reserve(number_as_vector.size());
//
//        while (number_as_vector[0] != 1 || 1 < number_as_vector.size()) { // number != 1
//          bits.push_back((static_cast< int >(*number_as_vector.rbegin()) % 2 != 0));
//
//          detail::decimal_number_divide_by_two(number_as_vector);
//        }
//
//        data_expression result(core::detail::gsMakeDataExprC1());
//
//        for (std::vector< bool >::const_reverse_iterator i = bits.rbegin(); i != bits.rend(); ++i) {
//          result = data_expression(gsMakeDataExprCDub(((*i) ? gsMakeDataExprTrue() : gsMakeDataExprFalse()), result));
//        }
//
//        return result;
        return data_expression(gsMakeOpId(atermpp::aterm_string(n.c_str()), sort_pos::pos()));
      }
    }

    namespace sort_nat {

      /// \brief Constructs expression of type pos from an integral type
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
      nat(T t) {
        assert(0 <= t);
//// Temporary measure
//        return data_expression(core::detail::gsMakeDataExprNat(const_cast< char* >(detail::as_decimal_string(t).c_str())));
        return data_expression(core::detail::gsMakeOpId(atermpp::aterm_string(detail::as_decimal_string(t).c_str()), sort_nat::nat()));
      }

      /// \brief Constructs expression of type Nat from a string
      /// \param n A string
      inline data_expression nat(std::string const& n) {
//// Temporary measure
//        if (n == "0") {
//          return data_expression(core::detail::gsMakeDataExprC0());
//        }
//
//        return data_expression(core::detail::gsMakeDataExprCNat(sort_pos::pos(n)));
        return data_expression(core::detail::gsMakeOpId(atermpp::aterm_string(n.c_str()), sort_nat::nat()));
      }
    }

    namespace sort_int_ {

      /// \brief Constructs expression of type pos from an integral type
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
      int_(T t) {
        std::string number(detail::as_decimal_string< typename boost::make_unsigned< T >::type >((0 <= t) ? t : -t));

//// Temporary measure
//        return data_expression(core::detail::gsMakeDataExprInt(const_cast< char* >((0 < t) ?
//                        number.c_str() : std::string("-").append(number).c_str())));
        return data_expression(core::detail::gsMakeOpId(atermpp::aterm_string((0 < t) ?
                        number.c_str() : std::string("-").append(number).c_str()), sort_int_::int_()));
      }

      /// \brief Constructs expression of type Int from a string
      /// \param n A string
      /// \pre n is of the form (-[1...9][0...9]+)([0...9]+)
      inline data_expression int_(std::string const& n) {
//// Temporary measure
//        if (n[0] == '-') {
//          return data_expression(core::detail::gsMakeDataExprCNeg(sort_pos::pos(n.substr(1))));
//        }
//
//        return data_expression(core::detail::gsMakeDataExprCInt(sort_nat::nat(n)));
        return data_expression(core::detail::gsMakeOpId(atermpp::aterm_string(n.c_str()), sort_int_::int_()));
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
      /// \param t An expression of type T
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
      real_(T t) {
        std::string number(detail::as_decimal_string< typename boost::make_unsigned< T >::type >((0 <= t) ? t : -t));

//// Temporary measure
//        return data_expression(core::detail::gsMakeDataExprReal(const_cast< char* >((0 <= t) ?
//                        number.c_str() : std::string("-").append(number).c_str())));
        return data_expression(core::detail::gsMakeOpId(atermpp::aterm_string((0 <= t) ?
                        number.c_str() : std::string("-").append(number).c_str()), sort_real_::real_()));
      }

      /// \brief Constructs expression of type Real from a string
      /// \param n A string
      /// \pre n is of the form (-[1...9][0...9]+)([0...9]+)
      inline data_expression real_(std::string const& n) {
//// Temporary measure
//        return data_expression(core::detail::gsMakeDataExprCReal(sort_int_::int_(n), core::detail::gsMakeDataExprC1()));
        return data_expression(core::detail::gsMakeOpId(atermpp::aterm_string(n.c_str()), sort_real_::real_()));
      }
    }

    /// \brief Construct numeric expression from a string representing a number in decimal notation
    /// \param s A sort expression
    /// \param n A string
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

    /// \brief Iterator adapter for traversing the sorts of an underlying sequence of data expressions
    template < typename ForwardTraversalIterator >
    struct sort_of_iterator : public boost::transform_iterator<
      detail::sort_of< typename ForwardTraversalIterator::value_type >, ForwardTraversalIterator >
    {
      /// \brief Constructor from iterator
      sort_of_iterator(ForwardTraversalIterator const& i) :
        boost::transform_iterator< detail::sort_of< typename ForwardTraversalIterator::value_type >, ForwardTraversalIterator >(i)
      {}

      /// \brief Constructor for past-the-end iterator
      sort_of_iterator()
      {}
    };

    /// Fresh variable generator that generates new_data variables with
    /// names that do not appear in the given context.
    class fresh_variable_generator
    {
      protected:
        atermpp::set<core::identifier_string> m_identifiers;
        sort_expression m_sort;                    // used for operator()()
        std::string m_hint;                  // used as a hint for operator()()

      public:
        /// Constructor.
        ///
        fresh_variable_generator()
         : m_sort(sort_bool_::bool_()), m_hint("t")
        { }

        /// Constructor.
        ///
        template <typename Term>
        fresh_variable_generator(Term context, sort_expression s = sort_bool_::bool_(), std::string hint = "t")
        {
          m_identifiers = core::find_identifiers(context);
          m_hint = hint;
          m_sort = s;
        }

        /// Set a new hint.
        ///
        void set_hint(std::string hint)
        {
          m_hint = hint;
        }

        /// Returns the current hint.
        ///
        std::string hint() const
        {
          return m_hint;
        }

        /// Set a new context.
        ///
        template <typename Term>
        void set_context(Term context)
        {
          m_identifiers = core::find_identifiers(context);
        }

        /// Set a new sort.
        ///
        void set_sort(sort_expression s)
        {
          m_sort = s;
        }

        /// Returns the current sort.
        ///
        sort_expression sort() const
        {
          return m_sort;
        }

        /// Add term t to the context.
        ///
        template <typename Term>
        void add_to_context(Term t)
        {
          std::set<core::identifier_string> ids = core::find_identifiers(t);
          std::copy(ids.begin(), ids.end(), std::inserter(m_identifiers, m_identifiers.end()));
        }

        /// Returns a unique variable of the given sort, with the given hint as prefix.
        /// The returned variable is added to the context.
        ///
        variable operator()()
        {
          core::identifier_string id(m_hint);
          int index = 0;
          while (m_identifiers.find(id) != m_identifiers.end())
          {
            std::string name = str(boost::format(m_hint + "%02d") % index++);
            id = core::identifier_string(name);
          }
          m_identifiers.insert(id);
          return variable(id, m_sort);
        }

        /// Returns a unique variable with the same sort as the variable v, and with
        /// the same prefix. The returned variable is added to the context.
        ///
        variable operator()(variable v)
        {
          std::string hint = v.name();
          core::identifier_string id(hint);
          int index = 0;
          while (m_identifiers.find(id) != m_identifiers.end())
          {
            std::string name = str(boost::format(hint + "%02d") % index++);
            id = core::identifier_string(name);
          }
          m_identifiers.insert(id);
          return variable(id, v.sort());
        }
    };


    /// \brief Returns the application of f to a sufficient number of variables,
    ///        taking variables from context if possible, otherwise extending
    ///        the context.
    inline data_expression apply_function_symbol_to_variables(const function_symbol& f, variable_vector& context, variable_vector& used_context)
    {
      if(f.sort().is_function_sort())
      {
        fresh_variable_generator generator(context, sort_bool_::bool_(), "x");
        variable_vector tmp_context = context; // Use vars only once
        function_sort f_sort = static_cast<const function_sort&>(f.sort());
        variable_vector arguments;
        for(function_sort::domain_const_range i(f_sort.domain()); !i.empty(); i.advance_begin(1))
        {
          variable v;
          generator.set_sort(i.front());
          for(variable_vector::iterator j = tmp_context.begin(); j != tmp_context.end() && v == variable(); ++j)
          {
            if(j->sort() == i.front())
            {
              v = *j;
              tmp_context.erase(j);
            }
          }

          // No variable found in context
          if(v == variable())
          {
            v = generator(); // Make fresh
          }
          arguments.push_back(v);
        }
        used_context = arguments;

        return application(f, boost::make_iterator_range(arguments));
      }
      else
      {
        return f;
      }
    }

    /** \brief A collection of utilities for lazy expression construction
     *
     * The basic idea is to keep expressions that result from application of
     * any of the container operations by applying the usual rules of logic.
     *
     * For example and(true, x) as in `and' applied to `true' and `x' yields x.
     **/
    namespace lazy {
      /// \brief Returns an expression equivalent to not p
      /// \param p A data expression
      /// \return The value <tt>!p</tt>
      inline data_expression not_(data_expression const& p)
      {
        if (p == sort_bool_::true_()) {
          return sort_bool_::false_();
        }
        else if (p == sort_bool_::false_()) {
          return sort_bool_::true_();
        }

        return sort_bool_::not_(p);
      }

      /// \brief Returns an expression equivalent to p and q
      /// \param p A data expression
      /// \param q A data expression
      /// \return The value <tt>p && q</tt>
      inline data_expression or_(data_expression const& p, data_expression const& q)
      {
        if ((p == sort_bool_::true_()) || (q == sort_bool_::true_())) {
          return sort_bool_::true_();
        }
        else if ((p == q) || (p == sort_bool_::false_())) {
          return q;
        }
        else if (q == sort_bool_::false_()) {
          return p;
        }

        return sort_bool_::or_(p, q);
      }

      /// \brief Returns an expression equivalent to p or q
      /// \param p A data expression
      /// \param q A data expression
      /// \return The value p || q
      inline data_expression and_(data_expression const& p, data_expression const& q)
      {
        if ((p == sort_bool_::false_()) || (q == sort_bool_::false_())) {
          return sort_bool_::false_();
        }
        else if ((p == q) || (p == sort_bool_::true_())) {
          return q;
        }
        else if (q == sort_bool_::true_()) {
          return p;
        }

        return sort_bool_::and_(p, q);
      }

      /// \brief Returns an expression equivalent to p implies q
      /// \param p A data expression
      /// \param q A data expression
      /// \return The value p || q
      inline data_expression implies(data_expression const& p, data_expression const& q)
      {
        if ((p == sort_bool_::false_()) || (q == sort_bool_::true_()) || (p == q)) {
          return sort_bool_::true_();
        }
        else if (p == sort_bool_::true_()) {
          return q;
        }
        else if (q == sort_bool_::false_()) {
          return sort_bool_::not_(p);
        }

        return sort_bool_::implies(p, q);
      }

      /// \brief Returns an expression equivalent to p == q
      /// \param p A data expression
      /// \param q A data expression
      /// \return The value p == q
      inline data_expression equal_to(data_expression const& p, data_expression const& q)
      {
        if (p == q) {
          return sort_bool_::true_();
        }

        return new_data::equal_to(p, q);
      }

      /// \brief Returns an expression equivalent to p == q
      /// \param p A data expression
      /// \param q A data expression
      /// \return The value ! p == q
      inline data_expression not_equal_to(data_expression const& p, data_expression const& q)
      {
        if (p == q) {
          return sort_bool_::false_();
        }

        return new_data::not_equal_to(p, q);
      }

      /// \brief Returns or applied to the sequence of data expressions [first, last)
      /// \param first Start of a sequence of data expressions
      /// \param last End of a sequence of data expressions
      /// \return Or applied to the sequence of data expressions [first, last)
      template < typename ForwardTraversalIterator >
      data_expression join_or(ForwardTraversalIterator first, ForwardTraversalIterator last)
      {
        return core::detail::join(first, last, lazy::or_, static_cast< sort_expression const& >(sort_bool_::false_()));
      }

      /// \brief Returns and applied to the sequence of data expressions [first, last)
      /// \param first Start of a sequence of data expressions
      /// \param last End of a sequence of data expressions
      /// \return And applied to the sequence of data expressions [first, last)
      template < typename ForwardTraversalIterator >
      data_expression join_and(ForwardTraversalIterator first, ForwardTraversalIterator last)
      {
        return core::detail::join(first, last, lazy::and_, static_cast< sort_expression const& >(sort_bool_::true_()));
      }
    }

  } // namespace new_data

} // namespace mcrl2

#endif //MCRL2_NEW_DATA_DATA_EXPRESSION_UTILITY_H

