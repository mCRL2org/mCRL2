// Author(s): Jeroen van der Wulp
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/command_line_interface.h
//
#include <algorithm>
#include <vector>
#include <map>
#include <iosfwd>
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <memory>

#include <mcrl2/utilities/version_info.h> // for constants?

/**
 * \page notes Notes on the use of this library
 *
 * The interface components contained in this file are *not* for use in
 * libraries. Typical use of this small library is in an mCRL2 tool and in only
 * one source file for every tool.  No inclusion protection is added such that
 * people are made aware of wrong use.
 *
 * Activation of gs*Mesage filtering for --verbose (-v), --quiet (-q),
 * --debug (-d) requires the mcrl2/core/messaging.h header to be included
 *  *before* this file. A similar thing holds for the rewriter options. The
 *  reason is that no unnecessary dependencies on the data and core library are
 *  introduced.
 **/
/**
 * \page introduction A set of reusable components for command line interfacing for the mCRL2 toolset
 *
 * The mCRL2 toolset is growing rapidly and at present contains at least 36
 * tools. All of these tools have similar command line interfaces and very
 * similar usage patterns. Over time a lot of small inconsistencies have
 * arisen between the command line interfaces of the different tools.
 * Recently interface conventions have been formulated, collectively
 * reviewed and accepted, and only need to be applied. The cost of adapting
 * al tools is quite substantial and by itself does not negate the
 * possibility that new inconsistencies will arise in the future.
 *
 * This small library should fill the gap between the conventions and the
 * practice of command line interface programming for mCRL2 tools. It
 * serves two purposes.  Firstly, it standardises command line parsing
 * (using C++) for the mCRL2 toolset. Secondly, the library offers a
 * collection of patterns such as options -that need to be the same for all
 * tools- for configuring functionality imported from libraries (such as
 * the rewriter). Think of it as necessary infrastructure to cut down on
 * long-term maintenance cost for command line interface development and
 * enforcement of conventions.
 *
 * The conventions used for command line interfaces of mCRL2 tools can be found on 
 * the mCRL2 wiki: http://www.mcrl2.org/wiki/index.php/Tool_interface_guidelines .
 * The syntax of a command line command is structured as follows:
 * \code
 *  command ::= program-name (options | plain-arguments) *
 *   options         ::= ( white-space+ option ) *
 *   plain-arguments ::= ( white-space+ [^- \t\n] [^ \t\n]+ ) *
 *    white-space  ::= [ \t\n] +
 *    option       ::= ("--" long-option [ "=" argument ] ) | ("-" short-option [ argument ])
 *     long-option  ::= "--" alpha-low | long-option +
 *     short-option ::= "-" ( alpha-low )
 *      long-option-character  ::= '-' | short-option-character
 *      short-option-character ::= digit | alpha-low
 *       alpha-low             ::= 'a' | 'b' | 'c' | ... | 'z'
 *       alpha-high            ::= 'A' | 'B' | 'C' | ... | 'Z'
 *       digit                 ::= '0' | '1' | '2' | ... | '9'
 * \endcode
 * Some observations:
 *  \li the same options can occur multiple times on the command line
 *  \li options can have an optional default argument
 *  \li options have at most one argument
 *  \li options and plain-arguments can be mixed
 *
 * This library consists of two interface classes, one for interface
 * specification and the other for the actual parsing of a command line command
 * using an interface description. The first class called
 * mcrl2::utilities::interface_description acts as a detailed interface
 * specification containing all options and their possible arguments. In
 * addition it provides functionality for notifying a user about parse errors
 * and for execution of actions for default options.  The second class called
 * mcrl2::utilities::command_line_parser only harbours functionality for
 * parsing the command line using an interface specification, and ways to
 * access the results of parsing.
 */

namespace mcrl2 {
  namespace utilities {

    class command_line_parser;

    /**
     * \brief Command line interface description component.
     *
     * This component can be used to specify the command-line interface of
     * tools in terms of options and arguments to options. The scope is command
     * line interfaces of mCRL2 tools.
     *
     * Options that are mandatory for every tool are automatically present and
     * processed by default. This relieves the creator of a tool from this burden.
     * Options for functionality in toolset libraries that are shared amongst
     * multiple tools can be predefined in this component and included on
     * demand for individual tools.
     *
     * Options are identified by their long
     *
     * \attention See \ref notes for using default processing of options.
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

        /// Describes an individual option
        class option_descriptor;

        /// \endcond

      friend optional_argument< std::string >  make_optional_argument(std::string const&, std::string const&);

      friend mandatory_argument< std::string > make_mandatory_argument(std::string const&);

      private:

        typedef std::map< std::string, option_descriptor > option_map;

        typedef std::map< const char,  std::string >       short_to_long_map;

      private:

        // \brief Path to executable (as in the first command line argument)
        std::string       m_path;

        // \brief Usage and tool description
        std::string       m_name;

        // \brief Comma separated list of authors
        std::string       m_authors;

        // \brief Usage and tool description
        std::string       m_usage;

        // \brief Maps long option identifiers to option descriptor objects
        option_map        m_options;

        // \brief Maps a short option to its associated long option
        short_to_long_map m_short_to_long;

      private:

        /**
         * \brief Convenience method to strip a path to a filename without extension
         * \param[in] s string representing a long option
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

      public:

        /**
         * \brief Constructor
         *
         * \param[in] name tool name
         * \param[in] path the path to the executable (as in the first command line argument)
         * \param[in] authors string describing the authors
         * \param[in] usage message that explains tool usage and description
         * \param[in] messaging_options adds messaging options to interface
         **/
        inline interface_description(std::string const& path, std::string const& name, std::string const& authors,
                                     std::string const& usage, bool messaging_options = true) :
                                        m_path(path), m_name(name), m_authors(authors), m_usage(usage) {
          // Add mandatory options
          add_option("help", "display help information", 'h');
          add_option("version", "display version information");

          if (messaging_options) {
            add_option("quiet", "do not display warning messages", 'q');
            add_option("verbose", "display short intermediate messages", 'v');
            add_option("debug", "display detailed intermediate messages", 'd');
          }
        }

        /**
         * \brief Prints copyright message
         * \param[out] out the stream to which output is written
         *
         * The following example shows the effect of this method for a tool named test and author John Doe.
         *
         * \code
         *  print_copy_message(std::cerr);
         * \endcode
         *
         * The output is:
         *
         * \verbatim
         * test January 2008 (revision 4321M-shared)
         * Copyright (C) 2008 Eindhoven University of Technology.
         * This is free software.  You may redistribute copies of it under the
         * terms of the Boost Software License <http://www.boost.org/LICENSE_1_0.txt>.
         * There is NO WARRANTY, to the extent permitted by law.
         *
         * Written by John Doe
         * \endverbatim
         *
         * Where toolset version, revision and copyright year are constants
         * supplied internally at compile time.
         **/
        inline void print_copy_message(std::ostream& out) const {
          out << m_name << " " << MCRL2_VERSION << " (revision " << MCRL2_REVISION << ")" << std::endl
              << "Copyright (C) " << MCRL2_COPYRIGHT_YEAR << " Eindhoven University of Technology." << std::endl
              << "This is free software.  You may redistribute copies of it under the" << std::endl
              << "terms of the Boost Software License <http://www.boost.org/LICENSE_1_0.txt>." << std::endl
              << "There is NO WARRANTY, to the extent permitted by law." << std::endl
              << std::endl
              << "Written by " << m_authors << "." << std::endl;
        }

        /**
         * \brief Throws standard formatted std::runtime_error exception
         *
         * The following example shows the output of this method for a tool named test.
         *
         * \code
         *  try {
         *    throw_exception("ran out of time!");
         *  }
         *  catch (std::exception& e) {
         *    std::cerr << e.what();
         *  }
         * \endcode
         *
         * The output is:
         *
         * \verbatim
         * test: ran out of time
         * Try `test --help' for more information. \endverbatim
         **/
        void throw_exception(std::string const& m) const;

        /**
         * \brief The long option acts as identifier
         * \param[in] l the long option representation of the option
         * \param[in] a a mandatory or optional argument to the option
         * \param[in] d description of the option
         * \param[in] s an optional single-character short representation of the option
         * \pre l should be a non-empty string that only contain characters from [a-Z] or `-' '_'
         * \pre l must not be a known long option and s must not be a known short option
         * \see add_option(std::string const&, std::string const&, char const)
         **/
        template < typename T >
        interface_description& add_option(std::string const& l, T const& a, std::string const& d, char const s = '\0');

        /**
         * \brief The long option acts as identifier
         * \param[in] l the long option representation of the option
         * \param[in] d description of the option
         * \param[in] s an optional single-character short representation of the option
         * \pre l should be a non-empty string that only contain characters from [a-Z] or `-' '_'
         * \pre l must not be a known long option and s must not be a known short option
         *
         * The following adds a mandatory and optional option:
         * \code
         *  add_option("timeout", make_mandatory_argument("SEC"), "timeout occurs after SEC number of seconds", 's');
         *  add_option("recursive", make_optional_argument("DEPTH", "2"), "stop at recursion level DEPTH (default 2)", 'r');
         * \endcode
         * The result is a command line interface with parser behaviour:
         * \verbatim
         *  test --timeout=2 --recursive              (pass)
         *  test -t2 -r3                              (pass)
         *  test --timeout="bla" --recursive="bla"    (pass)
         *  test --timeout                            (failure)
         *  test -t                                   (failure) \endverbatim
         **/
        interface_description& add_option(std::string const& l, std::string const& d, char const s = '\0');

        /**
         * \brief Adds options for the rewriter
         **/
        void add_rewriting_options();

        /**
         * \brief Prints a human readable interface description
         * \param[out] s the stream to which output is written
         **/
        void print(std::ostream& s) const;
    };

    /**
     * \brief Main parsing component for command line strings as well as
     * interface to the results of parsing. The input is a specification of an
     * interface, as well as functionality to communicate parse problems with
     * the user (see interface_description).
     *
     * This class represents one of the two main interface components
     * responsible for actual parsing of the command line and communicating the
     * results. The other component the interface_description class provides an
     * interface description along with functionality to print and throw
     * standard exceptions in a format that is prescribed by the interface
     * conventions.
     *
     * Access to parse results is provided through the STL multimap options
     * (type std::multimap) and the STL vector unmatched (type std::vector).
     * The former is mapping between an option (through long-option identifier)
     * found on the command line to the string that represents its argument.
     * The latter contains from left-to-right the command line arguments that
     * were not recognised as option or argument to option.
     *
     * The following example illustrates the use of parsing results:
     * \code
     *  interface_description cli("test", "John Doe", "[OPTIONS]... [PATHS]");
     *
     *  // Note default options: --help,(-h), --version, --verbose (-v), --debug (-d) and --quiet (-q)
     *  cli.
     *   add_option("recursive", "recursively test all files in directories", 'r').
     *   add_option("tool", make_mandatory_argument("FOO"), "path that identifies the tool executable to test with", 't').
     *   add_option("timeout", make_optional_argument("SEC", "2"), "optional timeout period");
     *
     *  // parse command line
     *  command_line_parser(cli, "test -v --verbose -r --time --timeout --tool=foo bar1 bar2");
     *
     *  std::cerr << parser.options.count("recursive");        // prints: "1"
     *  std::cerr << parser.options.count("verbose");          // prints: "2"
     *  std::cerr << parser.options.count("tool");             // prints: "1"
     *  std::cerr << parser.options.count("timeout");          // prints: "1"
     *  std::cerr << parser.options.count("bar1");             // prints: "0"
     *
     *  std::cerr << parser.option_argument["tool"];           // prints: "foo"
     *  std::cerr << parser.option_argument_as< int >["tool"]; // prints: "2"
     *  std::cerr << parser.option_argument["recursive"];      // prints: ""
     *
     *  std::cerr << parser.unmatched.size();                  // prints: "2"
     *  std::cerr << parser.unmatched[0];                      // prints: "bar1"
     *  std::cerr << parser.unmatched[1];                      // prints: "bar2"
     * \endcode
     *
     * Note the use of the option_argument method.  It simplifies argument
     * extraction in the case that the same option does not occur multiple
     * times the auxiliary option_argument() and option_argument_as()
     * functions are provided.
     **/
    class command_line_parser {

      private:

        /// Used to maps options to arguments
        typedef std::multimap< std::string, std::string >  option_map;

        /// Used to store command line arguments that were not recognised as option or arguments to options
        typedef std::vector< std::string >                 unmatched_list;

      private:

        /// \brief The list of options found on the command line
        option_map              m_options;

        /// \brief The list of arguments that have not been matched with an option
        unmatched_list          m_unmatched;

        /// \brief The command line interface specification
        interface_description&  m_interface;

      public:

        /// \brief The list of options found on the command line
        option_map const&       options;

        /// \brief The list of arguments that have not been matched with an option
        unmatched_list const&   unmatched;

      private:

        /// \brief Parses string as if it is an unparsed command line
        static std::vector< std::string > parse_command_line(char const* const arguments);

        /// \brief Converts C-style array with specified length to STL vector of strings
        template < typename C >
        static std::vector< std::string > convert(const int count, C const* const* const arguments);

        /// \brief Identifies all options and option arguments
        void collect(interface_description& d, std::vector< std::string > const& arguments);

        /// \brief Executes actions for default options
        void process_default_options(interface_description& d);

      public:

        /**
         * \brief Parses a string that represents a command on the command line, and executes default procedures for default options.
         * \param[in] d the interface description
         * \param[in] command_line the string that represents the unparsed command line
         **/
        inline command_line_parser(interface_description& d, char const* const command_line) :
                                         m_interface(d), options(m_options), unmatched(m_unmatched) {
          collect(d, parse_command_line(command_line));

          process_default_options(d);
        }

        /**
         * \brief Recognises options from an array that represents a pre-parsed command on the command line, and executes default procedures for default options.
         * \param[in] d the interface description
         * \param[in] c amount of arguments
         * \param[in] a C-style array with arguments
         **/
        template < typename C >
        command_line_parser(interface_description& d, const int c, C const* const* const a);

        /**
         * \brief Returns the argument (or the empty string) of the first option matching a name
         * \param[in] n the long name for the option
         * \pre 0 < options.count(n)
         **/
        std::string const& option_argument(std::string const& n) {
          assert(0 < options.count(n));

          return options.find(n)->second;
        }

        /**
         * \brief Returns the converted argument of the first option matching a name
         * \param[in] n the long name for the option
         * \pre 0 < options.count(n) and !options.find(n)->second.empty()
         * \throw std::runtime_exception containing a message that the argument cannot be converted to the specified type
         **/
        template < typename T >
        T option_argument_as(std::string const& n) {
          T result;

          std::istringstream in(option_argument(n));
           
          in >> result;

          if (in.fail()) {
            const char short_option(m_interface.long_to_short(n));

            m_interface.throw_exception("argument `" + option_argument(n) + "' to option --" + n +
                ((short_option == '\0') ? " " : " or -" + std::string(1, short_option)) + " is invalid!");
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
    class interface_description::option_descriptor {
      friend class command_line_parser;
      friend class interface_description;

      private:

        /// Long representation for the option
        std::string                       m_long;

        /// Short representation for the option or 0
        const char                        m_short;

        /// Description for the option
        std::string                       m_description;

        /// Option argument
        std::auto_ptr < basic_argument >  m_argument;

      protected:

        /// Prints the option specification to stream
        void print(std::ostream& s, const size_t w = 27) const;

      public:

        /**
         * \brief Constructor
         * \param[in] d description of the option
         * \param[in] l the long option representation of the option
         * \param[in] s an optional single-character short representation of the option
         * \pre l should be a non-empty string that only contain characters from [0-9a-Z] or `-'
         **/
        option_descriptor(std::string const& l, std::string const& d, const char s) :
                    m_long(l), m_short(s), m_description(d) {

          assert(!l.empty());
          assert(l.find_first_not_of("_-0123456789abcdefghijklmnopqrstuvwxyz") == std::string::npos);
        }

        /// copy constructor
        option_descriptor(option_descriptor const& o) : m_long(o.m_long), m_short(o.m_short), m_description(o.m_description) {
          m_argument.reset(const_cast< std::auto_ptr< basic_argument >& > (o.m_argument).release());
        }

        /**
         * \brief Sets option argument
         * \param[in] a an optional argument specifier object
         **/
        template < typename T >
        void set_argument(optional_argument< T > const& a) {
          m_argument.reset(new optional_argument< T >(a));
        }

        /**
         * \brief Sets option argument
         * \param[in] a an optional argument specifier object
         **/
        template < typename T >
        void set_argument(mandatory_argument< T > const& a) {
          m_argument.reset(new mandatory_argument< T >(a));
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
          throw std::runtime_error("Fatal error: default argument requested for mandatory option! (this is a bug)\n");
        }

        /// whether the argument is optional or not
        inline bool is_optional() const {
          return false;
        }
    };
    /// \endcond

    template < typename T >
    interface_description& interface_description::add_option(std::string const& l, T const& a, std::string const& d, char const s) {
      add_option(l, d, s);

      m_options.find(l)->second.set_argument(a);

      return *this;
    };

    /**
     * \param[in] d the interface description
     **/
    inline void command_line_parser::process_default_options(interface_description& d) {
      if (m_options.count("help")) {
        d.print(std::cout);

        exit(0);
      }
      else if (m_options.count("version")) {
        d.print_copy_message(std::cout);

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
      else { // check validity of argument to rewriter option
        std::string rewriter(m_options.find("rewriter")->second);
#if defined(__LIBREWRITE_H)
        if (RewriteStrategyFromString(rewriter.c_str()) == GS_REWR_INVALID) {
          d.throw_exception("argument `" + rewriter + "' to -r or --rewriter option is invalid");
        }
#endif
      }

#if defined(__MCRL2_MESSAGING_H__)
      if (m_options.count("quiet")) {
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
  }
}
