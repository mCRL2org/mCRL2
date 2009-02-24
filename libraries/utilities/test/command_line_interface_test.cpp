// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file command_line_interface_test.cpp

#include <iostream>
#include <string>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/included/unit_test_framework.hpp>

#define MCRL2_REVISION "xXxXx"
#include "mcrl2/new_data/rewriter.h"
#include "mcrl2/new_data/detail/prover/bdd_path_eliminator.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_rewriting.h"
#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/utilities/command_line_proving.h"

using namespace ::mcrl2::utilities;

template < typename TypeName, bool b >
void string_to_type_test(std::string const& value) {
  std::istringstream is(value);
  TypeName           s;

  BOOST_CHECK((is >> s).fail() != b);
}

template < bool b >
inline void string_to_strategy_test(std::string const& strategy) {
  string_to_type_test< mcrl2::new_data::rewriter::strategy, b >(strategy);
}

template < bool b >
inline void string_to_prover_type_test(std::string const& prover_type) {
  string_to_type_test< mcrl2::new_data::detail::SMT_Solver_Type, b >(prover_type);
}

BOOST_AUTO_TEST_CASE(border_invalid) {
  interface_description test_interface("test", "TEST", "Kilroy", "[OPTIONS]... [PATH]", "whatis", "description");

  // Empty command line
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, ""));
  char    c = '\0';
  char*   pc = &c;
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, 0, &pc));
  wchar_t  w = L'\0';
  wchar_t* pw = &w;
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, 0, &pw));
}

BOOST_AUTO_TEST_CASE(parsing) {
  interface_description test_interface("test", "TEST", "Kilroy", "[OPTIONS]... [PATH]", "whatis", "description");

  // Valid option -h
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test -v"));
  // Repeated options --help options
  BOOST_CHECK_THROW(command_line_parser(test_interface, "test --verbose -v -v"), std::runtime_error);
  // Invalid combination of short options
  BOOST_CHECK_THROW(command_line_parser(test_interface, "test -ve"), std::runtime_error);

  // Duplicate long option without argument
  BOOST_CHECK_THROW(test_interface.add_option("verbose","An option"), std::logic_error);
  // Duplicate long option with short option and without argument
  BOOST_CHECK_THROW(test_interface.add_option("verbose", "An option", 'h'), std::logic_error);
  // Duplicate long option with short option and with optional argument
  BOOST_CHECK_THROW(test_interface.add_option("verbose",make_mandatory_argument("STR"), "An option", 'v'), std::logic_error);
  // Duplicate long option with short option and with optional argument
  BOOST_CHECK_THROW(test_interface.add_option("verbose",make_optional_argument("STR", "XxXxX"), "An option", 'v'), std::logic_error);

  test_interface.add_option("mandatory", make_mandatory_argument("STR"), "option with mandatory argument", 'm');
  // Missing mandatory argument for option --mandatory
  BOOST_CHECK_THROW(command_line_parser(test_interface, "test --mandatory"), std::runtime_error);
  // Valid option with valid argument
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test --mandatory=test"));
  // Valid option with valid argument
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test -m=test"));
  // Valid option with valid argument
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test -m test"));
  // Valid short option v followed by option m with valid argument
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test -vm=test"));

  test_interface.add_option("optional", make_optional_argument("STR", "*XxXxX*"), "option with optional argument", 'o');
  // Missing mandatory argument for option --mandatory
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test --optional"));
  // Valid option with valid argument
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test --optional=test"));
  // Valid option with valid argument
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test -otest"));
  // Valid option without argument
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test -o test"));
  // Valid short option v followed by option m with valid argument
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test -vmtest"));
}

BOOST_AUTO_TEST_CASE(conformance) {
  interface_description test_interface("test", "TEST", "Kilroy", "[OPTIONS]... [PATH]", "whatis", "description");

  // Valid options -v, --verbose
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test -v"));
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test --verbose"));
  // Valid options -q, --quiet
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test -q"));
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test --quiet"));
  // Valid options -d, --debug
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test -d"));
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test --debug"));

  // Check conversion with wide characters
  wchar_t const* arguments[] = { L"test", L"--debug", L"--verbose=2" } ;

  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, 2, arguments));
  BOOST_CHECK_THROW(command_line_parser(test_interface, 3, arguments), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(rewriting_options) {
  interface_description test_interface("test", "TEST", "Kilroy", "[OPTIONS]... [PATH]", "whatis", "description");

  // testing rewriter strategy extraction
  string_to_strategy_test< true >("jitty");
  string_to_strategy_test< false >("ijitty");
  string_to_strategy_test< false >("jitta");
  string_to_strategy_test< false >("jittya");
  string_to_strategy_test< true >("jittyp");
#if defined(MCRL2_JITTYC_AVAILABLE)
  string_to_strategy_test< true >("jittyc");
#endif
  string_to_strategy_test< true >("inner");
  string_to_strategy_test< true >("innerp");
#if defined(MCRL2_INNERC_AVAILABLE)
  string_to_strategy_test< true >("innerc");
#endif
  string_to_strategy_test< false >("innera");
  string_to_strategy_test< false >("ainner");

  // Test rewriting options (-r and --rewrite with a mandatory argument)
  test_interface.add_rewriting_options();

  // Missing mandatory argument for option --rewriter
  BOOST_CHECK_THROW(command_line_parser(test_interface, "test --rewriter"), std::runtime_error);
  // Valid rewriter option with valid argument
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test --rewriter=inner"));
  // Valid rewriter option with valid argument
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test -rinner"));
  // Valid rewriter option with invalid argument
  BOOST_CHECK_THROW(command_line_parser(test_interface, "test --rewriter=invalid"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(prover_options) {
  interface_description test_interface("test", "TEST", "Rincewind", "[OPTIONS]... [PATH]", "whatis", "description");

  // testing smt prover type extraction
  string_to_prover_type_test< true >("ario");
  string_to_prover_type_test< false >("arion");
  string_to_prover_type_test< false >("aario");
  string_to_prover_type_test< false >("ar");
  string_to_prover_type_test< false >("cvc-");
  string_to_prover_type_test< false >("cvca");
  string_to_prover_type_test< true >("cvc");
  string_to_prover_type_test< false >("acvc");

  // Test rewriting options (-z and --smt-solver with a mandatory argument)
  test_interface.add_prover_options();

  // Missing mandatory argument for option --smt-solver
  BOOST_CHECK_THROW(command_line_parser(test_interface, "test --smt-solver"), std::runtime_error);
  // Valid smt-solver option with valid argument
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test --smt-solver=ario"));
  // Valid smt-solver option with valid argument
  BOOST_CHECK_NO_THROW(command_line_parser(test_interface, "test -zcvc"));
  // Valid smt-solver option with invalid argument
  BOOST_CHECK_THROW(command_line_parser(test_interface, "test --smt-solver=invalid"), std::runtime_error);
}

inline std::string const& first_of(command_line_parser const& p, std::string const& option) {
  return p.options.equal_range(option).first->second;
}

inline std::string const& last_of(command_line_parser const& p, std::string const& option) {
  command_line_parser::option_map::const_iterator i(p.options.equal_range(option).second);

  return (--i)->second;
}

BOOST_AUTO_TEST_CASE(result_browsing) {
  interface_description test_interface("test", "TEST", "Kilroy", "[OPTIONS]... [PATH]", "whatis", "description");

  // disable check for duplicate options
  test_interface.add_option("cli-testing-no-duplicate-option-checking", "");

  {
    command_line_parser parser(test_interface, "test -v --debug -d --verbose");

    BOOST_CHECK(parser.options.size() == 4);
    BOOST_CHECK(parser.options.count("verbose") == 2);
    BOOST_CHECK(first_of(parser, "verbose").empty());
    BOOST_CHECK(last_of(parser, "verbose").empty());
    BOOST_CHECK_THROW(parser.option_argument("verbose"), std::logic_error);
    BOOST_CHECK(parser.options.count("debug") == 2);
    BOOST_CHECK(first_of(parser, "debug").empty());
    BOOST_CHECK(last_of(parser, "debug").empty());
    BOOST_CHECK(parser.options.count("quiet") == 0);
    BOOST_CHECK(parser.arguments.size() == 0);
  }

  {
    command_line_parser parser(test_interface, "test /bin/ls -v \\or\\more:1234567890|,<>.:;[]}{+-_=~!@#$%^&*()");

    BOOST_CHECK(parser.options.size() == 1);
    BOOST_CHECK(first_of(parser, "verbose").empty());
    BOOST_CHECK(parser.arguments.size() == 2);
    BOOST_CHECK(parser.arguments[0] == "/bin/ls");
    BOOST_CHECK(parser.arguments[1] == "\\or\\more:1234567890|,<>.:;[]}{+-_=~!@#$%^&*()");
  }

  test_interface.add_option("mandatory", make_mandatory_argument("STR"), "option with mandatory argument", 'm');
  test_interface.add_option("optional", make_optional_argument("STR", "4321"), "option with optional argument", 'o');

  {
    command_line_parser parser(test_interface, "test --mandatory=BLA --optional=1234 -vo -vmALB");

    BOOST_CHECK(parser.options.size() == 6);
    BOOST_CHECK(first_of(parser, "mandatory") != last_of(parser, "mandatory"));
    BOOST_CHECK((first_of(parser, "mandatory") == "BLA" && last_of(parser, "mandatory") == "ALB") ||
                (first_of(parser, "mandatory") == "ALB" && last_of(parser, "mandatory") == "BLA"));
    BOOST_CHECK((first_of(parser, "optional") == "4321" && last_of(parser, "optional") == "1234") ||
                (first_of(parser, "optional") == "1234" && last_of(parser, "optional") == "4321"));
    BOOST_CHECK(parser.option_argument_as< int >("optional") == 1234 ||
                parser.option_argument_as< int >("optional") == 4321);
    BOOST_CHECK(parser.arguments.size() == 0);
  }
  {
    command_line_parser parser(test_interface, "test -m BLA -o 1234");

    BOOST_CHECK(first_of(parser, "mandatory") == "BLA");
    BOOST_CHECK(parser.option_argument_as< int >("optional") == 4321);
    BOOST_CHECK(parser.arguments.size() == 1);
  }
}
