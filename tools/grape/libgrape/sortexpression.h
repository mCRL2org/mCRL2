// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sortexpression.h
//
// Declares the sortexpression datatype.

#ifndef LIBGRAPE_SORTEXPRESSION_H
#define LIBGRAPE_SORTEXPRESSION_H

#include <wx/wx.h>
#include <wx/dynarray.h>

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a sort expression.
     */
    class sortexpression
    {
      protected:
        wxString m_expression; /**< The sortexpression's text */
      public:
        /**
         * Default constructor.
         * Initializes sortexpression.
         */
        sortexpression( void );

        /**
         * Copy constructor.
         * Creates a new sort expression based on an existing one.
         * @param p_sortexpression The sort expression to copy.
         */
        sortexpression( const sortexpression &p_sortexpression );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~sortexpression( void );

        /**
         * The sort expression text assignment function
         * @param p_expression The text of the sort expression
         */
        void set_expression( const wxString &p_expression );

        /**
         * The sort expression text retreival function
         * @return The text of the sort expression
         */
        wxString get_expression( void ) const;
    };

    /**
     * List of sortexpression. Represents a list of sortexpression datatypes.
     */
    WX_DECLARE_OBJARRAY( sortexpression, list_of_sortexpression );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_SORTEXPRESSION_H
