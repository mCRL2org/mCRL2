#ifndef SIP_CONFIGURATION_H
#define SIP_CONFIGURATION_H

#include <list>
#include <map>
#include <ostream>
#include <sstream>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <utility/visitor.h>
#include <utility/indirect_iterator.h>

#include <sip/detail/option.h>
#include <sip/detail/object.h>
#include <sip/detail/common.h>
#include <sip/mime_type.h>

namespace sip {

  /** \brief This class models a tool configuration */
  class configuration : public boost::noncopyable, public utility::visitable < configuration > {
    friend class report;
    friend class sip::tool::communicator;
    friend class sip::controller::communicator;

    public:

      /** \brief Type used to constrain occurrences of options within a configuration */
      struct option_constraint {
        unsigned short minimum; ///< \brief minimum occurrences of this option in a single configuration
        unsigned short maximum; ///< \brief maximum occurrences of this option in a single configuration
      };

      /** \brief Until there is something better this is the type for a tool category */
      typedef std::string                         tool_category;

      /** \brief Type to hide for a pointer to an option object */
      typedef boost::shared_ptr < configuration > sptr;

    private:

      /** \brief Convenience type for container for options */
      typedef std::map < option::sptr, option_constraint >  option_list;

      /** \brief Convenience type for container for objects */
      typedef std::list < object::sptr >                    object_list;

    private:

      /** \brief Default Constructor */
      inline configuration();

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
      option_list   options;

      /** \brief The list of input/output objects */
      object_list   objects;

      /** \brief The selected category in which the tool operates */
      tool_category category;

      /** \brief Whether or not the tool accepted this configuration in the past */
      bool          fresh;

      /** \brief Prefix for output objects */
      std::string   output_prefix;

    private:

      /** \brief Constructor */
      inline configuration(tool_category);

    public:

      /** \brief Returns whether the configuration is empty or not */
      bool is_empty() const;

      /** \brief Add an option to the configuration */
      option& add_option(const option::identifier, bool = true);

      /** \brief Establishes whether an option exists (by identifier) */
      bool option_exists(const option::identifier) const;

      /** \brief Remove an option from the configuration */
      void remove_option(const option::identifier);

      /** \brief Get the state of the configuration */
      bool is_fresh();

      /** \brief Set the prefix for output files */
      void set_output_prefix(std::string const&);

      /** \brief Get the prefix for output files */
      std::string get_output_prefix();

      /** \brief Prepends the output prefix to the argument to form a valid file name */
      std::string get_input_name(std::string const&);

      /** \brief Prepends the output prefix to the argument to form a valid file name */
      std::string get_output_name(std::string const&);

      /** \brief The category in which the tool operates */
      tool_category get_category() const;

      /** \brief Get the value of an option argument */
      boost::any get_option_value(const option::identifier id) const;

      /** \brief Get an option by its id */
      option::sptr get_option(const option::identifier) const;

      /** \brief Add an input/output object to the configuration */
      void add_object(object::identifier const&, mime_type const&, object::type, object::uri = "");

      /** \brief Add an input/output object to the configuration */
      void add_object(object::sptr);

      /** \brief Establishes whether an object exists (by identifier) */
      bool object_exists(const object::identifier) const;

      /** \brief Remove an input/output object from the configuration */
      void remove_object(const object::identifier);

      /** \brief Get an input/output object from the configuration */
      object::sptr const get_object(const object::identifier) const;

      /** \brief Add an input object to the configuration */
      void add_input(object::identifier const&, mime_type const&, object::uri = "");

      /** \brief Remove an input object from the configuration */
      void remove_input(object::identifier const&);

      /** \brief Get an input object by its id */
      object::sptr get_input(const object::identifier);

      /** \brief Add an output object to the configuration */
      void add_output(object::identifier const&, mime_type const&, object::uri = "");

      /** \brief Remove an output object from the configuration */
      void remove_output(object::identifier const&);

      /** \brief Get an output object by its id */
      object::sptr get_output(const object::identifier);

      /** \brief Get an iterator for the objects */
      object_iterator get_object_iterator();

      /** \brief Output XML representation to string */
      std::string write() const;

      /** \brief Output XML representation to stream */
      void write(std::ostream&) const;

      /** \brief Read a configuration class from XML */
      static configuration::sptr read(const std::string&);

      /** \brief Read a configuration class from XML */
      static configuration::sptr read(xml2pp::text_reader&);
  };

  inline configuration::configuration() : fresh(true) {
  }

  inline configuration::configuration(tool_category c) : category(c), fresh(true) {
  }

  inline bool configuration::is_empty() const {
    return (0 == options.size());
  }

  /**
   * \pre the option with the matching id must exist in the configuration and have exactly one argument
   **/
  inline boost::any configuration::get_option_value(const option::identifier id) const {
    return(get_option(id)->get_value());
  }

  inline configuration::tool_category configuration::get_category() const {
    return (category);
  }

  inline bool configuration::is_fresh() {
    return (fresh);
  }

  /**
   * @param[in] p the string to set as output prefix
   **/
  inline void configuration::set_output_prefix(std::string const& p) {
    output_prefix = p;
  }

  inline std::string configuration::get_output_prefix() {
    return (output_prefix);
  }

  /**
   * @param[in] n suffix of the name
   **/
  inline std::string configuration::get_output_name(std::string const& n) {
    return (output_prefix + n);
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
   * @param id a unique identifier for the object
   * @param m the storage format the object uses
   * @param l the location for the object (optional)
   **/
  inline void configuration::add_input(object::identifier const& id, mime_type const& m, object::uri l) {
    add_object(id, m, object::input, l);
  }

  /**
   * @param id a unique identifier for the object
   * @param m the storage format the object uses
   * @param l the location for the object (optional)
   **/
  inline void configuration::add_output(object::identifier const& id, mime_type const& m, object::uri l) {
    add_object(id, m, object::output, l);
  }

  /**
   * @param id a unique identifier for the object
   **/
  inline void configuration::remove_input(object::identifier const& id) {
    assert(get_object(id)->get_type() == object::input);

    remove_object(id);
  }

  /**
   * @param id a unique identifier for the object
   **/
  inline void configuration::remove_output(object::identifier const& id) {
    assert(get_object(id)->get_type() == object::output);

    remove_object(id);
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

}

#endif
