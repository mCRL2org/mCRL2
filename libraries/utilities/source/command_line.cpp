#include <algorithm>
#include <vector>
#include <map>
#include <iosfwd>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <locale>

#include "boost/algorithm/string.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

// dummy necessary for compiling
#define MCRL2_REVISION "0"
#define __COMMAND_LINE_INTERFACE__

#include <mcrl2/utilities/command_line_interface.h>

namespace mcrl2 {
  namespace utilities {

    /// \cond INTERNAL
    /**
     * Inserts newline characters while reading the input from left to right.
     * When a newline is inserted the next read newline is discarded.
     *  - if a newline is read the previous line was wrapped the first 
     *
     * To force a new-line at some position add two successive newline characters.
     *
     * \param[in] input the input string
     * \param[in] indent used as fixed indentation after end of line
     * \param[in] width the maximum width of the text
     * \pre 0 < width and no word can be longer than width
     * \return string with newlines inserted such that the number of characters
     * between any two consecutive newlines is smaller than width
     **/
    static std::string word_wrap(std::string const& input, const size_t width, std::string const& indent = "") {
      std::ostringstream out;

      std::string            variable_indent = input.substr(0, input.find_first_not_of(" \t"));
      std::string::size_type space_left      = width;

      std::string::const_iterator i = input.begin();
      std::string::const_iterator word_start = i;

      while (i != input.end()) {
        if (space_left - (i - word_start) < 1) { // line too long

          out << std::endl << indent << variable_indent << std::string(word_start, ++i);

          space_left = width - (i - word_start) - variable_indent.size();
          word_start = i;
        }
        else if (*i == ' ' || *i == '\t') {
          out << std::string(word_start, ++i);

          space_left -= i - word_start;
          word_start  = i;
        }
        else if (*i == '\n') {
          if (word_start != i) {
            out << std::string(word_start, i);
          }

          ++i;

          // store new indent
          std::string::size_type start = i - input.begin();
          std::string::size_type end   = input.find_first_not_of(" \t", start);

          if (end != std::string::npos) {
            i              += end - start;
            variable_indent = input.substr(start, end - start);
          }
          else {
            variable_indent.clear();
          }

          // copy word and newline
          out << std::endl << indent << variable_indent;

          space_left = width - variable_indent.size();
          word_start = i;
        }
        else {
          ++i;
        }
      }

      out << std::string(word_start, input.end());

      return out.str();
    }

    /**
     * \param[in] left_width the width of the left column
     * \param[in] right_width the width of the right column
     * \return formatted string that represents the option description
     **/
    std::string interface_description::option_descriptor::textual_description(
                        const size_t left_width, const size_t right_width) const {
      std::ostringstream s;
      std::string        options;

      if (m_short != '\0') {
        options = "  -" + std::string(1, m_short);
        
        if (m_argument.get() != 0) {
          options += (m_argument->is_optional()) ?
                "[" + m_argument->get_name() + "]" : m_argument->get_name();
        }

        options += ", ";
      }
      else {
        options = "      ";
      }

      options += "--" + m_long;

      if (m_argument.get() != 0) {
        options += ((m_argument->is_optional()) ?
                "[=" + m_argument->get_name() + "]" : "=" + m_argument->get_name());
      }

      if (options.size() < left_width) {
        s << options << std::string(left_width - options.size(), ' ');
      }
      else {
        s << options << std::endl << std::string(left_width, ' ');
      }

      s << word_wrap(m_description, right_width, std::string(left_width, ' ')) << std::endl;

      return s.str();
    }

    std::string interface_description::option_descriptor::man_page_description() const {
      std::ostringstream s;
      
      s << ".TP" << std::endl;

      if (m_short != '\0') {
        s << "\\fB\\-" << std::string(1, m_short) << "\\fR";

        if (m_argument.get() != 0) {
          if (m_argument->is_optional()) {
            s << "[\\fI" << m_argument->get_name() << "\\fR]";
          }
          else {
            s << "\\fI" << m_argument->get_name() << "\\fR";
          }
        }

        s << ", ";
      }

      s << "\\fB\\-\\-" << m_long << "\\fR";

      if (m_argument.get() != 0) {
        s << ((m_argument->is_optional()) ?
              "[=\\fI" + m_argument->get_name() + "\\fR]" :
              "=\\fI" + m_argument->get_name() + "\\fR");
      }

      s << std::endl
        << boost::replace_all_copy(word_wrap(m_description, 80), "'", "\\&'") << std::endl;

      return s.str();
    }
    /// \endcond

    /**
     * \param[in] name the name used to reference the argument
     * \param[in] default_value the default value
     **/
    interface_description::optional_argument< std::string >
       make_optional_argument(std::string const& name, std::string const& default_value) {

      return interface_description::optional_argument< std::string >(name, default_value);
    }

    /**
     * \param[in] name the name used to reference the argument
     **/
    interface_description::mandatory_argument< std::string >
                                make_mandatory_argument(std::string const& name) {
      return interface_description::mandatory_argument< std::string >(name);
    }

    interface_description::interface_description(std::string const& path,
          std::string const& name, std::string const& authors, std::string const& usage,
          std::string const& known_issues) :
                                    m_path(path), m_name(name), m_authors(authors),
                                    m_known_issues(known_issues) {

      std::istringstream usage_and_description(usage);

      getline(usage_and_description, m_usage);

      std::ostringstream description_only;

      description_only << usage_and_description.rdbuf();

      m_description = description_only.str();

      // Add mandatory options
      add_hidden_option("help", "display help information", 'h');
      add_hidden_option("version", "display version information");
      add_hidden_option("quiet", "do not display warning messages", 'q');
      add_hidden_option("verbose", "display short intermediate messages", 'v');
      add_hidden_option("debug", "display detailed intermediate messages", 'd');
    }

    std::string interface_description::copyright_message() const {
      return "Copyright (c) " + std::string(MCRL2_COPYRIGHT_YEAR) +
                                        " Eindhoven University of Technology.\n"
        "This is free software.  You may redistribute copies of it under the\n"
        "terms of the Boost Software License <http://www.boost.org/LICENSE_1_0.txt>.\n"
        "There is NO WARRANTY, to the extent permitted by law.\n";
    }

    inline void interface_description::add_hidden_option(
                std::string const& long_identifier,
                basic_argument const& argument_specification,
                std::string const& description, char const short_identifier) {

      add_option(long_identifier, argument_specification, description, short_identifier);

      m_options.find(long_identifier)->second.m_show = false;
    }

    inline void interface_description::add_hidden_option(
                std::string const& long_identifier,
                std::string const& description,
                char const short_identifier) {

      add_option(long_identifier, description, short_identifier);

      m_options.find(long_identifier)->second.m_show = false;
    }

    void interface_description::add_rewriting_options() {
      add_option(
        "rewriter", make_mandatory_argument("NAME"),
        "use rewrite strategy NAME:\n"
        "'jitty' for jitty rewriting (default),\n"
        "'jittyp' for jitty rewriting with prover,\n"
        "'jittyc' for compiled jitty rewriting,\n"
        "'inner' for innermost rewriting,\n"
        "'innerp' for innermost rewriting with prover, or\n"
        "'innerc' for compiled innermost rewriting",
        'r'
      );
    }

    void interface_description::add_prover_options() {
      add_option("smt-solver", make_mandatory_argument("SOLVER"),
        "use SOLVER to remove inconsistent paths from the internally used BDDs:\n"
#if defined(HAVE_CVC)
        "'ario' for the SMT solver Ario, or\n"
        "'cvc' for the SMT solver CVC3;\n"
        "'cvc-fast' for the fast implementation of the\n"
#else
        "'ario' for the SMT solver Ario, or\n"
        "'cvc' for the SMT solver CVC3;\n"
#endif
        "by default, no path elimination is applied", 'z');
    }

    interface_description& interface_description::add_option(std::string const& l, std::string const& d, const char s) {
      if (m_options.find(l) != m_options.end()) {
        throw std::logic_error("Duplicate long option (--" + l + "); this is a serious program error!");
      }

      if (s != '\0') {
        if (m_short_to_long.find(s) != m_short_to_long.end()) {
          throw std::logic_error("Duplicate short option (-" + std::string(1, s) + "); this is a serious program error!");
        }

        m_short_to_long[s] = l;
      }

      m_options.insert(std::make_pair(l, option_descriptor(l, d, s)));

      return *this;
    }

    interface_description& interface_description::add_option(std::string const& l, basic_argument const& a, std::string const& d, char const s) {
      add_option(l, d, s);

      m_options.find(l)->second.set_argument(a.clone());

      return *this;
    };

    std::string interface_description::textual_description() const {
      std::ostringstream s;

      s << "Usage: " << m_path << " " << m_usage << std::endl
        << word_wrap(m_description, 80) << std::endl << std::endl;

      if (5 < m_options.size()) { // tool-specific options
        s << "Options:" << std::endl;

        for (option_map::const_iterator i = m_options.begin(); i != m_options.end(); ++i) {
          option_descriptor const& option(i->second);

          if (option.m_show) {
            s << option.textual_description(27, 53);
          }
        }

        s << std::endl;
      }

      s << "Default options:" << std::endl
        << m_options.find("help")->second.textual_description(27, 53)
        << m_options.find("quiet")->second.textual_description(27, 53)
        << m_options.find("verbose")->second.textual_description(27, 53)
        << m_options.find("debug")->second.textual_description(27, 53)
        << m_options.find("version")->second.textual_description(27, 53)
        << std::endl;

      if (!m_known_issues.empty()) {
        s << "Known Issues:" << std::endl
          << word_wrap(m_known_issues, 80) << std::endl << std::endl;
      }

      s << "Report bugs at <http://www.mcrl2.org/issuetracker>." << std::endl
        << std::endl
        << "See also the manual at <http://www.mcrl2.org/wiki/index.php/" << m_name << ">.\n";

      return s.str();
    }

    inline std::string mark_name_in_usage(std::string const& usage) {
      std::string result;
      bool        name_character = false;

      result.reserve(2 * usage.size());

      for (std::string::const_iterator i = usage.begin(); i != usage.end(); ++i) {
        if (*i == '[') {
          name_character = true;
          result.append("[\\fI");
        }
        else if ((*i == ' ') || (*i == ']')) {
          if (name_character) {
            result.append("\\fR");

            name_character = false;
          }
          result.append(1, *i);
        }
        else {
          result.append(1, *i);
        }
      }

      return result;
    }

    std::string interface_description::man_page(std::string const& revision) const {
      std::ostringstream s;

      s.imbue(std::locale(s.getloc(), new boost::gregorian::date_facet("%B %Y")));

      s << ".\" " << "manual for " << m_name <<
           " which part of the mCRL2 toolset revision " << revision << std::endl;
      s << ".TH " << boost::to_upper_copy(m_name) << " \"1\" \""
                  << boost::gregorian::day_clock::local_day() << "\" \"" 
                  << m_name << " " << std::string(MCRL2_VERSION)
        << "\" \"User Commands\"" << std::endl;

      s << ".SH NAME" << std::endl 
        << m_name << " \\- manual page for " << m_name << " "
                                         << std::string(MCRL2_VERSION) << std::endl;

      s << ".SH SYNOPSIS" << std::endl 
        << ".B " << m_name << std::endl
        << mark_name_in_usage(m_usage) << std::endl;

      s << ".SH DESCRIPTION" << std::endl
        << word_wrap(m_description, 80) << std::endl;
      
      if (0 < m_options.size()) {
        s << ".SH OPTIONS" << std::endl;

        for (option_map::const_iterator i = m_options.begin(); i != m_options.end(); ++i) {
          option_descriptor const& option(i->second);

          if (option.m_show) {
            s << option.man_page_description();
          }
        }
      }

      s << ".SS \"DEFAULT OPTIONS\"" << std::endl
        << m_options.find("help")->second.man_page_description()
        << m_options.find("quiet")->second.man_page_description()
        << m_options.find("verbose")->second.man_page_description()
        << m_options.find("debug")->second.man_page_description()
        << m_options.find("version")->second.man_page_description()
        << std::endl;

      if (!m_known_issues.empty()) {
        s << ".SH \"KNOWN ISSUES\"" << std::endl
          << word_wrap(m_known_issues, 80) << std::endl;
      }

      s << ".SH AUTHOR" << std::endl
        << "Written by " << m_authors << "." << std::endl;
      s << ".SH \"REPORTING BUGS\"" << std::endl 
        << "Report bugs at <http://www.mcrl2.org/issuetracker>." << std::endl;
      s << ".SH COPYRIGHT" << std::endl
        << "Copyright \\(co " + std::string(MCRL2_COPYRIGHT_YEAR) +
                                        " Eindhoven University of Technology.\n"
        << ".br" << std::endl
        << "This is free software.  You may redistribute copies of it under the\n"
           "terms of the Boost Software License <http://www.boost.org/LICENSE_1_0.txt>.\n"
           "There is NO WARRANTY, to the extent permitted by law.\n";
      s << ".SH \"SEE ALSO\"" << std::endl
        << "See also the manual at <http://www.mcrl2.org/wiki/index.php/" << m_name << ">.\n";

      return s.str();
    }

    /**
     * The interface description specifies the available options and their
     * possible arguments. This procedure traverses the command-line arguments
     * in a left-to-right fashion and assigns those arguments to options. The
     * output is a pair of a map and a vector: the option map and the arguments
     * vector. The option map (a multimap) maps an options found as argument to
     * its argument or an empty string (in case of no argument). The arguments
     * vector contains command line arguments that were not recognised as
     * option or argument to option.
     *
     * \param[in] d interface description
     * \param[in] arguments vector (as ordered list) with command line arguments
     **/
    void command_line_parser::collect(interface_description& d, std::vector< std::string > const& arguments) {
      if (0 < arguments.size()) {
        for (std::vector< std::string >::const_iterator i = arguments.begin() + 1; i != arguments.end(); ++i) {
          if (!i->empty()) {
            std::string const& argument(*i);
          
            if (argument[0] == '-' && 1 < argument.size()) {
              if (argument[1] == '-') {
                // Assume that the argument is a long option
                std::string option(argument, 2);
           
                if (option.find_first_of('=') < option.size()) {
                  // remove argument to single out the long option
                  option.resize(option.find_first_of('='));
                }
           
                if (d.m_options.find(option) == d.m_options.end()) {
                  if (argument == "--generate-man-page") {
                    // Special option
                    m_options.insert(std::make_pair(argument.substr(2), ""));
                  }
                  else {
                    error("command line argument `--" + option + "' not recognised");
                  }
                }
                else {
                  std::string const& long_option = (d.m_options.find(option))->first;

                  interface_description::option_descriptor const& descriptor =
                                                (d.m_options.find(long_option))->second;

                  if (argument.size() == option.size() + 2) { // no argument
           
                    if (descriptor.needs_argument()) {
                      error("expected argument to option `--" + option + "'!");
                    }
                    else if (descriptor.m_argument.get() == 0) {
                      m_options.insert(std::make_pair(long_option, ""));
                    }
                    else {
                      m_options.insert(std::make_pair(long_option,
                                        descriptor.m_argument->get_default()));
                    }
                  }
                  else {
                    std::string option_argument(argument, option.size() + 3);

                    if (descriptor.m_argument.get() == 0) {
                      error("did not expect argument to option `--" + option + "'");
                    }
                    else if (!descriptor.m_argument->validate(option_argument)) {
                      error("argument to option `--" + option + "' is invalid");
                    }

                    m_options.insert(std::make_pair(long_option, option_argument));
                  }
                }
              }
              else {
                if (argument[1] == 'a' && argument[2] == 't' && argument[3] == '-') {
                  // Assume it is an option to the ATerm library, so discard
                }

                for (std::string::size_type i = 1; i < argument.size(); ++i) {
                  std::string option(1, argument[i]);
           
                  // Assume that the argument is a short option
                  if (d.m_short_to_long.find(argument[i]) == d.m_short_to_long.end()) {
                    error("command line argument `-" + option + "' not recognised");
                  }
                  else {
                    std::string const& long_option =
                        d.m_options.find(d.m_short_to_long[argument[i]])->first;

                    interface_description::option_descriptor const& descriptor =
                                                (d.m_options.find(long_option))->second;
                 
                    if (argument.size() - i == 1) { // the last option without argument
                      if (descriptor.needs_argument()) {
                        error("expected argument to option `-" + option + "'");
                      }
                      else if (descriptor.m_argument.get() == 0) {
                        m_options.insert(std::make_pair(long_option, ""));
                      }
                      else {
                        m_options.insert(std::make_pair(long_option, descriptor.m_argument->get_default()));
                      }
                    }
                    else { // intermediate option or option with argument
                      if (d.m_options.find(long_option)->second.accepts_argument()) {
                        std::string option_argument(argument, i + 1);

                        if (descriptor.m_argument.get() == 0) {
                          error("did not expect argument to option `-" + option + "'");
                        }
                        else if (!descriptor.m_argument->validate(option_argument)) {
                          error("argument to option `-" + option + "' is invalid");
                        }

                        // must be the last option, so take the remainder as option argument
                        m_options.insert(std::make_pair(long_option, option_argument));
           
                        break;
                      }
                      else {
                        m_options.insert(std::make_pair(long_option, ""));
                      }
                    }
                  }
                }
              }
            }
            else {
              m_arguments.push_back(argument);
            }
          }
        }
      }
    }

    /// \cond INTERNAL
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

#ifndef __CYGWIN__ // wstring is not available for cygwin
    /**
     * Converts an array of C-style strings and a count to an STL vector of STL strings.
     *
     * \param[in] count the number of arguments
     * \param[in] arguments C-style array with arguments as C-style zero-terminated string
     * \pre arguments uses UTF-8 encoding
     **/
    std::vector< std::string > command_line_parser::convert(const int count, wchar_t const* const* const arguments) {
      std::vector< std::string > result;

      if (0 < count) {
        std::ostringstream converter;

        result.resize(count);

        std::vector< std::string >::reverse_iterator j = result.rbegin();

        for (wchar_t const* const* i = &arguments[count - 1]; i != &arguments[0]; --i) {
          std::wstring argument(*i);
          
          *(j++) = std::string(argument.begin(), argument.end());
        }
      }

      return result;
    }
    /// \endcond
#endif // __CYGWIN__

    /**
     * \param[in] message the body of the exception message
     **/
    void command_line_parser::error(std::string const& message) const {
      throw std::runtime_error(m_interface.m_name + ": " + message
                + "\nTry `" + m_interface.m_name + " --help' for more information.");
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
