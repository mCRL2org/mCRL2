// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file state.h
//
// Declares the state class.

#ifndef LIBGRAPE_STATE_H
#define LIBGRAPE_STATE_H

#include "compoundstate.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a state.
     * A situation in which a process can possibly reside. A State has
     * an optional name.
     * @see compound_state
     */
    class state : public compound_state
    {
      protected:
        wxString                          m_name;                 /**< (optional) name of this state. */
      public:
        /**
         * Default constructor.
         * Initializes state.
         */
        state( void  );

        /**
         * Copy constructor.
         * Creates a new state based on an existing one.
         * @param p_state The state to copy.
         */
        state( const state &p_state );

        /**
         * Default destructor.
         * Frees allocated memory  and removes all references to the object.
         * Destroys all associated transitions
         * Destroys all associated initial designators
         */
        ~state( void );
    };

    /**
     * Array of State.
     */
    WX_DECLARE_OBJARRAY( state, arr_state );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_STATE_H
