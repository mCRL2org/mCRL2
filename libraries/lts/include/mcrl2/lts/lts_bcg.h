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
 * \brief This file contains a class that contains labelled transition systems in bcg format.
 * \details A labelled transition system in bcg format is a transition system 
 * with as state labels vectors of strings, and as transition labels strings.
 * \author Jan Friso Groote
 */


#ifndef MCRL2_LTS_LTS_BCG_H
#define MCRL2_LTS_LTS_BCG_H

#ifdef USE_BCG
#include <string>
#include <vector>
#include "mcrl2/core/messaging.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/lts/lts.h"


namespace mcrl2
{
namespace lts
{
  namespace detail
  {

    /* A state label is just a vector of integers. Each integer i at position j
       corresponds with the element m_state_element_values[j][i].
    */
    class state_label_bcg:
          public std::vector < unsigned int >
    {};

    class action_label_bcg:
          public std::string
    {
      public:

        action_label_bcg()
        {}

        action_label_bcg(const std::string &s):std::string(s)
        {}

        void hide_actions(const std::vector<std::string> &tau_actions)
        {
          throw mcrl2::runtime_error("Cannot hide action labels in an bcg file");
        }
    };


  } // namespace detail


  class lts_bcg_t : public lts_< detail::state_label_bcg, detail::action_label_bcg >
  {
    protected:
      /* state_element_values contain the values that can occur at the i-th
         position of a state label */
      std::vector < std::vector < std::string > > m_state_element_values;
      
      std::vector < std::pair < std::string, std::string > > m_parameters;
  
    public:
      /** \brief Return the parameters of the state vectors stored in this LTS.
      * \return The state parameters stored in this LTS.
      */
      // data::variable_list process_parameters() const
      // {
      //   return m_parameters;
      // }
  
      /** \brief Returns the i-th parameter of the state vectors stored in this LTS.
       * \return The state parameters stored in this LTS.
      */
      std::pair < std::string, std::string > process_parameter(unsigned int i) const
      {
        assert(i<m_parameters.size());
        return m_parameters[i];
      }
  
  
      /** \brief Set the state parameters for this LTS.
       * \param[in] params  The state parameters for this lts.
      */
      void set_process_parameters(const data::variable_list &params)
      {
        for(data::variable_list::const_iterator i=params.begin();
                    i!=params.end(); ++i)
        { 
          m_parameters.push_back(std::pair<std::string,std::string>(core::pp(i->name()),core::pp(i->sort())));
        }
      }
  
      /** \brief Set the state parameters for this LTS.
       * \param[in] params  The state parameters for this lts.
      */
      void add_process_parameter(const std::string name, const std::string sort)
      {
        // assert(num_state_parameters()==0 || num_state_parameters()==params.size());
        m_parameters.push_back(std::pair<std::string,std::string>(name,sort));
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
#endif
