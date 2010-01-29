// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file processor.ipp
/// \brief Add your file description here.

#ifndef _PROCESSOR_IPP__
#define _PROCESSOR_IPP__

#include <algorithm>
#include <functional>

#include <boost/bind.hpp>
#include <boost/crc.hpp>
#include <boost/filesystem/convenience.hpp>

#include "task_monitor.hpp"
#include "processor.hpp"
#include "project_manager.hpp"
#include "executor.hpp"
#include "tool_manager.ipp"

namespace squadt {
  /// \cond INTERNAL_DOCS

  class processor_impl : public utility::visitable {
    friend class processor;
    friend class processor::monitor;
    friend class project_manager_impl;
    friend class squadt::restore_visitor_impl;

    friend class processor::object_descriptor;

    template < typename R, typename S >
    friend class utility::visitor;

    private:

      /** \brief Pointer type for implementation object (handle-body idiom) */
      typedef boost::shared_ptr < processor_impl >                      impl_ptr;

      /** \brief Pointer type for interface object (handle-body idiom) */
      typedef boost::shared_ptr < processor >                           interface_ptr;

      /** \brief Type alias for monitor class */
      typedef processor::monitor                                        monitor;

      /** \brief Type alias for list of inputs */
      typedef std::vector < processor::configurated_object_descriptor > input_list;

      /** \brief Type alias for list of inputs */
      typedef std::vector < processor::configurated_object_descriptor > output_list;

      class object_descriptor : public processor::object_descriptor {
        friend class processor_impl;
        friend class project_manager_impl;
        friend class processor::monitor;

        template < typename R, typename S >
        friend class utility::visitor;

        private:

          boost::weak_ptr < processor >       generator;      ///< The processor responsible for generating this object
          boost::weak_ptr < project_manager > project;        ///< The manager of the project of which this object is part (generator->impl->manager)
          tipi::mime_type                     mime_type;      ///< The used storage format
          tipi::uri                           location;       ///< The location of the object
          boost::uint32_t                     checksum;       ///< The digest for the completed object
          std::time_t                         timestamp;      ///< The last time the file was modified just before the last checksum was computed

        public:

          /** \brief Constructor */
          object_descriptor(boost::weak_ptr < processor > const& g, boost::weak_ptr < project_manager> m,
                tipi::mime_type const& f, tipi::uri const& u, processor::object_descriptor::status_type t = processor::object_descriptor::original) :
                            processor::object_descriptor(t), generator(g), project(m), mime_type(f), location(u), timestamp(0) {
          }

          /** \brief Assignment */
          void operator=(object_descriptor const&);

          /** \brief Returns the storage format as MIME type */
          tipi::mime_type get_format() const;

          /** \brief Returns the location as URI */
          boost::filesystem::path get_location() const;

          /** \brief Returns the last recorded MD5 checksum of the file */
          boost::uint32_t get_checksum() const;

          /** \brief Returns the last recorded timestamp of the file */
          std::time_t get_timestamp() const;

          /** \brief Returns the processor that generated this object */
          boost::shared_ptr< processor > get_generator() const;

          /** \brief Verifies whether or not the object is present in the store and updates status accordingly */
          bool present_in_store();

          /** \brief Verifies whether or not the object is physically available and not changed */
          bool self_check();

          /** \brief Verifies whether or not the object is physically available and not changed */
          bool self_check(const long int);
      };

    private:

      /** \brief Weak pointer to this object for passing */
      boost::weak_ptr < processor >                         interface_object;

      /** \brief Identifies the tool that is required to run the command */
      boost::shared_ptr < const tool >                      tool_descriptor;

      /** \brief The selected input combination of the tool */
      boost::shared_ptr< const tool::input_configuration >  selected_input_configuration;

      /** \brief The information about inputs of this processor */
      input_list                                            inputs;

      /** \brief The information about outputs of this processor */
      output_list                                           outputs;

      /** \brief The current task that is running or about to run */
      boost::shared_ptr< monitor >                          current_monitor;

      /** \brief The associated project manager */
      boost::weak_ptr< project_manager >                    manager;

      /** \brief The directory from which tools should be run on behalf of this object */
      std::string                                           output_directory;

    private:

      /** \brief Helper function for adjusting status */
      static bool try_change_status(processor::object_descriptor&, processor::object_descriptor::status_type);

      /** \brief Update if object is up-to-date */
      void update_on_success(boost::shared_ptr < object_descriptor >, interface_ptr const&, boost::shared_ptr < tipi::configuration >, bool);

      /** \brief synchronises the configuration with outputs */
      void update_configuration(tipi::configuration& c) const;

      /** \brief Handler that is executed when an edit operation is completed */
      void edit_completed();

      /** \brief Tries to convert a path to a path that is relative to the project store */
      boost::filesystem::path try_convert_to_store_relative(boost::filesystem::path const&) const;

    private:

      /** \brief Basic constructor */
      inline processor_impl(boost::shared_ptr < processor > const&, boost::weak_ptr < project_manager >);

      /** \brief Constructor with tool selection */
      inline processor_impl(boost::shared_ptr < processor > const&, boost::weak_ptr < project_manager >,
                                 boost::shared_ptr < const tool >, boost::shared_ptr < const tool::input_configuration >);

      /** \brief Execute an edit command on one of the outputs */
      void edit(execution::command*);

      /** \brief Returns the prefix for output files */
      std::string get_output_prefix(std::string const&) const;

      /** \brief Extracts useful information from a configuration object */
      void process_configuration(boost::shared_ptr < tipi::configuration >&, std::set < tipi::configuration::object const* >&, bool = true);

      /** \brief Find an object descriptor for a given pointer to an object */
      boost::shared_ptr < processor::object_descriptor > find_output_by_id(tipi::configuration::parameter::identifier const&);

      /** \brief Find an object descriptor for a given pointer to an object */
      boost::shared_ptr < processor::object_descriptor > find_input_by_id(tipi::configuration::parameter::identifier const&);

      /** \brief Get the most original (main) input */
      boost::shared_ptr < object_descriptor > find_primary_input();

      /** \brief Change format for an object */
      void change_format(object_descriptor const& o, build_system::storage_format const& n);

      /** \brief Relocate an input object */
      void relocate_input(object_descriptor const&, std::string const&);

      /** \brief Relocates an output object */
      void relocate_output(object_descriptor const&, std::string const&);

      /** \brief Find an object descriptor for a given name and rename if it exists */
      void relocate_object(processor::configurated_object_descriptor&, std::string const&);

      /** \brief Prepends the absolute path to the project store */
      std::string make_output_path(std::string const&) const;

      /** \brief Check the inputs with respect to the outputs and adjust status accordingly */
      bool check_status(bool);

      /** \brief Sets the status of the inputs to out-of-date if the processor is inactive */
      bool demote_status();

      /** \brief Start tool configuration */
      void configure(interface_ptr const&, boost::shared_ptr< const tool::input_configuration >, const boost::filesystem::path&, std::string const& = "");

      /** \brief Start tool configuration */
      void configure(interface_ptr const&, boost::shared_ptr < tipi::configuration > const&, std::string const& = "");

      /** \brief Start tool reconfiguration */
      void reconfigure(interface_ptr const&, boost::shared_ptr < tipi::configuration > const&, std::string const& = "");

      /** \brief Start processing: generate outputs from inputs */
      void run(interface_ptr const&, boost::shared_ptr < tipi::configuration > c, bool b = false);

      /** \brief Start running and afterward execute a function */
      void run(interface_ptr const&, boost::function < void () > h, boost::shared_ptr < tipi::configuration > c, bool b = false);

      /** \brief Start processing if not all outputs are up to date */
      void update(interface_ptr const&, boost::shared_ptr < tipi::configuration > c, bool b = false);

      /** \brief Start updating and afterward execute a function */
      void update(interface_ptr const&, boost::function < void () > h, boost::shared_ptr < tipi::configuration > c, bool b = false);

      /** \brief Add (or modify) an output object */
      void register_input(tipi::configuration::parameter::identifier const&, boost::shared_ptr < object_descriptor > const&);

      /** \brief Add (or modify) an output object */
      void register_output(tipi::configuration::parameter::identifier const&, boost::shared_ptr < object_descriptor >&);

      /** \brief Add an output object */
      void register_output(tipi::configuration::parameter::identifier const&, tipi::configuration::object const&,
                object_descriptor::status_type const& = object_descriptor::reproducible_nonexistent);

      /** \brief Removes the outputs of this processor from storage */
      void flush_outputs();

      /** \brief Whether or not a process is running on behalf of this processor */
      bool is_active() const;

      /** \brief Terminates running processes and deactivates monitor */
      void shutdown();

    public:

      /** \brief Destructor */
      ~processor_impl();
  };

  /**
   * \param[in] t shared pointer to the interface object
   * \param[in] h the function to execute when the process terminates
   * \param[in] c the configuration object to use
   * \param[in] b whether or not to run when there are no input objects defined
   *
   * \pre t.get() == this
   **/
  inline void processor_impl::update(interface_ptr const& t, boost::function < void () > h, boost::shared_ptr < tipi::configuration > c, bool b) {
    current_monitor->once_on_completion(h);

    update(t, c, b);
  }

  /**
   * \param[in] t shared pointer to the interface object
   * \param[in] h the function to execute when the process terminates
   * \param[in] c the configuration object to use
   * \param[in] b whether or not to run when there are no input objects defined
   *
   * \pre t.get() == this
   **/
  inline void processor_impl::run(interface_ptr const& t, boost::function < void () > h, boost::shared_ptr < tipi::configuration > c, bool b) {
    current_monitor->once_on_completion(h);

    run(t, c, b);
  }

  inline processor_impl::processor_impl(boost::shared_ptr < processor > const& tp, boost::weak_ptr < project_manager > p) :
                interface_object(tp), current_monitor(new monitor(tp)), manager(p) {

    if(!(manager.lock().get())) {
      std::runtime_error("Cannot get lock for manager.\n");
    };
  }

  /**
   * \param[in] tp shared pointer to the interface object
   * \param[in] p the associated project manager
   * \param[in] t the tool descriptor of the tool that is to be used to produce the output from the input
   * \param[in] t the selected input configuration
   **/
  inline processor_impl::processor_impl(boost::shared_ptr < processor > const& tp,
        boost::weak_ptr < project_manager > p, boost::shared_ptr < const tool > t, boost::shared_ptr < const tool::input_configuration > c) :
               interface_object(tp), tool_descriptor(t), selected_input_configuration(c), current_monitor(new monitor(tp)), manager(p) {

    if(!(manager.lock().get())) {
      std::runtime_error("Cannot get lock for manager.\n");
    }
    if(!(tool_descriptor.get())) {
      std::runtime_error("Cannot get tool descriptor.\n");
    }
    if(!(selected_input_configuration.get())) {
      std::runtime_error("Cannot get selected input configuration.\n");
    }
  }

  /**
   * \param[in] name the name of a file on which the prefix will be based
   **/
  inline std::string processor_impl::get_output_prefix(std::string const& name) const {
    std::string basename(boost::filesystem::basename(name));

    if (4 < basename.size()) {
      std::string suffix(basename.substr(basename.size() - 4));

      if (suffix[0] == '-') {
        for (size_t i = 1; i < 4; ++i) {
          if (suffix[i] < '0' || '9' < suffix[i]) {
            return basename;
          }
        }

        return basename.substr(0, basename.size() - 4);
      }
    }

    return basename;
  }

  inline bool processor_impl::is_active() const {
    return current_monitor->get_status() == execution::process::running;
  }

  inline processor_impl::~processor_impl() {
    shutdown();
  }

  inline void processor_impl::shutdown() {
    current_monitor->shutdown();
    current_monitor->reset_handlers();
    current_monitor->finish();
  }

  /**
   * \param[in] t shared pointer to the interface object
   * \param[in] w the path to the directory in which to run the tool
   * \param[in] c the configuration object to use
   *
   * \pre The existing configuration must contain the input object matching the selected input combination
   * \pre t.get() == this
   *
   * \attention This function is non-blocking
   **/
  inline void processor_impl::reconfigure(interface_ptr const& t, boost::shared_ptr < tipi::configuration > const& c, std::string const& w) {
    if(!(selected_input_configuration.get())) {
      std::runtime_error("Cannot get selected input configuration.\n");
    };

    c->fresh(true);

    configure(t, c, w);
  }

  /**
   * \param[in] c the configuration object to use
   **/
  inline void processor_impl::update_configuration(tipi::configuration& c) const {
    BOOST_FOREACH(processor::configurated_object_descriptor const& i, inputs) {
      if (c.input_exists(i.identifier)) {
        c.get_input(i.identifier).location(
                boost::static_pointer_cast< object_descriptor > (i.object)->location);
      }
    }
  }

  /**
   * \param[in] o object that is checked to be up-to-date
   * \param[in] t shared pointer to the interface object
   * \param[in] c the configuration object to use
   * \param[in] b whether or not to run when there are no input objects are specified
   **/
  inline void processor_impl::update_on_success(boost::shared_ptr < object_descriptor > o, interface_ptr const& t, boost::shared_ptr < tipi::configuration > c, bool b) {
    if (o->is_up_to_date()) {
      update(t, c, b);
    }
  }
  /// \endcond
}

#endif
