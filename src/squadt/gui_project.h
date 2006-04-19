#ifndef SQUADT_PROJECT_H
#define SQUADT_PROJECT_H

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/string.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>

#include "processor.h"

namespace squadt {

  class project_manager;

  namespace GUI {

    class main;

    /**
     * \brief Represents the main view of a project
     **/
    class project : wxSplitterWindow {
      friend class squadt::GUI::main;

      private:

        /**
         * \brief Container for data associated with a single node in a wxTreeCtrl
         *
         * In addition objects of this type also receive updates of state
         * changes of the processor. These state changes will be visualised
         * through this object.
         **/
        class node_data : public processor, public wxTreeItemData {
          private:

            /** \brief The associated project */
            project&       parent;

          private:

            /* \brief Updates the GUI to reflect the change in state */
            inline void change_state(processor::output_status);

          public:

            /* \brief Constructor */
            node_data(project&);
        };

      private:

        /** \brief The view on processors and their interdependencies */
        wxTreeCtrl*                           processor_view;

        /** \brief The view on progress report regions for tools */
        wxPanel*                              process_display_view;

        /** \brief The location (incomplete path) to project directory the project name is the basename */
        boost::shared_ptr < project_manager > manager;

      private:

        /** \brief Initially places the widgets */
        void build();

        /** \brief Event handler for when a context menu is requested for a tree item */
        void on_tree_item_activate(wxTreeEvent&);

        /** \brief Creates a context menu for the selected processor */
        void spawn_context_menu(processor*);

      public:

        /** \brief Constructor */
        project(wxWindow* p, const boost::filesystem::path&);

        /** \brief Constructor, with project description */
        project(wxWindow* p, const boost::filesystem::path&, const std::string&);

        /** \brief Returns the name of the project */
        wxString get_name() const;
    };

    /**
     * @param[in] o the state of the output objects for this processor
     **/
    inline void project::node_data::change_state(processor::output_status o) {
    }
  }
}

#endif

