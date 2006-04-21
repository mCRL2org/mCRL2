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
        class node_data : public wxTreeItemData {
          friend class project;

          private:

            /** \brief The associated project */
            project&       parent;

            /** \brief The associated processor */
            processor::ptr target;

          private:

            /** \brief Updates the GUI to reflect the change in state */
            inline void update_state(processor::output_status);

          public:

            /** \brief Constructor */
            inline node_data(project&);

            /** \brief Sets the associated processor */
            inline void set_processor(processor::ptr);

            /** \brief Gets the associated processor */
            inline processor::ptr& get_processor();

            /** \brief Gets the associated project window */
            inline project const* get_project() const;
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

        /** \brief Event handler for when an item of a context menu is selected */
        void on_context_menu_select(wxCommandEvent&);

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
    inline void project::node_data::update_state(processor::output_status o) {
    }

    inline project const* project::node_data::get_project() const {
      return (&parent);
    }

    /**
     * @param p pointer to the process that should be associated with this
     **/
    inline void project::node_data::set_processor(processor::ptr p) {
      target = p;
    }

    inline processor::ptr& project::node_data::get_processor() {
      return (target);
    }
  }
}

#endif

