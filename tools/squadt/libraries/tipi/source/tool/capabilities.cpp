//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tool/capabilities.cpp

#include <ostream>
#include <sstream>
#include <utility>

#include <boost/bind.hpp>

#include "tipi/controller/capabilities.hpp"
#include "tipi/tool/capabilities.hpp"
#include "tipi/visitors.hpp"

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
    void capabilities::add_input_configuration(std::string const& id, mime_type const& f, tool::category const& c) {
      boost::shared_ptr< const input_configuration > ic(new input_configuration(c, f, id));
 
      m_input_configurations.insert(ic);
    }
 
    /**
     * \param[in] id a unique identifier for the output object
     * \param[in] f mime-type for the object
     **/
    void capabilities::add_output_configuration(std::string const& id, mime_type const& f) {
      boost::shared_ptr< const output_configuration > oc(new output_configuration(f, id));
 
      m_output_configurations.insert(oc);
    }
 
    version capabilities::get_version() const {
      return (m_protocol_version);
    }
 
    capabilities::input_configuration_range capabilities::get_input_configurations() const {
      return (boost::make_iterator_range(m_input_configurations));
    }
 
    capabilities::output_configuration_range capabilities::get_output_configurations() const {
      return (boost::make_iterator_range(m_output_configurations));
    }

    /**
     * \param[in] f the storage format
     * \param[in] t the category in which the tool operates
     **/
    boost::shared_ptr< const capabilities::input_configuration >
              capabilities::find_input_configuration(const mime_type& f, const tool::category& t) const {
 
      for (input_configuration_list::const_iterator i = m_input_configurations.begin(); i != m_input_configurations.end(); ++i) {
        if ((*i)->get_category() == t && (*i)->get_primary_object_descriptor().second == f) {
          return *i;
        }
      }

      return boost::shared_ptr< const input_configuration >();
    }
  }
}
