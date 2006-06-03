#ifndef SIP_CONFIGURATION_H
#define SIP_CONFIGURATION_H

#include <list>
#include <map>
#include <ostream>
#include <sstream>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <iterator_wrapper/indirect_iterator.h>

#include <sip/detail/option.h>
#include <sip/detail/object.h>

namespace sip {

  /** \brief This class models a tool configuration */
  class configuration {
    friend class report;

    public:

      /** \brief Type used to contrain occurences of options within a configuration */
      struct option_constraint {
        unsigned short minimum; ///< \brief minimum occurences of this option in a single configuration
        unsigned short maximum; ///< \brief maximum occurences of this option in a single configuration
      };

      /** \brief Until there is something better this is the type for a tool category */
      typedef std::string                         tool_category;

      /** \brief Type to hide for a pointer to an option object */
      typedef boost::shared_ptr < configuration > sptr;

    private:

      /** Convenience type for container for options */
      typedef std::map < option::sptr, option_constraint >  option_list;

      /** Convenience type for container for objects */
      typedef std::list < object::sptr >                    object_list;

    public:

      /** \brief Type for iterating the object list */
      typedef iterator_wrapper::constant_indirect_iterator < object_list, object > object_iterator;

    public:

      /** \brief The optional constraint, option is either not present or only present once */
      const static option_constraint              constrain_optional;
  
      /** \brief The required constraint, option (with possible arguments) must be present */
      const static option_constraint              constrain_required;

    private:

      /** \brief The list of configuration options */
      option_list options;

      /** \brief The list of input/output objects */
      object_list objects;

      /** \brief Points to an object that contains describes the main input file and the selected tool_category */
      std::pair < object::sptr, tool_category > input_combination;

    public:

      /** \brief Constructor */
      inline configuration();

      /** \brief Returns whether the configuration is empty or not */
      inline bool is_empty() const;

      /** \brief Add an option to the configuration */
      inline option& add_option(const option::identifier, bool = true);

      /** \brief Establishes whether an option exists (by identifier) */
      inline bool option_exists(const option::identifier);

      /** \brief Remove an option from the configuration */
      inline void remove_option(const option::identifier);

      /** \brief Get an option by its id */
      inline option::sptr get_option(const option::identifier) const;

      /** \brief Add an input/output object to the configuration */
      inline void add_object(const object::identifier, object::storage_format, object::type, object::uri = "");

      /** \brief Add an input/output object to the configuration */
      inline void add_object(object::sptr);

      /** \brief Establishes whether an object exists (by identifier) */
      inline bool object_exists(const object::identifier);

      /** \brief Remove an input/output object from the configuration */
      inline void remove_object(const object::identifier);

      /** \brief Get an input/output object from the configuration */
      inline object::sptr get_object(const object::identifier);

      /** \brief Add an input object to the configuration */
      inline void add_input(const object::identifier, object::storage_format, object::uri = "");

      /** \brief Remove an input object from the configuration */
      inline void remove_input(const object::identifier);

      /** \brief Get an input object by its id */
      inline object::sptr get_input(const object::identifier);

      /** \brief Add an output object to the configuration */
      inline void add_output(const object::identifier, object::storage_format, object::uri = "");

      /** \brief Remove an output object from the configuration */
      inline void remove_output(const object::identifier);

      /** \brief Get an output object by its id */
      inline object::sptr get_output(const object::identifier);

      /** \brief Get an iterator for the objects */
      inline object_iterator get_object_iterator();

      /** \brief Output XML representation to string */
      inline std::string write() const;

      /** \brief Output XML representation to stream */
      inline void write(std::ostream&) const;

      /** \brief Read a configuration class from XML */
      inline static configuration::sptr read(const std::string&);

      /** \brief Read a configuration class from XML */
      inline static configuration::sptr read(xml2pp::text_reader&) throw ();
  };

  /**
   * \brief Operator for writing to stream
   *
   * @param s stream to write to
   * @param c the configuration object to write out
   **/
  inline std::ostream& operator << (std::ostream& s, const configuration& c) {
    c.write(s);

    return (s);
  }

  inline configuration::configuration() {
  }

  inline bool configuration::is_empty() const {
    return (0 == options.size());
  }

  /**
   * @param id an identifier for the option
   * @param r whether or not to replace an existing option with the same id
   **/
  inline option& configuration::add_option(const option::identifier id, bool r) {
    using namespace std;
    using namespace boost;

    assert(r || find_if(options.begin(), options.end(), bind(equal_to < option::identifier >(),
                    bind(&option::get_id,
                            bind(&option::sptr::get,
                                    bind(&option_list::value_type::first, _1))),id)) == options.end());

    option::sptr o(new option(id));

    options[o] = constrain_optional;

    return (*o);
  }

  /**
   * @param id an identifier for the option
   **/
  inline bool configuration::option_exists(const option::identifier id) {
    using namespace std;
    using namespace boost;

    return (find_if(options.begin(), options.end(), bind(equal_to < option::identifier >(),
                    bind(&option::get_id,
                            bind(&option::sptr::get,
                                    bind(&option_list::value_type::first, _1))),id)) != options.end());
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
                                    bind(&option::sptr::get,
                                            bind(&option_list::value_type::first, _1))), id));

    assert(i != options.end());

    options.erase(i);
  }

  /**
   * @param id an identifier for the option
   **/
  inline option::sptr configuration::get_option(const option::identifier id) const {
    using namespace std;
    using namespace boost;

    option_list::const_iterator i = find_if(options.begin(), options.end(),
                    bind(equal_to < option::identifier >(),
                            bind(&option::get_id,
                                    bind(&option::sptr::get,
                                            bind(&option_list::value_type::first, _1))), id));

    assert(i != options.end());

    return ((*i).first);
  }

  inline configuration::object_iterator configuration::get_object_iterator() {
    return (object_iterator(objects));
  }

  inline std::string configuration::write() const {
    std::ostringstream output;

    write(output);

    return (output.str());
  }

  /**
   * @param output the stream to which the output is written
   **/
  inline void configuration::write(std::ostream& output) const {
    output << "<configuration>";

    /* Add input combination */
    if (input_combination.first.get() != 0) {
      output << "<input-combination category=\"" << input_combination.second
             << "\" format=\"" << input_combination.first->format;
     
      if (!input_combination.first->location.empty()) {
        output << "\" location=\"" << input_combination.first;
      }
     
      output << "\">";
    }

    {
            option_list::const_iterator i = options.begin();
      const option_list::const_iterator b = options.end();
     
      while (i != b) {
        (*i).first->write(output);
     
        ++i;
      }
    }

    {
            object_list::const_iterator i = objects.begin();
      const object_list::const_iterator b = objects.end();
     
      while (i != b) {
        (*i)->write(output);
     
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
  inline void configuration::add_object(const object::identifier id, object::storage_format f, object::type t, object::uri l) {
    using namespace std;
    using namespace boost;

    assert(find_if(objects.begin(), objects.end(), bind(equal_to < object::identifier >(),
                    bind(&object::get_id,
                            bind(&object::sptr::get,_1)),id)) == objects.end());

    objects.push_back(object::sptr(new object(id, f, l, t)));
  }

  /**
   * @param o a pointer to an existing object
   **/
  inline void configuration::add_object(object::sptr o) {
    using namespace std;
    using namespace boost;

    assert(find_if(objects.begin(), objects.end(), bind(equal_to < object::identifier >(),
                    bind(&object::get_id,
                            bind(&object::sptr::get,_1)),o->get_id())) == objects.end());

    objects.push_back(o);
  }

  /**
   * @param id a unique identifier for the object
   * @param f the storage format the object uses
   * @param l the location for the object (optional)
   **/
  inline void configuration::add_input(const object::identifier id, object::storage_format f, object::uri l) {
    add_object(id, f, object::input, l);
  }

  /**
   * @param id a unique identifier for the object
   * @param f the storage format the object uses
   * @param l the location for the object (optional)
   **/
  inline void configuration::add_output(const object::identifier id, object::storage_format f, object::uri l) {
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
                                    bind(&object::sptr::get,_1)), id));

    assert(i != objects.end());

    objects.erase(i);
  }

  /**
   * @param id a unique identifier for the object
   **/
  inline void configuration::remove_input(const object::identifier id) {
    assert(get_object(id)->get_type() == object::input);

    remove_object(id);
  }

  /**
   * @param id a unique identifier for the object
   **/
  inline bool configuration::object_exists(const object::identifier id) {
    using namespace std;
    using namespace boost;

    return (find_if(objects.begin(), objects.end(), bind(equal_to < object::identifier >(),
                    bind(&object::get_id,
                            bind(&object::sptr::get,_1)),id)) != objects.end());
  }

  /**
   * @param id a unique identifier for the object
   **/
  inline void configuration::remove_output(const object::identifier id) {
    assert(get_object(id)->get_type() == object::output);

    remove_object(id);
  }

  /**
   * @param id an identifier for the object
   **/
  inline object::sptr configuration::get_object(const object::identifier id) {
    using namespace std;
    using namespace boost;

    object_list::iterator i = find_if(objects.begin(), objects.end(),
                    bind(equal_to < object::identifier >(),
                            bind(&object::get_id,
                                    bind(&object::sptr::get,_1)), id));

    assert(i != objects.end());

    return (*i);
  }

  /**
   * @param id a unique identifier for the object
   **/
  inline object::sptr configuration::get_input(const object::identifier id) {
    assert(get_object(id)->get_type() == object::input);

    return (get_object(id));
  }

  /**
   * @param id a unique identifier for the object
   **/
  inline object::sptr configuration::get_output(const object::identifier id) {
    assert(get_object(id)->get_type() == object::output);

    return (get_object(id));
  }

  /**
   * @param s the string containing an XML specification of the configuration
   **/
  inline configuration::sptr configuration::read(const std::string& s) {
    xml2pp::text_reader reader(s.c_str());

    return (read(reader));
  }

  /**
   * @param reader is a reference to a libXML 2 text reader instance
   * /pre the reader points to a \<configuration\> instance
   * /post the readers position is just past the configuration block
   **/
  inline configuration::sptr configuration::read(xml2pp::text_reader& reader) throw () {
    configuration::sptr c(new configuration);

    assert(reader.is_element("configuration"));

    reader.read();

    /* Read input combination */
    if (reader.is_element("input-combination")) {
      object::storage_format format;
      object::uri            location;

      reader.get_attribute(&c->input_combination.second, "category");
      reader.get_attribute(&format, "format");
      reader.get_attribute(&location, "location");

      c->input_combination.first = object::sptr(new object (0, format, location));

      reader.read();

      if (reader.is_end_element() && reader.is_element("input-combination")) {
        reader.read();
      }
    }

    while (!(reader.is_end_element() && reader.is_element("configuration"))) {
      /* Current element must be <option> */
      if (reader.is_element("option")) {
        c->options[option::read(reader)] = constrain_optional;
      }
      else if (reader.is_element("object")) {
        c->objects.push_back(object::read(reader));
      }
    }

    reader.skip_end_element("configuration");

    return (c);
  }
}

#endif
