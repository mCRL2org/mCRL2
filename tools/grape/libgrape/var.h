// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file var.h
//
// Declares the var datatype.

#ifndef LIBGRAPE_VAR_H
#define LIBGRAPE_VAR_H

#include <wx/wx.h>
#include <wx/dynarray.h>

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a variable.
     */
    class var
    {
      protected:
        wxString m_var;        /**< variable */
        wxString m_name;       /**< name declarations of the variable. */
        wxString m_type;       /**< type declaration of the variable. */
      public:
        /**
         * Default constructor.
         * Initializes var.
         */
        var( void );

        /**
         * Copy constructor.
         * Creates a new variable based on an existing one.
         * @param p_var The variable to copy.
         */
        var( const var &p_var );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~var( void );

        /**
         * Variable assignment function.
         * @param p_var The new variable.
         */
        void set_var( const wxString &p_var );

        /**
         * Variable retrieval function.
         * @return The declared variable.
         */
        wxString get_var( void ) const;
    };

    /**
     * List of var. Represents a list of var datatypes.
     */
    WX_DECLARE_OBJARRAY( var, list_of_var );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_VAR_H
