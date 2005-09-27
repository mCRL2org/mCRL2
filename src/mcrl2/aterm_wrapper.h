///////////////////////////////////////////////////////////////////////////////
/// \file mcrl2/list_iterator.h
/// Contains a general aterm_wrapper iterator.

#ifndef MCRL2_ATERM_WRAPPER_H
#define MCRL2_ATERM_WRAPPER_H

#include <list>
#include <boost/iterator/iterator_facade.hpp>
#include "atermpp/aterm.h"

namespace mcrl2 {

using atermpp::aterm_appl;

  //---------------------------------------------------------//
  //                     aterm_wrapper
  //---------------------------------------------------------//
  class aterm_wrapper
  {
   protected:
      aterm_appl m_term;

   public:
    aterm_wrapper()
    {}

    aterm_wrapper(aterm_appl term)
      : m_term(term)
    {}

    /// Two terms are considered equal if the underlying aterms are identical.
    /// 
    bool operator==(const aterm_wrapper& t) const
    {
      return m_term == t.m_term;
    }

    /// Returns a string representation of the term.
    ///
    std::string to_string() const
    {
      return m_term.to_string();
    }

    /// Returns the internal aterm representation.
    ///
    aterm_appl term() const
    {
      return m_term;
    }

/*    
    /// Applies a sequence of substitutions to this term and returns the result.
    /// The SubstIter objects must supply the method aterm_appl operator()(aterm_appl).
    ///
    template <typename SubstIter>
    aterm_appl aterm_appl_substitute(SubstIter first, SubstIter last) const
    {
      aterm_appl result = m_term;
      for (SubstIter i = first; i != last; ++i)
      {
        result = (*i)(result);
      }
      return result;
    }    
*/  
  };

} // namespace mcrl

#endif // MCRL2_ATERM_WRAPPER_H
