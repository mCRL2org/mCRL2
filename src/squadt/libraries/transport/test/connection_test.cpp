#include <fstream>
#include <sstream>
#include <iostream>

#include <transport/transporter.h>
#include <transport/detail/socket_transceiver.h>

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>

#define BOOST_TEST_SHOW_PROGRESS yes

using boost::unit_test::test_suite;

using namespace transport;

class stream_messenger : public transporter {
  private:

  public:

    std::ostringstream output;

    stream_messenger() : transporter() {
    }

    void deliver(std::istream& input, transceiver::basic_transceiver*) {
      output << input.rdbuf();
    }

    void deliver(std::string& input, transceiver::basic_transceiver*) {
      output.str(input);
    }

    inline const std::string get_string() {
      return (output.str());
    }

    inline void set_string(std::string& string) {
      return (output.str(string));
    }
};

static const std::string test_data("1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.,:;&!~@#$%^*+-_=(){}[]|\\/><");

/* Test direct connection functionality */
void direct_connection_test() {
  BOOST_MESSAGE("Testing direct connection: ");

  stream_messenger    initiator;
  stream_messenger    connector;

  BOOST_MESSAGE("  Connecting peer...");
  connector.connect(initiator);

  BOOST_MESSAGE("  Disconnecting peer...");
  initiator.disconnect();

  BOOST_MESSAGE("  Connecting to peer...");
  initiator.connect(connector);

  BOOST_MESSAGE("  Disconnecting from peer...");
  connector.disconnect();

  BOOST_CHECK(initiator.number_of_listeners() == 0 && initiator.number_of_connections() == 0);
}

/* Test socket connection functionality */
void socket_connection_test() {
  BOOST_MESSAGE("Testing socket connection: ");

  stream_messenger    initiator;
  stream_messenger    connector;

  BOOST_MESSAGE("  Activating listener...");
  connector.add_listener();

  BOOST_MESSAGE("  Connecting peer...");
  initiator.connect();
  BOOST_CHECK(connector.number_of_listeners() == 1 && initiator.number_of_connections() == 1);

  BOOST_MESSAGE("  Disconnecting peer...");
  initiator.disconnect();

  BOOST_MESSAGE("  Deactivating listener...");
  connector.remove_listener(0);

  while (connector.number_of_listeners() != 0 || connector.number_of_connections() != 0 || initiator.number_of_connections() != 0) {
  }

  BOOST_MESSAGE("  Activating listener on peer...");
  initiator.add_listener();

  BOOST_MESSAGE("  Connecting to peer...");
  connector.connect();

  BOOST_CHECK(initiator.number_of_listeners() == 1 && connector.number_of_connections() == 1);

  BOOST_MESSAGE("  Disconnecting from peer...");
  connector.disconnect();

  BOOST_MESSAGE("  Deactivating listener on peer...");
  initiator.remove_listener(0);

  while (connector.number_of_listeners() != 0 || connector.number_of_connections() != 0 || initiator.number_of_connections() != 0) {
  }
}

/* Test whether data send directly between a sender and a receiver */
void direct_send_receive() {
  std::string        initiator_data;
  std::string        connector_data;

  stream_messenger    initiator;
  stream_messenger    connector;

  connector.connect(initiator);

  BOOST_MESSAGE("Testing direct send/receive: ");

  /* connector and initiator have a direct connection */
  BOOST_MESSAGE("  From connector to initiator (pass 1): ");

  initiator.set_string(initiator_data);
  connector.set_string(connector_data);
  connector.send(test_data);
  initiator_data = initiator.get_string(); 
  connector_data = connector.get_string();

  BOOST_CHECK(!(initiator_data != test_data || !connector_data.empty()));

  BOOST_MESSAGE("  From initiator to connector (pass 2): ");

  initiator_data.clear();
  connector_data.clear();
  initiator.set_string(initiator_data);
  connector.set_string(connector_data);
  initiator.send(test_data);
  initiator_data = initiator.get_string(); 
  connector_data = connector.get_string();

  BOOST_CHECK(!(!initiator_data.empty() || connector_data != test_data));

  initiator.disconnect();
}

/* Test whether data can be communicated via a socket connection between a sender and a receiver */
void socket_send_receive() {
  class transport::transceiver::socket_transceiver;

  std::string        initiator_data;
  std::string        connector_data;

  stream_messenger    initiator;
  stream_messenger    connector;

  connector.add_listener();
  initiator.connect();

  BOOST_MESSAGE("Testing socket send/receive: ");

  /* connector and initiator have a direct connection */
  BOOST_MESSAGE("  From connector to initiator (pass 1): ");

  initiator.set_string(initiator_data);
  connector.set_string(connector_data);

  initiator.send(test_data);

  while (connector.get_string().size() < test_data.size()) {
    /* Wait for all data to arrive */
  }

  initiator_data = initiator.get_string(); 
  connector_data = connector.get_string();

  BOOST_CHECK(connector_data == test_data && initiator_data.empty());
  BOOST_MESSAGE("  From initiator to connector (pass 2): ");

  initiator_data.clear();
  connector_data.clear();
  initiator.set_string(initiator_data);
  connector.set_string(connector_data);
  connector.send(test_data);

  while (initiator.get_string().size() < test_data.size()) {
    /* Wait for all data to arrive */
  }

  initiator_data = initiator.get_string(); 
  connector_data = connector.get_string();

  BOOST_CHECK(initiator_data == test_data && connector_data.empty());

  initiator.disconnect();
  connector.remove_listener();
}

/* Test multiplexing (a sender and two direct receivers) */
void direct_multiplexing() {
}

/* Test multiplexing (a sender and two socket receivers) */
void socket_multiplexing() {
}

/* Test multiplexing (a sender, a direct receivers and a socket receiver) */
void mixed_multiplexing() {
}

/* Test multiplexing (a receiver and two direct senders) */
void direct_demultiplexing() {
}

/* Test multiplexing (a receiver and two socket senders) */
void socket_demultiplexing() {
}

/* Test demultiplexing (a receiver, a direct sender and a socket sender) */
void mixed_demultiplexing() {
}

test_suite* init_unit_test_suite( int argc, char * argv[] ) {
  using namespace boost::unit_test;

  test_suite* test = BOOST_TEST_SUITE( "Transport Library" );

  /* Change log parameters */
  unit_test_log_t::instance().set_threshold_level(log_messages);

  test->add(BOOST_TEST_CASE(&direct_connection_test), 0, 2);
  test->add(BOOST_TEST_CASE(&socket_connection_test), 0, 10);
  test->add(BOOST_TEST_CASE(&direct_send_receive), 0, 2);
  test->add(BOOST_TEST_CASE(&socket_send_receive), 0, 10);
  test->add(BOOST_TEST_CASE(&direct_multiplexing), 0, 2);
  test->add(BOOST_TEST_CASE(&socket_multiplexing), 0, 10);
  test->add(BOOST_TEST_CASE(&mixed_multiplexing), 0, 2);
  test->add(BOOST_TEST_CASE(&direct_demultiplexing), 0, 2);
  test->add(BOOST_TEST_CASE(&socket_demultiplexing), 0, 10);
  test->add(BOOST_TEST_CASE(&mixed_demultiplexing), 0, 2);

  return (test);
}

