// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file preamble.h
//
// Declares the preamble datatype.

#ifndef LIBGRAPE_PREAMBLE_H
#define LIBGRAPE_PREAMBLE_H

#include "decl.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a preamble.
     * A parameter declaration and a local variable declaration.
     */
    class preamble
    {
      protected:
        list_of_decl      m_parameter_declarations_list;      /**< parameter declarations list in this preamble, is set up when check_paramater_declarations_syntax is called, and is Empty until then. */
        list_of_decl_init m_local_variable_declarations_list; /**< local viariable declarations list in this preamble, is set up when check_local_variable_declaration_syntax is called, and is empty until then. */
        float             m_width;    /**< width of preamble. */
        float             m_height;   /**< height of preamble. */
      public:
        /**
         * Default constructor.
         * Initializes preamble.
         */
        preamble( void );

        /**
         * Copy constructor.
         * Creates a new preamble based on an existing preamble.
         * @param p_preamble The preamble to copy.
         */
        preamble( const preamble &p_preamble );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~preamble( void );

        /**
         * Parameter declarations retrieval function
         * @return The parameter declarations of the preamble as a string
         */
        wxString get_parameter_declarations( void ) const;

        /**
         * Parameter declarations assignment function.
         * Assigns the text to the parameter declarations of the object.
         * @param p_parameter_declarations The new parameter declarations
         * @return wheter input is correct or not
         */
        bool set_parameter_declarations( const wxString &p_parameter_declarations );

         /**
         * Parameter declarations retrieval function
         * @pre check_parameter_declarations_syntax() == true
         * @return The parameter declarations of the preamble as a list_of_decl_init.
         */
        list_of_decl get_parameter_declarations_list( void ) const;

        /**
         * Parameter declarations assignment function.
         * Assigns the list of declarations to the parameter declarations of the object.
         * @param p_parameter_declarations The new parameter declarations
         */
        void set_parameter_declarations_list( const list_of_decl &p_parameter_declarations );

        /**
         * Local variable declarations retrieval function
         * @return The local variable declarations of the preamble as a string
         */
        wxString get_local_variable_declarations( void ) const;

        /**
         * Local variable declarations assignment function.
         * Assigns the text to the local variable declarations of the object.
         * @param p_local_variable_declarations The new local variable declarations
         * @return whether input is correct or not
         */
        bool set_local_variable_declarations( const wxString &p_local_variable_declarations );

         /**
         * Local variable declarations retrieval function
         * @pre check_local_variable_declarations_syntax() == True
         * @return The local variable declarations of the preamble as a list_of_decl
         */
        list_of_decl_init get_local_variable_declarations_list( void ) const;

        /**
         * Parameter declarations assignment function.
         * Assigns the list of declarations to the parameter declarations of the object.
         * @param p_local_variable_declarations The new parameter declarations
         */
        void set_local_variable_declarations_list( const list_of_decl_init &p_local_variable_declarations );

        /**
         * Width retrieval function
         * Returns
         * @return The width of the preamble.
         */
        float get_width( void ) const;

        /**
         * Width assignment function.
         * Assigns the width to the width of the preamble.
         * @param p_width The new width.
         */
        void set_width( float p_width );

        /**
         * Height retrieval function
         * @return The height of the preamble.
         */
        float get_height( void ) const;

        /**
         * Height assignment function.
         * Assigns the height to the width of the preamble.
         * @param p_height The new height.
         */
        void set_height( float p_height );

    };

    /**
     * List of preamble. Represents a list of preamble datatypes.
     */
    WX_DECLARE_OBJARRAY( preamble, list_of_preamble );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_PREAMBLE_H

