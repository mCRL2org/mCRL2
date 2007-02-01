#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>

#include <sip/visitors.h>
#include <sip/report.h>
#include <sip/tool/capabilities.h>
#include <sip/controller/capabilities.h>
#include <sip/detail/basic_datatype.h>
#include <sip/detail/layout_elements.h>
#include <sip/detail/layout_manager.h>
#include <sip/detail/layout_tool_display.h>
#include <sip/detail/event_handlers.h>
#include <sip/detail/restore_visitor.tcc>
#include <sip/detail/common.h>

#include <utility/visitor.tcc>

template void utility::visitor< sip::restore_visitor_impl, false >::visit(sip::configuration&);
template void utility::visitor< sip::restore_visitor_impl, false >::visit(sip::tool::capabilities&);
template void utility::visitor< sip::restore_visitor_impl, false >::visit(sip::controller::capabilities&);
template void utility::visitor< sip::restore_visitor_impl, false >::visit(sip::layout::tool_display&);
template void utility::visitor< sip::restore_visitor_impl, false >::visit(sip::report&);
template void utility::visitor< sip::restore_visitor_impl, false >::visit(sip::message&);

namespace sip {

  class restore_visitor_alternate_impl : public restore_visitor_impl {

    private:

      /** \brief The source of input */
      xml2pp::text_reader in;

    public:

      /** \brief Reads from string */
      restore_visitor_alternate_impl(restore_visitor&, std::string const&);

      /** \brief Reads from file */
      restore_visitor_alternate_impl(restore_visitor&, boost::filesystem::path const&);
  };

  template < >
  restore_visitor::restore_visitor(xml2pp::text_reader& s) :
        utility::visitor_interface< restore_visitor_impl, false >(boost::shared_ptr < visitor_type > (new restore_visitor_impl(*this, s))) {
  }

  restore_visitor::restore_visitor(std::string const& s) :
        utility::visitor_interface< restore_visitor_impl, false >(boost::shared_ptr < visitor_type > (new restore_visitor_alternate_impl(*this, s))) {
  }

  restore_visitor::restore_visitor(boost::filesystem::path const& p) :
        utility::visitor_interface< restore_visitor_impl, false >(boost::shared_ptr < visitor_type > (new restore_visitor_alternate_impl(*this, p))) {
  }

  inline restore_visitor_impl::restore_visitor_impl(restore_visitor& i, xml2pp::text_reader& s) : m_interface(i), in(s) {
  }

  inline restore_visitor_alternate_impl::restore_visitor_alternate_impl(restore_visitor& i, std::string const& s) : restore_visitor_impl(i, in), in(s) {
  }

  inline restore_visitor_alternate_impl::restore_visitor_alternate_impl(restore_visitor& i, boost::filesystem::path const& p) : restore_visitor_impl(i, in), in(p) {
  }

  template <>
  void restore_visitor_impl::visit(boost::shared_ptr < sip::datatype::basic_datatype >&, std::string&);

  template <>
  void restore_visitor_impl::visit(sip::message& o) {
    assert(in.is_element("message-meta"));

    if (in.get_attribute("type")) {
      o.type = static_cast < sip::message::type_identifier_t > (boost::lexical_cast < size_t > (in.get_attribute_as_string("type")));
    }
    else {
      o.type = sip::message::message_unknown;
    }

    in.next_element();

    o.content = in.get_value_as_string();

    in.next_element();
    in.next_element();
  }

  template <>
  void restore_visitor_impl::visit(sip::object& o) {
    using sip::exception;

    assert(in.is_element("object"));

    o.m_mime_type = mime_type(in.get_attribute_as_string("mime-type"));
    
    in.get_attribute(&o.m_location, "location");
    
    in.next_element();
    in.skip_end_element("object");
  }

  template <>
  void restore_visitor_impl::visit(sip::option& o) {
    using sip::exception;

    assert(in.is_element("option"));

    if (!in.is_empty_element()) {
      in.next_element();
    
      while (!in.is_end_element("option")) {
        using namespace sip::datatype;

        /* The current element must be a datatype specification */
        std::pair < boost::shared_ptr < sip::datatype::basic_datatype >, std::string >      p;

        visit(p.first, p.second);

        o.m_arguments.push_back(p);
      }
    }

    in.skip_end_element("option");
  }

  /**
   * \param reader is a reference to a libXML 2 text reader instance
   * /pre the reader points to a \<configuration\> instance
   * /post the readers position is just past the configuration block
   **/
  template <>
  void restore_visitor_impl::visit(sip::configuration& c) {
    assert(in.is_element("configuration"));

    /* reset object state */
    c.m_fresh = true;
    c.m_parameter_by_id.clear();
    c.m_options.clear();
    c.m_input_objects.clear();
    c.m_output_objects.clear();
    c.m_positions.clear();

    c.m_fresh         = in.get_attribute("fresh");
    c.m_output_prefix = in.get_attribute_as_string("output-prefix");

    in.get_attribute(&c.m_category, "category");

    in.next_element();

    while (in.is_element("option") || in.is_element("object")) {
      std::string identifier = in.get_attribute_as_string("id");
     
      if (in.is_element("option")) {
        boost::shared_ptr < option > o(new sip::option);
     
        o->accept(*this);
     
        c.add_option(identifier, o);
      }
      else {
        std::string identifier = in.get_attribute_as_string("id");
     
        boost::shared_ptr < object > o(new sip::object);
     
        if (in.get_attribute_as_string("type") == "input") {
          o->accept(*this);
     
          c.add_input(identifier, o);
        }
        else {
          o->accept(*this);
     
          c.add_output(identifier, o);
        }
      }
    }

    in.skip_end_element("configuration");
  }

  template <>
  void restore_visitor_impl::visit(sip::datatype::boolean& e, std::string& s) {
    assert(in.is_element("boolean"));

    s = in.get_attribute_as_string("value");

    if (s != sip::datatype::boolean::true_string) {
      s = sip::datatype::boolean::false_string;
    }

    in.next_element();
    in.skip_end_element("boolean");

    assert(e.validate(s));

    /* Set to default if value is invalid */
    if (!(e.validate(s))) {
      s = sip::datatype::boolean::false_string;
    }
  }

  template <>
  void restore_visitor_impl::visit(sip::datatype::integer& e, std::string& s) {
    /* Current element must be <integer> */
    assert(in.is_element("integer"));

    e.m_minimum = datatype::integer::implementation_minimum;
    e.m_maximum = datatype::integer::implementation_maximum;

    in.get_attribute(&e.m_minimum, "minimum");
    in.get_attribute(&e.m_maximum, "maximum");

    e.m_default_value = e.m_minimum;

    if (in.get_attribute(&e.m_default_value, "default")) {
      s = e.m_default_value;
    }

    s = in.get_attribute_as_string("value");

    in.next_element();
    in.skip_end_element("integer");

    assert(e.validate(s));

    /* Set to default if value is invalid */
    if (!(e.validate(s))) {
      s = "0";
    }
  }

  template <>
  void restore_visitor_impl::visit(sip::datatype::real& e, std::string& s) {
    /* Current element must be <integer> */
    assert(in.is_element("real"));

    e.m_minimum = datatype::real::implementation_minimum;
    e.m_maximum = datatype::real::implementation_maximum;

    in.get_attribute(&e.m_minimum, "minimum");
    in.get_attribute(&e.m_maximum, "maximum");

    e.m_default_value = e.m_minimum;

    if (in.get_attribute(&e.m_default_value, "default")) {
      s = boost::lexical_cast < std::string > (e.m_default_value);
    }

    s = in.get_attribute_as_string("value");

    in.next_element();
    in.skip_end_element("real");

    assert(e.validate(s));

    /* Set to default if value is invalid */
    if (!(e.validate(s))) {
      s = "0";
    }
  }

  template <>
  void restore_visitor_impl::visit(sip::datatype::enumeration& e, std::string& s) {
    /* Current element must be <enumeration> */
    assert(in.is_element("enumeration"));

    in.get_attribute(&e.m_default_value, "default");

    s = in.get_attribute_as_string("value");

    in.next_element();

    while (!in.is_end_element("enumeration")) {
      /* Assume element */
      e.add_value(in.get_attribute_as_string("value"));

      in.next_element();
      in.skip_end_element("element");
    }

    in.skip_end_element("enumeration");

    if (s.empty()) {
      s = e.m_values[e.m_default_value];
    }

    assert(e.validate(s));
  }

  template <>
  void restore_visitor_impl::visit(sip::datatype::string& e, std::string& s) {
    /* Current element must be <string> */
    assert(in.is_element("string"));

    e.m_minimum_length = 0;
    e.m_maximum_length = sip::datatype::string::implementation_maximum_length;

    in.get_attribute(&s, "default");
    in.get_attribute(&e.m_minimum_length, "minimum-length");
    in.get_attribute(&e.m_maximum_length, "maximum-length");

    if (!in.is_empty_element()) {
      in.next_element();

      in.get_value(&s);
    }

    in.next_element();
    in.skip_end_element("string");

    assert(e.validate(s));
  }

  template < typename T >
  inline void visit_tuple_datatype(restore_visitor_impl* v, boost::shared_ptr < sip::datatype::basic_datatype >& c, std::string& l) {
    c.reset(new T);

    v->visit(*(boost::static_pointer_cast < T > (c)), l);
  }

  template <>
  void restore_visitor_impl::visit(boost::shared_ptr < sip::datatype::basic_datatype >& c, std::string& v) {

    if (in.is_element("enumeration")) {
      visit_tuple_datatype< sip::datatype::enumeration >(this, c, v);
    }
    else if (in.is_element("boolean")) {
      visit_tuple_datatype< sip::datatype::boolean >(this, c, v);
    }
    else if (in.is_element("integer")) {
      visit_tuple_datatype< sip::datatype::integer >(this, c, v);
    }
    else if (in.is_element("real")) {
      visit_tuple_datatype< sip::datatype::real >(this, c, v);
    }
    else if (in.is_element("uri")) {
//      return (uri::read(r));
    }
    else if (in.is_element("string")) {
      visit_tuple_datatype< sip::datatype::string >(this, c, v);
    }
    else {
      /* Unknown type in configuration */
      throw (sip::exception(sip::message_unknown_type, in.element_name()));
    }
  }

  template <>
  void restore_visitor_impl::visit(sip::tool::capabilities& c) {
    if (in.is_element("capabilities")) {
      in.next_element();
     
      if (in.is_element("protocol-version")) {
        in.get_attribute(&c.m_protocol_version.major, "major");
        in.get_attribute(&c.m_protocol_version.minor, "minor");
      }
     
      in.next_element();
      in.skip_end_element("protocol-version");
     
      assert(in.is_element("input-configuration"));
     
      while (in.is_element("input-configuration")) {
        std::string category_name;
        std::string format;
        std::string identifier;
     
        in.get_attribute(&category_name, "category");
        in.get_attribute(&format, "format");
        in.get_attribute(&identifier, "identifier");
     
        c.m_input_combinations.insert(sip::tool::capabilities::input_combination(
                tool::category::fit(category_name),mime_type(format),identifier));
     
        in.next_element();
        in.skip_end_element("input-configuration");
      }
     
      while (in.is_element("output-configuration")) {
        std::string format;
        std::string identifier;
     
        in.get_attribute(&format, "format");
        in.get_attribute(&identifier, "identifier");
     
        c.m_output_combinations.insert(sip::tool::capabilities::output_combination(
                                        mime_type(format),identifier));
     
        in.next_element();
        in.skip_end_element("output-configuration");
      }
    }
  }

  template <>
  void restore_visitor_impl::visit(sip::controller::capabilities& c) {
    if (in.is_element("capabilities")) {
      in.next_element();
 
      assert (in.is_element("protocol-version"));
 
      in.get_attribute(&c.m_protocol_version.major, "major");
      in.get_attribute(&c.m_protocol_version.minor, "minor");
 
      in.next_element();
      in.skip_end_element("protocol-version");
 
      assert (in.is_element("display-dimensions"));
 
      in.get_attribute(&c.m_dimensions.x, "x");
      in.get_attribute(&c.m_dimensions.y, "y");
      in.get_attribute(&c.m_dimensions.z, "z");
 
      in.next_element();
    }
  }  

  template <>
  void restore_visitor_impl::visit(sip::report& c) {
    assert(in.is_element("report"));

    c.m_report_type = static_cast < sip::report::type > (boost::lexical_cast < unsigned int > (in.get_attribute_as_string("type")));

    in.next_element();

    if (in.is_element("description")) {
      in.next_element();

      in.get_value(&c.description);

      in.next_element();
    }

    in.skip_end_element("report");
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::elements::label& c) {
    
    in.next_element();

    if (!in.is_empty_element() && !in.is_element("label")) {
      in.get_value(&c.m_text);

      in.next_element();
    }

    in.skip_end_element("label");

    c.m_event_handler->process(&c);
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::elements::button& c) {
    in.get_attribute(&c.m_label, "label");

    in.next_element();

    c.m_event_handler->process(&c);
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::elements::radio_button& c, sip::display::element_for_id& element_by_id) {
    using sip::layout::elements::radio_button;

    sip::layout::element_identifier connected_to = reinterpret_cast < sip::layout::element_identifier > (&c);

    in.get_attribute(&c.m_label, "label");

    if (in.get_attribute("connected")) {
      connected_to = boost::lexical_cast < sip::layout::element_identifier > (in.get_attribute_as_string("connected"));
    }

    c.m_first    = in.get_attribute("first");
    c.m_selected = in.get_attribute("selected");

    if (connected_to != reinterpret_cast < sip::layout::element_identifier > (&c)) {
      if (element_by_id.count(connected_to) != 0) {
        c.m_connection = dynamic_cast < radio_button* > (element_by_id[connected_to]);
     
        for (radio_button* i = c.m_connection; i != &c; i = i->m_connection) {
          i->m_connection = dynamic_cast < radio_button* > (element_by_id[reinterpret_cast < sip::layout::element_identifier > (i->m_connection)]);
        }
      }
      else {
        c.m_connection = reinterpret_cast < radio_button* > (connected_to);
      }
    }
     
    if (c.m_selected) {
      /* Make sure all related radio buttons are not unselected */
      c.set_selected(true);
    }

    in.next_element();

    c.m_event_handler->process(&c);
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::elements::checkbox& c) {
    in.get_attribute(&c.m_label, "label");

    c.m_status = in.get_attribute("status");

    in.next_element();

    c.m_event_handler->process(&c);
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::elements::progress_bar& c) {
    in.get_attribute(&c.m_minimum, "minimum");
    in.get_attribute(&c.m_maximum, "maximum");
    in.get_attribute(&c.m_current, "current");

    in.next_element();

    c.m_event_handler->process(&c);
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::elements::text_field& c) {
    in.next_element();

    if (in.is_element("text")) {
      if (!in.is_end_element()) {
        in.next_element();

        if (!in.is_end_element()) { 
          in.get_value(&c.m_text);

          in.next_element();
        }

        in.skip_end_element("text");
      }
    }

    if (!in.is_end_element("text")) {
      /* Assume datatype specification */
//      type = basic_datatype::read(r.reader).first;
    }

    in.skip_end_element("text-field");

    c.m_event_handler->process(&c);
  }

  template <>
  void restore_visitor_impl::visit(std::auto_ptr < sip::layout::element >& c, sip::layout::element_identifier& id) {
    using namespace sip::layout::elements;

    std::string name(in.element_name());

    in.get_attribute(&id, "id");

    if (name == "label") {
      c.reset(new label());
    }
    else if (name == "button") {
      c.reset(new button());
    }
    else if (name == "radio-button") {
      c.reset(new radio_button());
    }
    else if (name == "checkbox") {
      c.reset(new checkbox());
    }
    else if (name == "progress-bar") {
      c.reset(new progress_bar());
    }
    else if (name == "text-field") {
      c.reset(new text_field());
    }

    if (c.get() != 0) {
      /* Read abstract element specific data */
      c->accept(*this);
    }
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::box& c, sip::display::element_for_id& element_by_id) {
    sip::layout::properties current_properties = sip::layout::manager::default_properties;

    c.clear();

    if (!in.is_empty_element()) {
      in.next_element();

      while (!in.is_end_element("box-layout-manager")) {
        if (in.is_element("properties")) {
          current_properties.accept(*this);
        }

        sip::layout::element_identifier        id;
        std::auto_ptr < sip::layout::element > p;

        visit(p, id);

        element_by_id[id] = p.release();

        c.m_children[id] = std::make_pair(element_by_id[id], current_properties);
      }
    }

    in.next_element();
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::horizontal_box& c, sip::display::element_for_id& element_by_id) {
    visit(static_cast < sip::layout::box& > (c), element_by_id);
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::vertical_box& c, sip::display::element_for_id& element_by_id) {
    visit(static_cast < sip::layout::box& > (c), element_by_id);
  }

  template <>
  void restore_visitor_impl::visit(std::auto_ptr < sip::layout::manager >& c, sip::display::element_for_id& element_by_id) {
    using sip::exception;

    std::string name(in.element_name());

    if (name == "box-layout-manager") {
      if (in.get_attribute_as_string("variant") == "vertical") {
        c.reset(new sip::layout::vertical_box());

        visit(static_cast < sip::layout::vertical_box& > (*c), element_by_id);
      }
      else {
        c.reset(new sip::layout::horizontal_box());

        visit(static_cast < sip::layout::horizontal_box& > (*c), element_by_id);
      }
    }
    else {
      throw (new exception(sip::unknown_layout_element, name));
    }
  }

  /** \brief Finds a member of the visibility domain for a string */
  static sip::layout::visibility text_to_visibility(std::string const& s) {
    if (s == "visible") {
      return (sip::layout::visible);
    }
    else if (s == "hidden") {
      return (sip::layout::hidden);
    }
    else { /* Assume none */
      return (sip::layout::none);
    }
  }

  /** \brief Finds a member of the visibility domain for a string */
  static sip::layout::vertical_alignment text_to_vertical_alignment(std::string const& s) {
    if (s == "top") {
      return (sip::layout::top);
    }
    else if (s == "middle") {
      return (sip::layout::middle);
    }
    else { /* Assume bottom */
      return (sip::layout::bottom);
    }
  }

  /** \brief Finds a member of the visibility domain for a string */
  static sip::layout::horizontal_alignment text_to_horizontal_alignment(std::string const& s) {
    if (s == "left") {
      return (sip::layout::left);
    }
    else if (s == "center") {
      return (sip::layout::center);
    }
    else { /* Assume right */
      return (sip::layout::right);
    }
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::properties& c) {
    if (in.get_attribute("horizontal-alignment")) {
      c.m_alignment_horizontal = text_to_horizontal_alignment(in.get_attribute_as_string("horizontal-alignment"));
    }
    if (in.get_attribute("vertical-alignment")) {
      c.m_alignment_vertical = text_to_vertical_alignment(in.get_attribute_as_string("vertical-alignment"));
    }

    in.get_attribute(&c.m_margin.top, "margin-top");
    in.get_attribute(&c.m_margin.left, "margin-left");
    in.get_attribute(&c.m_margin.bottom, "margin-bottom");
    in.get_attribute(&c.m_margin.right, "margin-right");

    if (in.get_attribute("visibility")) { 
      c.m_visible = text_to_visibility(in.get_attribute_as_string("visibility"));
    }

    c.m_grow    = in.get_attribute("grow");
    c.m_enabled = in.get_attribute("enabled");

    in.next_element();
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::tool_display& c) {
    c.m_visible = in.get_attribute("visible");

    in.next_element();

    if (!in.is_end_element("display-layout")) {
      std::auto_ptr < sip::layout::manager > p;

      visit(p, c.m_element_by_id);

      c.set_manager(p);
    }
  }
}
