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
#include <vector>

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
// #include "mcrl2/data/map_substitution.h"

// sorts
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/alias.h"
#include "mcrl2/data/standard.h"

#include "mcrl2/data/detail/container_utility.h"

namespace mcrl2 {

  namespace data {

    class data_specification;

    /// \cond INTERNAL_DOCS
    namespace detail 
    {
      atermpp::aterm_appl data_specification_to_aterm_data_spec(const data_specification&);
    }
    /// \endcond

    /// \brief data specification.

    class data_specification
    {
      protected:

        /// \brief map from sort expression to constructors
        typedef atermpp::multimap< sort_expression, data::function_symbol > sort_to_symbol_map;

        /// \brief map from basic_sort (names) to sort expression
        typedef atermpp::multimap< basic_sort, sort_expression >           ltr_aliases_map;

      private:

        /// \cond INTERNAL_DOCS
        /// \brief projects a pair of sort and a constructor to the latter
        struct symbol_projection : public std::unary_function< sort_to_symbol_map::value_type const, data::function_symbol >
        {
          /// \brief Application to constant pair
          data::function_symbol const& operator()(sort_to_symbol_map::value_type const& v) const
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

        friend atermpp::aterm_appl detail::data_specification_to_aterm_data_spec(const data_specification&);

        ///\brief Adds system defined sorts and standard mappings for all internally used sorts

        ///\brief Normalise sorts.
        sort_expression normalise_sorts_helper(const sort_expression & e) const;


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

        /// \brief A mapping of sort expressions to the constructors corresponding to that sort.
        sort_to_symbol_map                  m_constructors;

        /// \brief The mappings of the specification.
        sort_to_symbol_map                  m_mappings;

        /// \brief The equations of the specification.
        atermpp::set< data_equation >       m_equations;

        /// \brief Set containing all the sorts, including the system defined ones.
        mutable atermpp::set< sort_expression >         m_normalised_sorts;

        /// \brief Set containing all constructors, including the system defined ones.
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

    protected: 

        /// \brief Adds a sort to this specification, and marks it as system
        ///        defined
        ///
        /// \param[in] s A sort expression.
        /// \pre s does not yet occur in this specification.
        /// \post is_system_defined(s) = true
        /// \note this operation does not invalidate iterators of sorts_const_range
        void add_system_defined_sort(const sort_expression& s) const
        { m_normalised_sorts.insert(normalise_sorts(s));
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
        { add_function(m_normalised_mappings,normalise_sorts(f));
        }
  
        /// \brief Adds an equation to this specification, and marks it as system
        ///        defined.
        ///
        /// \param[in] e An equation.
        /// \pre e does not yet occur in this specification.
        /// \post is_system_defined(f) == true
        /// \note this operation does not invalidate iterators of equations_const_range
        void add_system_defined_equation(const data_equation& e) const
        { m_normalised_equations.insert(normalise_sorts(e));
        }
  
        /// \brief Adds constructors, mappings and equations for a structured sort
        ///        to this specification, and marks them as system defined.
        ///
        /// \param[in] sort A sort expression that is representing the structured sort.
        void insert_mappings_constructors_for_structured_sort(const structured_sort &sort) const
        { add_system_defined_sort(normalise_sorts(sort));

          structured_sort s_sort(sort);
          function_symbol_vector f(s_sort.constructor_functions(sort));
          std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
          f = s_sort.projection_functions(sort);
          std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
          f = s_sort.recogniser_functions(sort);
          std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));

          data_equation_vector e(s_sort.constructor_equations(sort));
          std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
          e = s_sort.projection_equations(sort);
          std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
          e = s_sort.recogniser_equations(sort);
          std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
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

      ///\brief Default constructor. Generate a data specification that contains
      ///       only booleans.
      data_specification()
        : m_normalised_data_is_up_to_date(false)
      {}

      ///\brief Constructor from an aterm.
      /// \param[in] t a term adhering to the internal format.
      data_specification(const atermpp::aterm_appl& t)
        : m_normalised_data_is_up_to_date(false)
      {
        build_from_aterm(t);
      }

      ///\brief Constructor
      template < typename SortsRange, typename AliasesRange, typename ConstructorsRange,
                 typename MappingsRange, typename EquationsRange >
      data_specification(const SortsRange& sorts,
                         const AliasesRange& aliases,
                         const ConstructorsRange& constructors,
                         const MappingsRange& mappings,
                         const EquationsRange& equations)
        : m_normalised_data_is_up_to_date(false)
      {
        std::for_each(sorts.begin(), sorts.end(),
           boost::bind(&data_specification::add_sort, this, _1));
        std::for_each(constructors.begin(), constructors.end(),
                   boost::bind(&data_specification::add_constructor, this, _1));
        std::for_each(mappings.begin(), mappings.end(),
                   boost::bind(&data_specification::add_mapping, this, _1));
        std::for_each(equations.begin(), equations.end(),
                   boost::bind(&data_specification::add_equation, this, _1));
      }

      /// \brief Gets all sort declarations including those that are system defined.
      ///
      /// \details The time complexity of this operation is constant, except when 
      ///      the data specification has been changed, in which case it can be that
      ///      it must be normalised again. This operation is linear in the size of 
      ///      the specification.
      /// \return The sort declarations of this specification.
      inline
      sorts_const_range sorts() const
      {
        normalise_specification_if_required();
        return sorts_const_range(m_normalised_sorts);
      }

      /// \brief Gets all sorts defined by a user (excluding the system defined sorts).
      ///
      /// \details The time complexity of this operation is constant.
      /// \return The user defined sort declaration.
      inline
      sorts_const_range user_defined_sorts() const
      {
        return sorts_const_range(m_sorts);
      }

      /// \brief Gets all constructors including those that are system defined.
      /// \details The time complexity is the same as for sorts().
      /// \return All constructors in this specification, including those for
      /// structured sorts.
      inline
      constructors_const_range constructors() const
      {
        normalise_specification_if_required();
        return constructors_const_range(m_normalised_constructors);
      }

      /// \brief Gets the constructors defined by the user, excluding those that
      /// are system defined.
      /// \details The time complexity for this operation is constant.
      inline
      constructors_const_range user_defined_constructors() const
      { 
        return constructors_const_range(m_constructors);
      }

      /// \brief Gets all constructors of a sort including those that are system defined.
      ///
      /// \details The time complexity is the same as for sorts().
      /// \param[in] s A sort expression.
      /// \return The constructors for sort s in this specification.
      inline
      constructors_const_range constructors(const sort_expression& s) const
      {
        normalise_specification_if_required();
        return constructors_const_range(m_normalised_constructors.equal_range(normalise_sorts(s)));
      }

      /// \brief Gets all mappings in this specification including those that are system defined.
      ///
      /// \brief The time complexity is the same as for sorts().
      /// \return All mappings in this specification, including recognisers and
      /// projection functions from structured sorts.
      inline
      mappings_const_range mappings() const
      {
        normalise_specification_if_required();
        return mappings_const_range(m_normalised_mappings);
      }

      /// \brief Gets all user defined mappings in this specification.
      ///
      /// \brief The time complexity is constant.
      /// \return All mappings in this specification, including recognisers and
      /// projection functions from structured sorts.
      inline
      mappings_const_range user_defined_mappings() const
      { 
        return mappings_const_range(m_mappings);
      }

      /// \brief Gets all mappings of a sort including those that are system defined
      ///
      /// \param[in] s A sort expression.
      /// \return All mappings in this specification, for which s occurs as a
      /// right-hand side of the mapping's sort.
      inline
      mappings_const_range mappings(const sort_expression& s) const
      {
        normalise_specification_if_required();
        return mappings_const_range(m_normalised_mappings.equal_range(normalise_sorts(s)));
      }

      /// \brief Gets all equations in this specification including those that are system defined
      ///
      /// \details The time complexity of this operation is the same as that for sort().
      /// \return All equations in this specification, including those for
      ///  structured sorts.
      inline
      equations_const_range equations() const
      {
        normalise_specification_if_required();
        return equations_const_range(m_normalised_equations);
      } 

      /// \brief Gets all user defined equations.
      ///
      /// \details The time complexity of this operation is constant. 
      /// \return All equations in this specification, including those for
      ///  structured sorts.
      inline
      equations_const_range user_defined_equations() const
      { 
        return equations_const_range(m_equations);
      } 

      /// \brief Gets a normalisation mapping that maps each sort to its unique normalised sort
      /// \details When in a specification sort aliases are used, like sort A=B or
      ///    sort Tree=struct leaf | node(Tree,Tree) then there are different representations
      ///    for each sort. The normalisation mapping maps each sort to a unique representant.
      ///    Moreover, it is this unique sort that it provides in internal mappings.
      const atermpp::map< sort_expression, sort_expression > &sort_alias_map() const
      {
        normalise_specification_if_required();
        return m_normalised_aliases;
      }

      /// \brief Gets the user defined aliases.
      /// \details The time complexity is constant.
      inline
      ltr_aliases_map user_defined_aliases() const
      { 
        return m_aliases;
      }

      /// \brief Return the user defined context sorts of the current specification.
      /// \details Time complexity is constant.
      sorts_const_range context_sorts()
      {
        return m_sorts_in_context;
      }

      /// \brief Adds a sort to this specification
      ///
      /// \param[in] s A sort expression.
      /// \note this operation does not invalidate iterators of sorts_const_range
      void add_sort(const sort_expression& s)
      {
        if(m_sorts.insert(s).second)
        {
          data_is_not_necessarily_normalised_anymore();
        }
      }

      /// \brief Adds an alias (new name for a sort) to this specification
      ///
      /// \param[in] a an alias
      /// \pre !search_sort(s.name()) || is_alias(s.name()) || constructors(s.name()).empty()
      /// \note this operation does not invalidate iterators of aliases_const_range
      /// \post is_alias(s.name()) && normalise_sorts(s.name()) = normalise_sorts(s.reference())
      void add_alias(alias const& a)
      {
        m_aliases.insert(std::pair<basic_sort, sort_expression>(a.name(),a.reference()));
        data_is_not_necessarily_normalised_anymore();
      }

      /// \brief Adds a constructor to this specification
      ///
      /// \param[in] f A function symbol.
      /// \pre a mapping f does not yet occur in this specification.
      /// \note this operation does not invalidate iterators of constructors_const_range
      void add_constructor(const function_symbol& f)
      {
        add_function(m_constructors, f);
        data_is_not_necessarily_normalised_anymore();
      }

      /// \brief Adds a mapping to this specification
      ///
      /// \param[in] f A function symbol.
      /// \pre a constructor f does not yet occur in this specification.
      /// \note this operation does not invalidate iterators of mappings_const_range
      void add_mapping(const function_symbol& f)
      {
        add_function(m_mappings, f);
        data_is_not_necessarily_normalised_anymore();
      }

      /// \brief Adds an equation to this specification
      ///
      /// \param[in] e An equation.
      /// \pre e does not yet occur in this specification.
      /// \note this operation does not invalidate iterators of equations_const_range
      void add_equation(const data_equation& e)
      {
        m_equations.insert(e);
        data_is_not_necessarily_normalised_anymore();
      }

      ///\brief Adds the sort s to the context sorts
      /// \param[in] s a sort expression. It is
      /// added to m_sorts_in_context. For this sort standard functions are generated
      /// automatically (if, <,<=,==,!=,>=,>) and if the sort is a standard sort,
      /// the necessary constructors, mappings and equations are added to the data type.
      void add_context_sort(const sort_expression& s) const
      {
        if (m_sorts_in_context.insert(s).second)
        {
          data_is_not_necessarily_normalised_anymore();
        }
      }

      ///\brief Adds the sorts in c to the context sorts
      /// \param[in] c a container of sort expressions. These are
      /// added to m_sorts_in_context. For these sorts standard functions are generated
      /// automatically (if, <,<=,==,!=,>=,>) and if the sorts are standard sorts,
      /// the necessary constructors, mappings and equations are added to the data type.
      template <typename Container>
      void add_context_sorts(const Container &c, typename detail::enable_if_container<Container>::type* = 0) const
      {
        std::for_each(c.begin(), c.end(),
            boost::bind(&data_specification::add_context_sort, this, _1));
      }

  private:

      ///\brief Normalises the sorts in the data specification
      ///\details See \ref normalise_sorts on arbitrary objects for a more detailed description.
      /// All sorts in the constructors, mappings and equations are normalised.
    
      void normalise_sorts() const;

      /// \brief 
      /// \details
      void normalise_specification_if_required() const
      {
        if (!m_normalised_data_is_up_to_date)
        {
          m_normalised_data_is_up_to_date=true;
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
      // template <typename Object> Object normalise_sorts(const Object& o) const;
      /* { normalise_specification_if_required();
        std::cerr << "Object " << o << "\n";
        substitution < Object, sort_expression, Object > sigma(m_normalised_aliases);
        return sigma(o);
      } */
      
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
      {
        m_sorts.erase(s);
        m_normalised_sorts.erase(normalise_sorts(s));
      }

      /// \brief Removes alias from specification.
      /// \post !search_sort(a.name()) && !is_alias(a.name())
      void remove_alias(alias const& a)
      {
        m_sorts.erase(a.name());
        m_aliases.erase(a.name());
        data_is_not_necessarily_normalised_anymore();
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
      {
        remove_function(m_normalised_constructors,normalise_sorts(f));
        remove_function(m_constructors,f);
      }

      /// \brief Removes mapping from specification.
      ///
      /// Note that this does not remove equations in which the mapping occurs.
      /// \param[in] f A function.
      /// \post f does not occur as constructor.
      /// \note this operation does not invalidate iterators of mappings_const_range, 
      /// only if they point to the element that is removed
      void remove_mapping(const function_symbol& f)
      {
        remove_function(m_mappings,f);
        remove_function(m_normalised_mappings,normalise_sorts(f));
      }

      /// \brief Removes equation from specification.
      ///
      /// \param[in] e An equation.
      /// \post e is removed from this specification.
      /// \note this operation does not invalidate iterators of equations_const_range, 
      /// only if they point to the element that is removed
      void remove_equation(const data_equation& e)
      {
        m_equations.erase(e);
        m_normalised_equations.erase(normalise_sorts(e));
      }

      /// \brief Checks whether two sort expressions represent the same sort
      ///
      /// \param[in] s1 A sort expression
      /// \param[in] s2 A sort expression
      bool equal_sorts(sort_expression const& s1, sort_expression const& s2) const
      {
        normalise_specification_if_required();
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
      {
        normalise_specification_if_required();
        const sort_expression normalised_sort=normalise_sorts(s);
        return !normalised_sort.is_function_sort() && !constructors(normalised_sort).empty();
      }

      /// \brief Returns true if
      /// <ul>
      /// <li>the domain and range sorts of constructors are contained in the list of sorts</li>
      /// <li>the domain and range sorts of mappings are contained in the list of sorts</li>
      /// </ul>
      /// \return True if the data specification is well typed.
      bool is_well_typed() const;

      bool operator==(const data_specification& other) const
      {
        normalise_specification_if_required();
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

