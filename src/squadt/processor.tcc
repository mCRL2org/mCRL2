#ifndef PROCESSOR_TCC
#define PROCESSOR_TCC

#include <algorithm>

#include <boost/thread/thread.hpp>

#include "task_monitor.h"
#include "processor.h"

namespace squadt {

  /**
   * @param[in] o the processor that owns of this object
   * @param[in] h the function or functor that is called/invoked when the process status changes
   **/
  processor::reporter::reporter(processor& o) : owner(o) {
    on_status_change = status_change_dummy;
    on_layout_change = layout_change_dummy;

    /* Set the handler for incoming layout messages */
    activate_layout_handler(on_layout_change);
  }

  /**
   * @param[in] h the function or functor that is invoked at layout change
   **/
  inline void processor::reporter::set_layout_handler(layout_callback_function h) {
    /* Set the handler for incoming layout messages */
    activate_layout_handler(h);
  }

  /**
   * @param[in] h the function or functor that is invoked at status change
   **/
  inline void processor::reporter::set_status_handler(status_callback_function h) {
    on_status_change = h;
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
   * @param b whether or not to send the start signal after the configuration is accepted
   **/
  inline void processor::reporter::start_pilot(bool b) {
    boost::thread thread(boost::bind(&processor::reporter::pilot, this, b));
  }

  /**
   * @param b whether or not to send the start signal after the configuration is accepted
   **/
  inline void processor::reporter::pilot(bool b) {
    /* Wait until the tool has connected and identified itself */
    await_connection();

    if (connected) {
      send_configuration();
     
      /* Wait until configuration is accepted, or the tool has terminated */
      if (await_message(sip::send_accept_configuration).get() != 0 && b) {
        send_start_signal();
      }
      else {
        /* End tool execution */
        finish();
      }
    }
  }

  /**
   * \brief Operator for writing to stream
   *
   * @param[in] s stream to write to
   * @param[out] p the processor to write out
   **/
  inline std::ostream& operator << (std::ostream& s, const processor& p) {
    p.write(s);

    return (s);
  }

  /**
   * @param[in] h the function that is called when the status of the output changes
   **/
  inline processor::processor() : monitor(new reporter(*this)) {
  }

  /**
   * @param[in] h the function that is called when the status of the output changes
   * @param[in] t the tool descriptor of the tool that is to be used to produce the output from the input
   **/
  inline processor::processor(tool::ptr t) :
                tool_descriptor(t), monitor(new reporter(*this)) {
  }

  inline processor::~processor() {
  }

  /**
   * @param[in] t the tool descriptor of the tool that is to be used to produce the output from the input
   **/
  inline void processor::set_tool(tool::ptr& t) {
    tool_descriptor = t;
  }

  /**
   * @param[in] t the tool descriptor of the tool that is to be used to produce the output from the input
   **/
  inline void processor::set_tool(tool::ptr t) {
    tool_descriptor = t;
  }

  inline const tool::ptr processor::get_tool() {
    return (tool_descriptor);
  }

  inline const processor::reporter::ptr processor::get_monitor() {
    return (monitor);
  }

  inline processor::input_object_iterator processor::get_inputs_iterator() const {
    return (input_object_iterator(inputs));
  }

  /**
   * @param p weak pointer to an object descriptor
   **/
  inline void processor::append_input(object_descriptor::wptr& p) {
    inputs.push_back(p);
  }

  inline processor::output_object_iterator processor::get_outputs_iterator() const {
    return (output_object_iterator(outputs));
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
    p->timestamp = time(0);
    p->checksum.zero_out();

    append_output(p);
  }

  /**
   * @param[in] ic the input combination that is to be used
   * @param[in] l the file that serves as main input
   * \attention This function is non-blocking
   **/
  inline void processor::configure(const tool::input_combination* ic, const boost::filesystem::path& l) {
    selected_input_combination = const_cast < tool::input_combination* > (ic);

    sip::configuration::ptr c(new sip::configuration);

    c->add_input(ic->identifier, ic->format, l.string());

    monitor->set_configuration(c);

    global_tool_manager->execute(*tool_descriptor, boost::dynamic_pointer_cast < execution::task_monitor, reporter > (monitor), true);

    monitor->start_pilot();
  }

  /**
   * \attention This function is non-blocking
   *
   * \pre the configure member must have been called
   **/
  inline void processor::process() {
    global_tool_manager->execute(*tool_descriptor, boost::dynamic_pointer_cast < execution::task_monitor, reporter > (monitor), false);

    monitor->start_pilot();
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
 
      monitor->on_status_change(current_output_status);
    }
  }
}

#endif
