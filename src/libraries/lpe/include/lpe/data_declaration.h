///////////////////////////////////////////////////////////////////////////////
/// \file data_declaration.h

#ifndef LPE_DATA_DECLARATION_H
#define LPE_DATA_DECLARATION_H

#include "atermpp/aterm.h"
#include "lpe/sort.h"
#include "lpe/function.h"
#include "lpe/data.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::aterm_traits;

///////////////////////////////////////////////////////////////////////////////
// data_declaration
/// \brief a data declaration of a mCRL specification.
///
// syntax: DataSpec(
//           SortSpec(list<sort> sorts),
//           ConsSpec(list<operation> constructors),
//           MapSpec(list<operation> mappings),
//           DataEqnSpec(list<data_equation> equations)
//         )
//
// <DataSpec>     ::= DataSpec(SortSpec(<SortDecl>*), ConsSpec(<OpId>*),
//                      MapSpec(<OpId>*), DataEqnSpec(<DataEqn>*)
class data_declaration: public aterm_appl
{

  protected:
    sort_list          m_sorts;       
    function_list      m_constructors;
    function_list      m_mappings;    
    data_equation_list m_equations;   

    /// Initialize the data_declaration with an aterm_appl.
    ///
    void init_term(aterm_appl t)
    {
      m_term = aterm_traits<aterm_appl>::term(t);
      aterm_appl::iterator i = t.begin();      
      m_sorts        = sort_list(aterm_appl(*i++).argument(0));
      m_constructors = function_list(aterm_appl(*i++).argument(0));
      m_mappings     = function_list(aterm_appl(*i++).argument(0));
      m_equations    = data_equation_list(aterm_appl(*i).argument(0));
    }

  public:
    typedef sort_list::iterator          sort_iterator;
    typedef function_list::iterator      function_iterator;
    typedef data_equation_list::iterator equation_iterator;

    data_declaration()
    {}

    data_declaration(aterm_appl t)
    {
      assert(gsIsDataSpec(t));
      init_term(t);
    }

    data_declaration(sort_list sorts, function_list constructors, function_list mappings, data_equation_list equations)
      : m_sorts(sorts),
        m_constructors(constructors),
        m_mappings(mappings),
        m_equations(equations)
    {
      m_term = reinterpret_cast<ATerm>(
        gsMakeDataSpec(
          gsMakeSortSpec(sorts),
          gsMakeConsSpec(constructors),
          gsMakeMapSpec(mappings),
          gsMakeDataEqnSpec(equations)
        )
      );
    }

    /// Returns the list of sorts.
    ///
    sort_list sorts() const
    {
      return m_sorts;
    }

    /// Returns the list of constructors.
    ///
    function_list constructors() const
    {
      return m_constructors;
    }

    /// Returns the list of mappings.
    ///
    function_list mappings() const
    {
      return m_mappings;
    }

    /// Returns the list of equations.
    ///
    data_equation_list equations() const
    {
      return m_equations;
    }

    /// Sets the sequence of sorts.
    ///
    void set_sorts(sort_list sorts)
    { m_sorts = sorts; }

    /// Sets the sequence of constructors.
    ///
    void set_constructors(function_list constructors)
    { m_constructors = constructors; }

    /// Sets the sequence of mappings.
    ///
    void set_mappings(function_list mappings)
    { m_mappings = mappings; }

    /// Sets the sequence of data equations.
    ///
    void set_equations(data_equation_list equations)
    { m_equations = equations; }

    std::string to_string() const
    {
      std::string s1 = m_sorts.to_string();
      std::string s2 = m_constructors.to_string();
      std::string s3 = m_mappings.to_string();
      std::string s4 = m_equations.to_string();
      return s1 + "\n" + s2 + "\n" + s3 + "\n" + s4;
    }

};

} // namespace mcrl

#endif // LPE_DATA_DECLARATION_H
