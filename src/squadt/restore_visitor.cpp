#include <fstream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/foreach.hpp>

#include "build_system.h"
#include "settings_manager.h"
#include "tool_manager.tcc"
#include "project_manager.tcc"
#include "type_registry.h"
#include "executor.tcc"
#include "processor.tcc"

#include <utilities/generic_visitor.tcc>

#include "visitors.h"

#include <sip/visitors.h>

#include <ticpp.h>

namespace squadt {

  /// \cond PRIVATE_PART
  class restore_visitor_impl {

    protected:

      /** \brief Points to the current element */
      ticpp::Element*  tree;

    public:

      restore_visitor_impl(ticpp::Element* t) : tree(t) { }

      restore_visitor_impl() { }
  };

  class restore_visitor_impl_frontend : public utility::visitor < restore_visitor_impl > {

    private:

      /** \brief The source of input */
      ticpp::Document in;

    public:

      /** \brief Reads from parse tree */
      restore_visitor_impl_frontend(ticpp::Element* s);

      /** \brief Reads from stream */
      restore_visitor_impl_frontend(std::istream& s);

      /** \brief Reads from string */
      restore_visitor_impl_frontend(std::string const&);

      /** \brief Reads from file */
      restore_visitor_impl_frontend(boost::filesystem::path const&);
  };

  inline restore_visitor_impl_frontend::restore_visitor_impl_frontend(ticpp::Element* s) {
    tree = s;
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
  /// \endcond

  /**
   * \brief Constructor for reading directly from parse tree
   * \param[in] s parse tree from which to read
   **/
  template < >
  restore_visitor::restore_visitor(ticpp::Element& s) :
        utility::visitor_interface< squadt::restore_visitor_impl >(
                boost::shared_ptr < utility::visitor< squadt::restore_visitor_impl > >(new restore_visitor_impl_frontend(&s))) {
  }

  /**
   * \param[in] s a string from which to read
   **/
  restore_visitor::restore_visitor(std::string const& s) :
        utility::visitor_interface< squadt::restore_visitor_impl >(
                boost::shared_ptr < utility::visitor< squadt::restore_visitor_impl > >(new restore_visitor_impl_frontend(s))) {
  }

  /**
   * \param[in] p a path to the file from which to read
   **/
  restore_visitor::restore_visitor(boost::filesystem::path const& p) :
        utility::visitor_interface< restore_visitor_impl >(
                boost::shared_ptr < utility::visitor< squadt::restore_visitor_impl > > (new restore_visitor_impl_frontend(p))) {
  }
}

namespace utility {
  using namespace squadt;

  /**
   * \param[in] t the tool object to restore
   **/
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(tool& t) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "tool");

    tree->GetAttribute("name", &t.m_name);
    tree->GetAttribute("location", &t.m_location);

    if (!tree->NoChildren()) {
      t.m_capabilities.reset(new sip::tool::capabilities);

      try {
        tree->FirstChildElement("capabilities");

        sip::visitors::restore(*t.m_capabilities, *tree);
      }
      catch (...) {
      }
    }
  }

  /**
   * \param[in] t the tool_manager object to restore
   **/
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(tool_manager& t) {
    do_visit(*t.impl);
  }

  /// \cond PRIVATE_PART
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(tool_manager_impl& tm) {
    using namespace boost::filesystem;

    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "tool-catalog");

    for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
      tool new_tool;

      visitor< squadt::restore_visitor_impl >(e).visit(new_tool);

      tm.add_tool(new_tool);
    }
  }
  /// \endcond

  /**
   * \param[in] e the executor object to restore
   **/
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(executor& e) {
    do_visit(*e.impl);
  }

  /// \cond PRIVATE_PART
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(executor_impl& o) {
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
  /// \endcond

  /**
   * \param[in] r the type_registry object to restore
   **/
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(type_registry& r) {
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

  /// \cond PRIVATE_PART
  typedef std::map < unsigned long, processor::object_descriptor::sptr > id_conversion_map;

  struct id_helper {
    id_conversion_map  cmap;
    ticpp::Element*    tree;

    inline id_helper(ticpp::Element* t) : tree(t) {
    }
  };
  /// \endcond

  /**
   * \param[in] p the processor object to restore
   * \param[in] h a helper map for resolving getting a processor by a unique identifier
   **/
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(processor& p, id_helper& h) {
    do_visit(*p.impl, h);
  }

  /// \cond PRIVATE_PART
  /**
   * \attention the same map m must be used to read back all processor instances that were written with write()
   **/
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(processor_impl& p, id_helper& h) {
    assert((h.tree->Type() == TiXmlNode::ELEMENT) && h.tree->Value() == "processor");

    try {
      p.tool_descriptor            = global_build_system.get_tool_manager()->
                                       get_tool_by_name(h.tree->GetAttributeValue("tool-name"));
      p.selected_input_combination = p.tool_descriptor->find_input_combination(
                                       sip::tool::category::fit(h.tree->GetAttributeValue("category")),
                                       sip::mime_type(h.tree->GetAttributeValue("format")));
    }
    catch (...) {
    }

    h.tree->GetAttribute("output-directory", &p.output_directory, false);

    for (ticpp::Element* e = h.tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
      if (e->Value() == "input") {
        /* Read input */
        unsigned long id;

        e->GetAttribute("id", &id);

        assert(h.cmap.find(id) != h.cmap.end());
       
        p.inputs.push_back(processor::object_descriptor::sptr(h.cmap[id]));
      }
      else if (e->Value() == "output") {
        /* Read output */
        unsigned long id;
       
        e->GetAttribute("id", &id);

        assert(h.cmap.find(id) == h.cmap.end());
       
        h.cmap[id] = processor::object_descriptor::sptr(new processor::object_descriptor(sip::mime_type(e->GetAttributeValue("format"))));
        
        p.outputs.push_back(h.cmap[id]);
       
        processor::object_descriptor& new_descriptor = *h.cmap[id];
        
        e->GetAttribute("location", &new_descriptor.location);
        e->GetAttribute("timestamp", &new_descriptor.timestamp);
        e->GetAttributeOrDefault("status", &new_descriptor.status, processor::object_descriptor::original);

        if (new_descriptor.status != processor::object_descriptor::original) {
          e->GetAttribute("identifier", &new_descriptor.identifier);
        }
        
        if (new_descriptor.status == processor::object_descriptor::generation_in_progress) {
          new_descriptor.status = processor::object_descriptor::reproducible_out_of_date;
        }
        
        e->GetAttributeOrDefault("digest", &new_descriptor.checksum, md5pp::zero_digest);

        new_descriptor.generator = boost::weak_ptr < processor > (p.interface_object);
      }
      else if (e->Value() == "configuration") {
        boost::shared_ptr < sip::configuration > new_configuration(new sip::configuration);

        sip::visitors::restore(*new_configuration, *e);

        p.current_monitor->set_configuration(new_configuration);
      }
    }
  }
  /// \endcond

  /**
   * \param[in] p the project_manager object to restore
   **/
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(squadt::project_manager& p) {
    do_visit(*p.impl);
  }

  /// \cond PRIVATE_PART
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(squadt::project_manager_impl& p) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "squadt-project");

    tree->GetAttribute("count", &p.count);

    p.processors.clear();

    for (id_helper h(tree->FirstChildElement(false)); h.tree != 0; h.tree = h.tree->NextSiblingElement(false)) {
      if (h.tree->Value() == "processor") {
        processor::sptr new_processor(processor::create(p.m_interface));
       
        visit(*new_processor, h);

        new_processor->check_status(true);
       
        p.processors.push_back(new_processor);
      }
      else if (h.tree->Value() == "description") {
        h.tree->GetText(&p.description, false);
      }
    }

    p.sort_processors();
  }
  /// \endcond

  /// \cond PRIVATE_PART
  template <>
  bool visitor< squadt::restore_visitor_impl >::initialise() {
    register_visit_method< squadt::tool >();
    register_visit_method< squadt::tool_manager >();
    register_visit_method< squadt::tool_manager_impl >();
    register_visit_method< squadt::executor >();
    register_visit_method< squadt::executor_impl >();
    register_visit_method< squadt::processor, id_helper >();
    register_visit_method< squadt::processor_impl, id_helper >();
    register_visit_method< squadt::type_registry >();
    register_visit_method< squadt::project_manager >();
    register_visit_method< squadt::project_manager_impl >();

    return true;
  }
  /// \endcond
}
