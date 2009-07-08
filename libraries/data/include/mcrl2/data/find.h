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
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/detail/traverser.h"

namespace mcrl2 {

namespace data {

/// \cond INTERNAL_DOCS
namespace detail {

  template < typename Expression, typename OutputIterator >
  class collect_action {

    protected:

      OutputIterator m_sink;

    public:

      void operator()(Expression const& e)
      {
        *m_sink++ = e;
      }

      collect_action(OutputIterator const& sink) : m_sink(sink)
      { }
  };

  template < typename Expression, typename Action, template < class > class Traverser = detail::traverser >
  class find_helper : public Traverser< find_helper< Expression, Action, Traverser > > {

     typedef Traverser< find_helper< Expression, Action, Traverser > > super;

    protected:

      Action m_action;

    public:

      using super::enter;
      using super::leave;

      void enter(Expression const& e)
      {
        m_action(e);
      }

      find_helper()
      { }

      find_helper(Action action) : m_action(action)
      { }
  };

  template < typename Expression, typename OutputIterator >
  find_helper< Expression, collect_action< Expression, OutputIterator > >
  make_find_helper(OutputIterator sink)
  {
    return find_helper< Expression, collect_action< Expression, OutputIterator > >(collect_action< Expression, OutputIterator >(sink));
  }

  template < typename Expression, typename OutputIterator >
  find_helper< Expression, collect_action< Expression, OutputIterator >, detail::sort_traverser >
  make_sort_find_helper(OutputIterator sink)
  {
    return find_helper< Expression, collect_action< Expression, OutputIterator >, detail::sort_traverser >(
							collect_action< Expression, OutputIterator >(sink));
  }

  template < typename OutputIterator >
  find_helper< variable, collect_action< variable, OutputIterator > >
  make_variable_find_helper(OutputIterator sink)
  {
    return find_helper< variable, collect_action< variable, OutputIterator > >(
							collect_action< variable, OutputIterator >(sink));
  }

  class search_traversal_condition {
    private:
      bool m_result;

    public:

      search_traversal_condition() : m_result(true)
      { }

      bool operator()() {
        return m_result;
      }

      template < typename Expression >
      bool operator()(Expression const&)
      {
        return m_result;
      }

      void operator=(bool result)
      {
        m_result = result;
      }
  };

  /**
   * \brief Component for searching expressions
   *
   * Types:
   *  \arg Expression the type of sub expressions that is considered
   *  \arg AdaptablePredicate represents the search test on expressions (of type Expression)
   *
   * When m_predicate(e) becomes false traversal of sub-expressions will be
   * cut-short. The search_traversal_condition represents a condition
   * that is true initially and becomes false when the search predicate has
   * become false. It is used to cut-short expression traversal to return a
   * result.
   **/
  template < typename Expression, typename AdaptablePredicate, template < class, class > class SelectiveTraverser = detail::selective_data_traverser >
  class search_helper : public SelectiveTraverser< search_helper< Expression, AdaptablePredicate, SelectiveTraverser >, search_traversal_condition > {

      typedef SelectiveTraverser< search_helper< Expression, AdaptablePredicate, SelectiveTraverser >, search_traversal_condition > super;

    protected:

      AdaptablePredicate m_predicate;

    public:

      using super::operator();
      using super::enter;
      using super::leave;

      void enter(Expression const& e)
      {
        super::m_traverse_condition = super::m_traverse_condition() && !m_predicate(e);
      }

      template < typename Container >
      bool apply(Container const& container) {
        (*this)(container);

        return !super::m_traverse_condition();
      }

      search_helper()
      { }

      search_helper(AdaptablePredicate search_predicate) : m_predicate(search_predicate)
      { }
  };

  template < typename Expression, typename AdaptablePredicate >
  search_helper< Expression, AdaptablePredicate >
  make_search_helper(AdaptablePredicate search_predicate)
  {
    return search_helper< Expression, AdaptablePredicate >(search_predicate);
  }

  template < typename Expression, typename AdaptablePredicate >
  search_helper< Expression, AdaptablePredicate, detail::selective_sort_traverser >
  make_sort_search_helper(AdaptablePredicate search_predicate)
  {
    return search_helper< Expression, AdaptablePredicate, detail::selective_sort_traverser >(search_predicate);
  }

  template < typename AdaptablePredicate >
  search_helper< variable, AdaptablePredicate, detail::selective_data_traverser >
  make_variable_search_helper(AdaptablePredicate search_predicate)
  {
    return search_helper< variable, AdaptablePredicate >(search_predicate);
  }

  template < typename Action >
  class free_variable_find_helper : public detail::binding_aware_traverser< free_variable_find_helper< Action > > {

     typedef detail::binding_aware_traverser< free_variable_find_helper< Action > > super;

    protected:

      Action m_action;

    public:

      using super::operator();
      using super::enter;
      using super::leave;

      void enter(variable const& v)
      {
        if (!super::is_bound(v))
        {
          m_action(v);
        }
      }

      void operator()(assignment const& a)
      {
        (*this)(a.rhs());
      }

      free_variable_find_helper()
      { }

      free_variable_find_helper(Action action) : m_action(action)
      { }

      template < typename Container >
      free_variable_find_helper(Container const& bound, Action action) : super(bound), m_action(action)
      { }
  };

  template < typename OutputIterator >
  free_variable_find_helper< collect_action< variable, OutputIterator > >
  make_free_variable_find_helper(OutputIterator sink)
  {
    return free_variable_find_helper< collect_action< variable, OutputIterator > >(
							collect_action< variable, OutputIterator >(sink));
  }

  template < typename Container, typename OutputIterator >
  free_variable_find_helper< collect_action< variable, OutputIterator > >
  make_free_variable_find_helper(Container const& bound, OutputIterator sink)
  {
    return free_variable_find_helper< collect_action< variable, OutputIterator > >(bound,
							collect_action< variable, OutputIterator >(sink));
  }

  /**
   * \brief Component for searching expressions
   *
   * Types:
   *  \arg Expression the type of sub expressions that is considered
   *  \arg AdaptablePredicate represents the search test on expressions (of type Expression)
   *
   * When m_predicate(e) becomes true expression traversal will terminate.
   **/
  template < typename AdaptablePredicate >
  class free_variable_search_helper : public detail::selective_binding_aware_traverser<
			 free_variable_search_helper< AdaptablePredicate >, search_traversal_condition > {

      typedef detail::selective_binding_aware_traverser<
			 free_variable_search_helper< AdaptablePredicate >, search_traversal_condition > super;

    protected:

      AdaptablePredicate m_search_predicate;

    public:

      using super::operator();
      using super::enter;
      using super::leave;

      void enter(variable const& v)
      {
        if (!super::is_bound(v))
        {
          super::m_traverse_condition = !m_search_predicate(v);
        }
      }

      void operator()(assignment const& a)
      {
        (*this)(a.rhs());
      }

      template < typename Container >
      bool apply(Container const& container) {
        (*this)(container);

        return !super::m_traverse_condition();
      }

      free_variable_search_helper()
      { }

      free_variable_search_helper(AdaptablePredicate search_predicate) : m_search_predicate(search_predicate)
      { }

      template < typename Container >
      free_variable_search_helper(Container const& bound,
			 AdaptablePredicate search_predicate) : super(bound), m_search_predicate(search_predicate)
      { }
  };

  template < typename AdaptablePredicate >
  free_variable_search_helper< AdaptablePredicate >
  make_free_variable_search_helper(AdaptablePredicate search_predicate)
  {
    return free_variable_search_helper< AdaptablePredicate >(search_predicate);
  }

  template < typename Container, typename AdaptablePredicate >
  free_variable_search_helper< AdaptablePredicate >
  make_free_variable_search_helper(Container const& bound, AdaptablePredicate search_predicate)
  {
    return free_variable_search_helper< AdaptablePredicate >(bound, search_predicate);
  }
}
/// \endcond

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in,out] o an output iterator to which all data variables occurring in t
///             are added.
/// \return All data variables that occur in the term t
template < typename Container, typename OutputIterator >
void find_variables(Container const& container, OutputIterator const& o)
{
  detail::make_variable_find_helper(o)(container);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All data variables that occur in the term t
template < typename Container >
std::set< variable > find_variables(Container const& container)
{
  std::set< variable > result;
  find_variables(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given variable as subterm.
/// \param[in] container an expression or container with expressions
/// \param[in] v an expression or container with expressions
/// \param d A variable
/// \return True if the term has a given variable as subterm.
template < typename Container >
bool search_variable(Container const& container, const variable& v)
{
  return detail::make_variable_search_helper(detail::compare_variable(v)).apply(container);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in,out] o an output iterator to which all data variables occurring in t
///             are added.
/// \return All data variables that occur in the term t
template < typename Container, typename OutputIterator >
void find_free_variables(Container const& container, OutputIterator const& o,
		           typename detail::disable_if_container< OutputIterator >::type* = 0)
{
  detail::make_free_variable_find_helper(o)(container);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in,out] o an output iterator to which all data variables occurring in t
///             are added.
/// \param[in] bound a set of variables that should be considered as bound
/// \return All data variables that occur in the term t
/// TODO prevent copy of Sequence
template < typename Container, typename OutputIterator, typename Sequence >
void find_free_variables(Container const& container, OutputIterator const& o, Sequence const& bound)
{
  detail::make_free_variable_find_helper(bound, o)(container);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All data variables that occur in the term t
template < typename Container >
std::set< variable > find_free_variables(Container const& container)
{
  std::set< variable > result;
  find_free_variables(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in] bound a set of variables that should be considered as bound
/// \return All data variables that occur in the term t
/// TODO prevent copy of Sequence
template < typename Container, typename Sequence >
std::set< variable > find_free_variables(Container const& container, Sequence const& bound,
                                        typename detail::enable_if_container< Sequence, variable >::type* = 0)
{
  std::set< variable > result;
  find_free_variables(container, std::inserter(result, result.end()), bound);
  return result;
}

/// \brief Returns true if the term has a given variable as subterm.
/// \param[in] container an expression or container with expressions
/// \param d A data variable
/// \return True if the term has a given variable as subterm.
template < typename Container >
bool search_free_variable(Container container, const variable& d)
{
  return detail::make_variable_search_helper(detail::compare_variable(d)).apply(container);
}


/// \brief Returns true if the term has a given sort expression as subterm.
/// \param[in] container an expression or container of expressions
/// \param[in] s A sort expression
/// \return True if the term has a given sort expression as subterm.
template < typename Container >
bool search_sort_expression(Container const& container, const sort_expression& s)
{
  return detail::make_sort_search_helper< sort_expression >(detail::compare_sort(s))(container);
}

/// \brief Returns all sort expressions that occur in the term t
/// \param[in] container an expression or container of expressions
/// \param[in] o an output iterator
/// \return All sort expressions that occur in the term t
template < typename Container, typename OutputIterator >
void find_sort_expressions(Container const& container, OutputIterator o)
{
  detail::make_sort_find_helper< sort_expression >(o)(container);
}

/// \brief Returns all sort expressions that occur in the term t
/// \param[in] container an expression or container of expressions
/// \return All sort expressions that occur in the term t
template < typename Container >
std::set<sort_expression> find_sort_expressions(Container const& container)
{
  std::set<sort_expression> result;
  find_sort_expressions(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given sort identifier as subterm.
/// \param[in] container an expression or container of expressions
/// \param[in] s A sort identifier
/// \return True if the term has a given sort identifier as subterm.
template < typename Container >
bool search_basic_sort(Container const& container, const basic_sort& s)
{
  return detail::make_sort_search_helper< basic_sort >(detail::compare_sort(s)).apply(container);
}

/// \brief Returns all sort identifiers that occur in the term t
/// \param[in] container an expression or container of expressions
/// \param[out] o an output iterator
/// \return All sort identifiers that occur in the term t
template < typename Container, typename OutputIterator >
void find_basic_sorts(Container const& container, OutputIterator o)
{
  return detail::make_sort_find_helper< basic_sort >(o)(container);
}

/// \brief Returns all basic sorts that occur in the term t
/// \param[in] container an expression or container of expressions
/// \param[in] o an output iterator
/// \return All sort expressions that occur in the term t
template < typename Container >
std::set<basic_sort> find_basic_sorts(Container const& container)
{
  std::set<basic_sort> result;
  find_basic_sorts(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given data expression as subterm.
/// \param[in] container an expression or container of expressions
/// \param[in] s A data expression
/// \return True if the term has a given data expression as subterm.
template <typename Container >
bool search_data_expression(Container const& container, const data_expression& s)
{
  return detail::make_search_helper< data_expression >(detail::compare_term< data_expression >(s)).apply(container);
}

/// \brief Returns all data expressions that occur in the term t
/// \param[in] container an expression or container of expressions
/// \return All data expressions that occur in the term t
template < typename Container, typename OutputIterator >
void find_data_expressions(Container const& container, OutputIterator o)
{
  detail::make_find_helper< data_expression >(o)(container);
}

/// \brief Returns all data expressions that occur in the term t
/// \param[in] container an expression or container of expressions
/// \return All data expressions that occur in the term t
template < typename Container >
std::set< data_expression > find_data_expressions(Container const& container)
{
  std::set<data_expression> result;
  find_data_expressions(container, std::inserter(result, result.end()));
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
