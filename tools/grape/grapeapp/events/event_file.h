// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_file.h
//
// Declares GraPE events for file operations.

#ifndef GRAPE_EVENT_FILE_H
#define GRAPE_EVENT_FILE_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {

    /**
     * \short Represents the new event.
     */
    class grape_event_new : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_new( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_new( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };

    /**
     * \short Represents the open event.
     */
    class grape_event_open : public grape_event_base
    {
      private:
        wxFileName      m_filename;     /**< Filename optionally provided by the command line */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_filename The file name of the to be opened grape specification.
         */
        grape_event_open( grape_frame *p_main_frame,  const wxString &p_filename = wxEmptyString );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_open( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };

    /**
     * \short Represents the close event.
     */
    class grape_event_close : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_close( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_close( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };

    /**
     * \short Represents the save event.
     */
    class grape_event_save : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_save( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_save( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };

    /**
     * \short Represents the save as event.
     */
    class grape_event_saveas : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_saveas( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_saveas( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };

    /**
     * \short Represents the print event.
     */
    class grape_event_print : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_print( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_print( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };

    /**
     * \short Represents the import event.
     */
    class grape_event_import : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_import( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_import( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };

    /**
     * \short Represents the export to text event for the datatype specification.
     */
    class grape_event_export_datatype_specification_text : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_export_datatype_specification_text( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_export_datatype_specification_text( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };
  }
}

#endif // GRAPE_EVENT_FILE_H
