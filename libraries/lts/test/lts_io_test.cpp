// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts_io_test.cpp
/// \brief Tests for reading and writing labelled transition systems in the binary .lts format.

#define BOOST_TEST_MODULE lts_io_test
#include <boost/test/included/unit_test.hpp>

#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "mcrl2/atermpp/aterm_io_binary.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/real.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/lts_lts.h"

using namespace mcrl2;
using namespace mcrl2::lts;

namespace
{

#ifdef NDEBUG
constexpr bool assertions_enabled = false;
#else
constexpr bool assertions_enabled = true;
#endif

using probabilistic_state_t = probabilistic_lts_lts_t::probabilistic_state_t;

// Action labels and process parameters shared by the tests.
const process::action_label& label_a()
{
  static const process::action_label a("a", data::sort_expression_list({data::sort_nat::nat()}));
  return a;
}

const process::action_label& label_b()
{
  static const process::action_label b("b", data::sort_expression_list());
  return b;
}

const process::action_label& label_c()
{
  static const process::action_label c("c", data::sort_expression_list({data::sort_bool::bool_()}));
  return c;
}

process::action action_a(std::size_t n)
{
  return process::action(label_a(), data::data_expression_list({data::sort_nat::nat(n)}));
}

process::action action_b()
{
  return process::action(label_b(), data::data_expression_list());
}

process::action action_c(bool value)
{
  return process::action(label_c(),
    data::data_expression_list({value ? data::sort_bool::true_() : data::sort_bool::false_()}));
}

action_label_lts make_label(const std::vector<process::action>& actions)
{
  return action_label_lts(lps::multi_action(process::action_list(actions.begin(), actions.end())));
}

action_label_lts make_timed_label(const std::vector<process::action>& actions, const std::string& time)
{
  return action_label_lts(
    lps::multi_action(process::action_list(actions.begin(), actions.end()), data::sort_real::real_(time)));
}

state_label_lts make_state_label(std::size_t x, bool b)
{
  return state_label_lts(std::vector<data::data_expression>{data::sort_nat::nat(x),
    b ? data::sort_bool::true_() : data::sort_bool::false_()});
}

/// \brief The labels used in the example LTS; deliberately not in the order in which they first occur.
std::vector<action_label_lts> example_labels()
{
  return {
    make_label({action_a(1)}),
    make_label({action_b()}),
    make_label({action_a(2)}),
    make_label({action_a(1), action_b()}), // A multi-action.
    make_label({action_c(true)}),
    make_timed_label({action_a(1)}, "3"), // A timed action.
    action_label_lts::tau_action(),
  };
}

void set_header(lts_lts_t& l)
{
  l.set_data(data::data_specification());
  l.set_process_parameters(
    data::variable_list({data::variable("x", data::sort_nat::nat()), data::variable("y", data::sort_bool::bool_())}));
  l.set_action_label_declarations(process::action_label_list({label_a(), label_b(), label_c()}));
}

/// \brief An LTS with state labels, multi-actions, a timed action, tau, self-loops, duplicate transitions and a
///        non-zero initial state.
lts_lts_t example_lts(bool with_state_labels)
{
  lts_lts_t l;
  set_header(l);

  const std::size_t number_of_states = 6;
  if (with_state_labels)
  {
    for (std::size_t i = 0; i < number_of_states; ++i)
    {
      l.add_state(make_state_label(i, i % 2 == 0));
    }
  }
  else
  {
    l.set_num_states(number_of_states, false);
  }

  std::vector<std::size_t> label_index;
  for (const action_label_lts& label: example_labels())
  {
    label_index.push_back(l.add_action(label));
  }

  const std::vector<std::tuple<std::size_t, std::size_t, std::size_t>> transitions = {
    {3, 4, 0},
    {0, 0, 1},
    {1, 6, 2}, // tau
    {2, 2, 2}, // self-loop
    {2, 3, 4},
    {4, 5, 5},
    {5, 1, 3},
    {0, 0, 1}, // duplicate
    {1, 0, 5},
  };
  for (const auto& [from, label, to]: transitions)
  {
    l.add_transition(transition(from, label_index[label], to));
  }
  l.set_initial_state(3);
  return l;
}

/// \brief The pretty printed action labels of an LTS, by index. Printing each label once, rather than once per
///        transition, keeps describe fast in Debug builds.
template<typename LTS>
std::vector<std::string> action_label_strings(const LTS& l)
{
  std::vector<std::string> result;
  for (std::size_t i = 0; i < l.num_action_labels(); ++i)
  {
    result.push_back(pp(l.action_label(i)));
  }
  return result;
}

/// \brief A canonical, index-independent description of an LTS: the header, the number of states, the initial
///        state, the state labels, and the transitions in order with their action labels written out. Loading may
///        renumber action labels, so comparing these descriptions is more meaningful than comparing with ==.
std::vector<std::string> describe(const lts_lts_t& l)
{
  const std::vector<std::string> labels = action_label_strings(l);
  std::vector<std::string> result;
  result.push_back("parameters " + data::pp(l.process_parameters()));
  result.push_back("action declarations " + process::pp(l.action_label_declarations()));
  result.push_back("states " + std::to_string(l.num_states()));
  result.push_back("initial " + std::to_string(l.initial_state()));
  for (std::size_t i = 0; i < l.num_state_labels(); ++i)
  {
    result.push_back("state label " + std::to_string(i) + " " + pp(l.state_label(i)));
  }
  for (const transition& t: l.get_transitions())
  {
    result.push_back(
      std::to_string(t.from()) + " -" + labels[l.apply_hidden_label_map(t.label())] + "-> " + std::to_string(t.to()));
  }
  return result;
}

std::string describe(const probabilistic_state_t& state)
{
  if (state.size() <= 1)
  {
    return std::to_string(state.get());
  }
  std::string result = "{";
  for (const auto& p: state)
  {
    result += std::to_string(p.state()) + ":" + data::pp(p.probability()) + " ";
  }
  return result + "}";
}

std::vector<std::string> describe(const probabilistic_lts_lts_t& l)
{
  const std::vector<std::string> labels = action_label_strings(l);
  std::vector<std::string> result;
  result.push_back("parameters " + data::pp(l.process_parameters()));
  result.push_back("action declarations " + process::pp(l.action_label_declarations()));
  result.push_back("states " + std::to_string(l.num_states()));
  result.push_back("initial " + describe(l.initial_probabilistic_state()));
  for (std::size_t i = 0; i < l.num_state_labels(); ++i)
  {
    result.push_back("state label " + std::to_string(i) + " " + pp(l.state_label(i)));
  }
  for (const transition& t: l.get_transitions())
  {
    result.push_back(
      std::to_string(t.from()) + " -" + labels[t.label()] + "-> " + describe(l.probabilistic_state(t.to())));
  }
  return result;
}

template<typename LTS>
std::string write_lts(const LTS& l)
{
  std::stringstream stream;
  {
    atermpp::binary_aterm_ostream output(stream);
    output << l;
  }
  return stream.str();
}

template<typename LTS>
LTS read_lts(const std::string& bytes)
{
  std::stringstream stream(bytes);
  atermpp::binary_aterm_istream input(stream);
  LTS result;
  input >> result;
  return result;
}

template<typename LTS>
void check_round_trip(const LTS& original)
{
  const std::string bytes = write_lts(original);
  const LTS loaded = read_lts<LTS>(bytes);

  const std::vector<std::string> expected = describe(original);
  const std::vector<std::string> actual = describe(loaded);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
  BOOST_CHECK(loaded.data() == original.data());

  // Writing the loaded LTS again gives exactly the same bytes.
  BOOST_CHECK(write_lts(loaded) == bytes);
}

std::string hex(const std::string& bytes)
{
  std::ostringstream out;
  for (std::size_t i = 0; i < bytes.size(); ++i)
  {
    out << (i == 0 ? "" : ", ") << (i % 16 == 0 && i > 0 ? "\n" : "") << "0x" << std::hex << std::setw(2)
        << std::setfill('0') << static_cast<unsigned int>(static_cast<unsigned char>(bytes[i]));
  }
  return out.str();
}

/// \brief A temporary file name in the current directory that is removed at the end of the scope.
class temporary_file
{
public:
  explicit temporary_file(const std::string& name)
    : m_name(name)
  {}

  ~temporary_file()
  {
    std::filesystem::remove(m_name);
  }

  temporary_file(const temporary_file&) = delete;
  temporary_file& operator=(const temporary_file&) = delete;

  const std::string& name() const
  {
    return m_name;
  }

private:
  std::string m_name;
};

} // namespace

BOOST_AUTO_TEST_CASE(round_trip_with_state_labels)
{
  check_round_trip(example_lts(true));
}

BOOST_AUTO_TEST_CASE(round_trip_without_state_labels)
{
  check_round_trip(example_lts(false));
}

BOOST_AUTO_TEST_CASE(round_trip_via_file)
{
  const lts_lts_t original = example_lts(true);
  temporary_file file("lts_io_test_round_trip_via_file.lts");
  original.save(file.name());

  lts_lts_t loaded;
  loaded.load(file.name());
  const std::vector<std::string> expected = describe(original);
  const std::vector<std::string> actual = describe(loaded);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());

  // The file contains exactly the bytes of the stream operator.
  std::ifstream stream(file.name(), std::ios::binary);
  const std::string contents((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
  BOOST_CHECK(contents == write_lts(original));
}

// An LTS with a single state and no transitions.
BOOST_AUTO_TEST_CASE(round_trip_single_state)
{
  lts_lts_t with_label;
  set_header(with_label);
  with_label.add_state(make_state_label(0, true));
  with_label.set_initial_state(0);
  check_round_trip(with_label);

  lts_lts_t without_label;
  set_header(without_label);
  without_label.set_num_states(1, false);
  without_label.set_initial_state(0);
  check_round_trip(without_label);
}

// A larger LTS, so that term and function symbol indices need many bits.
BOOST_AUTO_TEST_CASE(round_trip_large)
{
  std::mt19937_64 generator(1);
  lts_lts_t l;
  set_header(l);

  const std::size_t number_of_states = 2000;
  for (std::size_t i = 0; i < number_of_states; ++i)
  {
    l.add_state(make_state_label(i, generator() % 2 == 0));
  }
  std::vector<std::size_t> labels;
  for (std::size_t i = 0; i < 300; ++i)
  {
    labels.push_back(l.add_action(make_label({action_a(i)})));
  }
  for (std::size_t i = 0; i < 10000; ++i)
  {
    l.add_transition(
      transition(generator() % number_of_states, labels[generator() % labels.size()], generator() % number_of_states));
  }
  // Ensure that the highest state occurs in a transition; see isolated_states_are_kept for why.
  l.add_transition(transition(0, labels[0], number_of_states - 1));
  l.set_initial_state(0);
  check_round_trip(l);
}

// The exact bytes of a small LTS. Files written by earlier versions of the toolset must remain readable, so this
// pins down the .lts format on top of the binary aterm format.
BOOST_AUTO_TEST_CASE(exact_encoding)
{
  lts_lts_t l;
  set_header(l);
  l.add_state(make_state_label(0, true));
  l.add_state(make_state_label(1, false));
  const std::size_t a1 = l.add_action(make_label({action_a(1)}));
  const std::size_t ab = l.add_action(make_label({action_a(2), action_b()}));
  l.add_transition(transition(0, a1, 1));
  l.add_transition(transition(1, ab, 0));
  l.add_transition(transition(1, 0, 1));
  l.set_initial_state(0);

  const std::string actual = write_lts(l);
  // clang-format off
  const std::vector<unsigned char> expected_bytes = {
      0x00, 0x8b, 0xaf, 0x83, 0x08, 0x06, 0x9b, 0x18, 0x58, 0x99, 0x5b, 0x1b, 0x19, 0x59, 0x17, 0xdd,
      0x1c, 0x98, 0x5b, 0x9c, 0xda, 0x5d, 0x1a, 0x5b, 0xdb, 0x97, 0xdc, 0xde, 0x5c, 0xdd, 0x19, 0x5b,
      0x40, 0x27, 0x00, 0xc0, 0x30, 0x0c, 0x03, 0x00, 0x03, 0x0f, 0x19, 0x5b, 0x5c, 0x1d, 0x1e, 0x57,
      0xdb, 0x1a, 0x5c, 0xdd, 0x0f, 0x80, 0x18, 0x04, 0x42, 0x6f, 0x6f, 0x6c, 0x00, 0x58, 0x0c, 0xa6,
      0xde, 0xe4, 0xe8, 0x92, 0xc8, 0x02, 0xc4, 0x01, 0x79, 0x00, 0x68, 0x12, 0x88, 0xc2, 0xe8, 0xc2,
      0xac, 0xc2, 0xe4, 0x92, 0xc8, 0x04, 0xe6, 0x81, 0x23, 0xc6, 0xc6, 0x97, 0x37, 0x45, 0xf6, 0x36,
      0xf6, 0xe7, 0x37, 0x47, 0x27, 0x56, 0x37, 0x46, 0xf7, 0x23, 0xe0, 0x25, 0xe0, 0x00, 0xd3, 0x98,
      0x5d, 0x00, 0x18, 0x53, 0x00, 0x2f, 0x00, 0x0c, 0xad, 0x0f, 0x3c, 0xaa, 0xe4, 0x00, 0x0b, 0x18,
      0x03, 0x40, 0x2a, 0x0b, 0x1b, 0xa2, 0x4b, 0x20, 0x13, 0x77, 0x4b, 0xe0, 0x00, 0x2c, 0x40, 0x0e,
      0x37, 0xc0, 0xbb, 0xda, 0xb9, 0xc0, 0x00, 0xb0, 0x80, 0x3a, 0xdc, 0xa3, 0x3c, 0xe0, 0x05, 0x3a,
      0x39, 0x30, 0xb7, 0x39, 0xb4, 0xba, 0x34, 0xb7, 0xb7, 0x00, 0x5d, 0x80, 0x00, 0x8a, 0x4c, 0xac,
      0x2d, 0x80, 0x0b, 0xd2, 0x50, 0x0f, 0x40, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64,
      0x5f, 0x72, 0x65, 0x61, 0x6c, 0x00, 0x60, 0x9a, 0xd5, 0x02, 0xcf, 0x18, 0x5d, 0x19, 0x5c, 0x9b,
      0x57, 0xda, 0x5b, 0x9d, 0x0f, 0x80, 0x18, 0x80, 0x80, 0xa8, 0x0e, 0xed, 0xee, 0x4c, 0x80, 0x0c,
      0x92, 0x65, 0x3e, 0x80, 0x04, 0xa9, 0xb7, 0xb9, 0x3a, 0x20, 0xb9, 0x39, 0x37, 0xbb, 0x81, 0x33,
      0xd9, 0xc1, 0xa4, 0x06, 0xd6, 0xf7, 0x37, 0x45, 0xf7, 0x36, 0x96, 0x76, 0xe6, 0x96, 0x66, 0x96,
      0x36, 0x16, 0xe7, 0x45, 0xf6, 0x46, 0x96, 0x76, 0x97, 0x44, 0xe6, 0x17, 0x40, 0x06, 0x80, 0x5a,
      0x7b, 0x82, 0x4b, 0x22, 0x73, 0x7a, 0x4b, 0x73, 0x23, 0x2b, 0xc0, 0x13, 0x5e, 0xf0, 0x08, 0x44,
      0x61, 0x74, 0x61, 0x41, 0x70, 0x70, 0x6c, 0x02, 0x6d, 0xec, 0x27, 0x7c, 0x00, 0x19, 0x05, 0x8d,
      0xd1, 0xa5, 0xbd, 0xb8, 0x09, 0xba, 0x70, 0x27, 0x84, 0x00, 0x31, 0x51, 0xa5, 0xb5, 0x95, 0x91,
      0x35, 0xd5, 0xb1, 0xd1, 0x05, 0x8d, 0xd0, 0x0a, 0xc4, 0x4b, 0xe0, 0x33, 0xb0, 0x16, 0xe7, 0x80,
      0x9e, 0x30, 0x18, 0x81, 0x36, 0x7a, 0x54, 0xf3, 0x00, 0xdd, 0x39, 0xd3, 0xd1, 0x25, 0x8a, 0x57,
      0xc0, 0x27, 0x60, 0x36, 0x00, 0x5f, 0x01, 0x01, 0x1d, 0x1c, 0x9d, 0x59, 0x40, 0x1c, 0xb5, 0xa8,
      0x26, 0x20, 0x0d, 0x9e, 0xb0, 0x06, 0x40, 0x6e, 0x6f, 0x64, 0x65, 0x40, 0x02, 0x75, 0x6d, 0x71,
      0xee, 0x00, 0x05, 0x66, 0x61, 0x6c, 0x73, 0x65, 0x00, 0x76, 0xd6, 0xf0, 0x9d, 0x3f, 0x84, 0x7c,
      0x40, 0x03, 0x5a, 0x5b, 0x9a, 0x5d, 0x1a, 0x58, 0x5b, 0x17, 0xdc, 0xdd, 0x18, 0x5d, 0x19, 0x40,
      0x2e, 0x60, 0x38, 0x02, 0x00, 0x00, 0x00, 0x00,
  };
  // clang-format on
  const std::string expected(expected_bytes.begin(), expected_bytes.end());
  BOOST_CHECK_MESSAGE(actual == expected, "Encoding changed; the bytes written are now:\n" << hex(actual));

  // The reader must decode the exact bytes, independently of what the current writer produces.
  const lts_lts_t loaded = read_lts<lts_lts_t>(expected);
  const std::vector<std::string> expected_description = describe(l);
  const std::vector<std::string> actual_description = describe(loaded);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual_description.begin(),
    actual_description.end(),
    expected_description.begin(),
    expected_description.end());
}

// The functions that lps2lts uses to write an LTS incrementally. The transitions, state labels and initial state
// can be written in any order.
BOOST_AUTO_TEST_CASE(incremental_writing)
{
  const lts_lts_t expected = example_lts(true);

  std::stringstream stream;
  {
    atermpp::binary_aterm_ostream output(stream);
    write_lts_header(output, expected.data(), expected.process_parameters(), expected.action_label_declarations());
    write_initial_state(output, expected.initial_state());

    // Interleave the transitions and the state labels.
    std::size_t next_label = 0;
    for (const transition& t: expected.get_transitions())
    {
      write_transition(output, t.from(), expected.action_label(t.label()), t.to());
      if (next_label < expected.num_state_labels())
      {
        write_state_label(output, expected.state_label(next_label++));
      }
    }
    for (; next_label < expected.num_state_labels(); ++next_label)
    {
      write_state_label(output, expected.state_label(next_label));
    }
  }

  lts_lts_t loaded;
  atermpp::binary_aterm_istream input(stream);
  input >> loaded;
  const std::vector<std::string> expected_description = describe(expected);
  const std::vector<std::string> actual_description = describe(loaded);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual_description.begin(),
    actual_description.end(),
    expected_description.begin(),
    expected_description.end());
}

// Loading assigns action label indices in the order in which labels first occur in the transitions (after tau, which
// is always 0), and labels that no transition uses are not stored.
BOOST_AUTO_TEST_CASE(action_labels_are_renumbered)
{
  lts_lts_t l;
  set_header(l);
  l.set_num_states(2, false);
  const std::size_t unused = l.add_action(make_label({action_b()}));
  const std::size_t c = l.add_action(make_label({action_c(false)}));
  const std::size_t a = l.add_action(make_label({action_a(0)}));
  BOOST_CHECK_EQUAL(unused, 1U);
  l.add_transition(transition(0, a, 1));
  l.add_transition(transition(1, c, 0));
  l.set_initial_state(0);

  const lts_lts_t loaded = read_lts<lts_lts_t>(write_lts(l));
  BOOST_REQUIRE_EQUAL(loaded.num_action_labels(), 3U);
  BOOST_CHECK(loaded.action_label(0) == action_label_lts::tau_action());
  BOOST_CHECK(loaded.action_label(1) == make_label({action_a(0)}));
  BOOST_CHECK(loaded.action_label(2) == make_label({action_c(false)}));
  BOOST_CHECK_EQUAL(loaded.get_transitions()[0].label(), 1U);
  BOOST_CHECK_EQUAL(loaded.get_transitions()[1].label(), 2U);
}

// Hidden actions are written as tau; the set of hidden labels itself is not stored.
BOOST_AUTO_TEST_CASE(hidden_labels_are_written_as_tau)
{
  lts_lts_t l;
  set_header(l);
  l.set_num_states(2, false);
  const std::size_t a = l.add_action(make_label({action_a(0)}));
  const std::size_t b = l.add_action(make_label({action_b()}));
  l.add_transition(transition(0, b, 1));
  l.add_transition(transition(1, a, 0));
  l.hidden_label_set().insert(b);
  l.set_initial_state(0);

  const lts_lts_t loaded = read_lts<lts_lts_t>(write_lts(l));
  BOOST_CHECK(loaded.hidden_label_set().empty());
  BOOST_REQUIRE_EQUAL(loaded.num_transitions(), 2U);
  BOOST_CHECK(loaded.is_tau(loaded.get_transitions()[0].label()));
  BOOST_CHECK(loaded.action_label(loaded.get_transitions()[1].label()) == make_label({action_a(0)}));
}

// States that do not occur in any transition, and that have a higher index than every state that does, must survive
// a round trip, including their state labels. Currently the number of states is derived from the transitions only,
// so such states are lost. See https://github.com/mCRL2org/mCRL2/issues/1975; remove expected_failures once fixed.
BOOST_AUTO_TEST_CASE(isolated_states_are_kept, *boost::unit_test::expected_failures(3))
{
  lts_lts_t l;
  set_header(l);
  for (std::size_t i = 0; i < 5; ++i)
  {
    l.add_state(make_state_label(i, true));
  }
  l.add_transition(transition(0, l.add_action(make_label({action_b()})), 1));
  l.set_initial_state(0);

  const lts_lts_t loaded = read_lts<lts_lts_t>(write_lts(l));
  BOOST_CHECK_EQUAL(loaded.num_states(), 5U);
  BOOST_CHECK_EQUAL(loaded.num_state_labels(), 5U);
  BOOST_CHECK(write_lts(loaded) == write_lts(l));
}

// The initial state must be a state of the loaded LTS, even if it does not occur in any transition. Currently it can
// end up outside the LTS. See https://github.com/mCRL2org/mCRL2/issues/1974; remove expected_failures and enable_if
// once fixed. The test only runs without assertions: with assertions, loading aborts in lts::set_initial_state, which
// corrupts the aterm pool for the test cases that follow.
BOOST_AUTO_TEST_CASE(initial_state_is_a_state,
  *boost::unit_test::expected_failures(1) * boost::unit_test::enable_if<!assertions_enabled>())
{
  lts_lts_t l;
  set_header(l);
  l.set_num_states(5, false);
  l.add_transition(transition(0, l.add_action(make_label({action_b()})), 1));
  l.set_initial_state(4);

  const lts_lts_t loaded = read_lts<lts_lts_t>(write_lts(l));
  BOOST_CHECK_EQUAL(loaded.initial_state(), 4U);
  BOOST_CHECK_LT(loaded.initial_state(), loaded.num_states());
}

BOOST_AUTO_TEST_CASE(probabilistic_round_trip)
{
  probabilistic_lts_lts_t l;
  l.set_data(data::data_specification());
  l.set_action_label_declarations(process::action_label_list({label_a(), label_b()}));
  for (std::size_t i = 0; i < 4; ++i)
  {
    l.add_state(make_state_label(i, false));
  }

  probabilistic_state_t distribution;
  distribution.add(1, lps::probabilistic_data_expression(1, 3));
  distribution.add(2, lps::probabilistic_data_expression(2, 3));
  probabilistic_state_t three_way;
  three_way.add(0, lps::probabilistic_data_expression(1, 2));
  three_way.add(2, lps::probabilistic_data_expression(1, 4));
  three_way.add(3, lps::probabilistic_data_expression(1, 4));

  const std::size_t a = l.add_action(make_label({action_a(7)}));
  const std::size_t b = l.add_action(make_label({action_b()}));
  l.add_transition(transition(0, a, l.add_probabilistic_state(distribution)));
  l.add_transition(transition(1, b, l.add_probabilistic_state(probabilistic_state_t(3))));
  l.add_transition(transition(2, 0, l.add_probabilistic_state(three_way)));
  l.add_transition(transition(3, a, l.add_probabilistic_state(distribution))); // The same distribution again.
  l.set_initial_probabilistic_state(three_way);

  check_round_trip(l);
}

BOOST_AUTO_TEST_CASE(probabilistic_round_trip_via_file)
{
  probabilistic_lts_lts_t l;
  l.set_data(data::data_specification());
  l.set_num_states(3, false);
  probabilistic_state_t distribution;
  distribution.add(1, lps::probabilistic_data_expression(1, 2));
  distribution.add(2, lps::probabilistic_data_expression(1, 2));
  l.add_transition(transition(0, l.add_action(make_label({action_b()})), l.add_probabilistic_state(distribution)));
  l.set_initial_probabilistic_state(probabilistic_state_t(0));

  temporary_file file("lts_io_test_probabilistic_round_trip_via_file.lts");
  l.save(file.name());
  probabilistic_lts_lts_t loaded;
  loaded.load(file.name());
  const std::vector<std::string> expected = describe(l);
  const std::vector<std::string> actual = describe(loaded);
  BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expected.begin(), expected.end());
}

// A non-probabilistic LTS can be read as a probabilistic one.
BOOST_AUTO_TEST_CASE(read_plain_as_probabilistic)
{
  const lts_lts_t plain = example_lts(true);
  const probabilistic_lts_lts_t loaded = read_lts<probabilistic_lts_lts_t>(write_lts(plain));
  BOOST_CHECK_EQUAL(loaded.num_states(), plain.num_states());
  BOOST_CHECK_EQUAL(loaded.num_transitions(), plain.num_transitions());
  BOOST_CHECK_EQUAL(describe(loaded.initial_probabilistic_state()), std::to_string(plain.initial_state()));
  for (std::size_t i = 0; i < plain.num_transitions(); ++i)
  {
    const transition& p = plain.get_transitions()[i];
    const transition& q = loaded.get_transitions()[i];
    BOOST_CHECK_EQUAL(q.from(), p.from());
    BOOST_CHECK(loaded.action_label(q.label()) == plain.action_label(p.label()));
    BOOST_CHECK_EQUAL(describe(loaded.probabilistic_state(q.to())), std::to_string(p.to()));
  }
}

BOOST_AUTO_TEST_CASE(read_probabilistic_as_plain_fails)
{
  probabilistic_lts_lts_t l;
  l.set_data(data::data_specification());
  l.set_num_states(3, false);
  probabilistic_state_t distribution;
  distribution.add(1, lps::probabilistic_data_expression(1, 2));
  distribution.add(2, lps::probabilistic_data_expression(1, 2));
  l.add_transition(transition(0, l.add_action(make_label({action_b()})), l.add_probabilistic_state(distribution)));
  l.set_initial_probabilistic_state(probabilistic_state_t(0));

  BOOST_CHECK_THROW(read_lts<lts_lts_t>(write_lts(l)), mcrl2::runtime_error);
}

BOOST_AUTO_TEST_CASE(invalid_input)
{
  // Not an LTS: a binary aterm stream that starts with a different term.
  {
    std::stringstream stream;
    {
      atermpp::binary_aterm_ostream output(stream);
      output << atermpp::aterm(atermpp::function_symbol("not_an_lts", 0));
    }
    BOOST_CHECK_THROW(read_lts<lts_lts_t>(stream.str()), mcrl2::runtime_error);
  }

  // An LTS without an initial state.
  {
    const lts_lts_t l = example_lts(false);
    std::stringstream stream;
    {
      atermpp::binary_aterm_ostream output(stream);
      write_lts_header(output, l.data(), l.process_parameters(), l.action_label_declarations());
      write_transition(output, 0, make_label({action_b()}), 1);
    }
    BOOST_CHECK_THROW(read_lts<lts_lts_t>(stream.str()), mcrl2::runtime_error);
  }

  // A truncated file.
  {
    const std::string bytes = write_lts(example_lts(true));
    BOOST_CHECK_THROW(read_lts<lts_lts_t>(bytes.substr(0, bytes.size() / 2)), mcrl2::runtime_error);
  }

  // A file that does not exist.
  {
    lts_lts_t l;
    BOOST_CHECK_THROW(l.load("lts_io_test_this_file_does_not_exist.lts"), mcrl2::runtime_error);
  }
}
