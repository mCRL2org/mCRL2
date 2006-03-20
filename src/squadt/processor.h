#ifndef SPECIFICATION_H
#define SPECIFICATION_H

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
#include <sip/detail/configuration.h>

#include "tool.h"
#include "executor.h"

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
  class processor : public execution::process_change_listener {
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
        processor*          generator;      ///< The process responsible for generating this object
        std::string         storage_format; ///< The used storage format
        std::string         location;       ///< The location of the object
        md5::compact_digest checksum;       ///< The digest for the completed object
        std::time_t         timestamp;      ///< A timestamp of a time just before the last checksum

        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::shared_ptr < object_descriptor >  sptr;
       
        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::weak_ptr < object_descriptor >    wptr;
      };

      /** \brief Type for functions that can be used for visualising a state change */
      typedef boost::function < void (processor::output_status) > visualisation_handler;

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

      /** \brief A tool configuration that can be send to configure a tool */
      sip::configuration::ptr             configuration;
 
      /** \brief The information about inputs of this processor */
      input_list                          inputs;

      /** \brief The information about outputs of this processor */
      output_list                         outputs;
 
      /** \brief the current status of the outputs of the processor */
      output_status                       current_output_status;
 
      /** \brief Visualisation function for state changes */
      visualisation_handler               visualise;
 
    private:

      /** \brief The default visualisation function that does nothing */
      static void dummy_visualiser(output_status);

      /** \brief Manually sets the output status */
      inline void set_output_status(const processor::output_status);

      /** \brief Handler function that is called when an associated process changes state */
      inline void report_change(execution::process::status);

    public:
 
      /** \brief Constructor without visualisation handler */
      inline processor(tool& t);

      /** \brief Constructor with visualisation handler */
      inline processor(tool& t, visualisation_handler);

      /** \brief Check the inputs with respect to the outputs and adjust status accordingly */
      inline bool check_status();

      /** \brief Validate whether the inputs to this process are not dangling pointers */
      inline bool consistent_inputs() const;

      /** \brief Start processing: generate outputs from inputs */
      void process() throw ();
 
      /** \brief Get a pointer to the configuration object that was last received by from the tool */
      inline sip::configuration::ptr get_configuration();

      /** \brief Get the object for the tool associated with this processor */
      inline const tool& get_tool();

      /** \brief Read from XML using a libXML2 reader */
      static processor::ptr read(id_conversion_map&, xml2pp::text_reader& reader) throw ();
 
      /** \brief Write as XML to stream */
      void write(std::ostream& stream = std::cout) const;

      /** \brief Get input objects */
      inline input_list& get_inputs();
 
      /** \brief Get output objects */
      inline output_list& get_outputs();
 
      /** \brief Pretty prints the fields of the specification */
      void print(std::ostream& stream = std::cerr) const;
 
      /** \brief The number of input objects of this processor */
      inline const unsigned int number_of_inputs() const;
 
      /** \brief The number output objects of this processor */
      inline const unsigned int number_of_outputs() const;
 
      /** \brief Checks or rechecks the state of the outputs with respect to the inputs */
      inline void check_output_status();

      /** \brief Removes the outputs of this processor from storage */
      inline void flush_outputs();
 
      /** Recursively set the status of specification and */
      inline const output_status get_output_status() const;

      /** \brief Destructor */
      inline ~processor();
  };

  /**
   * \brief Operator for writing to stream
   *
   * @param s stream to write to
   * @param p the processor to write out
   **/
  inline std::ostream& operator << (std::ostream& s, const processor& p) {
    p.write(s);

    return (s);
  }

  inline processor::processor(tool& t) : program(t), visualise(dummy_visualiser) {
  }

  /**
   * @param t the tool that is used for processing
   * @param h the function that is called when the status of the output changes
   **/
  inline processor::processor(tool& t, visualisation_handler h) : program(t), visualise(h) {
  }

  inline processor::~processor() {
  }

  inline sip::configuration::ptr processor::get_configuration() {
    return (configuration);
  }

  inline const tool& processor::get_tool() {
    return (program);
  }

  inline processor::input_list& processor::get_inputs() {
    /* Potentially violates consistency */
    return (inputs);
  }

  inline processor::output_list& processor::get_outputs() {
    /* Potentially violates consistency */
    return (outputs);
  }

  /**
   * \return whether the status was adjusted or not
   **/
  inline bool processor::check_status() {
    /* TODO */
    return (true);
  }

  inline const unsigned int processor::number_of_inputs() const {
    return (inputs.size());
  }

  inline const unsigned int processor::number_of_outputs() const {
    return (outputs.size());
  }

  inline bool processor::consistent_inputs() const {
    input_list::const_iterator i = inputs.begin();

    while (i != inputs.end()) {
      if ((*i).lock().get() == 0) {
        return false;
      }

      ++i;
    }

    return (true);
  }

  inline void processor::flush_outputs() {
    set_output_status(non_existent);

    /* TODO remove all output objects from storage */
  }

  inline const processor::output_status processor::get_output_status() const {
    return(current_output_status);
  }

  inline void processor::set_output_status(const processor::output_status s) {
    if (current_output_status != s) {
      current_output_status = s;
 
      visualise(current_output_status);
    }
  }

  inline void processor::report_change(execution::process::status s) {
    using namespace execution;

    switch (s) {
      case process::stopped:
        set_output_status(not_up_to_date);
        break;
      case process::running:
        set_output_status(being_computed);
        break;
      case process::completed:
        set_output_status(up_to_date);
        break;
      case process::aborted:
        set_output_status(non_existent);
        break;
    }
  }
}

#endif

