#include <sip/sip_tool.h>

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

/* Single communication */
void report_to_xml() {
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

  BOOST_MESSAGE("Simple complete report: ");

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
r.to_xml(std::cerr);
  r.to_xml(temporary);

  m.set_content(temporary.str());

  c.send_message(m);

  d.await_message();

  std::ostringstream copy;

  report::from_xml(d.pop_message().to_string())->to_xml(copy);
  r.to_xml(temporary);

  BOOST_CHECK(copy.str() == temporary.str());

  c.disconnect(d);
}

test_suite* init_unit_test_suite( int argc, char * argv[] ) {
  using namespace boost::unit_test;

  test_suite* test = BOOST_TEST_SUITE( "SIP Communicator" );

  /* Change log parameters */
  unit_test_log_t::instance().set_threshold_level(log_messages);

  test->add(BOOST_TEST_CASE(&report_to_xml), 0, 2);

  return (test);
}

