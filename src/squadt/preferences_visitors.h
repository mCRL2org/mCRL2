#include <ostream>

#include "boost/filesystem/path.hpp"

#include "build_system.h"

namespace squadt {
  class tool_manager;
  class executor;
  class type_registry;

  class preferences_write_visitor_impl;

  /**
   * \brief Writes the current state of components to file
   **/
  class preferences_write_visitor {

    protected:

      std::auto_ptr < preferences_write_visitor_impl > impl;

    private:

      /** \brief Constructor */
      preferences_write_visitor(boost::filesystem::path const&);

    public:

      /** \brief Convenience function to read from file using the visitor */
      static void store(build_system&, boost::filesystem::path const&);
  };

  class preferences_read_visitor_impl;

  /**
   * \brief Reads preferences from file and updates the state of components accordingly
   **/
  class preferences_read_visitor {

    protected:
      
      std::auto_ptr < preferences_read_visitor_impl > impl;

    private:

      /** \brief Constructor */
      preferences_read_visitor(boost::filesystem::path const&);
      
    public:

      /** \brief Convenience function to read from file using the visitor */
      static void restore(build_system&, boost::filesystem::path const&);
  };
}
