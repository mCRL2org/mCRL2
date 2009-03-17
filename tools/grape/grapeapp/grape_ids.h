// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_ids.h
//
// Declares identifiers used by the widgets.

#ifndef GRAPEAPP_GRAPE_IDS_H
#define GRAPEAPP_GRAPE_IDS_H

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Contains all widget and event identifiers.
     */
    enum
    {
      GRAPE_SPLITTER = 1,
      GRAPE_TIMER,

      // menubar events
      GRAPE_MENU_EXPORTMCRL2,
      GRAPE_MENU_EXPORTIMAGE,
      GRAPE_MENU_EXPORTTEXT,
      GRAPE_MENU_VALIDATE,
      GRAPE_MENU_VALIDATE_DIAGRAM,

      // diagram manipulation
      GRAPE_MENU_SELECT_ALL,
      GRAPE_MENU_DESELECT_ALL,

      // dialog boxes appear when these are invoked
      GRAPE_MENU_ADD_PROCESS_DIAGRAM,
      GRAPE_MENU_ADD_ARCHITECTURE_DIAGRAM,
      GRAPE_MENU_RENAME_DIAGRAM,
      GRAPE_MENU_REMOVE_DIAGRAM,

      // toolbar tools
      // both diagram tools
      GRAPE_TOOL_SELECT,
      GRAPE_TOOL_ATTACH,
      GRAPE_TOOL_DETACH,
      GRAPE_TOOL_ADD_COMMENT,
      // process diagram tools
      GRAPE_TOOL_ADD_TERMINATING_TRANSITION,
      GRAPE_TOOL_ADD_NONTERMINATING_TRANSITION,
      GRAPE_TOOL_ADD_INITIAL_DESIGNATOR,
      GRAPE_TOOL_ADD_STATE,
      GRAPE_TOOL_ADD_REFERENCE_STATE,
      // architecture diagram tools
      GRAPE_TOOL_ADD_PROCESS_REFERENCE,
      GRAPE_TOOL_ADD_ARCHITECTURE_REFERENCE,
      GRAPE_TOOL_ADD_CHANNEL,
      GRAPE_TOOL_ADD_CHANNEL_COMMUNICATION,
      GRAPE_TOOL_ADD_VISIBLE,
      GRAPE_TOOL_ADD_BLOCKED,

      // listbox events
      GRAPE_DATATYPE_SPEC_BUTTON,
      GRAPE_ARCHITECTURE_DIAGRAM_LIST,
      GRAPE_PROCESS_DIAGRAM_LIST,

      // object manipulation methods
      GRAPE_MENU_PROPERTIES,

      // diagram manipulation methods
      GRAPE_MENU_RENAME,
      GRAPE_MENU_DELETE,

      GRAPE_MENU_IMPORT_FROM_MCRL2,
//      GRAPE_MENU_EXPORT_TO_MCRL2,
//      GRAPE_MENU_EXPORT_TO_PICTURE,
      GRAPE_MENU_SET_PARAMETERS,
      GRAPE_MENU_SIMULATE,

      GRAPE_MENU_DATATYPESPEC,

      GRAPE_DATASPEC_TEXT,

      // dialog events
      GRAPE_GRID_TEXT,
      GRAPE_PARAMETER_GRID_TEXT,
      GRAPE_LOCALVAR_GRID_TEXT,

      GRAPE_VAR_DECLS_INPUT_TEXT,
      GRAPE_MULTIACTION_INPUT_TEXT,

      GRAPE_TIMESTAMP_INPUT_TEXT,
      GRAPE_CONDITION_INPUT_TEXT,
      GRAPE_VAR_UPDATES_INPUT_TEXT,
      
      GRAPE_CHANNEL_NAME_INPUT_TEXT,
      
      GRAPE_COMBO_TEXT
    };

  }
} // namespace grape

#endif // GRAPEAPP_GRAPE_IDS_H
