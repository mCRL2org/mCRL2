#include <sstream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#include <sip/visitors.h>
#include <sip/report.h>
#include <sip/controller/capabilities.h>
#include <sip/tool/capabilities.h>
#include <sip/detail/layout_elements.h>
#include <sip/detail/layout_manager.h>
#include <sip/detail/layout_tool_display.h>
#include <sip/detail/store_visitor.tcc>
#include <sip/detail/common.h>

#include <utility/visitor.tcc>

template void utility::visitor< sip::store_visitor_impl, true >::visit(sip::configuration const&);
template void utility::visitor< sip::store_visitor_impl, true >::visit(sip::tool::capabilities const&);
template void utility::visitor< sip::store_visitor_impl, true >::visit(sip::controller::capabilities const&);
template void utility::visitor< sip::store_visitor_impl, true >::visit(sip::layout::tool_display const&);
template void utility::visitor< sip::store_visitor_impl, true >::visit(sip::report const&);
template void utility::visitor< sip::store_visitor_impl, true >::visit(sip::message const&);

namespace sip {

  class store_visitor_path_impl : public store_visitor_impl {

    private:

      /** \brief Storage for temporary std::ostream object */
      std::ofstream    m_help_stream;

    public:

      /** \brief Writes to file */
      store_visitor_path_impl(store_visitor&, boost::filesystem::path const&);
  };

  class store_visitor_string_impl : public store_visitor_impl {

    private:

      /** \brief Storage for temporary std::ostream object */
      std::ostringstream  m_help_stream;

      /** \brief Storage for character data */
      std::string&        m_target_string;

    public:

      /** \brief Constructor for writing to string */
      store_visitor_string_impl(store_visitor&, std::string&);

      /** \brief Writes to file */
      ~store_visitor_string_impl();
  };
  
  store_visitor::store_visitor(std::string& s) :
        utility::visitor_interface< store_visitor_impl >(boost::shared_ptr < visitor_type > (new store_visitor_string_impl(*this, s))) {
  }

  store_visitor::store_visitor(boost::filesystem::path const& p) :
        utility::visitor_interface< store_visitor_impl >(boost::shared_ptr < visitor_type > (new store_visitor_path_impl(*this, p))) {
  }

  store_visitor::store_visitor(std::ostream& o) :
        utility::visitor_interface< store_visitor_impl >(boost::shared_ptr < visitor_type > (new store_visitor_impl(*this, o))) {
  }

  inline store_visitor_string_impl::store_visitor_string_impl(store_visitor& i, std::string& s) :
                                store_visitor_impl(i, m_help_stream),
                                m_target_string(s) {
  }

  inline store_visitor_string_impl::~store_visitor_string_impl() {
    m_target_string.assign(m_help_stream.str());
  }

  inline store_visitor_path_impl::store_visitor_path_impl(store_visitor& i, boost::filesystem::path const& p) :
                                store_visitor_impl(i, m_help_stream),
                                m_help_stream(p.string().c_str(), std::ios_base::out) {

  }

  inline store_visitor_impl::store_visitor_impl(store_visitor& i, std::ostream& o) : m_interface(i), out(o) {
  }

  template <>
  void store_visitor_impl::visit(sip::message const& o) {
    out << "<message-meta type=\"" << o.m_type << "\">"
        << "<![CDATA[";

    size_t i = 0;
    size_t j = o.m_content.find("]]>", 0); 

    while (j < o.m_content.size()) {
      out << o.m_content.substr(i,j) << "]]]]><![CDATA[>";

      i = j + 3;
      j = o.m_content.find("]]>", i);
    }

    out << o.m_content.substr(i) << "]]></message-meta>";
  }

  template <>
  void store_visitor_impl::visit(object const& o) {
    if (!o.m_location.empty()) {
      out << " location=\"" << o.m_location << "\"";
    }

    out << " mime-type=\"" << o.m_mime_type << "\"";
  }

  template <>
  void store_visitor_impl::visit(option const& o) {
    using sip::exception;

    out << ">";

    if (o.takes_arguments()) {
      BOOST_FOREACH(option::type_value_list::value_type i, o.m_arguments) {
        try {
          m_interface.do_accept(*i.first, i.second);
        }
        catch (exception e) {
          /* Invalid datatype exception; substitute context */
          e.message() % boost::str(boost::format("option -> argument %u") % i.second);
        }
      }
    }
  }

  /**
   * \param[in,out] o the stream to which the output is written
   **/
  template <>
  void store_visitor_impl::visit(sip::configuration const& c) {
    out << "<configuration";

    if (c.m_fresh) {
      out << " fresh=\"true\"";
    }

    if (!c.m_output_prefix.empty()) {
      out << " output-prefix=\"" << c.m_output_prefix << "\"";
    }

    /* Add input combination */
    out << " category=\"" << c.m_category << "\">";

    for (sip::configuration::position_list::const_iterator i = c.m_positions.begin(); i != c.m_positions.end(); ++i) {
      if (c.is_option(*(*i))) {
        out << "<option id=\"" << c.get_identifier(**i) << "\"";
    
        static_cast < option const* >((*i).get())->accept(*this);

        out << "</option>";
      }
      else {
        out << "<object id=\"" << c.get_identifier(*(*i)) << "\" type=\""
            << std::string((c.is_input(*(*i))) ? "in" : "out") << "put\"";
     
        static_cast < object const* >((*i).get())->accept(*this);
     
        out << "/>";
      }
    }

    out << "</configuration>";
  }

  template <>
  void store_visitor_impl::visit(sip::datatype::boolean const& e, std::string const& s) {
    out << "<boolean";
    
    if (s.compare(sip::datatype::boolean::true_string) == 0) {
      out << " value=\"" << s << "\"";
    }

    out << "/>";
  }

  template <>
  void store_visitor_impl::visit(sip::datatype::integer const& e, std::string const& s) {
    out << "<integer";

    if (!s.empty()) {
      out << " value=\"" << std::dec << s << "\"";
    }

    if (e.m_minimum != sip::datatype::integer::implementation_minimum) {
      out << " minimum=\"" << e.m_minimum << "\"";
    }
      
    if (e.m_maximum != sip::datatype::integer::implementation_maximum) {
      out << " maximum=\"" << e.m_maximum << "\"";
    }

    if (e.m_default_value != e.m_minimum) {
      out << " default=\"" << e.m_default_value << "\"";
    }

    out << "/>";
  }

  template <>
  void store_visitor_impl::visit(sip::datatype::real const& e, std::string const& s) {
    out << "<real";

    if (!s.empty()) {
      out << " value=\"" << std::dec << s << "\"";
    }

    if (e.m_minimum != sip::datatype::real::implementation_minimum) {
      out << " minimum=\"" << e.m_minimum << "\"";
    }
      
    if (e.m_maximum != sip::datatype::real::implementation_maximum) {
      out << " maximum=\"" << e.m_maximum << "\"";
    }

    if (e.m_default_value != e.m_minimum) {
      out << " default=\"" << e.m_default_value << "\"";
    }

    out << "/>";
  }

  template <>
  void store_visitor_impl::visit(sip::datatype::enumeration const& e, std::string const& s) {
    out << "<enumeration value=\"" << s;

    if (e.m_default_value != 0) {
      out << "\" default=\"" << e.m_default_value;
    }

    out << "\">";

    for (std::vector < std::string >::const_iterator i = e.m_values.begin(); i != e.m_values.end(); ++i) {
      out << "<element value=\"" << *i << "\"/>";
    }

    out << "</enumeration>";
  }

  template <>
  void store_visitor_impl::visit(sip::datatype::string const& e, std::string const& s) {
    assert(!boost::regex_search(s, boost::regex("]]>")));

    out << "<string";

    if (e.m_minimum_length != 0) {
      out << " minimum-length=\"" << e.m_minimum_length << "\"";
    }
    if (e.m_maximum_length != sip::datatype::string::implementation_maximum_length) {
      out << " maximum-length=\"" << e.m_maximum_length << "\"";
    }

    if (!e.m_default_value.empty()) {
      out << " default=\"" << e.m_default_value << "\"";
    }

    if (!s.empty()) {
      out << "><![CDATA[" << s << "]]></string>";
    }
    else {
      out << "/>";
    }
  }

  template <>
  void store_visitor_impl::visit(sip::tool::capabilities const& c) {
    out << "<capabilities>"
        << "<protocol-version major=\"" << (unsigned short) c.m_protocol_version.major
        << "\" minor=\"" << (unsigned short) c.m_protocol_version.minor << "\"/>";

    for (sip::tool::capabilities::input_combination_list::const_iterator i = c.m_input_combinations.begin(); i != c.m_input_combinations.end(); ++i) {
      out << "<input-configuration category=\"" << (*i).m_category
          << "\" format=\"" << (*i).m_mime_type
          << "\" identifier=\"" << (*i).m_identifier << "\"/>";
    }

    for (sip::tool::capabilities::output_combination_list::const_iterator i = c.m_output_combinations.begin(); i != c.m_output_combinations.end(); ++i) {
      out << "<output-configuration format=\"" << (*i).m_mime_type
          << "\" identifier=\"" << (*i).m_identifier << "\"/>";
    }
 
    out << "</capabilities>";
  }

  template <>
  void store_visitor_impl::visit(sip::report const& c) {
    using sip::exception;

    out << "<report type=\"" << c.m_report_type << "\">";

    /* Include description */
    if (!c.description.empty()) {
      const std::string pattern("]]>");

      /* Sanity check... (todo better would be to use Base-64 or some other encoding) */
      if (std::search(c.description.begin(), c.description.end(), pattern.begin(), pattern.end()) != c.description.end()) {
        throw (std::exception());
      }

      out << "<description><![CDATA[" << c.description << "]]></description>";
    }

    out << "</report>";
  }

  template <>
  void store_visitor_impl::visit(sip::controller::capabilities const& c) {
    out << "<capabilities>"
        << "<protocol-version major=\"" << (unsigned short) c.m_protocol_version.major
        << "\" minor=\"" << (unsigned short) c.m_protocol_version.minor << "\"/>"
        << "<display-dimensions x=\"" << c.m_dimensions.x
        << "\" y=\"" << c.m_dimensions.y
        << "\" z=\"" << c.m_dimensions.z << "\"/>"
        << "</capabilities>";
  }

  template <>
  void store_visitor_impl::visit(sip::layout::elements::label const& c, sip::layout::element_identifier const& id) {
    out << "<label id=\"" << id << "\"><![CDATA[" << c.m_text << "]]></label>";
  }

  template <>
  void store_visitor_impl::visit(sip::layout::elements::button const& c, sip::layout::element_identifier const& id) {
    out << "<button id=\"" << id << "\" label=\"" << c.m_label << "\"/>";
  }

  template <>
  void store_visitor_impl::visit(sip::layout::elements::radio_button const& c, sip::layout::element_identifier const& id) {
    out << "<radio-button id=\"" << id << "\" label=\"" << c.m_label
        << "\" connected=\"" << c.m_connection;

    if (c.m_selected) {
      out << "\" selected=\"" << c.m_selected;
    }
    if (c.m_first) {
      out << "\" first=\"true";
    }

    out << "\"/>";
  }

  template <>
  void store_visitor_impl::visit(sip::layout::elements::checkbox const& c, sip::layout::element_identifier const& id) {
    out << "<checkbox id=\"" << id << "\" label=\"" << c.m_label << "\" status=\"" << c.m_status << "\"/>";
  }

  template <>
  void store_visitor_impl::visit(sip::layout::elements::progress_bar const& c, sip::layout::element_identifier const& id) {
    out << "<progress-bar id=\"" << id << "\" minimum=\"" << c.m_minimum
        << "\" maximum=\"" << c.m_maximum << "\" current=\"" << c.m_current <<  "\"/>";
  }

  template <>
  void store_visitor_impl::visit(sip::layout::elements::text_field const& c, sip::layout::element_identifier const& id) {
    out << "<text-field id=\"" << id << "\">"
        << "<text><![CDATA[" << c.m_text << "]]></text>";

//    type->write(o, text);

    out << "</text-field>";
  }

  /** \brief Maps alignment to a string */
  static const char* alignment_to_text[6] = {"top","middle","bottom","left","center","right"};

  /** \brief Maps visibility to a string */
  static const char* visibility_to_text[3] = {"visible","hidden","none"};

  template <>
  void store_visitor_impl::visit(sip::layout::properties const& c) {
    out << "<properties "
        << "horizontal-alignment=\"" << alignment_to_text[c.m_alignment_horizontal]
        << "\" vertical-alignment=\"" << alignment_to_text[c.m_alignment_vertical]
        << "\" margin-top=\"" << c.m_margin.top
        << "\" margin-left=\"" << c.m_margin.left
        << "\" margin-bottom=\"" << c.m_margin.bottom
        << "\" margin-right=\"" << c.m_margin.right;

    if (c.m_grow) {
      out << "\" grow=\"" << c.m_grow;
    }

    if (c.m_enabled) {
      out << "\" enabled=\"" << c.m_enabled;
    }

    out << "\" visibility=\"" << visibility_to_text[c.m_visible] << "\"/>";
  }

  template <>
  void store_visitor_impl::visit(sip::layout::properties const& c0, sip::layout::properties const& c1) {
    out << "<properties";

    if (c0.m_alignment_horizontal != c1.m_alignment_horizontal) {
      out << " horizontal-alignment=\"" << alignment_to_text[c0.m_alignment_horizontal] << "\"";
    }
    if (c0.m_alignment_vertical != c1.m_alignment_vertical) {
      out << " vertical-alignment=\"" << alignment_to_text[c0.m_alignment_vertical] << "\"";
    }
    if (c0.m_margin.top != c1.m_margin.top) {
      out << " margin-top=\"" << c0.m_margin.top << "\"";
    }
    if (c0.m_margin.left != c1.m_margin.left) {
      out << " margin-left=\"" << c0.m_margin.left << "\"";
    }
    if (c0.m_margin.bottom != c1.m_margin.bottom) {
      out << " margin-bottom=\"" << c0.m_margin.bottom << "\"";
    }
    if (c0.m_margin.right != c1.m_margin.right) {
      out << " margin-right=\"" << c0.m_margin.right << "\"";
    }
    if (!c1.m_grow && c0.m_grow) {
      out << " grow=\"" << c0.m_grow << "\"";
    }
    if (!c1.m_enabled && c0.m_enabled) {
      out << " enabled=\"" << c0.m_enabled << "\"";
    }
    if (!c1.m_visible && c0.m_visible) {
      out << " visibility=\"" << visibility_to_text[c0.m_visible] << "\"";
    }

    out << "/>";
  }

  template <>
  void store_visitor_impl::visit(sip::layout::box const& c) {
    sip::layout::properties const* current_properties = &sip::layout::manager::default_properties;

    for (sip::layout::vertical_box::children_list::const_iterator i = c.m_children.begin(); i != c.m_children.end(); ++i) {
      if ((*i).second.second != *current_properties) {
        visit(*current_properties, (*i).second.second);

        current_properties = &((*i).second.second);
      }

      (*i).second.first->accept(*this, (*i).first);
    }
  }

  template <>
  void store_visitor_impl::visit(sip::layout::vertical_box const& c) {
    out << "<box-layout-manager variant=\"vertical\">";

    visit(static_cast < sip::layout::box const& > (c));

    out << "</box-layout-manager>";
  }

  template <>
  void store_visitor_impl::visit(sip::layout::horizontal_box const& c) {
    out << "<box-layout-manager variant=\"horizontal\">";

    visit(static_cast < sip::layout::box const& > (c));

    out << "</box-layout-manager>";
  }

  template <>
  void store_visitor_impl::visit(sip::layout::tool_display const& c) {
    out << "<display-layout visible=\"" << c.m_visible << "\">";

    if (c.m_manager.get() != 0) {
      dynamic_cast < sip::layout::element* > (c.m_manager.get())->accept(*this);
    }

    out << "</display-layout>";
  }
}
