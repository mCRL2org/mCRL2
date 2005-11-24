///////////////////////////////////////////////////////////////////////////////
/// \file filter_program.h
/// Contains a simple template for filter programs.

#ifndef LPE_FILTER_PROGRAM_H
#define LPE_FILTER_PROGRAM_H

#include <string>
#include <cstdio>
#include <boost/algorithm/string.hpp>
#include "specification.h"

namespace lpe {

  ///////////////////////////////////////////////////////////////////////////////
  // filter_program
  /// \brief represents a command line program that acts as a filter
  ///
  class filter_program
  {
   protected:
      std::string m_name;             // the name of the program
      std::string m_version;          // the version of the program
      std::string m_input_extension;  // the extension of the input file
      std::string m_output_extension; // the extension of the output file
      std::string m_input_file;       // if empty string then input comes from stdin
      std::string m_output_file;      // if empty string then output goes to stdout
      bool        m_quiet;            // "do not display warning messages"
      bool        m_verbose;          // "turn on the display of short intermediate messages"
      bool        m_debug;            // "turn on the display of detailed intermediate messages"

   public:
      filter_program(const std::string& name,
                     const std::string& version,
                     const std::string& input_extension,
                     const std::string& output_extension,
                     const std::string& input_file,
                     const std::string& output_file,
                     bool quiet,
                     bool verbose,
                     bool debug
                    )
        : m_name(name),
          m_version(version),
          m_input_extension(input_extension),
          m_output_extension(output_extension),
          m_input_file(input_file),
          m_output_file(output_file),
          m_quiet(quiet),
          m_verbose(verbose),
          m_debug(debug)
      {}
  
      virtual ~filter_program()
      {}

      /// Returns the name of the program.
      ///
      const std::string& name() const
      {
        return m_name;
      }
  
      /// Returns the version of the program.
      ///
      const std::string& version() const
      {
        return m_version;
      }
  
      /// Returns the input file of the program (empty if the input should be read from stdin).
      ///
      std::string input_file() const
      {
        if (m_input_file.empty() || boost::ends_with(m_input_file, m_input_extension))
        {
          return m_input_file;
        }
        else
        {
          return m_input_file + m_input_extension;
        }
      }
  
      /// Returns the output file of the program (empty if the output should be written to stdout).
      ///
      std::string output_file() const
      {
        if (m_output_file.empty() || boost::ends_with(m_output_file, m_output_extension))
        {
          return m_output_file;
        }
        else
        {
          return m_output_file + m_output_extension;
        }
      }

      /// Returns true if no warnings should be displayed.
      ///
      bool quiet() const
      {
        return m_quiet;
      }

      /// Returns true if short intermediate messages should be displayed.
      ///
      bool verbose() const
      {
        return m_verbose;
      }

      /// Returns true if detailed intermediate messages should be displayed.
      ///
      bool debug() const
      {
        return m_debug;
      }

      // Reads a specification from file or from stdin.
      //  
      specification read_specification()
      {
        specification result;
        if (m_input_file.empty())
        {
          result = specification(ATreadFromFile(stdin));
        }
        else
        {
          result.load(m_input_file);
        }
        return result;
      }
    
      // Writes a specification to file or to stdout. Returns true if succeeded.
      //
      bool write_specification(specification spec)
      {
        bool result;

        if (m_output_file.empty())
        {
          result = ATwriteToBinaryFile(aterm(spec), stdout);
        }
        else
        {
          result = spec.save(m_output_file);
        }
        return result;
      }

      /// Executes the program.
      ///
      virtual void run() = 0;     
  };

} // namespace lpe

#endif // LPE_FILTER_PROGRAM_H
