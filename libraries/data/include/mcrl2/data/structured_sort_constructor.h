// Author(s): Jeroen Keiren, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/structured_sort_constructor.h
/// \brief The classes structured_sort_constructor.

#ifndef MCRL2_DATA_STRUCTURED_SORT_CONSTRUCTOR_H
#define MCRL2_DATA_STRUCTURED_SORT_CONSTRUCTOR_H

#include <string>
#include <iterator>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/structured_sort_constructor_argument.h"

namespace mcrl2
{

namespace data
{

class structured_sort;

//--- start generated class structured_sort_constructor ---//
/// \brief A constructor for a structured sort
class structured_sort_constructor: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    structured_sort_constructor()
      : atermpp::aterm_appl(core::detail::default_value_StructCons())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit structured_sort_constructor(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_StructCons(*this));
    }

    /// \brief Constructor.
    structured_sort_constructor(const core::identifier_string& name, const structured_sort_constructor_argument_list& arguments, core::identifier_string& recogniser)
      : atermpp::aterm_appl(core::detail::function_symbol_StructCons(), name, arguments, recogniser)
    {}

    /// \brief Constructor.
    template <typename Container>
    structured_sort_constructor(const std::string& name, const Container& arguments, const std::string& recogniser, typename atermpp::detail::enable_if_container<Container, structured_sort_constructor_argument>::type* = 0)
      : atermpp::aterm_appl(core::detail::function_symbol_StructCons(), core::identifier_string(name), structured_sort_constructor_argument_list(arguments.begin(), arguments.end()), core::identifier_string(recogniser))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>((*this)[0]);
    }

    const structured_sort_constructor_argument_list& arguments() const
    {
      return atermpp::aterm_cast<const structured_sort_constructor_argument_list>((*this)[1]);
    }

    const core::identifier_string& recogniser() const
    {
      return atermpp::aterm_cast<const core::identifier_string>((*this)[2]);
    }
//--- start user section structured_sort_constructor ---//
    friend class structured_sort;

  private:

    struct get_argument_sort : public
        std::unary_function< structured_sort_constructor_argument const&, sort_expression >
    {

      sort_expression operator()(structured_sort_constructor_argument const& s) const
      {
        return s.sort();
      }
    };

  protected:
    /// \brief Returns the sorts of the arguments in an output iterator.
    ///
    template <typename OutIter>
    void argument_sorts(OutIter out) const
    {
      for(structured_sort_constructor_argument_list::const_iterator i = arguments().begin(); i != arguments().end(); ++i)
      {
        *out++ = i->sort();
      }
    }

  public:
    /// \brief Constructor
    structured_sort_constructor(const core::identifier_string& name, const core::identifier_string& recogniser)
      : atermpp::aterm_appl(core::detail::function_symbol_StructCons(), name, structured_sort_constructor_argument_list(), recogniser)
    {}

    /// \brief Constructor.
    structured_sort_constructor(const std::string& name, const std::string& recogniser)
      : atermpp::aterm_appl(core::detail::function_symbol_StructCons(), core::identifier_string(name), structured_sort_constructor_argument_list(), core::identifier_string(recogniser))
    {}

    /// \brief Constructor.
    template <typename Container>
    structured_sort_constructor(const std::string& name, const structured_sort_constructor_argument_list& arguments, typename atermpp::detail::enable_if_container<Container, structured_sort_constructor_argument>::type* = 0)
      : atermpp::aterm_appl(core::detail::function_symbol_StructCons(), core::identifier_string(name), arguments, core::empty_identifier_string())
    {}

    /// \brief Constructor.
    template <typename Container>
    structured_sort_constructor(const std::string& name, const Container& arguments, typename atermpp::detail::enable_if_container<Container, structured_sort_constructor_argument>::type* = 0)
      : atermpp::aterm_appl(core::detail::function_symbol_StructCons(), core::identifier_string(name), structured_sort_constructor_argument_list(arguments.begin(), arguments.end()), core::empty_identifier_string())
    {}

    /// \brief Constructor
    structured_sort_constructor(const core::identifier_string& name)
      : atermpp::aterm_appl(core::detail::function_symbol_StructCons(), name, structured_sort_constructor_argument_list(), core::empty_identifier_string())
    {}

    /// \brief Constructor.
    structured_sort_constructor(const std::string& name)
      : atermpp::aterm_appl(core::detail::function_symbol_StructCons(), core::identifier_string(name), structured_sort_constructor_argument_list(), core::empty_identifier_string())
    {}

    /// \brief Constructor
    ///
    /// \overload to work around problem that MSVC reinterprets char* or char[] as core::identifier_string
    template < typename Container, size_t S, size_t S0 >
    structured_sort_constructor(const char(&name)[S],
                                const Container& arguments,
                                const char(&recogniser)[S0],
                                typename atermpp::detail::enable_if_container< Container, structured_sort_constructor_argument >::type* = 0)
      : atermpp::aterm_appl(core::detail::function_symbol_StructCons(), core::identifier_string(name), structured_sort_constructor_argument_list(arguments.begin(), arguments.end()), core::identifier_string(recogniser))
    {}

    /// \brief Returns the constructor function for this constructor,
    ///        assuming it is internally represented with sort s.
    /// \param s Sort expression this sort is internally represented as.
    ///
    /// In general, constructor_function is used with s the structured
    /// sort of which this constructor is a part.
    /// Consider for example struct c|d, be a structured sort, where
    /// this constructor is c, then this.constructor_function(struct c|d)
    /// returns the fuction symbol c : struct c|d, i.e. the function c of
    /// sort struct c|d.
    function_symbol constructor_function(const sort_expression& s) const
    {
      sort_expression_vector arguments;
      std::back_insert_iterator<sort_expression_vector> inserter(arguments);
      argument_sorts(inserter);

      return function_symbol(name(), (arguments.empty()) ? s : function_sort(arguments, s));
    }

    /// \brief Returns the projection functions for this constructor.
    /// \param s The sort as which the structured sort this constructor corresponds
    ///          to is treated internally.
    function_symbol_vector projection_functions(const sort_expression& s) const
    {
      function_symbol_vector result;
      for (structured_sort_constructor_argument_list::const_iterator i = arguments().begin(); i != arguments().end(); ++i)
      {
        if (i->name() != core::empty_identifier_string())
        {
          result.push_back(function_symbol(i->name(), make_function_sort(s, i->sort())));
        }
      }
      return result;
    }

    /// \brief Returns the function corresponding to the recogniser of this
    /// constructor, such that it is usable in the rewriter.
    /// \param s The sort as which the structured sort this constructor
    /// corresponds to is treated internally.
    function_symbol recogniser_function(const sort_expression& s) const
    {
      return function_symbol(recogniser(), make_function_sort(s, sort_bool::bool_()));
    }
//--- end user section structured_sort_constructor ---//
};

/// \brief list of structured_sort_constructors
typedef atermpp::term_list<structured_sort_constructor> structured_sort_constructor_list;

/// \brief vector of structured_sort_constructors
typedef std::vector<structured_sort_constructor>    structured_sort_constructor_vector;

/// \brief Test for a structured_sort_constructor expression
/// \param x A term
/// \return True if \a x is a structured_sort_constructor expression
inline
bool is_structured_sort_constructor(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StructCons;
}

// prototype declaration
std::string pp(const structured_sort_constructor& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const structured_sort_constructor& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(structured_sort_constructor& t1, structured_sort_constructor& t2)
{
  t1.swap(t2);
}
//--- end generated class structured_sort_constructor ---//

// template function overloads
std::string pp(const structured_sort_constructor_list& x);
std::string pp(const structured_sort_constructor_vector& x);

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_STRUCTURED_SORT_CONSTUCTOR_H
