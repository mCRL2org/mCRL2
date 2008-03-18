#include <algorithm>
#include <vector>
#include <map>
#include <iosfwd>
#include <sstream>
#include <stdexcept>
#include <memory>

// dummy necessary for compiling
#define MCRL2_REVISION "0"

#include <mcrl2/utilities/command_line_interface.h>

namespace mcrl2 {
  namespace utilities {

    /// \cond INTERNAL
    /**
     * \param[in,out] s the stream to use for output
     * \param[in] w the width of the first column
     **/
    void interface_description::option_descriptor::print(std::ostream& s, const size_t w) const {
      std::string options;

      if (m_short != '\0') {
        options = "  -" + std::string(1, m_short);
        
        if (m_argument.get() != 0) {
          options += (m_argument->is_optional()) ? "[" + m_argument->get_name() + "]" : m_argument->get_name();
        }

        options += ", ";
      }
      else {
        options = "      ";
      }

      options += "--" + m_long;

      if (m_argument.get() != 0) {
        options += ((m_argument->is_optional()) ? "[=" + m_argument->get_name() + "]" : "=" + m_argument->get_name());
      }

      if (w - 1 <= options.size()) {
        s << options << std::endl << std::string(w, ' ');
      }
      else {
        s << options << std::string(w - options.size(), ' ');
      }

      std::istringstream description(m_description);

      while (description.good()) {
        getline(description, options);

        s << options << std::endl;

        if (!description.eof()) {
           s << std::string(w, ' ');
        }
      }
    }
    /// \endcond

    /**
     * \param[in] name the name used to reference the argument
     * \param[in] default_value the default value
     **/
    interface_description::optional_argument< std::string > make_optional_argument(std::string const& name, std::string const& default_value) {
      return interface_description::optional_argument< std::string >(name, default_value);
    }

    /**
     * \param[in] name the name used to reference the argument
     **/
    interface_description::mandatory_argument< std::string > make_mandatory_argument(std::string const& name) {
      return interface_description::mandatory_argument< std::string >(name);
    }

    void interface_description::add_rewriting_options() {
      add_option(
        "rewriter",
        make_mandatory_argument("NAME"),
        "use rewrite strategy NAME:\n"
        " 'jitty' for jitty rewriting (default),\n"
        " 'jittyp' for jitty rewriting with prover,\n"
        " 'jittyc' for compiled jitty rewriting,\n"
        " 'inner' for innermost rewriting,\n"
        " 'innerp' for innermost rewriting with prover,\n"
        " 'innerc' for compiled innermost rewriting",
        'r'
      );
    }

    interface_description& interface_description::add_option(std::string const& l, std::string const& d, const char s) {
      assert(m_options.find(l) == m_options.end());

      if (s != '\0') {
        assert(m_short_to_long.find(s) == m_short_to_long.end());

        m_short_to_long[s] = l;
      }

      m_options.insert(std::make_pair(l, option_descriptor(l, d, s)));

      return *this;
    }

    /**
     * \param[in] m the body of the exception message
     **/
    void interface_description::throw_exception(std::string const& m) const {
      std::string actual_tool_name(path_to_toolname(m_name));

      throw std::runtime_error(m_name + ": " + m + "\nTry `" + m_name + " --help' for more information.");
    }

    void interface_description::print(std::ostream& s) const {
      s << "Usage: " << m_path << " " << m_usage << std::endl
        << "Options:" << std::endl;

      for (option_map::const_iterator i = m_options.begin(); i != m_options.end(); ++i) {
        i->second.print(s);
      }

      s << std::endl << "Report bugs at <http://www.mcrl2.org/issuetracker>." << std::endl;
    }

    /**
     * The interface description specifies the available options and their
     * possible arguments. This procedure traverses the command-line arguments
     * in a left-to-right fashion and assigns those arguments to options. The
     * output is a pair of a map and a vector: the option map and the unmatched
     * vector. The option map (a multimap) maps an options found as argument to
     * its argument or an empty string (in case of no argument). The unmatched
     * vector contains command line arguments that were not recognised as
     * option or argument to option.
     *
     * \param[in] d interface description
     * \param[in] arguments vector (as ordered list) with command line arguments
     **/
    void command_line_parser::collect(interface_description& d, std::vector< std::string > const& arguments) {
      for (std::vector< std::string >::const_iterator i = arguments.begin() + 1; i != arguments.end(); ++i) {
        std::string const& argument(*i);
        
        if (argument[0] == '-') {
          if (argument[1] == '-') {
            // Assume that the argument is a long option
            std::string option(argument, 2);

            if (option.find_first_of('=') < option.size()) {
              // remove argument to single out the long option
              option.resize(option.find_first_of('='));
            }

            if (d.m_options.find(option) == d.m_options.end()) {
              if (argument[2] == 'a' && argument[3] == 't' && argument[4] == '-') {
                // Assume it is an option to the ATerm library, so discard
              }
              else {
                d.throw_exception("Parse error: command line argument `--" + option + "' not recognised!");
              }
            }
            else {
              std::string const& long_option = (d.m_options.find(option))->first;
            
              if (argument.size() == option.size() + 2) { // no argument
                interface_description::option_descriptor& descriptor = (d.m_options.find(long_option))->second;

                if (descriptor.needs_argument()) {
                  d.throw_exception("Parse error: expected argument to option `--" + option + "'!");
                }
                else if (descriptor.m_argument.get() == 0) {
                  m_options.insert(std::make_pair(long_option, ""));
                }
                else {
                  m_options.insert(std::make_pair(long_option, descriptor.m_argument->get_default()));
                }
              }
              else {
                m_options.insert(std::make_pair(long_option, std::string(argument, option.size() + 3)));
              }
            }
          }
          else if (1 < argument.size()) {
            std::string option(1, argument[1]);

            // Assume that the argument is a short option
            if (d.m_short_to_long.find(argument[1]) == d.m_short_to_long.end()) {
              d.throw_exception("Parse error: command line argument `-" + option + "' not recognised!");
            }
            else {
              std::string const& long_option = d.m_options.find(d.m_short_to_long[argument[1]])->first;

              if (argument.size() == 2) { // no argument
                interface_description::option_descriptor& descriptor = (d.m_options.find(long_option))->second;

                if (descriptor.needs_argument()) {
                  d.throw_exception("Parse error: expected argument to option `-" + option + "'!");
                }
                else if (descriptor.m_argument.get() == 0) {
                  m_options.insert(std::make_pair(long_option, ""));
                }
                else {
                  m_options.insert(std::make_pair(long_option, descriptor.m_argument->get_default()));
                }
              }
              else {
                m_options.insert(std::make_pair(long_option, std::string(argument, 2)));
              }
            }
          }
        }
        else {
          m_unmatched.push_back(argument);
        }
      }
    }

    /**
     * Converts an array of C-style strings and a count to an STL vector of STL strings.
     *
     * \param[in] count the number of arguments
     * \param[in] arguments C-style array with arguments as C-style zero-terminated string
     **/
    std::vector< std::string > command_line_parser::convert(const int count, char const* const* const arguments) {
      std::vector< std::string > result;

      if (0 < count) {
        result.resize(count);

        std::vector< std::string >::reverse_iterator j = result.rbegin();

        for (char const* const* i = &arguments[count - 1]; i != &arguments[0]; --i) {
          *(j++) = std::string(*i);
        }
      }

      return result;
    }

    /**
     * Parses a string as if it were an unparsed command line and stores it as
     * an array of C-style strings and a count to an STL vector of STL strings.
     *
     * It parses the following grammar:
     *
     * \verbatim
     *  command     ::= name white-space* argument
     *  argument    ::= ( non-white-space | block ) +
     *  block       ::= "'" ( character ) * "'" | '"' ( character )* '"'
     *  white-space ::= ' ' | '\t'
     * \endverbatim
     *
     * Where `non-white-space' is a character that is neither space nor tab;
     * and `name' is a path to a file in the local filesystem and operating
     * system path syntax.
     *
     * The strings that represent 'name' and 'argument' are identified from
     * left-to-right and stored in the output vector.
     *
     * \note This method is includes specifically for on MS Windows systems. On
     * such systems the command used to start the program is not always
     * available in another form than the unparsed command line.
     *
     * \param[in] arguments C-style zero-terminated string
     **/
    std::vector< std::string > command_line_parser::parse_command_line(char const* const arguments) {
      std::vector< std::string > result;

      if (arguments != 0) {
        char const* current = arguments;
       
        while (*current != '\0') {
          // skip initial white space
          while (*current == '\0' || *current == ' ') {
            ++current;
          }
       
          if (*current != '\0') {
            char const* current_argument = current;
           
            do {
              if (*current == '\'') {
                do {
                  ++current;
                }
                while (*current != '\0' && *current != '\'');

                if (*current == '\'') {
                  ++current;
                }
              }
              else if (*current == '\"') {
                do {
                  ++current;
                }
                while (*current != '\0' && *current != '\"');

                if (*current == '\"') {
                  ++current;
                }
              }
              else {
                ++current;
              }
            }
            while (*current != '\0' && *current != ' ');

            result.push_back(std::string(current_argument, current - current_argument));
          }
        }
      }

      return result;
    }
  }
}
