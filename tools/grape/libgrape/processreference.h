// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file processreference.h
//
// Declares the process_reference class.

#ifndef LIBGRAPE_PROCESSREFERENCE_H
#define LIBGRAPE_PROCESSREFERENCE_H

#include "compoundreference.h"
#include "varupdate.h"

namespace grape
{
  namespace libgrape
  {

    // Forward declarations
    class process_diagram;

    /**
     * \short Represents a process reference.
     * A reference to a Process Diagram with matching name and parameters.
     * A Process Reference contains the initialization of each parameter.
     * @see compound_reference
     */
    class process_reference : public compound_reference
    {
      protected:
        list_of_varupdate m_parameter_assignments; /**< initial parameter values. */
        process_diagram*  m_refers_to_process;     /**< the process this reference refers to; is evaluated upon mCRL2 conversion, is 0 until then. */
      public:
        /**
         * Default constructor.
         * Initializes process_reference.
         */
        process_reference( void );

        /**
         * Copy constructor.
         * Creates a new process reference based on an existing one.
         * @param p_process_ref The process reference to copy.
         */
        process_reference( const process_reference &p_process_ref );

        /**
         * Default destructor.
         * Frees allocated memory and removes all references to the object.
         */
        ~process_reference( void );

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
     * Array of ProcessReference.
     */
    WX_DECLARE_OBJARRAY( process_reference, arr_process_reference );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_PROCESSREFERENCE_H
