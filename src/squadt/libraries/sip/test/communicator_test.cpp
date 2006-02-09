#include <sip/detail/sip_communicator.h>

#include <iostream>

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>

#define BOOST_TEST_SHOW_PROGRESS yes

using boost::unit_test::test_suite;

using namespace transport;
using namespace sip;

static const std::string empty;
static const std::string garbage("garbage");

static const std::string data("1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.,:;&!~@#$%^*+-_=(){}[]|\\/><");
static const std::string start_message("<message>");
static const std::string end_message("</message>");

/* Single communication */
void no_message() {
  BOOST_MESSAGE("No message: ");

  sip_communicator c;
  sip_communicator d;

  c.connect(d);

  c.send(empty);

  c.disconnect(d);
}

/* Single communication */
void empty_message() {
  BOOST_MESSAGE("Empty message: ");

  sip_communicator c;
  sip_communicator d;

  d.add_listener();
  c.connect();

  c.send(start_message + end_message);

  d.await_message();

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string().empty());
}

/* Single communication */
void non_empty_message() {
  BOOST_MESSAGE("Non-empty message: ");

  sip_communicator c;
  sip_communicator d;

  c.connect(d);

  c.send(start_message + data + end_message);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data);
}

/* Two communications */
void split2_empty_message() {
  BOOST_MESSAGE("Empty message, two commmunications: ");

  sip_communicator c;
  sip_communicator d;

  c.connect(d);

  c.send(start_message);
  c.send(end_message);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string().empty());
}

/* Two communications */
void split2_non_empty_message() {
  BOOST_MESSAGE("Non-empty message, two communications: ");

  sip_communicator c;
  sip_communicator d;

  c.connect(d);

  c.send(start_message + data);
  c.send(end_message);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data);
}

/* Three communications */
void split3_non_empty_message() {
  BOOST_MESSAGE("Non-empty message, three communications: ");

  sip_communicator c;
  sip_communicator d;

  c.connect(d);

  c.send(start_message);
  c.send(data);
  c.send(end_message);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data);
}

/* Three communications */
void split3_non_empty_message_fragmented() {
  BOOST_MESSAGE("Non-empty fragmented message, three communications: ");

  sip_communicator c;
  sip_communicator d;

  c.connect(d);

  c.send(start_message + data);
  c.send(data);
  c.send(data + end_message);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data + data + data);
}

/* Three communications */
void split3_non_empty_message_and_garbage() {
  BOOST_MESSAGE("Non-empty fragmented message, three communications and garbage: ");

  sip_communicator c;
  sip_communicator d;

  c.connect(d);

  c.send(garbage + start_message + data);
  c.send(data);
  c.send(data + end_message + garbage);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data + data + data);
}

void message_wrapper() {
  BOOST_MESSAGE("Data wrapping and sending: ");

  sip_communicator c;
  sip_communicator d;
  message m(data);

  c.connect(d);

  c.send_message(m);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data);
}

test_suite* init_unit_test_suite( int argc, char * argv[] ) {
  using namespace boost::unit_test;

  test_suite* test = BOOST_TEST_SUITE( "SIP Communicator" );

  /* Change log parameters */
  unit_test_log_t::instance().set_threshold_level(log_messages);

  test->add(BOOST_TEST_CASE(&no_message), 0, 2);
  test->add(BOOST_TEST_CASE(&empty_message), 0, 2);
  test->add(BOOST_TEST_CASE(&non_empty_message), 0, 2);
  test->add(BOOST_TEST_CASE(&split2_empty_message), 0, 2);
  test->add(BOOST_TEST_CASE(&split2_non_empty_message), 0, 2);
  test->add(BOOST_TEST_CASE(&split3_non_empty_message), 0, 2);
  test->add(BOOST_TEST_CASE(&split3_non_empty_message_fragmented), 0, 2);
  test->add(BOOST_TEST_CASE(&split3_non_empty_message_and_garbage), 0, 2);
  test->add(BOOST_TEST_CASE(&message_wrapper), 0, 2);

  return (test);
}

