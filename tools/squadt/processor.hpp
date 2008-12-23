// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file processor.h
/// \brief Add your file description here.

#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/iterator_range.hpp>

#include "task_monitor.hpp"
#include "tool.hpp"

namespace squadt {

  class project_manager;

  class processor_impl;

  /**
   * \brief A processor represents a tool configuration.
   *
   * The processor is a container to distinguish configurations that might or
   * might not actually be the same. The most notable part of such a
   * configuration are the inputs and outputs. The inputs are important since
   * the controller must ensure that input exists before running a tool with
   * this configuration.  The outputs are important because they can be the
   * input of other processors.
   *
   **/
  class processor : public utility::visitable, private boost::noncopyable {
    friend class processor_impl;
    friend class project_manager_impl;
    friend class tool_manager;
    friend class executor;

    template < typename R, typename S >
    friend class utility::visitor;

    public:

      class monitor;

      /** \brief Type to hold information about output objects */
      class object_descriptor {
        friend class processor_impl;
        friend class monitor;

        public:

          /** \brief Type that is used to keep the status of the output of a processor */
          enum status_type {
            original,                 /* unique not reproducible */
            reproducible_nonexistent, /* can be reproduced but does not exist */
            reproducible_out_of_date, /* can be reproduced, exists and is out of date */
            reproducible_up_to_date,  /* can be reproduced, exists, and is up to date */
            generation_in_progress    /* outputs are being generated */
          };

        protected:

          status_type status; ///< The status of this object

        protected:

          /** \brief Constructor */
          object_descriptor(status_type s) : status(s) {
          }

        public:

          /** \brief Returns the storage format as MIME type */
          virtual status_type get_status() const;

          /** \brief Returns the storage format as MIME type */
          virtual tipi::mime_type get_format() const = 0;

          /** \brief Returns the location as URI */
          virtual boost::filesystem::path get_location() const = 0;

          /** \brief Returns the processor that generated this object */
          virtual boost::shared_ptr< processor > get_generator() const = 0;

          /** \brief Verifies and reports whether or not the object is up to date */
          bool is_up_to_date();

          /** \brief Checks and updates status */
          virtual bool self_check() = 0;

          /** \brief Destructor */
          virtual ~object_descriptor();
      };

      /** \brief Type that is used to associate a configuration object identifier to an object descriptor */
      struct configurated_object_descriptor {
        tipi::configuration::parameter::identifier identifier; ///< The identifier of the associated output object in a configuration
        boost::shared_ptr < object_descriptor >    object;

        inline configurated_object_descriptor() {
        }
      };

      /** \brief Convenience type for hiding the implementation of a list with input information */
      typedef std::vector < configurated_object_descriptor >                                      input_list;

      /** \brief Convenience type for hiding the implementation of a list with output information */
      typedef std::vector < configurated_object_descriptor >                                      output_list;

      /** \brief Type of a functor to project {input,output}list elements to object_descriptors  */
      typedef boost::function<
                boost::shared_ptr < object_descriptor > (configurated_object_descriptor const&) > projection_functor_type;

      /** \brief Type for iterating the input object descriptors */
      typedef boost::transform_iterator < projection_functor_type, input_list::const_iterator >   input_object_iterator;

      /** \brief Type for iterating the output object descriptors */
      typedef boost::transform_iterator < projection_functor_type, output_list::const_iterator >  output_object_iterator;

    private:

      /** \brief Private implementation */
      boost::shared_ptr < processor_impl > impl;

    private:

      /** \brief Basic constructor */
      inline processor() { }

    public:

      /** \brief Factory method for creating instances of this object */
      static boost::shared_ptr < processor > create(boost::weak_ptr < project_manager > const&);

      /** \brief Factory method for creating instances of this object */
      static boost::shared_ptr < processor > create(boost::weak_ptr < project_manager > const&,
                        boost::shared_ptr < const tool >, boost::shared_ptr < const tool::input_configuration >);

      /** \brief Check the inputs with respect to the outputs and adjust status accordingly */
      bool check_status(bool);

      /** \brief Execute an edit command on one of the outputs */
      void edit(execution::command*);

      /** \brief Sets the status of the outputs to out-of-date if the processor is inactive */
      bool demote_status();

      /** \brief Start tool configuration */
      void configure(boost::shared_ptr< const tool::input_configuration >, const boost::filesystem::path&, std::string const& = "");

      /** \brief Start tool configuration */
      void configure(std::string const& = "");

      /** \brief Start tool reconfiguration */
      void reconfigure(std::string const& = "");

      /** \brief Start processing: generate outputs from inputs */
      void run(bool b = false);

      /** \brief Start running and afterward execute a function */
      void run(boost::function < void () > h, bool b = false);

      /** \brief Start processing if not all outputs are up to date */
      void update(bool b = false);

      /** \brief Start updating and afterward execute a function */
      void update(boost::function < void () > h, bool b = false);

      /** \brief Get the object for the tool associated with this processor */
      void set_tool(boost::shared_ptr < tool > const&);

      /** \brief Get the object for the tool associated with this processor */
      boost::shared_ptr < const tool > get_tool() const;

      /** \brief Get the input combination if one is already selected */
      void set_input_configuration(boost::shared_ptr< const tool::input_configuration >);

      /** \brief Whether or not an input combination has been set */
      bool has_input_configuration();

      /** \brief Get the input combination if one is already selected */
      boost::shared_ptr < const tool::input_configuration > get_input_configuration() const;

      /** \brief Get the object for the tool associated with this processor */
      boost::shared_ptr < monitor > get_monitor();

      /** \brief Whether or not a tool is running for this object */
      bool is_active() const;

      /** \brief Get input objects */
      boost::iterator_range < input_object_iterator > get_input_iterators() const;

      /** \brief Get output objects */
      boost::iterator_range < output_object_iterator > get_output_iterators() const;

      /** \brief Add (or modify) an input object */
      void register_input(tipi::configuration::parameter::identifier const&,
                        boost::shared_ptr < object_descriptor > const&);

      /** \brief Find an object descriptor for a given pointer to an object (by id) */
      const boost::shared_ptr < object_descriptor > find_input(
                        tipi::configuration::parameter::identifier const&) const;

      /** \brief Find an object descriptor for a given pointer to an object (by id) */
      const boost::shared_ptr < object_descriptor > find_output(
                        tipi::configuration::parameter::identifier const&) const;

      /** \brief Find an object descriptor for a given name and rename if it exists */
      void relocate_output(object_descriptor&, std::string const&);

      /** \brief Find an object descriptor for a given name and rename if it exists */
      void relocate_input(object_descriptor&, std::string const&);

      /** \brief Find an object descriptor for a given name and change format if it exists */
      void change_format(object_descriptor&, build_system::storage_format const&);

      /** \brief Add an output object */
      void register_output(tipi::configuration::parameter::identifier const&,
                const build_system::storage_format&, const std::string&,
                object_descriptor::status_type const& = object_descriptor::reproducible_nonexistent);

      /** \brief Add an output object */
      void replace_output(tipi::configuration::parameter::identifier const&,
                boost::shared_ptr < object_descriptor >, tipi::configuration::object const&,
                object_descriptor::status_type const& = object_descriptor::reproducible_up_to_date);

      /** \brief The number of input objects of this processor */
      size_t number_of_inputs() const;

      /** \brief The number output objects of this processor */
      size_t number_of_outputs() const;

      /** \brief Removes the outputs of this processor from storage */
      void flush_outputs();

      /** \brief Stops running processes and deactivates monitor */
      void shutdown();
  };

  std::istream& operator >> (std::istream&, processor::object_descriptor::status_type&);

  /**
   * \brief Basic monitor for task progress
   *
   * The process(es) that are spawned for this task are monitored via the
   * task_monitor interface.
   **/
  class processor::monitor : public execution::task_monitor {
    friend class processor_impl;

    public:

      /** \brief Type for functions that is used to handle incoming process state changes */
      typedef boost::function < void () >                                                     status_callback_function;

      /** \brief Type for functions that is used to handle incoming layout state changes */
      typedef boost::function < void (boost::shared_ptr < tipi::tool_display >) >             display_layout_callback_function;

      /** \brief Type for functions that is used to handle incoming (G)UI state changes */
      typedef boost::function < void (std::vector< tipi::layout::element const* > const&) >   display_update_callback_function;

      /** \brief Type for functions that is used to handle incoming layout state changes */
      typedef boost::function < void (boost::shared_ptr < tipi::report >) >                   status_message_callback_function;

    public:

      /** \brief The processor that owns this object */
      boost::weak_ptr < processor > owner;

    private:

      /** \brief Actualisation function for process state changes */
      status_callback_function         status_change_handler;

    private:

      /** \brief Handler function that is called when an associated process changes state */
      void signal_change(const execution::process::status);

      /** \brief Handler function that is called when an associated process changes state */
      void signal_change(boost::shared_ptr < execution::process >, const execution::process::status);

      /** \brief Actual tool configuration operation */
      void tool_configuration(boost::shared_ptr < processor >, boost::shared_ptr < tipi::configuration >);

      /** \brief Actual tool execution with a configuration */
      void tool_operation(boost::shared_ptr < processor >, boost::shared_ptr < tipi::configuration > const&);

    public:

      /** \brief Constructor with a callback handler */
      monitor(boost::shared_ptr < processor >);

      /** \brief Set the callback handler for status changes */
      void set_status_handler(status_callback_function);

      /** \brief Set the callback handler for display layout changes */
      void set_display_layout_handling(display_layout_callback_function const&, display_update_callback_function const&);

      /** \brief Set the callback handler for incoming status messages */
      void set_status_message_handler(status_message_callback_function);

      /** \brief Set the callback handler for display layout changes to default */
      void reset_display_layout_handling();

      /** \brief Set the callback handler for incoming status messages */
      void reset_status_message_handler();

      /** \brief Set all callback handlers to default */
      void reset_handlers();

      /** \brief Gets the status of the associated process, if there is one */
      execution::process::status get_status();
  };
}

#endif

