#ifndef COMMAND_H
#define COMMAND_H

#include <deque>

#include <boost/shared_array.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/filesystem/operations.hpp>

namespace squadt {
  namespace execution {

    /**
     * \brief Captures the concept of a command
     *
     * The class serves as a helper for platform independent process execution.
     * Sadly the most portable standard available popen() and pclose() give not
     * enough control over a process. We need the ability to kill it. And on
     * the other hand system() is not thread safe, so it is unusable.
     **/
    class command {
      friend class process_impl;
  
      private:
  
        /** \brief Path to the program that is to be executed */
        std::string                 executable;

        /** \brief Path to the working directory of the tool */
        std::string                 working_directory;
  
        /** \brief The arguments to the command */
        std::deque < std::string >  arguments;

      private:
       
        /** \brief Get the command line arguments as an array */
        boost::shared_array < char const* > get_array(bool = true) const;

      public:

        /** \brief A sequence of arguments that are part of a command */
        typedef boost::iterator_range < std::deque < std::string >::const_iterator > const_argument_sequence;
  
        /** \brief A sequence of arguments that are part of a command */
        typedef boost::iterator_range < std::deque < std::string >::iterator >       argument_sequence;
  
      public:

        /** \brief Parse command as if passed from the command line */
        static std::auto_ptr < command > from_command_line(std::string const&);

      public:
  
        /** \brief Constructor */
        command(const std::string&);

        /** \brief Constructor with working directory */
        command(const std::string&, std::string const&);

        /** \brief Copy constructor */
        command(command const&);

        /** \brief Gets the working directory */
        std::string get_working_directory() const;
  
        /** \brief Sets the working directory */
        void set_working_directory(std::string const&);
  
        /** \brief Adds an argument */
        void prepend_argument(std::string const&);

        /** \brief Adds an argument */
        void append_argument(std::string const&);

        /** \brief Get arguments as a traditional C string */
        std::string as_string(bool = true) const;

        /** \brief Get arguments as an array of constant C strings */
        const_argument_sequence get_arguments() const;

        /** \brief Get arguments as an array of constant C strings */
        argument_sequence get_arguments();

        /** \brief Returns the basename of the executable */
        void set_executable(std::string const&);

        /** \brief Returns the basename of the executable */
        std::string get_executable() const;
    };
  
    /**
     * @param[in] e a complete path to an executable of the program that is to be executed
     **/
    inline command::command(std::string const& e) : executable(e),
                working_directory(boost::filesystem::current_path().native_file_string()) {
    }

    /**
     * @param[in] e a complete path to an executable of the program that is to be executed
     * @param[in] w path to the working directory for this tool
     **/
    inline command::command(std::string const& e, std::string const& w) : executable(e), working_directory(w) {
    }

    /**
     * @param[in] o the object to copy
     **/
    inline command::command(command const& o) : executable(o.executable),
                working_directory(o.working_directory),
                arguments(o.arguments) {
    }

    inline std::string command::get_working_directory() const {
      return (working_directory);
    }

    inline void command::set_working_directory(std::string const& w) {
      working_directory = w;
    }

    /**
     * @param a a single argument
     **/
    inline void command::prepend_argument(std::string const& a) {
      arguments.push_front(a);
    }
  
    /**
     * @param a a single argument
     **/
    inline void command::append_argument(std::string const& a) {
      arguments.push_back(a);
    }
  
    /**
     * @param e name of an executable or its full path
     **/
    inline void command::set_executable(std::string const& e) {
      executable = e;
    }

    inline std::string command::get_executable() const {
      return (boost::filesystem::path(executable).leaf());
    }
  }
}

#endif
