#ifndef TOOL_H
#define TOOL_H

#include <string>

#include <xml2pp/text_reader.h>
#include <sip/detail/tool_capabilities.h>

#include "exception.h"

namespace squadt {

  class extractor;

  /**
   * \brief Container for information about a single tool
   **/
  class tool {
    friend class extractor;

    public:

      /** \brief Convenience type for hiding shared pointer implementation */
      typedef boost::shared_ptr < tool >  ptr;

    private:

      /** \brief A name for the tool */
      std::string                         name;

      /** \brief The location where the tool can be found */
      std::string                         location;

      /** \brief Stores the tool capabilities object obtained through protocol implementation */
      sip::tool::capabilities::ptr        capabilities;

    private:

      /** Constructor */
      inline tool(std::string, std::string, sip::tool::capabilities::ptr);

    public:

      /** Constructor */
      inline tool(std::string, std::string);

      /** \brief Write configuration to stream */
      void write(std::ostream& = std::cout) const;

      /** \brief Read configuration from file */
      inline static tool::ptr read(xml2pp::text_reader&) throw ();

      /** \brief Get the last received capabilities object for this tool */
      inline const sip::tool::capabilities& get_capabilities() const;

      /** \brief Get the location to for this tool */
      inline std::string& get_location();

      /** \brief Get the name of this tool */
      inline std::string& get_name();
  };

  /**
   * @param l a full path to the executable
   * @param n a name for the tool
   **/
  inline tool::tool(std::string n, std::string l) : name(n), location(l) {
  }

  /**
   * @param l a full path to the executable
   * @param n a name for the tool
   * @param c a tool::capabilities object for the tool
   **/
  inline tool::tool(std::string n, std::string l, sip::tool::capabilities::ptr c) : name(n), location(l), capabilities(c) {
  }

  /**
   * @param s the stream to write to
   **/
  inline void tool::write(std::ostream& s) const {
    s << "<tool name=\"" << name
      << "\" location=\"" << location << "\"";

    if (capabilities.get() != 0) {
      s << capabilities << "</tool>";
    }
    else {
      s << "/>";
    }
  }

  /**
   * \pre the reader points to a tool element
   *
   * @param r the XML text reader to read the data from
   **/
  inline tool::ptr tool::read(xml2pp::text_reader& r) throw () {
    std::string name;
    std::string location;

    if (!(r.get_attribute(&name, "name") && r.get_attribute(&location, "location"))) {
      throw (exception(exception_identifier::required_attributes_missing, "tool"));
    }

    r.read();

    sip::tool::capabilities::ptr c = sip::tool::capabilities::read(r);

    r.skip_end_element("tool");

    if (c.get() == 0) {
      return (tool::ptr(new tool(name, location)));
    }
    else {
      return (tool::ptr(new tool(name, location, c)));
    }
  }

  inline const sip::tool::capabilities& tool::get_capabilities() const {
    return (*capabilities);
  }

  inline std::string& tool::get_location() {
    return (location);
  }

  inline std::string& tool::get_name() {
    return (name);
  }
}

#endif
