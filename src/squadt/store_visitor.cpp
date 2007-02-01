#include "visitors.h"

#include <fstream>

#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

// necessary because utility/visitor.tcc will be included
#include <sip/detail/store_visitor.tcc>

#include "build_system.h"
#include "settings_manager.h"
#include "project_manager.tcc"
#include "tool_manager.tcc"
#include "type_registry.h"
#include "executor.tcc"
#include "processor.tcc"

#include <utility/visitor.h>

namespace squadt {

  class store_visitor_impl : public utility::visitor< store_visitor_impl > {

    friend class visitors;
    friend class store_visitor;

    private:

      /** \brief The destination of output */
      std::ostream&   out;

    public:

      /** \brief Writes to stream */
      store_visitor_impl(std::ostream&);

      /** \brief Writes state for objects of type T */
      template < typename T >
      void visit(T const&);

      template < typename T, typename U >
      void visit(T const&, U const&);
  };
}

#include <utility/visitor.tcc>

namespace utility {
  template void visitor< squadt::store_visitor_impl, true >::visit(squadt::tool_manager const&);
  template void visitor< squadt::store_visitor_impl, true >::visit(squadt::project_manager const&);
  template void visitor< squadt::store_visitor_impl, true >::visit(squadt::project_manager_impl const&);
  template void visitor< squadt::store_visitor_impl, true >::visit(squadt::executor const&);
  template void visitor< squadt::store_visitor_impl, true >::visit(squadt::type_registry const&);
}

namespace squadt {

  class store_visitor_path_impl : public store_visitor_impl {

    private:

      /** \brief Storage for temporary std::ostream object */
      std::ofstream    m_help_stream;

    public:

      /** \brief Writes to file */
      store_visitor_path_impl(boost::filesystem::path const&);
  };

  class store_visitor_string_impl : public store_visitor_impl {

    private:

      /** \brief Storage for temporary std::ostream object */
      std::ostringstream  m_help_stream;

      /** \brief Storage for character data */
      std::string&        m_target_string;

    public:

      /** \brief Constructor for writing to string */
      store_visitor_string_impl(std::string&);

      /** \brief Writes to file */
      ~store_visitor_string_impl();
  };

  /**
   * \param[in] s a string to write to
   **/
  store_visitor::store_visitor(std::string& s) :
        utility::visitor_interface< store_visitor_impl >(boost::shared_ptr < visitor_type > (new store_visitor_string_impl(s))) {
  }

  /**
   * \param[in] p a path to the file to write to
   **/
  store_visitor::store_visitor(boost::filesystem::path const& p) :
        utility::visitor_interface< store_visitor_impl >(boost::shared_ptr < visitor_type > (new store_visitor_path_impl(p))) {
  }

  /**
   * \param[in] s a stream to write to
   **/
  store_visitor::store_visitor(std::ostream& o) :
        utility::visitor_interface< store_visitor_impl >(boost::shared_ptr < visitor_type > (new store_visitor_impl(o))) {
  }

  inline store_visitor_string_impl::store_visitor_string_impl(std::string& s) :
                                store_visitor_impl(m_help_stream),
                                m_target_string(s) {
  }

  inline store_visitor_string_impl::~store_visitor_string_impl() {
    m_target_string.assign(m_help_stream.str());
  }

  /**
   * \param[in] p a path to the file from which to read
   **/
  inline store_visitor_path_impl::store_visitor_path_impl(boost::filesystem::path const& p) :
                                store_visitor_impl(m_help_stream),
                                m_help_stream(p.string().c_str(), std::ios_base::out) {

  }

  inline store_visitor_impl::store_visitor_impl(std::ostream& o) : out(o) {
  }

  template <>
  void store_visitor_impl::visit(tool const& t) {
    out << "<tool name=\"" << t.get_name()
                    << "\" location=\"" << t.get_location() << "\"/>\n";
  }

  template <>
  void store_visitor_impl::visit(tool_manager const& t) {
    t.impl->accept(*this);
  }

  template <>
  void store_visitor_impl::visit(tool_manager_impl const& tm) {
    /* Write header */
    out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                    << "<tool-catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
                    << " xsi:noNamespaceSchemaLocation=\"tool_catalog.xsd\" version=\"1.0\">\n";
 
    BOOST_FOREACH(tool_manager::tool_list::value_type t, tm.get_tools()) {
      t->accept(*this);
    }
 
    /* Write footer */
    out << "</tool-catalog>\n";
  }

  template <>
  void store_visitor_impl::visit(execution::executor const& t) {
    t.impl->accept(*this);
  }

  template <>
  void store_visitor_impl::visit(executor_impl const& e) {
    /** FIXME temporary measure until xml2pp is phased out */
    out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        << "<squadt-preferences xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" version=\"1.0\">\n";

    out << "<execution-settings>\n"
                    << " <concurrency-constraints maximum-process-total=\""
                    << e.get_maximum_instance_count() << "\"/>\n"
                    << "</execution-settings>\n";
  }

  template <>
  void store_visitor_impl::visit(type_registry const& r) {

    out << "<default-actions>\n";

    BOOST_FOREACH(type_registry::actions_for_type::value_type c, r.command_for_type) {
      out << " <associate-commands mime-type=\"" << c.first << "\">\n";
      if (c.second == type_registry::command_system) {
        /// Associated command is specified by the system
        out << "  <command /";
      }
      else if (c.second == type_registry::command_none) {
        /// No command is to be associated with this type
        out << "  <no-command /";
      }
      else {
        /// A custom command is associated with this type
        out << "  <command><![CDATA[" << c.second << "]]></command";
      }

      out << ">\n </associate-commands>\n";
    }

    out << "</default-actions>\n";

    /** FIXME temporary measure until xml2pp is phased out */
    out << "</squadt-preferences>";
  }

  template <>
  void store_visitor_impl::visit(processor const& p) {
    p.impl->accept(*this);
  }

  template <>
  void store_visitor_impl::visit(processor_impl const& p) {
    out << "<processor";

    if (p.tool_descriptor.get() != 0) {
      out << " tool-name=\"" << p.tool_descriptor->get_name() << "\"";

      if (p.selected_input_combination != 0) {
        out << " format=\"" << p.selected_input_combination->m_mime_type << "\"";
        out << " category=\"" << p.selected_input_combination->m_category << "\"";
      }
    }

    if (!p.output_directory.empty()) {
      out << " output-directory=\"" << p.output_directory << "\">";
    }
    else {
      out << ">";
    }

    /* The last received configuration from the tool */
    boost::shared_ptr < sip::configuration > c = p.current_monitor->get_configuration();

    if (c.get() != 0) {
      sip::visitors::store(*c, out);
    }

    /* The inputs */
    for (processor::input_list::const_iterator i = p.inputs.begin(); i != p.inputs.end(); ++i) {
      out << "<input id=\"" << std::dec << reinterpret_cast < unsigned long > ((*i).get()) << "\"/>\n";
    }

    /* The outputs */
    for (processor::output_list::const_iterator i = p.outputs.begin(); i != p.outputs.end(); ++i) {
      out << "<output id=\"" << std::dec << reinterpret_cast < unsigned long > ((*i).get())
        << "\" format=\"" << (*i)->mime_type
        << "\" location=\"" << (*i)->location;

      if ((*i)->status != processor::object_descriptor::original) {
        out << "\" identifier=\"" << std::dec << (*i)->identifier;
      }

      out << "\" status=\"" << (*i)->status;

      if (!(*i)->checksum.is_zero()) {
        out << "\" digest=\"" << (*i)->checksum;
      }

      out << "\" timestamp=\"" << std::dec << (*i)->timestamp << "\"/>\n";
    }

    out << "</processor>\n";
  }

  template <>
  void store_visitor_impl::visit(squadt::project_manager const& p) {
    p.impl->accept(*this);
  }

  template <>
  void store_visitor_impl::visit(squadt::project_manager_impl const& p) {
    /* Write header */
    out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
      << "<squadt-project xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
      << " xsi:noNamespaceSchemaLocation=\"project.xsd\" version=\"1.0\" count=\""
      << p.count << "\">\n";

    if (!p.description.empty()) {
      out << "<description>" << p.description << "</description>\n";
    }
 
    BOOST_FOREACH(project_manager::processor_list::value_type i, p.processors) {
      i->accept(*this);
    }
 
    out << "</squadt-project>\n";
  }
}

