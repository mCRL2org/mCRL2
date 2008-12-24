// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
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

#include "action.h"
#include "decl.h"
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
        wxString          m_text;         /**< The label's text 	m_declarations.[m_condition]m_actions@timestamp/varupdates */
        list_of_decl      m_declarations; /**< The label's declarations */
        dataexpression    m_condition;    /**< The label's condition */
        list_of_action    m_actions;      /**< The label's actions */
        dataexpression    m_timestamp;    /**< The label's timestamp */
        list_of_varupdate m_variable_updates; /**< The label's variable updates */

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
        wxString get_text( void ) const;

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
        list_of_action &get_actions( void );

        wxString get_actions_text( void ) const;

        /**
         * Label declarations retrieval function
         * @return the declarations of the label
         */
        list_of_decl &get_declarations( void );

        wxString get_declarations_text( void ) const;

        /**
         * Label timestamp retrieval function
         * @return the timestamp of the label
         */
        wxString get_timestamp( void ) const;

        /**
         * Label variable updates retrieval function
         * @return the list of variable updates
         */
        list_of_varupdate &get_variable_updates( void );

        wxString get_variable_updates_text( void ) const;

        /**
         * Label donditions retrieval function
         * @return the conditions of the label
         */
        wxString get_condition( void ) const;

        void set_declarations( const list_of_decl &p_declarations );
        void set_condition( const wxString &p_condition );
        void set_actions( const list_of_action &p_actions );
        void set_timestamp( const wxString &p_timestamp );
        void set_variable_updates( const list_of_varupdate &p_variable_updates );

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
