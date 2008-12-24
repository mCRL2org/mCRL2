// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file datatypespecification.h
//
// Declares the datatype_specification datatype.

#ifndef LIBGRAPE_DATATYPESPECIFICATION_H
#define LIBGRAPE_DATATYPESPECIFICATION_H

#include "dataexpression.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a datatype specification.
     * A specification of datatypes, operations on them and properties
     * of the operations.
     */
    class datatype_specification
    {
      private:
      protected:
        wxString               m_dataexpressions;
        list_of_dataexpression m_dataexpression_list;  /**< list of dataexpressions in the datatype_specification */
      public:
        /**
         * Default constructor.
         * Initializes datatype_specification.
         */
        datatype_specification( void );

        /**
         * Copy constructor.
         * Creates a new datatype_specification based on an existing one.
         * @param p_datatype_specification The data specification to copy.
         */
        datatype_specification( const datatype_specification &p_datatype_specification );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~datatype_specification( void );

        /**
        * Declarations retrieval function
        * @return Returns a sring containing all dataexpressions.
        */
        wxString get_declarations( void ) const;

        /**
         * Dataexpressions assignment function
         * @param p_declarations A string containing all dataexpressions.
         */
        void set_declarations( const wxString &p_declarations );

        /**
         * List of dataexpressions retrieval function.
         * @pre The string containing all dataexpressions is a valid list of dataexpressions.
         * @return Returns a list of dataexpressions.
         */
        list_of_dataexpression get_declaration_list( void ) const;

        /**
         * Dataexpression count function
         * @pre The string containing all dataexpressions is a valid list of dataexpressions.
         * @return Returns the number of dataexpressions in the datatypespecification.
         */
        unsigned int get_count_dataexpression( void );
    };

    /**
     * List of datatype_specification. Represents a list of datatype_specification datatypes.
     */
    WX_DECLARE_OBJARRAY( datatype_specification, list_of_datatype_specification );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_DATATYPESPECIFICATION_H
