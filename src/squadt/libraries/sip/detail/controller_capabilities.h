#ifndef SIP_CONTROLLER_CAPABILITIES_H
#define SIP_CONTROLLER_CAPABILITIES_H

#include <ostream>
#include <sstream>

#include <xml2pp/text_reader.h>
#include <sip/detail/common.h>

namespace sip {
  namespace tool {
    class communicator;
  }

  namespace controller {

    class communicator;

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
    class capabilities {
      friend class tool::communicator;
      friend class controller::communicator;
 
      public:
        /** \brief Type for display dimensions */
        struct display_dimensions {
          unsigned short x; ///< \brief Horizontal dimension
          unsigned short y; ///< \brief Vertical dimension
          unsigned short z; ///< \brief Unused for the moment
        };
 
        /** Convenience type to hide boost shared pointer implementation */
        typedef boost::shared_ptr < capabilities > ptr;
 
      private:
 
        /** \brief The protocol version */
        version            current_protocol_version;
 
        /** \brief The dimensions of the screen that are currently reserved for this tool */
        display_dimensions current_dimensions;
 
        /** \brief Constructor */
        inline capabilities(const version = default_protocol_version);
 
        /** \brief Read from XML stream */
        inline static capabilities::ptr read(xml2pp::text_reader& reader) throw ();
 
      public:
 
        /** \brief Get the protocol version */
        inline version get_version() const;
 
        /** \brief Set display dimensions */
        inline void set_display_dimensions(const unsigned short x, const unsigned short y, const unsigned short z);
 
        /** \brief Get the dimensions of the part of the display that is reserved for this tool */
        inline display_dimensions get_display_dimensions() const;
 
        /** \brief Read from XML string */
        inline static capabilities::ptr read(const std::string&);

        /** \brief Write to XML string */
        inline std::string write() const;
 
        /** \brief Write to XML stream */
        inline void write(std::ostream&) const;
    };
 
    /**
     * \brief Operator for writing to stream
     *
     * @param s stream to write to
     * @param c the capabilities object to write out
     **/
    inline std::ostream& operator << (std::ostream& s, const capabilities& c) {
      c.write(s);
 
      return (s);
    }
 
    inline capabilities::capabilities(const version v) : current_protocol_version(v) {
      current_dimensions.x = 0;
      current_dimensions.y = 0;
      current_dimensions.z = 0;
    }
 
    inline version capabilities::get_version() const {
      return (current_protocol_version);
    }
 
    inline void capabilities::set_display_dimensions(const unsigned short x, const unsigned short y, const unsigned short z) {
      current_dimensions.x = x;
      current_dimensions.y = y;
      current_dimensions.z = z;
    }
 
    inline capabilities::display_dimensions capabilities::get_display_dimensions() const {
      return (current_dimensions);
    }
 
    inline void capabilities::write(std::ostream& output) const {
      output << "<capabilities>"
             << "<protocol-version major=\"" << (unsigned short) current_protocol_version.major
             << "\" minor=\"" << (unsigned short) current_protocol_version.minor << "\"/>"
             << "<display-dimensions x=\"" << current_dimensions.x
             << "\" y=\"" << current_dimensions.y
             << "\" z=\"" << current_dimensions.z << "\"/>"
             << "</capabilities>";
    }
 
    inline std::string capabilities::write() const {
      std::ostringstream output;
 
      write(output);
 
      return (output.str());
    }
 
    /**
     * @param s the string to read from
     **/
    inline capabilities::ptr capabilities::read(const std::string& s) {
      xml2pp::text_reader r(s);

      return (read(r));
    }

    /**
     * @param r the XML text reader to read from
     *
     * \attention if the reader does not point at a capabilities element nothing is read
     **/
    inline capabilities::ptr capabilities::read(xml2pp::text_reader& r) throw () {
      if (r.is_element("capabilities")) {
        version v = {0,0};
 
        r.read();
 
        assert (r.is_element("protocol-version"));
 
        r.get_attribute(&v.major, "major");
        r.get_attribute(&v.minor, "minor");
 
        capabilities::ptr c(new capabilities(v));
 
        r.read();
        r.skip_end_element("protocol-version");
 
        assert (r.is_element("display-dimensions"));
 
        r.get_attribute(&c->current_dimensions.x, "x");
        r.get_attribute(&c->current_dimensions.y, "y");
        r.get_attribute(&c->current_dimensions.z, "z");
 
        r.read();
 
        return (c);
      }
 
      return capabilities::ptr();
    }
  }
}

#endif

