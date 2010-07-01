// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file
 *
 * \brief A header file defining a transition as a triple from,label,to.
 * \author Muck van Weerdenburg, Jan Friso Groote
 */

#ifndef MCRL2_LTS_TRANSITION_H
#define MCRL2_LTS_TRANSITION_H

namespace mcrl2
{

namespace lts
{

  /** \brief Transition sort styles.
   * \details This enumerated type defines sort styles for transitions.
   * They can be used to sort the transitions of an LTS based on various
   * criteria. */
  enum transition_sort_style
  {
    src_lbl_tgt, /**< Sort first on source state, then on label, then on target state */
    lbl_tgt_src  /**< Sort first on label, then on target state, then on source state*/
  };

  /// \brief A class containing triples, source label and target representing transitions.
  /// \details A transition consists of three indices, indicated by transition::size_type
  ///          that refer to a source, label and target.
  class transition
  {
    public:
      /// \brief The type of the elements in a transition.
      typedef unsigned int size_type;

    private:
      size_type m_from;
      size_type m_label;
      size_type m_to;

    public:
      // There is no default constructor
      //   transition():from(0),label(0),to(0)
      //   {}
  
      /// \brief Constructor (there is no default constructor). 
      transition(const unsigned int f, 
                 const unsigned int l,
                 const unsigned int t):m_from(f),m_label(l),m_to(t)
      {}
      
      /// \brief Copy constructor.
      transition(const transition &t)
      { m_from=t.m_from;
        m_label=t.m_label;
        m_to=t.m_to;
      }

      /// \brief The source of the transition.
      size_type from() const
      { return m_from;
      }

      /// \brief The label of the transition.
      size_type label() const
      { return m_label;
      }

      ///\brief The target of the transition.
      size_type to() const
      { return m_to;
      }

      /// \brief Set the source of the transition.
      void set_from(const size_type from)
      { m_from=from;
      }

      /// \brief Set the label of the transition.
      void set_label(const size_type label)
      { m_label=label;
      }

      ///\brief Set the target of the transition.
      void set_to(const size_type to)
      { m_to=to;
      }

      bool operator <(const transition &t) const
      { return m_from < t.m_from ||
               (m_from==t.m_from && (m_label<t.m_label ||
                    (m_label==t.m_label && m_to<t.m_to)));
      }
  };

  /// \brief An iterator const range to visit transitions.
  typedef boost::iterator_range< std::vector< transition >::const_iterator > transition_const_range;

  /// \brief An iterator range to visit and change transitions.
  typedef boost::iterator_range< std::vector< transition >::iterator > transition_range;


}
}

#endif // MCRL2_LTS_TRANSITION_H
