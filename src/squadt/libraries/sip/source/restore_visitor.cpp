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

  class restore_visitor_impl_frontend : public restore_visitor_impl {

    private:

      /** \brief The source of input */
      ticpp::Document in;

    public:

      /** \brief Reads from stream */
      restore_visitor_impl_frontend(restore_visitor&, std::istream& s);

      /** \brief Reads from string */
      restore_visitor_impl_frontend(restore_visitor&, std::string const&);

      /** \brief Reads from file */
      restore_visitor_impl_frontend(restore_visitor&, boost::filesystem::path const&);
  };

  template < >
  restore_visitor::restore_visitor(ticpp::Element& s) :
        utility::visitor_interface< restore_visitor_impl, false >(boost::shared_ptr < visitor_type > (new restore_visitor_impl(*this, &s))) {
  }

  restore_visitor::restore_visitor(std::string const& s) :
        utility::visitor_interface< restore_visitor_impl, false >(boost::shared_ptr < visitor_type > (new restore_visitor_impl_frontend(*this, s))) {
  }

  restore_visitor::restore_visitor(boost::filesystem::path const& p) :
        utility::visitor_interface< restore_visitor_impl, false >(boost::shared_ptr < visitor_type > (new restore_visitor_impl_frontend(*this, p))) {
  }

  inline restore_visitor_impl::restore_visitor_impl(restore_visitor& i) : m_interface(i) {
  }

  inline restore_visitor_impl::restore_visitor_impl(restore_visitor& i, ticpp::Element* s) : m_interface(i), tree(s) {
  }

  inline restore_visitor_impl& restore_visitor_impl::visit_tree(ticpp::Element* s) {
    tree = s;

    return (*this);
  }

  inline restore_visitor_impl_frontend::restore_visitor_impl_frontend(restore_visitor& i, std::istream& s) : restore_visitor_impl(i) {
    s >> in;

    tree = in.FirstChildElement();
  }

  inline restore_visitor_impl_frontend::restore_visitor_impl_frontend(restore_visitor& i, std::string const& s) : restore_visitor_impl(i) {
    std::istringstream ins(s);

    ins >> in;

    tree = in.FirstChildElement();
  }

  inline restore_visitor_impl_frontend::restore_visitor_impl_frontend(restore_visitor& i, boost::filesystem::path const& p) : restore_visitor_impl(i) {
    std::ifstream ins(p.native_file_string().c_str());

    ins >> in;

    tree = in.FirstChildElement();
  }

  template <>
  void restore_visitor_impl::visit(boost::shared_ptr < sip::datatype::basic_datatype >&, std::string&);

  std::istream& operator >> (std::istream& s, sip::message::type_identifier_t& id) {
    size_t t;

    s >> t;

    id = static_cast < sip::message::type_identifier_t > (t);

    return (s);
  }

  template <>
  void restore_visitor_impl::visit(sip::message& o) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "message-meta");

    tree->GetAttributeOrDefault("type", &o.m_type, sip::message::message_unknown);

    o.m_content.assign(tree->GetText());
  }

  template <>
  void restore_visitor_impl::visit(sip::object& o) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "object");

    o.m_mime_type = mime_type(tree->GetAttributeValue("mime-type"));
    
    tree->GetAttribute("location", &o.m_location);
  }

  template <>
  void restore_visitor_impl::visit(sip::option& o) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "option");

    if (!tree->NoChildren() && (tree = tree->FirstChildElement(false))) {
      for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e->NextSiblingElement(false)) {
        /* The current element must be a valid datatype specification */
        std::pair < boost::shared_ptr < sip::datatype::basic_datatype >, std::string >      p;

        visit(p.first, p.second);

        o.m_arguments.push_back(p);
      }
    }
  }

  /**
   * \param reader is a reference to a libXML 2 text reader instance
   * /pre the reader points to a \<configuration\> instance
   * /post the readers position is just past the configuration block
   **/
  template <>
  void restore_visitor_impl::visit(sip::configuration& c) {
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
        boost::shared_ptr < option > o(new sip::option);

        o->accept(*this);

        c.add_option(identifier, o);
      }
      else if (e->Value() == "object") {
        boost::shared_ptr < object > o(new sip::object);

        o->accept(visit_tree(e));

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
  void restore_visitor_impl::visit(sip::datatype::boolean& e, std::string& s) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "boolean");

    s = tree->GetAttributeValue("value");

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
  void restore_visitor_impl::visit(sip::datatype::integer& e, std::string& s) {
    /* Current element must be <integer> */
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "integer");

    tree->GetAttributeOrDefault("minimum", &e.m_minimum, datatype::integer::implementation_minimum);
    tree->GetAttributeOrDefault("maximum", &e.m_maximum, datatype::integer::implementation_maximum);
    tree->GetAttributeOrDefault("default", &e.m_default_value, e.m_minimum);
    tree->GetAttributeOrDefault("value", &s, boost::lexical_cast < std::string > (e.m_default_value));

    assert(e.validate(s));

    /* Set to default if value is invalid */
    if (!(e.validate(s))) {
      s = "0";
    }
  }

  template <>
  void restore_visitor_impl::visit(sip::datatype::real& e, std::string& s) {
    /* Current element must be <integer> */
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "real");

    tree->GetAttributeOrDefault("minimum", &e.m_minimum, datatype::integer::implementation_minimum);
    tree->GetAttributeOrDefault("maximum", &e.m_maximum, datatype::integer::implementation_maximum);
    tree->GetAttributeOrDefault("default", &e.m_default_value, e.m_minimum);
    tree->GetAttributeOrDefault("value", &s, boost::lexical_cast < std::string > (e.m_default_value));

    assert(e.validate(s));

    /* Set to default if value is invalid */
    if (!(e.validate(s))) {
      s = "0";
    }
  }

  template <>
  void restore_visitor_impl::visit(sip::datatype::enumeration& e, std::string& s) {
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
  void restore_visitor_impl::visit(sip::datatype::string& e, std::string& s) {
    /* Current element must be <string> */
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "string");

    tree->GetAttributeOrDefault("default", &e.m_default_value, "");
    tree->GetAttributeOrDefault("minimum", &e.m_minimum_length, 0);
    tree->GetAttributeOrDefault("maximum", &e.m_maximum_length, datatype::string::implementation_maximum_length);

    if ((tree = tree->FirstChildElement(false))) {
      s = tree->GetText();
    }

    assert(e.validate(s));
  }

  template < typename T >
  inline void visit_tuple_datatype(restore_visitor_impl* v, boost::shared_ptr < sip::datatype::basic_datatype >& c, std::string& l) {
    c.reset(new T);

    v->visit(*(boost::static_pointer_cast < T > (c)), l);
  }

  template <>
  void restore_visitor_impl::visit(boost::shared_ptr < sip::datatype::basic_datatype >& c, std::string& v) {

    std::string name(tree->Value());

    if (name == "enumeration") {
      visit_tuple_datatype< sip::datatype::enumeration >(this, c, v);
    }
    if (name == "boolean") {
      visit_tuple_datatype< sip::datatype::boolean >(this, c, v);
    }
    if (name == "integer") {
      visit_tuple_datatype< sip::datatype::integer >(this, c, v);
    }
    if (name == "real") {
      visit_tuple_datatype< sip::datatype::real >(this, c, v);
    }
    if (name == "uri") {
//      return (uri::read(r));
    }
    if (name == "string") {
      visit_tuple_datatype< sip::datatype::string >(this, c, v);
    }
    else {
      /* Unknown type in configuration */
      throw (sip::exception(sip::message_unknown_type, name));
    }
  }

  template <>
  void restore_visitor_impl::visit(sip::tool::capabilities& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "capabilities");

    if (tree->Value() == "capabilities") {
      for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
     
        if (e->Value() == "protocol-version") {
          c.m_protocol_version.major = boost::lexical_cast < unsigned short > (e->GetAttributeValue("major"));
          c.m_protocol_version.minor = boost::lexical_cast < unsigned short > (e->GetAttributeValue("minor"));
        }
        else if (e->Value() == "input-configuration") {
          c.m_input_combinations.insert(
              sip::tool::capabilities::input_combination(
                  tool::category::fit(e->GetAttributeValue("category")),
                  mime_type(e->GetAttributeValue("format")), e->GetAttributeValue("identifier")));
        }
        else if (e->Value() == "output-configuration") {
          c.m_output_combinations.insert(
              sip::tool::capabilities::output_combination(
                      mime_type(e->GetAttributeValue("format")), e->GetAttributeValue("identifier")));
        }
      }
    }
  }

  template <>
  void restore_visitor_impl::visit(sip::controller::capabilities& c) {
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
  void restore_visitor_impl::visit(sip::report& c) {
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
  void restore_visitor_impl::visit(sip::layout::elements::label& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "label");
    
    c.m_text = tree->GetText();

    c.m_event_handler->process(&c);
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::elements::button& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "button");

    tree->GetAttribute("label", &c.m_label);

    c.m_event_handler->process(&c);
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::elements::radio_button& c, sip::display::element_for_id& element_by_id) {
    using sip::layout::elements::radio_button;

    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "radio-button");

    tree->GetAttribute("label", &c.m_label);

    sip::layout::element_identifier connected_to =
                boost::lexical_cast < sip::layout::element_identifier > (
                        tree->GetAttributeValueOrDefault("connected", boost::lexical_cast < std::string > (&c)));

    tree->GetAttributeOrDefault("first", &c.m_first, false);
    tree->GetAttributeOrDefault("selected", &c.m_selected, false);

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

    c.m_event_handler->process(&c);
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::elements::checkbox& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "checkbox");

    tree->GetAttribute("label", &c.m_label);
    tree->GetAttributeOrDefault("status", &c.m_status, false);

    c.m_event_handler->process(&c);
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::elements::progress_bar& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "progress-bar");

    tree->GetAttribute("minimum", &c.m_minimum);
    tree->GetAttribute("maximum", &c.m_maximum);
    tree->GetAttribute("current", &c.m_current);

    c.m_event_handler->process(&c);
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::elements::text_field& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "text-field");

    if (!tree->NoChildren()) {
      c.m_text = tree->GetText();
    }

    c.m_event_handler->process(&c);
  }

  template <>
  void restore_visitor_impl::visit(std::auto_ptr < sip::layout::element >& c, sip::layout::element_identifier& id) {
    using namespace sip::layout::elements;

    std::string name(tree->Value());

    tree->GetAttribute("id", &id);

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
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "box-layout-manager");

    sip::layout::properties current_properties = sip::layout::manager::default_properties;

    c.clear();

    for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
      if (e->Value() == "properties") {
        current_properties.accept(visit_tree(e));
      }
      else {
        std::auto_ptr < sip::layout::element > p;
        sip::layout::element_identifier        id;

        visit(p, id);

        element_by_id[id] = p.release();

        c.m_children[id] = std::make_pair(element_by_id[id], current_properties);
      }
    }
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
    std::string name(tree->Value());

    if (name == "box-layout-manager") {
      if (tree->GetAttributeValue("variant", false) == "vertical") {
        c.reset(new sip::layout::vertical_box());

        visit(static_cast < sip::layout::vertical_box& > (*c), element_by_id);
      }
      else {
        c.reset(new sip::layout::horizontal_box());

        visit(static_cast < sip::layout::horizontal_box& > (*c), element_by_id);
      }
    }
    else {
      throw (new sip::exception(sip::unknown_layout_element, name));
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
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "properties");

    c.m_alignment_horizontal = text_to_horizontal_alignment(tree->GetAttributeValueOrDefault("horizontal-alignment", "right"));
    c.m_alignment_vertical   = text_to_vertical_alignment(tree->GetAttributeValueOrDefault("vertical-alignment", "bottom"));
    c.m_visible              = text_to_visibility(tree->GetAttributeValueOrDefault("visibility", "visible"));

    tree->GetAttribute("margin-top", &c.m_margin.top);
    tree->GetAttribute("margin-left", &c.m_margin.left);
    tree->GetAttribute("margin-bottom", &c.m_margin.bottom);
    tree->GetAttribute("margin-right", &c.m_margin.right);
    tree->GetAttribute("grow", &c.m_grow);
    tree->GetAttribute("enabled", &c.m_enabled);
  }

  template <>
  void restore_visitor_impl::visit(sip::layout::tool_display& c) {
    assert((tree->Type() == TiXmlNode::ELEMENT) && tree->Value() == "display-layout");

    if (tree->Value() == "display-layout") {
      tree->GetAttribute("visible", &c.m_visible, false);
     
      if (!tree->NoChildren()) {
        visit_tree(tree->FirstChildElement(false)).visit(c.m_manager, c.m_element_by_id);
      }
    }
  }
}

template void utility::visitor< sip::restore_visitor_impl, false >::visit(sip::layout::tool_display& c, std::vector < sip::layout::element const* >& elements);

namespace sip {
  /** \todo create and move to mediator / update visitors */
  template <>
  void restore_visitor_impl::visit(sip::layout::tool_display& c, std::vector < sip::layout::element const* >& elements) {
  
    if (c.m_manager.get() != 0) {

      for (ticpp::Element* e = tree->FirstChildElement(false); e != 0; e = e->NextSiblingElement(false)) {
        try {
          sip::layout::element_identifier id = boost::lexical_cast < sip::layout::element_identifier > (e->GetAttributeValue("id")); 
         
          if (c.m_element_by_id.count(id)) {
       
            sip::layout::element* t = c.m_element_by_id[id];
        
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
  }
}
