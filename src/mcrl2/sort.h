///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains sort data structures for the mcrl2 library.

#ifndef MCRL2_SORT_H
#define MCRL2_SORT_H

#include <list>
#include "atermpp/aterm.h"
#include "mcrl2/list_iterator.h"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::make_term;

///////////////////////////////////////////////////////////////////////////////
// Sort
/// \brief sort expression.
///
/// Models sorts of shape <tt>A -\> B</tt>, where A is the domain and B the range. A constant sort
/// has an empty domain, for example <tt>-\> S</tt>, or simply <tt>S</tt>.
/// 
class Sort
{
  public:
    /// Constructs a sort with internal representation t.
    ///
    Sort(aterm_appl t)
      : m_term(t)
    {}

    /// Constructs a sort with the given domain and range.
    ///
    Sort(aterm_appl domain, aterm_appl range)
    {}

    /// Constructs a sort from a string. Currently the aterm string representation is
    /// used, but probably something like Sort s("A-\>(B-\>C)"); should be supported.
    ///
    Sort(std::string s)
      : m_term(make_term(s).to_appl())
    {}

    /// Returns the domain of the sort expression. Note that the domain is a list.
    /// For example the domain of the expression A -\> B -\> C is equal to [A,B].
    ///
    /// The domain of sort expression (A-\>B)-\>C-\>D is [A-\>B, C].
    std::list<Sort> domain() const
    {
      return std::list<Sort>();
    }
    
    /// Returns the range of the sort expression.
    /// For example the range of the expression <tt>A -\> B -\> C</tt> is equal to C.
    /// The range of sort expression (A-\>B)-\>C-\>D is D.
    ///
    Sort range() const
    {
      return Sort("empty");
    }

    /// Returns the left hand side of the sort expression.
    ///
    /// The left hand side of sort expression (A-\>B)-\>C-\>D is A-\>B.
    Sort lhs() const
    {
      return Sort("lhs");
    }   

    /// Returns the right hand side of the sort expression.
    ///
    /// The right hand side of sort expression (A-\>B)-\>C-\>D is C-\>D.
    Sort rhs() const
    {
      return Sort("rhs");
    }   

    /// Returns true if the sort is constant, i.e. has an empty domain.
    ///
    bool is_constant() const
    {
      return m_term.function().name() == "SortID";
    }

    /// Returns a string representation of the sort.
    ///
    std::string to_string() const
    {
      return m_term.to_string();
    }

    /// Under what conditions are two sorts considered equal? How about
    /// <tt>(A-\>B)-\>C</tt> and <tt>A-\>(B-\>C)</tt>?
    ///
    bool operator==(Sort t) const
    {
      return m_term == t.m_term;
    }

    friend Sort make_sort(Sort domain, Sort range);

  protected:
    aterm_appl m_term;
};

/// Creates a sort with the given domain and range.
///
inline
Sort make_sort(Sort domain, Sort range)
{
  return Sort(domain.m_term, range.m_term);
}

} // namespace mcrl

#endif // MCRL2_SORT_H
