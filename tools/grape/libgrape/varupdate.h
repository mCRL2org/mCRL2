// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file varupdate.h
//
// Declares the varupdate datatype.

#ifndef LIBGRAPE_VARUPDATE_H
#define LIBGRAPE_VARUPDATE_H

#include <wx/string.h>
#include <wx/regex.h>

#include "var.h"
#include "dataexpression.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a variable update.
     * This is used in the label of a transition.
     */
    class varupdate
    {
      protected:
        dataexpression   m_dataexpression;   /**< data expression declaration of variable update. */
        var              m_var;              /**< variable declaration of variable update. */
      public:
        /**
         * Default constructor.
         * Initializes varupdate.
         */
        varupdate( void );

        /**
         * Copy constructor.
         * Creates a new copy constructor based on an existing one.
         * @param p_varupdate The varupdate to copy.
         */
        varupdate( const varupdate &p_varupdate );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~varupdate( void );

        /**
         * Variable update retrieval function.
         * @return Returns the variable update as a string.
         */
        wxString get_varupdate( void ) const;

        /**
         * Variable update text assignment function.
         * @param p_varupdate The text of the variable update.
         * @return Returns wether the variable update had a correct syntax.
         */
        bool set_varupdate( const wxString &p_varupdate );

        /**
         * Left hand side retrieval function.
         * @return The variable of the variable update.
         */
        wxString get_lhs( void ) const;

        /**
         * Left hand side assignment function.
         * @param p_var The text of the left hand side, a variable
         */
        void set_lhs( const wxString& p_var );

        /**
         * Right hand side retrieval function.
         * @return The data expression of the variable update.
         */
        wxString get_rhs( void ) const;

        /**
         * Right hand side assignment function.
         * @param p_update The text of the right hand side, a data expression
         */
        void set_rhs( const wxString& p_update );
    };

    /**
     * List of varupdate. Represents a list of varupdate datatypes.
     */
    WX_DECLARE_OBJARRAY( varupdate, list_of_varupdate );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_VARUPDATE_H
