// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/configuration.hpp
/// \brief Type used for representing tool configurations (protocol concept)

#ifndef TIPI_CONFIGURATION_HPP__
#define TIPI_CONFIGURATION_HPP__

#include <vector>
#include <string>
#include <map>
#include <set>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/range/iterator_range.hpp>

#include "tipi/detail/utility/generic_visitor.hpp"

#include "tipi/uri.hpp"
#include "tipi/mime_type.hpp"
#include "tipi/tool/category.hpp"

namespace tipi {

  namespace controller {
    class communicator;
    class communicator_impl;
  }
  namespace tool {
    class communicator;
    class communicator_impl;
  }

  class configuration;

  class report;

  namespace controller {
    class communicator;
    class communicator_impl;
  }

  namespace tool {
    class communicator;
    class communicator_impl;
  }

  /** \brief This class models a tool configuration */
  class configuration : public ::utility::visitable {

    friend class tipi::report;
    friend class tipi::tool::communicator_impl;
    friend class tipi::tool::communicator;
    friend class tipi::controller::communicator_impl;
    friend class tipi::controller::communicator;

    template < typename R, typename S >
    friend class ::utility::visitor;

    public:

      /** \brief Base class for configuration atoms */
      class parameter : public ::utility::visitable {

        public:

          /** \brief Parameter identifier */
          typedef std::string identifier;
      };

      /** \brief Concrete descriptor for a data object (such as a file in the local file system) */
      class object;

      /** \brief Descriptor for optional functionality */
      class option;

    private:

      /** \brief Convenience type for associating string identifiers to objects */
      typedef std::map < parameter::identifier, size_t >              id_parameter_map;

      /** \brief Type to model a partition over position list */
      typedef std::set < parameter* >                                 position_list_partition;

      /** \brief Type for keeping the relative ordering of objects and options */
      typedef std::vector < boost::shared_ptr < parameter > >   position_list;


    public:

      /** \brief Type for position list iteration (dereferences pointers) */
      typedef boost::indirect_iterator < position_list::const_iterator >                         const_indirect_position_iterator;

      /** \brief Type for position list iteration (dereferences pointers) */
      typedef boost::indirect_iterator < position_list::iterator >                               indirect_position_iterator;

      /** \brief Predicate for filtering on type */
      typedef boost::function< bool (parameter const&) >                                         type_filter_predicate;

      /** \brief Type for option iteration */
      typedef boost::filter_iterator < type_filter_predicate, const_indirect_position_iterator > const_option_iterator;

      /** \brief Type for option iteration */
      typedef boost::filter_iterator < type_filter_predicate, indirect_position_iterator >       option_iterator;

      /** \brief Type for input iteration */
      typedef boost::filter_iterator < type_filter_predicate, const_indirect_position_iterator > const_input_iterator;

      /** \brief Type for input iteration */
      typedef boost::filter_iterator < type_filter_predicate, indirect_position_iterator >       input_iterator;

      /** \brief Type for output iteration */
      typedef boost::filter_iterator < type_filter_predicate, const_indirect_position_iterator > const_output_iterator;

      /** \brief Type for output iteration */
      typedef boost::filter_iterator < type_filter_predicate, indirect_position_iterator >       output_iterator;

      /** \brief Type for option iterator ranges */
      typedef boost::iterator_range < const_option_iterator >                                    const_iterator_option_range;

      /** \brief Type for option iterator ranges */
      typedef boost::iterator_range < option_iterator >                                          iterator_option_range;

      /** \brief Type for input iterator ranges */
      typedef boost::iterator_range < const_input_iterator >                                     const_iterator_input_range;

      /** \brief Type for input iterator ranges */
      typedef boost::iterator_range < input_iterator >                                           iterator_input_range;

      /** \brief Type for output iterator ranges */
      typedef boost::iterator_range < const_output_iterator >                                    const_iterator_output_range;

      /** \brief Type for output iterator ranges */
      typedef boost::iterator_range < output_iterator >                                          iterator_output_range;

    private:

      /** \brief The list of configuration options */
      position_list_partition     m_options;

      /** \brief The list of input objects */
      position_list_partition     m_input_objects;

      /** \brief The list of output objects */
      position_list_partition     m_output_objects;

      /** \brief Mapping of id to option objects */
      id_parameter_map            m_parameter_by_id;

      /** \brief Sequence of identifiers of options and objects */
      position_list               m_positions;

      /** \brief The selected category in which the tool operates */
      tipi::tool::category        m_category;

      /** \brief Whether or not the tool accepted this configuration in the past */
      bool                        m_fresh;

      /** \brief Prefix for output objects (TODO replace when naming component is added) */
      std::string                 m_output_prefix;

    public:

      /** \brief Constructor */
      inline configuration(const tool::category::standard_category_type = tool::category::unknown);

      /** \brief Constructor */
      inline configuration(tool::category const&);

      /** \brief Returns whether the configuration is empty or not */
      bool empty() const;

      /** \brief Get the state of the configuration */
      bool fresh() const;

      /** \brief Marks configuration as fresh */
      void fresh(bool);

      /** \brief Whether or not a parameter is an option */
      bool is_option(parameter const&) const;

      /** \brief Whether or not a parameter is an input object specifier */
      bool is_input(parameter const&) const;

      /** \brief Whether or not a parameter is an output object specifier */
      bool is_output(parameter const&) const;

      /** \brief Get the identifier for a parameter within the configuration */
      std::string get_identifier(parameter const&) const;

      /** \brief Get the identifier for an object within the configuration */
      std::string get_identifier(option const&) const;

      /** \brief Get the identifier for an object within the configuration */
      std::string get_identifier(object const&) const;

      /** \brief Set the prefix for output files */
      void output_prefix(std::string const&);

      /** \brief Get the prefix for output files */
      std::string output_prefix() const;

      /** \brief The category in which the tool operates */
      tipi::tool::category category() const;

      /** \brief Prepends the output prefix to the argument to form a valid file name */
      std::string get_input_name(std::string const&) const;

      /** \brief Prepends the output prefix to the argument to form a valid file name */
      std::string get_output_name(std::string const&) const;

      /** \brief Add an option to the configuration */
      option& add_option(parameter::identifier const&, bool = true);

      /** \brief Add an option to the configuration */
      option& add_option(parameter::identifier const&, boost::shared_ptr < option >&, bool = true);

      /** \brief Establishes whether an option exists (by identifier) */
      bool option_exists(parameter::identifier const&) const;

      /** \brief Remove an option from the configuration, if it exists */
      void remove_option(parameter::identifier const&);

      /** \brief Get an option by its id */
      option const& get_option(parameter::identifier const&) const;

      /** \brief Get an option by its id */
      option& get_option(parameter::identifier const&);

      /** \brief Get the value of an option argument */
      template < typename T >
      T get_option_argument(parameter::identifier const& id, size_t const& n = 0) const;

      /** \brief Establishes whether an input object is known by this identifier (by identifier) */
      bool input_exists(parameter::identifier const&) const;

      /** \brief Establishes whether an output object is known by this identifier (by identifier) */
      bool output_exists(parameter::identifier const&) const;

      /** \brief Add an input object to the configuration */
      object& add_input(parameter::identifier const&, mime_type const&, uri const& = "");

      /** \brief Add an input object to the configuration */
      object& add_input(parameter::identifier const&, object&);

      /** \brief Add an input object to the configuration */
      object& add_input(parameter::identifier const&, boost::shared_ptr < object >&);

      /** \brief Add an output object to the configuration */
      object& add_output(parameter::identifier const&, mime_type const&, uri const& = "");

      /** \brief Add an output object to the configuration */
      object& add_output(parameter::identifier const&, object&);

      /** \brief Add an output object to the configuration */
      object& add_output(parameter::identifier const&, boost::shared_ptr < object >&);
      /** \brief Get an input object by id */
      object const& get_input(parameter::identifier const&) const;

      /** \brief Get an input object by id */
      object& get_input(parameter::identifier const&);

      /** \brief Get an output object by id */
      object const& get_output(parameter::identifier const&) const;

      /** \brief Get an output object by id */
      object& get_output(parameter::identifier const&);

      /** \brief Remove an input object from the configuration */
      void remove_input(size_t);

      /** \brief Remove an input object from the configuration */
      void remove_input(parameter::identifier const&);

      /** \brief Remove an output object from the configuration */
      void remove_output(parameter::identifier const&);

      /** \brief Remove an output object from the configuration */
      void remove_output(size_t);

      /** \brief Get an iterator for the objects that are taken as input */
      const_iterator_option_range get_options() const;

      /** \brief Get an iterator for the objects that are taken as input */
      iterator_option_range get_options();

      /** \brief Get an iterator for the objects that are taken as input */
      const_iterator_input_range get_input_objects() const;

      /** \brief Get an iterator for the objects that are taken as input */
      iterator_input_range get_input_objects();

      /** \brief Get an iterator for the objects that are produced as output */
      const_iterator_output_range get_output_objects() const;

      /** \brief Get an iterator for the objects that are produced as output */
      iterator_output_range get_output_objects();

      /** \brief Get number of options */
      size_t number_of_options() const;

      /** \brief Get number of input objects */
      size_t number_of_inputs() const;

      /** \brief Get number of output objects */
      size_t number_of_outputs() const;
  };
}

#include "tipi/option.hpp"
#include "tipi/object.hpp"

namespace tipi {

  inline configuration::configuration(tool::category const& c) :
                 m_category(c), m_fresh(true) {
  }

  inline configuration::configuration(const tool::category::standard_category_type c) :
                 m_category(tool::category::standard_categories()[c]), m_fresh(true) {
  }

  inline bool configuration::empty() const {
    return (0 == m_options.size());
  }

  inline bool configuration::fresh() const {
    return (m_fresh);
  }

  inline void configuration::fresh(bool b) {
    m_fresh = b;
  }

  /** \param[in] p the parameter to check */
  inline bool configuration::is_option(parameter const& p) const {
    return (m_options.count(const_cast < parameter* > (&p)) != 0);
  }

  /** \param[in] p the parameter to check */
  inline bool configuration::is_input(parameter const& p) const {
    return (m_input_objects.count(const_cast < parameter* > (&p)) != 0);
  }

  /** \param[in] p the parameter to check */
  inline bool configuration::is_output(parameter const& p) const {
    return (m_output_objects.count(const_cast < parameter* > (&p)) != 0);
  }

  /**
   * \param[in] p the string to set as output prefix
   *  \todo remove after naming component is added)
   **/
  inline void configuration::output_prefix(std::string const& p) {
    m_output_prefix = p;
  }

  /**
   *  \todo remove after naming component is added)
   **/
  inline std::string configuration::output_prefix() const {
    return (m_output_prefix);
  }

  inline tipi::tool::category configuration::category() const {
    return (m_category);
  }

  /**
   * \param[in] id unique identifier for the input object
   * \param[in] m the storage format the object uses
   * \param[in] l the location for the object (optional)
   * \pre no object or option is known by this identifier
   * \throws std::runtime_error on failure
   * \return the object corresponding with get_input(id)
   **/
  inline configuration::object& configuration::add_input(std::string const& id, mime_type const& m, uri const& l) {
    boost::shared_ptr < object > new_object(new object(m, l));

    return (add_input(id, new_object));
  }

  /**
   * \param[in] id unique identifier for the input object
   * \param[in] o reference to object
   * \pre no object or option is known by this identifier
   * \throws std::runtime_error on failure
   * \return the object corresponding with get_input(id)
   **/
  inline configuration::object& configuration::add_input(std::string const& id, object& o) {
    boost::shared_ptr < object > new_object(new object(o));

    return (add_input(id, new_object));
  }

  /**
   * \param[in] id unique identifier for the output object
   * \param[in] m the storage format the object uses
   * \param[in] l the location for the object (optional)
   * \pre no object or option is known by this identifier
   * \throws std::runtime_error on failure
   * \return the object corresponding with get_output(id)
   **/
  inline configuration::object& configuration::add_output(std::string const& id, mime_type const& m, uri const& l) {
    boost::shared_ptr < object > new_object(new object(m, l));

    return (add_output(id, new_object));
  }

  /**
   * \param[in] id unique identifier for the output object
   * \param[in] o shared pointer to object
   * \pre no object or option is known by this identifier
   * \throws std::runtime_error on failure
   * \return the object corresponding with get_output(id)
   **/
  inline configuration::object& configuration::add_output(std::string const& id, object& o) {
    boost::shared_ptr < object > new_object(new object(o));

    return (add_output(id, new_object));
  }

  /**
   * \param[in] id an identifier for the option
   * \return whether an option identified by id is part of the configuration
   **/
  inline bool configuration::option_exists(std::string const& id) const {
    return (m_parameter_by_id.count(id) != 0);
  }

  /**
   * \param[in] id an identifier for the option
   * \return whether an output identified by id is part of the configuration
   **/
  inline bool configuration::input_exists(std::string const& id) const {
    return (m_parameter_by_id.count(id) != 0);
  }

  /**
   * \param[in] id an identifier for the option
   * \return whether an output identified by id is part of the configuration
   **/
  inline bool configuration::output_exists(std::string const& id) const {
    return (m_parameter_by_id.count(id) != 0);
  }

  /**
   * \param[in] id unique identifier for the output object
   * \pre objects_exist(id) must hold
   * \return reference to the input object identified by id
   * \throws std::runtime_error if no input object corresponding to id is found
   **/
  inline configuration::object const& configuration::get_input(std::string const& id) const {
    if (m_parameter_by_id.count(id) == 0) {
      throw std::runtime_error("A tipi-input-configuration for \"" + id + "\" does not exist");
    }

    return (*boost::static_pointer_cast< const object >(m_positions[(*m_parameter_by_id.find(id)).second]));
  }

  /**
   * \param[in] id unique identifier for the output object
   * \pre objects_exist(id) and m_parameter_by_id[id].get() must hold
   * \return reference to the input object identified by id
   * \throws std::runtime_error if no input object corresponding to id is found
   **/
  inline configuration::object& configuration::get_input(std::string const& id) {
    if (m_parameter_by_id.count(id) == 0) {
      throw std::runtime_error("A tipi-input-configuration for \"" + id + "\" does not exist");
    }

    return (*boost::static_pointer_cast< object >(m_positions[m_parameter_by_id[id]]));
  }

  /**
   * \param[in] id an identifier for the object
   * \pre objects_exist(id) and m_parameter_by_id[id].get() in output_objects must hold
   * \return reference to the output object identified by id
   * \throws std::runtime_error if no output object corresponding to id is found
   **/
  inline configuration::object const& configuration::get_output(std::string const& id) const {
    if (m_parameter_by_id.count(id) == 0) {
      throw std::runtime_error("A tipi-input-configuration for \"" + id + "\" does not exist");
    }

    return (*boost::static_pointer_cast< const object >(m_positions[(*m_parameter_by_id.find(id)).second]));
  }

  /**
   * \param[in] id an identifier for the object
   * \pre objects_exist(id) must hold
   * \return reference to the object identified by id
   * \throws std::runtime_error if no output object corresponding to id is found
   **/
  inline configuration::object& configuration::get_output(std::string const& id) {
    if (m_parameter_by_id.count(id) == 0) {
      throw std::runtime_error("A tipi-input-configuration for \"" + id + "\" does not exist");
    }

    return (*boost::static_pointer_cast< object >(m_positions[m_parameter_by_id[id]]));
  }

  inline size_t configuration::number_of_options() const {
    return (m_options.size());
  }

  inline size_t configuration::number_of_inputs() const {
    return (m_input_objects.size());
  }

  inline size_t configuration::number_of_outputs() const {
    return (m_output_objects.size());
  }

  /**
   * \param[in] id an identifier for the option
   * \param[in] n an optional identifier for the option
   * \pre option must take at least one argument and n must be smaller than the number of arguments
   * \pre option with this identifier must be part of the configuration, use
   *      option_exists member to establish this
   * \return the value for the n-th argument of the option corresponding with id
   **/
  template < typename T >
  inline T configuration::get_option_argument(std::string const& id, size_t const& n) const {
    option const& o = *boost::static_pointer_cast < const option > (
        const_cast < position_list& > (m_positions)[(*m_parameter_by_id.find(id)).second]);

    return o.get_value< T >(n);
  }
}

#endif
