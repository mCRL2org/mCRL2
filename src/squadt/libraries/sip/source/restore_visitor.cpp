#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>

#include <utility/visitor.h>

#include <sip/visitors.h>
#include <sip/tool/capabilities.h>
#include <sip/controller/capabilities.h>
#include <sip/detail/basic_datatype.h>

#include <xml2pp/text_reader.h>

namespace sip {

  class restore_visitor_impl :
                        public utility::visitor < restore_visitor_impl, void, false >,
                        private boost::noncopyable {

    friend class visitors;
    friend class restore_visitor;

    private:

      /** \brief Alias for the first base class */
      typedef utility::visitor < restore_visitor_impl, void, false > base;

    private:

      /** \brief Points to interface object */
      restore_visitor&     interface;

      /** \brief The source of input */
      xml2pp::text_reader& in;

    protected:

      /** \brief Reads from string */
      restore_visitor_impl(restore_visitor&, xml2pp::text_reader&);

    public:

      template < typename T >
      void visit(T&);
  };

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
  restore_visitor::restore_visitor(xml2pp::text_reader& s) {
    impl.reset(new restore_visitor_impl(*this, s));
  }

  restore_visitor::restore_visitor(std::string const& s) {
    impl.reset(new restore_visitor_alternate_impl(*this, s));
  }

  restore_visitor::restore_visitor(boost::filesystem::path const& p) {
    impl.reset(new restore_visitor_alternate_impl(*this, p));
  }

  template < typename T >
  void restore_visitor::visit(T& t) {
    impl->visit(t);
  }

  template void restore_visitor::visit(sip::configuration&);
  template void restore_visitor::visit(sip::tool::capabilities&);
  template void restore_visitor::visit(sip::controller::capabilities&);
  template void restore_visitor::visit(boost::tuple < sip::datatype::string&, std::string& >&);
  template void restore_visitor::visit(boost::tuple < sip::datatype::enumeration&, std::string& >&);
  template void restore_visitor::visit(boost::tuple < sip::datatype::boolean&, std::string& >&);
  template void restore_visitor::visit(boost::tuple < sip::datatype::integer&, std::string& >&);
  template void restore_visitor::visit(boost::tuple < sip::datatype::real&, std::string& >&);


  inline restore_visitor_impl::restore_visitor_impl(restore_visitor& i, xml2pp::text_reader& s) : interface(i), in(s) {
  }

  inline restore_visitor_alternate_impl::restore_visitor_alternate_impl(restore_visitor& i, std::string const& s) : restore_visitor_impl(i, in), in(s) {
  }

  inline restore_visitor_alternate_impl::restore_visitor_alternate_impl(restore_visitor& i, boost::filesystem::path const& p) : restore_visitor_impl(i, in), in(p) {
  }

  template <>
  void restore_visitor_impl::visit(boost::tuple < boost::shared_ptr < sip::datatype::basic_datatype >&, std::string& >&);

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
        boost::tuple < boost::shared_ptr < sip::datatype::basic_datatype >&, std::string& > lp(p.first, p.second);

        visit(lp);

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
  void restore_visitor_impl::visit(boost::tuple < sip::datatype::boolean&, std::string& >& o) {
    assert(in.is_element("boolean"));

    o.get< 1 >() = in.get_attribute_as_string("value");

    if (o.get< 1 >() != sip::datatype::boolean::true_string) {
      o.get< 1 >() = sip::datatype::boolean::false_string;
    }

    in.next_element();
    in.skip_end_element("boolean");

    assert(o.get< 0 >().validate(o.get< 1 >()));

    /* Set to default if value is invalid */
    if (!(o.get< 0 >().validate(o.get< 1 >()))) {
      o.get< 1 >() = sip::datatype::boolean::false_string;
    }
  }

  template <>
  void restore_visitor_impl::visit(boost::tuple < sip::datatype::integer&, std::string& >& o) {
    /* Current element must be <integer> */
    assert(in.is_element("integer"));

    o.get< 0 >().m_minimum = datatype::integer::implementation_minimum;
    o.get< 0 >().m_maximum = datatype::integer::implementation_maximum;

    in.get_attribute(&o.get< 0 >().m_minimum, "minimum");
    in.get_attribute(&o.get< 0 >().m_maximum, "maximum");

    o.get< 0 >().m_default_value = o.get< 0 >().m_minimum;

    if (in.get_attribute(&o.get< 0 >().m_default_value, "default")) {
      o.get< 1 >() = o.get< 0 >().m_default_value;
    }

    o.get< 1 >() = in.get_attribute_as_string("value");

    in.next_element();
    in.skip_end_element("integer");

    assert(o.get< 0 >().validate(o.get< 1 >()));

    /* Set to default if value is invalid */
    if (!(o.get< 0 >().validate(o.get< 1 >()))) {
      o.get< 1 >() = "0";
    }
  }

  template <>
  void restore_visitor_impl::visit(boost::tuple < sip::datatype::real&, std::string& >& o) {
    /* Current element must be <integer> */
    assert(in.is_element("real"));

    o.get< 0 >().m_minimum = datatype::real::implementation_minimum;
    o.get< 0 >().m_maximum = datatype::real::implementation_maximum;

    in.get_attribute(&o.get< 0 >().m_minimum, "minimum");
    in.get_attribute(&o.get< 0 >().m_maximum, "maximum");

    o.get< 0 >().m_default_value = o.get< 0 >().m_minimum;

    if (in.get_attribute(&o.get< 0 >().m_default_value, "default")) {
      o.get< 1 >() = boost::lexical_cast < std::string > (o.get< 0 >().m_default_value);
    }

    o.get< 1 >() = in.get_attribute_as_string("value");

    in.next_element();
    in.skip_end_element("real");

    assert(o.get< 0 >().validate(o.get< 1 >()));

    /* Set to default if value is invalid */
    if (!(o.get< 0 >().validate(o.get< 1 >()))) {
      o.get< 1 >() = "0";
    }
  }

  template <>
  void restore_visitor_impl::visit(boost::tuple < sip::datatype::enumeration&, std::string& >& o) {
    /* Current element must be <enumeration> */
    assert(in.is_element("enumeration"));

    in.get_attribute(&o.get< 0 >().m_default_value, "default");

    o.get< 1 >() = in.get_attribute_as_string("value");

    in.next_element();

    while (!in.is_end_element("enumeration")) {
      /* Assume element */
      o.get< 0 >().add_value(in.get_attribute_as_string("value"));

      in.next_element();
      in.skip_end_element("element");
    }

    in.skip_end_element("enumeration");

    if (o.get< 1 >().empty()) {
      o.get< 1 >() = o.get< 0 >().m_values[o.get< 0 >().m_default_value];
    }

    assert(o.get< 0 >().validate(o.get< 1 >()));
  }

  template <>
  void restore_visitor_impl::visit(boost::tuple < datatype::string&, std::string& >& o) {
    /* Current element must be <string> */
    assert(in.is_element("string"));

    o.get< 0 >().m_minimum_length = 0;
    o.get< 0 >().m_maximum_length = sip::datatype::string::implementation_maximum_length;

    in.get_attribute(&o.get< 1 >(), "default");
    in.get_attribute(&o.get< 0 >().m_minimum_length, "minimum-length");
    in.get_attribute(&o.get< 0 >().m_maximum_length, "maximum-length");

    if (!in.is_empty_element()) {
      in.next_element();

      in.get_value(&o.get< 1 >());
    }

    in.next_element();
    in.skip_end_element("string");

    assert(o.get< 0 >().validate(o.get< 1 >()));
  }

  template < typename T >
  inline void visit_tuple_datatype(restore_visitor_impl& v, boost::tuple < boost::shared_ptr < sip::datatype::basic_datatype >&, std::string& >& o) {
    o.get< 0 >().reset(new T);

    boost::tuple < T&, std::string& > lp(*(boost::static_pointer_cast < T > (o.get< 0 >())), o.get< 1 >());

    v.visit(lp);
  }

  template <>
  void restore_visitor_impl::visit(boost::tuple < boost::shared_ptr < sip::datatype::basic_datatype >&, std::string& >& o) {

    if (in.is_element("enumeration")) {
      visit_tuple_datatype< sip::datatype::enumeration >(*this, o);
    }
    else if (in.is_element("boolean")) {
      visit_tuple_datatype< sip::datatype::boolean >(*this, o);
    }
    else if (in.is_element("integer")) {
      visit_tuple_datatype< sip::datatype::integer >(*this, o);
    }
    else if (in.is_element("real")) {
      visit_tuple_datatype< sip::datatype::real >(*this, o);
    }
    else if (in.is_element("uri")) {
//      return (uri::read(r));
    }
    else if (in.is_element("string")) {
      visit_tuple_datatype< sip::datatype::string >(*this, o);
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
}
