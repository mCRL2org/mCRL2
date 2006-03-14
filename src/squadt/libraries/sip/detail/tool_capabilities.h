#ifndef SIP_TOOL_CAPABILITIES_H
#define SIP_TOOL_CAPABILITIES_H

#include <set>
#include <ostream>
#include <sstream>

#include <boost/shared_ptr.hpp>

#include <xml2pp/text_reader.h>
#include <sip/detail/object.h>
#include <sip/detail/capabilities.h>

namespace sip {

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
  class tool_capabilities {
    friend class tool_communicator;
    friend class controller_communicator;

    public:

      /** \brief Until there is something better this is the type for a storage format */
      typedef object::storage_format                       storage_format;

      /** \brief Until there is something better this is the type for a tool category */
      typedef configuration::tool_category                 tool_category;

      /** \brief Convenience type definition for an input configuration */
      typedef std::pair < tool_category, storage_format >  input_combination;

      /** \brief Convenience type for a list of input configurations */
      typedef std::set  < input_combination >              input_combination_list;

      /** \brief Convenience type that hides the shared pointer implementation */
      typedef boost::shared_ptr < tool_capabilities >      tool_capabilities_ptr;

    private:

      /** \brief The protocol version */
      version                  protocol_version;

      /** \brief The available input configurations */
      input_combination_list   input_combinations;

      /** \brief Whether the configuration can be changed through user interaction, after the start signal */
      bool                     interactive;

      /** \brief Read from XML stream */
      inline static tool_capabilities_ptr from_xml(xml2pp::text_reader& reader) throw ();

    public:

      /** \brief Constructor */
      inline tool_capabilities(const version = default_protocol_version);

      /** \brief Add an input configuration */
      inline void add_input_combination(object::identifier, tool_category, storage_format);

      /** \brief Get the protocol version */
      inline version get_version() const;

      /** \brief Set or reset flag that the tool is interactive (configuration may change through user interaction) */
      inline void set_interactive(bool);

      /** \brief Write to XML string */
      inline std::string to_xml() const;

      /** \brief Write to XML stream */
      inline void to_xml(std::ostream&) const;

      /** \brief Returns a reference to input_combinations.begin() */
      inline input_combination_list::iterator begin_input_combinations();

      /** \brief Returns a reference to input_combinations.end() */
      inline input_combination_list::iterator end_input_combinations();
  };

  inline tool_capabilities::tool_capabilities(const version v) : protocol_version(v), interactive(false) {
  }

  inline void tool_capabilities::add_input_combination(object::identifier identifier, tool_category c, storage_format f) {
    input_combination ic(c, f);

    input_combinations.insert(ic);
  }

  inline version tool_capabilities::get_version() const {
    return (protocol_version);
  }

  inline std::string tool_capabilities::to_xml() const {
    std::ostringstream output;

    to_xml(output);

    return (output.str());
  }

  /** \brief Returns a reference to input_combinations.begin() */
  inline tool_capabilities::input_combination_list::iterator tool_capabilities::begin_input_combinations() {
    return (input_combinations.begin());
  }

  /** \brief Returns a reference to input_combinations.end() */
  inline tool_capabilities::input_combination_list::iterator tool_capabilities::end_input_combinations() {
    return (input_combinations.end());
  }

  inline void tool_capabilities::to_xml(std::ostream& output) const {
    output << "<capabilities>"
           << "<protocol-version major=\"" << (unsigned short) protocol_version.major
           << "\" minor=\"" << (unsigned short) protocol_version.minor << "\"/>";

    if (interactive) {
      // Tool is interactive
      output << "<interactivity level=\"1\"/>";
    }
    
    for (input_combination_list::const_iterator i = input_combinations.begin(); i != input_combinations.end(); ++i) {
      output << "<input-configuration category=\"" << (*i).first
             << "\" format=\"" << (*i).second << "\"/>";
    }

    output << "</capabilities>";
  }

  inline void tool_capabilities::set_interactive(bool b) {
    interactive = b;
  }

  /** \pre the reader must point at a capabilities element */
  inline tool_capabilities::tool_capabilities_ptr tool_capabilities::from_xml(xml2pp::text_reader& reader) throw () {
    version v = {0,0};

    reader.read();

    assert (reader.is_element("protocol-version"));
    
    reader.get_attribute(&v.major, "major");
    reader.get_attribute(&v.minor, "minor");

    tool_capabilities_ptr c(new tool_capabilities(v));

    reader.read();

    /* Skip end element */
    if (reader.is_end_element()) {
      reader.read();
    }

    if (reader.is_element("interactivity")) {
      c->interactive = reader.get_attribute("level");

      reader.read();

      /* Skip end element */
      if (reader.is_end_element()) {
        reader.read();
      }
    }

    assert (reader.is_element("input-configuration"));

    while (reader.is_element("input-configuration")) {
      input_combination ic;

      reader.get_attribute(&ic.first, "category");
      reader.get_attribute(&ic.second, "format");

      c->input_combinations.insert(ic);

      reader.read();
     
      /* Skip end element */
      if (reader.is_end_element() && reader.is_element("input-configuration")) {
        reader.read();
      }
    }

    return (c);
  }
}

#endif

