// Author(s): Jeroen Keiren, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_specification.h
/// \brief The class data_specification.

#ifndef MCRL2_DATA_DATA_SPECIFICATION_H
#define MCRL2_DATA_DATA_SPECIFICATION_H

#include <algorithm>
#include <functional>

#include "boost/iterator/transform_iterator.hpp"

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/table.h"
#include "mcrl2/atermpp/set.h"

// sorts
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/alias.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/standard.h"

// data expressions
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/data_equation.h"

namespace mcrl2 {

  namespace data {

    class data_specification;

    /// \cond INTERNAL_DOCS
    namespace detail {
      atermpp::aterm_appl data_specification_to_aterm_data_spec(const data_specification&);
    }
    /// \endcond

    /// \brief data specification.
    ///
    /// \invariant The specification is complete with respect to standard sorts
    class data_specification
    {
      protected:

        /// \brief map from sort expression to constructors
        typedef atermpp::multimap< sort_expression, function_symbol > sort_to_symbol_map;

        /// \brief map from basic_sort (names) to sort expression
        typedef atermpp::map< basic_sort, sort_expression >           ltr_aliases_map;

        /// \brief map from sort expression to names
        typedef std::multimap< sort_expression, basic_sort >          rtl_aliases_map;

      private:

        /// \brief projects a pair of sort and a constructor to the latter
        struct symbol_projection : public std::unary_function< sort_to_symbol_map::value_type const, function_symbol >
        {
          /// \brief Application to constant pair
          function_symbol const& operator()(sort_to_symbol_map::value_type const& v) const {
            return v.second;
          }
        };

        struct convert_to_alias : public std::unary_function< const std::pair< sort_expression, basic_sort >, alias >
        {
          alias operator()(std::pair< sort_expression, basic_sort > const& e) const
          {
            return alias(e.second, e.first);
          }
        };

      public:

        /// \brief iterator range over list of sort expressions
        typedef boost::iterator_range< atermpp::set< sort_expression >::iterator >                    sorts_range;
        /// \brief iterator range over constant list of sort expressions
        typedef boost::iterator_range< atermpp::set< sort_expression >::const_iterator >              sorts_const_range;

        /// \brief iterator range over list of sort expressions
        typedef boost::iterator_range< atermpp::set< variable >::iterator >                           variable_range;
        /// \brief iterator range over constant list of sort expressions
        typedef boost::iterator_range< atermpp::set< variable >::const_iterator >                     variable_const_range;

        /// \brief iterator over constructors
        typedef boost::transform_iterator< convert_to_alias, rtl_aliases_map::iterator >              aliases_iterator;
        /// \brief const iterator over constructors
        typedef boost::transform_iterator< convert_to_alias, rtl_aliases_map::const_iterator >        aliases_const_iterator;
        /// \brief iterator range over list of sort expressions
        typedef boost::iterator_range< aliases_iterator >                                             aliases_range;
        /// \brief iterator range over constant list of sort expressions
        typedef boost::iterator_range< aliases_const_iterator >                                       aliases_const_range;

        /// \brief iterator over constructors
        typedef boost::transform_iterator< symbol_projection, sort_to_symbol_map::iterator >          constructors_iterator;
        /// \brief const iterator over constructors
        typedef boost::transform_iterator< symbol_projection, sort_to_symbol_map::const_iterator >    constructors_const_iterator;
        /// \brief iterator range over constructors
        typedef boost::iterator_range< constructors_iterator >                                        constructors_range;
        /// \brief const iterator range over constructors
        typedef boost::iterator_range< constructors_const_iterator >                                  constructors_const_range;

        /// \brief iterator over constructors
        typedef boost::transform_iterator< symbol_projection, sort_to_symbol_map::iterator >          mappings_iterator;
        /// \brief const iterator over constructors
        typedef boost::transform_iterator< symbol_projection, sort_to_symbol_map::const_iterator >    mappings_const_iterator;
        /// \brief iterator range over constructors
        typedef boost::iterator_range< mappings_iterator >                                            mappings_range;
        /// \brief const iterator range over constructors
        typedef boost::iterator_range< mappings_const_iterator >                                      mappings_const_range;

        /// \brief iterator range over list of data equations
        typedef boost::iterator_range< atermpp::set< data_equation >::iterator >                      equations_range;
        /// \brief iterator range over constant list of data equations
        typedef boost::iterator_range< atermpp::set< data_equation >::const_iterator >                equations_const_range;

      private:

        friend data_specification& remove_all_system_defined(data_specification&);
        friend data_specification  remove_all_system_defined(data_specification const&);
        friend atermpp::aterm_appl detail::data_specification_to_aterm_data_spec(const data_specification&);

        ///\brief Adds system defined sorts when necessary to make the specification complete
        void make_system_defined_complete();

        ///\brief Adds system defined sorts when necessary to make the specification complete
        /// \param[in] s a sort that is added to a specification that is system-defined complete 
        void make_system_defined_complete(sort_expression const&);

        ///\brief Removes system defined sorts including constructors, mappings and equations
        void purge_system_defined();

        ///\brief Builds a specification from aterm
        void build_from_aterm(const atermpp::aterm_appl& t);

      protected:

        ///\brief The basic sorts and structured sorts in the specification.
        atermpp::set< sort_expression > m_sorts;

        ///\brief The basic sorts and structured sorts in the specification.
        ltr_aliases_map                 m_aliases_by_name;

        ///\brief The basic sorts and structured sorts in the specification.
        rtl_aliases_map                 m_aliases_by_sort;

        ///\brief A mapping of sort expressions to the constructors corresponding to that sort.
        sort_to_symbol_map              m_constructors;

        ///\brief The mappings of the specification.
        sort_to_symbol_map              m_mappings;

        ///\brief The equations of the specification.
        atermpp::set< data_equation >   m_equations;

        ///\brief Table containing system defined sorts.
        mutable atermpp::table m_sys_sorts;

        ///\brief Table containing system defined constructors.
        mutable atermpp::table m_sys_constructors;

        ///\brief Table containing system defined mappings.
        mutable atermpp::table m_sys_mappings;

        ///\brief Table containing system defined equations.
        mutable atermpp::table m_sys_equations;

      protected:

      void insert_sort(const sort_expression& s)
      {
        if (s.is_alias())
        { // add aliases as names for sort expressions that are non-aliases
          add_alias(s);
        }
        else if (!s.is_function_sort()) // do not add function sorts
        {
          m_sorts.insert(s);

          add_system_defined_mappings(boost::make_iterator_range(standard_generate_functions_code(s)));
          add_system_defined_equations(boost::make_iterator_range(standard_generate_equations_code(s)));
        }
      }

      void add_function(sort_to_symbol_map& container, const function_symbol& f)
      {
        sort_expression index_sort(normalise(f.sort().target_sort()));

        constructors_const_range relevant_range(container.equal_range(index_sort));

        if (std::find(relevant_range.begin(), relevant_range.end(), f) == relevant_range.end())
        {
          container.insert(std::make_pair(index_sort, f));
        }
      }

      public:

      ///\brief Default constructor
      data_specification()
      {
        make_system_defined_complete();
      }

      ///\brief Constructor
      ///
      /// \param[in] t a term adhering to the internal format.
      data_specification(const atermpp::aterm_appl& t)
      {
        build_from_aterm(t);
        make_system_defined_complete();
      }

      ///\brief Constructor
      template < typename SortsIterator, typename ConstructorsIterator,
                 typename MappingsIterator, typename EquationsIterator >
      data_specification(const boost::iterator_range< SortsIterator >& sorts,
                         const boost::iterator_range< ConstructorsIterator >& constructors,
                         const boost::iterator_range< MappingsIterator >& mappings,
                         const boost::iterator_range< EquationsIterator >& equations)
      {
        add_sorts(sorts);
        add_constructors(constructors);
        add_mappings(mappings);
        add_equations(equations);
        make_system_defined_complete();
      }

      /// \brief Gets the sort declarations
      ///
      /// Time complexity of this operation is constant.
      /// \return The sort declarations of this specification.
      inline
      sorts_const_range sorts() const
      {
        return boost::make_iterator_range(m_sorts);
      }

      /// \brief Gets the aliases
      /// Time complexity of this operation is constant.
      inline
      aliases_const_range aliases() const
      {
        return aliases_const_range(m_aliases_by_sort);
      }

      /// \brief Gets the aliases
      ///
      /// \param[in] s A sort expression
      /// \return The aliases of sort s
      inline
      aliases_const_range aliases(sort_expression const& s) const
      {
        return aliases_const_range(m_aliases_by_sort.equal_range(s));
      }

      /// \brief Gets all constructors
      ///
      /// \return All constructors in this specification, including those for
      /// structured sorts.
      inline
      constructors_const_range constructors() const
      {
        return boost::iterator_range< constructors_const_iterator >(m_constructors);
      }

      /// \brief Gets all constructors of a sort.
      ///
      /// Time complexity of this operation is constant.
      /// \param[in] s A sort expression.
      /// \return The constructors for sort s in this specification.
      constructors_const_range constructors(const sort_expression& s) const
      {
        return boost::iterator_range< constructors_const_iterator >(m_constructors.equal_range(normalise(s)));
      }

      /// \brief Gets all mappings in this specification
      ///
      /// Time complexity of this operation is constant.
      /// \return All mappings in this specification, including recognisers and
      /// projection functions from structured sorts.
      inline
      mappings_const_range mappings() const
      {
        return boost::iterator_range< mappings_const_iterator >(m_mappings);
      }

      /// \brief Gets all mappings of a sort
      ///
      /// \param[in] s A sort expression.
      /// \return All mappings in this specification, for which s occurs as a
      /// right-hand side of the mapping's sort.
      mappings_const_range mappings(const sort_expression& s) const
      {
        return boost::iterator_range< mappings_const_iterator >(m_mappings.equal_range(normalise(s)));
      }

      /// \brief Gets all equations in this specification
      ///
      /// Time complexity of this operation is constant.
      /// \return All equations in this specification, including those for
      ///  structured sorts.
      inline
      equations_const_range equations() const
      {
        return boost::make_iterator_range(m_equations);
      }

      /// \brief Adds a sort to this specification
      ///
      /// \param[in] s A sort expression.
      /// \note this operation does not invalidate iterators of sorts_const_range
      void add_sort(const sort_expression& s)
      {
        insert_sort(s);
        make_system_defined_complete(s);
      }

      /// \brief Adds a sort to this specification, and marks it as system
      ///        defined
      ///
      /// \param[in] s A sort expression.
      /// \pre s does not yet occur in this specification.
      /// \post is_system_defined(s) = true
      /// \note this operation does not invalidate iterators of sorts_const_range
      void add_system_defined_sort(const sort_expression& s)
      {
        insert_sort(s);
        m_sys_sorts.put(s,s);
      }

      /// \brief Adds an alias (new name for a sort) to this specification
      ///
      /// \param[in] s an alias
      /// \pre !search_sort(s.name()) || is_alias(s.name()) || constructors(s.name()).empty()
      /// \note this operation does not invalidate iterators of aliases_const_range
      /// \post is_alias(s.name()) && find_referenced_sort(s.name()) = normalise(s.reference())
      void add_alias(alias const& s)
      {
        assert(!search_sort(s.name()) || is_alias(s.name()) || (constructors(s.name()).empty()));

        sort_expression canonical_sort(normalise(s.reference()));

        m_sorts.insert(alias(s.name(), canonical_sort)); // TODO do aliases need to be part of the set of sorts? -- jwulp

        m_aliases_by_name[s.name()] = canonical_sort;
        m_aliases_by_sort.insert(rtl_aliases_map::value_type(canonical_sort, s.name()));

        boost::iterator_range< rtl_aliases_map::iterator > relevant_range(m_aliases_by_sort.equal_range(s.name()));

        if (!relevant_range.empty())
        {
          // s.name() was known as sort but not as alias, remove standard mappings/equations and update aliases
          remove_mappings(boost::make_iterator_range(standard_generate_functions_code(s.name())));
          remove_equations(boost::make_iterator_range(standard_generate_equations_code(s.name())));

          for (rtl_aliases_map::iterator i = relevant_range.begin(), j = relevant_range.begin();
                                                                     j++ != relevant_range.end(); i = j)
          {
            m_aliases_by_name[i->second] = canonical_sort;
            m_aliases_by_sort.insert(rtl_aliases_map::value_type(canonical_sort, i->second));
            m_aliases_by_sort.erase(i);

            m_sorts.erase(alias(i->second, i->first));        // TODO do aliases need to be part of the set of sorts? -- jwulp
            m_sorts.insert(alias(i->second, canonical_sort)); // TODO do aliases need to be part of the set of sorts? -- jwulp
          }
        }

        if (s.reference().is_standard())
        {
          // Make sure that the sort is also part of the specification
          add_sort(s.reference());
        }
      }

      /// \brief Adds a constructor to this specification
      ///
      /// \param[in] f A function symbol.
      /// \pre a mapping f does not yet occur in this specification.
      /// \note this operation does not invalidate iterators of constructors_const_range
      void add_constructor(const function_symbol& f)
      {
        assert(!search_mapping(f));
        add_function(m_constructors, f);
        make_system_defined_complete(f.sort());
      }

      /// \brief Adds a constructor to this specification, and marks it as
      ///        system defined.
      ///
      /// \param[in] f A function symbol.
      /// \pre f does not yet occur in this specification.
      /// \post is_system_defined(f) == true
      /// \note this operation does not invalidate iterators of constructors_const_range
      inline
      void add_system_defined_constructor(const function_symbol& f)
      {
        add_function(m_constructors, f);
        m_sys_constructors.put(f,f);
      }

      /// \brief Adds a mapping to this specification
      ///
      /// \param[in] f A function symbol.
      /// \pre a constructor f does not yet occur in this specification.
      /// \note this operation does not invalidate iterators of mappings_const_range
      void add_mapping(const function_symbol& f)
      {
        assert(!search_constructor(f));
        add_function(m_mappings, f);
        make_system_defined_complete(f.sort());
      }

      /// \brief Adds a mapping to this specification, and marks it as system
      ///        defined.
      ///
      /// \param[in] f A function symbol.
      /// \pre f does not yet occur in this specification.
      /// \post is_system_defined(f) == true
      /// \note this operation does not invalidate iterators of mappings_const_range
      void add_system_defined_mapping(const function_symbol& f)
      {
        add_function(m_mappings, f);
        m_sys_mappings.put(f,f);
      }

      /// \brief Adds an equation to this specification
      ///
      /// \param[in] e An equation.
      /// \pre e does not yet occur in this specification.
      /// \note this operation does not invalidate iterators of equations_const_range
      void add_equation(const data_equation& e)
      {
        m_equations.insert(e);
      }

      /// \brief Adds an equation to this specification, and marks it as system
      ///        defined.
      ///
      /// \param[in] e An equation.
      /// \pre e does not yet occur in this specification.
      /// \post is_system_defined(f) == true
      /// \note this operation does not invalidate iterators of equations_const_range
      void add_system_defined_equation(const data_equation& e)
      {
        add_equation(e);
        m_sys_equations.put(e,e);
      }

      /// \brief Adds sorts to this specification
      ///
      /// \param[in] sl A range of sort expressions.
      /// \note this operation does not invalidate iterators of sorts_const_range
      template < typename ForwardTraversalIterator >
      void add_sorts(const boost::iterator_range< ForwardTraversalIterator >& sl)
      {
        for (ForwardTraversalIterator i = sl.begin(); i != sl.end(); ++i)
        {
          add_sort(*i);
        }
      }

      /// \brief Adds sorts to this specification, and marks them as system
      /// defined.
      ///
      /// \param[in] sl A range of sort expressions.
      /// \post for all s in sl: is_system_defined(s)
      /// \note this operation does not invalidate iterators of sorts_const_range
      template < typename ForwardTraversalIterator >
      void add_system_defined_sorts(const boost::iterator_range< ForwardTraversalIterator >& sl)
      {
        for (ForwardTraversalIterator i = sl.begin(); i != sl.end(); ++i)
        {
          add_system_defined_sort(*i);
        }
      }

      /// \brief Adds constructors to this specification
      ///
      /// \param[in] fl A range of function symbols.
      /// \note this operation does not invalidate iterators of constructors_const_range
      template < typename ForwardTraversalIterator >
      void add_constructors(const boost::iterator_range< ForwardTraversalIterator >& fl)
      {
        for (ForwardTraversalIterator i = fl.begin(); i != fl.end(); ++i)
        {
          add_constructor(*i);
        }
      }

      /// \brief Adds constructors to this specification, and marks them as
      ///        system defined.
      ///
      /// \param[in] fl A range of function symbols.
      /// \post for all f in fl: is_system_defined(f)
      /// \note this operation does not invalidate iterators of constructors_const_range
      template < typename ForwardTraversalIterator >
      void add_system_defined_constructors(const boost::iterator_range< ForwardTraversalIterator >& fl)
      {
        for (ForwardTraversalIterator i = fl.begin(); i != fl.end(); ++i)
        {
          add_system_defined_constructor(*i);
        }
      }

      /// \brief Adds mappings to this specification
      ///
      /// \param[in] fl A range of function symbols.
      /// \note this operation does not invalidate iterators of mappings_const_range
      template < typename ForwardTraversalIterator >
      void add_mappings(const boost::iterator_range< ForwardTraversalIterator >& fl)
      {
        for (ForwardTraversalIterator i = fl.begin(); i != fl.end(); ++i)
        {
          add_mapping(*i);
        }
      }

      /// \brief Adds mappings to this specification, and marks them as system
      ///        defined.
      ///
      /// \param[in] fl A range of function symbols.
      /// \post for all f in fl: is_system_defined(f)
      /// \note this operation does not invalidate iterators of mappings_const_range
      template < typename ForwardTraversalIterator >
      void add_system_defined_mappings(const boost::iterator_range< ForwardTraversalIterator >& fl)
      {
        for (ForwardTraversalIterator i = fl.begin(); i != fl.end(); ++i)
        {
          add_system_defined_mapping(*i);
        }
      }

      /// \brief Adds equations to this specification
      ///
      /// \param[in] el A range of equations.
      /// \note this operation does not invalidate iterators of equations_const_range
      template < typename ForwardTraversalIterator >
      void add_equations(const boost::iterator_range< ForwardTraversalIterator >& el)
      {
        for (ForwardTraversalIterator i = el.begin(); i != el.end(); ++i)
        {
          add_equation(*i);
        }
      }

      /// \brief Adds equations to this specification, and marks them as system
      ///        defined.
      ///
      /// \param[in] el A range of equations.
      /// \post for all e in el: is_system_defined(e)
      /// \note this operation does not invalidate iterators of equations_const_range
      template < typename ForwardTraversalIterator >
      void add_system_defined_equations(const boost::iterator_range< ForwardTraversalIterator >& el)
      {
        for (ForwardTraversalIterator i = el.begin(); i != el.end(); ++i)
        {
          add_system_defined_equation(*i);
        }
      }

      /// \brief Normalises a sort expression by following aliases
      /// TODO finish for container sorts and structured sorts
      /// \param[in] e a sort expression
      sort_expression normalise(sort_expression const& e) const
      {
        if (e.is_basic_sort())
        {
          return find_referenced_sort(e);
        }

        return e;
      }

      /// \brief Follows aliases to the sort expression for which
      /// constructors, mappings equations are added
      ///
      /// \param[in] s A sort basic sort.
      /// \return the final sort referenced by s or s in case of failure
      sort_expression find_referenced_sort(basic_sort const& s) const
      {
        sort_expression found = s;

        std::set< sort_expression > visited;

        do {
          visited.insert(found);

          // search aliases for a reference that matches s
          ltr_aliases_map::const_iterator result(m_aliases_by_name.find(found));

          if (result != m_aliases_by_name.end())
          {
            found = result->second;
          }
        } while (found.is_basic_sort() && visited.find(found) == visited.end());

        return found;
      }

      /// \brief Removes sort from specification.
      ///
      /// Note that this also removes aliases for the sort but does not remove
      /// constructors, mappings and equations.
      /// \param[in] s A sort expression.
      /// \post s does not occur in this specification.
      /// \note this operation does not invalidate iterators of sorts_const_range, only if they point to the element that is removed
      void remove_sort(const sort_expression& s)
      {
        if (is_system_defined(s))
        {
          m_sys_sorts.remove(s);
        }
        if (s.is_alias())
        {
          remove_alias(s);
        }
        else {
          std::set< alias > aliases_of_s(convert< std::set< alias > >(aliases(s)));

          for (std::set< alias >::const_iterator i = aliases_of_s.begin(); i != aliases_of_s.end(); ++i)
          {
            remove_alias(*i);
          }

          m_sorts.erase(s);
        }
      }

      /// \brief Removes alias from specification.
      /// \post !search_sort(a.name()) && !is_alias(a.name())
      void remove_alias(alias const& a)
      {
        m_sorts.erase(a.name());
        m_sorts.erase(a); // TODO do aliases need to be part of the set of sorts -- jwulp

        m_aliases_by_sort.erase(std::find(m_aliases_by_sort.lower_bound(a.reference()),
                                          m_aliases_by_sort.upper_bound(a.reference()),
                                      rtl_aliases_map::value_type(a.reference(), a.name())));
        m_aliases_by_name.erase(a.name());
      }

      /// \brief Removes sorts from specification.
      ///
      /// \param[in] sl A range of sorts.
      /// \post for all s in sl: s no in sorts()
      template < typename ForwardTraversalIterator >
      void remove_sorts(const boost::iterator_range< ForwardTraversalIterator >& sl)
      {
        for (ForwardTraversalIterator i = sl.begin(); i != sl.end(); ++i)
        {
          remove_sort(*i);
        }
      }

      /// \brief Removes constructor from specification.
      ///
      /// Note that this does not remove equations containing the constructor.
      /// \param[in] f A constructor.
      /// \pre f occurs in the specification as constructor.
      /// \post f does not occur as constructor.
      /// \note this operation does not invalidate iterators of constructors_const_range, only if they point to the element that is removed
      void remove_constructor(const function_symbol& f)
      {
        constructors_const_range cs(constructors());
        assert(std::count(cs.begin(), cs.end(), f) != 0);
        if (is_system_defined(f))
        {
          m_sys_constructors.remove(f);
        }

        boost::iterator_range< sort_to_symbol_map::iterator > r(m_constructors.equal_range(normalise(f.sort().target_sort())));

        sort_to_symbol_map::iterator i =
                std::find(r.begin(), r.end(), sort_to_symbol_map::value_type(f.sort().target_sort(), f));

        if (i != m_constructors.end()) {
          m_constructors.erase(i);
        }
      }

      /// \brief Removes constructors from specification.
      ///
      /// \param[in] cl A range of constructors.
      /// \post for all c in cl: c not in constructors()
      template < typename ForwardTraversalIterator >
      void remove_constructors(const boost::iterator_range< ForwardTraversalIterator >& cl)
      {
        for (ForwardTraversalIterator i = cl.begin(); i != cl.end(); ++i)
        {
          remove_constructor(*i);
        }
      }

      /// \brief Removes mapping from specification.
      ///
      /// Note that this does not remove equations in which the mapping occurs.
      /// \param[in] f A function.
      /// \post f does not occur as constructor.
      /// \note this operation does not invalidate iterators of mappings_const_range, only if they point to the element that is removed
      void remove_mapping(const function_symbol& f)
      {
        if (is_system_defined(f))
        {
          m_sys_mappings.remove(f);
        }

        boost::iterator_range< sort_to_symbol_map::iterator > r(m_mappings.equal_range(normalise(f.sort().target_sort())));

        sort_to_symbol_map::iterator i =
                std::find(r.begin(), r.end(), sort_to_symbol_map::value_type(f.sort().target_sort(), f));

        if (i != m_constructors.end()) {
          m_constructors.erase(i);
        }
      }

      /// \brief Removes mappings from specification.
      ///
      /// \param[in] fl A range of constructors.
      /// \post for all f in fl: f not in mappings()
      template < typename ForwardTraversalIterator >
      void remove_mappings(const boost::iterator_range< ForwardTraversalIterator >& fl)
      {
        for (ForwardTraversalIterator i = fl.begin(); i != fl.end(); ++i)
        {
          remove_mapping(*i);
        }
      }

      /// \brief Removes equation from specification.
      ///
      /// \param[in] e An equation.
      /// \post e is removed from this specification.
      /// \note this operation does not invalidate iterators of equations_const_range, only if they point to the element that is removed
      void remove_equation(const data_equation& e)
      {
        if (is_system_defined(e))
        {
          m_sys_equations.remove(e);
        }
        m_equations.erase(std::find(m_equations.begin(), m_equations.end(), e));
      }

      /// \brief Removes equations from specification.
      ///
      /// \param[in] el A range of equations.
      /// \post for all e in el: e not in equations()
      template < typename ForwardTraversalIterator >
      void remove_equations(const boost::iterator_range< ForwardTraversalIterator >& el)
      {
        for (ForwardTraversalIterator i = el.begin(); i != el.end(); ++i)
        {
          remove_equation(*i);
        }
      }

      /// \brief Checks whether a sort is system defined.
      ///
      /// \param[in] s A sort expression.
      /// \return true iff s is system defined, false otherwise.
      bool is_system_defined(const sort_expression& s) const
      {
        return m_sys_sorts.get(s) != atermpp::aterm();
      }

      /// \brief Checks whether a function symbol is system defined.
      ///
      /// \param[in] f A function symbol.
      /// \return true iff f is system defined (either as constructor or as
      ///      mapping), false otherwise.
      bool is_system_defined(const function_symbol& f) const
      {
        return (m_sys_constructors.get(f) != atermpp::aterm() ||
                m_sys_mappings.get(f)    != atermpp::aterm());
      }

      /// \brief Checks whether an equation is system defined.
      ///
      /// \param[in] e An equation.
      /// \return true iff e is system defined, false otherwise.
      bool is_system_defined(const data_equation& e) const
      {
        return m_sys_equations.get(e) != atermpp::aterm();
      }

      /// \brief Checks whether two sort expressions represent the same sort
      ///
      /// \param[in] s1 A sort expression
      /// \param[in] s2 A sort expression
      bool equal_sorts(sort_expression const& s1, sort_expression const& s2) const
      {
        if (s1 == s2)
        {
          return true;
        }

        sort_expression normal_s1 = (s1.is_basic_sort()) ? find_referenced_sort(s1) : s1;
        sort_expression normal_s2 = (s2.is_basic_sort()) ? find_referenced_sort(s2) : s2;

        return normal_s1 == normal_s2;
      }

      /// \brief Checks whether a sort is certainly finite.
      ///
      /// \param[in] s A sort expression
      /// \return true if s can be determined to be finite,
      ///      false otherwise.
      bool is_certainly_finite(const sort_expression& s) const;

      /// \brief Checks whether all sort expressions are certainly finite.
      ///
      /// \param[in] s A range of sort expressions
      template < typename ForwardTraversalIterator >
      bool is_certainly_finite(const boost::iterator_range< ForwardTraversalIterator >& s) const
      {
        for (ForwardTraversalIterator i = s.begin(); i != s.end(); ++i) {
          if (!is_certainly_finite(*i)) {
            return false;
          }
        }

        return true;
      }

      ///\brief Adds the system defined sorts in a sequence
      void import_system_defined_sort(sort_expression const&);

      /// \brief Returns true if
      /// <ul>
      /// <li>the domain and range sorts of constructors are contained in the list of sorts</li>
      /// <li>the domain and range sorts of mappings are contained in the list of sorts</li>
      /// </ul>
      /// \return True if the data specification is well typed.
      bool is_well_typed() const;

      /// \brief Determines whether a basic sort is in fact a sort under alias
      ///
      /// param[in] s the target sort
      bool is_alias(const basic_sort& s) const
      {
        return m_aliases_by_name.find(s) != m_aliases_by_name.end();
      }

      /// \brief Returns true if the data specification contains the given sort
      /// param[in] s the target sort
      bool search_sort(const sort_expression& s) const
      {
        return m_sorts.find(s) != m_sorts.end() || (s.is_basic_sort() && is_alias(s));
      }

      /// \brief Returns true if the data specification contains the constructor
      bool search_constructor(const function_symbol& f) const
      {
        constructors_const_range range(m_constructors.equal_range(f.sort().target_sort()));

        return std::find(range.begin(), range.end(), f) != range.end();
      }

      /// \brief Returns true if the data specification contains the constructor
      bool search_mapping(const function_symbol& f) const
      {
        constructors_const_range range(m_mappings.equal_range(f.sort().target_sort()));

        return std::find(range.begin(), range.end(), f) != range.end();
      }

      /// \brief Returns true if the data specification contains the constructor
      bool search_equation(const data_equation& e) const
      {
        return std::find(m_equations.begin(), m_equations.end(), e) != m_equations.end();
      }

      bool operator==(const data_specification& other) const
      {
        return
          m_sorts == other.m_sorts &&
          m_aliases_by_name == other.m_aliases_by_name &&
          m_constructors == other.m_constructors &&
          m_mappings == other.m_mappings &&
          m_equations == other.m_equations;
      }

    }; // class data_specification

    /// \brief Removes all system defined sorts, constructors, mappings and equations
    /// \param specification A data specification
    /// \warning this makes a data specification incomplete with respect to system defined
    /// sorts, constructors, mappings and equations
    inline data_specification& remove_all_system_defined(data_specification& specification)
    {
      specification.purge_system_defined();

      return specification;
    }

    /// \brief Removes all system defined sorts, constructors, mappings and equations
    /// \param specification A data specification
    /// \warning this makes a data specification incomplete with respect to system defined
    /// sorts, constructors, mappings and equations
    inline data_specification remove_all_system_defined(data_specification const& specification)
    {
      data_specification new_specification(specification);

      new_specification.purge_system_defined();

      return new_specification;
    }

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DATA_SPECIFICATION_H

