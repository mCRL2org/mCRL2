#ifndef SIP_OBJECT_H
#define SIP_OBJECT_H

#include <set>
#include <ostream>
#include <cstring>

#include <boost/shared_ptr.hpp>

#include <utility/visitor.h>

#include <xml2pp/text_reader.h>
#include <sip/exception.h>
#include <sip/detail/option.h>
#include <sip/mime_type.h>

namespace sip {

  namespace controller {
    class communicator;
  }

  namespace tool {
    class communicator;
  }

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
  class object : public utility::visitable < object > {
    friend class tool::communicator;
    friend class controller::communicator;
    friend class configuration;

    public:
      /**
       * \brief Type to indicate what the object is used for.
       *
       * This is useful for instance to a controller that can make sure that
       * input objects exists before giving the tool the start signal.
       **/
      enum type {
        input,          ///< \brief object is used as input
        output          ///< \brief object is used as output
      };

      /** \brief Until there is something better this is the type for a URI */
      typedef std::string                   uri;

      /** \brief convenience type to hide the shared pointer implementation */
      typedef boost::shared_ptr < object >  sptr;

      /** \brief Datatype for the textual identifier of an option/object */
      typedef option::identifier            identifier;

    private:

      /** \brief the type of the object */
      mime_type            m_mime_type;

      /** \brief The format used for storing this object */
      uri                  m_location;

      /** \brief The type of this object (TODO moved to configuration level) */
      type                 _type;

      /** \brief Must uniquely identify the object in a configuration (TODO moved to configuration level) */
      identifier           id;

      /** \brief String representations for types, used for XML encoding */
      const static char*   type_strings[];

    private:

      /** \brief Read from XML stream */
      inline static object::sptr read(xml2pp::text_reader& reader);

      /** \brief Constructor */
      inline object(identifier const&, mime_type const&, uri const& = "", const type = input);

    public:
      /** \brief Returns the objects identifier */
      inline identifier get_id() const;

      /** \brief Returns the objects type */
      inline type get_type() const;

      /** \brief Returns the object storage format */
      inline mime_type get_mime_type() const;

      /** \brief Sets the object storage format */
      inline void set_mime_type(mime_type const&);

      /** \brief Returns the object location */
      inline uri get_location() const;

      /** \brief Sets the object location */
      inline void set_location(object::uri const&);

      /** \brief Write to XML stream */
      inline void write(std::ostream&) const;
  };

#ifdef SIP_IMPORT_STATIC_DEFINITIONS
  const char* object::type_strings[] = {
    "input"      /// \brief object is used for input
   ,"output"     /// \brief object is used for output
   ,0
  };
#endif

  inline object::object(identifier const& i, mime_type const& m, uri const& l, const type t) : m_mime_type(m), m_location(l), _type(t), id(i) {
  }

  inline object::identifier object::get_id() const {
    return (id);
  }

  inline object::type object::get_type() const {
    return (_type);
  }

  inline mime_type object::get_mime_type() const {
    return (m_mime_type);
  }

  inline void object::set_mime_type(mime_type const& m) {
    m_mime_type = m;
  }

  inline object::uri object::get_location() const {
    return (m_location);
  }

  inline void object::set_location(object::uri const& l) {
    m_location = l;
  }

  inline void object::write(std::ostream& output) const {
    output << "<object id=\"" << id
           << "\" type=\"" << type_strings[_type]
           << "\" storage-format=\"" << m_mime_type << "\"";
    
    if (!m_location.empty()) {
      output << " location=\"" << m_location << "\"";
    }

    output << "/>";
  }

  /** \pre the reader must point at an object element} */
  inline object::sptr object::read(xml2pp::text_reader& r) {
    using sip::exception;

    object::identifier id = 0;

    assert(r.is_element("object"));
    
    if (!r.get_attribute(&id, "id")) {
      throw (exception(sip::message_missing_required_attribute, "id", "object"));
    }
    else {
      std::string new_type;

      if (!r.get_attribute(&new_type, "type")) {
        throw (exception(sip::message_missing_required_attribute, "type", "object"));
      }
     
      size_t i = 0;
     
      while (type_strings[i] != 0 && strcmp(new_type.c_str(), type_strings[i]) != 0) {
        ++i;
      }
     
      if (type_strings[i] == 0) {
        throw (exception(sip::message_unknown_type, new_type, "object"));
      }
     
      std::string new_format;
     
      if (!r.get_attribute(&new_format, "storage-format")) {
        throw (exception(sip::message_missing_required_attribute, "storage-format", "object"));
      }
     
      std::string new_location;
     
      r.get_attribute(&new_location, "location");
     
      r.next_element();
      r.skip_end_element("object");
     
      return (object::sptr(new object(id, new_format, new_location, static_cast < object::type > (i))));
    }
  }
}

#endif

