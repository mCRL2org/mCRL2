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
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/structured_sort_constructor_argument.h"

namespace mcrl2
{

namespace data
{

class structured_sort;

namespace detail
{

//--- start generated class structured_sort_constructor ---//
//--- end generated class structured_sort_constructor ---//

/// \brief A constructor for a structured sort
class structured_sort_constructor_base: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    structured_sort_constructor_base()
      : atermpp::aterm_appl(core::detail::constructStructCons())
    {}

    /// \brief Constructor.
    /// \param term A term
    structured_sort_constructor_base(const atermpp::aterm_appl& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_StructCons(*this));
    }

    /// \brief Constructor.
    structured_sort_constructor_base(const core::identifier_string& name, const structured_sort_constructor_argument_list& arguments, core::identifier_string& recogniser)
      : atermpp::aterm_appl(core::detail::gsMakeStructCons(name, arguments, recogniser))
    {}

    /// \brief Constructor.
    template <typename Container>
    structured_sort_constructor_base(const std::string& name, const Container& arguments, const std::string& recogniser, typename atermpp::detail::enable_if_container<Container, structured_sort_constructor_argument>::type* = 0)
      : atermpp::aterm_appl(core::detail::gsMakeStructCons(core::identifier_string(name), atermpp::convert<structured_sort_constructor_argument_list>(arguments), core::identifier_string(recogniser)))
    {}

    core::identifier_string name() const
    {
      return  atermpp::aterm_cast<core::identifier_string>(atermpp::arg1(*this));
    }

    structured_sort_constructor_argument_list arguments() const
    {
      return structured_sort_constructor_argument_list(atermpp::list_arg2(*this));
    }

    core::identifier_string recogniser() const
    {
      return atermpp::aterm_cast<core::identifier_string>(atermpp::arg3(*this));
    }
};

} // namespace detail

/// \brief A structured sort constructor.
///
/// A structured sort constructor has a mandatory name, a mandatory,
/// non-empty list of arguments and and optional recogniser name.
class structured_sort_constructor: public detail::structured_sort_constructor_base
{
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

    typedef detail::structured_sort_constructor_base super;

    inline
    static atermpp::aterm_appl make_constructor(core::identifier_string const& name,
        atermpp::term_list<structured_sort_constructor_argument> arguments,
        core::identifier_string const& recogniser)
    {
      assert(name != no_identifier());

      return core::detail::gsMakeStructCons(name, arguments,
                                 (recogniser == no_identifier()) ? core::detail::gsMakeNil() : static_cast< atermpp::aterm_appl >(recogniser));
    }

    inline
    static atermpp::aterm_appl make_constructor(core::identifier_string const& name, core::identifier_string const& recogniser)
    {
      return make_constructor(name, atermpp::term_list< structured_sort_constructor_argument >(), recogniser);
    }

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

    /// \\overload
    structured_sort_constructor()
      : detail::structured_sort_constructor_base()
    {}

    /// \brief Constructor.
    /// \param term A term
    structured_sort_constructor(const aterm &term)
      : detail::structured_sort_constructor_base(atermpp::aterm_cast<const atermpp::aterm_appl>(term))
    {}

    /// \brief Constructor
    ///
    /// \param[in] name The name of the constructor.
    /// \param[in] arguments a container of constructor arguments (of type structured_sort_constructor_argument)
    /// \param[in] recogniser The name of the recogniser.
    /// \pre name is not empty.
    /// \pre recogniser is not empty.
    template < typename Container >
    structured_sort_constructor(const core::identifier_string& name,
                                const Container& arguments,
                                const core::identifier_string& recogniser = no_identifier(),
                                typename atermpp::detail::enable_if_container< Container, structured_sort_constructor_argument >::type* = 0)
      : detail::structured_sort_constructor_base(make_constructor(name, atermpp::convert< atermpp::term_list< structured_sort_constructor_argument > >(arguments), recogniser))
    { }

    /// \brief Constructor
    ///
    /// \overload
    template < typename Container >
    structured_sort_constructor(const std::string& name,
                                const Container& arguments,
                                const std::string& recogniser,
                                typename atermpp::detail::enable_if_container< Container, structured_sort_constructor_argument >::type* = 0)
      : detail::structured_sort_constructor_base(make_constructor(detail::make_identifier(name),
          atermpp::convert< atermpp::term_list< structured_sort_constructor_argument > >(arguments), detail::make_identifier(recogniser)))
    { }

    /// \brief Constructor
    ///
    /// \overload to work around problem that MSVC reinterprets char* or char[] as core::identifier_string
    template < typename Container, size_t S, size_t S0 >
    structured_sort_constructor(const char(&name)[S],
                                const Container& arguments,
                                const char(&recogniser)[S0],
                                typename atermpp::detail::enable_if_container< Container, structured_sort_constructor_argument >::type* = 0)
      : detail::structured_sort_constructor_base(make_constructor(detail::make_identifier(name),
          atermpp::convert< atermpp::term_list< structured_sort_constructor_argument > >(arguments), detail::make_identifier(recogniser)))
    { }

    /// \brief Constructor
    ///
    /// \overload
    template < typename Container >
    structured_sort_constructor(const std::string& name,
                                const Container& arguments,
                                typename atermpp::detail::enable_if_container< Container, structured_sort_constructor_argument >::type* = 0)
      : detail::structured_sort_constructor_base(make_constructor(detail::make_identifier(name),
          atermpp::convert< atermpp::term_list< structured_sort_constructor_argument > >(arguments), no_identifier()))
    { }

    /// \brief Constructor
    ///
    /// \param[in] name The name of the constructor.
    /// \param[in] recogniser The name of the recogniser.
    /// \pre name is not empty.
    /// \pre recogniser is not empty.
    structured_sort_constructor(const core::identifier_string& name, const core::identifier_string& recogniser = no_identifier())
      : detail::structured_sort_constructor_base(make_constructor(name, recogniser))
    { }

    /// \brief Constructor
    ///
    /// \param[in] name The name of the constructor.
    /// \param[in] recogniser The name of the recogniser.
    /// \pre name is not empty.
    /// \pre recogniser is not empty.
    structured_sort_constructor(const std::string& name, const std::string& recogniser = "")
      : detail::structured_sort_constructor_base(make_constructor(detail::make_identifier(name), detail::make_identifier(recogniser)))
    { }

    /// \brief Constructor
    ///
    /// \overload to work around problem that MSVC reinterprets char* or char[] as core::identifier_string
    template < size_t S, size_t S0 >
    structured_sort_constructor(const char(&name)[S], const char(&recogniser)[S0])
      : detail::structured_sort_constructor_base(make_constructor(detail::make_identifier(name), detail::make_identifier(recogniser)))
    { }

    /// \overload
    core::identifier_string recogniser() const
    {
      return detail::make_identifier(super::recogniser());
    }

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
        if (i->name() != no_identifier())
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

}; // class structured_sort_constructor

/// \brief List of structured_sort_constructor.
typedef atermpp::term_list< structured_sort_constructor > structured_sort_constructor_list;

/// \brief List of structured_sort_constructor.
typedef std::vector< structured_sort_constructor >    structured_sort_constructor_vector;

// template function overloads
std::string pp(const structured_sort_constructor& x);
std::string pp(const structured_sort_constructor_list& x);
std::string pp(const structured_sort_constructor_vector& x);

} // namespace data

} // namespace mcrl2

namespace std {
template <>
inline void swap(mcrl2::data::structured_sort_constructor& t1, mcrl2::data::structured_sort_constructor& t2)
{
  t1.swap(t2);
}
} // namespace std

#endif // MCRL2_DATA_STRUCTURED_SORT_CONSTUCTOR_H
