#include <sip/sip_tool.h>
#include <sip/sip_controller.h>

#include <sstream>
#include <iostream>

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>

#define BOOST_TEST_SHOW_PROGRESS yes

using boost::unit_test::test_suite;

using namespace transport;
using namespace sip;
using namespace sip::communicator;

/* Serialisation and deserialisation of a controller capabilities object */
void controller_capabilities_serialisation() {
  BOOST_MESSAGE(" Empty controller_capabilities object: ");

  controller_communicator cc;
  tool_communicator       tc;

  /** Initiative is on the side of the tool */
  tc.connect(cc);

  tc.request_controller_capabilities();

  tc.disconnect();

  BOOST_MESSAGE(" Filled controller_capabilities object: ");

  /* Example display dimensions */
  controller_capabilities capabilities();

  cc.set_display_dimensions(50,50,0);

  tc.add_input_configuration("Testing", "aut");
  tc.add_input_configuration("Testing", "svc");

  tc.connect(cc);

  tc.disconnect();
}

/* Serialisation and deserialisation of a report object */
void report_serialisation() {
  BOOST_MESSAGE(" Empty report object: ");
  BOOST_MESSAGE(" Filled report object: ");
}

/* Single communication */
void report_communication() {
  BOOST_MESSAGE("Empty report: ");

  std::ostringstream temporary;

  sip_communicator   c;
  sip_communicator   d;
  report             r;
  message            m;

  r.to_xml(temporary);

  m.set_content(temporary.str());

  d.add_listener();
  c.connect();

  c.send_message(m);

  d.await_message();

  BOOST_CHECK(d.pop_message().to_string() == std::string("<report></report>"));

  BOOST_MESSAGE("Simple report: ");

  /* New tool configuration */
  configuration* config = new configuration;

  /* An option for specifying an input file */
  config->add_option(std::string("Input file 0"));
  config->add_option(std::string("-r"));
  config->add_option(std::string("-s"));

  option::option_ptr t = config->get_option("Input file 0");

  /* The option has a URI as argument */
//  t.append_argument(sip::datatype::uri, std::string("/bin/bash"));
  t->append_argument(sip::datatype::standard_string, std::string("/bin/bash"));
//  something.append_type(sip::datatype::integer);
//  something.append_type(sip::datatype::real);
//  something.append_type(sip::datatype::enumeration);

  /* Add the option to the configuration */

  r.set_error("Everything okay!");
  r.set_comment("Lookin' good!");
  r.add_output("/etc/passwd", "text/plain");
  r.set_configuration(config);

  /* Serialise r */
  temporary.str(std::string(""));
  r.to_xml(temporary);

  m.set_content(temporary.str());

  c.send_message(m);

  d.await_message();

  std::ostringstream copy;

std::cerr << " Reconstruction from XML stream ..." << std::endl;
std::cerr << "  Message  `" << d.peek_message().to_string() << "'" << std::endl;
  temporary.str(std::string(""));
  r.to_xml(temporary);

  /* New reader */
  xml2pp::text_reader reader(d.peek_message().to_string());

  report::from_xml(reader)->to_xml(copy);
std::cerr << "  Message  `" << copy.str() << "'" << std::endl;

  BOOST_CHECK(copy.str() == temporary.str());

  c.disconnect(d);
}

test_suite* init_unit_test_suite( int argc, char * argv[] ) {
  using namespace boost::unit_test;

  test_suite* test = BOOST_TEST_SUITE( "SIP Communicator" );

  /* Change log parameters */
  unit_test_log_t::instance().set_threshold_level(log_messages);

  test->add(BOOST_TEST_CASE(&controller_capabilities_serialisation), 0, 2);
  test->add(BOOST_TEST_CASE(&report_serialisation), 0, 2);
  test->add(BOOST_TEST_CASE(&report_communication), 0, 2);

  return (test);
}

