#ifndef PROCESSOR_TCC
#define PROCESSOR_TCC

#include <algorithm>
#include <functional>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "task_monitor.h"
#include "processor.h"
#include "project_manager.h"

namespace squadt {

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

  inline processor::~processor() {
  }

  /**
   * @param[in] t the tool descriptor of the tool that is to be used to produce the output from the input
   **/
  inline void processor::set_tool(tool::sptr& t) {
    tool_descriptor = t;
  }

  /**
   * @param[in] t the tool descriptor of the tool that is to be used to produce the output from the input
   **/
  inline void processor::set_tool(tool::sptr t) {
    tool_descriptor = t;
  }

  inline const processor::object_descriptor::sptr processor::find_initial_object() const {
    if (number_of_inputs() != 0) {
      object_descriptor::sptr o(inputs[0]);

      assert(o.get() != 0);

      processor::sptr a(o->generator.lock());

      assert(a.get() != 0);

      return (a->find_initial_object());
    }
    else {
      assert(0 < number_of_outputs());

      return (outputs[0]);
    }
  }

  /**
   * @param[in] i the input combination to set
   **/
  inline void processor::set_input_combination(tool::input_combination* i) {
    selected_input_combination = i;
  }

  inline tool::input_combination const* processor::get_input_combination() const {
    return(selected_input_combination);
  }

  inline const tool::sptr processor::get_tool() {
    return (tool_descriptor);
  }

  inline const processor::monitor::sptr processor::get_monitor() {
    return (current_monitor);
  }

  inline processor::input_object_iterator processor::get_input_iterator() const {
    return (input_object_iterator(inputs));
  }

  /**
   * @param p weak pointer to an object descriptor
   **/
  inline void processor::append_input(object_descriptor::wptr& p) {
    inputs.push_back(p);
  }

  /**
   * @param p weak pointer to an object descriptor
   **/
  inline void processor::append_input(object_descriptor::wptr p) {
    inputs.push_back(p);
  }

  /**
   * @param o the name (location) of the object to change
   * @param n the new name (location) of the object
   **/
  inline void processor::rename_input(std::string const& o, std::string const& n) {
    rename_object(find_output(o), n);
  }

  /**
   * @param o the name (location) of the object to change
   * @param n the new name (location) of the object
   **/
  inline void processor::rename_output(std::string const& o, std::string const& n) {
    rename_object(find_output(o), n);
  }

  inline processor::output_object_iterator processor::get_output_iterator() const {
    return (output_object_iterator(outputs));
  }

  /**
   * @param p shared pointer to an object descriptor
   **/
  inline void processor::append_output(object_descriptor::sptr& p) {
    p->generator = this_object;

    if (std::find_if(outputs.begin(), outputs.end(),
                boost::bind(std::equal_to < std::string >(), p->location,
                        boost::bind(&object_descriptor::location,
                               boost::bind(&object_descriptor::sptr::get, _1)))) == outputs.end()) {

      outputs.push_back(p);
    }
  }

  /**
   * @param[in] f the storage format that l uses
   * @param[in] l a URI (local path) to where the file is stored
   **/
  inline void processor::append_output(const storage_format& f, const std::string& l) {
    object_descriptor::sptr p = object_descriptor::sptr(new object_descriptor);

    p->generator  = this_object;
    p->format     = f;
    p->location   = l;
    p->identifier = 0;
    p->timestamp  = time(0);
    p->checksum.zero_out();

    append_output(p);
  }

  /**
   * @param[in] o a sip::object object that describes an output object
   **/
  inline void processor::append_output(sip::object const& o) {
    object_descriptor::sptr p = object_descriptor::sptr(new object_descriptor);

    p->generator  = this_object;
    p->format     = o.get_format();
    p->location   = o.get_location();
    p->identifier = o.get_id();
    p->timestamp  = time(0);
    p->checksum.zero_out();

    append_output(p);
  }

  /**
   * @param[in] p the object descriptor that should be replaced
   * @param[in] o a sip::object object that describes an output object
   **/
  inline void processor::replace_output(object_descriptor::sptr p, sip::object const& o) {
    p->format     = o.get_format();
    p->location   = o.get_location();
    p->identifier = o.get_id();
    p->timestamp  = time(0);
    p->checksum.zero_out();
  }

  inline void processor::process_configuration() {
    process_configuration(current_monitor->get_configuration());
  }

  /**
   * @param[in] ic the input combination that is to be used
   * @param[in] w the path to the directory in which to run the tool
   * @param[in] l absolute path to the file that serves as main input
   * @param[in] h a function object that is invoked when configuration has completed
   *
   * \attention This function is non-blocking
   **/
  inline void processor::configure(const tool::input_combination* ic, std::string const& w, const boost::filesystem::path& l, boost::function < void() > h) {
    configure(ic, w, l);

    current_monitor->once_on_completion(h);
  }

  /**
   * @param[in] w the path to the directory in which to run the tool
   *
   * \pre The existing configuration must contain the input object matching the selected input combination
   *
   * \attention This function is non-blocking
   **/
  inline void processor::configure(std::string const& w) {
    global_tool_manager->execute(*tool_descriptor, w, boost::dynamic_pointer_cast < execution::task_monitor, monitor > (current_monitor), true);

    current_monitor->once_on_completion(boost::bind(&processor::process_configuration, this));
    current_monitor->start_pilot();
  }

  /**
   * @param[in] w the path to the directory in which to run the tool
   * @param[in] h a function object that is invoked when configuration has completed
   *
   * \attention This function is non-blocking
   **/
  inline void processor::reconfigure(std::string const& w, boost::function < void() > h) {
    current_monitor->once_on_completion(h);

    output_directory = w;

    reconfigure(w);
  }

  /**
   * @param[in] w the path to the directory in which to run the tool
   *
   * \pre The existing configuration must contain the input object matching the selected input combination
   *
   * \attention This function is non-blocking
   **/
  inline void processor::reconfigure(std::string const& w) {
    assert(selected_input_combination != 0);

    output_directory = w;

    sip::configuration::sptr c(sip::controller::communicator::new_configuration(*selected_input_combination));

    c->add_object(current_monitor->get_configuration()->get_object(selected_input_combination->identifier));

    current_monitor->set_configuration(c);

    configure(w);
  }

  inline void processor::process(boost::function < void () > h) {
    current_monitor->once_on_completion(h);

    process();
  }

  inline const size_t processor::number_of_inputs() const {
    return (inputs.size());
  }

  inline const size_t processor::number_of_outputs() const {
    return (outputs.size());
  }

  /**
   * @param[in] b whether or not to check the status first
   **/
  inline bool processor::check_output_consistency(bool b) {
    if (b) {
      check_status(true);
    }

    return(current_output_status);
  }

  inline void processor::set_output_status(const processor::output_status s) {
    if (current_output_status != s) {
      current_output_status = s;
 
      current_monitor->on_status_change(current_output_status);
    }
  }
}

#endif
