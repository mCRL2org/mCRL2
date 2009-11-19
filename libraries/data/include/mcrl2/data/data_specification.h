// Author(s): Jeroen Keiren, Jeroen van der Wulp, Jan Friso Groote
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

// data expressions
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/assignment.h"

// sorts
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/alias.h"
#include "mcrl2/data/standard.h"

namespace mcrl2 {

  namespace data {

    class data_specification;

    /// \cond INTERNAL_DOCS
    namespace detail {
      template < typename Term >
      Term apply_compatibility_renamings(const data_specification&, Term const&);
      template < typename Term >
      Term undo_compatibility_renamings(const data_specification&, Term const&);
      atermpp::aterm_appl data_specification_to_aterm_data_spec(const data_specification&, bool = false);
    }
    /// \endcond

    /// \brief data specification.
    ///
    /// \invariant The specification is complete with respect to standard sorts:
    ///  - for every s in sorts() all system-defined sorts in s are in sorts()
    ///  - for every f in constructors() all system-defined sorts in f.sort() are in sorts()
    ///  - for every f in mappings() all system-defined sorts in f.sort() are in sorts()
    ///  - for every e in equations() all system-defined sorts occurring in subexpressions of e are in sorts()
    ///  - for every s in sorts() the specification has the standard mappings/equations
    ///  - for sorts of non-standard mappings/equations:
    ///    - for every non-standard f in constructors() standard mappings/equations for f.sort() are in mappings() 
    ///    - for every non-standard f in mappings() standard mappings/equations for f.sort() are in mappings() 
    ///    - for every non-standard e in equations() all sorts of subexpressions are contained in mappings()
    /// \invariant For every system defined sort s, if search_sort(s) if and only if search_function(s)

    class data_specification
    {
      protected:

        /// \brief map from sort expression to constructors
        typedef atermpp::multimap< sort_expression, function_symbol > sort_to_symbol_map;

        /// \brief map from basic_sort (names) to sort expression
        typedef atermpp::map< basic_sort, sort_expression >           ltr_aliases_map;

        /// \brief map from sort expression to names
        // typedef std::multimap< sort_expression, basic_sort >          reverse_aliases_map;

      private:

        /// \cond INTERNAL_DOCS
        /// \brief projects a pair of sort and a constructor to the latter
        struct symbol_projection : public std::unary_function< sort_to_symbol_map::value_type const, function_symbol >
        {
          /// \brief Application to constant pair
          function_symbol const& operator()(sort_to_symbol_map::value_type const& v) const 
          { return v.second;
          }
        };

        struct convert_to_alias : public std::unary_function< ltr_aliases_map::value_type const, alias >
        {
          /// \brief Application to constant pair
          alias operator()(ltr_aliases_map::value_type const& e) const 
          { return alias(e.first,e.second);
          }
        };

        /// \endcond

      public:

        /// \brief iterator range over list of sort expressions
        typedef boost::iterator_range< atermpp::set< sort_expression >::iterator >                    sorts_range;
        /// \brief iterator range over constant list of sort expressions
        typedef boost::iterator_range< atermpp::set< sort_expression >::const_iterator >              sorts_const_range;

        /// \brief iterator range over list of sort expressions
        typedef boost::iterator_range< atermpp::set< variable >::iterator >                           variable_range;
        /// \brief iterator range over constant list of sort expressions
        typedef boost::iterator_range< atermpp::set< variable >::const_iterator >                     variable_const_range;

        /// \brief iterator over aliases (objects of type function_symbol)
        typedef boost::transform_iterator< convert_to_alias, ltr_aliases_map::iterator >          aliases_iterator;
        /// \brief const iterator over aliases (objects of type function_symbol)
        typedef boost::transform_iterator< convert_to_alias, ltr_aliases_map::const_iterator >    aliases_const_iterator;
        /// \brief iterator range over list of aliases
        typedef boost::iterator_range< aliases_iterator >                                             aliases_range;
        /// \brief iterator range over constant list of aliases
        typedef boost::iterator_range< aliases_const_iterator >                                       aliases_const_range;

        /// \brief iterator over constructors (objects of type function_symbol)
        typedef boost::transform_iterator< symbol_projection, sort_to_symbol_map::iterator >          constructors_iterator;
        /// \brief const iterator over constructors (objects of type function_symbol)
        typedef boost::transform_iterator< symbol_projection, sort_to_symbol_map::const_iterator >    constructors_const_iterator;
        /// \brief iterator range over constructors (objects of type function_symbol)
        typedef boost::iterator_range< constructors_iterator >                                        constructors_range;
        /// \brief const iterator range over constructors (objects of type function_symbol)
        typedef boost::iterator_range< constructors_const_iterator >                                  constructors_const_range;

        /// \brief iterator over mappings (objects of type function_symbol)
        typedef boost::transform_iterator< symbol_projection, sort_to_symbol_map::iterator >          mappings_iterator;
        /// \brief const iterator over mappings (objects of type function_symbol)
        typedef boost::transform_iterator< symbol_projection, sort_to_symbol_map::const_iterator >    mappings_const_iterator;
        /// \brief iterator range over mappings (objects of type function_symbol)
        typedef boost::iterator_range< mappings_iterator >                                            mappings_range;
        /// \brief const iterator range over mappings (objects of type function_symbol)
        typedef boost::iterator_range< mappings_const_iterator >                                      mappings_const_range;

        /// \brief iterator range over list of data equations (objects of type data_equation)
        typedef boost::iterator_range< atermpp::set< data_equation >::iterator >                      equations_range;
        /// \brief iterator range over constant list of data equations (objects of type data_equation)
        typedef boost::iterator_range< atermpp::set< data_equation >::const_iterator >                equations_const_range;

      private:

        // friend data_specification& remove_all_system_defined(data_specification&);
        // friend data_specification  remove_all_system_defined(data_specification const&);
        friend atermpp::aterm_appl detail::data_specification_to_aterm_data_spec(const data_specification&, bool);

        ///\brief Adds system defined sorts and standard mappings for all internally used sorts
        void make_complete() const;

        ///\brief Normalise sorts.
        sort_expression normalise_sorts_helper(const sort_expression & e) const;

        /// \brief Helper function for make_complete() methods
        template < typename Term >
        void gather_sorts(Term const& term, std::set< sort_expression >& sorts);

        ///\brief Builds a specification from aterm
        void build_from_aterm(const atermpp::aterm_appl& t);

        void reconstruct_m_normalised_aliases() const;

      protected:

        /// \brief This boolean indicates whether the variables 
        /// m_normalised_constructors, m_mappings, m_equations, m_normalised_sorts,
        /// m_normalised_aliases.
        mutable bool m_normalised_data_is_up_to_date;

        /// \brief The basic sorts and structured sorts in the specification.
        atermpp::set< sort_expression >     m_sorts;

        /// \brief The sorts that occur in the context of this data specification.
        /// The normalised sorts, constructors, mappings and equations are complete
        /// with respect to these sorts. 
        mutable atermpp::set< sort_expression >     m_sorts_in_context;

        /// \brief The basic sorts and structured sorts in the specification.
        ltr_aliases_map                     m_aliases;

        /// \brief The basic sorts and structured sorts in the specification.
        // reverse_aliases_map                     m_aliases_by_sort;

        /// \brief A mapping of sort expressions to the constructors corresponding to that sort.
        sort_to_symbol_map                  m_constructors;

        /// \brief The mappings of the specification.
        sort_to_symbol_map                  m_mappings;

        /// \brief The equations of the specification.
        atermpp::set< data_equation >       m_equations;

        /// \brief Set containing all the sorts, including the system defined ones.
        mutable atermpp::set< sort_expression >         m_normalised_sorts;

        /// \brief Set containing system defined functions.
        // std::set< function_symbol >         m_sys_functions;

        /// \brief Set containing system defined all constructors, including the system defined ones.
        /// The types in these constructors are normalised.
        mutable sort_to_symbol_map         m_normalised_constructors;
        
        /// \brief Set containing system defined all mappings, including the system defined ones.
        /// The types in these mappings are normalised.
        mutable sort_to_symbol_map         m_normalised_mappings;
        //
        /// \brief Table containing all equations, including the system defined ones.
        ///        The sorts in these equations are normalised.
        mutable atermpp::set< data_equation >           m_normalised_equations;

        /// \brief Table containing how sorts should be mapped to normalised sorts.
        // sort_normaliser               m_sort_normaliser;
        mutable atermpp::map< sort_expression, sort_expression > m_normalised_aliases;

        void data_is_not_necessarily_normalised_anymore() const
        { m_normalised_data_is_up_to_date=false;
        }

    public: 
        // The add_system_defined should be private or protected. Currently they are
        // used by functions adding standard data types. The standard data types should
        // just provide the constructors, mappings and equations. The specification should
        // put these at the right spot. The type checker can the also use these.

        /// \brief Adds a sort to this specification, and marks it as system
        ///        defined
        ///
        /// \param[in] s A sort expression.
        /// \pre s does not yet occur in this specification.
        /// \post is_system_defined(s) = true
        /// \note this operation does not invalidate iterators of sorts_const_range
        void add_system_defined_sort(const sort_expression& s) const
        { 
          m_normalised_sorts.insert(normalise_sorts(s));
        }
  
        /// \brief Adds a constructor to this specification, and marks it as
        ///        system defined.
        ///
        /// \param[in] f A function symbol.
        /// \pre f does not yet occur in this specification.
        /// \post is_system_defined(f) == true
        /// \note this operation does not invalidate iterators of constructors_const_range
        inline
        void add_system_defined_constructor(const function_symbol& f) const
        { add_function(m_normalised_constructors,normalise_sorts(f));
        }
  
        /// \brief Adds a mapping to this specification, and marks it as system
        ///        defined.
        ///
        /// \param[in] f A function symbol.
        /// \pre f does not yet occur in this specification.
        /// \post is_system_defined(f) == true
        /// \note this operation does not invalidate iterators of mappings_const_range
        void add_system_defined_mapping(const function_symbol& f) const
        {
          // add_function(m_mappings, f);
          add_function(m_normalised_mappings,normalise_sorts(f));
        }
  
        /// \brief Adds an equation to this specification, and marks it as system
        ///        defined.
        ///
        /// \param[in] e An equation.
        /// \pre e does not yet occur in this specification.
        /// \post is_system_defined(f) == true
        /// \note this operation does not invalidate iterators of equations_const_range
        void add_system_defined_equation(const data_equation& e) const
        { const data_equation normalised_equation=normalise_sorts(e);
          m_normalised_equations.insert(normalised_equation);
        }
  
        /// \brief Adds equations to this specification, and marks them as system
        ///        defined.
        ///
        /// \param[in] fl A container with equations (objects of type convertible to data_equation).
        /// \post for all e in el: is_system_defined(e)
        /// \note this operation does not invalidate iterators of equations_const_range
        template < typename Container >
        void add_system_defined_equations(const Container& el,
                typename detail::enable_if_container< Container, data_equation >::type* = 0) const
        { for (typename Container::const_iterator i = el.begin(); i != el.end(); ++i)
          { add_system_defined_equation(*i); 
          }
        }
  
        /// \brief Adds constructors to this specification, and marks them as
        ///        system defined.
        ///
        /// \param[in] fl A container with function symbols (objects of type convertible to function_symbol).
        /// \post for all f in fl: is_system_defined(f)
        /// \note this operation does not invalidate iterators of constructors_const_range
        template < typename Container >
        void add_system_defined_constructors(const Container& fl,
                typename detail::enable_if_container< Container, function_symbol >::type* = 0) const
        {
          for (typename Container::const_iterator i = fl.begin(); i != fl.end(); ++i)
          {
            add_system_defined_constructor(*i);
          }
        }
  
        /// \brief Adds mappings to this specification, and marks them as system
        ///        defined.
        ///
        /// \param[in] fl A container with function symbols (objects of type convertible to function_symbol).
        /// \post for all f in fl: is_system_defined(f)
        /// \note this operation does not invalidate iterators of mappings_const_range
        template < typename Container >
        void add_system_defined_mappings(const Container& fl,
                typename detail::enable_if_container< Container, function_symbol >::type* = 0) const
        {
          for (typename Container::const_iterator i = fl.begin(); i != fl.end(); ++i)
          {
            add_system_defined_mapping(*i);
          }
        }
  
        /// \brief Adds sorts to this specification, and marks them as system
        /// defined.
        ///
        /// \param[in] sl A container with sort expressions (objects of type convertible to sort expression).
        /// \post for all s in sl: is_system_defined(s)
        /// \note this operation does not invalidate iterators of sorts_const_range
        template < typename Container >
        void add_system_defined_sorts(const Container& sl,
                typename detail::enable_if_container< Container, sort_expression >::type* = 0) const
        {
          for (typename Container::const_iterator i = sl.begin(); i != sl.end(); ++i)
          {
            add_system_defined_sort(*i);
          }
        }

    protected:

        void insert_mappings_constructors_for_structured_sort(const structured_sort &sort) const
        { 
          add_system_defined_sort(normalise_sorts(sort));

          structured_sort s_sort(sort);
          add_system_defined_constructors(s_sort.constructor_functions(sort));
          add_system_defined_mappings(s_sort.projection_functions(sort));
          add_system_defined_mappings(s_sort.recogniser_functions(sort));
          add_system_defined_equations(s_sort.constructor_equations(sort));
          add_system_defined_equations(s_sort.projection_equations(sort));
          add_system_defined_equations(s_sort.recogniser_equations(sort));

        }

        void remove_function(sort_to_symbol_map& container, const function_symbol& f)
        { boost::iterator_range< sort_to_symbol_map::iterator > r(container.equal_range(f.sort().target_sort()));

          sort_to_symbol_map::iterator i =
                  std::find(r.begin(), r.end(), sort_to_symbol_map::value_type(f.sort().target_sort(), f));
  
          if (i != container.end())
          { container.erase(i);
          }
        }

        void add_function(sort_to_symbol_map& container, const function_symbol& f) const
        { sort_expression index_sort(f.sort().target_sort());
          constructors_const_range relevant_range(container.equal_range(index_sort));
          if (std::find(relevant_range.begin(), relevant_range.end(), f) == relevant_range.end())
          { container.insert(std::make_pair(index_sort, f));
          }
        }

        void add_function(sort_to_symbol_map& container, const function_symbol& f)
        { sort_expression index_sort(f.sort().target_sort());
          constructors_const_range relevant_range(container.equal_range(index_sort));
          if (std::find(relevant_range.begin(), relevant_range.end(), f) == relevant_range.end())
          { container.insert(std::make_pair(index_sort, f));
          }
        }

        void add_standard_mappings_and_equations(sort_expression const& sort) const
        {
          function_symbol_vector symbols(standard_generate_functions_code(normalise_sorts(sort)));

          for (function_symbol_vector::const_iterator i = symbols.begin(); i != symbols.end(); ++i)
          { add_function(m_normalised_mappings,*i);
          }

          data_equation_vector equations(standard_generate_equations_code(sort));

          for (data_equation_vector::const_iterator i = equations.begin(); i != equations.end(); ++i)
          { add_system_defined_equation(*i);
          }
        }

      public:

      ///\brief Default constructor
      data_specification():m_normalised_data_is_up_to_date(false)
      { make_complete();
      }

      ///\brief Constructor from an aterm.
      /// \param[in] t a term adhering to the internal format.
      data_specification(const atermpp::aterm_appl& t):m_normalised_data_is_up_to_date(false)
      { build_from_aterm(t);
        make_complete();
      }

      ///\brief Constructor
      template < typename SortsRange, typename AliasesRange, typename ConstructorsRange,
                 typename MappingsRange, typename EquationsRange >
      data_specification(const SortsRange& sorts,
                         const AliasesRange& aliases,
                         const ConstructorsRange& constructors,
                         const MappingsRange& mappings,
                         const EquationsRange& equations):m_normalised_data_is_up_to_date(false)
      { add_sorts(sorts);
        add_constructors(constructors);
        add_mappings(mappings);
        add_equations(equations);
      }

      /// \brief Gets the sort declarations
      ///
      /// Time complexity of this operation is constant.
      /// \return The sort declarations of this specification.
      inline
      sorts_const_range sorts() const
      { normalise_specification_if_required();
        return sorts_const_range(m_normalised_sorts);
      }

      /// \brief Gets all constructors
      ///
      /// \return All constructors in this specification, including those for
      /// structured sorts.
      inline
      constructors_const_range constructors() const
      { normalise_specification_if_required();
        return constructors_const_range(m_normalised_constructors);
      }

      /// \brief Gets all constructors of a sort.
      ///
      /// Time complexity of this operation is constant.
      /// \param[in] s A sort expression.
      /// \return The constructors for sort s in this specification.
      inline
      constructors_const_range constructors(const sort_expression& s) const
      { normalise_specification_if_required();
        return constructors_const_range(m_normalised_constructors.equal_range(normalise_sorts(s)));
      }

      /// \brief Gets all mappings in this specification
      ///
      /// Time complexity of this operation is constant.
      /// \return All mappings in this specification, including recognisers and
      /// projection functions from structured sorts.
      inline
      mappings_const_range mappings() const
      { normalise_specification_if_required();
        return mappings_const_range(m_normalised_mappings);
      }

      /// \brief Gets all mappings of a sort
      ///
      /// \param[in] s A sort expression.
      /// \return All mappings in this specification, for which s occurs as a
      /// right-hand side of the mapping's sort.
      inline
      mappings_const_range mappings(const sort_expression& s) const
      { normalise_specification_if_required();
         return mappings_const_range(m_normalised_mappings.equal_range(normalise_sorts(s)));
      }

      /// \brief Gets all equations in this specification
      ///
      /// Time complexity of this operation is constant.
      /// \return All equations in this specification, including those for
      ///  structured sorts.
      inline
      equations_const_range equations() const
      { normalise_specification_if_required();
        return equations_const_range(m_normalised_equations);
      } 

      /// \brief Adds a sort to this specification
      ///
      /// \param[in] s A sort expression.
      /// \note this operation does not invalidate iterators of sorts_const_range
      void add_sort(const sort_expression& s)
      { atermpp::set < sort_expression >::size_type old_size=m_sorts.size();
        m_sorts.insert(s);
        if (old_size!=m_sorts.size())
        { data_is_not_necessarily_normalised_anymore();
        }
      }

      /// \brief Adds an alias (new name for a sort) to this specification
      ///
      /// \param[in] a an alias
      /// \pre !search_sort(s.name()) || is_alias(s.name()) || constructors(s.name()).empty()
      /// \note this operation does not invalidate iterators of aliases_const_range
      /// \post is_alias(s.name()) && normalise_sorts(s.name()) = normalise_sorts(s.reference())
      void add_alias(alias const& a)
      { assert(!search_sort(a.name()) || ((constructors(a.name()).empty()) && !is_alias(a.name())));
        m_aliases[a.name()] = a.reference(); 
        data_is_not_necessarily_normalised_anymore();
      }

      /// \brief Adds a constructor to this specification
      ///
      /// \param[in] f A function symbol.
      /// \pre a mapping f does not yet occur in this specification.
      /// \note this operation does not invalidate iterators of constructors_const_range
      void add_constructor(const function_symbol& f)
      { assert(!search_mapping(f));
        add_function(m_constructors, f);
        add_system_defined_constructor(f);
        make_complete(f.sort());
      }

      /// \brief Adds a mapping to this specification
      ///
      /// \param[in] f A function symbol.
      /// \pre a constructor f does not yet occur in this specification.
      /// \note this operation does not invalidate iterators of mappings_const_range
      void add_mapping(const function_symbol& f)
      { assert(!search_constructor(f));
        add_function(m_mappings, f);
        add_system_defined_mapping(f);
        make_complete(f.sort());
      }

      /// \brief Adds an equation to this specification
      ///
      /// \param[in] e An equation.
      /// \pre e does not yet occur in this specification.
      /// \note this operation does not invalidate iterators of equations_const_range
      void add_equation(const data_equation& e)
      { m_equations.insert(e);
        add_system_defined_equation(e);
        make_complete(e);
      }

      /// \brief Adds sorts to this specification
      ///
      /// \param[in] sl A container with sort expressions (objects of type convertible to sort expression).
      /// \note this operation does not invalidate iterators of sorts_const_range
      template < typename Container >
      void add_sorts(const Container& sl,
              typename detail::enable_if_container< Container, sort_expression >::type* = 0)
      { for (typename Container::const_iterator i = sl.begin(); i != sl.end(); ++i)
        { add_sort(*i);
        }
      }

      /// \brief Adds aliases to this specification
      ///
      /// \param[in] sl A container with sort expressions (objects of type convertible to sort expression).
      /// \note this operation does not invalidate iterators of sorts_const_range
      template < typename Container >
      void add_aliases(const Container& sl,
              typename detail::enable_if_container< Container, alias >::type* = 0)
      { for (typename Container::const_iterator i = sl.begin(); i != sl.end(); ++i)
        { add_alias(*i);
        }
      }

      /// \brief Adds constructors to this specification
      ///
      /// \param[in] fl A container with function symbols (objects of type convertible to function_symbol).
      /// \note this operation does not invalidate iterators of constructors_const_range
      template < typename Container >
      void add_constructors(const Container& fl,
              typename detail::enable_if_container< Container, function_symbol >::type* = 0)
      { for (typename Container::const_iterator i = fl.begin(); i != fl.end(); ++i)
        { add_constructor(*i);
        }
      }

      /// \brief Adds mappings to this specification
      ///
      /// \param[in] fl A container with function symbols (objects of type convertible to function_symbol).
      /// \note this operation does not invalidate iterators of mappings_const_range
      template < typename Container >
      void add_mappings(const Container& fl,
              typename detail::enable_if_container< Container, function_symbol >::type* = 0)
      { for (typename Container::const_iterator i = fl.begin(); i != fl.end(); ++i)
        { add_mapping(*i);
        }
      }

      /// \brief Adds equations to this specification
      ///
      /// \param[in] fl A container with equations (objects of type convertible to data_equation).
      /// \note this operation does not invalidate iterators of equations_const_range
      template < typename Container >
      void add_equations(const Container& el,
              typename detail::enable_if_container< Container, data_equation >::type* = 0) 
      { for (typename Container::const_iterator i = el.begin(); i != el.end(); ++i)
        { add_equation(*i);
        }
      }

  public:
      ///\brief Adds system defined sorts when necessary to make the specification complete
      /// \param[in] range an iterator range of objects: data/sort expressions,
      ///  equations, assignments for which the specificaiton should be complete
      /// \pre specification is complete, but not necessarily with respect to sorts in e
      /// \post specification has all constructors/mappings/equations for sorts referenced in range
      template < typename Container >
      void make_complete(Container const& range,
              typename detail::enable_if_container< Container >::type* = 0) const
      { std::set< sort_expression > sorts;
        for (typename Container::const_iterator i = range.begin(); i != range.end(); ++i)
        { gather_sorts(*i, sorts);
        }
        make_complete(sorts);
      }

      ///\brief Adds system defined sorts when necessary to make the specification complete
      /// \param[in] s a set of sort expressions that is added to a specification that is system-defined complete 
      /// \pre specification is complete, but not necessarily with respect to sorts in e
      /// \post specification has all constructors/mappings/equations for sorts in e
      void make_complete(std::set< sort_expression > const& s) const;

      ///\brief Adds system defined sorts when necessary to make the specification complete
      /// \param[in] e a data expression that is added to a specification that is system-defined complete 
      /// \pre specification is complete, but not necessarily with respect to sorts in e
      /// \post specification has all constructors/mappings/equations for sorts in e
      void make_complete(data_expression const& e) const;

      ///\brief Adds system defined sorts when necessary to make the specification complete
      /// \param[in] e a equation that is added to a specification that is system-defined complete 
      /// \pre specification is complete, but not necessarily with respect to sorts in e
      /// \post specification has all constructors/mappings/equations for sorts in e
      void make_complete(data_equation const& e) const;

      ///\brief Adds system defined sorts when necessary to make the specification complete
      /// \param[in] s a sort that is added to a specification that is system-defined complete 
      /// \pre specification is complete, but not necessarily with respect to s
      /// \post specification has all constructors/mappings/equations for s
      void make_complete(sort_expression const& s) const;

      ///\brief Normalises the sorts in the data specification
      ///\details See \ref normalise_sorts on arbitrary objects for a more detailed description.
      /// All sorts in the constructors, mappings and equations are normalised.
    
  private:

      void normalise_sorts() const;

      /// \brief 
      /// \details
      void normalise_specification_if_required() const
      { if (!m_normalised_data_is_up_to_date)
        { m_normalised_data_is_up_to_date=true;
          normalise_sorts();
        }
      }

      ///\brief Adds the system defined sorts in a sequence
      void import_system_defined_sort(sort_expression const&) const;

   public:
      /// \brief Normalises a sort expression by replacing sorts by a unique representative sort.
      /// \details Sort aliases and structured sorts have as effect that different sort names
      /// represent the same sort. E.g. after the alias sort A=B, the sort A and B are the same,
      /// and every object of sort A is an object of sort B. As all algorithms use syntactic equality
      /// to check whether objects are the same, the sorts A and B must be made equal. This is done
      /// by defining a unique representative for each sort, and to replace each sort by this representative.
      /// For sort aliases, the reprentative is always the sort at the right hand side, and for structured
      /// sorts the sort at the left hand side is taken.
      /// \param[in] e a sort expression
      /// \result a sort expression with normalised sorts satisfying 
      /// normalise_sorts(e) = normalise_sorts(normalise_sorts(e))
      sort_expression normalise_sorts(sort_expression const& e) const;
      
      /// \brief Normalises a data expression by replacing all sorts in it by a unique 
      /// representative sort.
      /// \details See the explanation of normalise_sorts(sort_expression).
      /// \param[in] e a data expression
      /// \result a data expression e with normalised sorts 
      data_expression normalise_sorts(data_expression const& e) const;

      /// \brief Normalises a variable v by replacing sorts in it by a unique representative sort.
      /// \details See the explanation of normalise_sorts(sort_expression).
      /// \param[in] v a variable
      /// \result a variable with a normalised sort expression
      variable normalise_sorts(variable const& v) const
      { normalise_specification_if_required();
        return variable(v.name(),normalise_sorts(v.sort()));
      }

 
      /// \brief Normalises the sorts in a function symbol.
      function_symbol normalise_sorts(function_symbol const& f) const;

      /// \brief Normalises an equation e by replacing sorts in it by a unique representative sort.
      /// \details See the explanation of normalise_sorts(sort_expression).
      /// \param[in] e a data_equation.
      /// \result a variable with a normalised sort expression
      data_equation normalise_sorts(const data_equation& e) const
      { normalise_specification_if_required();
        return data_equation(normalise_sorts(e.variables()),
                             normalise_sorts(e.condition()),
                             normalise_sorts(e.lhs()),
                             normalise_sorts(e.rhs())); 
      }

      /// \brief Normalises an assignment a by replacing sorts in it by a unique representative sort.
      /// \details See the explanation of normalise_sorts(sort_expression).
      /// \param[in] a an assignment
      /// \result an assignment with normalised sort expressions
      assignment normalise_sorts(assignment const& a) const
      { normalise_specification_if_required();
        return assignment(normalise_sorts(a.lhs()),normalise_sorts(a.rhs()));
      }

      /// \brief Normalises a atermpp list l by replacing sorts in it by a unique representative sort.
      /// \details See the explanation of normalise_sorts(sort_expression).
      /// \param[in] l a list of variables 
      /// \result a variable list with a normalised sort expression
      template < typename T>
      atermpp::term_list <T> normalise_sorts(atermpp::term_list < T > const& l) const
      { normalise_specification_if_required();
        atermpp::term_list <T> result;
        for(typename atermpp::term_list <T>::const_iterator i=l.begin();
                i!=l.end(); ++i)
        { result=push_front(result,normalise_sorts(*i));
        }
        return reverse(result);
      }

      /// \brief Removes sort from specification.
      /// Note that this also removes aliases for the sort but does not remove
      /// constructors, mappings and equations.
      /// \param[in] s A sort expression.
      /// \post s does not occur in this specification.
      /// \note this operation does not invalidate iterators of sorts_const_range, 
      /// only if they point to the element that is removed
      void remove_sort(const sort_expression& s)
      { m_sorts.erase(s);
        m_normalised_sorts.erase(normalise_sorts(s));
      }

      /// \brief Removes alias from specification.
      /// \post !search_sort(a.name()) && !is_alias(a.name())
      void remove_alias(alias const& a)
      { m_sorts.erase(a.name());
        m_aliases.erase(a.name());
        data_is_not_necessarily_normalised_anymore();
      }

      /// \brief Removes sorts from specification.
      ///
      /// \param[in] sl A range of sorts.
      /// \post for all s in sl: s no in sorts()
      template < typename SortsForwardRange >
      void remove_sorts(const SortsForwardRange& sl)
      { for (typename SortsForwardRange::iterator i = sl.begin(); i != sl.end(); ++i)
        { remove_sort(*i);
        }
      }

      /// \brief Removes constructor from specification.
      ///
      /// Note that this does not remove equations containing the constructor.
      /// \param[in] f A constructor.
      /// \pre f occurs in the specification as constructor.
      /// \post f does not occur as constructor.
      /// \note this operation does not invalidate iterators of constructors_const_range, 
      /// only if they point to the element that is removed
      void remove_constructor(const function_symbol& f)
      { remove_function(m_normalised_constructors,normalise_sorts(f));
        remove_function(m_constructors,f);
      }

      /// \brief Removes constructors from specification.
      ///
      /// \param[in] cl A range of constructors.
      /// \post for all c in cl: c not in constructors()
      template < typename ConstructorsForwardRange >
      void remove_constructors(const ConstructorsForwardRange& cl)
      { for (typename ConstructorsForwardRange::const_iterator i = cl.begin(); i != cl.end(); ++i)
        { remove_constructor(*i);
        }
      }

      /// \brief Removes mapping from specification.
      ///
      /// Note that this does not remove equations in which the mapping occurs.
      /// \param[in] f A function.
      /// \post f does not occur as constructor.
      /// \note this operation does not invalidate iterators of mappings_const_range, 
      /// only if they point to the element that is removed
      void remove_mapping(const function_symbol& f)
      { remove_function(m_mappings,f);
        remove_function(m_normalised_mappings,normalise_sorts(f));
      }

      /// \brief Removes mappings from specification.
      ///
      /// \param[in] fl A range of constructors.
      /// \post for all f in fl: f not in mappings()
      template < typename MappingsForwardRange >
      void remove_mappings(const MappingsForwardRange& fl)
      { for (typename MappingsForwardRange::const_iterator i = fl.begin(); i != fl.end(); ++i)
        { remove_mapping(*i);
        }
      }

      /// \brief Removes equation from specification.
      ///
      /// \param[in] e An equation.
      /// \post e is removed from this specification.
      /// \note this operation does not invalidate iterators of equations_const_range, 
      /// only if they point to the element that is removed
      void remove_equation(const data_equation& e)
      { m_equations.erase(e);
        m_normalised_equations.erase(normalise_sorts(e));
      }

      /// \brief Removes equations from specification.
      ///
      /// \param[in] el A range of equations.
      /// \post for all e in el: e not in equations()
      template < typename EquationsForwardRange >
      void remove_equations(const EquationsForwardRange& el)
      { for (typename EquationsForwardRange::const_iterator i = el.begin(); i != el.end(); ++i)
        { remove_equation(*i);
        }
      }

      /// \brief Checks whether two sort expressions represent the same sort
      ///
      /// \param[in] s1 A sort expression
      /// \param[in] s2 A sort expression
      bool equal_sorts(sort_expression const& s1, sort_expression const& s2) const
      { normalise_specification_if_required();
        const sort_expression normalised_sort1=normalise_sorts(s1);
        const sort_expression normalised_sort2=normalise_sorts(s2);
        return (normalised_sort1 == normalised_sort2);
      }

      /// \brief Checks whether a sort is certainly finite.
      ///
      /// \param[in] s A sort expression
      /// \return true if s can be determined to be finite,
      ///      false otherwise.
      bool is_certainly_finite(const sort_expression& s) const;

      /// \brief Checks whether a sort is a constructor sort
      ///
      /// \param[in] s A sort expression
      /// \return true if s is a constructor sort
      bool is_constructor_sort(const sort_expression& s) const
      { normalise_specification_if_required();
        const sort_expression normalised_sort=normalise_sorts(s);
        assert(search_sort(normalised_sort));
        return !normalised_sort.is_function_sort() && !constructors(normalised_sort).empty();
      }

      /// \brief Checks whether all sort expressions are certainly finite.
      ///
      /// \param[in] s A range of sort expressions
      template < typename ForwardRange >
      bool is_certainly_finite(const ForwardRange& s,
		 typename detail::enable_if_container< ForwardRange, sort_expression >::type* = 0) const
      { for (typename ForwardRange::const_iterator i = s.begin(); i != s.end(); ++i) {
          if (!is_certainly_finite(*i)) {
            return false;
          }
        }
        return true;
      }

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
        return m_aliases.find(s) != m_aliases.end();
      }

      /// \brief Returns true if the data specification contains the given sort
      /// param[in] s the target sort
      bool search_sort(const sort_expression& s) const
      { normalise_specification_if_required();
        return m_normalised_sorts.find(normalise_sorts(s)) != m_normalised_sorts.end() || 
               (s.is_basic_sort() && is_alias(s));
      }

      /// \brief Returns true if the data specification contains the constructor
      bool search_constructor(const function_symbol& f) const
      { normalise_specification_if_required();
        constructors_const_range range(m_normalised_constructors.equal_range(f.sort().target_sort()));
        return std::find(range.begin(), range.end(), f) != range.end();
      }

      /// \brief Returns true if the data specification contains the constructor
      /// \param[in] f the symbol to look for
      bool search_mapping(const function_symbol& f) const
      { normalise_specification_if_required();
        mappings_const_range range(m_mappings.equal_range(f.sort().target_sort()));

        return std::find(range.begin(), range.end(), f) != range.end();
      }

      /// \brief Returns true if the data specification contains a mapping or constructor that matches f
      /// \param[in] f the symbol to look for
      bool search_function(const function_symbol& f) const
      { normalise_specification_if_required();
        return search_constructor(f) || search_mapping(f);
      }

      /// \brief Returns true if the data specification contains the constructor
      /// \param[in] e the equation to look for
      bool search_equation(const data_equation& e) const
      { normalise_specification_if_required();
        return std::find(m_equations.begin(), m_equations.end(), e) != m_equations.end();
      }

      bool operator==(const data_specification& other) const
      { normalise_specification_if_required();
        other.normalise_specification_if_required();
        return
          // m_sorts_in_context == other.m_sorts_in_context &&
          m_normalised_sorts == other.m_normalised_sorts &&
          m_normalised_constructors == other.m_normalised_constructors &&
          m_normalised_mappings == other.m_normalised_mappings &&
          m_normalised_equations == other.m_normalised_equations;
      }

      data_specification &operator=(const data_specification &other) 
      { 
        m_normalised_data_is_up_to_date=other.m_normalised_data_is_up_to_date;
        m_sorts=other.m_sorts;
        m_sorts_in_context=other.m_sorts_in_context;
        m_aliases=other.m_aliases;
        m_constructors=other.m_constructors;
        m_mappings=other.m_mappings;
        m_equations=other.m_equations;
        m_normalised_sorts=other.m_normalised_sorts;
        m_normalised_mappings=other.m_normalised_mappings;
        m_normalised_constructors=other.m_normalised_constructors;
        m_normalised_equations=other.m_normalised_equations;
        m_normalised_aliases=other.m_normalised_aliases;
        return *this;
      }

    }; // class data_specification

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DATA_SPECIFICATION_H

