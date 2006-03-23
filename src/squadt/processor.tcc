#ifndef PROCESSOR_TCC
#define PROCESSOR_TCC

#include <algorithm>

#include "processor.h"

namespace squadt {

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
    return (current_configuration);
  }

  inline const tool& processor::get_tool() {
    return (program);
  }

  inline const processor::input_list& processor::get_inputs() const {
    return (inputs);
  }

  inline const processor::output_list& processor::get_outputs() const {
    return (outputs);
  }

  /**
   * \attention This function is non-blocking
   **/
  inline void processor::process() throw () {
    global_tool_manager->execute(program, this);
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
