// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file restore_visitor.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

#include <fstream>

#include <boost/thread.hpp> // workaround for boost::thread that includes errno.h (stlport)
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/foreach.hpp>

#include "tipi/detail/utility/generic_visitor.hpp"
#include "tipi/detail/visitors.hpp"

#include "tool_manager.ipp"
#include "project_manager.ipp"
#include "settings_manager.hpp"
#include "build_system.hpp"
#include "type_registry.hpp"
#include "executor.ipp"
#include "processor.ipp"

#include "visitors.hpp"

#include "ticpp.h"

namespace squadt {

  /// \cond INTERNAL_DOCS
  class restore_visitor_impl {

    protected:

      typedef std::map < unsigned long, boost::shared_ptr < processor_impl::object_descriptor > > id_conversion_map;

      struct id_helper {
        id_conversion_map  cmap;
        ticpp::Element*    tree;

        inline id_helper(ticpp::Element* t) : tree(t) {
        }
      };

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
    std::ostringstream l;

    l << s.rdbuf();

    in.Parse(l.str());

    tree = in.FirstChildElement();
  }

  inline restore_visitor_impl_frontend::restore_visitor_impl_frontend(std::string const& s) {
    in.Parse(s);

    tree = in.FirstChildElement();
  }

  inline restore_visitor_impl_frontend::restore_visitor_impl_frontend(boost::filesystem::path const& p) {
    in.LoadFile(p.native_file_string().c_str());

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
    if(!((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "tool")){
      throw std::runtime_error("Expected XML tree value \"tool\"");
    } 

    std::string location;

    tree->GetAttribute("name", &t.m_name);
    tree->GetAttribute("location", &location);

    t.set_location(location);

    if (!tree->NoChildren()) {
      t.m_capabilities.reset(new tipi::tool::capabilities);

      try {
        tree->FirstChildElement("capabilities");

        tipi::visitors::restore(*t.m_capabilities, *tree);
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

  /// \cond INTERNAL_DOCS
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(tool_manager_impl& tm) {
    using namespace boost::filesystem;

    if(!((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "tool-catalog")){
      throw std::runtime_error("Expected XML tree value \"tool-catalog\"");
    } 

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

  /// \cond INTERNAL_DOCS
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(executor_impl& o) {
    if (tree->Value() == "squadt-preferences") {
      tree = tree->FirstChildElement();
    }

    if(!((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "execution-settings")){
      throw std::runtime_error("Expected XML tree value \"execution-settings\"");
    } 

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
    if(!((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "default-actions")){
      throw std::runtime_error("Expected XML tree value \"default-actions\"");
    } 

    if (tree->Value() == "default-actions") {
      for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
        if (e->Value() == "associate-commands") {
          mime_type t(e->GetAttribute("mime-type"));

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

  /**
   * \param[in] p the processor object to restore
   * \param[in] h a helper map for resolving getting a processor by a unique identifier
   **/
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(processor& p, id_helper& h) {
    do_visit(*p.impl, h);
  }

  /// \cond INTERNAL_DOCS
  /**
   * \attention the same map m must be used to read back all processor instances that were written with write()
   **/
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(processor_impl& p, id_helper& h) {
    if(!((h.tree->Type() == TiXmlNode::ELEMENT) && h.tree->Value() == "processor")){
      throw std::runtime_error("Expected XML tree value \"processor\"");
    } 

    try {
      p.tool_descriptor            = global_build_system.get_tool_manager().
                                       get_tool_by_name(h.tree->GetAttribute("tool-name"));
      p.selected_input_configuration = p.tool_descriptor->find_input_configuration(
                                       tipi::tool::category::match(h.tree->GetAttribute("category")),
                                       tipi::mime_type(h.tree->GetAttribute("format")));
    }
    catch (...) {
    }

    h.tree->GetAttribute("output-directory", &p.output_directory, false);

    for (ticpp::Element* e = h.tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
      tipi::configuration::parameter::identifier tipi_id;

      if (e->Value() == "input") {
        /* Read input */
        unsigned long id;

        e->GetAttribute("id", &id);

        if (h.cmap.find(id) == h.cmap.end()) {
          throw std::runtime_error("Parse error! Reference to unknown object.");
        }

        e->GetAttribute("identifier", &tipi_id);

        p.register_input(tipi_id, h.cmap[id]);
      }
      else if (e->Value() == "output") {
        /* Read output */
        unsigned long id;

        e->GetAttribute("id", &id);

        if(!(h.cmap.find(id) == h.cmap.end())){
           throw std::runtime_error("Cannot find ID in mapping");
        };

        h.cmap[id] = boost::shared_ptr < processor_impl::object_descriptor >(
                new processor_impl::object_descriptor(p.interface_object, p.manager, tipi::mime_type(e->GetAttribute("format")), tipi::uri(e->GetAttribute("location"))));

        processor_impl::object_descriptor& new_descriptor(*h.cmap[id]);

        e->GetAttribute("timestamp", &new_descriptor.timestamp);
        e->GetAttributeOrDefault("status", &new_descriptor.status, processor::object_descriptor::original);

        if (new_descriptor.status != processor::object_descriptor::original) {
          e->GetAttribute("identifier", &tipi_id);
        }

        if (new_descriptor.status == processor::object_descriptor::generation_in_progress) {
          new_descriptor.status = processor::object_descriptor::reproducible_out_of_date;
        }

        e->GetAttribute("digest", &new_descriptor.checksum, false);

        p.register_output(tipi_id, h.cmap[id]);
      }
      else if (e->Value() == "configuration") {
        boost::shared_ptr < tipi::configuration > new_configuration(new tipi::configuration);

        tipi::visitors::restore(*new_configuration, *e);

        p.current_monitor->set_configuration(new_configuration);
      }
    }

    // Try reconstructing the selected input combination
    if (p.tool_descriptor && p.selected_input_configuration == 0) {
      if (0 < p.inputs.size()) {
        // Take the first input as main input and try to find a combination
        p.selected_input_configuration = p.tool_descriptor->find_input_configuration(
        p.current_monitor->get_configuration()->category(), p.inputs[0].object->get_format());
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

  /// \cond INTERNAL_DOCS
  template <>
  template <>
  void visitor< squadt::restore_visitor_impl >::visit(squadt::project_manager_impl& p) {
    if(!((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "squadt-project")){
      throw std::runtime_error("Expected XML tree value \"squadt-project\"");
    } 

    tree->GetAttribute("count", &p.count);

    p.processors.clear();

    for (id_helper h(tree->FirstChildElement(false)); h.tree != 0; h.tree = h.tree->NextSiblingElement(false)) {
      if (h.tree->Value() == "processor") {
        boost::shared_ptr < processor > new_processor(processor::create(p.m_interface));

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

  /// \cond INTERNAL_DOCS
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
