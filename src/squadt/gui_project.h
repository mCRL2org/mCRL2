#ifndef SQUADT_PROJECT_H
#define SQUADT_PROJECT_H

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/string.h>
#include <wx/splitter.h>

namespace squadt {

  class project_manager;

  namespace GUI {

    class squadt_main;

    /**
     * \brief Represents the main view of a project
     **/
    class project : wxSplitterWindow {
      friend class squadt::GUI::main;

      private:

        /** \brief The location (incomplete path) to project directory the project name is the basename */
        boost::shared_ptr < project_manager > manager;

      private:

        /** \brief Initially places the widgets */
        void build();

      public:

        /** \brief Constructor */
        project(wxWindow* p, const boost::filesystem::path&);

        /** \brief Constructor, with project description */
        project(wxWindow* p, const boost::filesystem::path&, const std::string&);

        /** \brief Returns the name of the project */
        wxString get_name() const;
    };
  }
}

#endif

