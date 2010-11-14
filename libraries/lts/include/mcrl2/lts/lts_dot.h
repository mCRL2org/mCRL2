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
 * \brief This file contains a class that contains labelled transition systems in dot format.
 * \details A labelled transition system in dot format is a transition system 
 * with strings as state and transition labels.
 * \author Jan Friso Groote
 */


#ifndef MCRL2_LTS_LTS_DOT_H
#define MCRL2_LTS_LTS_DOT_H

#include <string>
#include <vector>
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/print.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/lts/action_label_string.h"


namespace mcrl2
{
namespace lts
{
  namespace detail
  {

    /* A state label is just a vector of integers. Each integer i at position j
       corresponds with the element m_state_element_values[j][i].
    */
    class state_label_dot
    {
      private:
        std::string m_state_name;
        std::string m_state_label;

      public:

        state_label_dot()
        {}

        state_label_dot(const std::string &state_name, 
                        const std::string &state_label):m_state_name(state_name),m_state_label(state_label)
        {}

        // state_label_dot(const ATermList s):std::string(ATwriteToString((ATerm)s))
        // {}

        void set_name(const std::string &s)
        {
          m_state_name=s;
        }
 
        std::string name() const
        {
          return m_state_name;
        }

        void set_label(const std::string &s)
        {
          m_state_label=s;
        }

        std::string label() const
        {
          return m_state_label;
        }

        bool operator ==(const state_label_dot &l) const
        {
          return m_state_name==l.name() && m_state_label==l.label();
        }

        bool operator !=(const state_label_dot &l) const
        {
          return !(*this==l); 
        }
    };

    inline std::string pp(const state_label_dot &l)
    {
      return l.label();
    }
  } // namespace detail


  class lts_dot_t : public lts< detail::state_label_dot, detail::action_label_string >
  {
  
    public:

      lts_type type()
      {
        return lts_dot;
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
