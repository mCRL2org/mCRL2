#include <iostream>

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>

#include <sip/detail/basic_messenger.tcc>

#define BOOST_TEST_SHOW_PROGRESS yes

using boost::unit_test::test_suite;

using namespace transport;
using namespace sip;
using namespace sip::messenger;

static const std::string empty;
static const std::string garbage("garbage");

static const std::string data("1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.,:;&!~@#$%^*+-_=(){}[]|\\/><");
static const std::string start_message("<message>");
static const std::string end_message("</message>");

enum message_identifier_t {
  unknown,
  good,
  bad
};

typedef message < message_identifier_t, ::unknown > message_t;
typedef basic_messenger < message_t >               messenger_t;

/* Single communication */
void no_message() {
  BOOST_MESSAGE("No message ...");

  messenger_t c;
  messenger_t d;

  c.connect(d);

  c.send(empty);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 0);

  BOOST_MESSAGE(" done");
}

/* Single communication */
void empty_message() {
  BOOST_MESSAGE("Empty message ...");

  messenger_t c;
  messenger_t d;

  c.connect(d);

  c.send(start_message + end_message);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string().empty());

  BOOST_MESSAGE(" done");
}

/* Single communication */
void non_empty_message() {
  BOOST_MESSAGE("Non-empty message ...");

  messenger_t c;
  messenger_t d;

  c.connect(d);

  c.send(start_message + data + end_message);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data);

  BOOST_MESSAGE(" done");
}

/* Two communications */
void split2_empty_message() {
  BOOST_MESSAGE("Empty message, two commmunications ...");

  messenger_t c;
  messenger_t d;

  c.connect(d);

  c.send(start_message);
  c.send(end_message);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string().empty());

  BOOST_MESSAGE(" done");
}

/* Two communications */
void split2_non_empty_message() {
  BOOST_MESSAGE("Non-empty message, two communications ...");

  messenger_t c;
  messenger_t d;

  c.connect(d);

  c.send(start_message + data);
  c.send(end_message);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data);

  BOOST_MESSAGE(" done");
}

/* Three communications */
void split3_non_empty_message() {
  BOOST_MESSAGE("Non-empty message, three communications ...");

  messenger_t c;
  messenger_t d;

  c.connect(d);

  c.send(start_message);
  c.send(data);
  c.send(end_message);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data);

  BOOST_MESSAGE(" done");
}

/* Three communications */
void split3_non_empty_message_fragmented() {
  BOOST_MESSAGE("Non-empty fragmented message, three communications ...");

  messenger_t c;
  messenger_t d;

  c.connect(d);

  c.send(start_message + data);
  c.send(data);
  c.send(data + end_message);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data + data + data);

  BOOST_MESSAGE(" done");
}

/* Three communications */
void split3_non_empty_message_and_garbage() {
  BOOST_MESSAGE("Non-empty fragmented message, three communications and garbage ...");

  messenger_t c;
  messenger_t d;

  c.connect(d);

  c.send(garbage + start_message + data);
  c.send(data);
  c.send(data + end_message + garbage);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data + data + data);

  BOOST_MESSAGE(" done");
}

/* Two communications fragmented open tag */
void non_empty_message_and_fragmented_open_tag() {
  BOOST_MESSAGE("Non-empty fragmented message open tag, two communications ...");

  messenger_t c;
  messenger_t d;

  c.connect(d);

  c.send(start_message.substr(0, start_message.size() >> 1));
  c.send(start_message.substr(start_message.size() >> 1, start_message.size()) + data + end_message);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data);

  BOOST_MESSAGE(" done");
}

/* Two communications and fragmented close tag */
void non_empty_message_and_fragmented_close_tag() {
  BOOST_MESSAGE("Non-empty fragmented message close tag, two communications ...");

  messenger_t c;
  messenger_t d;

  c.connect(d);

  c.send(start_message + data + end_message.substr(0, end_message.size() >> 1));
  c.send(end_message.substr(end_message.size() >> 1, end_message.size()));

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data);

  BOOST_MESSAGE(" done");
}

/* Three communications fragmented open and close tags */
void non_empty_message_and_fragmented_tags() {
  BOOST_MESSAGE("Non-empty fragmented message open and close tags, three communications ...");

  messenger_t c;
  messenger_t d;

  c.connect(d);

  c.send(start_message.substr(0, start_message.size() >> 1));
  c.send(start_message.substr(start_message.size() >> 1, start_message.size()) + data + end_message.substr(0, end_message.size() >> 1));
  c.send(end_message.substr(end_message.size() >> 1, end_message.size()));

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data);

  BOOST_MESSAGE(" done");
}

void message_wrapper() {
  BOOST_MESSAGE("Data wrapping and sending ...");

  messenger_t c;
  messenger_t d;
  messenger_t::message m(data);

  c.connect(d);

  c.send_message(m);

  c.disconnect(d);

  BOOST_CHECK(d.number_of_messages() == 1 && d.pop_message().to_string() == data);

  BOOST_MESSAGE(" done");
}

bool event_triggered = false;

void event_handler(messenger_t::message_ptr&) {
  event_triggered = true;
}

void message_delivery_event_handling() {
  BOOST_MESSAGE("Delivery event handling ...");

  messenger_t c;
  messenger_t d;
  messenger_t::message_ptr m(new messenger_t::message);

  /* Set handler for any message type */
  d.set_handler(event_handler);

  c.connect(d);

  c.send_message(*m.get());

  c.disconnect(d);

  BOOST_CHECK(event_triggered);

  BOOST_MESSAGE(" done");
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
  test->add(BOOST_TEST_CASE(&non_empty_message_and_fragmented_open_tag), 0, 2);
  test->add(BOOST_TEST_CASE(&non_empty_message_and_fragmented_close_tag), 0, 2);
  test->add(BOOST_TEST_CASE(&non_empty_message_and_fragmented_tags), 0, 2);
  test->add(BOOST_TEST_CASE(&message_wrapper), 0, 2);
  test->add(BOOST_TEST_CASE(&message_delivery_event_handling), 0, 2);

  return (test);
}

