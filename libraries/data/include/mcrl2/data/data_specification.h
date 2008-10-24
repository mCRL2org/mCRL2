// Author(s): Jeroen Keiren
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

#include <iostream>
#include <algorithm>

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

// data expressions
#include "data_expression.h"
#include "function_symbol.h"
#include "application.h"

#include "data_equation.h"
#include "detail/compatibility.h"

namespace mcrl2 {
  
  namespace data {

    /// \brief data specification.
    class data_specification
    {
      private:

      /// \brief Determines the sorts on which a constructor depends
      ///
      /// \param[in] f A function symbol.
      /// \pre f is a constructor.
      /// \ret All sorts on which f depends.
      inline
      boost::iterator_range<sort_expression_list::const_iterator> dependent_sorts(const function_symbol& f, atermpp::set<sort_expression>& visited)
      {
        if (f.sort().is_basic_sort())
        {
          return boost::make_iterator_range(sort_expression_list());
        }
        else
        {
          sort_expression_list result;
          function_sort f_sort(f.sort());
          for (sort_expression_list::const_iterator i = f_sort.domain().begin(); i != f_sort.domain().end(); ++i)
          {
            result.push_back(*i);
            visited.insert(*i);
            boost::iterator_range<sort_expression_list::const_iterator> l(dependent_sorts(*i, visited));
            result.insert(result.end(), l.begin(), l.end());
          }
          return boost::make_iterator_range(result);
        }
      }

      /// \brief Determines the sorts on which a sort expression depends
      ///
      /// \param[in] s A sort expression.
      /// \ret All sorts on which s depends.
      inline
      boost::iterator_range<sort_expression_list::const_iterator> dependent_sorts(const sort_expression& s, atermpp::set<sort_expression>& visited)
      {
        if (visited.find(s) != visited.end())
        {
          return boost::make_iterator_range(sort_expression_list());
        }
        visited.insert(s);

        if (s.is_basic_sort())
        {
          sort_expression_list result;

          for (function_symbol_list::const_iterator i = constructors(s).begin(); i != constructors(s).end(); ++i)
          {
            boost::iterator_range<sort_expression_list::const_iterator> l(dependent_sorts(*i, visited));
            result.insert(result.end(), l.begin(), l.end());
          }

          return boost::make_iterator_range(result);
        }
        else if (s.is_container_sort())
        {
          return dependent_sorts(static_cast<container_sort>(s).element_sort(), visited);
        }
        else if (s.is_function_sort())
        {
          sort_expression_list result;
          function_sort fs(s);

          for (sort_expression_list::const_iterator i = fs.domain().begin(); i != fs.domain().end(); ++i)
          {
            boost::iterator_range<sort_expression_list::const_iterator> l(dependent_sorts(*i, visited));
            result.insert(result.end(), l.begin(), l.end());
          }

          boost::iterator_range<sort_expression_list::const_iterator> l(dependent_sorts(fs.codomain(), visited));
          result.insert(result.end(), l.begin(), l.end());
          return boost::make_iterator_range(result);
        }
        else if (s.is_structured_sort())
        {
          sort_expression_list result;
          boost::iterator_range<structured_sort_constructor_list::const_iterator> scl(static_cast<structured_sort>(s).struct_constructors());

          for (structured_sort_constructor_list::const_iterator i = scl.begin(); i != scl.end(); ++i)
          {
            boost::iterator_range<structured_sort_constructor_argument_list::const_iterator> scal(i->arguments());
            for (structured_sort_constructor_argument_list::const_iterator j = scal.begin(); j != scal.end(); ++j)
            {
              boost::iterator_range<sort_expression_list::const_iterator> sl(dependent_sorts(j->sort(), visited));
              result.insert(result.end(), sl.begin(), sl.end());
            }
          }

          return boost::make_iterator_range(result);
        }
        else
        {
          assert(false);
        }
      }

      protected:
        
        ///\brief The basic sorts and structured sorts in the specification.
        sort_expression_list m_sorts;

        ///\brief A mapping of sort expressions to the constructors
        ///corresponding to that sort.
        atermpp::map<sort_expression, function_symbol_list> m_constructors;

        ///\brief The functions of the specification.
        function_symbol_list m_functions;

        ///\brief The equations of the specification.
        data_equation_list m_equations;

        ///\brief Table containing system defined sorts.
        atermpp::table m_sys_sorts;

        ///\brief Table containing system defined constructors.
        atermpp::table m_sys_constructors;

        ///\brief Table containing system defined functions.
        atermpp::table m_sys_functions;

        ///\brief Table containing system defined equations.
        atermpp::table m_sys_equations;

      public:

      ///\brief Default constructor
      data_specification()
      {}

      ///\internal
      data_specification(const atermpp::aterm_appl& t)
        : m_sorts(detail::aterm_sort_spec_to_sort_expression_list(atermpp::arg1(t))),
          m_constructors(detail::aterm_cons_spec_to_constructor_map(atermpp::arg2(t))),
          m_functions(detail::aterm_map_spec_to_function_list(atermpp::arg3(t))),
          m_equations(detail::aterm_data_eqn_spec_to_equation_list(atermpp::arg4(t)))
      {}

      ///\brief Constructor
      data_specification(const boost::iterator_range<sort_expression_list::const_iterator>& sorts,
                         const boost::iterator_range<atermpp::map<sort_expression, function_symbol_list>::const_iterator>& constructors,
                         const boost::iterator_range<function_symbol_list::const_iterator>& functions,
                         const boost::iterator_range<data_equation_list::const_iterator>& equations)
        : m_sorts(sorts.begin(), sorts.end()),
          m_constructors(constructors.begin(), constructors.end()),
          m_functions(functions.begin(), functions.end()),
          m_equations(equations.begin(), equations.end())
      {}

      /// \brief Gets the sort declarations
      ///
      /// \ret The sort declarations of this specification.
      inline
      boost::iterator_range<sort_expression_list::const_iterator> sorts() const
      {
        return boost::make_iterator_range(m_sorts);
      }

      /// \brief Gets the aliases
      ///
      /// \param[in] s A sort expression
      /// \ret The aliases of sort s
      inline
      boost::iterator_range<alias_list::const_iterator> aliases(sort_expression& s) const
      {
        //TODO
        return boost::make_iterator_range(alias_list());
      }

      /// \brief Gets all constructors
      ///
      /// \ret All constructors in this specification, including those for
      /// structured sorts.

      inline
      function_symbol_list constructors() const
      {
        function_symbol_list result;
        for (atermpp::map<sort_expression, function_symbol_list>::const_iterator i = m_constructors.begin(); i != m_constructors.end(); ++i)
        {
          result.insert(result.end(), i->second.begin(), i->second.end());
        }
        result.ATprotectTerms();
        return result;
      }

      /// \brief Gets all constructors of a sort.
      ///
      /// \param[in] s A sort expression.
      /// \ret The constructors for sort s in this specification.
      inline
      boost::iterator_range<function_symbol_list::const_iterator> constructors(const sort_expression& s) const
      {
        if (m_constructors.find(s) == m_constructors.end())
        {
          return boost::make_iterator_range(function_symbol_list());
        }
        else
        {
          return boost::make_iterator_range(m_constructors.find(s)->second);
        }
      }

      /// \brief Gets all functions in this specification
      ///
      /// \ret All functions in this specification, including recognisers and
      /// projection functions from structured sorts.
      inline
      boost::iterator_range<function_symbol_list::const_iterator> functions() const
      {
        return boost::make_iterator_range(m_functions);
      }

      /// \brief Gets all functions of a sort
      ///
      /// \param[in] s A sort expression.
      /// \ret All functions in this specification, for which s occurs as a
      /// righthandside of the function's sort.
      inline
      function_symbol_list functions(const sort_expression& s) const
      {
        function_symbol_list result;
        for (function_symbol_list::const_iterator i = m_functions.begin(); i != m_functions.end(); ++i)
        {
          if(i->sort().is_function_sort())
          {
            if(static_cast<function_sort>(i->sort()).codomain() == s) //TODO check.
            {
              result.push_back(*i);
            }
          }
          else
          {
            if(i->sort() == s)
            {
              result.push_back(*i);
            }
          }
        }
        return result;
      }

      /// \brief Gets all equations in this specification
      ///
      /// \ret All equations in this specification, including those for
      ///  structured sorts.
      inline
      boost::iterator_range<data_equation_list::const_iterator> equations() const
      {
        return boost::make_iterator_range(m_equations);
      }

      /// \brief Gets all equations with a data expression as head
      /// on one of its sides.
      ///
      /// \param[in] d A data expression.
      /// \ret All equations with d as head in one of its sides.
      inline
      data_equation_list equations(const data_expression& d) const
      {
        data_equation_list result;
        for (data_equation_list::const_iterator i = m_equations.begin(); i != m_equations.end(); ++i)
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

      /// \brief Adds a sort to this specification
      ///
      /// \param[in] s A sort expression.
      /// \pre s does not yet occur in this specification.
      inline
      void add_sort(const sort_expression& s)
      {
        assert(std::find(m_sorts.begin(), m_sorts.end(), s) == m_sorts.end());
        m_sorts.push_back(s);
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
        function_symbol_list cs(constructors());
        assert(std::count(cs.begin(), cs.end(), f) == 0);
        assert(std::find(m_functions.begin(), m_functions.end(), f) == m_functions.end());
        sort_expression s;
        if (f.sort().is_function_sort())
        {
          s = static_cast<function_sort>(f.sort()).codomain();
        }
        else
        {
          s = f.sort();
        }
        m_constructors[s].push_back(f);
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

      /// \brief Adds a function to this specification
      ///
      /// \param[in] f A function symbol.
      /// \pre f does not yet occur in this specification.
      inline
      void add_function(const function_symbol& f)
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
        assert(std::count(m_functions.begin(), m_functions.end(), f) == 0);
        m_constructors.find(s)->second.push_back(f);
      }

      /// \brief Adds a function to this specification, and marks it as system
      ///        defined.
      ///
      /// \param[in] f A function symbol.
      /// \pre f does not yet occur in this specification.
      /// \post is_system_defined(f) == true
      inline
      void add_system_defined_function(const function_symbol& f)
      {
        add_function(f);
        m_sys_functions.put(f,f);
      }

      /// \brief Adds an equation to this specification
      ///
      /// \param[in] e An equation.
      /// \pre e does not yet occur in this specification.
      inline
      void add_equation(const data_equation& e)
      {
        assert(std::count(m_equations.begin(), m_equations.end(), e) == 0);
        m_equations.push_back(e);
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
      inline
      void add_sorts(const boost::iterator_range<sort_expression_list::const_iterator>& sl)
      {
        for (sort_expression_list::const_iterator i = sl.begin(); i != sl.end(); ++i)
        {
          add_sort(*i);
        }
      }

      /// \brief Adds sorts to this specification, and marks them as system
      /// defined.
      ///
      /// \param[in] sl A range of sort expressions.
      /// \post for all s in sl: is_system_defined(s)
      inline
      void add_system_defined_sorts(const boost::iterator_range<sort_expression_list::const_iterator>& sl)
      {
        for (sort_expression_list::const_iterator i = sl.begin(); i != sl.end(); ++i)
        {
          add_system_defined_sort(*i);
        }
      }

      /// \brief Adds constructors to this specification
      ///
      /// \param[in] fl A range of function symbols.
      inline
      void add_constructors(const boost::iterator_range<function_symbol_list::const_iterator>& fl)
      {
        for (function_symbol_list::const_iterator i = fl.begin(); i != fl.end(); ++i)
        {
          add_constructor(*i);
        }
      }

      /// \brief Adds constructors to this specification, and marks them as
      ///        system defined.
      ///
      /// \param[in] fl A range of function symbols.
      /// \post for all f in fl: is_system_defined(f)
      inline
      void add_system_defined_constructors(const boost::iterator_range<function_symbol_list::const_iterator>& fl)
      {
        for (function_symbol_list::const_iterator i = fl.begin(); i != fl.end(); ++i)
        {
          add_system_defined_constructor(*i);
        }
      }

      /// \brief Adds functions to this specification
      ///
      /// \param[in] fl A range of function symbols.
      inline
      void add_functions(const boost::iterator_range<function_symbol_list::const_iterator>& fl)
      {
        for (function_symbol_list::const_iterator i = fl.begin(); i != fl.end(); ++i)
        {
          add_function(*i);
        }
      }

      /// \brief Adds functions to this specification, and marks them as system
      ///        defined.
      ///
      /// \param[in] fl A range of function symbols.
      /// \post for all f in fl: is_system_defined(f)
      inline
      void add_system_defined_functions(const boost::iterator_range<function_symbol_list::const_iterator>& fl)
      {
        for (function_symbol_list::const_iterator i = fl.begin(); i != fl.end(); ++i)
        {
          add_system_defined_function(*i);
        }
      }

      /// \brief Adds equations to this specification
      ///
      /// \param[in] el A range of equations.
      inline
      void add_equations(const boost::iterator_range<data_equation_list::const_iterator>& el)
      {
        for (data_equation_list::const_iterator i = el.begin(); i != el.end(); ++i)
        {
          add_equation(*i);
        }
      }

      /// \brief Adds equations to this specification, and marks them as system
      ///        defined.
      ///
      /// \param[in] el A range of equations.
      /// \post for all e in el: is_system_defined(e)
      inline
      void add_system_defined_equations(const boost::iterator_range<data_equation_list::const_iterator>& el)
      {
        for (data_equation_list::const_iterator i = el.begin(); i != el.end(); ++i)
        {
          add_system_defined_equation(*i);
        }
      }

      /// \brief Removes sort from specification.
      ///
      /// Note that this does not remove constructors, functions and equations
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
      inline
      void remove_sorts(const boost::iterator_range<sort_expression_list::const_iterator>& sl)
      {
        for (sort_expression_list::const_iterator i = sl.begin(); i != sl.end(); ++i)
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
        function_symbol_list cs(constructors());
        assert(std::count(cs.begin(), cs.end(), f) != 0);
        if (is_system_defined(f))
        {
          m_sys_constructors.remove(f);
        }
        
        sort_expression s;
        if (f.sort().is_function_sort())
        {
          s = static_cast<function_sort>(f.sort()).codomain();
        }
        else
        {
          s = f.sort();
        }

        atermpp::map<sort_expression, function_symbol_list>::iterator i = m_constructors.find(s);
        i->second.erase(std::find(i->second.begin(), i->second.end(), f));        
      }

      /// \brief Removes constructors from specification.
      ///
      /// \param[in] cl A range of constructors.
      /// \post for all c in cl: c not in constructors()
      inline
      void remove_constructors(const boost::iterator_range<function_symbol_list::const_iterator>& cl)
      {
        for (function_symbol_list::const_iterator i = cl.begin(); i != cl.end(); ++i)
        {
          remove_constructor(*i);
        }
      }

      /// \brief Removes function from specification.
      ///
      /// Note that this does not remove equations containing the function.
      /// \param[in] f A function.
      /// \post f does not occur as constructor.
      inline
      void remove_function(const function_symbol& f)
      {
        if (is_system_defined(f))
        {
          m_sys_functions.remove(f);
        }
        m_functions.erase(std::find(m_functions.begin(), m_functions.end(), f));
      }

      /// \brief Removes functions from specification.
      ///
      /// \param[in] fl A range of constructors.
      /// \post for all f in fl: f not in functions()
      inline
      void remove_functions(const boost::iterator_range<function_symbol_list::const_iterator>& fl)
      {
        for (function_symbol_list::const_iterator i = fl.begin(); i != fl.end(); ++i)
        {
          remove_function(*i);
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
      inline
      void remove_equations(const boost::iterator_range<data_equation_list::const_iterator>& el)
      {
        for (data_equation_list::const_iterator i = el.begin(); i != el.end(); ++i)
        {
          remove_equation(*i);
        }
      }

      /// \brief Checks whether a sort is system defined.
      ///
      /// \param[in] s A sort expression.
      /// \ret true iff s is system defined, false otherwise.
      inline
      bool is_system_defined(const sort_expression& s)
      {
        return m_sys_sorts.get(s) != atermpp::aterm();
      }

      /// \brief Checks whether a function symbol is system defined.
      ///
      /// \param[in[ f A function symbol.
      /// \ret true iff f is system defined (either as constructor or as
      ///      mapping), false otherwise.
      inline
      bool is_system_defined(const function_symbol& f)
      {
        return (m_sys_constructors.get(f) != atermpp::aterm() ||
                m_sys_functions.get(f)    != atermpp::aterm());
      }

      /// \brief Checks wheter an equation is system defined.
      ///
      /// \param[in] e An equation.
      /// \ret true iff e is system defined, false otherwise.
      inline
      bool is_system_defined(const data_equation& e)
      {
        return m_sys_equations.get(e) != atermpp::aterm();
      }

      /// \brief Checks whether a sort is certainly finite.
      ///
      /// \param[in] s A sort expression
      /// \ret true if s can be determined to be finite,
      ///      false otherwise.
      inline
      bool is_certainly_finite(const sort_expression& s)
      {
        // Check for recursive occurrence.
        atermpp::set<sort_expression> visited;
        boost::iterator_range<sort_expression_list::const_iterator> dsl(dependent_sorts(s, visited));
        if (std::find(dsl.begin(), dsl.end(), s) != dsl.end())
        {
          return false;
        }

        if (s.is_basic_sort())
        {
          boost::iterator_range<function_symbol_list::const_iterator> fl(constructors(s));
          if (fl.empty())
          {
            return false;
          }

          for (function_symbol_list::const_iterator i = fl.begin(); i != fl.end(); ++i)
          {
            atermpp::set<sort_expression> visited;
            boost::iterator_range<sort_expression_list::const_iterator> sl(dependent_sorts(*i, visited));
            for (sort_expression_list::const_iterator j = sl.begin(); j != sl.end(); ++j)
            {
              if (!is_certainly_finite(*j))
              {
                return false;
              }
            }
          }
          return true;
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
          for (sort_expression_list::const_iterator i = fs.domain().begin(); i != fs.domain().end(); ++i)
          {
            if (!is_certainly_finite(*i))
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
          boost::iterator_range<sort_expression_list::const_iterator> sl(dependent_sorts(s, visited));
          for (sort_expression_list::const_iterator i = sl.begin(); i != sl.end(); ++i)
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

      /// \brief Returns a default expression for a sort.
      ///
      /// \param[in] s A sort expression.
      /// \ret Default expression of sort s.
      inline
      data_expression default_expression(const sort_expression& s)
      {
        return data_expression();
        //TODO
      }

    }; // class data_specification

    inline
    bool operator==(const data_specification& x, const data_specification& y)
    {
      return x.sorts() == y.sorts() &&
             x.constructors() == y.constructors() &&
             x.functions() == y.functions() &&
             x.equations() == y.equations();
    }

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DATA_SPECIFICATION_H

