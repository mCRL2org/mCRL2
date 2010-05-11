// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <fstream>

#include "boost/filesystem/path.hpp"
#include "boost/noncopyable.hpp"
#include "boost/foreach.hpp"
#include "boost/xpressive/xpressive_static.hpp"

#include "tipi/report.hpp"
#include "tipi/detail/visitors.hpp"
#include "tipi/controller/capabilities.hpp"
#include "tipi/tool/capabilities.hpp"
#include "tipi/layout_elements.hpp"
#include "tipi/layout_manager.hpp"
#include "tipi/detail/utility/generic_visitor.hpp"
#include "tipi/tool_display.hpp"
#include "tipi/common.hpp"

namespace tipi {

  /// \cond INTERNAL_DOCS
  class store_visitor_impl : private boost::noncopyable {

    friend class visitors;
    friend class store_visitor;

    protected:

      /** \brief The destination of output */
      std::ostream&        out;

    protected:

      /** \brief Writes to stream */
      store_visitor_impl(std::ostream& o) : out(o) {
      }
  };

  class store_visitor_path_impl : public ::utility::visitor< store_visitor_impl > {

    private:

      /** \brief Storage for temporary std::ostream object */
      std::ofstream    m_help_stream;

    public:

      /** \brief Writes to file */
      store_visitor_path_impl(boost::filesystem::path const& p) :
                                ::utility::visitor< store_visitor_impl >(m_help_stream),
                                m_help_stream(p.string().c_str(), std::ios_base::out) {

        m_help_stream.exceptions(std::ofstream::failbit|std::ofstream::badbit);
      }
  };

  /** \brief Maps alignment to a string */
  static const char* alignment_to_text[6] = {"top","middle","bottom","left","center","right"};

  /** \brief Maps visibility to a string */
  static const char* visibility_to_text[3] = {"visible","hidden","none"};

  store_visitor::store_visitor(boost::filesystem::path const& p) :
        ::utility::visitor_interface< store_visitor_impl >(boost::shared_ptr < ::utility::visitor< store_visitor_impl > > (new store_visitor_path_impl(p))) {
  }

  store_visitor::store_visitor(std::ostream& o) :
        ::utility::visitor_interface< store_visitor_impl >(boost::shared_ptr < ::utility::visitor< store_visitor_impl > > (new ::utility::visitor< store_visitor_impl >(o))) {
  }
  /// \endcond
}

namespace utility {
  /// \cond INTERNAL_DOCS

  /**
   * \param[in] o the tipi::message object to store
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::message const& o) {
    out << "<message type=\"" << o.m_type << "\">";

    size_t i = 0;
    size_t j = o.m_content.find(']', 0);
    bool   f = true;

    while (j < o.m_content.size() - 3) {
      if (o.m_content[++j] == ']') {
        if (o.m_content[++j] == '>') {
          if (f) {
            out << "<![CDATA[";

            f = false;
          }

          out << o.m_content.substr(i,j - i) << "]]><![CDATA[>";

          i = ++j;
        }
      }

      j = o.m_content.find(']', j);
    }

    if (!o.m_content.substr(i).empty()) {
      if (f) {
        out << "<![CDATA[";

        f = false;
      }

      out << o.m_content.substr(i);
    }

    if (!f) {
      out << "]]>";
    }

    out << "</message>";
  }

  /**
   * \param[in] o the tipi::configuration::object object to store
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::configuration::object const& o) {
    if (!o.m_location.empty()) {
      out << " location=\"" << o.m_location << "\"";
    }

    out << " format=\"" << o.m_mime_type << "\"";
  }

  /**
   * \param[in] o the tipi::configuration::option object to store
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::configuration::option const& o) {
    out << ">";

    if (o.takes_arguments()) {
      BOOST_FOREACH(tipi::configuration::option::type_value_list::value_type i, o.m_arguments) {
        do_visit(*i.first, i.second);
      }
    }
  }

  /**
   * \param[in] c the configuration object to store
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::configuration const& c) {
    out << "<configuration";

    if (c.m_fresh) {
      out << " interactive=\"true\"";
    }

    if (!c.m_output_prefix.empty()) {
      out << " output-prefix=\"" << c.m_output_prefix << "\"";
    }

    /* Add input combination */
    out << " category=\"" << c.m_category << "\">";

    for (tipi::configuration::position_list::const_iterator i = c.m_positions.begin(); i != c.m_positions.end(); ++i) {
      if (c.is_option(**i)) {
        out << "<option id=\"" << c.get_identifier(**i) << "\"";

        do_visit(static_cast < tipi::configuration::option const& >(**i));

        out << "</option>";
      }
      else {
        out << "<object id=\"" << c.get_identifier(**i) << "\" type=\""
            << std::string((c.is_input(**i)) ? "in" : "out") << "put\"";

        do_visit(static_cast < tipi::configuration::object const& >(**i));

        out << "/>";
      }
    }

    out << "</configuration>";
  }

  /**
   * \param[in] e the tipi::datatype::boolean object to store
   * \param[in] s an instance of the data type in text representation
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::datatype::boolean const& /*e*/, std::string const& s) {
    out << "<boolean";

    if (s.compare("true") == 0) {
      out << " value=\"" << s << "\"";
    }

    out << "/>";
  }

  /**
   * \param[in] e the tipi::datatype::integer object to store
   * \param[in] s an instance of the data type in text representation
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::datatype::basic_integer_range const& e, std::string const& s) {
    out << "<integer";

    if (!s.empty()) {
      out << " value=\"" << s << "\"";
    }

    out << " range=\"" << e << "\"/>";
  }

  /**
   * \param[in] e the tipi::datatype::real object to store
   * \param[in] s an instance of the data type in text representation
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::datatype::basic_real_range const& e, std::string const& s) {
    out << "<real";

    if (!s.empty()) {
      out << " value=\"" << s << "\"";
    }

    out << " range=\"" << e << "\"/>";
  }

  /**
   * \param[in] e the tipi::datatype::enumeration object to store
   * \param[in] s an instance of the data type in text representation
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::datatype::basic_enumeration const& e, std::string const& s) {
    out << "<enumeration";

    if (!s.empty()) {
      out << " value=\"" << s << "\"";;
    }

    out << ">";

    for (tipi::datatype::basic_enumeration::const_iterator_range i = e.values(); i.first != i.second; ++i.first) {
      out << "<element value=\"" << i.first->first << "\"><![CDATA[" << i.first->second << "]]></element>";
    }

    out << "</enumeration>";
  }

  /**
   * \param[in] e the tipi::datatype::string object to store
   * \param[in] s an instance of the data type in text representation
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::datatype::string const& e, std::string const& s) {
    using namespace boost::xpressive;

    if(!(!regex_search(s, sregex(as_xpr("]]>"))))){
      throw std::runtime_error( "No instance found for the data type in textual representation" );
    };

    out << "<string";

    if (e.m_minimum_length != 0) {
      out << " minimum-length=\"" << e.m_minimum_length << "\"";
    }
    if (e.m_maximum_length != boost::integer_traits< size_t >::const_max) {
      out << " maximum-length=\"" << e.m_maximum_length << "\"";
    }

    if (!s.empty()) {
      out << "><![CDATA[" << s << "]]></string>";
    }
    else {
      out << "/>";
    }
  }

  /**
   * \param[in] c the tipi::tool::capabilities object to store
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::tool::capabilities::input_configuration const& c) {
    using tipi::tool::capabilities;

    out << "<input-configuration category=\"" << c.get_category() << "\">";

    for (capabilities::input_configuration::object_map::const_iterator j = c.m_object_map.begin(); j != c.m_object_map.end(); ++j) {
      out << "<object id=\"" << j->first << "\" format=\"" << j->second << "\"/>";
    }

    out << "</input-configuration>";
  }

  /**
   * \param[in] c the tipi::tool::capabilities object to store
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::tool::capabilities const& c) {
    using tipi::tool::capabilities;

    out << "<capabilities>"
        << "<protocol-version major=\"" << (unsigned short) c.m_protocol_version.major
        << "\" minor=\"" << (unsigned short) c.m_protocol_version.minor << "\"/>";

    for (capabilities::input_configuration_list::const_iterator i = c.m_input_configurations.begin(); i != c.m_input_configurations.end(); ++i) {
      visit(*(*i));
    }

    for (capabilities::output_configuration_list::const_iterator i = c.m_output_configurations.begin(); i != c.m_output_configurations.end(); ++i) {
      out << "<output-configuration format=\"" << (*i)->get_format()
          << "\" id=\"" << (*i)->m_identifier << "\"/>";
    }

    out << "</capabilities>";
  }

  /**
   * \param[in] c the tipi::report object to store
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::report const& c) {
    out << "<report type=\"" << c.m_report_type << "\">";

    /* Include description */
    if (!c.m_description.empty()) {
      const std::string pattern("]]>");

      /* Sanity check... (todo better would be to use Base-64 or some other encoding) */
      if (std::search(c.m_description.begin(), c.m_description.end(), pattern.begin(), pattern.end()) != c.m_description.end()) {
        throw std::runtime_error("Illegal instance of ']]>' found");
      }

      out << "<description><![CDATA[" << c.m_description << "]]></description>";
    }

    out << "</report>";
  }

  /**
   * \param[in] c the tipi::controller::capabilities object to store
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::controller::capabilities const& c) {
    out << "<capabilities>"
        << "<protocol-version major=\"" << (unsigned short) c.m_protocol_version.major
        << "\" minor=\"" << (unsigned short) c.m_protocol_version.minor << "\"/>"
        << "</capabilities>";
  }

  /**
   * \param[in] c the tipi::layout::elements::label object to store
   * \param[in] id the identifier for the label
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::layout::elements::label const& c, ::tipi::display::element_identifier const& id) {
    out << "<label id=\"" << id << "\"><![CDATA[" << c.m_text << "]]></label>";
  }

  /**
   * \param[in] c the tipi::layout::elements::button object to store
   * \param[in] id the identifier for the button
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::layout::elements::button const& c, ::tipi::display::element_identifier const& id) {
    out << "<button id=\"" << id << "\"><![CDATA[" << c.m_label << "]]></button>";
  }

  /**
   * \param[in] c the tipi::layout::elements::radio_button object to store
   * \param[in] id the identifier for the radio_button
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::layout::elements::radio_button const& c, ::tipi::display::element_identifier const& id) {
    out << "<radio-button id=\"" << id
        << "\" connected=\"" << reinterpret_cast< tipi::display::element_identifier > (c.m_connection);

    if (c.m_selected) {
      out << "\" selected=\"" << c.m_selected;
    }

    out << "\"><![CDATA[" << c.m_label << "]]></radio-button>";
  }

  /**
   * \param[in] c the tipi::layout::elements::checkbox object to store
   * \param[in] id the identifier for the checkbox
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::layout::elements::checkbox const& c, ::tipi::display::element_identifier const& id) {
    out << "<checkbox id=\"" << id << "\" checked=\"" << c.m_status << "\"><![CDATA[" << c.m_label << "]]></checkbox>";
  }

  /**
   * \param[in] c the tipi::layout::elements::progress_bar object to store
   * \param[in] id the identifier for the progress bar
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::layout::elements::progress_bar const& c, ::tipi::display::element_identifier const& id) {
    out << "<progress-bar id=\"" << id << "\" minimum=\"" << c.m_minimum
        << "\" maximum=\"" << c.m_maximum << "\" current=\"" << c.m_current <<  "\"/>";
  }

  /**
   * \param[in] c the tipi::layout::elements::text_field object to store
   * \param[in] id the identifier for the text field
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::layout::elements::text_field const& c, ::tipi::display::element_identifier const& id) {
    out << "<text-field id=\"" << id << "\">"
        << "<text><![CDATA[" << c.m_text << "]]></text>";

    do_visit(*c.m_type, c.m_text);

    out << "</text-field>";
  }

  /**
   * \param[in] c the tipi::layout::properties object to store
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::layout::properties const& c) {
    out << "<properties "
        << "horizontal-alignment=\"" << tipi::alignment_to_text[c.m_alignment_horizontal]
        << "\" vertical-alignment=\"" << tipi::alignment_to_text[c.m_alignment_vertical]
        << "\" margin-top=\"" << c.m_margin.m_top
        << "\" margin-left=\"" << c.m_margin.m_left
        << "\" margin-bottom=\"" << c.m_margin.m_bottom
        << "\" margin-right=\"" << c.m_margin.m_right;

    if (c.m_grow) {
      out << "\" grow=\"" << c.m_grow;
    }

    if (c.m_enabled) {
      out << "\" enabled=\"" << c.m_enabled;
    }

    out << "\" visibility=\"" << tipi::visibility_to_text[c.m_visible] << "\"/>";
  }

  /**
   * \param[in] c0 a tipi::layout::properties object as reference
   * \param[in] c1 a tipi::layout::properties object to store the differences with the reference of
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::layout::properties const& c0, tipi::layout::properties const& c1) {
    out << "<properties";

    if (c0.m_alignment_horizontal != c1.m_alignment_horizontal) {
      out << " horizontal-alignment=\"" << tipi::alignment_to_text[c0.m_alignment_horizontal] << "\"";
    }
    if (c0.m_alignment_vertical != c1.m_alignment_vertical) {
      out << " vertical-alignment=\"" << tipi::alignment_to_text[c0.m_alignment_vertical] << "\"";
    }
    if (c0.m_margin.m_top != c1.m_margin.m_top) {
      out << " margin-top=\"" << c0.m_margin.m_top << "\"";
    }
    if (c0.m_margin.m_left != c1.m_margin.m_left) {
      out << " margin-left=\"" << c0.m_margin.m_left << "\"";
    }
    if (c0.m_margin.m_bottom != c1.m_margin.m_bottom) {
      out << " margin-bottom=\"" << c0.m_margin.m_bottom << "\"";
    }
    if (c0.m_margin.m_right != c1.m_margin.m_right) {
      out << " margin-right=\"" << c0.m_margin.m_right << "\"";
    }
    if (c1.m_grow != c0.m_grow) {
      out << " grow=\"" << c0.m_grow << "\"";
    }
    if (c1.m_enabled != c0.m_enabled) {
      out << " enabled=\"" << c0.m_enabled << "\"";
    }
    if (c1.m_visible != c0.m_visible) {
      out << " visibility=\"" << tipi::visibility_to_text[c0.m_visible] << "\"";
    }

    out << "/>";
  }

  /**
   * \internal
   * \param[in] c a tipi::layout::box object as reference
   * \param[in,out] d display with which the element is associated
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::layout::vertical_box const& c, ::tipi::display const& d) {
    static tipi::layout::properties default_properties;

    tipi::layout::properties const* current_properties = &default_properties;

    out << "<box-layout-manager variant=\"vertical\" id=\"" << d.find(&c) << "\">";

    for (tipi::layout::vertical_box::children_list::const_iterator i = c.m_children.begin(); i != c.m_children.end(); ++i) {
      if ((i->layout_properties) != *current_properties) {
        visit((i->layout_properties), *current_properties);

        current_properties = &(i->layout_properties);
      }

      try {
        do_visit< const tipi::layout::element, const tipi::display::element_identifier>(
                                                *(i->layout_element), d.find(i->layout_element));
      }
      catch (...) {
        // Assume element is a layout manager
        do_visit(*(i->layout_element), d);
      }
    }

    out << "</box-layout-manager>";
  }

  /**
   * \internal
   * \param[in] c a tipi::layout::box object as reference
   * \param[in,out] d display with which the element is associated
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::layout::horizontal_box const& c, ::tipi::display const& d) {
    static tipi::layout::properties default_properties;

    tipi::layout::properties const* current_properties = &default_properties;

    out << "<box-layout-manager variant=\"horizontal\" id=\"" << d.find(&c) << "\">";

    for (tipi::layout::horizontal_box::children_list::const_iterator i = c.m_children.begin(); i != c.m_children.end(); ++i) {
      if ((i->layout_properties) != *current_properties) {
        visit((i->layout_properties), *current_properties);

        current_properties = &(i->layout_properties);
      }

      try {
        do_visit< const tipi::layout::element, const tipi::display::element_identifier>(
                                                *(i->layout_element), d.find(i->layout_element));
      }
      catch (...) {
        // Assume element is a layout manager
        do_visit(*(i->layout_element), d);
      }
    }

    out << "</box-layout-manager>";
  }

  /**
   * \param[in] c a tipi::tool_display object to store
   **/
  template <>
  template <>
  void visitor< tipi::store_visitor_impl >::visit(tipi::tool_display const& c) {
    out << "<display-layout visible=\"" << c.m_visible << "\">"
        << "<layout-manager>";

    if (c.manager() != 0) {
      do_visit(*c.manager(), static_cast < tipi::display const& > (c));
    }

    out << "</layout-manager>"
        << "</display-layout>";
  }

  /** Registers all available visit methods */
  template <>
  bool visitor< tipi::store_visitor_impl >::initialise() {
    register_visit_method< const tipi::message >();
    register_visit_method< const tipi::datatype::boolean, const std::string >();
    register_visit_method< const tipi::datatype::basic_integer_range, const std::string >();
    register_visit_method< const tipi::datatype::basic_real_range, const std::string >();
    register_visit_method< const tipi::datatype::basic_enumeration, const std::string >();
    register_visit_method< const tipi::datatype::string, const std::string >();
    register_visit_method< const tipi::configuration::object >();
    register_visit_method< const tipi::configuration::option >();
    register_visit_method< const tipi::configuration >();
    register_visit_method< const tipi::controller::capabilities >();
    register_visit_method< const tipi::tool::capabilities >();
    register_visit_method< const tipi::tool::capabilities::input_configuration >();
    register_visit_method< const tipi::report >();
    register_visit_method< const tipi::tool_display >();
    register_visit_method< const tipi::layout::elements::button, const ::tipi::display::element_identifier >();
    register_visit_method< const tipi::layout::elements::checkbox, const ::tipi::display::element_identifier >();
    register_visit_method< const tipi::layout::elements::label, const ::tipi::display::element_identifier >();
    register_visit_method< const tipi::layout::elements::progress_bar, const ::tipi::display::element_identifier >();
    register_visit_method< const tipi::layout::elements::radio_button, const ::tipi::display::element_identifier >();
    register_visit_method< const tipi::layout::elements::text_field, const ::tipi::display::element_identifier >();
    register_visit_method< const tipi::layout::horizontal_box, const ::tipi::display >();
    register_visit_method< const tipi::layout::vertical_box, const ::tipi::display >();
    register_visit_method< const tipi::layout::properties >();

    return true;
  }
  /// \endcond
}
