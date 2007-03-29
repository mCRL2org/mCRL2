#include <boost/bind.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>

#include <utility/generic_visitor.tcc>

#include <sip/visitors.h>
#include <sip/report.h>
#include <sip/tool/capabilities.h>
#include <sip/controller/capabilities.h>
#include <sip/detail/basic_datatype.h>
#include <sip/detail/layout_elements.h>
#include <sip/detail/layout_manager.h>
#include <sip/detail/layout_tool_display.h>
#include <sip/detail/event_handlers.h>
#include <sip/detail/common.h>

#include <ticpp.h>

namespace sip {

  class restore_visitor_impl : private boost::noncopyable {

    friend class visitors;
    friend class restore_visitor;

    protected:

      /** \brief Points to the current element */
      ticpp::Element*  tree;

    public:

      /** \brief Reads from parse tree */
      restore_visitor_impl();

      /** \brief Reads from parse tree */
      restore_visitor_impl(ticpp::Element* s);
  };

  class restore_visitor_impl_frontend : public utility::visitor< restore_visitor_impl > {

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

  template < >
  restore_visitor::restore_visitor(ticpp::Element& s) :
        utility::visitor_interface< restore_visitor_impl >(boost::shared_ptr < utility::visitor< restore_visitor_impl > > (new utility::visitor< restore_visitor_impl >(&s))) {
  }

  restore_visitor::restore_visitor(std::string const& s) :
        utility::visitor_interface< restore_visitor_impl >(boost::shared_ptr < utility::visitor< restore_visitor_impl > > (new restore_visitor_impl_frontend(s))) {
  }

  restore_visitor::restore_visitor(boost::filesystem::path const& p) :
        utility::visitor_interface< restore_visitor_impl >(boost::shared_ptr < utility::visitor< restore_visitor_impl > > (new restore_visitor_impl_frontend(p))) {
  }

  inline restore_visitor_impl::restore_visitor_impl() {
  }

  inline restore_visitor_impl::restore_visitor_impl(ticpp::Element* s) : tree(s) {
  }

  inline restore_visitor_impl_frontend::restore_visitor_impl_frontend(std::istream& s) {
    s >> in;

    tree = in.FirstChildElement(false);
  }

  inline restore_visitor_impl_frontend::restore_visitor_impl_frontend(std::string const& s) {
    std::istringstream ins(s);

    ins >> in;

    tree = in.FirstChildElement(false);
  }

  inline restore_visitor_impl_frontend::restore_visitor_impl_frontend(boost::filesystem::path const& p) {
    std::ifstream ins(p.native_file_string().c_str());

    ins >> in;

    tree = in.FirstChildElement(false);
  }

  std::istream& operator >> (std::istream& s, sip::message::type_identifier_t& id) {
    size_t t;

    s >> t;

    id = static_cast < sip::message::type_identifier_t > (t);

    return (s);
  }
}

namespace utility {

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::message& o) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "message-meta");

    tree->GetAttributeOrDefault("type", &o.m_type, sip::message::message_unknown);

    o.m_content.clear();

    for (ticpp::Node* e = tree->FirstChild(false); e != 0; e = e->NextSibling(false)) {
      if (e->Type() == TiXmlNode::TEXT) {
        static_cast < ticpp::Text* > (e)->SetCDATA(false);

        o.m_content += e->ToString(*e);
      }
    }
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::datatype::boolean& e, std::string& s) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "boolean");

    s = tree->GetAttributeValue("value", false);

    if (s != sip::datatype::boolean::true_string) {
      s = sip::datatype::boolean::false_string;
    }

    assert(e.validate(s));

    /* Set to default if value is invalid */
    if (!(e.validate(s))) {
      s = sip::datatype::boolean::false_string;
    }
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::datatype::integer& e, std::string& s) {
    /* Current element must be <integer> */
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "integer");

    tree->GetAttributeOrDefault("minimum", &e.m_minimum, sip::datatype::integer::implementation_minimum);
    tree->GetAttributeOrDefault("maximum", &e.m_maximum, sip::datatype::integer::implementation_maximum);
    tree->GetAttributeOrDefault("default", &e.m_default_value, e.m_minimum);
    tree->GetAttributeOrDefault("value", &s, boost::lexical_cast < std::string > (e.m_default_value));

    assert(e.validate(s));

    /* Set to default if value is invalid */
    if (!(e.validate(s))) {
      s = "0";
    }
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::datatype::real& e, std::string& s) {
    /* Current element must be <integer> */
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "real");

    tree->GetAttributeOrDefault("minimum", &e.m_minimum, sip::datatype::integer::implementation_minimum);
    tree->GetAttributeOrDefault("maximum", &e.m_maximum, sip::datatype::integer::implementation_maximum);
    tree->GetAttributeOrDefault("default", &e.m_default_value, e.m_minimum);
    tree->GetAttributeOrDefault("value", &s, boost::lexical_cast < std::string > (e.m_default_value));

    assert(e.validate(s));

    /* Set to default if value is invalid */
    if (!(e.validate(s))) {
      s = "0";
    }
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::datatype::enumeration& e, std::string& s) {
    /* Current element must be <enumeration> */
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "enumeration");

    tree->GetAttribute("default", &e.m_default_value, false);

    for (ticpp::Element* ae = tree->FirstChildElement(false); ae != 0; ae = ae->NextSiblingElement(false)) {
      if (ae->Value() == "element") {
        e.add_value(ae->GetAttributeValue("value"));
      }
    }

    tree->GetAttributeOrDefault("value", &s, e.m_values[e.m_default_value]);

    assert(e.validate(s));
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::datatype::string& e, std::string& s) {
    /* Current element must be <string> */
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "string");

    tree->GetAttributeOrDefault("default", &e.m_default_value, "");
    tree->GetAttributeOrDefault("minimum", &e.m_minimum_length, 0);
    tree->GetAttributeOrDefault("maximum", &e.m_maximum_length, sip::datatype::string::implementation_maximum_length);

    if ((tree = tree->FirstChildElement(false))) {
      s = tree->GetText();
    }

    assert(e.validate(s));
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(boost::shared_ptr < sip::datatype::basic_datatype >& c, std::string& v) {
    std::string name(tree->Value());

    if (name == "enumeration") {
      c.reset(new sip::datatype::enumeration);
    }
    else if (name == "boolean") {
      c.reset(new sip::datatype::boolean);
    }
    else if (name == "integer") {
      c.reset(new sip::datatype::integer);
    }
    else if (name == "real") {
      c.reset(new sip::datatype::real);
    }
    else if (name == "uri") {
    }
    else if (name == "string") {
      c.reset(new sip::datatype::string);
    }
    else {
      /* Unknown type in configuration */
      throw std::runtime_error("Message received with unknown type: '" + name + "'");
    }

    do_visit(*c, v);
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::object& o) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "object");

    o.m_mime_type = sip::mime_type(tree->GetAttributeValue("mime-type"));
    
    tree->GetAttribute("location", &o.m_location, false);
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::option& o) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "option");

    for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
      /* The current element must be a valid datatype specification */
      std::pair < boost::shared_ptr < sip::datatype::basic_datatype >, std::string >      p;

      visitor< sip::restore_visitor_impl >(e).visit(p.first, p.second);

      o.m_arguments.push_back(p);
    }
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::configuration& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "configuration");

    /* reset object state */
    c.m_parameter_by_id.clear();
    c.m_options.clear();
    c.m_input_objects.clear();
    c.m_output_objects.clear();
    c.m_positions.clear();

    tree->GetAttributeOrDefault("fresh", &c.m_fresh, false);
    tree->GetAttribute("output-prefix", &c.m_output_prefix, false);
    tree->GetAttribute("category", &c.m_category);

    for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
      std::string identifier = e->GetAttributeValue("id");
     
      if (e->Value() == "option") {
        boost::shared_ptr < sip::option > o(new sip::option);

        visitor< sip::restore_visitor_impl >(e).visit(*o);

        c.add_option(identifier, o);
      }
      else if (e->Value() == "object") {
        boost::shared_ptr < sip::object > o(new sip::object);

        visitor< sip::restore_visitor_impl >(e).visit(*o);

        if (e->GetAttributeValue("type") == "input") {
          c.add_input(identifier, o);
        }
        else {
          c.add_output(identifier, o);
        }
      }
    }
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::tool::capabilities& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "capabilities");

    if (tree->Value() == "capabilities") {
      for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
     
        if (e->Value() == "protocol-version") {
          c.m_protocol_version.major = static_cast < unsigned char > (boost::lexical_cast < unsigned short > (e->GetAttributeValue("major")));
          c.m_protocol_version.minor = static_cast < unsigned char > (boost::lexical_cast < unsigned short > (e->GetAttributeValue("minor")));
        }
        else if (e->Value() == "input-configuration") {
          c.m_input_combinations.insert(
              sip::tool::capabilities::input_combination(
                  sip::tool::category::fit(e->GetAttributeValue("category")),
                  sip::mime_type(e->GetAttributeValue("format")), e->GetAttributeValue("identifier")));
        }
        else if (e->Value() == "output-configuration") {
          c.m_output_combinations.insert(
              sip::tool::capabilities::output_combination(
                      sip::mime_type(e->GetAttributeValue("format")), e->GetAttributeValue("identifier")));
        }
      }
    }
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::controller::capabilities& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "capabilities");

    if (tree->Value() == "capabilities") {
      for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
        if (e->Value() == "protocol-version") {
          tree->GetAttribute("major", &c.m_protocol_version.major);
          tree->GetAttribute("minor", &c.m_protocol_version.minor);
        }
        else if (e->Value() == "display-dimensions") {
          tree->GetAttribute("x", &c.m_dimensions.x, false);
          tree->GetAttribute("y", &c.m_dimensions.y, false);
          tree->GetAttribute("z", &c.m_dimensions.z, false);
        }
      }
    }
  }  

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::report& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "report");

    c.m_report_type = static_cast < sip::report::type > (boost::lexical_cast < unsigned int > (tree->GetAttributeValue("type")));

    c.description.clear();

    for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
      if (e->Value() == "description") {
        c.description += e->GetText();
      }
    }
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::layout::elements::label& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "label");
    
    c.m_text = tree->GetText();

    c.m_event_handler->process(&c);
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::layout::elements::button& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "button");

    tree->GetAttribute("label", &c.m_label);

    c.m_event_handler->process(&c);
  }

  template < typename T >
  std::istream& operator>>(std::istream& i, T*& t) {
    return (i >> reinterpret_cast < void*& > (t));
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::layout::elements::radio_button& c, sip::display::element_for_id& element_by_id) {
    using sip::layout::elements::radio_button;

    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "radio-button");

    tree->GetAttribute("label", &c.m_label);

    tree->GetAttributeOrDefault("connected", &c.m_connection, &c);
    tree->GetAttributeOrDefault("first", &c.m_first, false);
    tree->GetAttributeOrDefault("selected", &c.m_selected, false);

    if (c.m_connection != &c) {
      if (0 < element_by_id.count(reinterpret_cast < sip::layout::element_identifier > (c.m_connection))) {
        radio_button* i = &c;

        while (0 < element_by_id.count(reinterpret_cast < sip::layout::element_identifier > (i->m_connection))) {
          if (element_by_id[reinterpret_cast < sip::layout::element_identifier > (i->m_connection)] == &c) {
            i->m_connection = &c;
            i               = i->m_connection;

            while (i->m_connection != &c) {
              i->m_connection = static_cast < radio_button* > (element_by_id[reinterpret_cast < sip::layout::element_identifier > (i->m_connection)]);
              i               = i->m_connection;
            }

            if (c.m_selected) {
              /* Make sure all associated radio buttons are unselected */
              c.set_selected(true);
            }
         
            break;
          }

          i = static_cast < radio_button* > (element_by_id[reinterpret_cast < sip::layout::element_identifier > (i->m_connection)]);
        }
      }
    }
     
    c.m_event_handler->process(&c);
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::layout::elements::checkbox& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "checkbox");

    tree->GetAttribute("label", &c.m_label);
    tree->GetAttributeOrDefault("status", &c.m_status, false);

    c.m_event_handler->process(&c);
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::layout::elements::progress_bar& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "progress-bar");

    tree->GetAttribute("minimum", &c.m_minimum);
    tree->GetAttribute("maximum", &c.m_maximum);
    tree->GetAttribute("current", &c.m_current);

    c.m_event_handler->process(&c);
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::layout::elements::text_field& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "text-field");

    for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
      if (e->Value() == "text") {
        c.m_text = e->GetText();
      }
    }

    c.m_event_handler->process(&c);
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
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::layout::properties& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "properties");

    c.m_alignment_horizontal = text_to_horizontal_alignment(tree->GetAttributeValueOrDefault("horizontal-alignment", "right"));
    c.m_alignment_vertical   = text_to_vertical_alignment(tree->GetAttributeValueOrDefault("vertical-alignment", "bottom"));
    c.m_visible              = text_to_visibility(tree->GetAttributeValueOrDefault("visibility", "visible"));

    tree->GetAttribute("margin-top", &c.m_margin.top, false);
    tree->GetAttribute("margin-left", &c.m_margin.left, false);
    tree->GetAttribute("margin-bottom", &c.m_margin.bottom, false);
    tree->GetAttribute("margin-right", &c.m_margin.right, false);
    tree->GetAttribute("grow", &c.m_grow, false);
    tree->GetAttribute("enabled", &c.m_enabled, false);
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::layout::box&, sip::display::element_for_id&);

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(std::auto_ptr < sip::layout::manager >& c, sip::display::element_for_id& element_by_id) {
    std::string name(tree->Value());

    if (name == "box-layout-manager") {
      if (tree->GetAttributeValue("variant", false) == "vertical") {
        c.reset(new sip::layout::vertical_box());

      }
      else {
        c.reset(new sip::layout::horizontal_box());
      }

      visit(static_cast < sip::layout::box& > (*c), element_by_id);
    }
    else {
      throw std::runtime_error("Layout manager: '" + name + "' unknown");
    }
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::layout::horizontal_box& c, sip::display::element_for_id& element_by_id) {
    visit(static_cast < sip::layout::box& > (c), element_by_id);
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::layout::vertical_box& c, sip::display::element_for_id& element_by_id) {
    visit(static_cast < sip::layout::box& > (c), element_by_id);
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(std::auto_ptr < sip::layout::element >& c, sip::display::element_for_id& element_by_id);

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::layout::box& c, sip::display::element_for_id& element_by_id) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "box-layout-manager");

    sip::layout::properties current_properties = sip::layout::manager::default_properties;

    c.clear();

    for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
      if (e->Value() == "properties") {
        visitor< sip::restore_visitor_impl >(e).visit(current_properties);
      }
      else {
        sip::layout::element_identifier id;

        e->GetAttribute("id", &id, false);
       
        std::auto_ptr < sip::layout::element > p;

        visitor< sip::restore_visitor_impl >(e).visit(p, element_by_id);

        if (p.get() != 0) {
          element_by_id[id] = p.get();

          c.m_children.push_back(sip::layout::manager::layout_descriptor(p.release(), current_properties, id));
        }
      }
    }
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(std::auto_ptr < sip::layout::element >& c, sip::display::element_for_id& element_by_id) {
    using namespace sip::layout::elements;

    std::string name(tree->Value());

    if (name == "radio-button") {
      sip::layout::element_identifier id;

      tree->GetAttribute("id", &id, false);
      
      c.reset(new radio_button());

      element_by_id[id] = c.get();

      // Read concrete element data
      do_visit(*c, element_by_id);
    }
    else {
      if (name == "label") {
        c.reset(new label());
      }
      else if (name == "button") {
        c.reset(new button());
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

      if (c.get()) {
        // Read concrete element data
        do_visit(*c);
      }
      else {
        // Assume the element is a layout manager
        std::auto_ptr< sip::layout::manager > m;
         
        visit(m, element_by_id);
     
        if (m.get()) {
          c.reset(m.release()); 
        }
      }
    }
  }

  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::layout::tool_display& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "display-layout");

    if (tree->Value() == "display-layout") {
      c.m_element_by_id.clear();

      tree->GetAttribute("visible", &c.m_visible, false);
     
      for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
        if (e->Value() == "layout-manager" && !e->NoChildren()) {
          visitor< sip::restore_visitor_impl >(e->FirstChildElement(false)).visit(c.m_manager, c.m_element_by_id);
        }
      }
    }
  }

  /** \todo create and move to mediator / update visitors */
  template <>
  template <>
  void visitor< sip::restore_visitor_impl >::visit(sip::layout::tool_display& c, std::vector < sip::layout::element const* >& elements) {
  
    if (c.m_manager.get() != 0) {
      try {
        for (ticpp::Element* e = tree; e != 0; e = e->NextSiblingElement(false)) {
          sip::layout::element const* t = c.find(boost::lexical_cast < sip::layout::element_identifier > (e->GetAttributeValue("id")));

          if (t != 0) {
            sip::visitors::restore(*t, *e);

            elements.push_back(t);
          }
        }
      }
      catch (...) {
      }
    }
  }

  template <>
  bool visitor< sip::restore_visitor_impl >::initialise() {
    register_visit_method< sip::message >();
    register_visit_method< sip::datatype::boolean, std::string >();
    register_visit_method< sip::datatype::integer, std::string >();
    register_visit_method< sip::datatype::real, std::string >();
    register_visit_method< sip::datatype::enumeration, std::string >();
    register_visit_method< sip::datatype::string, std::string >();
    register_visit_method< sip::object >();
    register_visit_method< sip::option >();
    register_visit_method< sip::configuration >();
    register_visit_method< sip::controller::capabilities >();
    register_visit_method< sip::tool::capabilities >();
    register_visit_method< sip::report >();
    register_visit_method< sip::layout::tool_display >();
    register_visit_method< sip::layout::tool_display, std::vector< sip::layout::element const* > >();
    register_visit_method< sip::layout::elements::button >();
    register_visit_method< sip::layout::elements::checkbox >();
    register_visit_method< sip::layout::elements::label >();
    register_visit_method< sip::layout::elements::progress_bar >();
    register_visit_method< sip::layout::elements::radio_button, sip::display::element_for_id >();
    register_visit_method< sip::layout::elements::text_field >();
    register_visit_method< sip::layout::horizontal_box, sip::display::element_for_id >();
    register_visit_method< sip::layout::vertical_box, sip::display::element_for_id >();
    register_visit_method< sip::layout::properties >();

    return true;
  }
}
