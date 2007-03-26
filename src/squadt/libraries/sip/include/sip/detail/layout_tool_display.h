#ifndef LAYOUT_TOOL_DISPLAY_H
#define LAYOUT_TOOL_DISPLAY_H

#include <iosfwd>
#include <map>
#include <vector>
#include <functional>

#include <boost/bind.hpp>

#include "sip/layout_base.h"
#include "sip/detail/basic_datatype.h"
#include "sip/detail/layout_manager.h"

namespace sip {

  /** \brief Basic container class for layout elements */
  class display : public utility::visitable {
    template < typename R, typename S >
    friend class utility::visitor;

    public:

      /** \brief Type alias for the abstract element class */
      typedef sip::layout::element                                                       element;

      /** \brief Type alias for element identifiers */
      typedef sip::layout::element_identifier                                            element_identifier;

      /** \brief Type alias for a mapping from identifier to element */
      typedef std::map < sip::layout::element_identifier, sip::layout::element* >        element_for_id;

    protected:

      /** \brief Whether or not the tool display is visible to the user */
      element_for_id        m_element_by_id;

    protected:

      /** \brief Associate an id with an element */
      void associate(sip::layout::element_identifier const&, sip::layout::element*);

      /** \brief Disassociate an id with an element */
      void disassociate(sip::layout::element const*);

      /** \brief Disassociate an id with an element */
      void disassociate(sip::layout::element_identifier const&);

    public:

      /** \brief Find an element by its id */
      sip::layout::element const* find(sip::layout::element_identifier const&) const;

      /** \brief Find the id for an element */
      sip::layout::element_identifier find(sip::layout::element const*) const;

      /** \brief Pure virtual destructor */
      virtual ~display() = 0;
  };

  namespace layout {

    /** \brief Basic container class for controller-side layout definitions */
    class tool_display : public sip::display {
      template < typename R, typename S >
      friend class utility::visitor;

      public:

        /** \brief Convenience type alias to hide the shared pointer implementation */
        typedef boost::shared_ptr < tool_display >          sptr;

        /** \brief Convenience type for passing and returning a sequence of layout elements */
        typedef std::vector < sip::layout::element const* > constant_elements;

      private:

        /** \brief the layout manager that contains all widgets for this display */
        std::auto_ptr < layout::manager > m_manager;

        /** \brief Whether or not the tool display is visible to the user */
        bool                              m_visible;

      public:

        /** \brief Constructor */
        inline tool_display();

        /** \brief Constructor */
        inline tool_display(std::auto_ptr < layout::manager >);

        /** \brief Factory function */
        inline static boost::shared_ptr < layout::tool_display > create(std::auto_ptr < layout::manager >);

        /** \brief Returns whether or not the contents should be visible */
        inline bool get_visibility() const;

        /** \brief Whether or not the tool display is shown */
        inline void show(bool);

        /** \brief Get the layout manager that contains all widgets for this display */
        inline layout::manager const* get_manager() const;

        /** \brief Set the layout manager that contains all widgets for this display */
        inline void set_manager(std::auto_ptr < layout::manager >);

        /** \brief Creates a (G)UI for this tool_display */
        mediator::wrapper_aptr instantiate(mediator* m) const;

        /** \brief Update (part of) the layout structure */
        void update(std::string const&, std::vector < sip::layout::element const* >& elements);
    };

    inline tool_display::tool_display() : m_visible(true) {
    }

    inline tool_display::tool_display(std::auto_ptr < layout::manager > m) : m_manager(m), m_visible(true) {
    }

    inline boost::shared_ptr < layout::tool_display > tool_display::create(std::auto_ptr < layout::manager > m) {
      boost::shared_ptr < layout::tool_display > p(new tool_display(m));

      return (p);
    }

    inline layout::manager const* tool_display::get_manager() const {
      return (m_manager.get());
    }

    inline void tool_display::set_manager(std::auto_ptr < layout::manager > m) {
      m_manager = m;
    }

    inline bool tool_display::get_visibility() const {
      return (m_visible);
    }

    /**
     * \param[in] s Whether or not the tool display must be visible
     **/
    inline void tool_display::show(bool s) {
      m_visible = s;
    }
  }

  inline display::~display() {
  }

  /**
   * \pre the element should be in the list
   **/
  inline sip::layout::element const* display::find(sip::layout::element_identifier const& id) const {
    return visitors::search(*this, id);
  }

  /**
   * \pre the element should be in the list
   **/
  inline sip::layout::element_identifier display::find(sip::layout::element const* e) const {
    return visitors::search(*this, e);
  }

  inline void display::associate(sip::layout::element_identifier const& id, sip::layout::element* e) {
    m_element_by_id[id] = e;
  }

  inline void display::disassociate(sip::layout::element_identifier const& id) {
    element_for_id::iterator i = m_element_by_id.find(id);

    if (i != m_element_by_id.end()) {
      m_element_by_id.erase(i);
    }
  }
}

#endif
