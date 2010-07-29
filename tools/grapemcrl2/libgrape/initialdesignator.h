// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file initialdesignator.h
//
// Declares the InitialDesignator class.

#ifndef LIBGRAPE_INITIALDESIGNATOR_H
#define LIBGRAPE_INITIALDESIGNATOR_H

#include "object.h"

namespace grape
{
  namespace libgrape
  {

    // forward declarations
    class compound_state;

    /**
     * \short Represents an initial designator.
     * An initial designator points to the initial situation in which a process resides upon
     * start of execution of the process. The Initial State Designator /
     * Initial Process Reference Designator points to either a State or a
     * Process Reference.
     * @see state
     * @see process_reference
     */
    class initial_designator : public object
    {
      protected:
        compound_state   *m_designates;  /**< state associated with this designator. */

      public:
        /**
         * Default constructor.
         * Initializes initial_designator.
         */
        initial_designator( void );

        /**
         * Copy constructor.
         * Creates a new initial designator based on an existing one.
         * @param p_init_designator The initial designator to copy.
         */
        initial_designator( const initial_designator &p_init_designator );

        /**
         * Default destructor.
         * Frees allocated memory  and removes all references to the object.
         */
        ~initial_designator( void );

        /**
         * compound_state association function.
         * @param p_compound_state The compound_state that the initial_designator designates
         */
        void attach( compound_state* p_compound_state );


        /**
         * compound_state dissociation function.
         */
        void detach( void );

        /**
         * Attached state retrieval function.
         * @return Returns a pointer to the state the initial designator is attached to.
         */
        compound_state * get_attached_state( void );
    };

    /**
     * Array of InitialDesignator pointers.
     */
    WX_DEFINE_ARRAY_PTR(initial_designator *, arr_initial_designator_ptr);

    /**
     * Array of InitialDesignator.
     */
    WX_DECLARE_OBJARRAY(initial_designator, arr_initial_designator);

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_INITIALDESIGNATOR_H
