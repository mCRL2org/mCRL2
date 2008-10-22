// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file decl.h
//
// Declares the decl datatype.

#ifndef LIBGRAPE_DECL_H
#define LIBGRAPE_DECL_H

#include "sortexpression.h"

#include <wx/wx.h>
#include <wx/dynarray.h>

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a declaration.
     */
    class decl
    {
      protected:
        wxString m_name; /**< name of this action. */
        sortexpression m_type; /**< type of this action. */
      public:
        /**
         * Default constructor.
         * Initializes decl.
         */
        decl( void );

        /**
         * Copy constructor.
         * Creates a new declaration based on an existing one.
         */
        decl( const decl &p_decl );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~decl( void );

        /**
         * Declaration name retrieval function
         * @return the name of the declaration
         */
        wxString get_name( void ) const;

        /**
         * Declaration name assignment function
         * @param p_name The new name for the declaration
         */
        void set_name( const wxString &p_name );

        /**
         * Declaration type retrieval function
         * @return the type of the declaration
         */
        wxString get_type( void ) const;

        /**
         * Declaration type assignment function
         * @param p_type The new type for the declaration
         */
        void set_type( const wxString &p_type );

    };

    /**
     * List of decl. Represents a list of decl datatypes.
     */
    WX_DECLARE_OBJARRAY( decl, list_of_decl );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_DECL_H
