#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <algorithm>
#include <string>
#include <vector>
#include <iosfwd>
#include <ctime>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <md5pp/md5pp.h>
#include <xml2pp/text_reader.h>

#include <utility/indirect_iterator.h>
#include "executor.h"
#include "tool_manager.h"
#include "tool.h"
#include "task_monitor.h"
#include "core.h"

namespace squadt {

  using iterator_wrapper::constant_indirect_iterator;

  class project_manager;

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
  class processor : boost::noncopyable {
    friend class project_manager;
    friend class tool_manager;
    friend class executor;
 
    public:

      /** \brief Convenience type for hiding shared pointer implementation */
      typedef boost::shared_ptr < processor >                               ptr;

      /** \brief Convenience type for hiding shared pointer implementation */
      typedef boost::shared_ptr < processor >                               sptr;

      /** \brief Convenience type for hiding shared pointer implementation */
      typedef boost::weak_ptr < processor >                                 wptr;

      /** \brief Type that is used to keep the status of the output of a processor */
      enum output_status {
        non_existent,         /* outputs do not exist */
        being_computed,       /* outputs are being generated */
        not_up_to_date,       /* outputs exist but are not up to date */
        up_to_date            /* outputs exist and are up-to-date */
      };

      /** \brief Type to hold information about output objects */
      struct object_descriptor {
        processor::wptr         generator;      ///< The process responsible for generating this object
        storage_format          format;         ///< The used storage format
        std::string             location;       ///< The location of the object
        unsigned int            identifier;     ///< The identifier of the associated output object in a configuration
        md5pp::compact_digest   checksum;       ///< The digest for the completed object
        std::time_t             timestamp;      ///< The last time the file was modified just before the last checksum was computed

        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::shared_ptr < object_descriptor >  sptr;
       
        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::weak_ptr < object_descriptor >    wptr;
      };

      /**
       * \brief Basic monitor for task progress
       *
       * The process(es) that are spawned for this task are monitored via the
       * task_monitor interface.
       **/
      class monitor : public execution::task_monitor {
        friend class processor;

        public:

          /** \brief Type for functions that is used to handle incoming process state changes */
          typedef boost::function < void (processor::output_status) >                            status_callback_function;

          /** \brief Type for functions that is used to handle incoming layout state changes */
          typedef boost::function < void (sip::layout::tool_display::sptr) >                     display_layout_callback_function;

          /** \brief Type for functions that is used to handle incoming (G)UI state changes */
          typedef boost::function < void (sip::layout::tool_display::constant_elements const&) > display_data_callback_function;

          /** \brief Type for functions that is used to handle incoming layout state changes */
          typedef boost::function < void (sip::report::sptr) >                                   status_message_callback_function;

          /** \brief Convenience type for hiding shared pointer implementation */
          typedef boost::shared_ptr < monitor >                                                  sptr;

        public:

          /** \brief The processor that owns this object */
          processor& owner;

        private:
  
          /** \brief Actualisation function for layout changes */
          display_layout_callback_function on_layout_change;
  
          /** \brief Actualisation function for user interface state changes */
          display_data_callback_function   on_state_change;
  
          /** \brief Actualisation function for incoming change messages */
          status_message_callback_function on_message_change;

          /** \brief Actualisation function for process state changes */
          status_callback_function         on_status_change;
  
        private:
  
          /** \brief Handler function that is called when an associated process changes state */
          void report_change(execution::process::status);
  
          /** \brief The default callback function that does nothing */
          void status_change_dummy(output_status);

          /** \brief The default callback function that does nothing */
          void display_layout_change_dummy(sip::layout::tool_display::sptr);

          /** \brief The default callback function that does nothing */
          void display_data_change_dummy(sip::layout::tool_display::constant_elements const&);

          /** \brief The default callback function that does nothing */
          void status_message_change_dummy(sip::report::sptr);

          /** \brief Helper function for communication with a tool, starts a new thread with pilot() */
          inline void start_pilot(bool = true);

          /** \brief Helper function for communication with a tool */
          void pilot(bool = true);

        public:
  
          /** \brief Constructor with a callback handler */
          monitor(processor&);

          /** \brief Constructor with a callback handler */
          inline void set_status_handler(status_callback_function);

          /** \brief Constructor with a callback handler */
          inline void set_display_layout_handler(display_layout_callback_function);

          /** \brief Constructor with a callback handler */
          inline void set_display_data_handler(sip::layout::tool_display::sptr, display_data_callback_function);

          /** \brief Constructor with a callback handler */
          inline void set_status_message_handler(status_message_callback_function);
      };

      /** \brief Convenience type for hiding the implementation of a list with input information */
      typedef std::vector < object_descriptor::wptr >                       input_list;

      /** \brief Convenience type for hiding the implementation of a list with output information */
      typedef std::vector < object_descriptor::sptr >                       output_list;

      /** \brief Helper type for read() members */
      typedef std::map < unsigned long, object_descriptor::sptr >           id_conversion_map;

      /** \brief Type for iterating the input objects */
      typedef constant_indirect_iterator < input_list, object_descriptor >  input_object_iterator;

      /** \brief Type for iterating the output objects */
      typedef constant_indirect_iterator < output_list, object_descriptor > output_object_iterator;

    private:

      /** \brief Weak pointer to this object for passing */
      wptr                           this_object;
 
      /** \brief Identifies the tool that is required to run the command */
      tool::sptr                     tool_descriptor;

      /** \brief The information about inputs of this processor */
      input_list                     inputs;

      /** \brief The information about outputs of this processor */
      output_list                    outputs;
 
      /** \brief The current status of the outputs of the processor */
      output_status                  current_output_status;

      /** \brief The current task that is running or about to run */
      monitor::sptr                  current_monitor;

      /** \brief The associated project manager */
      project_manager*               manager;
 
      /** \brief The selected input combination of the tool */
      tool::input_combination const* selected_input_combination;

      /** \brief The directory from which tools should be run on behalf of this object */
      std::string                    output_directory;

    private:

      /** \brief Basic constructor */
      inline processor(project_manager&);

      /** \brief Constructor with callback handler */
      inline processor(project_manager&, tool::sptr);

      /** \brief Manually sets the output status */
      inline void set_output_status(const processor::output_status);

      /** \brief Extracts useful information from a configuration object */
      inline void process_configuration();

      /** \brief Extracts useful information from a configuration object */
      void process_configuration(sip::configuration::sptr const& c);

      /** \brief Find an object descriptor for a given pointer to an object */
      const object_descriptor::sptr find_output(std::string const&) const;
 
      /** \brief Find an object descriptor for a given pointer to an object */
      const object_descriptor::sptr find_input(std::string const&) const;
 
      /** \brief Find an object descriptor for a given name and rename if it exists */
      void rename_object(object_descriptor::sptr const&, std::string const&);

      /** \brief Get the most original (main) input */
      const object_descriptor::sptr find_initial_object() const;
 
    public:

      /** \brief Factory method for creating instances of this object */
      static processor::sptr create(project_manager&);
 
      /** \brief Factory method for creating instances of this object */
      static processor::sptr create(project_manager&, tool::sptr);
 
      /** \brief Check the inputs with respect to the outputs and adjust status accordingly */
      bool check_status(bool);

      /** \brief Validate whether the inputs to this process are not dangling pointers */
      bool check_input_consistency() const;

      /** \brief Checks or rechecks the state of the outputs with respect to the inputs */
      bool check_output_consistency(bool = false);

      /** \brief Start tool configuration, with callback on completion */
      void configure(const tool::input_combination*, std::string const&, const boost::filesystem::path&, boost::function < void () >);

      /** \brief Start tool configuration */
      void configure(const tool::input_combination*, std::string const&, const boost::filesystem::path&);
 
      /** \brief Start tool configuration */
      void configure(std::string const&);
 
      /** \brief Start tool (re)configuration */
      void reconfigure(std::string const&, boost::function < void () >);
 
      /** \brief Start tool reconfiguration */
      void reconfigure(std::string const&);
 
      /** \brief Start processing: generate outputs from inputs */
      void process(boost::function < void () >);
 
      /** \brief Start processing: generate outputs from inputs */
      void process();
 
      /** \brief Get the object for the tool associated with this processor */
      inline void set_tool(tool::sptr&);

      /** \brief Sets the object for the tool associated with this processor */
      inline void set_tool(tool::sptr);

      /** \brief Get the object for the tool associated with this processor */
      inline const tool::sptr get_tool();

      /** \brief Get the input combination if one is already selected */
      inline void set_input_combination(tool::input_combination*);

      /** \brief Get the input combination if one is already selected */
      inline tool::input_combination const* get_input_combination() const;

      /** \brief Get the object for the tool associated with this processor */
      inline const monitor::sptr get_monitor();

      /** \brief Read from XML using a libXML2 reader */
      static processor::sptr read(project_manager&, id_conversion_map&, xml2pp::text_reader&) throw ();

      /** \brief Write as XML to stream */
      void write(std::ostream& stream = std::cout) const;

      /** \brief Get input objects */
      inline input_object_iterator get_input_iterator() const;
 
      /** \brief Add an input object */
      inline void append_input(object_descriptor::wptr&);

      /** \brief Add an input object */
      inline void append_input(object_descriptor::wptr);
 
      /** \brief Find an object descriptor for a given pointer to an object */
      const object_descriptor::sptr find_output(object_descriptor*) const;
 
      /** \brief Find an object descriptor for a given pointer to an object */
      const object_descriptor::sptr find_input(object_descriptor*) const;
 
      /** \brief Find an object descriptor for a given pointer to an object (by id) */
      const object_descriptor::sptr find_output(const unsigned int) const;
 
      /** \brief Find an object descriptor for a given pointer to an object (by id) */
      const object_descriptor::sptr find_input(const unsigned int) const;
 
      /** \brief Find an object descriptor for a given name and rename if it exists */
      void rename_output(std::string const&, std::string const&);
 
      /** \brief Find an object descriptor for a given name and rename if it exists */
      void rename_input(std::string const&, std::string const&);
 
      /** \brief Get output objects */
      inline output_object_iterator get_output_iterator() const;
 
      /** \brief Add an output object */
      inline void append_output(object_descriptor::sptr&);

      /** \brief Add an output object */
      inline void append_output(const storage_format&, const std::string&);
 
      /** \brief Add an output object */
      inline void append_output(sip::object const&);

      /** \brief Add an output object */
      inline void replace_output(object_descriptor::sptr, sip::object const&);

      /** \brief Pretty prints the fields of the specification */
      void print(std::ostream& stream = std::cerr) const;
 
      /** \brief The number of input objects of this processor */
      inline const size_t number_of_inputs() const;
 
      /** \brief The number output objects of this processor */
      inline const size_t number_of_outputs() const;
 
      /** \brief Removes the outputs of this processor from storage */
      void flush_outputs();
 
      /** \brief Destructor */
      inline ~processor();
  };
}

#endif

