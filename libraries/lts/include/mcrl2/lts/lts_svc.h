// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file
 *
 * \brief This file contains a class that contains labelled transition systems in lts (mcrl2) format.
 * \details A labelled transition system in lts/mcrl2 format is a transition system 
 * with as state labels vectors of strings, and as transition labels strings.
 * \author Jan Friso Groote
 */


#ifndef MCRL2_LTS_SVC_MCRL2_H
#define MCRL2_LTS_SVC_MCRL2_H

#include <string>
#include <vector>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/lts/lts.h"


namespace mcrl2
{
namespace lts
{
  namespace detail
  {

  // A state label for svc format is just an ATerm.
  class state_label_svc:public atermpp::aterm
  {

    public:
      state_label_svc()
      {}

      state_label_svc(const ATerm a):atermpp::aterm(a)
      { 
      } 

      state_label_svc(const atermpp::aterm_appl a):atermpp::aterm(a)
      { 
      } 

      ATerm aterm() const
      {
        return m_term;
      }
  };

  /** \brief Pretty print a state value of this LTS.
   * \param[in] l  The state value to pretty print.
   * \return           The pretty-printed representation of value.
   * \pre              value occurs as state value in this LTS. */

  inline std::string pp(const state_label_svc l)
  {
    return ATwriteToString(l.aterm());
  } 


  class action_label_svc:public atermpp::aterm
  {
    public:
      action_label_svc()
      {}

      action_label_svc(const ATerm a):atermpp::aterm(a)
      { 
      } 

      ATerm aterm() const
      { 
        return m_term;
      }

      bool hide_actions(const std::vector<std::string> &)
      { 
        using namespace std;
        std::cerr << "Hiding actions in a .svc file is not possible, because the labels do not necessarily have an action structure \n";
        return true;
      }
  };


  inline std::string pp(const action_label_svc l)
  { 
    return ATwriteToString(l.aterm());
  }

  } // namespace detail


  /** \brief Stores additional LTS information.
   * \details This class is provided for storing additional information with an
   * LTS. This can be either a muCRL specification, an mCRL2 specificiation, or
   * options for the Dot format. */

  class lts_svc_t : public lts< detail::state_label_svc, detail::action_label_svc >
  {

    public:

      lts_type type()
      {
        return lts_svc;
      }

      /** \brief Save the labelled transition system to file.
       *  \details If the filename is empty, the result is read from stdout.
       *  \param[in] filename Name of the file from which this lts is read.
       */
      void load(const std::string &filename);

      /** \brief Save the labelled transition system to file.
       *  \details If the filename is empty, the result is written to stdout.
       *  \param[in] filename Name of the file to which this lts is written.
       */
      void save(const std::string &filename) const;
  };
} // namespace lts
} // namespace mcrl2

#endif
