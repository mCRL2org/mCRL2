// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/testtype.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_TESTTYPE_H
#define MCRL2_FDR_TESTTYPE_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Test type
  class testtype: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      testtype()
        : atermpp::aterm_appl(fdr::detail::constructTestType())
      {}

      /// \brief Constructor.
      /// \param term A term
      testtype(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_TestType(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A deterministic
class deterministic: public testtype
{
  public:
    /// \brief Default constructor.
    deterministic()
      : testtype(fdr::detail::constructdeterministic())
    {}

    /// \brief Constructor.
    /// \param term A term
    deterministic(atermpp::aterm_appl term)
      : testtype(term)
    {
      assert(fdr::detail::check_term_deterministic(m_term));
    }
};

/// \brief A deadlock free
class deadlock_free: public testtype
{
  public:
    /// \brief Default constructor.
    deadlock_free()
      : testtype(fdr::detail::constructdeadlock_free())
    {}

    /// \brief Constructor.
    /// \param term A term
    deadlock_free(atermpp::aterm_appl term)
      : testtype(term)
    {
      assert(fdr::detail::check_term_deadlock_free(m_term));
    }
};

/// \brief A livelock free
class livelock_free: public testtype
{
  public:
    /// \brief Default constructor.
    livelock_free()
      : testtype(fdr::detail::constructlivelock_free())
    {}

    /// \brief Constructor.
    /// \param term A term
    livelock_free(atermpp::aterm_appl term)
      : testtype(term)
    {
      assert(fdr::detail::check_term_livelock_free(m_term));
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a deterministic expression
    /// \param t A term
    /// \return True if it is a deterministic expression
    inline
    bool is_deterministic(const testtype& t)
    {
      return fdr::detail::gsIsdeterministic(t);
    }

    /// \brief Test for a deadlock_free expression
    /// \param t A term
    /// \return True if it is a deadlock_free expression
    inline
    bool is_deadlock_free(const testtype& t)
    {
      return fdr::detail::gsIsdeadlock_free(t);
    }

    /// \brief Test for a livelock_free expression
    /// \param t A term
    /// \return True if it is a livelock_free expression
    inline
    bool is_livelock_free(const testtype& t)
    {
      return fdr::detail::gsIslivelock_free(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_TESTTYPE_H
