#include "visitors.h"

#include <fstream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/foreach.hpp>

// necessary because utility/visitor.tcc will be included
#include <sip/detail/restore_visitor.tcc>

#include "build_system.h"
#include "settings_manager.h"
#include "tool_manager.tcc"
#include "project_manager.tcc"
#include "type_registry.h"
#include "executor.tcc"
#include "processor.tcc"

#include <xml2pp/text_reader.h>

#include <utility/visitor.h>

#include <sip/visitors.h>

namespace squadt {

  class restore_visitor_impl : public utility::visitor< restore_visitor_impl, false > {

    private:

      xml2pp::text_reader& in;

    public:

      restore_visitor_impl(xml2pp::text_reader& s);

      /** \brief Reads state for objects of type T */
      template < typename T >
      void visit(T&);

      /** \brief Reads state for objects of type T */
      template < typename T, typename U >
      void visit(T&, U&);
  };
}

#include <utility/visitor.tcc>

namespace utility {
  template void visitor< squadt::restore_visitor_impl, false >::visit(squadt::tool_manager&);
  template void visitor< squadt::restore_visitor_impl, false >::visit(squadt::project_manager&);
  template void visitor< squadt::restore_visitor_impl, false >::visit(squadt::project_manager_impl&);
  template void visitor< squadt::restore_visitor_impl, false >::visit(squadt::executor&);
  template void visitor< squadt::restore_visitor_impl, false >::visit(squadt::type_registry&);
}

namespace squadt {

  class restore_visitor_alternate_impl : public restore_visitor_impl {

    private:

      /** \brief The source of input */
      xml2pp::text_reader in;

    public:

      /** \brief Reads from string */
      restore_visitor_alternate_impl(std::string const&);

      /** \brief Reads from file */
      restore_visitor_alternate_impl(boost::filesystem::path const&);
  };

  inline restore_visitor_impl::restore_visitor_impl(xml2pp::text_reader& s) : in(s) {
  }

  inline restore_visitor_alternate_impl::restore_visitor_alternate_impl(std::string const& s) : restore_visitor_impl(in), in(s) {
  }

  inline restore_visitor_alternate_impl::restore_visitor_alternate_impl(boost::filesystem::path const& p) : restore_visitor_impl(in), in(p) {
  }

  template < >
  restore_visitor::restore_visitor(xml2pp::text_reader& s) :
        utility::visitor_interface< restore_visitor_impl, false >(boost::shared_ptr < visitor_type > (new restore_visitor_impl(s))) {
  }

  /**
   * \param[in] p a string from which to read
   **/
  restore_visitor::restore_visitor(std::string const& s) :
        utility::visitor_interface< restore_visitor_impl, false >(boost::shared_ptr < visitor_type > (new restore_visitor_alternate_impl(s))) {
  }

  /**
   * \param[in] p a path to the file from which to read
   **/
  restore_visitor::restore_visitor(boost::filesystem::path const& p) :
        utility::visitor_interface< restore_visitor_impl, false >(boost::shared_ptr < visitor_type > (new restore_visitor_alternate_impl(p))) {
  }

  template <>
  void restore_visitor_impl::visit(tool& t) {
    if (!(in.get_attribute(&t.m_name, "name") && in.get_attribute(&t.m_location, "location"))) {
      throw (exception::exception(exception::required_attributes_missing, "tool"));
    }

    if (!in.is_end_element()) {
      t.m_capabilities.reset(new sip::tool::capabilities);

      in.next_element();

      sip::visitors::restore(*t.m_capabilities, in);
    }

    in.skip_end_element("tool");
  }

  template <>
  void restore_visitor_impl::visit(tool_manager& t) {
    t.impl->accept(*this);
  }

  template <>
  void restore_visitor_impl::visit(tool_manager_impl& tm) {
    using namespace boost::filesystem;

    assert(in.is_element("tool-catalog"));

    in.next_element();

    while (!in.is_end_element("tool-catalog")) {
      /* Add a new tool to the list of tools */
      tool new_tool;

      new_tool.accept(*this);

      tm.add_tool(new_tool);
    }
  }

  template <>
  void restore_visitor_impl::visit(executor& e) {
    e.impl->accept(*this);
  }

  template <>
  void restore_visitor_impl::visit(executor_impl& e) {
    /** FIXME temporary measure until xml2pp is phased out */
    if (in.is_element("squadt-preferences")) {
      in.next_element();
    }

    assert(in.is_element("execution-settings"));

    if (in.is_element("execution-settings")) {
      unsigned int maximum_instance_count = 3;
     
      while (!in.is_end_element("execution-settings")) {
        in.next_element();
      
        if (in.is_element("concurrency-constraints")) {
          if (in.get_attribute(&maximum_instance_count, "maximum-process-total")) {
            e.set_maximum_instance_count(maximum_instance_count);
          }
        }
      }
     
      in.skip_end_element("execution-settings");
    }
  }

  template <>
  void restore_visitor_impl::visit(type_registry& r) {
    assert(in.is_element("default-actions"));

    while (!in.is_end_element("default-actions")) {
      in.next_element();

      if (in.is_element("associate-commands")) {
        while (!in.is_end_element("associate-commands")) {
          mime_type t(in.get_attribute_as_string("mime-type"));

          in.next_element();

          if (in.is_element("command")) {
            if (in.is_end_element() || in.is_empty_element()) {
              r.register_command(t, type_registry::command_system);
            }
            else {
              in.next_element();

              r.register_command(t, in.get_value_as_string());

              in.next_element();
            }

            in.skip_end_element("command");
          }
          else {
            in.skip_end_element("no-command");
          }

          in.skip_end_element("associate_commands");
        }
      }
    }

    in.skip_end_element("default-actions");
  }

  typedef std::map < unsigned long, processor::object_descriptor::sptr > id_conversion_map;

  template <>
  void restore_visitor_impl::visit(processor& p, id_conversion_map& m) {
    p.impl->accept(*this, m);
  }

  /**
   * \attention the same map m must be used to read back all processor instances that were written with write()
   **/
  template <>
  void restore_visitor_impl::visit(processor_impl& p, id_conversion_map& m) {
    std::string tool_name = in.get_attribute_as_string("tool-name");

    if (!tool_name.empty()) {
      p.tool_descriptor = global_build_system.get_tool_manager()->get_tool_by_name(tool_name);

      std::string format   = in.get_attribute_as_string("category");
      std::string category = in.get_attribute_as_string("format");

      if (!(format.empty() || category.empty())) {
        p.selected_input_combination = p.tool_descriptor->find_input_combination(
                      sip::tool::category::fit(category), sip::mime_type(format));
      }
    }

    p.output_directory = in.get_attribute_as_string("output-directory");

    in.next_element();

    if (in.is_element("configuration")) {
      boost::shared_ptr < sip::configuration > new_configuration(new sip::configuration);

      sip::visitors::restore(*new_configuration, in);

      p.current_monitor->set_configuration(new_configuration);
    }

    /* Read inputs */
    while (in.is_element("input")) {
      unsigned long id;

      if (!in.get_attribute(&id, "id")) {
        throw (exception::exception(exception::required_attributes_missing, "processor->input"));
      }
      else {
        assert(m.find(id) != m.end());

        p.inputs.push_back(processor::object_descriptor::sptr(m[id]));
      }

      in.next_element();

      in.skip_end_element("input");
    }

    /* Read outputs */
    while (in.is_element("output")) {
      unsigned long id;
      bool          b = in.get_attribute(&id, "id");

      if (b) {
        std::string format = in.get_attribute_as_string("format");

        if (!format.empty()) {
          assert(m.find(id) == m.end());

          m[id] = processor::object_descriptor::sptr(new processor::object_descriptor(sip::mime_type(format)));
       
          p.outputs.push_back(m[id]);
        }
        else {
          throw (exception::exception(exception::required_attributes_missing, "processor->output"));
        }
      }

      processor::object_descriptor& new_descriptor = *m[id];

      if (!(b && in.get_attribute(&new_descriptor.location, "location")
              && in.get_attribute(&id, "status"))
              && (in.get_attribute(&new_descriptor.identifier, "identifier") || id == processor::object_descriptor::original)
              && in.get_attribute(&new_descriptor.timestamp, "timestamp")) {

        throw (exception::exception(exception::required_attributes_missing, "processor->output"));
      }

      new_descriptor.status = static_cast < processor_impl::object_status > ((id == processor::object_descriptor::generation_in_progress) ?
                                                      processor::object_descriptor::reproducible_nonexistent : id);

      std::string digest = in.get_attribute_as_string("digest");

      if (digest.empty()) {
        new_descriptor.checksum = md5pp::zero_digest;
      }
      else {
        new_descriptor.checksum.read(digest);
      }

      new_descriptor.generator = boost::weak_ptr < processor > (p.interface_object);

      in.next_element();

      in.skip_end_element("output");
    }

    in.skip_end_element("processor");
  }

  template <>
  void restore_visitor_impl::visit(squadt::project_manager& p) {
    p.impl->accept(*this);
  }

  template <>
  void restore_visitor_impl::visit(squadt::project_manager_impl& p) {
    if (!in.is_element("squadt-project")) {
      throw (exception::exception(exception::failed_loading_object, "SQuADT project", "expected squadt-project element"));
    }

    in.get_attribute(&p.count, "count");

    /* Advance beyond project element */
    in.next_element();

    if (in.is_element("description") && !in.is_empty_element()) {
      in.next_element();

      in.get_value(&p.description);

      in.next_element(1);
    }

    id_conversion_map cmap;

    p.processors.clear();

    /* Read processors */
    while (in.is_element("processor")) {
      processor::sptr new_processor(processor::create(p.m_interface));

      new_processor->accept(*this, cmap);
      
      new_processor->check_status(true);

      p.processors.push_back(new_processor);
    }

    p.sort_processors();
  }
}
