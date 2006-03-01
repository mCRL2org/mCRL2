#include <cstring>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <sip/detail/exception.h>

namespace sip {

  namespace cli {

    /**
     * This class provides functionality to extract protocol specific arguments
     * from the list of command line arguments that a tool receives.
     **/
    class command_line_argument_extractor {
      public:

        /** \brief Abstract base class for container types for parsed information about schemes */
        class scheme : public boost::noncopyable {
          public:
            virtual ~scheme() = 0;
        };

        /** \brief Derived class for the traditional scheme */
        class traditional_scheme : public scheme {
          public:
            traditional_scheme();
        };

        /** \brief Derived class for the socket scheme */
        class socket_scheme : public scheme {
          public:
            socket_scheme();
        };

        /** \brief Convenience type for hiding boost shared pointers */
        typedef boost::shared_ptr < scheme > scheme_ptr;

      private:

        /** The list of known of options */
        static const char*  known_options[];

        /** The list of known of schemes */
        static const char*  known_schemes[];

        /** The number of options in known_options[] */
        static const size_t known_option_number;

        /** The number of schemes in known_schemes[] */
        static const size_t known_scheme_number;

        /** \brief The number of the last matched known_option or known_scheme. */
        size_t last_matched;

        /** \brief Parses a minimum prefix of argument to search for a known option */
        inline char* parse_option(char* const);

        /** \brief Parses a minimum prefix of argument to search for a known scheme */
        inline char* parse_scheme(char* const) throw ();

        /** \brief the scheme that was last parsed succesfully */
        scheme_ptr   selected_scheme;

      public:

        /** \brief constructor that performs extraction */
        inline command_line_argument_extractor(int&, char** const) throw ();

        /** \brief get the arguments for the selected scheme */
        inline scheme_ptr get_scheme() const;
    };

    const size_t command_line_argument_extractor::known_option_number = 1;
    const size_t command_line_argument_extractor::known_scheme_number = 2;

    const char*  command_line_argument_extractor::known_options[known_option_number] = { "--si-connect" };
    const char*  command_line_argument_extractor::known_schemes[known_scheme_number] = { "traditional", "socket" };

    /** Constructor */
    command_line_argument_extractor::scheme::~scheme() {
    }

    /** Constructor */
    command_line_argument_extractor::traditional_scheme::traditional_scheme() {
    }

    /** Constructor */
    command_line_argument_extractor::socket_scheme::socket_scheme() {
    }

    /**
     * \returns The arguments for a selected scheme (e.g. hostname and port for the socket scheme).
     **/
    inline command_line_argument_extractor::scheme_ptr command_line_argument_extractor::get_scheme() const {
      return (selected_scheme);
    }

    /**
     * \returns A pointer within argv to the point upto which the option was matched.
     **/
    inline char* command_line_argument_extractor::parse_option(char* const option) {
      unsigned int i = 0;

      while (i < known_option_number) {
        size_t l = strlen(known_options[i]);

        if (strncmp(option, known_options[i], l) == 0) {
          last_matched = i;

          return (option + l);
        }

        ++i;
      }

      return (option);
    }

    /**
     * \returns A pointer within argv to the point upto which the option was matched.
     **/
    inline char* command_line_argument_extractor::parse_scheme(char* const option) throw () {
      unsigned int i = 0;

      while (i < known_scheme_number) {
        size_t l = strlen(known_schemes[i]);

        if (strncmp(option, known_schemes[i], l) == 0) {
          last_matched = i;

          char* s = option + l;

          if (last_matched != 0 && strncmp(s, "://", 3) != 0) {
            throw (exception(exception_identifier::cli_parse_error_expected, "://", s));
          }

          s += 3;
         
          switch (last_matched) {
            case 1: /* Socket scheme (host:port) */
              selected_scheme = scheme_ptr(new socket_scheme);
              break;
            default: /* Traditional scheme */
              selected_scheme = scheme_ptr(new traditional_scheme);
              break;
          }

          return (s);
        }

        ++i;
      }

      return (option);
    }

    /**
     * The following connection options are recognised and extracted from the command line arguments:
     *
     *  - --si-connect=\<scheme\>, where \<scheme\> is one of
     *    - socket://\<host\>:\<port\> (for a socket connection)
     *    - traditional:// (for standard input/output communication)
     *
     * @param argc the number of command line arguments
     * @param argv a pointer to the list of command line arguments
     * \attention the specific command line options are removed, so and argc and argv are modified
     * \return whether options were found and whether a connection is being opened with a controller
     **/
    inline command_line_argument_extractor::command_line_argument_extractor(int& argc, char** const argv) throw () {
      size_t i = 0;
      size_t j = 0;

      while (static_cast < int> (i) < argc) {
        char* s = parse_option(argv[i]);

        if (s != argv[i]) {
          /* Ignore anything before the `=' character */
          s = strchr(s, '=');

          if (s == 0) {
            throw (exception(exception_identifier::cli_parse_error_expected, '=', s));
          }

          ++s;

          /* Now s should point to a known scheme identifier */
          char* t = parse_scheme(s);

          if (s == t) {
            throw (exception(exception_identifier::cli_parse_error_expected, "valid scheme", s));
          }
        }
        else {
          if (j < i) {
            argv[j] = s;
          }

          ++j;
        }
     
        ++i;
      }

      argc = j;
    }
  }
}

