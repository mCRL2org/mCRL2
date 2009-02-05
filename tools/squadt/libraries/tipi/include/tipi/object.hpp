// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/object.hpp
/// \brief Type used to represent an object in a configuration (protocol concept)

#ifndef TIPI_OBJECT_H
#define TIPI_OBJECT_H

#include <string>

#include "tipi/mime_type.hpp"
#include "tipi/uri.hpp"

namespace tipi {

  class configuration;

  /**
   * \brief Describes some tool capabilities (e.g. supported protocol version)
   *
   * Objects of this type contain information about the capabilities of a tool:
   *
   *  - what version of the protocol the controller uses
   *  - a list of input configurations
   *  - ...
   *
   * As well as any information about the controller that might be interesting
   * for a tool developer.
   **/
  class configuration::object : public configuration::parameter {
    friend class tipi::configuration;

    template < typename R, typename S >
    friend class ::utility::visitor;

    private:

      /** \brief the type of the object */
      mime_type            m_mime_type;

      /** \brief The format used for storing this object */
      uri                  m_location;

    private:

      /** \brief Constructor */
      inline object() : m_mime_type("unknown") {
      }

      /** \brief Constructor */
      inline object(mime_type const& m, uri const& l = "") : m_mime_type(m), m_location(l) {
      }

    public:

      /** \brief Returns the object storage format */
      inline mime_type type() const {
        return (m_mime_type);
      }

      /** \brief Sets the object storage format */
      inline void type(mime_type const& m) {
        m_mime_type = m;
      }

      /** \brief Returns the object location */
      inline uri location() const {
        return (m_location);
      }

      /** \brief Sets the object location */
      inline void location(uri const& l) {
        m_location = l;
      }
  };
}

#endif

