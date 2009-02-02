// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file referencestate.h
//
// Declares the reference_state class.

#ifndef LIBGRAPE_REFERENCESTATE_H
#define LIBGRAPE_REFERENCESTATE_H

#include "compoundstate.h"

namespace grape
{
  namespace libgrape
  {

    // Forward declarations
    class process_diagram;

    /**
     * \short Represents a process reference as used in a process diagram.
     * A situation in which a process can possibly reside. A State has
     * an optional name.
     */
    class reference_state : public compound_state
    {
      protected:
        list_of_varupdate   m_parameter_assignments;  /**< initial parameter values */
        process_diagram     *m_refers_to_process;     /**< the process this reference refers to; is evaluated upon mCRL2 conversion, is 0 until then. */
      public:
        /**
         * Default constructor.
         * Initializes reference_state.
         */
        reference_state(void);

        /**
         * Copy constructor.
         * Creates a new reference state based on an existing one.
         * @param p_ref_state The reference state to copy.
         */
        reference_state( const reference_state &p_ref_state );

        /**
         * Default destructor.
         * Frees allocated memory  and removes all references to the object.
         */
        ~reference_state(void);

        /**
         * 'Refers-to' retrieval function.
         * @return Returns a pointer to the process diagram that this reference refers to.
         */
        process_diagram* get_relationship_refers_to( void );

        /**
         * 'Refers-to' assignment function.
         * Assigns the specified diagram to the refers-to relationship.
         * @param p_proc_diagram The process diagram this reference refers to.
         */
        void set_relationship_refers_to( process_diagram* p_proc_diagram );

        /**
         * Parameterassignments retrieval function.
         * @return Returns the list of parameterassignments in the form of variable updates.
         */
        list_of_varupdate get_parameter_updates( void ) const;

        void set_parameter_updates( const list_of_varupdate& p_parameter_assignments );

        /**
         * Sets the variable updates with a string. Will process the text;
         * @param p_text The text containing variable updates.
         * @return @c true if the text was valid, otherwise @c false .
         */
        bool set_text( const wxString &p_text );

        /** @return The variable updates as a string. */
        wxString get_text() const;
    };

    /**
     * Array of ReferenceState.
     */
    WX_DECLARE_OBJARRAY(reference_state, arr_reference_state);

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_REFERENCESTATE_H
