// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file compoundreference.h
//
// Declares the compound_reference class.

#ifndef LIBGRAPE_COMPOUNDREFERENCE_H
#define LIBGRAPE_COMPOUNDREFERENCE_H

#include "channel.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a compound (process- or architecture-) reference.
     * @see process_reference
     * @see architecture_reference
     */
    class compound_reference : public object
    {
      private:
        /**
         * Default constructor. Shouldn't be called from GraPE code.
         * Initializes CompoundReference.
         */
        compound_reference( void );

      protected:
        wxString        m_name;             /**< name of reference. */
        bool            m_current_reference; /**< flag used during simulation. */

        arr_channel_ptr m_has_channel;      /**< channels connected to this compound reference. */

      public:
        /**
         * Constructor.
         * Initializes CompoundReference.
         * @param p_type The type of this reference. Should be either @c PROCESS_REFERENCE or @c ARCHITECTURE_REFERENCE .
         */
        compound_reference( object_type p_type );

        /**
         * Copy constructor.
         * Creates a compound reference from an existing one.
         * @param p_compound_ref The connection to copy.
         */
        compound_reference( const compound_reference &p_compound_ref );

        /**
         * Default destructor.
         * Frees allocated memory  and removes all references to the object.
         */
        ~compound_reference( void );

        /**
         * Name retrieval function.
         * @return The name of the compound reference.
         */
        wxString get_name(void) const;

       /**
        * Name assignment function.
        * @pre The compound reference is selected.
        * @param p_name The new name of the compound reference.
        */
        void set_name( const wxString &p_name );

        /**
         * Channel attach function.
         * Associates the specified channel with the compound reference.
         * @param p_channel The channel.
         */
        void attach_channel( channel* p_channel );

        /**
         * Channel dissociate function.
         * Dissociates the specified channel from the compound_reference.
         * @param p_channel The channel.
         */
        void detach_channel( channel* p_channel );

        /**
         * Channel retrieval function.
         * @return Returns a pointer to the channel at position i in the list of channels.
         */
        channel* get_channel( int i );

        /**
         * Channel count function.
         * @return Returns the number of channels associated with the reference.
         */
        unsigned int count_channel( void );
    };

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_COMPOUNDREFERENCE_H
