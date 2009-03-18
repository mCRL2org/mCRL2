// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/data_specification.h
/// \brief The class data_specification.

#ifndef MCRL2_NEW_DATA_DATA_SPECIFICATION_H
#define MCRL2_NEW_DATA_DATA_SPECIFICATION_H

#include <iostream>
#include <algorithm>

#include "boost/iterator/transform_iterator.hpp"
#include "boost/iterator/iterator_adaptor.hpp"

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/table.h"
#include "mcrl2/atermpp/set.h"

// sorts
#include "sort_expression.h"
#include "alias.h"
#include "container_sort.h"
#include "function_sort.h"
#include "structured_sort.h"

// new_data expressions
#include "data_expression.h"
#include "function_symbol.h"
#include "application.h"

#include "data_equation.h"
#include "mcrl2/new_data/detail/compatibility.h"
#include "mcrl2/new_data/detail/sequence_algorithm.h"
#include "mcrl2/new_data/detail/data_utility.h"
#include "mcrl2/new_data/utility.h"

namespace mcrl2 {

  namespace new_data {

    /// \brief new_data specification.
    class data_specification
    {
      protected:

        /// \brief map from sort expression to constructors
        typedef atermpp::multimap< sort_expression, function_symbol > constructors_map;

        struct constructor_projection : public std::unary_function< constructors_map::value_type const&, function_symbol const& >
        {
          function_symbol& operator()(constructors_map::value_type& v) const {
            return v.second;
          }
          function_symbol const& operator()(constructors_map::value_type const& v) const {
            return v.second;
          }
        };

        /// \brief iterates the aliases of a range of sort expressions
        template < typename ForwardTraversalIterator >
        class aliases_iterator : public
                boost::iterator_adaptor< aliases_iterator< ForwardTraversalIterator >,
                                ForwardTraversalIterator, alias, boost::use_default, alias >
        {
          friend class boost::iterator_core_access;

          private:

            sort_expression m_sort;

            void increment()
            {
              while (++(this->base_reference()) != ForwardTraversalIterator()) {
                if (this->base_reference()->is_alias() && (alias(*this->base_reference()).reference() == m_sort)) {
                  break;
                }
              }
            }

          public:

            aliases_iterator()
            { }

            explicit aliases_iterator(ForwardTraversalIterator const& i, sort_expression const& s) :
                                  aliases_iterator::iterator_adaptor_(i), m_sort(s)
            { }
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

        /// \brief iterator range over list of sort expressions
        typedef boost::iterator_range< aliases_iterator< sorts_range::iterator > >                    aliases_range;
        /// \brief iterator range over constant list of sort expressions
        typedef boost::iterator_range< aliases_iterator< sorts_const_range::iterator > >              aliases_const_range;

        /// \brief iterator over constructors
        typedef boost::transform_iterator< constructor_projection, constructors_map::iterator >       constructors_iterator;
        /// \brief const iterator over constructors
        typedef boost::transform_iterator< constructor_projection, constructors_map::const_iterator > constructors_const_iterator;
        /// \brief iterator range over constructors
        typedef boost::iterator_range< constructors_iterator >                                        constructors_range;
        /// \brief const iterator range over constructors
        typedef boost::iterator_range< constructors_const_iterator >                                  constructors_const_range;

        /// \brief iterator range over list of sort expressions
        typedef boost::iterator_range< atermpp::set< function_symbol >::iterator >                    mappings_range;
        /// \brief iterator range over constant list of sort expressions
        typedef boost::iterator_range< atermpp::set< function_symbol >::const_iterator >              mappings_const_range;

        /// \brief iterator range over list of data equations
        typedef boost::iterator_range< atermpp::set< data_equation >::iterator >                      equations_range;
        /// \brief iterator range over constant list of data equations
        typedef boost::iterator_range< atermpp::set< data_equation >::const_iterator >                equations_const_range;

      private:

      /// \brief Determines the sorts on which a constructor depends
      ///
      /// \param[in] f A function symbol.
      /// \pre f is a constructor.
      /// \return All sorts on which f depends.
      inline
      sort_expression_vector dependent_sorts(const function_symbol& f, atermpp::set<sort_expression>& visited) const
      {
        if (f.sort().is_basic_sort())
        {
          return sort_expression_vector();
        }
        else
        {
          sort_expression_vector result;
          function_sort f_sort(f.sort());
          for (function_sort::domain_const_range i(f_sort.domain()); !i.empty(); i.advance_begin(1))
          {
            result.push_back(i.front());
            visited.insert(i.front());
            sort_expression_vector l(dependent_sorts(i.front(), visited));
            result.insert(result.end(), l.begin(), l.end());
          }
          return result;
        }
      }

      /// \brief Determines the sorts on which a sort expression depends
      ///
      /// \param[in] s A sort expression.
      /// \return All sorts on which s depends.
      inline
      sort_expression_vector dependent_sorts(const sort_expression& s, atermpp::set<sort_expression>& visited) const
      {
        if (visited.find(s) != visited.end())
        {
          return sort_expression_vector();
        }
        visited.insert(s);

        if (s.is_basic_sort())
        {
          sort_expression_vector result;

          function_symbol_vector constructors_s(constructors(s));

          for (function_symbol_vector::const_iterator i = constructors_s.begin(); i != constructors_s.end(); ++i)
          {
            sort_expression_vector l(dependent_sorts(*i, visited));
            result.insert(result.end(), l.begin(), l.end());
          }

          return result;
        }
        else if (s.is_container_sort())
        {
          return dependent_sorts(static_cast<container_sort>(s).element_sort(), visited);
        }
        else if (s.is_function_sort())
        {
          sort_expression_vector result;
          function_sort fs(s);

          for (function_sort::domain_const_range i(fs.domain()); !i.empty(); i.advance_begin(1))
          {
            sort_expression_vector l(dependent_sorts(i.front(), visited));
            result.insert(result.end(), l.begin(), l.end());
          }

          sort_expression_vector l(dependent_sorts(fs.codomain(), visited));
          result.insert(result.end(), l.begin(), l.end());
          return result;
        }
        else if (s.is_structured_sort())
        {
          sort_expression_vector result;
          structured_sort::constructor_const_range scl(static_cast<structured_sort>(s).struct_constructors());

          for (structured_sort::constructor_const_range::const_iterator i = scl.begin(); i != scl.end(); ++i)
          {
            for (structured_sort_constructor::arguments_const_range j(i->arguments()); !j.empty(); j.advance_begin(1))
            {
              sort_expression_vector sl(dependent_sorts(j.front().sort(), visited));
              result.insert(result.end(), sl.begin(), sl.end());
            }
          }

          return result;
        }
        else
        {
          assert(false);
        }
      }

      protected:

        ///\brief The basic sorts and structured sorts in the specification.
        atermpp::set< sort_expression > m_sorts;

        ///\brief A mapping of sort expressions to the constructors
        ///corresponding to that sort.
        atermpp::multimap< sort_expression, function_symbol > m_constructors;

        ///\brief The mappings of the specification.
        atermpp::set< function_symbol > m_mappings;

        ///\brief The equations of the specification.
        atermpp::set< data_equation >   m_equations;

        ///\brief Table containing system defined sorts.
        atermpp::table m_sys_sorts;

        ///\brief Table containing system defined constructors.
        atermpp::table m_sys_constructors;

        ///\brief Table containing system defined mappings.
        atermpp::table m_sys_mappings;

        ///\brief Table containing system defined equations.
        atermpp::table m_sys_equations;

      public:

      ///\brief Default constructor
      data_specification()
      {}

      ///\internal
      data_specification(const atermpp::aterm_appl& t)
        : m_sorts(detail::aterm_sort_spec_to_sort_expression_set(atermpp::arg1(t))),
          m_constructors(detail::aterm_cons_spec_to_constructor_map(atermpp::arg2(t))),
          m_mappings(detail::aterm_map_spec_to_function_set(atermpp::arg3(t))),
          m_equations(detail::aterm_data_eqn_spec_to_equation_set(atermpp::arg4(t)))
      {}

      ///\brief Constructor
      data_specification(const atermpp::set< sort_expression >& sorts,
                         const atermpp::multimap<sort_expression, function_symbol >& constructors,
                         const atermpp::set< function_symbol >& mappings,
                         const atermpp::set< data_equation >& equations)
        : m_sorts(sorts.begin(), sorts.end()),
          m_constructors(constructors.begin(), constructors.end()),
          m_mappings(mappings.begin(), mappings.end()),
          m_equations(equations.begin(), equations.end())
      {}

      ///\brief Constructor
      template < typename SortsIterator, typename ConstructorsIterator,
                 typename MappingsIterator, typename EquationsIterator >
      data_specification(const boost::iterator_range< SortsIterator >& sorts,
                         const boost::iterator_range< ConstructorsIterator >& constructors,
                         const boost::iterator_range< MappingsIterator >& mappings,
                         const boost::iterator_range< EquationsIterator >& equations)
        : m_sorts(sorts.begin(), sorts.end()),
          m_mappings(mappings.begin(), mappings.end()),
          m_equations(equations.begin(), equations.end())
      {
        for(ConstructorsIterator i = constructors.begin();
                                          i != atermpp::term_list_iterator<function_symbol>(); ++i)
        {
          constructors.insert(constructors_map::value_type(i->sort().target_sort(), *i));
        }
      }

      /// \brief Gets the sort declarations
      ///
      /// \return The sort declarations of this specification.
      inline
      sorts_const_range sorts() const
      {
        return boost::make_iterator_range(m_sorts);
      }

      /// \brief Gets the aliases
      ///
      /// \param[in] s A sort expression
      /// \return The aliases of sort s
      inline
      aliases_const_range aliases(sort_expression const& s) const
      {
        return aliases_const_range(aliases_const_range::iterator(m_sorts.begin(), s), aliases_const_range::iterator());
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
      /// \param[in] s A sort expression.
      /// \return The constructors for sort s in this specification.
      inline
      function_symbol_vector constructors(const sort_expression& s) const
      {
        function_symbol_vector result = boost::copy_range< function_symbol_vector >(
                 boost::iterator_range< constructors_const_iterator >(m_constructors.equal_range(s)));

        if (s.is_standard() && m_sorts.find(s) == m_sorts.end()) {
          function_symbol_vector specification_result;
          function_symbol_vector standard_result;

          if (s == sort_pos::pos()) {
            standard_result = sort_pos::pos_generate_constructors_code();
          }
          else if (s == sort_nat::nat()) {
            standard_result = sort_nat::nat_generate_constructors_code();
          }
          else if (s == sort_int_::int_()) {
            standard_result = sort_int_::int__generate_constructors_code();
          }
          else if (s == sort_real_::real_()) {
            standard_result = sort_real_::real__generate_constructors_code();
          }

          specification_result.swap(result);

          std::sort(standard_result.begin(), standard_result.end());
          std::sort(specification_result.begin(), specification_result.end());
          std::set_union(standard_result.begin(), standard_result.end(),
            specification_result.begin(), specification_result.end(), std::inserter(result, result.end()));

        }

        return result;
      }

      /// \brief Gets all mappings in this specification
      ///
      /// \return All mappings in this specification, including recognisers and
      /// projection functions from structured sorts.
      inline
      mappings_const_range mappings() const
      {
        return boost::make_iterator_range(m_mappings);
      }

      /// \brief Gets all mappings of a sort
      ///
      /// \param[in] s A sort expression.
      /// \return All mappings in this specification, for which s occurs as a
      /// right-hand side of the mapping's sort.
      inline
      function_symbol_vector mappings(const sort_expression& s) const
      {
        function_symbol_vector result;
        for (atermpp::set< function_symbol >::const_iterator i = m_mappings.begin(); i != m_mappings.end(); ++i)
        {
          if(i->sort().is_function_sort())
          {
            if(static_cast<function_sort>(i->sort()).codomain() == s) //TODO check.
            {
              result.push_back(*i);
            }
          }
          else if(i->sort() == s)
          {
            result.push_back(*i);
          }
        }
        if (s.is_standard() && m_sorts.find(s) == m_sorts.end())
        {
          std::set< function_symbol > specification_result(result.begin(), result.end());
          function_symbol_vector standard_result;

          result.clear();

          if (s == sort_pos::pos()) {
            standard_result = sort_pos::pos_generate_functions_code();
          }
          else if (s == sort_nat::nat()) {
            standard_result = sort_nat::nat_generate_functions_code();
          }
          else if (s == sort_int_::int_()) {
            standard_result = sort_int_::int__generate_functions_code();
          }
          else if (s == sort_real_::real_()) {
            standard_result = sort_real_::real__generate_functions_code();
          }

          std::sort(standard_result.begin(), standard_result.end());

          std::set_union(standard_result.begin(), standard_result.end(),
            specification_result.begin(), specification_result.end(), std::inserter(result, result.end()));
        }

        return result;
      }

      /// \brief Gets all equations in this specification
      ///
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
      /// \pre s does not yet occur in this specification.
      inline
      void add_sort(const sort_expression& s)
      {
        assert(std::find(m_sorts.begin(), m_sorts.end(), s) == m_sorts.end());
        m_sorts.insert(s);
      }

      /// \brief Adds a sort to this specification, and marks it as system
      ///        defined
      ///
      /// \param[in] s A sort expression.
      /// \pre s does not yet occur in this specification.
      /// \post is_system_defined(s) = true
      inline
      void add_system_defined_sort(const sort_expression& s)
      {
        add_sort(s);
        m_sys_sorts.put(s,s);
      }

      /// \brief Adds a constructor to this specification
      ///
      /// \param[in] f A function symbol.
      /// \pre f does not yet occur in this specification.
      inline
      void add_constructor(const function_symbol& f)
      {
        constructors_const_range cs(constructors());
        assert(std::count(cs.begin(), cs.end(), f) == 0);
        assert(std::find(m_mappings.begin(), m_mappings.end(), f) == m_mappings.end());
        sort_expression s;
        if (f.sort().is_function_sort())
        {
          s = static_cast<function_sort>(f.sort()).codomain();
        }
        else
        {
          s = f.sort();
        }
        m_constructors.insert(std::make_pair(s, f));
      }

      /// \brief Adds a constructor to this specification, and marks it as
      ///        system defined.
      ///
      /// \param[in] f A function symbol.
      /// \pre f does not yet occur in this specification.
      /// \post is_system_defined(f) == true
      inline
      void add_system_defined_constructor(const function_symbol& f)
      {
        add_constructor(f);
        m_sys_constructors.put(f,f);
      }

      /// \brief Adds a mapping to this specification
      ///
      /// \param[in] f A function symbol.
      inline
      void add_mapping(const function_symbol& f)
      {
        sort_expression s;
        if (f.sort().is_function_sort())
        {
          s = static_cast<function_sort>(f.sort()).codomain();
        }
        else
        {
          s = f.sort();
        }
        if (m_constructors.find(s) != m_constructors.end())
        {
          function_symbol_list fl(m_constructors.find(s)->second);
          assert(std::count(fl.begin(), fl.end(), f) == 0);
        }

        m_mappings.insert(f);
      }

      /// \brief Adds a mapping to this specification, and marks it as system
      ///        defined.
      ///
      /// \param[in] f A function symbol.
      /// \pre f does not yet occur in this specification.
      /// \post is_system_defined(f) == true
      inline
      void add_system_defined_mapping(const function_symbol& f)
      {
        add_mapping(f);
        m_sys_mappings.put(f,f);
      }

      /// \brief Adds an equation to this specification
      ///
      /// \param[in] e An equation.
      /// \pre e does not yet occur in this specification.
      inline
      void add_equation(const data_equation& e)
      {
        assert(std::count(m_equations.begin(), m_equations.end(), e) == 0);
        m_equations.insert(e);
      }

      /// \brief Adds an equation to this specification, and marks it as system
      ///        defined.
      ///
      /// \param[in] e An equation.
      /// \pre e does not yet occur in this specification.
      /// \post is_system_defined(f) == true
      inline
      void add_system_defined_equation(const data_equation& e)
      {
        add_equation(e);
        m_sys_equations.put(e,e);
      }

      /// \brief Adds sorts to this specification
      ///
      /// \param[in] sl A range of sort expressions.
      template < typename ForwardTraversalIterator >
      inline
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
      template < typename ForwardTraversalIterator >
      inline
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
      template < typename ForwardTraversalIterator >
      inline
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
      template < typename ForwardTraversalIterator >
      inline
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
      template < typename ForwardTraversalIterator >
      inline
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
      template < typename ForwardTraversalIterator >
      inline
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
      template < typename ForwardTraversalIterator >
      inline
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
      template < typename ForwardTraversalIterator >
      inline
      void add_system_defined_equations(const boost::iterator_range< ForwardTraversalIterator >& el)
      {
        for (ForwardTraversalIterator i = el.begin(); i != el.end(); ++i)
        {
          add_system_defined_equation(*i);
        }
      }

      /// \brief Removes sort from specification.
      ///
      /// Note that this does not remove constructors, mappings and equations
      /// for a sort.
      /// \param[in] s A sort expression.
      /// \post s does not occur in this specification.
      inline
      void remove_sort(const sort_expression& s)
      {
        if (is_system_defined(s))
        {
          m_sys_sorts.remove(s);
        }

        m_sorts.erase(std::find(m_sorts.begin(), m_sorts.end(), s));
      }

      /// \brief Removes sorts from specification.
      ///
      /// \param[in] sl A range of sorts.
      /// \post for all s in sl: s no in sorts()
      template < typename ForwardTraversalIterator >
      inline
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
      inline
      void remove_constructor(const function_symbol& f)
      {
        constructors_const_range cs(constructors());
        assert(std::count(cs.begin(), cs.end(), f) != 0);
        if (is_system_defined(f))
        {
          m_sys_constructors.remove(f);
        }

        sort_expression s = (f.sort().is_function_sort()) ?
           static_cast<function_sort>(f.sort()).codomain() : f.sort();

        constructors_map::iterator i =
                std::find(m_constructors.begin(), m_constructors.end(), constructors_map::value_type(s, f));

        if (i != m_constructors.end()) {
          m_constructors.erase(i);
        }
      }

      /// \brief Removes constructors from specification.
      ///
      /// \param[in] cl A range of constructors.
      /// \post for all c in cl: c not in constructors()
      template < typename ForwardTraversalIterator >
      inline
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
      inline
      void remove_mapping(const function_symbol& f)
      {
        if (is_system_defined(f))
        {
          m_sys_mappings.remove(f);
        }
        m_mappings.erase(std::find(m_mappings.begin(), m_mappings.end(), f));
      }

      /// \brief Removes mappings from specification.
      ///
      /// \param[in] fl A range of constructors.
      /// \post for all f in fl: f not in mappings()
      template < typename ForwardTraversalIterator >
      inline
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
      inline
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
      inline
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
      inline
      bool is_system_defined(const sort_expression& s)
      {
        return m_sys_sorts.get(s) != atermpp::aterm();
      }

      /// \brief Checks whether a function symbol is system defined.
      ///
      /// \param[in[ f A function symbol.
      /// \return true iff f is system defined (either as constructor or as
      ///      mapping), false otherwise.
      inline
      bool is_system_defined(const function_symbol& f)
      {
        return (m_sys_constructors.get(f) != atermpp::aterm() ||
                m_sys_mappings.get(f)    != atermpp::aterm());
      }

      /// \brief Checks whether an equation is system defined.
      ///
      /// \param[in] e An equation.
      /// \return true iff e is system defined, false otherwise.
      inline
      bool is_system_defined(const data_equation& e)
      {
        return m_sys_equations.get(e) != atermpp::aterm();
      }

      /// \brief Checks whether a sort is certainly finite.
      ///
      /// \param[in] s A sort expression
      /// \return true if s can be determined to be finite,
      ///      false otherwise.
      inline
      bool is_certainly_finite(const sort_expression& s) const
      {
        // Check for recursive occurrence.
        atermpp::set<sort_expression> visited;
        sort_expression_vector dsl(dependent_sorts(s, visited));
        if (std::find(dsl.begin(), dsl.end(), s) != dsl.end())
        {
          return false;
        }

        if (s.is_basic_sort())
        {
          if (s.is_standard()) {
            return sort_bool_::is_bool_(s);
          }

          function_symbol_vector fl(constructors(s));

          for (function_symbol_vector::const_iterator i = fl.begin(); i != fl.end(); ++i)
          {
            atermpp::set<sort_expression> visited;
            sort_expression_vector sl(dependent_sorts(*i, visited));
            for (sort_expression_vector::const_iterator j = sl.begin(); j != sl.end(); ++j)
            {
              if (!is_certainly_finite(*j))
              {
                return false;
              }
            }
          }

          return !fl.empty();
        }
        else if (s.is_container_sort())
        {
          container_sort cs(s);
          if(cs.is_set_sort())
          {
            return is_certainly_finite(cs.element_sort());
          }
          return false;
        }
        else if (s.is_function_sort())
        {
          function_sort fs(s);
          for (function_sort::domain_const_range i(fs.domain()); !i.empty(); i.advance_begin(1))
          {
            if (!is_certainly_finite(i.front()))
            {
              return false;
            }
          }

          if (fs.codomain() == s)
          {
            return false;
          }

          return is_certainly_finite(fs.codomain());
        }
        else if (s.is_structured_sort())
        {
          atermpp::set<sort_expression> visited;
          sort_expression_vector sl(dependent_sorts(s, visited));
          for (sort_expression_vector::const_iterator i = sl.begin(); i != sl.end(); ++i)
          {
            if (!is_certainly_finite(*i))
            {
              return false;
            }
          }
          return true;
        }
        else
        {
          assert(false);
        }
      }

      /// \brief Checks whether all sort expressions are certainly finite.
      ///
      /// \param[in] s A range of sort expressions
      /// \return std::find_if(s.begin(), s.end(), std::bind1st(std::mem_fun(&data_specification::is_certainly_finite), this)) == s.end()
      template < typename ForwardTraversalIterator >
      inline
      bool is_certainly_finite(const boost::iterator_range< ForwardTraversalIterator >& s) const
      {
        for (ForwardTraversalIterator i = s.begin(); i != s.end(); ++i) {
          if (!is_certainly_finite(*i)) {
            return false;
          }
        }

        return true;
      }

      /// \brief Returns a default expression for a sort.
      ///
      /// \param[in] s A sort expression.
      /// \return Default expression of sort s.
      inline
      data_expression default_expression(const sort_expression& s)
      {
        return data_expression();
        //TODO
      }

      /// \brief Returns true if
      /// <ul>
      /// <li>the domain and range sorts of constructors are contained in the list of sorts</li>
      /// <li>the domain and range sorts of mappings are contained in the list of sorts</li>
      /// </ul>
      /// \return True if the data specification is well typed.
      bool is_well_typed() const
      {
        // check 1)
        if (!detail::check_data_spec_sorts(constructors(), m_sorts))
        {
          std::clog << "data_specification::is_well_typed() failed: not all of the sorts appearing in the constructors "
                    << pp(constructors()) << " are declared in " << pp(m_sorts) << std::endl;
          return false;
        }

        // check 2)
        if (!detail::check_data_spec_sorts(mappings(), m_sorts))
        {
          std::clog << "data_specification::is_well_typed() failed: not all of the sorts appearing in the mappings "
                    << pp(mappings()) << " are declared in " << pp(m_sorts) << std::endl;
          return false;
        }

        return true;
      }

    }; // class data_specification

    /// \brief Pretty prints a data specification
    /// \param[in] specification a data specification
    template < typename Container >
    inline std::string pp(data_specification const& specification)
    {
      return core::pp(core::detail::gsMakeDataSpec(
        detail::sort_expression_list_to_aterm_sort_spec(specification.sorts()),
               detail::constructor_list_to_aterm_cons_spec(specification.constructors()),
               detail::function_list_to_aterm_map_spec(specification.mappings()),
               detail::data_equation_list_to_aterm_eqn_spec(specification.equations())));
    }

    inline
    bool operator==(const data_specification& x, const data_specification& y)
    {
      return x.sorts() == y.sorts() &&
             x.constructors() == y.constructors() &&
             x.mappings() == y.mappings() &&
             x.equations() == y.equations();
    }

  } // namespace new_data

} // namespace mcrl2

#endif // MCRL2_NEW_DATA_DATA_SPECIFICATION_H

