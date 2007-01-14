#ifndef SIP_OBJECT_H
#define SIP_OBJECT_H

#include <string>

#include <boost/shared_ptr.hpp>

#include <utility/visitor.h>

#include <sip/parameter.h>
#include <sip/mime_type.h>
#include <sip/uri.h>

namespace sip {

  namespace controller {
    class communicator;
  }

  namespace tool {
    class communicator;
  }

  class configuration;
  class store_visitor_impl;
  class restore_visitor_impl;

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
  class object : public sip::parameter, public utility::visitable < object > {
    friend class sip::configuration;
    friend class sip::restore_visitor_impl;
    friend class sip::store_visitor_impl;

    public:

      /** \brief convenience type to hide the shared pointer implementation */
      typedef boost::shared_ptr < object >  sptr;

    private:

      /** \brief the type of the object */
      mime_type            m_mime_type;

      /** \brief The format used for storing this object */
      uri                  m_location;

    private:

      /** \brief Constructor */
      inline object();

      /** \brief Constructor */
      inline object(mime_type const&, uri const& = "");

    public:

      /** \brief Returns the object storage format */
      inline mime_type get_mime_type() const;

      /** \brief Sets the object storage format */
      inline void set_mime_type(mime_type const&);

      /** \brief Returns the object location */
      inline uri get_location() const;

      /** \brief Sets the object location */
      inline void set_location(uri const&);
  };

  inline object::object() : m_mime_type("unknown") {
  }

  inline object::object(mime_type const& m, uri const& l) : m_mime_type(m), m_location(l) {
  }

  inline mime_type object::get_mime_type() const {
    return (m_mime_type);
  }

  inline void object::set_mime_type(mime_type const& m) {
    m_mime_type = m;
  }

  inline uri object::get_location() const {
    return (m_location);
  }

  inline void object::set_location(uri const& l) {
    m_location = l;
  }
}

#endif

