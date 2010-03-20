// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file comment.h
//
// Declares the comment class.

#ifndef LIBGRAPE_COMMENT_H
#define LIBGRAPE_COMMENT_H

#include "object.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a comment.
     */
    class comment : public object
    {
      protected:
        wxString  m_text;           /**< text of comment. */

        object    *m_connected_to;  /**< object comment is connected to. */

        bool m_reference_selected; /**< Selected reference of the comment. */
      public:
        /**
         * Default constructor.
         * Initializes comment.
         */
        comment( void );

        /**
         * Copy constructor.
         * Creates a new comment based on an existing one.
         * @param p_comment The comment to be copied.
         */
        comment( const comment &p_comment );

        /**
         * Default destructor.
         * Frees allocated memory  and removes all references to the object.
         */
        ~comment( void );

        /**
         * Text retrieval function
         * @return The text of the comment.
         */
        wxString get_text( void ) const;

        /**
         * Assigns the text to the text of the comment.
         * @pre The comment is selected.
         * @param p_text The new text.
         */
        void set_text( wxString &p_text );

        /**
         * Comment attachment function.
         * Attaches the comment to an object.
         * @param p_object A pointer to the object
         */
        void attach_to_object( object* p_object );

        /**
         * Comment detachment function.
         * Detaches the comment from whichever object is was attached to.
         * @pre true
         * @post m_connected_to == 0
         */
        void detach_from_object( void );

        /**
         * Attached object retrieval function.
         * @return Returns the object the comment is attached to.
         */
        object* get_attached_object( void );
        
        /**
         * Set the reference selected value
         * @param p_is_reference_selected assigned value
         */
        void set_reference_selected(bool p_is_reference_selected);

        /** 
         * Get the reference selected value
         * @return Returns whether the reference of the comment is selected.
         */
        bool get_reference_selected();
    };

    /**
     * Array of Comment.
     */
    WX_DECLARE_OBJARRAY( comment, arr_comment );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_COMMENT_H
