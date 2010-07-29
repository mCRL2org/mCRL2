// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/command_line_interface.h
/// \brief Components for command line interfaces of mCRL2 tools

#ifndef __MCRL2_UTILITIES_COMMAND_LINE_INTERFACE_HPP_
#define __MCRL2_UTILITIES_COMMAND_LINE_INTERFACE_HPP_
#include <algorithm>
#include <vector>
#include <map>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <sstream>

#include "boost/algorithm/string.hpp"
#include "boost/type_traits/is_pod.hpp"
#include "boost/shared_ptr.hpp"
#include "mcrl2/utilities/toolset_revision.h"

namespace mcrl2 {
  namespace utilities {

    class interface_description;
    class command_line_parser;

    /** \brief toolset version tag */
    inline std::string version_tag() {
      return "July 2010 (development)";
    }

    /** \brief toolset copyright period description */
    inline std::string copyright_period() {
      return "2010";
    }

    // \cond INTERNAL
    namespace detail {
      template < typename T >
      struct initialiser;

      /// Helper class to prevent uninitialised variable warnings
      template < typename T, bool = boost::is_pod< T >::value >
      struct instance_of;

      template < typename T >
      struct instance_of< T, true > {
        T m_wrapped;

        instance_of() {
          m_wrapped = static_cast< T >(0);
        }

        T& wrapped() {
          return m_wrapped;
        }
      };

      template < typename T >
      struct instance_of< T, false > {
        T m_wrapped;

        T& wrapped() {
          return m_wrapped;
        }
      };
    }
    // \endcond

    /**
     * \brief Command line interface description component.
     *
     * This component can be used to specify the command-line interface of
     * tools in terms of options and arguments to options. The scope is command
     * line interfaces of mCRL2 tools.
     *
     * Options that are standard (prescribed by convention) are automatically
     * added to the interface. Options for functionality in toolset libraries
     * that are shared amongst multiple tools can be predefined in this
     * component and included on demand for individual tools.
     *
     * The following examples illustrate use of the interface description
     * class. Construction of an interface description object.
     * \code
     *  interface_description interface(
     *    "/path/to/demo-tool",
     *    "demo-tool",
     *    "Rincewind",
     *    "demonstrate the interface_description class",
     *    "[OPTIONS]... [INFILE]",
     *    "Demonstrate the features of the interface_description class using INFILE as input.");
     * \endcode
     *
     * Adding options to an interface description.
     *
     * \code
     *  cli.
     *   add_option("recursive", "option without argument", 'r').
     *   add_option("timeout", make_optional_argument("SEC", "2"), "option with optional argument (default 2)").
     *   add_option("tool", make_mandatory_argument("FOO"), "option with mandatory argument").
     *   add_option("language", make_mandatory_argument("LANG", "english"), "option with mandatory argument (default english)").
     * \endcode
     *
     * Printing an interface description.
     *
     * \code
     *  std::cerr << cli.textual_description() << std::endl;
     * \endcode
     *
     * Prints:
     * \verbatim
        interface description:
        /path/to/demo-tool [OPTIONS]... [PATHS]
        Serves as demonstration of the features of the interface_description
        class.

        Options:
          -r, --recursive          option without argument
          -t[SEC], --timeout=[SEC] option optional argument (default 2)
              --tool=FOO           option with mandatory argument
              --language=LANG      option with mandatory argument (default english)

        Standard options:
          -q, --quiet              do not display warning messages
          -v, --verbose            display short intermediate messages
          -d, --debug              display detailed intermediate messages
          -h, --help               display help information
              --version            display version information

        Report bugs at <http://www.mcrl2.org/issuetracker>.

        See also the manual at <http://www.mcrl2.org/mcrl2/wiki/index.php/demo-tool>.
       \endverbatim
     * Printing version information.
     *
     * \code
     *
     *  std::cerr << "VERSION INFORMATION:" << std::endl
     *            << std::endl
     *            << cli.version_information() << std::endl;
     * \endcode
     * Prints:
     * \verbatim
        demo-tool July 2008 (development) (revision 4624M-shared)
        Copyright (c) 2008 Technische Universiteit Eindhoven.
        This is free software.  You may redistribute copies of it under the
        terms of the Boost Software License <http://www.boost.org/LICENSE_1_0.txt>.
        There is NO WARRANTY, to the extent permitted by law.

        Written by Rincewind. \endverbatim
     **/
    class interface_description {
      friend class command_line_parser;

      template < typename T >
      friend struct detail::initialiser;

      private:

        /// \cond INTERNAL
        /**
         * \brief Option argument.
         *
         * The boolean type parameter represents whether the argument is optional or not
         **/
        class basic_argument {

          protected:

            /// name of the argument (for reference purposes in option description)
            std::string m_name;

          protected:

            /// sets the name for the argument
            void set_name(std::string const& n) {
              m_name = n;
            }

          public:

            /// returns a copy of the object
            virtual basic_argument* clone() const = 0;

            /// returns the name for the argument
            std::string get_name() const {
              return m_name;
            }

            /// Gets default value for option argument
            virtual std::string const& get_default() const = 0;

            /// verifies that a string is a valid argument
            virtual bool validate(std::string const&) const = 0;

            /// whether the argument is optional or not
            virtual bool is_optional() const = 0;

            /// whether the argument is optional or not
            virtual bool has_default() const = 0;

            /// Destructor
            virtual ~basic_argument() {
            }
        };

        /// Argument to an option
        template < typename T >
        class typed_argument;

        /// Represents an optional argument to an option
        template < typename T = std::string >
        class optional_argument;

        /// Represents a mandatory argument to an option
        template < typename T = std::string >
        class mandatory_argument;

        /**
         * \brief Describes a single option
         *
         * Objects of this type represent option specifications. They consist of: a
         * long description (the option identifier) an optional short description,
         * a description of the option, and optionally an argument. The argument
         * itself can be optional or mandatory. An optional argument has a default
         * value that is taken instead of the argument when the option is found on
         * the command line without an argument. A mandatory argument requires the
         * user to specify an argument to the option.
         **/
        class option_descriptor {
          friend class command_line_parser;
          friend class interface_description;

          private:

            /// Long representation for the option
            std::string                         m_long;

            /// Description for the option
            std::string                         m_description;

            /// Option argument
            boost::shared_ptr< basic_argument > m_argument;

            /// Short representation for the option or 0
            char                                m_short;

            /// whether the option is printed as part of the tool-specific interface
            bool                                m_show;

          protected:

            /// Returns a textual description of the option
            std::string textual_description(const size_t left_width, const size_t right_width) const;

            /// Returns a man page description for the option
            std::string man_page_description() const;

            /// Returns a man page description for the option
            std::string wiki_page_description() const;

          public:

            /**
             * \brief Constructor
             * \param[in] d description of the option
             * \param[in] l the long option representation of the option
             * \param[in] s an optional single-character short representation of the option
             * \pre l should be a non-empty string that only contain characters from [0-9a-Z] or `-'
             **/
            option_descriptor(std::string const& l, std::string const& d, const char s) :
                        m_long(l), m_description(d), m_short(s), m_show(true) {

              assert(!l.empty());
              assert(l.find_first_not_of("_-0123456789abcdefghijklmnopqrstuvwxyz") == std::string::npos);
            }

            /// copy constructor
            option_descriptor(option_descriptor const& o) : m_long(o.m_long),
                    m_description(o.m_description), m_argument(o.m_argument), m_short(o.m_short), m_show(true) {
            }

            option_descriptor operator=(option_descriptor const& o) {
              return option_descriptor(o);
            }

            basic_argument const& argument() const {
              return *m_argument;
            }

            /**
             * \brief Sets option argument
             * \param[in] a an optional argument specifier object
             **/
            template < typename T >
            void set_argument(T* a) {
              m_argument.reset(a);
            }

            /**
             * \brief Gets default
             **/
            inline std::string const& get_default() const {
              return m_argument->get_default();
            }

            /**
             * \brief Gets the option description
             **/
            inline std::string get_description() const {
              return m_description;
            }

            /**
             * \brief Whether the option takes a mandatory argument
             **/
            inline bool needs_argument() const {
              return !(m_argument.get() == 0 || m_argument->is_optional());
            }

            /**
             * \brief Whether the option takes an argument
             **/
            inline bool accepts_argument() const {
              return m_argument.get() != 0;
            }
          };

          struct option_identifier_less {
            template < typename S >
            bool operator()(S const& s1, S const& s2) const {
              return boost::is_iless()(s1, s2) || (boost::is_iequal()(s1, s2) && s2 < s1);
            }
          };
        /// \endcond

      friend optional_argument< std::string >  make_optional_argument(std::string const&, std::string const&);
      friend mandatory_argument< std::string > make_mandatory_argument(std::string const&);
      friend mandatory_argument< std::string > make_mandatory_argument(std::string const&, std::string const&);

      template < typename ArgumentType >
      friend optional_argument< ArgumentType >  make_optional_argument(std::string const&, std::string const&);
      template < typename ArgumentType >
      friend mandatory_argument< ArgumentType > make_mandatory_argument(std::string const&);
      template < typename ArgumentType >
      friend mandatory_argument< ArgumentType > make_mandatory_argument(std::string const&, std::string const&);

        typedef std::map< std::string, option_descriptor > option_map;

        typedef std::map< const char,  std::string, option_identifier_less > short_to_long_map;

        /// \brief Maps long option identifiers to option descriptor objects
        option_map        m_options;

      private:

        /// \brief Path to executable (as in the first command line argument)
        std::string       m_path;

        /// \brief Name of the tool for usage and tool description
        std::string       m_name;

        /// \brief Comma separated list of authors
        std::string       m_authors;

        /// \brief One-line "what is" information (used only in man pages)
        std::string       m_what_is;

        /// \brief Usage and description
        std::string       m_usage;

        /// \brief Tool description
        std::string       m_description;

        /// \brief Description of known issues
        std::string       m_known_issues;



        /// \brief Maps a short option to its associated long option
        short_to_long_map m_short_to_long;

      private:

        /**
         * \brief Searches a short option for a given long option
         * \param[in] n string representing a long option
         * \return the short option, or '\0' in case no short option was found
         **/
        char long_to_short(std::string const& n) const {
          char result = '\0';

          for (short_to_long_map::const_iterator i = m_short_to_long.begin(); result == '\0' && i != m_short_to_long.end(); ++i) {
            if (i->second == n) {
              result = i->first;
            }
          }

          return result;
        }

        option_descriptor const& find_option(std::string const& long_identifier) const;

        /// \brief Stores a sequence of actions that automatically add options (used in constructor)
        static interface_description& get_standard_description();

        /// \brief Registers option that should be added to every interface
        template < typename T >
        inline static void register_initialiser() {
          T::add_options(get_standard_description());
        }

        /// \brief Returns the revision number
        inline static std::string const& revision() {
          static std::string revision;

          return revision;
        }

        /// \brief Internal use only
        inline interface_description() {
        }

      public:

        /**
         * \brief Constructor
         *
         * \param[in] path path or name that identifies the executable
         * \param[in] name the name of the tool
         * \param[in] authors string with the names of the authors
         * \param[in] what_is one-line description of the program (used only in man pages, must not refer to the synopsis)
         * \param[in] synopsis message that gives an abstract summary of tool usage
         * \param[in] description message that explains tool usage and description (may refer to the synopsis)
         * \param[in] known_issues textual description of known issues with the tool
         **/
        interface_description(std::string const& path, std::string const& name, std::string const& authors,
                    std::string const& what_is, std::string const& synopsis, std::string const& description,
                    std::string const& known_issues = "");

        /**
         * \brief Composes a copyright message
         * \return formatted string that represents the copyright message
         **/
        static std::string copyright_message();

        /**
         * \brief Composes a version information message
         * \return formatted string that represents version information
         *
         * The following example shows the effect of this method for a tool named test and author John Doe.
         *
         * \code
         *  std::cout << version_information();
         * \endcode
         *
         * The output is:
         *
         * \verbatim
           test January 2008 (revision 4321M-shared)
           Copyright (C) 2008 Technische Universiteit Eindhoven.
           This is free software.  You may redistribute copies of it under the
           terms of the Boost Software License <http://www.boost.org/LICENSE_1_0.txt>.
           There is NO WARRANTY, to the extent permitted by law.

           Written by John Doe \endverbatim
         *
         * Where toolset version, revision and copyright year are constants
         * supplied internally at compile time.
         **/
        std::string version_information() const;

        /**
         * \brief Adds an option with argument identified by a long-identifier to the interface
         *
         * Adds an option identified by a long-identifier with an argument to
         * the interface. The argument to the option is either optional, or
         * mandatory. In the former case a default value is assumed when the
         * option is found on a command line command. Method parameters:
         *
         * \param[in] long_identifier the long option representation of the option
         * \param[in] argument_specification a mandatory or optional argument to the option
         * \param[in] description description of the option
         * \param[in] short_identifier an optional single-character short representation of the option
         *
         * the type parameter T represents is type of the argument. That is the
         * argument to the option must be convertible (using <<) to T. 
         *
         * The argument_specification parameter is an object of a class derived
         * from basic_argument. Appropriate argument specification objects can
         * be obtained by using the friend functions make_mandatory_argument()
         * and make_optional_argument().
         *
         * More specifically it specifies one of the two template types:
         * \li interface_description::optional_argument< std::string >
         * representing a untyped optional option argument or,
         * \li interface_description::mandatory_argument< std::string > representing a untyped
         * mandatory option argument.
         *
         * The current implementation does not perform type checking of
         * arguments. More concretely it does not check whether a value
         * representing an argument with type mandatory_argument< int > to type
         * int.
         *
         * \return *this
         * \pre long_identifier must be a non-empty string that only contain characters from [a-z0-9] or `-'
         * \pre short_identifier must a single character [a-zA-Z0-9]
         * \throw std::runtime_error when an option with long_identifier is already part of the interface
         * \throw std::runtime_error when an option with short_identifier is already part of the interface
         * \see add_option(std::string const&, std::string const&, char const)
         *
         * The following example shows how to add mandatory and optional options:
         * \code
         *  add_option("timeout", make_mandatory_argument("SEC"), "timeout occurs after SEC number of seconds", 't');
         *  add_option("recursive", make_optional_argument("DEPTH", "2"), "stop at recursion level DEPTH (default 2)", 'r');
         * \endcode
         * The result is a command line interface with parser behaviour:
         * \verbatim
            tool --timeout=2 --recursive              (pass)
            tool -t2 -r3                              (pass)
            tool --timeout=bla --recursive=bla        (pass)
            tool --timeout                            (failure)
            tool -t                                   (failure) \endverbatim
         *
         * The human readable interface specification for these options is as follows:
         *
         * \verbatim
             -tSEC, --timeout=SEC        timeout occurs after SEC number of seconds
             -rDEPTH, --recursive=[DEPTH]  stop at recursion level DEPTH (default 2) \endverbatim
         **/
        template < typename T >
        interface_description& add_option(std::string const& long_identifier,
                                          typed_argument< T > const& argument_specification,
                                          std::string const& description,
                                          char const short_identifier = '\0') {

          add_option(long_identifier, description, short_identifier);

          m_options.find(long_identifier)->second.set_argument(argument_specification.clone());

          return *this;
        }

        /**
         * \brief Adds an option identified by a long-identifier to the interface
         * \param[in] long_identifier the long option representation of the option
         * \param[in] description description of the option
         * \param[in] short_identifier an optional single-character short representation of the option
         * \return *this
         * \pre long_identifier must be a non-empty string that only contain characters from [a-z0-9] or `-' '_'
         * \pre short_identifier must a single character [a-zA-Z0-9]
         * \throw std::runtime_error when an option with long_identifier is already part of the interface
         * \throw std::runtime_error when an option with short_identifier is already part of the interface
         *
         * The following example shows how to add an option without arguments
         * \code
         *  add_option("recursive", "recursively scans a directory", 'r');
         *  add_option("test", "tests\n a lot");
         * \endcode
         * The result is a command line interface with the following example parsing behaviour:
         * \verbatim
            tool --test                               (pass)
            tool --test --test                        (pass)
            tool -r --recursive                       (pass)
            tool --r                                  (failure)
            tool -t                                   (failure) \endverbatim
         *
         * The human readable interface specification for these options is as follows:
         *
         * \verbatim
             -r, --recursive          recursively scans a directory
                 --test               tests
                                       a lot \endverbatim
         **/
        interface_description& add_option(std::string const& long_identifier,
                                          std::string const& description,
                                          char const short_identifier = '\0') {

          if (m_options.find(long_identifier) != m_options.end()) {
            throw std::logic_error("Duplicate long option (--" + long_identifier + "); this is a serious program error!");
          }

          if (short_identifier != '\0') {
            if (m_short_to_long.find(short_identifier) != m_short_to_long.end()) {
              throw std::logic_error("Duplicate short option (-" + std::string(1, short_identifier) + "); this is a serious program error!");
            }

            m_short_to_long[short_identifier] = long_identifier;
          }

          m_options.insert(std::make_pair(long_identifier, option_descriptor(long_identifier, description, short_identifier)));

          return *this;
        }

        /**
         * \brief adds a hidden option
         *
         * A hidden option is not advertised as being part of the interface.
         * As a consequence the result of class methods such as textual_description,
         * man_page and wiki_page will not contain information about these options.
         *
         * \see add_option(std::string const&, basic_argument const& std::string const&, char const)
         **/
        template < typename T >
        void add_hidden_option(
                        std::string const& long_identifier,
                        typed_argument< T > const& argument_specification,
                        std::string const& description,
                        char const short_identifier = '\0') {

          add_option(long_identifier, argument_specification, description, short_identifier);

          m_options.find(long_identifier)->second.m_show = false;
       }

        /**
         * \brief adds a hidden option
         * \see add_hidden_option(std::string const&, basic_argument const& std::string const&, char const)
         * \see add_option(std::string const&, std::string const&, char const)
         **/
        void add_hidden_option(std::string const& long_identifier,
                               std::string const& description,
                               char const short_identifier = '\0') {

          add_option(long_identifier, description, short_identifier);

          m_options.find(long_identifier)->second.m_show = false;
        }

        /**
         * \brief Generates a human readable interface description (used for -h,--help)
         * \return string containing a description of the interface
         **/
        std::string textual_description() const;

        /**
         * \brief Returns the text of a man page
         * \return string containing a man page description of the interface
         **/
        std::string man_page() const;

        /**
         * \brief Returns the text of a wiki page
         * \param[in] revision the revision tag used in the heading of the man page
         * \return string containing a man page description of the interface
         **/
        std::string wiki_page() const;

        /**
         * \brief Returns the available long arguments with their associated help description
         * \return mapping consisting of strings where the key represent the long argument and the mapped value the help description
         **/
        std::map<std::string, std::string>  get_long_argument_with_description();

        /**
         * \brief Returns the toolname
         * \return string containing the name of the tool
         **/
        std::string get_toolname(){
        	return m_name;
        }
    };

    /**
     * \brief Main parsing component for command line strings as well as
     * interface to the results of parsing. The input is a specification of an
     * interface.
     *
     * This class represents one of the two main interface components
     * responsible for actual parsing of the command line and communicating the
     * results. The other component the interface_description class provides an
     * interface description along with functionality to format messages to the
     * user of the tool.
     *
     * Access to parse results is provided through the STL multimap options
     * (type std::multimap) and the STL vector arguments (type std::vector).
     * The former is mapping between an option (through long-option identifier)
     * found on the command line to the string that represents its argument.
     * The latter contains from left-to-right the command line arguments that
     * were not recognised as option or argument to option.
     *
     * The following example illustrates the use of parsing results:
     * \code
     *  interface_description interface("tool", "tool-name", "John Doe", "[OPTIONS]... [PATHS]");
     *
     *  // Note default options: --help,(-h), --version, --verbose, --debug (-d) and --quiet (-q)
     *  cli.
     *   add_option("recursive", "recursively test all files in directories", 'r').
     *   add_option("tool", make_mandatory_argument("FOO"), "path that identifies the tool executable to test with", 't').
     *   add_option("language", make_mandatory_argument("LANG", "english"), "language for output").
     *   add_option("timeout", make_optional_argument("SEC", "2"), "optional timeout period");
     *
     *  try {
     *    // parse command line
     *    command_line_parser parser(cli, "test -v --verbose -r --timeout --tool=foo bar1 bar2");
     *
     *    std::cerr << parser.options.count("recursive");        // prints: 1
     *    std::cerr << parser.options.count("verbose");          // prints: 2
     *    std::cerr << parser.options.count("tool");             // prints: 1
     *    std::cerr << parser.options.count("timeout");          // prints: 1
     *    std::cerr << parser.options.count("bar1");             // prints: 0
     *
     *    std::cerr << parser.option_argument("tool");           // prints: "foo"
     *    std::cerr << parser.option_argument_as< int >("tool"); // prints: 2
     *    std::cerr << parser.option_argument("recursive");      // prints: ""
     *
     *    std::cerr << parser.arguments.size();                  // prints: 2
     *    std::cerr << parser.arguments[0];                      // prints: "bar1"
     *    std::cerr << parser.arguments[1];                      // prints: "bar2"
     *
     *    std::cerr << parser.options.count("language");         // prints: 0
     *    std::cerr << parser.option_argument("language");       // prints: "english"
     *  }
     *  catch (std::exception& e) {
     *    std::cerr << e.what() << std::endl;
     *  }
     * \endcode
     *
     * Note the use of the option_argument method.  It simplifies argument
     * extraction in the case that the same option does not occur multiple
     * times the auxiliary functions option_argument() and option_argument_as()
     * are provided.
     **/
    class command_line_parser {
      template < typename T >
      friend struct detail::initialiser;

      public:

        /// Used to maps options to arguments
        typedef std::multimap< std::string, std::string >  option_map;

        /// Used to store command line arguments that were not recognised as option or arguments to options
        typedef std::vector< std::string >                 argument_list;

      private:

        /// \brief The list of options found on the command line
        option_map              m_options;

        /// \brief The list of arguments that have not been matched with an option
        argument_list           m_arguments;

        /// \brief The command line interface specification
        interface_description&  m_interface;

        /// \brief Whether the actions after parsing indicate that program execution should continue
        bool                    m_continue;

      public:

        /// \brief Maps options found on the command line to their argument or the empty string
        option_map const&       options;

        /// \brief The list of arguments that have not been matched with an option
        argument_list const&    arguments;

      private:

        /// \brief Parses string as if it is an unparsed command line
        static std::vector< std::string > parse_command_line(char const* const arguments);

        /// \brief Converts C-style array with specified length to STL vector of strings
        static std::vector< std::string > convert(const int count, char const* const* const arguments);

        /// \brief Converts C-style array with specified length to STL vector of strings
        static std::vector< std::string > convert(const int count, wchar_t const* const* const arguments);

        /// \brief Identifies all options and option arguments
        void collect(interface_description& d, std::vector< std::string > const& arguments);

        /// \brief Executes actions for default options
        void process_default_options(interface_description& d);

        /// \brief Stores a sequence of actions that are automatically executed after parsing completes
        static std::vector< bool (*)(command_line_parser&) >& get_registered_actions() {
          static std::vector< bool (*)(command_line_parser&) > actions;

          return actions;
        }

        /// \brief Registers default actions after successful parsing
        static void register_action(bool (*action)(command_line_parser&)) {
          get_registered_actions().push_back(action);
        }

      public:

        /**
         * \brief Parses a string that represents a command on the command
         * line, and executes default procedures for default options.
         * \param[in] interface_specification the interface description
         * \param[in] command_line the string that represents the unparsed command line
         * \throws std::runtime_error
         **/
        inline command_line_parser(interface_description& interface_specification, char const* const command_line) :
                     m_interface(interface_specification), m_continue(true), options(m_options), arguments(m_arguments) {
          collect(interface_specification, parse_command_line(command_line));

          process_default_options(interface_specification);
        }

        /**
         * \brief Recognises options from an array that represents a pre-parsed
         * command on the command line, and executes default procedures for
         * default options.
         * \param[in] interface_specification the interface description
         * \param[in] argument_count amount of arguments
         * \param[in] arguments C-style array with arguments
         * \throws std::runtime_error
         **/
        template < typename CharacterType >
        command_line_parser(interface_description& interface_specification,
                                const int argument_count, CharacterType const* const* const arguments);

        /**
         * \brief Throws standard formatted std::runtime_error exception
         * \param[in] message mandatory message indicating what went wrong
         *
         * The following example shows the output of this method for a tool named test.
         *
         * \code
         *  try {
         *    error("Parse error: option -b unknown");
         *  }
         *  catch (std::exception& e) {
         *    std::cerr << e.what();
         *  }
         * \endcode
         *
         * The output is:
         *
         * \verbatim
         * tool: Parse error: option -b unknown
         * Try `tool --help' for more information. \endverbatim
         **/
        void error(std::string const& message) const;

        /**
         * \brief Returns the argument of the first option matching a name
         * \param[in] long_identifier the long identifier for the option
         * Finds and returns the argument of an option with long identifier
         * matching long_identifier in this.options. There may be more than one
         * occurence of the option in this.options, if so the first argument
         * to the option will be returned. A default value is returned when a
         * user did not specify the option matching long_identifier on the
         * command line. A default value is only returned when the option
         * matching long_identifier is either an option with optional argument
         * or when it is an option with mandatory argument and a standard value.
         * \return options.find(long_identifier)->second
         * \pre 0 < options.count(long_identifier)
         * \throw std::logic_error containing a message that the option was not part of the command
         * \throw std::logic_error containing a message that the option does not take argument
         **/
        std::string const& option_argument(std::string const& long_identifier) const;

        /**
         * \brief Whether main program execution should continue
         **/
        inline bool continue_execution() const {
          return m_continue;
        }

        /**
         * \brief Returns the converted argument of the first option matching a name
         * \param[in] long_identifier the long identifier for the option
         * \pre 0 < options.count(long_identifier) and !options.find(long_identifier)->second.empty()
         * \throw std::runtime_error containing a message that the argument cannot be converted to the specified type
         * \return t : T where t << std::istringstream(option_argument_as(long_identifier))
         * \see std::string const& option_argument(std::string const& long_identifier)
         **/
        template < typename T >
        inline T option_argument_as(std::string const& long_identifier) const {
          std::istringstream in(option_argument(long_identifier));

          detail::instance_of< T > result;

          in >> result.wrapped();

          if (in.fail()) {
            const char short_option(m_interface.long_to_short(long_identifier));

            error("argument `" + option_argument(long_identifier) + "' to option --" + long_identifier +
                ((short_option == '\0') ? " " : " or -" + std::string(1, short_option)) + " is invalid");
          }

          return result.wrapped();
        }
    };

    /// Creates an optional option argument specification object
    template < typename ArgumentType >
    interface_description::optional_argument< ArgumentType > make_optional_argument(std::string const& name, std::string const& default_value) {
      return interface_description::optional_argument< ArgumentType >(name, default_value);
    }

    /** \brief Creates an optional option argument specification object
     *
     * Creates an object that specifies an option with an optional typed argument.
     * The default value is automatically substituted when the user specifies
     * the option but does not specify an option argument.
     *
     * \param[in] name a placeholder for referencing the argument in textual descriptions
     * \param[in] default_value the default value
     * \return a basic_argument derived object that represents an untyped optional option argument
     *
     * The following example demonstrates the effect of an option with optional argument:
     * \code
     *  add_option("recursive", make_optional_argument("DEPTH", "2"),
     *                         "stop at recursion level DEPTH (default 2)", 'r');
     * \endcode
     * The result is a command line interface with parsing behaviour:
     * \verbatim
       tool                     (effect: options("recursive").count() == 0)
       tool --recursive         (effect: options("recursive").count() == 1 && option_argument("recursive") == 2)
       tool -r                  (effect: options("recursive").count() == 1 && option_argument("recursive") == 2)
       tool --recursive=3       (effect: options("recursive").count() == 1 && option_argument("recursive") == 3)
       tool -r3                 (effect: options("recursive").count() == 1 && option_argument("recursive") == 3)
     */
    interface_description::optional_argument< std::string > make_optional_argument(std::string const& name, std::string const& default_value);

    /// Creates a mandatory option argument specification object
    template < typename ArgumentType >
    interface_description::mandatory_argument< ArgumentType > make_mandatory_argument(std::string const& name) {
      return interface_description::mandatory_argument< ArgumentType >(name);
    }

    /**
     * Creates an object that specifies an option with a mandatory argument.
     * Specifying the option in a command also requires specification of an
     * option argument.
     *
     * \param[in] name a placeholder for referencing the argument in textual descriptions
     * \return a basic_argument derived object that represents an untyped mandatory option argument
     *
     * The following example demonstrates the effect of an option with optional argument:
     * \code
     *  add_option("recursive", make_mandatory_argument("DEPTH"),
     *                         "stop at recursion level DEPTH", 'r');
     * \endcode
     * The result is a command line interface with parsing behaviour:
     * \verbatim
       tool                     (effect: options("recursive").count() == 0)
       tool --recursive         (effect: parsing fails)
       tool -r                  (effect: parsing fails)
       tool --recursive=3       (effect: options("recursive").count() == 1 && option_argument("recursive") == 3)
       tool -r3                 (effect: options("recursive").count() == 1 && option_argument("recursive") == 3)
     **/
    interface_description::mandatory_argument< std::string >
         make_mandatory_argument(std::string const& name);

    /** \brief Creates a mandatory typed option argument specification object
     *
     * \see make_mandatory_argument(std::string const&)
     **/
    template < typename ArgumentType >
    interface_description::mandatory_argument< ArgumentType >
    make_mandatory_argument(std::string const& name, std::string const& standard_value) {
      return interface_description::mandatory_argument< ArgumentType >(name, standard_value);
    }

    /**
     * Creates an object that specifies an option with a mandatory argument.
     * Specifying the option in a command also requires specification of an
     * option argument. The default value is substituted by the
     * option_argument() and option_argument_as() methods when the option is
     * not part of the parsed command.
     *
     * \param[in] name a placeholder for referencing the argument in textual descriptions
     * \return a basic_argument derived object that represents an untyped mandatory option argument
     *
     * The following example demonstrates the effect of an option with optional argument:
     * \code
     *  add_option("recursive", make_mandatory_argument("DEPTH", "2"),
     *                         "stop at recursion level DEPTH (default 2)", 'r');
     * \endcode
     * The result is a command line interface with parsing behaviour:
     * \verbatim
       tool                     (effect: options("recursive").count() == 0 && option_argument("recursive") == 2)
       tool --recursive         (effect: options("recursive").count() == 0 && option_argument("recursive") == 2)
       tool -r                  (effect: options("recursive").count() == 0 && option_argument("recursive") == 2)
       tool --recursive=3       (effect: options("recursive").count() == 1 && option_argument("recursive") == 3)
       tool -r3                 (effect: options("recursive").count() == 1 && option_argument("recursive") == 3)
     *
     **/
    interface_description::mandatory_argument< std::string >
      make_mandatory_argument(std::string const& name, std::string const& standard_value);
    /// \cond INTERNAL

    /**
     * \brief Represents a typed argument
     *
     * A typed argument is more specific than an untyped argument. All
     * arguments are represented by strings, but typed arguments have the
     * additional restriction that the string is checked to be convertible
     * (with >>) to the type specified by the type parameter T.
     **/
    template < typename T >
    class interface_description::typed_argument : public basic_argument {

      public:

        /// Checks whether string is convertible to a value of a specific type
        inline bool validate(std::string const& s) const {
          std::istringstream test(s);

          T result;

          test >> result;

          return !test.fail();
        }
    };

    /**
     * \brief specialisation for type std::string
     * \param[in] s the string to validate
     *
     * Specialisation for the case of a string. Every string is convertible to
     * a string so the implementation is trivial.
     **/
    template < >
    inline bool interface_description::typed_argument< std::string >::validate(std::string const& ) const {
      return true;
    }

    /// Represents an optional argument to an option
    template < typename T >
    class interface_description::optional_argument : public typed_argument< T > {
      friend class interface_description;
      friend class interface_description::option_descriptor;
      friend class command_line_parser;

      protected:

        /// default value
        std::string m_default;

      public:

        virtual basic_argument* clone() const {
          return new optional_argument< T >(*this);
        }

        /**
         * Constructor
         * \param[in] n the name of the argument
         * \param[in] d the default value for the argument
         **/
        inline optional_argument(std::string const& name, std::string const& d) {
          basic_argument::set_name(name);

          m_default = d;
        }

        /**
         * \brief Throws because mandatory arguments have no default
         **/
        inline bool has_default() const {
          return true;
        }

        /**
         * \brief Returns the default argument
         **/
        inline std::string const& get_default() const {
          return m_default;
        }

        /// whether the argument is optional or not
        inline bool is_optional() const {
          return true;
        }
    };

    /// Represents a mandatory argument to an option
    template < typename T >
    class interface_description::mandatory_argument : public typed_argument< T > {

      protected:

        /// default value
        std::string m_default;

        /// whether a default value has been specified
        bool        m_has_default;

      public:

        virtual basic_argument* clone() const {
          return new mandatory_argument< T >(*this);
        }

        /**
         * Constructor
         * \param[in] n the name of the argument
         **/
        inline mandatory_argument(std::string const& name) : m_has_default(false) {
          basic_argument::set_name(name);
        }

        /**
         * Constructor
         * \param[in] n the name of the argument
         **/
        inline mandatory_argument(std::string const& name, std::string const& d) : m_default(d), m_has_default(true) {
          basic_argument::set_name(name);
        }

        /**
         * \brief Throws because mandatory arguments have no default
         **/
        inline std::string const& get_default() const {
          return m_default;
        }

        /**
         * \brief Throws because mandatory arguments have no default
         **/
        inline bool has_default() const {
          return m_has_default;
        }

        /// whether the argument is optional or not
        inline bool is_optional() const {
          return false;
        }
    };

#if !defined(__COMMAND_LINE_INTERFACE__)
    namespace detail {
      template <>
      struct initialiser< void > {
        static bool set_revision(std::string const& r) {
          const_cast< std::string& > (interface_description::revision()) = r;

          return true;
        }
      };

      static bool initialised = initialiser< void >::set_revision(get_toolset_revision());
    }

    template <>
    inline command_line_parser::command_line_parser(interface_description& d, const int c, char const* const* const a) :
                                         m_interface(d), m_continue(true), options(m_options), arguments(m_arguments) {

      collect(d, convert(c, a));

      static_cast< void >(mcrl2::utilities::detail::initialised); // prevents unused variable warnings

      process_default_options(d);
    }
# ifndef __CYGWIN__ // std::wstring is not available for Cygwin
    template <>
    inline command_line_parser::command_line_parser(interface_description& d, const int c, wchar_t const* const* const a) :
                                         m_interface(d), m_continue(true), options(m_options), arguments(m_arguments) {

      collect(d, convert(c, a));

      process_default_options(d);
    }
# endif // __CYGWIN__
#endif
    /// \endcond
  }
}

#endif
