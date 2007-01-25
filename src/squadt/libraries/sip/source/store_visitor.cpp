#include <sstream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/noncopyable.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <boost/tuple/tuple.hpp>

#include <utility/visitor.h>

#include <sip/visitors.h>
#include <sip/controller/capabilities.h>
#include <sip/tool/capabilities.h>

#include <xml2pp/text_reader.h>

namespace sip {

  class store_visitor_impl : public utility::visitor < store_visitor_impl >,
                             private boost::noncopyable {

    friend class visitors;
    friend class store_visitor;

    private:

      /** \brief Alias for the first base class */
      typedef utility::visitor < store_visitor_impl > base;

    private:

      /** \brief Points to interface object */
      store_visitor&  m_interface;

      /** \brief The destination of output */
      std::ostream&   out;

    protected:

      /** \brief Writes to stream */
      store_visitor_impl(store_visitor&, std::ostream&);

    public:

      template < typename T >
      void visit(T const&);
  };

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
  
  store_visitor::store_visitor(std::string& s) {
    impl.reset(new store_visitor_string_impl(*this, s));
  }

  store_visitor::store_visitor(boost::filesystem::path const& p) {
    impl.reset(new store_visitor_path_impl(*this, p));
  }

  store_visitor::store_visitor(std::ostream& o) {
    impl.reset(new store_visitor_impl(*this, o));
  }

  template < typename T >
  void store_visitor::visit(T const& t) {
    impl->visit(t);
  }

  template void store_visitor::visit(sip::tool::capabilities const&);
  template void store_visitor::visit(sip::controller::capabilities const&);
  template void store_visitor::visit(sip::configuration const&);
  template void store_visitor::visit(sip::option const&);
  template void store_visitor::visit(sip::object const&);
  template void store_visitor::visit(boost::tuple < sip::datatype::string const&, std::string const& > const&);
  template void store_visitor::visit(boost::tuple < sip::datatype::enumeration const&, std::string const& > const&);
  template void store_visitor::visit(boost::tuple < sip::datatype::boolean const&, std::string const& > const&);
  template void store_visitor::visit(boost::tuple < sip::datatype::integer const&, std::string const& > const&);
  template void store_visitor::visit(boost::tuple < sip::datatype::real const&, std::string const& > const&);

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
          boost::tuple < sip::datatype::basic_datatype&, std::string& > p(*(i.first), i.second);

          i.first->accept(m_interface, p);
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
  void store_visitor_impl::visit(boost::tuple < sip::datatype::boolean const&, std::string const& > const& o) {
    out << "<boolean";
    
    if (o.get< 1 >().compare(sip::datatype::boolean::true_string) == 0) {
      out << " value=\"" << o.get< 1 >() << "\"";
    }

    out << "/>";
  }

  template <>
  void store_visitor_impl::visit(boost::tuple < sip::datatype::integer const&, std::string const& > const& o) {
    out << "<integer";

    if (!o.get< 1 >().empty()) {
      out << " value=\"" << std::dec << o.get< 1 >() << "\"";
    }

    if (o.get< 0 >().m_minimum != sip::datatype::integer::implementation_minimum) {
      out << " minimum=\"" << o.get< 0 >().m_minimum << "\"";
    }
      
    if (o.get< 0 >().m_maximum != sip::datatype::integer::implementation_maximum) {
      out << " maximum=\"" << o.get< 0 >().m_maximum << "\"";
    }

    if (o.get< 0 >().m_default_value != o.get< 0 >().m_minimum) {
      out << " default=\"" << o.get< 0 >().m_default_value << "\"";
    }

    out << "/>";
  }

  template <>
  void store_visitor_impl::visit(boost::tuple < sip::datatype::real const&, std::string const& > const& o) {
    out << "<real";

    if (!o.get< 1 >().empty()) {
      out << " value=\"" << std::dec << o.get< 1 >() << "\"";
    }

    if (o.get< 0 >().m_minimum != sip::datatype::real::implementation_minimum) {
      out << " minimum=\"" << o.get< 0 >().m_minimum << "\"";
    }
      
    if (o.get< 0 >().m_maximum != sip::datatype::real::implementation_maximum) {
      out << " maximum=\"" << o.get< 0 >().m_maximum << "\"";
    }

    if (o.get< 0 >().m_default_value != o.get< 0 >().m_minimum) {
      out << " default=\"" << o.get< 0 >().m_default_value << "\"";
    }

    out << "/>";
  }

  template <>
  void store_visitor_impl::visit(boost::tuple < sip::datatype::enumeration const&, std::string const& > const& o) {
    out << "<enumeration value=\"" << o.get< 1 >();

    if (o.get< 0 >().m_default_value != 0) {
      out << "\" default=\"" << o.get< 0 >().m_default_value;
    }

    out << "\">";

    for (std::vector < std::string >::const_iterator i = o.get< 0 >().m_values.begin(); i != o.get< 0 >().m_values.end(); ++i) {
      out << "<element value=\"" << *i << "\"/>";
    }

    out << "</enumeration>";
  }

  template <>
  void store_visitor_impl::visit(boost::tuple < sip::datatype::string const&, std::string const& > const& o) {
    assert(!boost::regex_search(o.get< 1 >(), boost::regex("]]>")));

    out << "<string";

    if (o.get< 0 >().m_minimum_length != 0) {
      out << " minimum-length=\"" << o.get< 0 >().m_minimum_length << "\"";
    }
    if (o.get< 0 >().m_maximum_length != sip::datatype::string::implementation_maximum_length) {
      out << " maximum-length=\"" << o.get< 0 >().m_maximum_length << "\"";
    }

    if (!o.get< 0 >().m_default_value.empty()) {
      out << " default=\"" << o.get< 0 >().m_default_value << "\"";
    }

    if (!o.get< 1 >().empty()) {
      out << "><![CDATA[" << o.get< 1 >() << "]]></string>";
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
  void store_visitor_impl::visit(sip::controller::capabilities const& c) {
    out << "<capabilities>"
        << "<protocol-version major=\"" << (unsigned short) c.m_protocol_version.major
        << "\" minor=\"" << (unsigned short) c.m_protocol_version.minor << "\"/>"
        << "<display-dimensions x=\"" << c.m_dimensions.x
        << "\" y=\"" << c.m_dimensions.y
        << "\" z=\"" << c.m_dimensions.z << "\"/>"
        << "</capabilities>";
  }
}
