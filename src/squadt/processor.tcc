#ifndef PROCESSOR_TCC
#define PROCESSOR_TCC

#include <algorithm>
#include <functional>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem/convenience.hpp>

#include "task_monitor.h"
#include "processor.h"
#include "project_manager.h"
#include "executor.h"
#include "tool_manager.tcc"

namespace squadt {
  /// \cond PRIVATE_PART

  class processor_impl {
    friend class processor;
    friend class processor::object_descriptor;
    friend class processor::monitor;

    private:

      /** \brief Pointer type for implementation object (handle-body idiom) */
      typedef boost::shared_ptr < processor_impl >    impl_ptr;

      /** \brief Pointer type for interface object (handle-body idiom) */
      typedef boost::shared_ptr < processor >         interface_ptr;

      /** \brief Type for object specification */
      typedef processor::object_descriptor            object_descriptor;

      /** \brief Type for object status specification */
      typedef processor::object_descriptor::t_status  object_status;

      /** \brief Type alias for monitor class */
      typedef processor::monitor                      monitor;

      /** \brief Type alias for list of inputs */
      typedef processor::input_list                   input_list;

      /** \brief Type alias for list of inputs */
      typedef processor::output_list                  output_list;

    private:

      /** \brief Helper type for read() members */
      typedef std::map < unsigned long, object_descriptor::sptr >           id_conversion_map;

    private:

      /** \brief Weak pointer to this object for passing */
      boost::weak_ptr < processor >        interface_object;
 
      /** \brief Identifies the tool that is required to run the command */
      tool::sptr                           tool_descriptor;

      /** \brief The information about inputs of this processor */
      input_list                           inputs;

      /** \brief The information about outputs of this processor */
      output_list                          outputs;
 
      /** \brief The current task that is running or about to run */
      monitor::sptr                        current_monitor;

      /** \brief The associated project manager */
      boost::weak_ptr < project_manager >  manager;
 
      /** \brief The selected input combination of the tool */
      tool::input_combination const*       selected_input_combination;

      /** \brief The directory from which tools should be run on behalf of this object */
      std::string                          output_directory;

    private:

      /** \brief Helper function for adjusting status */
      static bool try_change_status(processor::object_descriptor&, object_status);

    private:

      /** \brief Basic constructor */
      inline processor_impl(boost::shared_ptr < processor > const&, boost::weak_ptr < project_manager >);

      /** \brief Constructor with tool selection */
      inline processor_impl(boost::shared_ptr < processor > const&, boost::weak_ptr < project_manager >, tool::sptr);

      /** \brief Execute an edit command on one of the outputs */
      void edit(execution::command*);

      /** \brief Extracts useful information from a configuration object */
      void process_configuration(sip::configuration::sptr const& c);

      /** \brief Find an object descriptor for a given pointer to an object */
      const object_descriptor::sptr find_output(object_descriptor*) const;
 
      /** \brief Find an object descriptor for a given pointer to an object */
      const object_descriptor::sptr find_input(object_descriptor*) const;
 
      /** \brief Find an object descriptor for a given pointer to an object (by id) */
      const object_descriptor::sptr find_output(const unsigned int) const;
 
      /** \brief Find an object descriptor for a given pointer to an object (by id) */
      const object_descriptor::sptr find_input(const unsigned int) const;
 
      /** \brief Find an object descriptor for a given pointer to an object */
      const object_descriptor::sptr find_output(std::string const&) const;
 
      /** \brief Find an object descriptor for a given pointer to an object */
      const object_descriptor::sptr find_input(std::string const&) const;
 
      /** \brief Get the most original (main) input */
      const object_descriptor::sptr find_initial_object() const;
 
      /** \brief Find an object descriptor for a given name and rename if it exists */
      void rename_object(object_descriptor::sptr const&, std::string const&);

      /** \brief Prepends the absolute path to the project store */
      std::string make_output_path(std::string const&) const;

      /** \brief Check the inputs with respect to the outputs and adjust status accordingly */
      bool check_status(bool);

      /** \brief Sets the status of the inputs to out-of-date if the processor is inactive */
      bool demote_status();

      /** \brief Start tool configuration */
      void configure(interface_ptr const&, const tool::input_combination*, const boost::filesystem::path&, std::string const& = "");
 
      /** \brief Start tool configuration */
      void configure(interface_ptr const&, std::string const& = "");

      /** \brief Start tool reconfiguration */
      void reconfigure(interface_ptr const&, std::string const& = "");
 
      /** \brief Start processing: generate outputs from inputs */
      void run(interface_ptr const&, bool b = false);

      /** \brief Start running and afterward execute a function */
      void run(interface_ptr const&, boost::function < void () > h, bool b = false);

      /** \brief Start processing if not all outputs are up to date */
      void update(interface_ptr const&, bool b = false);
 
      /** \brief Start updating and afterward execute a function */
      void update(interface_ptr const&, boost::function < void () > h, bool b = false);

      /** \brief Add an output object */
      void append_output(object_descriptor::sptr&);

      /** \brief Add an output object */
      void append_output(sip::object const&,
                object_descriptor::t_status const& = object_descriptor::reproducible_nonexistent);

      /** \brief Replace an existing output object */
      void replace_output(object_descriptor::sptr, sip::object const&,
                object_descriptor::t_status const& = object_descriptor::reproducible_up_to_date);

      /** \brief Read from XML using a libXML2 reader */
      static processor::sptr read(boost::weak_ptr < project_manager > const&, id_conversion_map&, xml2pp::text_reader&);

      /** \brief Write as XML to stream */
      void write(std::ostream& stream = std::cout) const;
      
      /** \brief Removes the outputs of this processor from storage */
      void flush_outputs();

      /** \brief Whether or not a process is running on behalf of this processor */
      bool is_active() const;

      /** \brief Terminates running processes and deactivates monitor */
      void shutdown();
  };

  /**
   * \param[in] t shared pointer to the interface object
   * \param[in] h the function to execute when the process terminates
   * \param[in] b whether or not to run when there are no input objects defined
   *
   * \pre t.get() == this
   **/
  inline void processor_impl::update(interface_ptr const& t, boost::function < void () > h, bool b) {
    current_monitor->once_on_completion(h);

    run(t, b);
  }

  /**
   * \param[in] t shared pointer to the interface object
   * \param[in] h the function to execute when the process terminates
   * \param[in] b whether or not to run when there are no input objects defined
   *
   * \pre t.get() == this
   **/
  inline void processor_impl::run(interface_ptr const& t, boost::function < void () > h, bool b) {
    current_monitor->once_on_completion(h);

    run(t, b);
  }

  /**
   * \param p shared pointer to an object descriptor
   **/
  inline void processor_impl::append_output(object_descriptor::sptr& p) {
    p->generator = interface_object;

    if (std::find_if(outputs.begin(), outputs.end(),
                boost::bind(std::equal_to < std::string >(), p->location,
                        boost::bind(&object_descriptor::location,
                               boost::bind(&object_descriptor::sptr::get, _1)))) == outputs.end()) {

      outputs.push_back(p);
    }
  }

  /**
   * \brief Operator for writing to stream
   *
   * \param[in] s stream to write to
   * \param[out] p the processor to write out
   **/
  inline std::ostream& operator << (std::ostream& s, const processor& p) {
    p.write(s);

    return (s);
  }

  inline processor_impl::processor_impl(boost::shared_ptr < processor > const& tp, boost::weak_ptr < project_manager > p) :
                interface_object(tp), current_monitor(new monitor(*tp)), manager(p), selected_input_combination(0) {
  }

  /**
   * \param[in] tp shared pointer to the interface object
   * \param[in] p the associated project manager
   * \param[in] t the tool descriptor of the tool that is to be used to produce the output from the input
   **/
  inline processor_impl::processor_impl(boost::shared_ptr < processor > const& tp, boost::weak_ptr < project_manager > p, tool::sptr t) :
    interface_object(tp), tool_descriptor(t), current_monitor(new monitor(*tp)), manager(p), selected_input_combination(0) {
  }

  /**
   * \param[in] r whether to check recursively or not
   *
   * \return whether or not the outputs this processor can produce exist and are up to date
   **/
  inline bool processor_impl::check_status(const bool r) {
    bool   result                   = false;
    time_t maximum_input_timestamp  = 0;

    boost::shared_ptr < project_manager > g(manager.lock());

    if (g.get()) {
      /* Find the maximum timestamp of the inputs */
      BOOST_FOREACH(object_descriptor::wptr i, inputs) {
        object_descriptor::sptr d = i.lock();
      
        if (d.get() == 0) {
          throw (exception::exception(exception::missing_object_descriptor));
        }
      
        d->self_check(*g);
     
        result |= (d->status != object_descriptor::original) && (d->status != object_descriptor::reproducible_up_to_date);
      
        maximum_input_timestamp = std::max(maximum_input_timestamp, d->timestamp);
      }
     
      /* Check whether outputs all exist and find the minimum timestamp of the inputs */
      BOOST_FOREACH(object_descriptor::sptr i, outputs) {
        i->self_check(*g, maximum_input_timestamp);
     
        result |= (i->status != object_descriptor::original) && (i->status != object_descriptor::reproducible_up_to_date);
      }
     
      if (!result && r) {
        /* Status can still be okay, check recursively */
        BOOST_FOREACH(object_descriptor::wptr i, inputs) {
          object_descriptor::sptr d = i.lock();
      
          if (d.get() == 0) {
            throw (exception::exception(exception::missing_object_descriptor));
          }
     
          processor::sptr p(d->generator);
     
          if (p.get() != 0) {
            result |= p->check_status(true);
          }
        }
      }
     
      if (result) {
        if (0 < inputs.size()) {
          BOOST_FOREACH(object_descriptor::sptr i, outputs) {
            if (i->status == object_descriptor::reproducible_up_to_date) {
              try_change_status(*i, object_descriptor::reproducible_out_of_date);
            }
          }
        }
        else {
          /* User added files are always up to date */
          g->demote_status(interface_object.lock().get());
        }
      }

      return (result);
    }

    return (false);
  }

  bool processor_impl::demote_status() {
    bool result = false;

    if (!is_active()) {
      BOOST_FOREACH(object_descriptor::sptr i, outputs) {
        result |= try_change_status(*i, object_descriptor::reproducible_out_of_date);
      }
    }

    return (result);
  }

  /**
   * \param s the stream to write to
   **/
  inline void processor_impl::write(std::ostream& s) const {
    s << "<processor";

    if (tool_descriptor.get() != 0) {
      s << " tool-name=\"" << tool_descriptor->get_name() << "\"";

      if (selected_input_combination != 0) {
        s << " format=\"" << selected_input_combination->m_mime_type << "\"";
        s << " category=\"" << selected_input_combination->m_category << "\"";
      }
    }

    if (!output_directory.empty()) {
      s << " output-directory=\"" << output_directory << "\">";
    }
    else {
      s << ">";
    }

    /* The last received configuration from the tool */
    sip::configuration::sptr c = current_monitor->get_configuration();

    if (c.get() != 0) {
      c->write(s);
    }

    /* The inputs */
    for (input_list::const_iterator i = inputs.begin(); i != inputs.end(); ++i) {
      s << "<input id=\"" << std::dec << reinterpret_cast < unsigned long > ((*i).get()) << "\"/>\n";
    }

    /* The outputs */
    for (output_list::const_iterator i = outputs.begin(); i != outputs.end(); ++i) {
      s << "<output id=\"" << std::dec << reinterpret_cast < unsigned long > ((*i).get())
        << "\" format=\"" << (*i)->mime_type
        << "\" location=\"" << (*i)->location
        << "\" identifier=\"" << std::dec << (*i)->identifier
        << "\" status=\"" << (*i)->status;

      if (!(*i)->checksum.is_zero()) {
        s << "\" digest=\"" << (*i)->checksum;
      }

      s << "\" timestamp=\"" << std::dec << (*i)->timestamp << "\"/>\n";
    }

    s << "</processor>\n";
  }

  /**
   * \param[in] p reference to the associated project_manager object
   * \param[in] r an XML text reader object to read from
   * \param[in] m a map that is used to associate shared pointers to processors with identifiers
   *
   * \pre must point to a processor element
   * \attention the same map m must be used to read back all processor instances that were written with write()
   **/
  inline processor::sptr processor_impl::read(boost::weak_ptr < project_manager > const& p, id_conversion_map& m, xml2pp::text_reader& r) {
    processor::sptr c = processor::create(p);
    std::string     temporary;

    if (r.get_attribute(&temporary, "tool-name")) {
      c->impl->tool_descriptor = global_build_system.get_tool_manager()->get_tool_by_name(temporary);

      std::string format;
      std::string category;

      if (r.get_attribute(&category, "category") && r.get_attribute(&format, "format")) {
        c->impl->selected_input_combination = c->impl->tool_descriptor->find_input_combination(
                      sip::tool::category::fit(category), sip::mime_type(format));
      }
    }

    c->impl->output_directory = r.get_attribute_as_string("output-directory");

    r.next_element();

    if (r.is_element("configuration")) {
      c->impl->current_monitor->set_configuration(sip::configuration::read(r));
    }

    /* Read inputs */
    while (r.is_element("input")) {
      unsigned long id;

      if (!r.get_attribute(&id, "id")) {
        throw (exception::exception(exception::required_attributes_missing, "processor->input"));
      }
      else {
        assert(m.find(id) != m.end());

        c->impl->inputs.push_back(object_descriptor::sptr(m[id]));
      }

      r.next_element();

      r.skip_end_element("input");
    }

    /* Read outputs */
    while (r.is_element("output")) {
      unsigned long id;
      bool          b = r.get_attribute(&id, "id");

      if (b) {
        if (!r.get_attribute("format")) {
          throw (exception::exception(exception::required_attributes_missing, "processor->output"));
        }
       
        if (b) {
          assert(m.find(id) == m.end());
       
          m[id] = object_descriptor::sptr(new object_descriptor(sip::mime_type(r.get_attribute_as_string("format"))));
       
          c->impl->outputs.push_back(m[id]);
        }
      }

      object_descriptor& new_descriptor = *m[id];

      if (!(b && r.get_attribute(&new_descriptor.location, "location")
              && r.get_attribute(&new_descriptor.identifier, "identifier")
              && r.get_attribute(&new_descriptor.timestamp, "timestamp")
              && r.get_attribute(&id, "status"))) {

        throw (exception::exception(exception::required_attributes_missing, "processor->output"));
      }

      new_descriptor.status = static_cast < object_status > ((id == object_descriptor::generation_in_progress) ?
                                                      object_descriptor::reproducible_nonexistent : id);

      if (r.get_attribute(&temporary, "digest")) {
        new_descriptor.checksum.read(temporary.c_str());
      }
      else {
        new_descriptor.checksum = md5pp::zero_digest;
      }

      new_descriptor.generator = c;

      r.next_element();

      r.skip_end_element("output");
    }

    r.skip_end_element("processor");

    return (c);
  }

  inline bool processor_impl::is_active() const {
    return (current_monitor->get_status() == execution::process::running);
  }

  inline void processor_impl::shutdown() {
    current_monitor->shutdown();
    current_monitor->finish();
  }

  inline void processor_impl::flush_outputs() {
    using namespace boost::filesystem;

    boost::shared_ptr < project_manager > g(manager.lock());

    if (g.get() && !is_active()) {
      /* Make sure any output objects are removed from storage */
      for (output_list::const_iterator i = outputs.begin(); i != outputs.end(); ++i) {
        path p(g->get_path_for_name((*i)->location));

        if (exists(p)) {
          remove(p);

          try_change_status(*(*i), object_descriptor::reproducible_nonexistent);
        }
      }
      
      g->update_status(interface_object.lock().get());
    }
  }

  inline const processor::object_descriptor::sptr processor_impl::find_initial_object() const {
    if (inputs.size() != 0) {
      object_descriptor::sptr o(inputs[0]);

      assert(o.get() != 0);

      processor::sptr a(o->generator.lock());

      assert(a.get() != 0);

      return (a->impl->find_initial_object());
    }
    else {
      assert(0 < outputs.size());

      return (outputs[0]);
    }
  }

  /**
   * \param o a pointer to the object to find
   **/
  inline const processor::object_descriptor::sptr processor_impl::find_output(object_descriptor* o) const {
    output_list::const_iterator i = std::find_if(outputs.begin(), outputs.end(),
                boost::bind(std::equal_to < object_descriptor* >(), o, 
                               boost::bind(&object_descriptor::sptr::get, _1)));
                               
    if (i != outputs.end()) {
      return (*i);
    }

    object_descriptor::sptr s;

    return (s);
  }

  /**
   * \param o a pointer to the object to find
   **/
  inline const processor::object_descriptor::sptr processor_impl::find_input(object_descriptor* o) const {
    for (input_list::const_iterator i = inputs.begin(); i != inputs.end(); ++i) {
      object_descriptor::sptr s = (*i);

      if (s.get() == o) {
        return (s);
      }
    }
                               
    object_descriptor::sptr s;

    return (s);
  }

  /**
   * \param[in] id a pointer to the object to find
   **/
  inline const processor::object_descriptor::sptr processor_impl::find_output(const unsigned int id) const {
    for (output_list::const_iterator i = outputs.begin(); i != outputs.end(); ++i) {
      if ((*i)->identifier == id) {

        return (*i);
      }
    }

    object_descriptor::sptr s;

    return (s);
  }

  /**
   * \param[in] id the id of the object to find
   **/
  inline const processor::object_descriptor::sptr processor_impl::find_input(const unsigned int id) const {
    for (input_list::const_iterator i = inputs.begin(); i != inputs.end(); ++i) {
      object_descriptor::sptr s = (*i);

      if (s.get() != 0 && s->identifier == id) {
        return (s);
      }
    }
                               
    object_descriptor::sptr s;

    return (s);
  }

  /**
   * \param o the name of the input object to find
   **/
  inline const processor::object_descriptor::sptr processor_impl::find_output(std::string const& o) const {
    for (output_list::const_iterator i = outputs.begin(); i != outputs.end(); ++i) {
      if ((*i)->location == o) {

        return (*i);
      }
    }

    object_descriptor::sptr s;

    return (s);
  }

  /**
   * \param o the name of the input object to find
   **/
  inline const processor::object_descriptor::sptr processor_impl::find_input(std::string const& o) const {
    for (input_list::const_iterator i = inputs.begin(); i != inputs.end(); ++i) {
      object_descriptor::sptr s = (*i);

      if (s.get() != 0 && s->location == o) {
        return (s);
      }
    }
                               
    object_descriptor::sptr s;

    return (s);
  }

  /**
   * \param[in] o the name (location) of the object to change
   * \param[in] n the new name (location) of the object
   **/
  inline void processor_impl::rename_object(object_descriptor::sptr const& o, std::string const& n) {
    using namespace boost::filesystem;

    boost::shared_ptr < project_manager > g(manager.lock());

    if (g.get() != 0 && o.get() != 0) {
      path source(g->get_path_for_name(o->location));
      path target(g->get_path_for_name(n));

      if (exists(source) && source != target) {
        if (exists(target)) {
          remove(target);
        }

        rename(source, target);
      }

      o->location = n;

      /* Update configuration */
      sip::configuration::sptr c = current_monitor->get_configuration();

      if (c.get() != 0) {
        sip::object::sptr object(c->get_output(o->identifier));

        if (object.get() != 0) {
          object->set_location(n);
        }
      }
    }
  }

  /**
   * \param[in] c a reference to a configuration object
   **/
  inline void processor_impl::process_configuration(sip::configuration::sptr const& c) {
    boost::shared_ptr < project_manager > g(manager.lock());

    if (g.get()) {
      /* Extract information about output objects from the configuration */
      for (sip::configuration::object_iterator i = c->get_object_iterator(); i.valid(); ++i) {
        if ((*i)->get_type() == sip::object::output) {
          object_descriptor::sptr o = find_output((*i)->get_id());
      
          if (o.get() == 0) {
            /* Output not registered yet */
            append_output(*(*i), object_descriptor::reproducible_nonexistent);
          }
          else {
            if ((*i)->get_location() != o->location) {
              /* Output already known, but filenames do not match */
              remove(g->get_path_for_name(o->location));
            }
      
            replace_output(o, *(*i));
          }
      
          if (!boost::filesystem::exists(g->get_path_for_name((*i)->get_location()))) {
            /* TODO Signal error with exception */
            std::cerr << "Critical error, output file with name: " << (*i)->get_location() << " does not exist!" << std::endl;
          }
        }
      }
     
      if (0 < outputs.size()) {
        g->add(interface_object.lock());
      }
    }
    /* TODO Adjust status for outputs that are not produced using the new configuration */
  }

  /*
   * Prepends the project store to the argument and returns a native filesystem path
   *
   * \param[in] w a directory relative to the project store
   */
  inline std::string processor_impl::make_output_path(std::string const& w) const {
    using namespace boost::filesystem;

    boost::shared_ptr < project_manager > g(manager.lock());

    if (g.get()) {
      path output_path(g->get_project_store());
    
      if (!output_directory.empty()) {
        output_path /= path(output_directory);
      }

      return (output_path.native_file_string());
    }

    return (w);
  }

  /**
   * \param[in] t shared pointer to the interface object
   * \param[in] ic the input combination that is to be used
   * \param[in] l absolute path to the file that serves as main input
   * \param[in] w the path to the directory in which to run the tool
   *
   * \attention This function is non-blocking
   * \pre t.get() == this
   **/
  inline void processor_impl::configure(interface_ptr const& t, const tool::input_combination* ic, const boost::filesystem::path& l, std::string const& w) {
    using namespace boost;
    using namespace boost::filesystem;

    assert(ic != 0);

    boost::shared_ptr < project_manager > g(manager.lock());

    if (g.get()) {
      selected_input_combination = const_cast < tool::input_combination* > (ic);
     
      sip::configuration::sptr c(sip::controller::communicator::new_configuration(*selected_input_combination));
     
      c->set_output_prefix(str(format("%s%04X") % (basename(find_initial_object()->location)) % g->get_unique_count()));
     
      c->add_input(ic->m_identifier, ic->m_mime_type.as_string(), l.string());
     
      current_monitor->set_configuration(c);
     
      configure(t, w);
    }
  }

  /**
   * \param[in] t shared pointer to the interface object
   * \param[in] w the path to the directory relative to the project directory in which to run the tool
   *
   * \pre The existing configuration must contain the input object matching the selected input combination
   * \pre t->impl.get() == this
   *
   * \attention This function is non-blocking
   **/
  inline void processor_impl::configure(interface_ptr const& t, std::string const& w) {
    output_directory = w;

    global_build_system.get_tool_manager()->impl->execute(*tool_descriptor, make_output_path(w),
         boost::dynamic_pointer_cast < execution::task_monitor > (current_monitor), true);

    current_monitor->start_tool_configuration(t);
  }

  /**
   * \param[in] t shared pointer to the interface object
   * \param[in] w the path to the directory in which to run the tool
   *
   * \pre The existing configuration must contain the input object matching the selected input combination
   * \pre t.get() == this
   *
   * \attention This function is non-blocking
   **/
  inline void processor_impl::reconfigure(interface_ptr const& t, std::string const& w) {
    assert(selected_input_combination != 0);

    sip::configuration::sptr c(sip::controller::communicator::new_configuration(*selected_input_combination));

    c->set_output_prefix(current_monitor->get_configuration()->get_output_prefix());
    c->add_object(current_monitor->get_configuration()->get_object(selected_input_combination->m_identifier));

    current_monitor->set_configuration(c);

    configure(t, w);
  }

  /**
   * \param[in] t shared pointer to the interface object
   * \param[in] b whether or not to run when there are no input objects defined
   *
   * \attention This function is non-blocking
   *
   * \pre !is_active() and t.get() == this
   **/
  inline void processor_impl::run(interface_ptr const& t, bool b) {
    if (b || 0 < inputs.size()) {
      boost::shared_ptr < project_manager > g(manager);

      /* Check that dependent files exist and rebuild if this is not the case */
      BOOST_FOREACH(input_list::value_type i, inputs) {
        if (!i->present_in_store(*g)) {
          processor::sptr p(i->generator.lock());

          if (p.get() != 0) {
            /* Reschedule process operation after process p has completed */
            p->run(boost::bind(&processor_impl::run, this, t, false));

            return;
          }
          else {
            /* Should signal an error via the monitor ... */
            throw (exception::exception(exception::cannot_build, i->location));
          }
        }
      }
    
      current_monitor->start_tool_operation(t);

      global_build_system.get_tool_manager()->impl->execute(*tool_descriptor, make_output_path(output_directory),
         boost::dynamic_pointer_cast < execution::task_monitor > (current_monitor), false);
    }
    else {
      /* Signal completion to environment via monitor */
      current_monitor->signal_change(execution::process::aborted);
    }
  }

  /**
   * \param[in] t shared pointer to the interface object
   * \param[in] b whether or not to run when there are no input objects are specified
   *
   * \attention This function is non-blocking
   *
   * \pre !is_active() and t.get() == this
   **/
  inline void processor_impl::update(interface_ptr const& t, bool b) {
    if (b || 0 < inputs.size()) {
      /* Check that dependent files exist and rebuild if this is not the case */
      BOOST_FOREACH(input_list::value_type i, inputs) {
        processor::sptr p(i->generator.lock());
         
        if (p.get() != 0) {
          if (p->check_status(true)) {
            /* Reschedule process operation after process p has completed */
            p->update(boost::bind(&processor_impl::update, this, t, false));
     
            return;
          }
        }
        else {
          /* Should signal an error via the monitor ... */
          throw (exception::exception(exception::cannot_build, i->location));
        }
      }
    
      current_monitor->start_tool_operation(t);

      global_build_system.get_tool_manager()->impl->execute(*tool_descriptor, make_output_path(output_directory),
         boost::dynamic_pointer_cast < execution::task_monitor > (current_monitor), false);
    }
    else {
      /* Signal completion to environment via monitor */
      current_monitor->signal_change(execution::process::aborted);
    }
  }

  /**
   * \param[in] c the edit command to execute
   **/
  inline void processor_impl::edit(execution::command* c) {
    assert(c != 0);

    c->set_working_directory(make_output_path(output_directory));

    current_monitor->get_logger()->log(1, "executing command `" + c->argument_string() + "'\n");

    global_build_system.get_tool_manager()->impl->execute(c, boost::dynamic_pointer_cast < execution::task_monitor > (current_monitor), true);
  }

  /**
   * \param[in] c the edit command to execute
   **/
  void processor::edit(execution::command* c) {
    if (c != 0) {
      impl->edit(c);
    }
  }

  /// \endcond
}

#endif
