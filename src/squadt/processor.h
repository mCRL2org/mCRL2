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

#include <md5pp/md5pp.h>
#include <xml2pp/text_reader.h>

#include "executor.h"
#include "tool_manager.h"
#include "tool.h"
#include "task.h"
#include "core.h"

namespace squadt {

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
  class processor {
    friend class project_manager;
    friend class tool_manager;
    friend class executor;
 
    public:

      /** \brief Type that is used to keep the status of the output of a processor */
      enum output_status {
        non_existent,         /* outputs do not exist */
        being_computed,       /* outputs are being generated */
        not_up_to_date,       /* outputs exist but are not up to date */
        up_to_date            /* outputs exist and are up-to-date */
      };

      /** \brief Type to hold information about output objects */
      struct object_descriptor {
        processor*            generator;      ///< The process responsible for generating this object
        std::string           storage_format; ///< The used storage format
        std::string           location;       ///< The location of the object
        md5pp::compact_digest checksum;       ///< The digest for the completed object
        std::time_t           timestamp;      ///< The last time the file was modified just before the last checksum was computed

        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::shared_ptr < object_descriptor >  sptr;
       
        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::weak_ptr < object_descriptor >    wptr;
      };

      /**
       * \brief Basic reporter for task progress
       *
       * The process(es) that are spawned for this task are monitored via the
       * process_listener interface.
       **/
      class reporter : public execution::task {
          friend class processor;

        public:

          /** \brief Type for functions that is used to communicate state changes */
          typedef boost::function < void (processor::output_status) > callback_handler;

          /** \brief Convenience type for hiding shared pointer implementation */
          typedef boost::shared_ptr < reporter >                      ptr;

        private:
  
          /** \brief The processor that owns this object */
          processor& owner;

        private:
  
          /** \brief Visualisation function for state changes */
          callback_handler visualise;
  
        private:
  
          /** \brief Handler function that is called when an associated process changes state */
          inline void report_change(execution::process::status);
  
        public:
  
          /** \brief Constructor with a callback handler for actual visualisation */
          inline reporter(processor&, callback_handler);

          /** \brief The default visualisation function that does nothing */
          static void dummy(output_status);
      };
 
      /** \brief Convenience type for hiding shared pointer implementation */
      typedef boost::shared_ptr < processor >                     ptr;

      /** \brief Convenience type for hiding the implementation of a list with input information */
      typedef std::vector < object_descriptor::wptr >             input_list;

      /** \brief Convenience type for hiding the implementation of a list with output information */
      typedef std::vector < object_descriptor::sptr >             output_list;

      /** \brief Helper type for read() members */
      typedef std::map < unsigned long, object_descriptor::sptr > id_conversion_map;

    private:
 
      /** \brief Identifies the tool that is required to run the command */
      tool&                               program;

      /** \brief The information about inputs of this processor */
      input_list                          inputs;

      /** \brief The information about outputs of this processor */
      output_list                         outputs;
 
      /** \brief The current status of the outputs of the processor */
      output_status                       current_output_status;

      /** \brief The current task that is running or about to run */
      reporter::ptr                       task_monitor;
 
    private:

      /** \brief Manually sets the output status */
      inline void set_output_status(const processor::output_status);

    public:
 
      /** \brief Basic constructor */
      inline processor(tool& t);

      /** \brief Constructor with visualisation handler */
      inline processor(tool& t, reporter::callback_handler);

      /** \brief Check the inputs with respect to the outputs and adjust status accordingly */
      bool check_status(bool);

      /** \brief Validate whether the inputs to this process are not dangling pointers */
      inline bool consistent_inputs() const;

      /** \brief Start processing: generate outputs from inputs */
      void process();
 
      /** \brief Get the object for the tool associated with this processor */
      inline const tool& get_tool();

      /** \brief Read from XML using a libXML2 reader */
      static processor::ptr read(id_conversion_map&, xml2pp::text_reader& reader) throw ();
 
      /** \brief Write as XML to stream */
      void write(std::ostream& stream = std::cout) const;

      /** \brief Get input objects */
      inline const input_list& get_inputs() const;
 
      /** \brief Get output objects */
      inline const output_list& get_outputs() const;
 
      /** \brief Pretty prints the fields of the specification */
      void print(std::ostream& stream = std::cerr) const;
 
      /** \brief The number of input objects of this processor */
      inline const unsigned int number_of_inputs() const;
 
      /** \brief The number output objects of this processor */
      inline const unsigned int number_of_outputs() const;
 
      /** \brief Checks or rechecks the state of the outputs with respect to the inputs */
      inline void check_output_status();

      /** \brief Removes the outputs of this processor from storage */
      void flush_outputs();
 
      /** Recursively set the status of specification and */
      inline const output_status get_output_status() const;

      /** \brief Destructor */
      inline ~processor();
  };
}

#endif

