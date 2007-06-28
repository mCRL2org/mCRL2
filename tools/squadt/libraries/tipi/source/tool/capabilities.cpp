//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/tool/capabilities.cpp

#include <ostream>
#include <sstream>
#include <utility>

#include <boost/bind.hpp>

#include <tipi/controller/capabilities.h>
#include <tipi/tool/capabilities.h>
#include <tipi/visitors.h>

namespace tipi {
  namespace tool {
    /**
     * \brief Operator for writing to stream
     *
     * \param[in,out] s stream to write to
     * \param[in] t the capabilities object to write out
     **/
    std::ostream& operator << (std::ostream& s, tool::capabilities const& t) {
      visitors::store(t, s);
 
      return (s);
    }

    capabilities::capabilities(const version v) : m_protocol_version(v) {
    }
 
    /**
     * \param[in] id a unique identifier for the input object
     * \param[in] f mime-type for the object
     * \param[in] c category to which the functionality of the tool must be counted
     **/
    void capabilities::add_input_combination(std::string const& id, mime_type const& f, tool::category const& c) {
      input_combination ic(c, f, id);
 
      m_input_combinations.insert(ic);
    }
 
    /**
     * \param[in] id a unique identifier for the output object
     * \param[in] f mime-type for the object
     **/
    void capabilities::add_output_combination(std::string const& id, mime_type const& f) {
      output_combination oc(f, id);
 
      m_output_combinations.insert(oc);
    }
 
    version capabilities::get_version() const {
      return (m_protocol_version);
    }
 
    capabilities::input_combination_range capabilities::get_input_combinations() const {
      return (boost::make_iterator_range(m_input_combinations));
    }
 
    capabilities::output_combination_range capabilities::get_output_combinations() const {
      return (boost::make_iterator_range(m_output_combinations));
    }

    /**
     * \param[in] f the storage format
     * \param[in] t the category in which the tool operates
     **/
    capabilities::input_combination const*
              capabilities::find_input_combination(const mime_type& f, const tool::category& t) const {
 
      for (input_combination_list::const_iterator i = m_input_combinations.begin(); i != m_input_combinations.end(); ++i) {
        if (i->m_category == t && i->m_mime_type == f) {
          return (&(*i));
        }
      }

      return (0);
    }
  }
}
