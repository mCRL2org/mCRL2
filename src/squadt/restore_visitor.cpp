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

#include <ticpp.h>

#include <utility/visitor.h>

#include <sip/visitors.h>

namespace squadt {

  class restore_visitor_impl : public utility::visitor< restore_visitor_impl, false > {

    protected:

      /** \brief Points to the current element */
      ticpp::Element*  tree;

    protected:

      /** \brief Default constructor */
      restore_visitor_impl();

    private:

      /** \brief Changes the currently pointed to tree (FIXME this is a temporary solution) */
      restore_visitor_impl& visit_tree(ticpp::Element*);

    public:

      /** \brief Reads from parse tree */
      restore_visitor_impl(ticpp::Element* s);

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

  class restore_visitor_impl_frontend : public restore_visitor_impl {

    private:

      /** \brief The source of input */
      ticpp::Document in;

    public:

      /** \brief Reads from stream */
      restore_visitor_impl_frontend(std::istream& s);

      /** \brief Reads from string */
      restore_visitor_impl_frontend(std::string const&);

      /** \brief Reads from file */
      restore_visitor_impl_frontend(boost::filesystem::path const&);
  };

  inline restore_visitor_impl::restore_visitor_impl() {
  }

  inline restore_visitor_impl::restore_visitor_impl(ticpp::Element* s) : tree(s) {
  }

  inline restore_visitor_impl& restore_visitor_impl::visit_tree(ticpp::Element* s) {
    tree = s;

    return (*this);
  }

  inline restore_visitor_impl_frontend::restore_visitor_impl_frontend(std::istream& s) {
    s >> in;

    tree = in.FirstChildElement();
  }

  inline restore_visitor_impl_frontend::restore_visitor_impl_frontend(std::string const& s) {
    std::istringstream ins(s);

    ins >> in;

    tree = in.FirstChildElement();
  }

  inline restore_visitor_impl_frontend::restore_visitor_impl_frontend(boost::filesystem::path const& p) {
    std::ifstream ins(p.native_file_string().c_str());

    ins >> in;

    tree = in.FirstChildElement();
  }

  template < >
  restore_visitor::restore_visitor(ticpp::Element& s) :
        utility::visitor_interface< restore_visitor_impl, false >(boost::shared_ptr < visitor_type > (new restore_visitor_impl(&s))) {
  }

  /**
   * \param[in] p a string from which to read
   **/
  restore_visitor::restore_visitor(std::string const& s) :
        utility::visitor_interface< restore_visitor_impl, false >(boost::shared_ptr < visitor_type > (new restore_visitor_impl_frontend(s))) {
  }

  /**
   * \param[in] p a path to the file from which to read
   **/
  restore_visitor::restore_visitor(boost::filesystem::path const& p) :
        utility::visitor_interface< restore_visitor_impl, false >(boost::shared_ptr < visitor_type > (new restore_visitor_impl_frontend(p))) {
  }

  template <>
  void restore_visitor_impl::visit(tool& t) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "tool");

    tree->GetAttribute("name", &t.m_name);
    tree->GetAttribute("location", &t.m_location);

    if (!tree->NoChildren()) {
      t.m_capabilities.reset(new sip::tool::capabilities);

      try {
        tree->FirstChildElement("capabilities");

//        sip::visitors::restore(*t.m_capabilities, in);
      }
      catch (...) {
      }
    }
  }

  template <>
  void restore_visitor_impl::visit(tool_manager& t) {
    t.impl->accept(*this);
  }

  template <>
  void restore_visitor_impl::visit(tool_manager_impl& tm) {
    using namespace boost::filesystem;

    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "tool-catalog");

    for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
      tool new_tool;

      visit_tree(e).visit(new_tool);

      tm.add_tool(new_tool);
    }
  }

  template <>
  void restore_visitor_impl::visit(executor& e) {
    e.impl->accept(*this);
  }

  template <>
  void restore_visitor_impl::visit(executor_impl& o) {
    /** FIXME temporary measure until xml2pp is phased out */
    if (tree->Value() == "squadt-preferences") {
      tree = tree->FirstChildElement();
    }

    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "execution-settings");

    if (tree->Value() == "execution-settings") {
      unsigned int maximum_instance_count = 3;

      for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
        if (e->Value() == "concurrency-constraints") {
          try {
            e->GetAttribute("maximum-process-total", &maximum_instance_count);

            o.set_maximum_instance_count(maximum_instance_count);
          }
          catch (...) {
          }
        }
      }
    }

    tree = tree->NextSiblingElement(false);
  }

  template <>
  void restore_visitor_impl::visit(type_registry& r) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "default-actions");

    if (tree->Value() == "default-actions") {
      for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
        if (e->Value() == "associate-commands") {
          mime_type t(e->GetAttributeValue("mime-type"));

          ticpp::Element* command_element = e->FirstChildElement(false);
    
          if (command_element != 0 && command_element->Value() == "command") {
            if (command_element->NoChildren()) {
              r.register_command(t, type_registry::command_system);
            }
            else {
              r.register_command(t, command_element->GetText());
            }
          }
          else {
            r.register_command(t, type_registry::command_none);
          }
        }
      }
    }
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
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "processor");

    try {
      p.tool_descriptor            = global_build_system.get_tool_manager()->
                                       get_tool_by_name(tree->GetAttributeValue("tool-name"));
      p.selected_input_combination = p.tool_descriptor->find_input_combination(
                                       sip::tool::category::fit(tree->GetAttributeValue("category")),
                                       sip::mime_type(tree->GetAttributeValue("format")));
    }
    catch (...) {
    }

    tree->GetAttribute("output-directory", &p.output_directory, false);

    for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
      if (e->Value() == "input") {
        /* Read input */
        unsigned long id;

        e->GetAttribute("id", &id);

        assert(m.find(id) != m.end());
       
        p.inputs.push_back(processor::object_descriptor::sptr(m[id]));
      }
      else if (e->Value() == "output") {
        /* Read output */
        unsigned long id;
       
        e->GetAttribute("id", &id);

        assert(m.find(id) == m.end());
       
        m[id] = processor::object_descriptor::sptr(new processor::object_descriptor(sip::mime_type(e->GetAttributeValue("format"))));
        
        p.outputs.push_back(m[id]);
       
        processor::object_descriptor& new_descriptor = *m[id];
        
        e->GetAttribute("location", &new_descriptor.location);
        e->GetAttributeOrDefault("status", &new_descriptor.status, processor::object_descriptor::original);
        e->GetAttribute("identifier", &new_descriptor.identifier);
        e->GetAttribute("timestamp", &new_descriptor.timestamp);
        
        if (new_descriptor.status == processor::object_descriptor::generation_in_progress) {
          new_descriptor.status = processor::object_descriptor::reproducible_out_of_date;
        }
        
        e->GetAttributeOrDefault("digest", &new_descriptor.checksum, md5pp::zero_digest);

        new_descriptor.generator = boost::weak_ptr < processor > (p.interface_object);
      }
      else if (e->Value() == "configuration") {
        boost::shared_ptr < sip::configuration > new_configuration(new sip::configuration);

//        sip::visitors::restore(*new_configuration, in);

        p.current_monitor->set_configuration(new_configuration);
      }
    }
  }

  template <>
  void restore_visitor_impl::visit(squadt::project_manager& p) {
    p.impl->accept(*this);
  }

  template <>
  void restore_visitor_impl::visit(squadt::project_manager_impl& p) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "squadt-project");

    tree->GetAttribute("count", &p.count);

    id_conversion_map cmap;

    p.processors.clear();

    for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
      if (e->Value() == "processor") {
        processor::sptr new_processor(processor::create(p.m_interface));
       
        new_processor->accept(visit_tree(e), cmap);

        new_processor->check_status(true);
       
        p.processors.push_back(new_processor);
      }
      else if (e->Value() == "description") {
        e->GetText(&p.description, false);
      }
    }

    p.sort_processors();
  }
}
