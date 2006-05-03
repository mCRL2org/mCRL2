#ifndef SQUADT_PROJECT_H
#define SQUADT_PROJECT_H

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/splitter.h>
#include <wx/scrolwin.h>
#include <wx/treectrl.h>

#include "processor.h"
#include "gui_miscellaneous.h"

namespace squadt {

  class project_manager;

  namespace GUI {

    class main;
    class tool_display;

    /**
     * \brief Represents the main view of a project
     **/
    class project : wxSplitterWindow {
      friend class squadt::GUI::main;
      friend class squadt::GUI::tool_display;

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
            project&   parent;

            /** \brief The associated output object */
            processor::object_descriptor* target;

          private:

            /** \brief Updates the GUI to reflect the change in state */
            inline void update_state(processor::output_status);

          public:

            /** \brief Constructor */
            inline node_data(project&);

            /** \brief Constructor */
            inline node_data(project&, processor::object_descriptor*);

            /** \brief Sets the associated processor */
            inline void associate(processor::object_descriptor*);
        };

        /** \brief Performs GUI updates in idle time on behalf of other threads */
        class builder: public wxEvtHandler {
          friend class GUI::tool_display;
       
          private:
       
            /** \brief The list of tool displays that need to be updated */
            std::deque < tool_display* >  tool_displays;
       
          private:
       
            /** \brief The event handler that does the actual updating */
            void process(wxIdleEvent&);
       
          public:
       
            /** \brief Constructor */
            builder();
       
            /** \brief Schedule a tool display for update */
            void schedule_update(tool_display*);
        };
       
      private:

        /** \brief The view on processors and their interdependencies */
        wxTreeCtrl*                           file_view;

        /** \brief The view on progress report regions for tools */
        wxScrolledWindow*                     process_display_view;

        /** \brief The location (incomplete path) to project directory the project name is the basename */
        boost::shared_ptr < project_manager > manager;

        /** \brief The list of temporary processors that exist for tool configuration purposes */
        std::vector < processor::ptr >        temporary_processors;

        /** \brief Instantiation of a builder for this project */
        builder                               gui_builder;

      private:

        /** \brief Initially places the widgets */
        void build();

        /** \brief Loads the contents of the views (currently only file view) */
        void load_views();

        /** \brief Event handler for when a context menu is requested for a tree item */
        void on_tree_item_activate(wxTreeEvent&);

        /** \brief Event handler for when an item of a context menu is selected */
        void on_context_menu_select(wxCommandEvent&);

        /** \brief Creates a context menu for the selected processor */
        void spawn_context_menu(processor::object_descriptor const&);

        /** \brief Helper function to add tools by category to a context menu */
        void add_to_context_menu(const storage_format&, const miscellaneous::tool_selection_helper::tools_by_category::value_type&, wxMenu*, int*);

        /** \brief Add a new tool display to the process_display_view */
        GUI::tool_display* add_tool_display();

      public:

        /** \brief Constructor, with project description */
        project(wxWindow* p, const boost::filesystem::path&, const std::string& = "");

        /** \brief Descructor */
        ~project();

        /** \brief Returns the name of the project */
        wxString get_name() const;
    };

    /**
     * @param[in,out] p a shared pointer to the processor for which process is monitored and reported
     **/
    inline project::node_data::node_data(project& p) : parent(p) {
    }

    /**
     * @param[in,out] p a shared pointer to the processor for which process is monitored and reported
     * @param[in] t the processor that is to be associated 
     **/
    inline project::node_data::node_data(project& p, processor::object_descriptor* t) : parent(p), target(t) {
    }

    /**
     * @param[in] o the state of the output objects for this processor
     **/
    inline void project::node_data::update_state(processor::output_status o) {
    }

    /**
     * @param p pointer to the process that should be associated with this
     **/
    inline void project::node_data::associate(processor::object_descriptor* p) {
      target = p;
    }
  }
}

#endif

