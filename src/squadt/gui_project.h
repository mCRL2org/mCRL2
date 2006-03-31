#ifndef SQUADT_PROJECT_H
#define SQUADT_PROJECT_H

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>

#include <wx/wx.h>
#include <wx/panel.h>

namespace squadt {

  class project_manager;

  namespace GUI {

    class squadt_main;

    /**
     * \brief Represents the main view of a project
     **/
    class project : wxPanel {
      friend class squadt::GUI::main;

      private:

        /** \brief The location (incomplete path) to project directory the project name is the basename */
        boost::shared_ptr < project_manager > manager;

      public:

        /** \brief Constructor */
        project(boost::filesystem::path&);
    };
  }
}

#endif

