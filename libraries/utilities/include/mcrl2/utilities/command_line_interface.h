// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/command_line_interface.h

#include <algorithm>
#include <vector>
#include <map>
#include <iostream>
#include <cassert>
#include <sstream>
#include <memory>

#include "boost/algorithm/string.hpp"

#include "mcrl2/exception.h"

namespace mcrl2 {
  namespace utilities {

    /** \brief toolset version tag */
    const std::string version_tag("July 2008 (development)");

    /** \brief toolset copyright period description */
    const std::string copyright_period("2008");

#if defined(__LIBREWRITE_H)
    inline std::istream& operator>>(std::istream& is, RewriteStrategy& s) {
      char strategy[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    
      is.readsome(strategy, 9);

      s = RewriteStrategyFromString(strategy);

      if (s == GS_REWR_INVALID) {
        is.setstate(std::ios_base::failbit);
      }
    
      return is;
    }
#endif
#if defined(BDD_PATH_ELIMINATOR_H)
    inline std::istream& operator>>(std::istream& is, SMT_Solver_Type& s) {
      char solver_type[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    
      /// no standard conversion available function, so implement on-the-spot
      is.readsome(solver_type, 10);

      s = solver_type_cvc;

      if (std::strncmp(solver_type, "ario", 5) == 0) {
        s = solver_type_ario;
      }
      else if (std::strncmp(solver_type, "cvc", 3) == 0) {
        if (solver_type[3] != '\0') {
          is.setstate(std::ios_base::failbit);
        }
      }
      else {
        is.setstate(std::ios_base::failbit);
      }

      return is;
    }
#endif

    class command_line_parser;

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
     * The following example illustrates use of the interface description class:
     * \code
     *  interface_description interface("/path/to/demo-tool",
     *          "demo-tool", "Rincewind", "[OPTIONS]... [PATHS]"
     *          "Serves as demonstration of the features of the interface_description class.");
     *
     *  // Note default options: --help,(-h), --version, --verbose, --debug (-d) and --quiet (-q)
     *  cli.
     *   add_option("recursive", "recursively test all files in directories", 'r').
     *   add_option("tool", make_mandatory_argument("FOO"), "path that identifies the tool executable to test with").
     *   add_option("timeout", make_optional_argument("SEC", "2"), "optional timeout period (default 2 seconds)").
     *   add_rewriting_options().
     *   add_prover_options();
     *
     *  std::cerr << cli.textual_description() << std::endl;
     *
     *  std::cerr << "VERSION INFORMATION:" << std::endl
     *            << std::endl
     *            << cli.version_information() << std::endl;
     * \endcode
     * The output produced when running the program is something similar to:
     * \verbatim
        interface description:
        /path/to/demo-tool [OPTIONS]... [PATHS]
        Serves as demonstration of the features of the interface_description
        class.
      
        Options:
          -r, --recursive          recursively test all files in directories
          -t[SEC], --timeout=[SEC] optional timeout period (default 2 seconds)
              --tool=FOO           path that identifies the tool executable
                                   to test with
      
        Standard options:
          -q, --quiet              do not display warning messages
          -v, --verbose            display short intermediate messages
          -d, --debug              display detailed intermediate messages
          -h, --help               display help information
              --version            display version information
      
        Report bugs at <http://www.mcrl2.org/issuetracker>.
      
        See also the manual at <http://www.mcrl2.org/wiki/index.php/demo-tool>.
      
        VERSION INFORMATION:
      
        demo-tool July 2008 (development) (revision 4624M-shared)
        Copyright (c) 2008 Technische Universiteit Eindhoven.
        This is free software.  You may redistribute copies of it under the
        terms of the Boost Software License <http://www.boost.org/LICENSE_1_0.txt>.
        There is NO WARRANTY, to the extent permitted by law.
      
        Written by Rincewind. \endverbatim
     **/
    class interface_description {
      friend class command_line_parser;

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
            std::string                       m_long;

            /// Description for the option
            std::string                       m_description;

            /// Option argument
            std::auto_ptr < basic_argument >  m_argument;

            /// Short representation for the option or 0
            char                              m_short;

            /// whether the option is printed as part of the tool-specific interface
            bool                              m_show;

          protected:

            /// Returns a textual description of the option
            std::string textual_description(const size_t left_width, const size_t right_width) const;

            /// Returns a man page description for the option
            std::string man_page_description() const;

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
                    m_description(o.m_description), m_short(o.m_short), m_show(true) {

              m_argument.reset(const_cast< std::auto_ptr< basic_argument >& > (o.m_argument).release());
            }

            option_descriptor operator=(option_descriptor const& o) {
              return option_descriptor(o);
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
             * \pre m_argument->is_optional()
             **/
            inline std::string get_default() const {
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
             * \brief Whether the option takes a mandatory argument
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

        typedef std::map< std::string, option_descriptor > option_map;

        typedef std::map< const char,  std::string, option_identifier_less > short_to_long_map;

      private:

        /// \brief Path to executable (as in the first command line argument)
        std::string       m_path;

        /// \brief Name of the tool for usage and tool description
        std::string       m_name;

        /// \brief Comma separated list of authors
        std::string       m_authors;

        /// \brief Usage and description
        std::string       m_usage;

        /// \brief Tool description
        std::string       m_description;

        /// \brief Description of known issues
        std::string       m_known_issues;

        /// \brief Maps long option identifiers to option descriptor objects
        option_map        m_options;

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

        /**
         * \brief adds a hidden option
         * \see add_option(std::string const&, basic_argument const& std::string const&, char const)
         **/
        void add_hidden_option(
                        std::string const& long_identifier,
                        basic_argument const& argument_specification,
                        std::string const& description,
                        char const short_identifier = '\0');

        /**
         * \brief adds a hidden option
         * \see add_option(std::string const&, std::string const&, char const)
         **/
        void add_hidden_option(std::string const& long_identifier,
                                                 std::string const& description,
                                                 char const short_identifier = '\0');

      public:

        /**
         * \brief Constructor
         *
         * \param[in] path path or name that identifies the executable
         * \param[in] name the name of the tool
         * \param[in] authors string with the names of the authors
         * \param[in] synopsis message that gives an abstract summary of tool usage
         * \param[in] description message that explains tool usage and description
         * \param[in] known_issues textual description of known issues with the tool
         **/
        interface_description(std::string const& path, std::string const& name, std::string const& authors,
                     std::string const& synopsis, std::string const& description, std::string const& known_issues = "");

        /**
         * \brief Composes a copyright message
         * \return formatted string that represents the copyright message
         **/
        std::string copyright_message() const;

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
        inline std::string version_information() const {
          return m_name + " mCRL2 toolset " + version_tag + " (revision " + std::string(MCRL2_REVISION) + ")\n" +
                 copyright_message() +
                 "\nWritten by " + m_authors + ".\n";
        }

        /**
         * \brief Adds an option with argument identified by a long-identifier to the interface
         *
         * Adds an option identified by a long-identifier with an argument to
         * the interface. The argument to the option is either optional, or
         * mandatory. In the former case a default value is assumed when the
         * option is found on a command line command.
         *
         * \param[in] long_identifier the long option representation of the option
         * \param[in] argument_specification a mandatory or optional argument to the option
         * \param[in] description description of the option
         * \param[in] short_identifier an optional single-character short representation of the option
         *
         * The argument_specification parameter is an object of a class derived
         * from basic_argument. Appropriate argument specification objects can
         * be obtained by using the friend functions make_mandatory_argument()
         * and make_optional_argument().
         *
         * More specifically it specifies one of the two template types:
         * \li interface_description::optional_argument< std::string >
         * representing a untyped optional option argument or,
         * interface_description::mandatory_argument< std::string > representing a untyped
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
        interface_description& add_option(std::string const& long_identifier,
                                          basic_argument const& argument_specification,
                                          std::string const& description,
                                          char const short_identifier = '\0');

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
                                          char const short_identifier = '\0');

        /**
         * \brief Adds options for the rewriter
         * \return *this
         **/
        interface_description& add_rewriting_options();

        /**
         * \brief Adds options for the prover
         * \return *this
         **/
        interface_description& add_prover_options();

        /**
         * \brief Generates a human readable interface description (used for -h,--help)
         * \return string containing a description of the interface
         **/
        std::string textual_description() const;

        /**
         * \brief Returns the text of a man page
         * \param[in] revision the revision tag used in the heading of the man page
         * \return string containing a man page description of the interface
         **/
        std::string man_page(std::string const& revision) const;
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
     *   add_option("timeout", make_optional_argument("SEC", "2"), "optional timeout period");
     *
     *  try {
     *    // parse command line
     *    command_line_parser parser(cli, "test -v --verbose -r --timeout --tool=foo bar1 bar2");
     * 
     *    std::cerr << parser.options.count("recursive");        // prints: "1"
     *    std::cerr << parser.options.count("verbose");          // prints: "2"
     *    std::cerr << parser.options.count("tool");             // prints: "1"
     *    std::cerr << parser.options.count("timeout");          // prints: "1"
     *    std::cerr << parser.options.count("bar1");             // prints: "0"
     * 
     *    std::cerr << parser.option_argument("tool");           // prints: "foo"
     *    std::cerr << parser.option_argument_as< int >("tool"); // prints: "2"
     *    std::cerr << parser.option_argument("recursive");      // prints: ""
     * 
     *    std::cerr << parser.arguments.size();                  // prints: "2"
     *    std::cerr << parser.arguments[0];                      // prints: "bar1"
     *    std::cerr << parser.arguments[1];                      // prints: "bar2"
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

      public:

        /// \brief The list of options found on the command line
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

      public:

        /**
         * \brief Parses a string that represents a command on the command
         * line, and executes default procedures for default options.
         * \param[in] interface_specification the interface description
         * \param[in] command_line the string that represents the unparsed command line
         **/
        inline command_line_parser(interface_description& interface_specification, char const* const command_line) :
                                         m_interface(interface_specification), options(m_options), arguments(m_arguments) {
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
         **/
        template < typename CharacterType >
        command_line_parser(interface_description& interface_specification,
                                const int argument_count, CharacterType const* const* const arguments);

        /**
         * \brief Throws standard formatted std::runtime_error exception
         * \param[in] message the exception message
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
         * \return options.find(long_identifier)->second
         * \pre 0 < options.count(long_identifier)
         * \throw std::logic_error containing a message that the option
         * was not part of the command
         * \throw std::logic_error containing a message that the option
         * does not take argument
         **/
        std::string const& option_argument(std::string const& long_identifier) {
          if (options.count(long_identifier) == 0) {
            throw std::logic_error("Fatal error: argument requested of unspecified option!");
          }
          else if (!m_interface.m_options.find(long_identifier)->second.accepts_argument()) {
            throw std::logic_error("Fatal error: argument requested of option that does not take an argument!");
          }

          return options.find(long_identifier)->second;
        }

        /**
         * \brief Returns the converted argument of the first option matching a name
         * \param[in] long_identifier the long identifier for the option
         * \pre 0 < options.count(long_identifier) and !options.find(long_identifier)->second.empty()
         * \throw std::runtime_error containing a message that the argument cannot be converted to the specified type
         * \return t : T where t << std::istringstream(options.find(long_identifier)->second)
         **/
        template < typename T >
        inline T option_argument_as(std::string const& long_identifier) {
          std::istringstream in(option_argument(long_identifier));

          T result;
           
          in >> result;

          if (in.fail()) {
            const char short_option(m_interface.long_to_short(long_identifier));

            error("argument `" + option_argument(long_identifier) + "' to option --" + long_identifier +
                ((short_option == '\0') ? " " : " or -" + std::string(1, short_option)) + " is invalid");
          }

          return result;
        }
    };

    /// Creates an optional option argument specification object
    interface_description::optional_argument< std::string > make_optional_argument(std::string const& name, std::string const& default_value);

    /// Creates a mandatory option argument specification object
    interface_description::mandatory_argument< std::string > make_mandatory_argument(std::string const& name);

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
       
          s >> result;
         
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
    inline bool interface_description::typed_argument< std::string >::validate(std::string const& s) const {
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
    
      public:
    
        virtual basic_argument* clone() const {
          return new mandatory_argument< T >(*this);
        }

        /**
         * Constructor
         * \param[in] n the name of the argument
         **/
        inline mandatory_argument(std::string const& name) {
          basic_argument::set_name(name);
        }

        /**
         * \brief Throws because mandatory arguments have no default
         **/
        inline std::string const& get_default() const {
          throw std::logic_error("Fatal error: default argument requested for mandatory option! (this is a bug)\n");
        }

        /// whether the argument is optional or not
        inline bool is_optional() const {
          return false;
        }
    };

#if !defined(__COMMAND_LINE_INTERFACE__)
    template <>
    command_line_parser::command_line_parser(interface_description& d, const int c, char const* const* const a) :
                                         m_interface(d), options(m_options), arguments(m_arguments) {

      collect(d, convert(c, a));

      process_default_options(d);
    }

#ifndef __CYGWIN__ // std::wstring is not available for Cygwin
    template <>
    command_line_parser::command_line_parser(interface_description& d, const int c, wchar_t const* const* const a) :
                                         m_interface(d), options(m_options), arguments(m_arguments) {

      collect(d, convert(c, a));

      process_default_options(d);
    }
#endif // __CYGWIN__

    /**
     * \param[in] d the interface description
     **/
    void command_line_parser::process_default_options(interface_description& d) {
      if (m_options.count("help")) {
        std::cout << d.textual_description();

        exit(0);
      }
      else if (m_options.count("version")) {
        std::cout << d.version_information();

        exit(0);
      }
      else if (m_options.count("generate-man-page")) {
        std::cout << d.man_page(MCRL2_REVISION);

        exit(0);
      }
      else if (!m_options.count("rewriter")) {
        // Add rewrite option for default strategy
        interface_description::option_map::const_iterator i = d.m_options.find("rewriter");

        if (i != d.m_options.end()) {
          // Set default rewriter
          m_options.insert(std::make_pair("rewriter", "jitty"));
        }
      }
#if defined(__LIBREWRITE_H)
      if (m_options.count("rewriter")) {
        option_argument_as< RewriteStrategy >("rewriter");
      }
#endif
#if defined(BDD_PATH_ELIMINATOR_H)
      if (m_options.count("smt-solver")) {
        option_argument_as< SMT_Solver_Type >("smt-solver");
      }
#endif
#if defined(__MCRL2_MESSAGING_H__)
      if (m_options.count("quiet")) {
        if (m_options.count("debug")) {
          error("options -q/--quiet and -d/--debug cannot be used together\n");
        }
        if (m_options.count("verbose")) {
          error("options -q/--quiet and -v/--verbose cannot be used together\n");
        }

        mcrl2::core::gsSetQuietMsg();
      }
      if (m_options.count("verbose")) {
        mcrl2::core::gsSetVerboseMsg();
      }
      if (m_options.count("debug")) {
        mcrl2::core::gsSetDebugMsg();
      }
#endif
    }
#endif
    /// \endcond
  }
}
