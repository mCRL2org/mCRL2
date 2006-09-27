#include <algorithm>
#include <string>
#include <vector>
#include <iosfwd>
#include <ctime>

#include <boost/function.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

#include <xml2pp/text_reader.h>
#include <sip/controller.h>
#include <utility/logger.h>

#include "processor.tcc"
#include "exception.h"
#include "task_monitor.h"

namespace squadt {

  using namespace boost::filesystem;

  bool processor_impl::try_change_status(processor::object_descriptor& o, processor::object_descriptor::t_status s) {
    if (o.status != s) {
      o.status = s;

      o.generator.lock()->get_monitor()->status_change_handler();

      return (true);
    }

    return (false);
  }

  bool processor::object_descriptor::present_in_store(project_manager const& m) {
    path l(m.get_path_for_name(location));

    if (exists(l)) {
      return (true);
    }
    else {
      processor_impl::try_change_status(*this, reproducible_nonexistent);

      return (false);
    }
  }

  bool processor::object_descriptor::generator_exists() {
    return (generator.lock().get() != 0);
  }

  bool processor::object_descriptor::is_up_to_date() {
    if (status != original && status != reproducible_up_to_date) {
      return (false);
    }
    else {
      processor::sptr p(generator.lock());

      if (p) {
        return (!p->check_status(true));
      }
      else {
        return (false);
      }
    }
  }

  /**
   * \param[in] m a reference to a project manager, used to obtain complete paths files in the project
   * \param[in] o a shared pointer to the object on which to operate
   * \param[in] t objects older than this time stamp are considered obsolete
   **/
  bool processor::object_descriptor::self_check(project_manager const& m, long int const& t) {
    using namespace boost::filesystem;

    path l(m.get_path_for_name(location));
    
    if (exists(l)) {
      /* Input exists, get timestamp */ 
      time_t stamp = last_write_time(l);
    
      if (last_write_time(l) < t) {
        return (processor_impl::try_change_status(*this, reproducible_out_of_date));
      }
      else if (timestamp < stamp) {
        /* Compare checksums and update recorded checksum */
        md5pp::compact_digest old = checksum;
    
        checksum  = md5pp::MD5::MD5_sum(l);
    
        if (timestamp != 0 && !checksum.is_zero() && old != checksum) {
          return (processor_impl::try_change_status(*this, reproducible_out_of_date));
        }

        timestamp = stamp;
      }
    }
    else {
      return (processor_impl::try_change_status(*this, reproducible_nonexistent));
    }

    return (false);
  }

  void processor::monitor::status_change_dummy() {
    get_logger()->log(2, "No custom status change event handler connected!\n");
  }

  void processor::monitor::display_layout_change_dummy(sip::layout::tool_display::sptr) {
    get_logger()->log(2, "No custom display change event handler connected!\n");
  }

  void processor::monitor::display_data_change_dummy(sip::layout::tool_display::constant_elements const&) {
    get_logger()->log(2, "No custom display state change event handler connected!");
  }

  void processor::monitor::status_message_change_dummy(sip::report::sptr) {
    get_logger()->log(2, "Incoming report lost!");
  }

  /**
   * @param[in] o the processor that owns of this object
   **/
  processor::monitor::monitor(processor& o) : owner(o) {
    status_change_handler  = boost::bind(&processor::monitor::status_change_dummy, this);
    layout_change_handler  = boost::bind(&processor::monitor::display_layout_change_dummy, this, _1);
    state_change_handler   = boost::bind(&processor::monitor::display_data_change_dummy, this, _1);
    message_change_handler = boost::bind(&processor::monitor::status_message_change_dummy, this, _1);

    /* Set the handler for incoming layout messages */
    activate_display_layout_handler(layout_change_handler);

    /* Set the handler for incoming layout messages */
    activate_display_data_handler(sip::layout::tool_display::sptr(), state_change_handler);
  }

  /**
   * @param[in] s the new status
   **/
  void processor::monitor::signal_change(const execution::process::status s) {
    using namespace execution;

    task_monitor::signal_change(s);

    switch (s) {
      case process::stopped:
        break;
      case process::running:
        break;
      case process::completed:
        for (processor::output_object_iterator i = owner.get_output_iterator(); i.valid(); ++i) {
          (*i)->status = object_descriptor::reproducible_up_to_date;
        }
        break;
      default: /* aborted... */
        for (processor::output_object_iterator i = owner.get_output_iterator(); i.valid(); ++i) {
          if ((*i)->status == object_descriptor::generation_in_progress) {
            (*i)->status = object_descriptor::reproducible_nonexistent;
          }
        }
        break;
    }

    /* Update status for known processor outputs */
    status_change_handler();
  }

  execution::process::status processor::monitor::get_status() {
    execution::process::sptr p = get_process();

    if (p.get() != 0) {
      return (p->get_status());
    }

    return (execution::process::stopped);
  }

  void processor::monitor::tool_configuration(processor::sptr t) {
    assert(t.get() == &owner);

    /* Wait until the tool has connected and identified itself */
    await_connection();

    if (is_connected()) {
      /* Make sure that the task_monitor state is not cleaned up if the tool quits unexpectedly */
      send_configuration();

      /* Wait until configuration is accepted, or the tool has terminated */
      if (await_message(sip::message_accept_configuration).get()) {
        /* Successful, set new status */
        for (processor::output_object_iterator i = owner.get_output_iterator(); i.valid(); ++i) {
          (*i)->status = object_descriptor::reproducible_up_to_date;
        }
      }

      /* End tool execution */
      finish();

      /* Now run the tool */
      t->run(true);
    }
    else {
      /* End tool execution */
      finish();
    }
  }

  void processor::monitor::tool_operation(processor::sptr t) {
    assert(t.get() == &owner);

    /* Wait until the tool has connected and identified itself */
    await_connection();

    if (is_connected()) {
      /* Make sure that the task_monitor state is not cleaned up if the tool quits unexpectedly */
      send_configuration();

      /* Wait until configuration is accepted, or the tool has terminated */
      if (await_message(sip::message_accept_configuration).get() != 0) {
        send_start_signal();

        if (!await_message(sip::message_signal_done)->is_empty()) {
          /* Operation completed successfully */
          t->impl->process_configuration(get_configuration());
        }
      }
    }

    /* End tool execution */
    finish();

    /* Force the project manager to do a status update */
    t->impl->manager->update_status(t.get());
  }

  /**
   * @param[in] h the function or functor that is invoked at layout change
   **/
  void processor::monitor::set_display_layout_handler(display_layout_callback_function h) {
    /* Set the handler for incoming layout messages */
    activate_display_layout_handler(h);
  }

  /**
   * @param[in] d the tool display associated with this monitor
   * @param[in] h the function or functor that is invoked at layout change
   **/
  void processor::monitor::set_display_data_handler(sip::layout::tool_display::sptr d, display_data_callback_function h) {
    /* Set the handler for incoming layout messages */
    activate_display_data_handler(d, h);
  }

  /**
   * @param[in] h the function or functor that is invoked at layout change
   **/
  void processor::monitor::set_status_message_handler(status_message_callback_function h) {
    /* Set the handler for incoming layout messages */
    activate_status_message_handler(h);
  }

  /**
   * @param[in] h the function or functor that is invoked at layout change
   **/
  void processor::monitor::reset_display_layout_handler() {
    layout_change_handler  = boost::bind(&processor::monitor::display_layout_change_dummy, this, _1);

    /* Set the handler for incoming layout messages */
    activate_display_layout_handler(layout_change_handler);
  }

  /**
   * @param[in] d the tool display associated with this monitor
   * @param[in] h the function or functor that is invoked at layout change
   **/
  void processor::monitor::reset_display_data_handler() {
    /* Set the handler for incoming layout messages */
    state_change_handler = boost::bind(&processor::monitor::display_data_change_dummy, this, _1);

    activate_display_data_handler(sip::layout::tool_display::sptr(), state_change_handler);
  }

  /**
   * @param[in] h the function or functor that is invoked at layout change
   **/
  void processor::monitor::reset_status_message_handler() {
    /* Set the handler for incoming layout messages */
    message_change_handler = boost::bind(&processor::monitor::status_message_change_dummy, this, _1);

    activate_status_message_handler(message_change_handler);
  }

  /**
   * @param[in] h the function or functor that is invoked at status change
   **/
  void processor::monitor::set_status_handler(status_callback_function h) {
    status_change_handler = h;
  }

  void processor::monitor::start_tool_configuration(processor::sptr const& t) {
    boost::thread thread(boost::bind(&processor::monitor::tool_configuration, this, t));
  }

  void processor::monitor::start_tool_operation(processor::sptr const& t) {
    boost::thread thread(boost::bind(&processor::monitor::tool_operation, this, t));
  }

  processor::processor() {
  }

  /**
   * @param[in] p the associated project manager
   **/
  processor::sptr processor::create(project_manager& p) {
    processor::sptr n(new processor());

    n->impl.reset(new processor_impl(n, p));

    return (n);
  }

  /**
   * @param[in] p the associated project manager
   * @param[in] t the tool to use
   **/
  processor::sptr processor::create(project_manager& p, tool::sptr t) {
    processor::sptr n(new processor());

    n->impl.reset(new processor_impl(n, p, t));

    return (n);
  }

  /**
   * @param[in] r whether to check recursively or not
   **/
  bool processor::check_status(bool r) {
    return (impl->check_status(r));
  }

  /**
   * @param s the stream to write to
   **/
  void processor::write(std::ostream& s) const {
    impl->write(s);
  }

  /**
   * @param[in] t the tool descriptor of the tool that is to be used to produce the output from the input
   **/
  void processor::set_tool(tool::sptr const& t) {
    impl->tool_descriptor = t;
  }

  const tool::sptr processor::get_tool() {
    return (impl->tool_descriptor);
  }

  /**
   * @param[in] i the input combination to set
   **/
  void processor::set_input_combination(tool::input_combination* i) {
    impl->selected_input_combination = i;
  }

  tool::input_combination const* processor::get_input_combination() const {
    return(impl->selected_input_combination);
  }

  boost::shared_ptr < processor::monitor > processor::get_monitor() {
    return (impl->current_monitor);
  }

  processor::input_object_iterator processor::get_input_iterator() const {
    return (input_object_iterator(impl->inputs));
  }

  /**
   * @param p weak pointer to an object descriptor
   **/
  void processor::append_input(object_descriptor::sptr const& p) {
    impl->inputs.push_back(p);
  }

  /**
   * @param o the name (location) of the object to change
   * @param n the new name (location) of the object
   **/
  void processor::rename_input(std::string const& o, std::string const& n) {
    impl->rename_object(impl->find_input(o), n);
  }

  /**
   * @param o the name (location) of the object to change
   * @param n the new name (location) of the object
   **/
  void processor::rename_output(std::string const& o, std::string const& n) {
    impl->rename_object(impl->find_output(o), n);
  }

  processor::output_object_iterator processor::get_output_iterator() const {
    return (output_object_iterator(impl->outputs));
  }

  /**
   * @param[in] p reference to the associated project_manager object
   * @param[in] r an XML text reader object to read from
   * @param[in] m a map that is used to associate shared pointers to processors with identifiers
   *
   * \pre must point to a processor element
   * \attention the same map m must be used to read back all processor instances that were written with write()
   **/
  processor::sptr processor::read(project_manager& p, id_conversion_map& m, xml2pp::text_reader& r) {
    return (processor_impl::read(p, m, r));
  }

  void processor::flush_outputs() {
    impl->flush_outputs();
  }

  /**
   * @param[in] f the storage format that l uses
   * @param[in] l a URI (local path) to where the file is stored
   * @param[in] s the status of the new object
   **/
  void processor::append_output(const storage_format& f, const std::string& l, object_descriptor::t_status const& s) {
    object_descriptor::sptr p = object_descriptor::sptr(new object_descriptor);

    p->generator  = impl->interface_object;
    p->format     = f;
    p->location   = l;
    p->identifier = 0;
    p->status     = s;
    p->timestamp  = time(0);
    p->checksum.zero_out();

    impl->append_output(p);
  }

  /**
   * @param[in] o a sip::object object that describes an output object
   * @param[in] s the status of the new object
   **/
  void processor_impl::append_output(sip::object const& o, object_descriptor::t_status const& s) {
    object_descriptor::sptr p = object_descriptor::sptr(new object_descriptor);

    p->generator  = interface_object;
    p->format     = o.get_format();
    p->location   = o.get_location();
    p->identifier = o.get_id();
    p->status     = s;
    p->timestamp  = time(0);
    p->checksum.zero_out();

    append_output(p);
  }

  /**
   * @param[in] p the object descriptor that should be replaced
   * @param[in] o a sip::object object that describes an output object
   **/
  void processor_impl::replace_output(object_descriptor::sptr p, sip::object const& o, object_descriptor::t_status const& s) {
    p->format     = o.get_format();
    p->location   = o.get_location();
    p->identifier = o.get_id();
    p->status     = s;
    p->timestamp  = time(0);
    p->checksum.zero_out();
  }

  void processor::configure(std::string const& w) {
    assert(impl->interface_object.lock().get() == this);

    impl->configure(impl->interface_object.lock(), w);
  }

  void processor::configure(const tool::input_combination* i, const boost::filesystem::path& p, std::string const& w) {
    assert(impl->interface_object.lock().get() == this);

    impl->configure(impl->interface_object.lock(), i, p, w);
  }

  void processor::reconfigure(std::string const& w) {
    assert(impl->interface_object.lock().get() == this);

    impl->reconfigure(impl->interface_object.lock(), w);
  }

  /**
   * \param[in] h the function to execute when the process terminates
   * \param[in] b whether or not to run when there are no input objects defined
   *
   * \pre t.get() == this
   **/
  void processor::update(boost::function < void () > h, bool b) {
    assert(impl->interface_object.lock().get() == this);

    impl->run(impl->interface_object.lock(), h, b);
  }

  /**
   * \param[in] h the function to execute when the process terminates
   * \param[in] b whether or not to run when there are no input objects defined
   *
   * \pre t.get() == this
   **/
  void processor::run(boost::function < void () > h, bool b) {
    assert(impl->interface_object.lock().get() == this);

    impl->run(impl->interface_object.lock(), h, b);
  }

  void processor::run(bool b) {
    assert(impl->interface_object.lock().get() == this);

    impl->run(impl->interface_object.lock(), b);
  }

  void processor::update(bool b) {
    assert(impl->interface_object.lock().get() == this);

    impl->update(impl->interface_object.lock(), b);
  }

  const size_t processor::number_of_inputs() const {
    return (impl->inputs.size());
  }

  const size_t processor::number_of_outputs() const {
    return (impl->outputs.size());
  }

  bool processor::is_active() const {
    return (impl->current_monitor->get_status() == execution::process::running);
  }

}

