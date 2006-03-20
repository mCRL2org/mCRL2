#ifndef SIP_TOOL_CAPABILITIES_H
#define SIP_TOOL_CAPABILITIES_H

#include <set>
#include <ostream>
#include <sstream>

#include <boost/shared_ptr.hpp>

#include <xml2pp/text_reader.h>
#include <sip/detail/object.h>
#include <sip/detail/configuration.h>
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
      typedef boost::shared_ptr < tool_capabilities >      ptr;

    private:

      /** \brief The protocol version */
      version                  protocol_version;

      /** \brief The available input configurations */
      input_combination_list   input_combinations;

      /** \brief Whether the configuration can be changed through user interaction, after the start signal */
      bool                     interactive;

    public:

      /** \brief Constructor */
      inline tool_capabilities(const version = default_protocol_version);

      /** \brief Add an input configuration */
      inline void add_input_combination(object::identifier, tool_category, storage_format);

      /** \brief Get the protocol version */
      inline version get_version() const;

      /** \brief Set or reset flag that the tool is interactive (configuration may change through user interaction) */
      inline void set_interactive(bool);

      /** \brief Read from XML stream */
      inline static tool_capabilities::ptr read(xml2pp::text_reader& reader) throw ();

      /** \brief Write to XML string */
      inline std::string write() const;

      /** \brief Write to XML stream */
      inline void write(std::ostream&) const;

      /** \brief Returns a reference to the list of input combinations */
      inline const input_combination_list& get_input_combinations() const;
  };

  /**
   * \brief Operator for writing to stream
   *
   * @param s stream to write to
   * @param p the tool_capabilities object to write out
   **/
  inline std::ostream& operator << (std::ostream& s, const tool_capabilities& t) {
    t.write(s);

    return (s);
  }

  inline tool_capabilities::tool_capabilities(const version v) : protocol_version(v), interactive(false) {
  }

  inline void tool_capabilities::add_input_combination(object::identifier identifier, tool_category c, storage_format f) {
    input_combination ic(c, f);

    input_combinations.insert(ic);
  }

  inline version tool_capabilities::get_version() const {
    return (protocol_version);
  }

  inline std::string tool_capabilities::write() const {
    std::ostringstream output;

    write(output);

    return (output.str());
  }

  inline const tool_capabilities::input_combination_list& tool_capabilities::get_input_combinations() const {
    return (input_combinations);
  }

  inline void tool_capabilities::write(std::ostream& output) const {
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

  /** \attention if the reader does not point at a capabilities element nothing is read */
  inline tool_capabilities::tool_capabilities::ptr tool_capabilities::read(xml2pp::text_reader& r) throw () {
    if (r.is_element("capabilities")) {
      version v = {0,0};

      r.read();

      assert (r.is_element("protocol-version"));

      r.get_attribute(&v.major, "major");
      r.get_attribute(&v.minor, "minor");

      tool_capabilities::ptr c(new tool_capabilities(v));

      r.read();
      r.skip_end_element("protocol-version");

      if (r.is_element("interactivity")) {
        c->interactive = r.get_attribute("level");

        r.read();
        r.skip_end_element("interactivity");
      }

      assert (r.is_element("input-configuration"));

      while (r.is_element("input-configuration")) {
        input_combination ic;

        r.get_attribute(&ic.first, "category");
        r.get_attribute(&ic.second, "format");

        c->input_combinations.insert(ic);

        r.read();
        r.skip_end_element("input-configuration");
      }

      return (c);
    }

    return (tool_capabilities::ptr());
  }
}

#endif

