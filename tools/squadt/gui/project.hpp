// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/project.h
/// \brief Add your file description here.

#ifndef SQUADT_PROJECT_H
#define SQUADT_PROJECT_H

#include <deque>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/filesystem/path.hpp>

#include "../processor.hpp"

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/splitter.h>
#include <wx/scrolwin.h>
#include <wx/treectrl.h>
#include <wx/timer.h>

namespace squadt {

  class project_manager;

  namespace GUI {

    class main;
    class tool_display;

    /**
     * \brief Represents the main view of a project
     **/
    class project : public wxSplitterWindow {
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
        class tool_data;

        /** \brief Performs GUI updates in idle time on behalf of other threads */
        class builder: public wxEvtHandler {
          friend class GUI::tool_display;
       
          private:
       
            /** \brief The list of tool displays that need to be updated */
            std::deque < boost::function < void () > > tasks;

            /** \brief Timer for GUI update cycle for tool displays */
            wxTimer                                    timer;
       
          private:
       
            /** \brief The event handler that does the actual updating */
            void process(wxTimerEvent&);
       
          public:
       
            /** \brief Constructor */
            builder();
       
            /** \brief Schedule a tool display for update */
            void schedule_update(boost::function < void () >);
        };
       
      private:

        /** \brief The view on processors and their interdependencies */
        wxTreeCtrl*                           object_view;

        /** \brief The view on progress report regions for tools */
        wxScrolledWindow*                     process_display_view;

        /** \brief The location (incomplete path) to project directory the project name is the basename */
        boost::shared_ptr < project_manager > manager;

        /** \brief Instantiation of a builder for this project */
        builder                               gui_builder;

      private:

        /** \brief Initially places the widgets */
        void build();

        /** \brief Add a file to the project */
        void add_existing_file();

        /** \brief Add a new file to the project */
        void add_new_file();

        /** \brief Builds or rebuilds objects such that all objects in the project are up to date */
        void update();

        /** \brief Loads the contents of the views (currently only file view) */
        void load_views();

        /** \brief Handler for the event that a context menu is requested for a tree item */
        void on_tree_item_activate(wxTreeEvent&);

        /** \brief Handler for the event that an item of a context menu is selected */
        void on_context_menu_select(wxCommandEvent&);

        /** \brief Handler for the event that an item has been edited by the user */
        void on_object_name_edited(wxTreeEvent& e);

        /** \brief Handler for the event that objects are being dragged by the user */
        void on_object_drag(wxTreeEvent& e);

        /** \brief Creates a context menu for the selected processor */
        void spawn_context_menu(tool_data&);

        /** \brief Add new processor outputs to the object view */
        void update_after_configuration(wxTreeItemId, boost::shared_ptr< processor >, bool);

        /** \brief Adds the outputs produced by a processor as objects to the object view */
        bool add_outputs_as_objects(wxTreeItemId, boost::shared_ptr< processor >);

        /** \brief Adds a single object to the object view */
        void add_to_object_view(wxTreeItemId& s, boost::shared_ptr< processor::object_descriptor >);

        /** \brief Add a new tool display to the process_display_view */
        GUI::tool_display* install_tool_display(boost::shared_ptr < processor::monitor >, std::string const&);

        /** \brief Helper function that extracts the information out of a processor and calls install_tool_display() */
        void prepare_tool_display(processor* p);

        /** \brief Resolves conflicts, through user interaction, between processors that generate same output objects with the same name */
        void resolve_conflict(wxTreeItemId s, boost::shared_ptr< processor::object_descriptor > e, boost::shared_ptr< processor::object_descriptor > n);

        /** \brief Reports a conflict by presenting a non-interactive dialog window with details */
        void report_conflict(wxString const& s);

        /** \brief Updates the status of files in the object view */
        void set_object_status(boost::weak_ptr< processor > const&, const wxTreeItemId);

        /** \brief Updates the status of files in the object view */
        void update_object_status(boost::weak_ptr< processor > const&, const wxTreeItemId);

      public:

        /** \brief Constructor, with project description */
        project(main* p, const boost::filesystem::path&, const std::string& = "", bool = false);

        /** \brief Store project specification */
        void store();

        /** \brief Checks whether the project contains a file */
        bool exists(boost::filesystem::path const&) const;

        /** \brief Returns the name of the project */
        wxString get_name() const;

        /** \brief Destructor */
        ~project();
    };
  }
}

#endif

