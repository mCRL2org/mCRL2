#ifndef SIP_CONFIGURATION_H
#define SIP_CONFIGURATION_H

#include <list>
#include <map>
#include <ostream>
#include <sstream>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <sip/detail/option.h>
#include <sip/detail/object.h>

namespace sip {

  /** \brief This class models a tool configuration */
  class configuration {
    friend class report;

    public:
      /** \brief Type used to contrain occurences of options within a configuration */
      struct option_constraint {
        unsigned short minimum; /// \brief minimum occurences of this option in a single configuration
        unsigned short maximum; /// \brief maximum occurences of this option in a single configuration
      };

      /** \brief Type for a pointer to an option object */
      typedef option::option_ptr                  option_ptr;

      /** \brief Type for a pointer to an object object */
      typedef object::object_ptr                  object_ptr;

      /** \brief Type to hide for a pointer to an option object */
      typedef boost::shared_ptr < configuration > configuration_ptr;

      /** \brief The optional constraint, option is either not present or only present once */
      const static option_constraint              constrain_optional;
  
      /** \brief The required constraint, option (with possible arguments) must be present */
      const static option_constraint              constrain_required;

    private:

      /** Convenience type for container for options */
      typedef std::map < option_ptr, option_constraint >  option_list;

      /** Convenience type for container for objects */
      typedef std::list < object_ptr >                    object_list;

      /** \brief The list of configuration options */
      option_list options;

      /** \brief The list of input/output objects */
      object_list objects;

    public:

      inline configuration();

      /** \brief Returns whether the configuration is empty or not */
      inline bool is_empty() const;

      /** \brief Add an option to the configuration */
      inline void add_option(const option::identifier);

      /** \brief Remove an option from the configuration */
      inline void remove_option(const option::identifier);

      /** \brief Get an option by its id */
      inline option_ptr get_option(const option::identifier) const;

      /** \brief Add an input/output object to the configuration */
      inline void add_object(object::identifier, object::storage_format, object::type, object::uri = "");

      /** \brief Remove an input/output object from the configuration */
      inline void remove_object(object::identifier);

      /** \brief Get an input/output object from the configuration */
      inline object_ptr get_object(object::identifier);

      /** \brief Add an input object to the configuration */
      inline void add_input(object::identifier, object::storage_format, object::uri = "");

      /** \brief Remove an input object from the configuration */
      inline void remove_input(object::identifier);

      /** \brief Get an input object by its id */
      inline object_ptr get_input(object::identifier);

      /** \brief Add an output object to the configuration */
      inline void add_output(object::identifier, object::storage_format, object::uri = "");

      /** \brief Remove an output object from the configuration */
      inline void remove_output(object::identifier);

      /** \brief Get an output object by its id */
      inline object_ptr get_output(object::identifier);

      /** \brief Output XML representation to string */
      inline std::string to_xml() const;

      /** \brief Output XML representation to stream */
      inline void to_xml(std::ostream&) const;

      /** \brief Read a configuration class from XML */
      static inline configuration_ptr from_xml(xml2pp::text_reader&) throw ();
  };

  inline configuration::configuration() {
  }

  inline bool configuration::is_empty() const {
    return (0 == options.size());
  }

  /**
   * @param id an identifier for the option
   **/
  inline void configuration::add_option(const option::identifier id) {
    using namespace std;
    using namespace boost;

    assert(find_if(options.begin(), options.end(), bind(equal_to < option::identifier >(),
                    bind(&option::get_id,
                            bind(&option_ptr::get,
                                    bind(&option_list::value_type::first, _1))),id)) == options.end());

    options[option_ptr(new option(id))] = constrain_optional;
  }

  /**
   * @param id an identifier for the option
   **/
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

  /**
   * @param id an identifier for the option
   **/
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

  inline std::string configuration::to_xml() const {
    std::ostringstream output;

    to_xml(output);

    return (output.str());
  }

  /**
   * @param output the stream to which the output is written
   **/
  inline void configuration::to_xml(std::ostream& output) const {
    output << "<configuration>";

    {
            option_list::const_iterator i = options.begin();
      const option_list::const_iterator b = options.end();
     
      while (i != b) {
        (*i).first->to_xml(output);
     
        ++i;
      }
    }

    {
            object_list::const_iterator i = objects.begin();
      const object_list::const_iterator b = objects.end();
     
      while (i != b) {
        (*i)->to_xml(output);
     
        ++i;
      }
    }

    output << "</configuration>";
  }

  /**
   * @param id a unique identifier for the object
   * @param f the storage format the object uses
   * @param l the location for the object (optional)
   * @param t the object type
   **/
  inline void configuration::add_object(object::identifier id, object::storage_format f, object::type t, object::uri l) {
    using namespace std;
    using namespace boost;

    assert(find_if(objects.begin(), objects.end(), bind(equal_to < object::identifier >(),
                    bind(&object::get_id,
                            bind(&object_ptr::get,_1)),id)) == objects.end());

    objects.push_back(object_ptr(new object(id, f, l, t)));
  }

  /**
   * @param id a unique identifier for the object
   * @param f the storage format the object uses
   * @param l the location for the object (optional)
   **/
  inline void configuration::add_input(object::identifier id, object::storage_format f, object::uri l) {
    add_object(id, f, object::input, l);
  }

  /**
   * @param id a unique identifier for the object
   * @param f the storage format the object uses
   * @param l the location for the object (optional)
   **/
  inline void configuration::add_output(object::identifier id, object::storage_format f, object::uri l) {
    add_object(id, f, object::output, l);
  }

  /**
   * @param id an identifier for the object
   **/
  inline void configuration::remove_object(const object::identifier id) {
    using namespace std;
    using namespace boost;

    object_list::iterator i = find_if(objects.begin(), objects.end(),
                    bind(equal_to < object::identifier >(),
                            bind(&object::get_id,
                                    bind(&object_ptr::get,_1)), id));

    assert(i != objects.end());

    objects.erase(i);
  }

  /**
   * @param id a unique identifier for the object
   **/
  inline void configuration::remove_input(object::identifier id) {
    assert(get_object(id)->get_type() == object::input);

    remove_object(id);
  }

  /**
   * @param id a unique identifier for the object
   **/
  inline void configuration::remove_output(object::identifier id) {
    assert(get_object(id)->get_type() == object::output);

    remove_object(id);
  }

  /**
   * @param id an identifier for the object
   **/
  inline object::object_ptr configuration::get_object(const object::identifier id) {
    using namespace std;
    using namespace boost;

    object_list::iterator i = find_if(objects.begin(), objects.end(),
                    bind(equal_to < object::identifier >(),
                            bind(&object::get_id,
                                    bind(&object_ptr::get,_1)), id));

    assert(i != objects.end());

    return (*i);
  }

  /**
   * @param id a unique identifier for the object
   **/
  inline object::object_ptr configuration::get_input(object::identifier id) {
    assert(get_object(id)->get_type() == object::input);

    return (get_object(id));
  }

  /**
   * @param id a unique identifier for the object
   **/
  inline object::object_ptr configuration::get_output(object::identifier id) {
    assert(get_object(id)->get_type() == object::output);

    return (get_object(id));
  }

  /**
   * @param reader is a reference to a libXML 2 text reader instance
   * /pre the reader points to a <configuration> instance
   * /post the readers position is just past the configuration block
   **/
  inline configuration::configuration_ptr configuration::from_xml(xml2pp::text_reader& reader) throw () {
    configuration_ptr c(new configuration);

    assert(reader.is_element("configuration"));

    reader.read();

    while (!(reader.is_end_element() && reader.is_element("configuration"))) {
      /* Current element must be <option> */
      if (reader.is_element("option")) {
        c->options[option::from_xml(reader)] = constrain_optional;
      }
      else if (reader.is_element("object")) {
        c->objects.push_back(object::from_xml(reader));
      }
    }

    return (c);
  }
}

#endif
