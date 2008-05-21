// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tool.h
/// \brief Add your file description here.

#ifndef TOOL_H
#define TOOL_H

#include <string>
#include <iostream>

#include <boost/filesystem/path.hpp>

#include "tipi/tool/capabilities.hpp"

#include "tipi/detail/utility/generic_visitor.hpp"

#include "build_system.hpp"

namespace squadt {

  class extractor;

  /**
   * \brief Container for information about a single tool
   **/
  class tool : public utility::visitable {
    friend class extractor;
    friend class tool_manager_impl;

    template < typename R, typename S >
    friend class utility::visitor;

    public:

      /** \brief Convenience type alias */
      typedef tipi::tool::capabilities::input_configuration input_configuration;

    private:

      /** \brief A name for the tool */
      std::string                                     m_name;

      /** \brief The location where the tool can be found */
      boost::filesystem::path                         m_location;

      /** \brief Stores the tool capabilities object obtained through protocol implementation */
      boost::shared_ptr < tipi::tool::capabilities >  m_capabilities;

    private:

      /** \brief The default capabilities descriptor; one that is empty */
      static const boost::shared_ptr < tipi::tool::capabilities > no_capabilities;

    private:

      /** \brief Constructor */
      inline tool(std::string const&, boost::filesystem::path const&, boost::shared_ptr < tipi::tool::capabilities > = tool::no_capabilities);

      /** \brief Default constructor */
      inline tool();

    public:

      /** \brief Copy constructor */
      inline tool(tool const&);

      /** \brief Set capabilities object for this tool */
      inline void set_capabilities(boost::shared_ptr < tipi::tool::capabilities >);

      /** \brief Get capabilities object for this tool */
      inline boost::shared_ptr < tipi::tool::capabilities > get_capabilities() const;

      /** \brief Get the location to for this tool */
      inline boost::filesystem::path get_location() const;

      /** \brief Get the location to for this tool */
      inline void set_location(boost::filesystem::path const& p);

      /** \brief Get the name of this tool */
      inline std::string get_name() const;

      /** \brief Whether or not the tool is usable */
      inline bool is_usable() const;

      /** \brief Find a specific input combination of this tool, if it exists */
      inline boost::shared_ptr < const input_configuration > find_input_configuration(build_system::tool_category const&, build_system::storage_format const&) const;
  };

  /**
   * \param[in] l a full path to the executable
   * \param[in] n a name for the tool
   * \param[in] c a tool::capabilities object for the tool
   **/
  inline tool::tool(std::string const& n, boost::filesystem::path const& l, boost::shared_ptr < tipi::tool::capabilities > c) : m_name(n), m_location(l), m_capabilities(c) {
  }

  inline tool::tool(tool const& t) : m_name(t.m_name), m_location(t.m_location), m_capabilities(t.m_capabilities) {
  }

  inline tool::tool() : m_capabilities(tool::no_capabilities) {
  }

  /**
   * \param[in] c a shared pointer to a capabilities object
   **/
  inline void tool::set_capabilities(boost::shared_ptr < tipi::tool::capabilities > c) {
    if (c.get() != 0) {
      m_capabilities = c;
    }
  }

  inline boost::shared_ptr < tipi::tool::capabilities > tool::get_capabilities() const {
    return m_capabilities;
  }

  inline boost::filesystem::path tool::get_location() const {
    return m_location;
  }

  /**
   * \param[in] p a full path to the executable
   **/
  inline void tool::set_location(boost::filesystem::path const& p) {
    m_location = p;
  }

  inline std::string tool::get_name() const {
    return m_name;
  }

  inline bool tool::is_usable() const {
    return m_capabilities.get() != 0;
  }

  /**
   * \param[in] f the storage format
   * \param[in] t the category in which the tool operates
   **/
  inline boost::shared_ptr < const tool::input_configuration > tool::find_input_configuration(build_system::tool_category const& t, build_system::storage_format const& f) const {
    return m_capabilities->find_input_configuration(f, t);
  }
}

#endif
