#include <ostream>

#include "boost/filesystem/path.hpp"

#include "build_system.h"

namespace squadt {
  class tool_manager;
  class executor;
  class type_registry;

  class write_preferences_visitor_impl;

  /**
   * \brief Writes the current state of components to file
   **/
  class write_preferences_visitor {

    protected:

      std::auto_ptr < write_preferences_visitor_impl > impl;

    private:

      /** \brief Constructor */
      write_preferences_visitor(boost::filesystem::path const&);

    public:

      /** \brief Convenience function to read from file using the visitor */
      static void store(build_system&, boost::filesystem::path const&);
  };

  class read_preferences_visitor_impl;

  /**
   * \brief Reads preferences from file and updates the state of components accordingly
   **/
  class read_preferences_visitor {

    protected:
      
      std::auto_ptr < read_preferences_visitor_impl > impl;

    private:

      /** \brief Constructor */
      read_preferences_visitor(boost::filesystem::path const&);
      
    public:

      /** \brief Convenience function to read from file using the visitor */
      static void restore(build_system&, boost::filesystem::path const&);
  };
}
