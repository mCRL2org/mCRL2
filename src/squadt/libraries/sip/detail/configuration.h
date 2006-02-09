#ifndef SIP_CONFIGURATION_H
#define SIP_CONFIGURATION_H

#include <map>
#include <ostream>

#include <boost/bind.hpp>

#include <sip/detail/option.h>

namespace sip {

  /** This class models a tool configuration */
  class configuration {
    friend class report;

    public:
      /** Type used to contrain occurences of options within a configuration */
      struct option_constraint {
        unsigned short minimum; /* minimum occurences of this option in a single configuration */
        unsigned short maximum; /* maximum occurences of this option in a single configuration */
      };

      /** Type for a pointer to an option object */
      typedef boost::shared_ptr < option >             option_ptr;

      /** The optional constraint, option is either not present or only present once */
      const static option_constraint constrain_optional;
  
      /** The required constraint, option (with possible arguments) must be present */
      const static option_constraint constrain_required;

    private:

      typedef std::map < option_ptr, option_constraint >  option_list;

      /** The list of configuration options */
      option_list options;

      /** Whether the configuration can be changed through user interaction */
      bool interactive;

    public:

      inline configuration();

      /** Set or reset flag that the tool is interactive (configuration may change through user interaction) */
      inline void set_interactive(bool);

      /** Returns whether the configuration is empty or not */
      inline bool is_empty() const;

      /** Add an option to the configuration */
      inline void add_option(const option::identifier);

      /** Add an option from the configuration */
      inline void remove_option(const option::identifier);

      /** Get an option by its id */
      inline option_ptr get_option(const option::identifier) const;

      /** Generate XML representation */
      inline void to_xml(std::ostream&) const;

      /** Read a configuration class from XML */
      static inline configuration* from_xml(xml2pp::text_reader&);
  };

  inline configuration::configuration() : interactive(false) {
  }

  inline void configuration::set_interactive(bool b) {
    interactive = b;
  }

  inline bool configuration::is_empty() const {
    return (0 == options.size());
  }

  inline void configuration::add_option(const option::identifier id) {
    using namespace std;
    using namespace boost;

    assert(find_if(options.begin(), options.end(), bind(equal_to < option::identifier >(),
                    bind(&option::get_id,
                            bind(&option_ptr::get,
                                    bind(&option_list::value_type::first, _1))),id)) == options.end());

    options[option_ptr(new option(id))] = constrain_optional;
  }

  inline void configuration::remove_option(const option::identifier id) {
    using namespace std;
    using namespace boost;

    option_list::iterator i = find_if(options.begin(), options.end(),
                    bind(equal_to < option::identifier >(),
                            bind(&option::get_id,
                                    bind(&option_ptr::get,
                                            bind(&option_list::value_type::first, _1))), id));

    assert(i != options.end());

    options.erase(i);
  }

  inline option::option_ptr configuration::get_option(const option::identifier id) const {
    using namespace std;
    using namespace boost;

    option_list::const_iterator i = find_if(options.begin(), options.end(),
                    bind(equal_to < option::identifier >(),
                            bind(&option::get_id,
                                    bind(&option_ptr::get,
                                            bind(&option_list::value_type::first, _1))), id));

    assert(i != options.end());

    return ((*i).first);
  }

  inline void configuration::to_xml(std::ostream& output) const {
          option_list::const_iterator i = options.begin();
    const option_list::const_iterator b = options.end();

    output << "<configuration>";

    while (i != b) {
      (*i).first->to_xml(output);

      ++i;
    }

    output << "</configuration>";
  }

  /* Precondition : the current element must be <configuration> */
  inline configuration* configuration::from_xml(xml2pp::text_reader& reader) {
    configuration* c = new configuration();

    assert(reader.is_element("configuration"));

    reader.read();

    while (!reader.is_end_element()) {
      /* Current element must be <option> */
      c->options[option::from_xml(reader)] = constrain_optional;
    }

    return (c);
  }
}

#endif
