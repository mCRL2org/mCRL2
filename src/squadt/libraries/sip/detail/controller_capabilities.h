#ifndef SIP_CONTROLLER_CAPABILITIES_H
#define SIP_CONTROLLER_CAPABILITIES_H

#include <ostream>

#include <xml2pp/xml_text_reader.h>

namespace sip {

  /**
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
      /** Type for protocol version */
      struct version {
        unsigned char major;
        unsigned char minor;
      };

      /** Type for display dimensions */
      struct display_dimensions {
        unsigned short x; /* Horizontal */
        unsigned short y; /* Vertical */
        unsigned short z; /* Unused for the moment (Depth) */
      };

    private:

      /** The protocol version */
      version            protocol_version;

      /** The dimensions of the screen that are currently reserved for this tool */
      display_dimensions current_dimensions;

      /** Constructor */
      inline controller_capabilities(const version);

      /** Set display dimensions */
      inline void set_display_dimensions(const unsigned short x, const unsigned short y, const unsigned short z);

      /** Set display dimensions */
      inline void set_display_dimensions(const display_dimensions& d);

      /** Read from XML stream */
      inline static controller_capabilities* from_xml(xml2pp::text_reader& reader);

    public:

      /** Get the protocol version used by the controller */
      inline version get_version() const;

      /** Get the dimensions of the part of the display that is reserved for this tool */
      inline display_dimensions get_display_dimensions() const;

      /** Write to XML stream */
      inline void to_xml(std::ostream&) const;
  };

  inline controller_capabilities::controller_capabilities(const version v) : protocol_version(v) {
    current_dimensions.x = 0;
    current_dimensions.y = 0;
    current_dimensions.z = 0;
  }

  inline controller_capabilities::version controller_capabilities::get_version() const {
    return (protocol_version);
  }

  inline void controller_capabilities::set_display_dimensions(const display_dimensions& d) {
    current_dimensions = d;
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
           << "<protocol-version major=\"" << (unsigned short) protocol_version.major
           << "\" minor=\"" << (unsigned short) protocol_version.minor << "\"/>"
           << "<display-dimensions x=\"" << current_dimensions.x
           << "\" y=\"" << current_dimensions.y
           << "\" z=\"" << current_dimensions.z << "\"/>"
           << "</capabilities>";
  }

  /** \pre{the reader must point at a capabilities element} */
  inline controller_capabilities* controller_capabilities::from_xml(xml2pp::text_reader& reader) {
    controller_capabilities*         c;
    controller_capabilities::version v = {0,0};

    reader.read();

    assert (reader.is_element("protocol-version"));
    
    reader.get_attribute(&v.major, "major");
    reader.get_attribute(&v.minor, "minor");

    c = new controller_capabilities(v);

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

