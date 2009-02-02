// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
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
#include "dataexpression.h"

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
         * Declaration retrieval function
         * @return the declaration
         */
        wxString get_decl( void ) const;

        /**
         * Declaration assignment function
         * @param p_decl The new declaration
         * @return Returns wether the declaration had a correct syntax.
         */
        bool set_decl( const wxString &p_decl );

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



    /**
     * \short Represents a variable declaration and initialization.
     */
    class decl_init : public decl
    {
      protected:
        dataexpression    m_value;        /**< initial value of this variable. */
      public:
        /**
         * Default constructor.
         * Initializes decl_init.
         * @pre True.
         * @post This decl_init is initialized with default values.
         */
        decl_init(void);

        /**
         * Copy constructor.
         * Creates a new declaration and initialization, based on an existing one.
         * @param p_decl_init An existing decl_init to copy members from.
         * @pre p_decl_init is an existing decl_init.
         * @post This decl_init is a copy of p_decl_init.
         */
        decl_init(const decl_init &p_decl_init);

        /**
         * Default destructor.
         * Frees allocated memory.
         * @pre True.
         * @post The memory allocated by this decl_init is freed.
         */
        ~decl_init(void);

        /**
         * Initial declaration initialisation retrieval function.
         * @return Returns the declaration initialisation.
         */
        wxString get_decl_init( void ) const;

        /**
         * Initial declaration initialisation assignment function.
         * Assigns the initial value of this variable.
         * @param p_decl_init The declaration initialisation to assign.
         * @return Returns wether the declaration initialisation had a correct syntax.
         */
        bool set_decl_init( const wxString &p_decl_init );

        /**
         * Initial value retrieval function.
         * Returns the initial value of this variable.
         * @return The initial value of this variable.
         * @pre True.
         * @post The initial value of this variable is returned.
         */
        wxString get_value(void) const;

        /**
         * Initial value assignment function.
         * Assigns the initial value of this variable.
         * @param p_value The initial value to assign.
         * @pre True.
         * @post m_value is p_value.
         */
        void set_value(const wxString &p_value);
    };

    /**
     * List of decl_init. Represents a list of decl_init datatypes.
     */
    WX_DECLARE_OBJARRAY( decl_init, list_of_decl_init );
  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_DECL_H
