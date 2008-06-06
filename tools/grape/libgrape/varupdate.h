// Author(s): VitaminB100
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
        wxString m_varupdate;              /**< full string declaration of variable update. */
        dataexpression *m_dataexpression;   /**< data expression declaration of variable update. */
        var *m_var;                         /**< variable declaration of variable update. */
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
         */
        void set_varupdate( const wxString &p_varupdate );

        /**
         * Left hand side retrieval function.
         * @return The variable of the variable update.
         */
        var * get_lhs( void );

        /**
         * Right hand side retrieval function.
         * @return The data expression of the variable update.
         */
        dataexpression * get_rhs( void );
    };

    /**
     * List of varupdate. Represents a list of varupdate datatypes.
     */
    WX_DECLARE_OBJARRAY( varupdate, list_of_varupdate );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_VARUPDATE_H
