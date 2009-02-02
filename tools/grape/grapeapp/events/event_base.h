// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_base.h
//
// Declares the base event for GraPE events.

#ifndef GRAPE_EVENT_BASE_H
#define GRAPE_EVENT_BASE_H

#include <wx/cmdproc.h>

#include "architecturediagram.h"
#include "processdiagram.h"

using namespace grape::libgrape;

namespace grape
{
  namespace grapeapp
  {
    class grape_frame;

    /**
      * An enumerator for the diagram types.
      * Note: This enumerator is also used by the event handler, do not remove GRAPE_NO_DIAGRAM, GRAPE_PROCESS_DIAGRAM, GRAPE_ARCHITECTURE_DIAGRAM.
      */
    enum grape_diagram_type
    {
      GRAPE_NO_DIAGRAM = 0,
      GRAPE_PROCESS_DIAGRAM,
      GRAPE_ARCHITECTURE_DIAGRAM
    };

    /**
     * A geometry function used for resizing objects. Contains coordinate, width and height of a visual object.
     */
    struct grape_geometry
    {
      coordinate m_coord;
      float m_width;
      float m_height;
    };

    /**
     * \short Represents an event.
     */
    class grape_event_base : public wxCommand
    {
      protected:
        grape_frame   *m_main_frame;          /**< Pointer to the main frame. */

        /**
         * Closes and destroys the specification in m_main_frame gracefully.
         * @return @c true if closing went succesful, @c false otherwise.
         */
        bool close_specification();

        /**
         * Sets modified to true, refreshes the visuals and refreshes the canvas.
         */
        void finish_modification( void );

        /**
         * Object retrieval function.
         * Searches through all diagrams in the specification to find the object with the specified id and the specified type.
         * @param p_obj_id The identifier of the wanted object.
         * @param p_obj_type The type of the wanted object.
         * @param p_dia_id The identifier of the diagram the object is to be found in.
         * @return Returns a pointer to the desired object. Or @c 0 if the object couldn't be found.
         */
        object* find_object( unsigned int p_obj_id, object_type p_obj_type = ANY, int p_dia_id = -1 );

        /**
         * Diagram retrieval function.
         * Searches through all diagrams in the specification to find the one with the specified name and type.
         * @param p_name The name of the wanted diagram.
         * @param p_dia_type The type of the wanted diagram.
         * @return Returns a pointer to the desired diagram. Or @c 0 if the diagram couldn't be found.
         */
        diagram* find_diagram( wxString p_name, grape_diagram_type p_dia_type = GRAPE_NO_DIAGRAM );

        /**
         * Diagram retrieval function.
         * Searches through all diagrams in the specification to find the one with the specified id and specified type.
         * @param p_dia_id The identifier of the wanted diagram.
         * @param p_dia_type The type of the wanted diagram.
         * @return Returns a pointer to the desired diagram. Or @c 0 if the diagram couldn't be found.
         */
        diagram* find_diagram( unsigned int p_dia_id, grape_diagram_type p_dia_type = GRAPE_NO_DIAGRAM );
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_can_undo Flag raised when this event can be undone.
         * @param p_name Name of the event.
         */
        grape_event_base( grape_frame *p_main_frame, bool p_can_undo, const wxString &p_name );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_base( void );
    };
  }
}

#endif // GRAPE_EVENT_BASE_H
