///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains data data structures for the mcrl2 library.

#ifndef MCRL2_DATA_H
#define MCRL2_DATA_H

#include "atermpp/aterm.h"
#include "atermpp/aterm_algorithm.h"
#include "mcrl2/sort.h"
#include "mcrl2/list_iterator.h"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::aterm_list_iterator;

// prototypes
class DataVariable;
class Operation;
class DataApplication;

///////////////////////////////////////////////////////////////////////////////
// DataExpression
/// \brief data expression.
//
// There are three kind of expressions, namely expressions over sorts, over data
// and over processes.
//
// Zie DataApplication
// 
// sort -\> SortId("Nat")
//
class DataExpression
{
  protected:
    aterm_appl m_term;

  public:
    DataExpression()
    {}

    DataExpression(aterm_appl term)
      : m_term(term)
    {}

    /// Returns true if the data expression is an operation (OpId).
    ///
    // TODO: implement as an AFun comparison
    bool is_operation() const
    {
      return m_term.function().name() == "OpId"; 
    }

    /// Returns true if the data expression is a data variable (DataVarId).
    ///
    // TODO: implement as an AFun comparison
    bool is_data_variable() const
    {
      return m_term.function().name() == "DataVarId";
    }

    /// Returns true if the data expression is a data application (DataAppl).
    ///
    // TODO: implement as an AFun comparison
    bool is_data_application() const
    {
      return m_term.function().name() == "DataAppl"; 
    }

    /// Converts the DataExpression to a DataVariable. This function may
    /// only be called if is_data_variable() returns true. Only in debug
    /// mode an assertion check is done for this.
    ///
    DataVariable to_data_variable() const;

    /// Converts the DataExpression to an Operation. This function may
    /// only be called if is_operation() returns true. Only in debug
    /// mode an assertion check is done for this.
    ///
    Operation to_operation() const;

    /// Converts the DataExpression to a DataApplication. This function may
    /// only be called if is_data_application() returns true. Only in debug
    /// mode an assertion check is done for this.
    ///
    DataApplication to_data_application() const;

    /// Returns the head of the data expression.
    ///
    virtual DataExpression head() const
    {
      return DataExpression(make_term("DataExpression::head not yet implemented!"));
    }

    /// NOT YET IMPLEMENTED!
    /// Returns the arguments of the data expression.
    ///
    virtual std::list<DataExpression> arguments() const
    {
      return std::list<DataExpression>();
    }

    /// NOT YET IMPLEMENTED!
    /// Returns the sort of the data expression.
    ///
    virtual Sort sort() const
    {
      return Sort(make_term("DataExpression::sort not yet implemented!"));
    }

    /// Returns the data expression obtained by replacing lhs with rhs in
    /// all terms appearing in this data variable. This includes all data
    /// expressions and sort expressions appearing in sub-expressions!
    /// The replacement inside a term is not recursive. For example, if in
    /// the expression f(f(x)) the term f(x) is replaced by x, the result
    /// will be f(x) and not x.
    ///
    DataExpression replace(DataExpression lhs, DataExpression rhs) const
    {
      return DataExpression(atermpp::replace_non_recursive(m_term, lhs.m_term, rhs.m_term));
    }
  
    /// Returns the internal representation of the DataExpression.
    ///
    std::string to_string() const
    {
      return m_term.to_string();
    }

    /// Returns the data expression obtained by replacing lhs with rhs in
    /// all terms appearing in this data variable. This includes all data
    /// expressions and sort expressions appearing in sub-expressions!
    /// The replacement inside a term is not recursive. For example, if in
    /// the expression f(f(x)) the term f(x) is replaced by x, the result
    /// will be f(x) and not x.
    ///
    // DataExpression replace(DataVariable lhs, DataExpression rhs) const;    
    
    /// Returns the data expression obtained by applying a sequence of
    /// replacements to all terms appearing in this data variable.
    /// The substitutions are of type ATerm -\> ATerm. Always the first
    /// possible match is applied(?).
    /// x -\> a(y)
    /// b(x) -\> q(s)
    ///
    //template <typename FwdIt>
    //replace(FwdIt first, FwdIt last);
};


///////////////////////////////////////////////////////////////////////////////
// DataVariable
/// \brief data variable.
//
// g
// 
// DataVarId("g",SortArrow(SortId("Bool"),SortId("Nat")))
// 
// name -\> "b"
// type -\> SortArrow(SortId("Bool"),SortId("Nat"))
//
class DataVariable: public DataExpression
{
  public:
    DataVariable()
    {}

    DataVariable(aterm_appl t)
     : DataExpression(t)
    {}

    /// Returns the name of the DataVariable.
    ///
    std::string name() const
    {
      return m_term.argument(0).to_string();
    }

    /// Returns the head of the DataVariable.
    ///
    DataExpression head() const
    {
      return *this;
    }

    /// Returns the arguments of the DataVariable.
    ///
    std::list<DataExpression> arguments() const
    {
      return std::list<DataExpression>();
    }

    /// Returns the sort of the DataVariable.
    ///
    Sort sort() const
    {
      return Sort(m_term.argument(1));
    }
};


///////////////////////////////////////////////////////////////////////////////
// Operation
/// \brief operation identifier.
///
/// An operation identifier consists of a name and a type.
/// There are system defined operation identifiers for booleans, numbers, lists, sets,
/// bags and lambda expressions. The user can also declare custom operation identifiers.
///
/// f
/// 
/// OpId("f",SortArrow(SortId("Nat"),SortId("Nat")))
/// 
/// name -\> "f"
/// type -\> SortArrow(SortId("Nat"),SortId("Nat"))
/// 
/// 
/// SortArrow
/// 
/// (A-\>B)-\>C-\>D
/// 
/// SortArrow(SortArrow(SortId("A"),SortId("B")),SortArrow(SortId("C"),SortId("D")))
/// 
/// domain -\> A-\>B, C
/// range -\> D
/// lhs -\> A-\>B
/// rhs -\> C-\>D
///
class Operation: public DataExpression
{
  public:
    Operation()
    {}

    Operation(aterm_appl t)
     : DataExpression(t)
    {}

    /// Returns the name of the data variable.
    ///
    std::string name() const
    {
      return m_term.argument(0).to_string();
    }

    /// Returns the head of the Operation.
    ///
    DataExpression head() const
    {
      return *this;
    }

    /// Returns the arguments of the Operation.
    ///
    std::list<DataExpression> arguments() const
    {
      return std::list<DataExpression>();
    }

    /// Returns the sort of the Operation.
    ///
    Sort sort() const
    {
      return Sort(m_term.argument(1));
    }
};

///////////////////////////////////////////////////////////////////////////////
// DataApplication
/// \brief data application.
//
// n+m
// 
// DataAppl(
//    DataAppl(
//      OpId("+",SortArrow(SortId("Nat"),SortArrow(SortId("Nat"),SortId("Nat")))),
//      DataVarId("n",SortId("Nat")
//    ),
//    DataVarId("m",SortId("Nat")
// )
// 
// head -\>
// OpId("+",SortArrow(SortId("Nat"),SortArrow(SortId("Nat"),SortId("Nat"))))
// arguments -\> DataVarId("n",SortId("Nat"), DataVarId("m",SortId("Nat") lhs -\>
// DataAppl(
//      OpId("+",SortArrow(SortId("Nat"),SortArrow(SortId("Nat"),SortId("Nat")))),
//      DataVarId("n",SortId("Nat")
//    )
// rhs -\> DataVarId("m",SortId("Nat")
//
class DataApplication: public DataExpression
{
  protected:
    aterm_appl                      m_term;        // keep the original aterm for reference
    aterm_appl                      m_lhs;         // left hand side of the data application
    aterm_appl                      m_rhs;         // right hand side of the data application

  public:
    DataApplication(aterm_appl t)
     : DataExpression(t)
    {
      aterm_list_iterator i = m_term.argument_list().begin();
      m_lhs = *i++;
      m_rhs = *i;
    }

    /// Returns the left hand side of the DataApplication.
    ///
    DataExpression lhs() const
    {
      return DataExpression(m_lhs);
    }

    /// Returns the right hand side of the DataApplication.
    ///
    DataExpression rhs() const
    {
      return DataExpression(m_rhs);
    }

    /// NOT YET IMPLEMENTED!
    /// Returns the head of the DataApplication.
    ///
    DataExpression head() const
    {
      return DataExpression(make_term("Not yet implemented!"));
    }

    /// NOT YET IMPLEMENTED!
    /// Returns the arguments of the DataApplication.
    ///
    std::list<DataExpression> arguments() const
    {
      return std::list<DataExpression>();
    }

    /// NOT YET IMPLEMENTED!
    /// Returns the sort of the DataApplication.
    ///
    Sort sort() const
    {
      return Sort(make_term("Not yet implemented!"));
    }

    /// Returns the internal representation of the DataApplication.
    ///
    std::string to_string() const
    {
      return m_term.to_string();
    }
};

///////////////////////////////////////////////////////////////////////////////
// DataEquation
/// \brief data equation.
//
// DataEquation
// 
// var b: Bool;
// eqn f(Cb(b)) = Ca;
// 
// DataEqnSpec([DataEqn([DataVarId("b",SortId("Bool"))],Nil,DataAppl(OpId("f",SortA
// rrow(SortId("S"),SortId("S")),DataAppl(OpId("Cb",SortArrow(SortId("Bool"),SortId
// ("S"))),DataVarId("b",SortId("Bool")))),OpId("Ca",SortId("S")))])
// 
// condition -\> Nil    (of OpId("true",SortId("Bool")); dit is nog even de vraag)
// lhs -\>
// DataAppl(OpId("f",SortArrow(SortId("S"),SortId("S")),DataAppl(OpId("Cb",SortArro
// w(SortId("Bool"),SortId("S"))),DataVarId("b",SortId("Bool"))))
// rhs -\> OpId("Ca",SortId("S"))
// variables -\> DataVarId("b",SortId("Bool"))
//
class DataEquation
{
  protected:
    aterm_appl m_term;         // keep the original aterm for reference
    aterm_list m_variables;    // elements are of type DataVariable
    aterm_appl m_condition;
    aterm_appl m_lhs;
    aterm_appl m_rhs;

  public:
    typedef list_iterator<DataVariable>   variable_iterator;

    DataEquation()
    {}

    DataEquation(aterm_appl t)
     : m_term(t)
    {
      aterm_list_iterator i = m_term.argument_list().begin();
      m_variables = *i++;
      m_condition = *i++;
      m_lhs       = *i++;
      m_rhs       = *i;
    } 

    /// Returns a begin iterator to the sequence of variables.
    ///
    variable_iterator variables_begin() const
    {
      return variable_iterator(m_variables);
    }

    /// Returns an end iterator to the sequence of variables.
    ///
    variable_iterator variables_end() const
    {
      return variable_iterator();
    }

    /// Returns the condition of the summand (must be of type bool).
    ///
    DataExpression condition() const
    {
      return DataExpression(m_condition);
    }

    /// Returns the left hand side of the Assignment.
    ///
    DataExpression lhs() const
    {
      return DataExpression(m_lhs);
    }

    /// Returns the right hand side of the Assignment.
    ///
    DataExpression rhs() const
    {
      return DataExpression(m_rhs);
    }

    /// Returns the internal representation of the DataEquation.
    ///
    std::string to_string() const
    {
      return m_term.to_string();
    }
};

/// inline implementations

inline
DataVariable DataExpression::to_data_variable() const
{
  return DataVariable(m_term);
}

inline
Operation DataExpression::to_operation() const
{
  return Operation(m_term);
}

inline
DataApplication DataExpression::to_data_application() const
{
  return DataApplication(m_term);
}

} // namespace mcrl

#endif // MCRL2_DATA_H
