#ifndef SIP_CONTROLLER_CAPABILITIES_H
#define SIP_CONTROLLER_CAPABILITIES_H

#include <ostream>
#include <sstream>

#include <xml2pp/text_reader.h>
#include <sip/detail/capabilities.h>

namespace sip {

  /**
   * \brief Describes some controller capabilities (e.g. supported protocol version)
   *
   * Objects of this type contain information about the capabilities of a controller:
   *
   *  - the amount of display space that is reserved for a tool (that makes a request)
   *  - what version of the protocol the controller uses
   *  - ...
   *
   * As well as any information about the controller that might be interesting
   * for a tool developer.
   **/
  class controller_capabilities {
    friend class tool_communicator;
    friend class controller_communicator;

    public:
      /** \brief Type for display dimensions */
      struct display_dimensions {
        unsigned short x; /* Horizontal */
        unsigned short y; /* Vertical */
        unsigned short z; /* Unused for the moment (Depth) */
      };

      /** Convenience type to hide boost shared pointer implementation */
      typedef boost::shared_ptr < controller_capabilities > controller_capabilities_ptr;

    private:

      /** \brief The protocol version */
      version            current_protocol_version;

      /** \brief The dimensions of the screen that are currently reserved for this tool */
      display_dimensions current_dimensions;

      /** \brief Constructor */
      inline controller_capabilities(const version = protocol_version);

      /** \brief Read from XML stream */
      inline static controller_capabilities_ptr from_xml(xml2pp::text_reader& reader) throw ();

    public:

      /** \brief Get the protocol version */
      inline version get_version() const;

      /** \brief Set display dimensions */
      inline void set_display_dimensions(const unsigned short x, const unsigned short y, const unsigned short z);

      /** \brief Get the dimensions of the part of the display that is reserved for this tool */
      inline display_dimensions get_display_dimensions() const;

      /** \brief Write to XML string */
      inline std::string to_xml() const;

      /** \brief Write to XML stream */
      inline void to_xml(std::ostream&) const;
  };

  inline controller_capabilities::controller_capabilities(const version v) : current_protocol_version(v) {
    current_dimensions.x = 0;
    current_dimensions.y = 0;
    current_dimensions.z = 0;
  }

  inline version controller_capabilities::get_version() const {
    return (current_protocol_version);
  }

  inline void controller_capabilities::set_display_dimensions(const unsigned short x, const unsigned short y, const unsigned short z) {
    current_dimensions.x = x;
    current_dimensions.y = y;
    current_dimensions.z = z;
  }

  inline controller_capabilities::display_dimensions controller_capabilities::get_display_dimensions() const {
    return (current_dimensions);
  }

  inline void controller_capabilities::to_xml(std::ostream& output) const {
    output << "<capabilities>"
           << "<protocol-version major=\"" << (unsigned short) current_protocol_version.major
           << "\" minor=\"" << (unsigned short) current_protocol_version.minor << "\"/>"
           << "<display-dimensions x=\"" << current_dimensions.x
           << "\" y=\"" << current_dimensions.y
           << "\" z=\"" << current_dimensions.z << "\"/>"
           << "</capabilities>";
  }

  inline std::string controller_capabilities::to_xml() const {
    std::ostringstream output;

    to_xml(output);

    return (output.str());
  }

  /** \pre the reader must point at a capabilities element */
  inline controller_capabilities::controller_capabilities_ptr controller_capabilities::from_xml(xml2pp::text_reader& reader) throw () {
    version                  v = {0,0};

    reader.read();

    assert (reader.is_element("protocol-version"));
    
    reader.get_attribute(&v.major, "major");
    reader.get_attribute(&v.minor, "minor");

    controller_capabilities_ptr c(new controller_capabilities(v));

    reader.read();

    if (reader.is_end_element()) {
      reader.read();
    }

    assert (reader.is_element("display-dimensions"));

    reader.get_attribute(&c->current_dimensions.x, "x");
    reader.get_attribute(&c->current_dimensions.y, "y");
    reader.get_attribute(&c->current_dimensions.z, "z");

    reader.read();

    return (c);
  }
}

#endif

