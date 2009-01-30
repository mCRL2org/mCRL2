// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <sstream>
#include <iostream>
#include <cstdio>

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/unit_test_monitor.hpp>

#include <tipi/detail/basic_messenger.ipp>
#include "tipi/tool.hpp"
#include "tipi/controller.hpp"

#define BOOST_TEST_SHOW_PROGRESS yes

using boost::unit_test::test_suite;

using namespace transport;
using namespace tipi::messaging;

namespace cli = tipi::command_line_interface;

/* Serialisation, communication and deserialisation of a controller capabilities object */
void controller_capabilities_exchange() {
  BOOST_MESSAGE(" Communicating empty controller_capabilities object ... ");

  tipi::controller::communicator cc;
  tipi::tool::communicator       tc;

  std::ostringstream      check_stream0;
  std::ostringstream      check_stream1;

  /** Initiative is on the side of the tool */
  tc.connect(cc);

  cc.get_controller_capabilities().write(check_stream0);

  tc.request_controller_capabilities();
  tc.get_controller_capabilities()->write(check_stream1);

  tc.disconnect();

  BOOST_CHECK(check_stream0.str() == check_stream1.str());

  BOOST_MESSAGE("  done");
  BOOST_MESSAGE(" Communicating filled controller_capabilities object ... ");

  check_stream0.str("");
  check_stream1.str("");

  /* Example display dimensions */
  const_cast < tipi::controller::capabilities& > (cc.get_controller_capabilities()).set_display_dimensions(75,75,0);

  tc.connect(cc);

  tc.request_controller_capabilities();

  cc.get_controller_capabilities().write(check_stream0);
  tc.get_controller_capabilities()->write(check_stream1);

  tc.disconnect();

  BOOST_CHECK(check_stream0.str() == check_stream1.str());

  BOOST_MESSAGE("  done");
}

/* Serialisation, communication and deserialisation of an input configuration */
void tool_capabilities_exchange() {
  BOOST_MESSAGE(" Communicating tool capabilities ... ");

  tipi::controller::communicator cc;
  tipi::tool::communicator       tc;

  tipi::tool::capabilities&      tcp = tc.get_capabilities();

  tcp.add_input_configuration(0, "text/aut", "Testing");

  tc.connect(cc);

  cc.request_tool_capabilities();

  boost::shared_ptr< const tipi::message > m = cc.await_message(tipi::reply_tool_capabilities);

  cc.disconnect();

  tipi::tool::capabilities::ptr cp = tipi::tool::capabilities::read(m->to_string());

  BOOST_CHECK(tcp.write() == cp->write());
  BOOST_MESSAGE("  done");
}

/* Serialisation, communication and deserialisation of a report object */
void report_exchange() {
  BOOST_MESSAGE(" Communicating reports ... ");
  BOOST_MESSAGE("  Empty report ... ");

  std::ostringstream temporary;

  tipi::messenger c;
  tipi::messenger d;
  tipi::report    r;
  tipi::message   m;

  r.write(temporary);

  m.set_content(temporary.str());

  d.add_listener();
  c.connect();

  c.send_message(m);

  boost::shared_ptr< tipi::message > p(d.await_message(tipi::unknown));

  BOOST_CHECK(p->to_string() == std::string("<report></report>"));

  BOOST_MESSAGE("   done");
  BOOST_MESSAGE("  Report with an error, a comment and a configuration ... ");

  /* New tool configuration */
  tipi::configuration::ptr config(new tipi::configuration);

  const unsigned int r_option    = 0;
  const unsigned int s_option    = 1;
  const unsigned int f_option    = 2;
  const unsigned int input_file  = 3;
  const unsigned int output_file = 4;

  /* An option for specifying an input file */
  config->add_option(r_option);
  config->add_option(s_option);
  config->add_option(f_option);
  config->add_input(input_file, "text/mcrl2", "examples/abp.mcrl2");
  config->add_output(output_file, "text/plain", "/etc/passwd");

  tipi::configuration::option::ptr t = config->get_option(f_option);

  /* The option has a URI as argument */
//  t.append_argument(tipi::datatype::uri, std::string("/bin/bash"));
  t->append_argument(tipi::datatype::standard_string, std::string("/bin/bash"));
//  something.append_type(tipi::datatype::integer);
//  something.append_type(tipi::datatype::real);
//  something.append_type(tipi::datatype::enumeration);

  /* Add the option to the configuration */

  r.set_error("Everything okay!");
  r.set_comment("Lookin' good!");
  r.set_configuration(config);

  /* Serialise r */
  temporary.str(std::string(""));
  r.write(temporary);

  m.set_content(temporary.str());

  c.send_message(m);

  p = d.await_message(tipi::unknown);

  std::ostringstream copy;

  /* New reader */
  xml2pp::text_reader reader(p->to_string());

  tipi::report::read(reader)->write(copy);

  BOOST_CHECK(copy.str() == temporary.str());

  c.disconnect(d);

  BOOST_MESSAGE("   done");
}

void display_layout_exchange() {
  BOOST_MESSAGE(" Communicating display layout ... ");

  tipi::controller::communicator cc;
  tipi::tool::communicator       tc;

  std::ostringstream      check_stream0;
  std::ostringstream      check_stream1;

  tc.connect(cc);

  tc.disconnect();

  BOOST_MESSAGE("  done");
}

void command_line_interface_helper(int bc, int ac, char* const * const ba, const char* const * const aa, std::string m) {
  char* copy[bc];

  BOOST_MESSAGE("  " + m + "...");

  for (int i = 0; i < bc; ++i) {
    copy[i] = ba[i];
  }

  cli::argument_extractor t(bc, copy);

  for (int i = 0; i < ac; ++i) {
    BOOST_CHECK(strcmp(copy[i], aa[i]) == 0);
  }

  BOOST_MESSAGE("   done");
}

void command_line_interface() {
  char* arguments[7] = { "-i", "filename", "--si-connect=socket://localhost:55923", "--a", "filename", "--si-identifier=1", "--si-connect=traditional" };
  char* reference[4] = { "-i", "filename", "--a", "filename" };

  BOOST_MESSAGE(" Command line argument extraction... ");

  command_line_interface_helper(6, 4, arguments, reference, "Trying socket scheme");

  arguments[2] = "--si-connect=traditional";

  command_line_interface_helper(6, 4, arguments, reference, "Trying traditional scheme");

  command_line_interface_helper(7, 4, arguments, reference, "Multiple connect specifications");
}

test_suite* init_unit_test_suite(int argc, char * argv[]) {
  using namespace boost;
  using namespace boost::unit_test;

  test_suite* test = BOOST_TEST_SUITE( "SI protocol implementation" );

  /* Change log parameters */
  unit_test_log_t::instance().set_threshold_level(log_messages);

  test->add(BOOST_TEST_CASE(&controller_capabilities_exchange), 0, 2);
  test->add(BOOST_TEST_CASE(&tool_capabilities_exchange), 0, 2);
  test->add(BOOST_TEST_CASE(&report_exchange), 0, 2);
  test->add(BOOST_TEST_CASE(&command_line_interface), 0, 2);

  return (test);
}

