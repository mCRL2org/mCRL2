#ifndef SIP_OBJECT_H
#define SIP_OBJECT_H

#include <set>
#include <ostream>
#include <cstring>

#include <boost/shared_ptr.hpp>

#include <xml2pp/detail/text_reader.tcc>
#include <sip/detail/exception.h>
#include <sip/detail/option.h>

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
  class object {
    friend class tool::communicator;
    friend class controller::communicator;
    friend class configuration;

    public:
      /**
       * Type to indicate what the object is used for.
       *
       * This is useful for instance to a controller that can make sure that
       * input objects exists before giving the tool the start signal.
       **/
      enum type {
        input           /// \brief object is used as input
       ,output          /// \brief object is used as output
      };

      /** \brief Until there is something better this is the type for a URI */
      typedef std::string                   uri;

      /** \brief Until there is something better this is the type for a storage format */
      typedef std::string                   storage_format;

      /** \brief convenience type to hide the shared pointer implementation */
      typedef boost::shared_ptr < object >  ptr;

      /** Datatype for the textual identifier of an option/object */
      typedef option::identifier            identifier;

    private:

      /** \brief The format used for storing this object */
      const storage_format format;

      /** \brief The format used for storing this object */
      const uri            location;

      /** \brief The type of this object */
      type                 _type;

      /** \brief Must uniquely identify the object in a configuration */
      identifier           id;

      /** \brief String representations for types, used for XML encoding */
      const static char*   type_strings[];

    private:

      /** \brief Read from XML stream */
      inline static object::ptr read(xml2pp::text_reader& reader) throw ();

      /** \brief Constructor */
      inline object(identifier, const storage_format, const uri = "", const type = input);

    public:
      /** \brief Returns the objects identifier */
      inline const identifier get_id() const;

      /** \brief Returns the objects type */
      inline const type get_type() const;

      /** \brief Returns the objects uri */
      inline const uri get_location() const;

      /** \brief Write to XML stream */
      inline void write(std::ostream&) const;
  };

  inline object::object(identifier i, const storage_format f, const uri l, const type t) : format(f), location(l), _type(t), id(i) {
  }

  inline const object::identifier object::get_id() const {
    return (id);
  }

  inline const object::type object::get_type() const {
    return (_type);
  }

  inline const object::uri object::get_location() const {
    return (location);
  }

  inline void object::write(std::ostream& output) const {
    output << "<object id=\"" << id
           << "\" type=\"" << type_strings[_type]
           << "\" storage-format=\"" << format << "\"";
    
    if (!location.empty()) {
      output << " location=\"" << location << "\"";
    }

    output << "/>";
  }

  /** \pre the reader must point at an object element} */
  inline object::ptr object::read(xml2pp::text_reader& r) throw () {
    option::identifier id;

    assert (r.is_element("object"));
    
    if (!r.get_attribute(&id, "id")) {
      throw (exception(exception_identifier::message_missing_required_attribute, "id", "object"));
    }

    std::string new_type;

    if (!r.get_attribute(&new_type, "type")) {
      throw (exception(exception_identifier::message_missing_required_attribute, "type", "object"));
    }

    size_t i = 0;

    while (type_strings[i] != 0 && strcmp(new_type.c_str(), type_strings[i]) != 0) {
      ++i;
    }

    if (type_strings[i] == 0) {
      throw (exception(exception_identifier::message_unknown_type, new_type, "object"));
    }

    std::string new_format;

    if (!r.get_attribute(&new_format, "storage-format")) {
      throw (exception(exception_identifier::message_missing_required_attribute, "storage-format", "object"));
    }

    std::string new_location;

    r.get_attribute(&new_location, "location");

    r.read();
    r.skip_end_element("object");

    return (object::ptr(new object(id, new_format, new_location, static_cast < object::type > (i))));
  }
}

#endif

