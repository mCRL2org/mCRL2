#ifndef PROCESSOR_TCC
#define PROCESSOR_TCC

#include <algorithm>

#include "task_monitor.h"
#include "processor.h"

namespace squadt {

  /**
   * @param[in] o the processor that owns of this object
   * @param[in] h the function or functor that is called/invoked when the process status changes
   **/
  processor::reporter::reporter(processor& o, callback_handler h) : owner(o), callback(h) {
  }

  /**
   * @param[in] s the new status
   **/
  inline void processor::reporter::report_change(execution::process::status s) {
    using namespace execution;

    switch (s) {
      case process::stopped:
        owner.set_output_status(not_up_to_date);
        break;
      case process::running:
        owner.set_output_status(being_computed);
        break;
      case process::completed:
        owner.set_output_status(up_to_date);
        break;
      case process::aborted:
        owner.set_output_status(non_existent);
        break;
    }
  }

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

  inline processor::processor() : monitor(new reporter(*this, reporter::dummy)) {
  }

  /**
   * @param h the function that is called when the status of the output changes
   **/
  inline processor::processor(reporter::callback_handler h) : monitor(new reporter(*this, h)) {
  }

  inline processor::~processor() {
  }

  inline void processor::set_tool(tool::ptr& t) {
    tool_descriptor = t;
  }

  inline void processor::set_tool(tool::ptr t) {
    tool_descriptor = t;
  }

  inline const tool::ptr processor::get_tool() {
    return (tool_descriptor);
  }

  inline const processor::input_list& processor::get_inputs() const {
    return (inputs);
  }

  /**
   * @param p weak pointer to an object descriptor
   **/
  inline void processor::append_input(object_descriptor::wptr& p) {
    inputs.push_back(p);
  }

  inline const processor::output_list& processor::get_outputs() const {
    return (outputs);
  }

  /**
   * @param p shared pointer to an object descriptor
   **/
  inline void processor::append_output(object_descriptor::sptr& p) {
    p->generator = this;

    outputs.push_back(p);
  }

  /**
   * @param f the storage format that l uses
   * @param l a URI (local path) to where the file is stored
   **/
  inline void processor::append_output(const storage_format& f, const std::string& l) {
    object_descriptor::sptr p = object_descriptor::sptr(new object_descriptor);

    p->format    = f;
    p->location  = l;

    append_output(p);
  }

  /**
   * \attention This function is non-blocking
   **/
  inline void processor::configure(sip::tool::capabilities::input_combination* ic) {
    selected_input_combination = ic;

    sip::configuration::ptr c(new sip::configuration);

    monitor->set_configuration(c);

    global_tool_manager->execute(*tool_descriptor, boost::dynamic_pointer_cast < execution::task_monitor, reporter > (monitor), true);
  }

  /**
   * \attention This function is non-blocking
   *
   * \pre the configure member must have been called
   **/
  inline void processor::process() {
    global_tool_manager->execute(*tool_descriptor, boost::dynamic_pointer_cast < execution::task_monitor, reporter > (monitor), false);
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
 
      monitor->callback(current_output_status);
    }
  }
}

#endif
