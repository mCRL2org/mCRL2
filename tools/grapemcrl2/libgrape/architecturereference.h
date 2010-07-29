// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file architecturereference.h
//
// Declares the architecture_reference class.

#ifndef LIBGRAPE_ARCHITECTUREREFERENCE_H
#define LIBGRAPE_ARCHITECTUREREFERENCE_H

#include "compoundreference.h"

namespace grape
{
  namespace libgrape
  {

    // Forward declarations
    class architecture_diagram;

    /**
     * \short Represents an architecture reference.
     * A reference to an existing Architecture Diagram with matching name.
     * @see compound_reference
     */
    class architecture_reference : public compound_reference
    {
      protected:
        architecture_diagram    *m_refers_to_architecture;  /**< the architecture this reference refers to; is evaluated upon mCRL2 conversion, is 0 until then. */
      public:
        /**
         * Default constructor.
         * Initializes ArchitectureReference.
         */
        architecture_reference( void );

        /**
         * Copy constructor.
         * Creates a new architecture reference based on an existing one.
         * @param p_arch_ref The architecture reference to be copied.
         */
        architecture_reference( const architecture_reference &p_arch_ref );

        /**
         * Default destructor.
         * Frees allocated memory and removes all references to the object.
         */
        ~architecture_reference( void );

        /**
         * 'Refers-to' retrieval function.
         * @return Returns a pointer to the architecture diagram that this reference refers to.
         */
        architecture_diagram* get_relationship_refers_to( void );

        /**
         * 'Refers-to' assignment function.
         * Assigns the specified diagram to the refers-to relationship.
         * @param p_arch_diagram The architecture diagram this reference refers to.
         */
        void set_relationship_refers_to( architecture_diagram* p_arch_diagram );
    };

    /**
     * Array of ArchitectureReference.
     */
    WX_DECLARE_OBJARRAY( architecture_reference, arr_architecture_reference );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_ARCHITECTUREREFERENCE_H
