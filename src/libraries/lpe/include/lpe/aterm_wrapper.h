///////////////////////////////////////////////////////////////////////////////
/// \file lpe/aterm_wrapper.h
/// Contains a wrapper for ATerms with pretty print functionality.

#ifndef LPE_WRAPPER_H
#define LPE_WRAPPER_H

#include <list>
#include <iostream>
#include <boost/iterator/iterator_facade.hpp>
#include "atermpp/aterm.h"
#include "lpe/pretty_print.h"
#include "atermpp/aterm_list_iterator.h"

namespace lpe {

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::aterm_list;

  //---------------------------------------------------------//
  //                     aterm_wrapper
  //---------------------------------------------------------//
  /// A wrapper for ATerms.
  class aterm_wrapper
  {
    protected:
      ATerm m_term;

    public:
      aterm_wrapper()
      {}
      
      aterm_wrapper(ATerm term)
        : m_term(term)
      {}

      aterm_wrapper(ATermList term)
        : m_term(ATerm(term))
      {}

      aterm_wrapper(ATermAppl term)
        : m_term(ATerm(term))
      {}

      aterm_wrapper(aterm term)
        : m_term(term)
      {}

      operator ATerm() const
      {
        return m_term;
      }

      /// Returns true if the underlying aterm has not been initialized (i.e. is equal to ATfalse).
      /// Returns true if the underlying aterm has not been initialized (i.e. is equal to ATfalse).
      /// 
      bool operator!() const
      {
        return !aterm(m_term);
      }

      /// Returns a string representation of the term.
      ///
      std::string to_string() const
      {
        return aterm(m_term).to_string();
      }

      /// Returns a pretty print representation of the term.
      ///
      std::string pp() const
      {
        return pretty_print(m_term);
      }

      /// Protect the term.
      /// Protects the term from being freed at garbage collection.
      ///
      void protect()
      {
        aterm(m_term).protect();
      }

      /// Unprotect the term.
      /// Releases protection of the term which has previously been protected through a
      /// call to protect.
      ///
      void unprotect()
      {
        aterm(m_term).unprotect();
      }

      /// Mark the term for not being garbage collected.
      ///
      void mark()
      {
        aterm(m_term).mark();
      }
  };

  //---------------------------------------------------------//
  //                     aterm_appl_wrapper
  //---------------------------------------------------------//
  class aterm_appl_wrapper
  {
    protected:
      ATermAppl m_term;

    public:
      aterm_appl_wrapper()
      {}
      
      aterm_appl_wrapper(ATermAppl term)
        : m_term(term)
      {}

      operator ATermAppl() const
      {
        return ATermAppl(m_term);
      }

/*
      aterm_appl_wrapper& operator=(aterm t)
      {
        m_term = ATermAppl(t.term());
        return *this;
      }
*/
      /// Returns true if the underlying aterm has not been initialized (i.e. is equal to ATfalse).
      /// 
      bool operator!() const
      {
        return !aterm(m_term);
      }

      /// Returns a string representation of the term.
      ///
      std::string to_string() const
      {
        return aterm(m_term).to_string();
      }

      /// Returns a pretty print representation of the term.
      ///
      std::string pp() const
      {
        return pretty_print(aterm(m_term));
      }

      /// Protect the term.
      /// Protects the term from being freed at garbage collection.
      ///
      void protect()
      {
        aterm(m_term).protect();
      }

      /// Unprotect the term.
      /// Releases protection of the term which has previously been protected through a
      /// call to protect.
      ///
      void unprotect()
      {
        aterm(m_term).unprotect();
      }

      /// Mark the term for not being garbage collected.
      ///
      void mark()
      {
        aterm(m_term).mark();
      }
      
      aterm_list arguments() const
      {
        return aterm_appl(m_term).argument_list();
      }

      aterm argument(unsigned int i) const
      {
        return aterm_appl(m_term).argument(i);
      }
  };

  inline
  std::ostream& operator<<(std::ostream& out, aterm_appl_wrapper t)
  {
    return out << t.to_string();
  }
/*
  //---------------------------------------------------------//
  //                     aterm_list_wrapper
  //---------------------------------------------------------//
  class aterm_list_wrapper: public wrapper
  {
    public:
      aterm_list_wrapper()
      {}
      
      aterm_list_wrapper(ATerm term)
        : wrapper(term)
      {}

      aterm_list_wrapper(ATermList term)
        : wrapper(term)
      {}

      operator ATermList() const
      {
        return ATermList(m_term);
      }
  };
*/

} // namespace lpe

/*
namespace atermpp
{
  template <>
  class term_list_iterator_traits<lpe::aterm_appl_wrapper>
  {
    typedef ATermAppl value_type;
  };
} // namespace atermpp
*/

#endif // LPE_WRAPPER_H
