// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/find.h
/// \brief Search functions of the data library.

#ifndef MCRL2_DATA_FIND_H
#define MCRL2_DATA_FIND_H

#include <set>
#include <functional>
#include <iterator>
#include <functional>
#include "boost/bind.hpp"
#include "boost/utility/enable_if.hpp"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/detail/expression_traverser.h"

namespace mcrl2 {

namespace data {

/// \cond INTERNAL_DOCS
namespace detail {

  // The last argument of the functions below is used with boost::enable_if to
  // activate the correct overload. This way the compiler generates the
  // necessary code, which would otherwise need to be duplicated *at least*
  // four times for frequently used containers (currently std::set,
  // atermpp::set, std::vector, atermpp::vector). Adding overloads for a new
  // container type only requires instantiation of is_container_impl for the
  // appropriate type.

  template <typename T, typename MatchPredicate>
  atermpp::aterm_appl find_if(T t, MatchPredicate match, typename boost::disable_if< typename is_container< T >::type >::type* = 0)
  {
    return atermpp::find_if(t, match);
  }

  template <typename T, typename MatchPredicate>
  atermpp::aterm_appl find_if(T const& t, MatchPredicate match, typename boost::enable_if< typename is_container< T >::type >::type* = 0)
  {
    for (typename T::const_iterator i = t.begin(); i != t.end(); ++i) {
      atermpp::aterm_appl result = atermpp::find_if(*i, match);

      if (result != atermpp::aterm_appl()) {
        return result;
      }
    }

    return atermpp::aterm_appl();
  }

  template <typename T, typename MatchPredicate, typename StopPredicate>
  atermpp::aterm_appl partial_find_if(T t, MatchPredicate match, StopPredicate stop, typename boost::disable_if< typename is_container< T >::type >::type* = 0)
  {
    return atermpp::partial_find_if(t, match, stop);
  }

  template <typename T, typename MatchPredicate, typename StopPredicate>
  atermpp::aterm_appl partial_find_if(T const& t, MatchPredicate match, StopPredicate stop, typename boost::enable_if< typename is_container< T >::type >::type* = 0)
  {
    for (typename T::const_iterator i = t.begin(); i != t.end(); ++i) {
      atermpp::aterm_appl result = atermpp::partial_find_if(*i, match, stop);

      if (result != atermpp::aterm_appl()) {
        return result;
      }
    }

    return atermpp::aterm_appl();
  }

  template <typename T, typename MatchPredicate, typename OutputIterator>
  void find_all_if(T t, MatchPredicate match, OutputIterator destBegin, typename boost::disable_if< typename is_container< T >::type >::type* = 0)
  {
    atermpp::find_all_if(t, match, destBegin);
  }

  // container specialisation
  template <typename T, typename MatchPredicate, typename OutputIterator>
  void find_all_if(T const& t, MatchPredicate match, OutputIterator destBegin, typename boost::enable_if< typename is_container< T >::type >::type* = 0)
  {
    for (typename T::const_iterator i = t.begin(); i != t.end(); ++i) {
      atermpp::find_all_if(*i, match, destBegin);
    }
  }

  template <typename T, typename MatchPredicate, typename StopPredicate, typename OutputIterator>
  void partial_find_all_if(T t, MatchPredicate match, StopPredicate stop, OutputIterator const& destBegin, typename boost::disable_if< typename is_container< T >::type >::type* = 0)
  {
    atermpp::partial_find_all_if(t, match, stop, destBegin);
  }

  // container specialisation
  template <typename T, typename MatchPredicate, typename StopPredicate, typename OutputIterator>
  void partial_find_all_if(T const& t, MatchPredicate match, StopPredicate stop, OutputIterator const& destBegin, typename boost::enable_if< typename is_container< T >::type >::type* = 0)
  {
    for (typename T::const_iterator i = t.begin(); i != t.end(); ++i) {
      atermpp::partial_find_all_if(*i, match, stop, destBegin);
    }
  }

  template <typename MatchPredicate, typename OutputIterator>
  class variable_find_all_helper : public detail::expression_traverser< variable_find_all_helper< MatchPredicate, OutputIterator > > {

    protected:

      MatchPredicate&      m_match;
      OutputIterator       m_iterator;

      void operator()(where_clause const& w)
      {
        for (where_clause::declarations_const_range r(w.declarations()); !r.empty(); r.advance_begin(1))
        {
          (*this)(r.front().rhs());
        }

        (*this)(w.body());
      }
  };

  /// TODO migrate to generated visitor structure
  template <typename MatchPredicate, typename OutputIterator>
  class free_variable_find_all_helper : public detail::expression_traverser< free_variable_find_all_helper< MatchPredicate, OutputIterator > > {

      typedef detail::expression_traverser< free_variable_find_all_helper< MatchPredicate, OutputIterator > > super;

    protected:

      std::multiset< variable > m_bound;
      MatchPredicate&           m_match;
      OutputIterator            m_iterator;

      template < typename Container >
      inline
      void increase_bind_count(const Container& variables, typename detail::enable_if_container< Container, variable >::type* = 0)
      {
        for (typename Container::const_iterator i = variables.begin(); i != variables.end(); ++i)
        {
          m_bound.insert(*i);
        }
      }

      template < typename Container >
      inline
      void decrease_bind_count(const Container& variables, typename detail::enable_if_container< Container, variable >::type* = 0)
      {
        for (typename Container::const_iterator i = variables.begin(); i != variables.end(); ++i)
        {
          m_bound.erase(m_bound.find(*i));
        }
      }

    public:

      using super::operator();

      void operator()(where_clause const& w)
      {
        increase_bind_count(make_assignment_left_hand_side_range(w.declarations()));

        for (where_clause::declarations_const_range r(w.declarations()); !r.empty(); r.advance_begin(1))
        {
          (*this)(r.front().rhs());
        }

        (*this)(w.body());

        decrease_bind_count(make_assignment_left_hand_side_range(w.declarations()));
      }

      void operator()(application const& a)
      {
        (*this)(a.head());

        for (application::arguments_const_range r(a.arguments()); !r.empty(); r.advance_begin(1))
        {
          (*this)(r.front());
        }
      }

      void operator()(assignment const& a)
      {
        (*this)(a.lhs());
        (*this)(a.rhs());
      }

      void operator()(variable const& v)
      {
        if (m_bound.find(v) == m_bound.end() && m_match(v))
        {
          *m_iterator = v;
        }
      }

      void operator()(abstraction const& a)
      {
        increase_bind_count(a.variables());

        (*this)(a.body());

        decrease_bind_count(a.variables());
      }

    public:

      template < typename Container >
      free_variable_find_all_helper(MatchPredicate& match,
                                OutputIterator const& destBegin,
                                Container const& bound_by_context,
                                typename detail::enable_if_container< Container, variable >::type* = 0) :
                          m_bound(bound_by_context.begin(), bound_by_context.end()), m_match(match), m_iterator(destBegin)
      {
      }

      free_variable_find_all_helper(MatchPredicate& match, OutputIterator const& destBegin) :
                                           m_match(match), m_iterator(destBegin)
      {
      }
  };

  template <typename T, typename MatchPredicate, typename OutputIterator>
  void find_all_free_variables_if(atermpp::term_list< T > const& t, MatchPredicate match, OutputIterator const& destBegin)
  {
    free_variable_find_all_helper< MatchPredicate, OutputIterator > context(match, destBegin);

    for (typename atermpp::term_list< T >::const_iterator i = t.begin(); i != t.end(); ++i)
    {
      context(*i);
    }
  }

  template <typename MatchPredicate, typename OutputIterator>
  void find_all_free_variables_if(data_expression const& t, MatchPredicate match, OutputIterator const& destBegin)
  {
    free_variable_find_all_helper< MatchPredicate, OutputIterator >(match, destBegin)(t);
  }

  template <typename T, typename MatchPredicate, typename OutputIterator>
  void find_all_free_variables_if(T const& t, MatchPredicate match, OutputIterator const& destBegin, typename boost::enable_if< typename is_container< T, data_expression >::type >::type* = 0)
  {
    free_variable_find_all_helper< MatchPredicate, OutputIterator > context(match, destBegin);

    for (typename T::const_iterator i = t.begin(); i != t.end(); ++i)
    {
      context(*i);
    }
  }
}
/// \endcond

/// \brief Returns all data variables that occur in the term t
/// This is implementation is more efficient, but there are problems with it...
/// \param t an expression
/// \return All data variables that occur in the term t
template <typename Container>
std::set<variable> find_all_variables2(Container t)
{
  std::set<variable> result;
  detail::partial_find_all_if(t,
                               boost::bind(&detail::is_variable, _1),
                               boost::bind(std::logical_or<bool>(), boost::bind(&detail::is_variable, _1), boost::bind(&detail::is_function_symbol, _1)),
                               detail::make_inserter(result)
                              );
  return result;
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All data variables that occur in the term t
template <typename Container >
std::set<variable> find_all_variables(Container const& container)
{
  std::set<variable> result;
  detail::find_all_if(container, boost::bind(&detail::is_variable, _1), detail::make_inserter(result));
  return result;
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in,out] o an output iterator to which all data variables occurring in t
///             are added.
/// \return All data variables that occur in the term t
template <typename Container, typename OutputIterator >
void find_all_free_variables(Container const& container, OutputIterator const& o)
{
  detail::find_all_free_variables_if(container, boost::bind(&detail::is_variable, _1), o);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All data variables that occur in the term t
template <typename Container >
std::set<variable> find_all_free_variables(Container const& container)
{
  std::set<variable> result;

  find_all_free_variables(container, detail::make_inserter(result));

  return result;
}

/// \brief Returns true if the term has a given variable as subterm.
/// \param[in] t a container with expressions
/// \param[in] t an expression or container with expressions
/// \param d A data variable
/// \return True if the term has a given variable as subterm.
template <typename Container >
bool search_variable(Container t, const variable& d)
{
  return detail::partial_find_if(t, detail::compare_term<variable>(d), &detail::is_variable) != atermpp::aterm();
}

/// \brief Returns true if the term has a given variable as subterm.
/// \param[in] container an expression or container with expressions
/// \param d A data variable
/// \return True if the term has a given variable as subterm.
template <typename Container >
bool search_free_variable(Container container, const variable& d)
{ // TODO make more efficient implementation
  std::set<variable> result;

  find_all_free_variables(container, detail::make_inserter(result));
  
  return result.find(d) != result.end();
}

/// \brief Returns true if the term has a given sort expression as subterm.
/// \param[in] t an expression
/// \param[in] s A sort expression
/// \return True if the term has a given sort expression as subterm.
template <typename Container>
bool search_sort_expression(Container const& t, const sort_expression& s)
{
  return detail::find_if(t, boost::bind(std::equal_to<atermpp::aterm_appl>(), s, _1)) != atermpp::aterm();
}

/// \brief Returns all sort expressions that occur in the term t
/// \param[in] t an expression
/// \param[in] o an output iterator
/// \return All sort expressions that occur in the term t
template <typename Container, typename OutputIterator>
void find_all_sort_expressions(Container const& t, OutputIterator o)
{
  detail::find_all_if(t, is_sort_expression, o);
}

/// \brief Returns all sort expressions that occur in the term t
/// \param[in] t an expression
/// \return All sort expressions that occur in the term t
template <typename Container>
std::set<sort_expression> find_all_sort_expressions(Container const& t)
{
  std::set<sort_expression> result;
  detail::find_all_if(t, is_sort_expression, detail::make_inserter(result));
  return result;
}

/// \brief Returns true if the term has a given sort identifier as subterm.
/// \param[in] t an expression
/// \param[in] s A sort identifier
/// \return True if the term has a given sort identifier as subterm.
template <typename Container>
bool search_basic_sort(Container const& t, const basic_sort& s)
{
  return search_sort_expression(t, s);
}

/// \brief Returns all sort identifiers that occur in the term t
/// \param[in] t an expression
/// \param[out] o an output iterator
/// \return All sort identifiers that occur in the term t
template <typename Container, typename OutputIterator>
void find_all_basic_sorts(Container const& t, OutputIterator o)
{
  find_all_sort_expressions(t, detail::make_filter_inserter< sort_expression >(boost::bind(&detail::is_basic_sort, _1), o));
}

/// \brief Returns all basic sorts that occur in the term t
/// \param[in] t an expression
/// \param[in] o an output iterator
/// \return All sort expressions that occur in the term t
template < typename Container >
std::set<basic_sort> find_all_basic_sorts(Container const& t)
{
  std::set<basic_sort> result;
  find_all_basic_sorts(t, detail::make_inserter(result));
  return result;
}

/// \brief Returns true if the term has a given data expression as subterm.
/// \param[in] t an expression
/// \param[in] s A data expression
/// \return True if the term has a given data expression as subterm.
template <typename Container >
bool search_data_expression(Container const& t, const data_expression& s)
{
  return detail::find_if(t, boost::bind(std::equal_to<atermpp::aterm_appl>(), s, _1)) != atermpp::aterm();
}

/// \brief Returns all data expressions that occur in the term t
/// \param t an expression
/// \return All data expressions that occur in the term t
template <typename Container >
std::set<data_expression> find_all_data_expressions(Container const& t)
{
  std::set<data_expression> result;
  detail::find_all_if(t, is_data_expression, detail::make_inserter(result));
  return result;
}

/// \brief Finds a mapping in a data specification.
/// \param data A data specification
/// \param s A string
/// \return The found mapping
inline
function_symbol find_mapping(data_specification const& data, std::string const& s)
{
  data_specification::mappings_const_range r(data.mappings());

  data_specification::mappings_const_range::const_iterator i = std::find_if(r.begin(), r.end(), detail::function_symbol_has_name(s));
  return (i == r.end()) ? function_symbol() : *i;
}

/// \brief Finds a constructor in a data specification.
/// \param data A data specification
/// \param s A string
/// \return The found constructor
inline
function_symbol find_constructor(data_specification const& data, std::string const& s)
{
  data_specification::constructors_const_range r(data.constructors());
  data_specification::constructors_const_range::const_iterator i = std::find_if(r.begin(), r.end(), detail::function_symbol_has_name(s));
  return (i == r.end()) ? function_symbol() : *i;
}

/// \brief Finds a sort in a data specification.
/// \param data A data specification
/// \param s A string
/// \return The found sort
inline
sort_expression find_sort(data_specification const& data, std::string const& s)
{
  data_specification::sorts_const_range r(data.sorts());
  data_specification::sorts_const_range::const_iterator i = std::find_if(r.begin(), r.end(), detail::sort_has_name(s));
  return (i == r.end()) ? sort_expression() : *i;
}

/// \brief Gets all equations with a data expression as head
/// on one of its sides.
///
/// \param[in] specification A data specification.
/// \param[in] d A data expression.
/// \return All equations with d as head in one of its sides.
inline
data_equation_vector find_equations(data_specification const& specification, const data_expression& d)
{
  data_equation_vector result;
  data_specification::equations_const_range equations(specification.equations());
  for (data_specification::equations_const_range::const_iterator i = equations.begin(); i != equations.end(); ++i)
  {
    if (i->lhs() == d || i->rhs() == d)
    {
      result.push_back(*i);
    }
    else if(i->lhs().is_application())
    {
      if(static_cast<application>(i->lhs()).head() == d)
      {
        result.push_back(*i);
      }
    }
    else if (i->rhs().is_application())
    {
      if(static_cast<application>(i->rhs()).head() == d)
      {
        result.push_back(*i);
      }
    }
  }
  return result;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FIND_H
