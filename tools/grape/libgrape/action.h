// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file action.h
//
// Declares the action datatype.

#ifndef LIBGRAPE_ACTION_H
#define LIBGRAPE_ACTION_H

#include "dataexpression.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents an action.
     * An action is an event of any kind. An Action has a name and a number of
     * data expressions as parameters.
     */
    class action
    {
      private:
      protected:
        wxString                m_name;           /**< name of this action. */
        list_of_dataexpression  m_parameters;     /**< parameters of this action. */
      public:
        /**
         * Default constructor.
         * Initializes action.
         */
        action( void );

        /**
         * Copy constructor.
         * Creates a new action based on an existing action.
         * @param p_action The action to be copied.
         */
        action( const action &p_action );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~action( void );

        /**
         * Text retrieval function.
         * @return Returns the text of the action.
         */
        wxString get_text( void );

        /**
         * Name retrieval function.
         * @return Returns the name of the action.
         */
        wxString get_name( void );

        /**
         * Name assignment function.
         * @param p_name The new name of the action.
         */
        void set_name( const wxString &p_name );

        /**
         * Parameter retrieval function.
         * @return Returns the parameters of the action.
         */
        list_of_dataexpression get_parameters( void );

        /**
         * Parameter assignment function.
         * @param p_parameters String containing the parameters of the action.
         */
        void set_parameters_text( wxString p_parameters );
        
        /**
         * Parameter assignment function.
         * @param p_parameters The new parameters of the action.
         */
        void set_parameters( list_of_dataexpression p_parameters);
    };

    /**
     * List of action. Represents a list of action datatypes.
     */
    WX_DECLARE_OBJARRAY( action, list_of_action );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_ACTION_H
