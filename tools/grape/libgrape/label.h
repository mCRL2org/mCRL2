// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file label.h
//
// Declares the label datatype.

#ifndef LIBGRAPE_LABEL_H
#define LIBGRAPE_LABEL_H

#include "varupdate.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a transitionlabel.
     * A Transition Label has a number of Local Variable Declarations,
     * an optional Condition that needs to be satisfied in order for the
     * Transition to be enabled, a Parametrized Multiaction, an optional
     * Timestamp indicating the time at which the Transition can occur
     * and a number of parameter/variable updates.
     */
    class label
    {
      private:
        bool              m_is_valid;     /**< @c true if the label's text is syntactically correct and processed successfully.*/
      protected:
        wxString          m_text;         /**< The label's text */
        dataexpression    m_actions;      /**< The label's actions */
        dataexpression    m_declarations; /**< The label's declarations */
        dataexpression    m_timestamp;    /**< The label's timestamp */
        list_of_varupdate m_variable_updates; /**< The label's variable updates */
        dataexpression    m_conditions;   /**< The label's conditions */

        /**
         * Processes the text supplied with set_text().
         * Sets the several data expressions and variable updates.
         * @post m_is_valid is set to @c true if m_text appeared to be valid, otherwise @c false
         */
        void process_text( void );

      public:
        /**
         * Default constructor.
         * Initializes label.
         */
        label( void );

        /**
         * Copy constructor.
         * Creates a new label based on an existing one.
         */
        label( const label &p_label );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~label( void );

        /**
         * Label text retrieval function
         * @return the text of the label
         */
        wxString get_text( void );

        /**
         * Label text assignment function
         * @param p_text The new text for the label
         * @return @c true if the text was syntactically correct and processed successfully. @c false otherwise.
         */
        bool set_text( const wxString &p_text );

        /**
         * Label actions retrieval function
         * @return the actions of the label
         */
        dataexpression &get_actions( void );

        /**
         * Label declarations retrieval function
         * @return the declarations of the label
         */
        dataexpression &get_declarations( void );

        /**
         * Label timestamp retrieval function
         * @return the timestamp of the label
         */
        dataexpression &get_timestamp( void );

        /**
         * Label variable updates retrieval function
         * @return the list of variable updates
         */
        list_of_varupdate &get_variable_updates( void );

        /**
         * Label donditions retrieval function
         * @return the conditions of the label
         */
        dataexpression &get_conditions( void );

        /**
         * Label has a valid text.
         * @return @c true if the label has a valid text, otherwise @c false .
         */
        bool has_valid_text() const;
    };

    /**
     * List of label. Represents a list of label datatypes.
     */
    WX_DECLARE_OBJARRAY( label, list_of_label );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_LABEL_H
