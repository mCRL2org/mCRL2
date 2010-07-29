// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/layout_manager.hpp
/// \brief Implementations of layout managers (display related)

#ifndef LAYOUT_MANAGER_H
#define LAYOUT_MANAGER_H

#include <utility>
#include <vector>
#include <map>
#include <memory>

#include "tipi/layout_base.hpp"

namespace tipi {
  namespace layout {

    /** \brief Type for element visibility */
    enum visibility {
      visible,  ///< the element is visible
      hidden,   ///< the element is not visible but still has effect on the layout
      none      ///< the element is invisible and has no effect on the layout
    };

    /** \brief The directional alignment of layout elements perpendicular to the orientation of the box */
    enum vertical_alignment {
      top = 0,    ///< element is put as much to the top as possible
      middle = 1, ///< element is centered
      bottom = 2  ///< element is put as much to the bottom as possible
    };

    /** \brief The directional alignment of layout elements perpendicular to the orientation of the box */
    enum horizontal_alignment {
      left = 3,   ///< element is put as much to the left as possible
      center = 4, ///< element is centered
      right = 5   ///< element is put as much to the right as possible
    };

    /** \brief Type for the margins that should be observed around the element */
    struct margins {
      unsigned short m_top;    ///< top margin in pixels
      unsigned short m_right;  ///< right margin in pixels
      unsigned short m_bottom; ///< bottom margin in pixels
      unsigned short m_left;   ///< left margin in pixels

      /** \brief Constructor */
      inline margins(const unsigned short = 0, const unsigned short = 0, const unsigned short = 0, const unsigned short = 0);

      /** \brief Sets all margins at once */
      inline margins& set(const unsigned short n) {
        m_top = m_right = m_bottom = m_left = n;

        return *this;
      }

      /** \brief Sets top margin */
      inline margins& set_top(const unsigned short top) {
        m_top = top;

        return *this;
      }

      /** \brief Sets right margin */
      inline margins& set_right(const unsigned int right) {
        m_right = right;

        return *this;
      }

      /** \brief Sets bottom margin */
      inline margins& set_bottom(const unsigned int bottom) {
        m_bottom = bottom;

        return *this;
      }

      /** \brief Sets left margin */
      inline margins& set_left(const unsigned int nleft) {
        m_left = nleft;

        return *this;
      }

      /**
       * \brief Compares for equality
       * \param[in] other the margins to compare against
       **/
      inline bool operator==(margins const& other) const {
        return (m_top == other.m_top && m_left == other.m_left &&
                m_bottom == other.m_bottom && m_right == other.m_right);
      }

      /**
       * \brief Compares for inequality
       * \param[in] other the margins to compare against
       **/
      inline bool operator!=(margins const& other) const {
        return (m_top != other.m_top || m_left != other.m_left ||
                m_bottom != other.m_bottom || m_right != other.m_right);
      }
    };

    /// \cond INTERNAL_DOCS
    /**
     * \brief Base class for layout constraint containers
     **/
    class properties : public ::utility::visitable {
      template < typename R, typename S >
      friend class ::utility::visitor;

      public:

        horizontal_alignment m_alignment_horizontal; ///< how the element is aligned horizontally
        vertical_alignment   m_alignment_vertical;   ///< how the element is aligned vertically
        margins              m_margin;               ///< the margins that should be observed around the element
        visibility           m_visible;              ///< whether or not the element affects layout and is visible
        bool                 m_grow;                 ///< whether or not to expand the control to fill available space
        bool                 m_enabled;              ///< whether or not the control is enabled

        /** \brief Constructor */
        inline properties() : m_alignment_horizontal(left), m_alignment_vertical(top),
                              m_margin(0, 0, 0, 0), m_visible(visible), m_grow(true), m_enabled(true) {
        }

        /** \brief Copy constructor */
        inline properties(properties const& p) : m_alignment_horizontal(p.m_alignment_horizontal),
                                                 m_alignment_vertical(p.m_alignment_vertical),
                                                 m_margin(p.m_margin), m_visible(p.m_visible),
                                                 m_grow(p.m_grow), m_enabled(p.m_enabled) {
        }

        /** \brief Constructor */
        inline properties(vertical_alignment const& av, horizontal_alignment const& ah, margins const& m, visibility const& v) :
                                                m_alignment_horizontal(ah), m_alignment_vertical(av), m_margin(m), m_visible(v), m_grow(true), m_enabled(true) {
        }

        /** \brief Constructor, for when horizontal alignment does not matter */
        inline properties(vertical_alignment const& av, margins const& m, visibility const& v) :
                                                m_alignment_horizontal(center), m_alignment_vertical(av), m_margin(m), m_visible(v), m_grow(true), m_enabled(true) {
        }

        /** \brief Constructor, for when vertical alignment does not matter */
        inline properties(horizontal_alignment const& ah, margins const& m, visibility const& v) :
                                                m_alignment_horizontal(ah), m_alignment_vertical(top), m_margin(m), m_visible(v), m_grow(true), m_enabled(true) {
        }

        /** \brief Whether the control is allowed to grow */
        inline void set_growth(bool b) {
          m_grow = b;
        }

        /** \brief Compares for equality */
        inline bool operator==(properties const& other) const {
          return (m_alignment_horizontal == other.m_alignment_horizontal &&
              m_alignment_vertical == other.m_alignment_vertical &&
              m_margin == other.m_margin && m_visible == other.m_visible &&
              m_grow == other.m_grow && m_enabled == other.m_enabled);
        }

        /** \brief Compares for inequality */
        inline bool operator!=(properties const& other) const {
          return (m_alignment_horizontal != other.m_alignment_horizontal ||
              m_alignment_vertical != other.m_alignment_vertical ||
              m_margin != other.m_margin || m_visible != other.m_visible ||
              m_grow != other.m_grow || m_enabled != other.m_enabled);
        }
    };
    /// \endcond

    /** \brief Abstract base class for layout managers */
    class manager : public tipi::layout::element {
      friend class tipi::layout::element;

      template < typename R, typename S >
      friend class ::utility::visitor;

      protected:

        /// \cond INTERNAL_DOCS
        struct layout_descriptor {
          element*           layout_element;    ///< the element
          properties         layout_properties; ///< the current layout properties

          /** \brief Constructor for an element and layout properties */
          layout_descriptor(element* e, properties const& p) : layout_element(e), layout_properties(p) { }
        };
        /// \endcond

      protected:

        /** \brief Default layout properties for elements that are added */
        properties m_default_properties;

      protected:

        /** \brief Constructor */
        manager();

        /** \brief Whether an element is set to grow to fill available space */
        bool grows(element const& e) const {
          return e.get_grow();
        }

      public:

        /** \brief Adds a new element to the box */
        virtual element& add(element&) = 0;

        /** \brief Adds a new element to the box */
        virtual element& add(element&, margins const&) = 0;

        /** \brief Adds a new element to the box */
        virtual element& add(element&, margins const&, visibility const&) = 0;

        /** \brief Adds a new element to the box */
        virtual element& add(element&, visibility const&) = 0;

        /** \brief Sets the default horizontal alignment used for adding elements */
        void set_default_alignment(horizontal_alignment const&);

        /** \brief Sets the default vertical alignment used for adding elements */
        void set_default_alignment(vertical_alignment const&);

        /** \brief Sets the default vertical alignment used for adding elements */
        template < typename A >
        A get_default_alignment() const;

        /** \brief Gets the default horizontal alignment used for adding elements */
        horizontal_alignment get_default_horizontal_alignment() const;

        /** \brief Gets the default vertical alignment used for adding elements */
        vertical_alignment get_default_vertical_alignment() const;

        /** \brief Sets the default margins used for adding elements */
        void set_default_margins(margins const&);

        /** \brief Gets the default margins used for adding elements */
        margins get_default_margins() const;

        /** \brief Sets the default visibility used for adding elements */
        void set_default_visibility(visibility const&);

        /** \brief Gets the default visibility used for adding elements */
        visibility get_default_visibility() const;

        /** \brief Finds a child element and makes it enabled/disabled */
        virtual void enable(element*, bool = true) = 0;

        /** \brief Disables an element */
        virtual void disable(element*) = 0;

        /** \brief Finds a child element and makes it visible/invisible */
        virtual void show(element*, bool = true) = 0;

        /** \brief Finds an element and makes it invisible */
        virtual void hide(element*) = 0;

        /** \brief Destructor */
        virtual ~manager() {};
    };

    /**
     * \brief Box layout manager
     *
     * Elements are laid out horizontally or vertically, according to the chosen box variant.
     *
     * The template parameter represents the alignment type: horizontal_alignment or vertical_alignment.
     **/
    template < typename A >
    class box : public manager {
      friend class tipi::layout::manager;
      friend class tipi::layout::element;

      template < typename R, typename S >
      friend class ::utility::visitor;

      private:

        /** \brief The type of the list with the element managed by this manager */
        typedef std::vector< layout_descriptor > children_list;

      private:

        /** \brief The layout elements directly contained in this box */
        children_list    m_children;

      private:

        /** \brief Constructor */
        box() {}

        /** \brief Resets private members to defaults */
        void clear();

        /** \brief Adds a new element to the box */
        element& add(element&, properties const&);

      public:

        /** \brief Sets the default horizontal alignment used for adding elements */
        box< A >& set_default_alignment(A const&);

        /** \brief Sets the default margins used for adding elements */
        box< A >& set_default_margins(margins const&);

        /** \brief Sets the default visibility used for adding elements */
        box< A >& set_default_visibility(visibility const&);

        /** \brief Gets the default horizontal alignment used for adding elements */
        A get_default_alignment();

        /** \brief Adds a new element to the box */
        element& add(element&);

        /** \brief Adds a new element to the box */
        element& add(element&, margins const&);

        /** \brief Adds a new element to the box */
        element& add(element&, margins const&, visibility const&);

        /** \brief Adds a new element to the box */
        element& add(element&, visibility const&);

        /** Adds a new element to the box */
        template < typename T >
        box< A >& append(T&);

        /** Adds a new element to the box */
        template < typename T >
        box< A >& append(T&, margins const&);

        /** Adds a new element to the box */
        template < typename T >
        box< A >& append(T&, margins const&, visibility const&);

        /** Adds a new element to the box */
        template < typename T >
        box< A >& append(T&, visibility const&);

        /** Adds a new element to the box */
        template < typename T >
        box< A >& append(T&, A const&);

        /** Adds a new element to the box */
        template < typename T >
        box< A >& append(T&, A const&, margins const&);

        /** Adds a new element to the box */
        template < typename T >
        box< A >& append(T&, A const&, margins const&, visibility const&);

        /** \brief Enables an element */
        void enable(element*, bool = true);

        /** \brief Disables an element */
        void disable(element*);

        /** \brief Shows or hides the element is visible, (does not influence layout) */
        void show(element*, bool = true);

        /** \brief Makes an element invisible */
        void hide(element*);
    };

    /**
     * \brief Vertical box layout manager
     *
     * Elements are placed vertically one after the other in the available space
     **/
    typedef box< horizontal_alignment > vertical_box;

    /**
     * \brief Horizontal box layout manager
     *
     * Elements are laid out horizontally one after the other in the available space
     **/
    typedef box< vertical_alignment >   horizontal_box;

    /**
     * \param[in] top the top margin
     * \param[in] right the right margin
     * \param[in] bottom the bottom margin
     * \param[in] left the left margin
     **/
    inline margins::margins(const unsigned short top, const unsigned short right,
                            const unsigned short bottom, const unsigned short left) :
                               m_top(top), m_right(right), m_bottom(bottom), m_left(left) {
    }

    inline manager::manager() : m_default_properties(top, left, margins(0,0,0,0), visible) {
    }

    /**
     * \param[in] a the new horizontal alignment
     **/
    inline void manager::set_default_alignment(horizontal_alignment const& a) {
      m_default_properties.m_alignment_horizontal = a;
    }

    /**
     * \return the default horizontal alignment
     **/
    template < >
    inline horizontal_alignment manager::get_default_alignment() const {
      return m_default_properties.m_alignment_horizontal;
    }

    /**
     * \return the default vertical alignment
     **/
    template < >
    inline vertical_alignment manager::get_default_alignment() const {
      return m_default_properties.m_alignment_vertical;
    }

    /**
     * \return the default horizontal alignment
     **/
    inline horizontal_alignment manager::get_default_horizontal_alignment() const {
      return m_default_properties.m_alignment_horizontal;
    }

    /**
     * \param[in] a the new vertical alignment
     **/
    inline void manager::set_default_alignment(vertical_alignment const& a) {
      m_default_properties.m_alignment_vertical = a;
    }

    /**
     * \return the default vertical alignment
     **/
    inline vertical_alignment manager::get_default_vertical_alignment() const {
      return m_default_properties.m_alignment_vertical;
    }

    /**
     * \param[in] m the new margins
     **/
    inline void manager::set_default_margins(margins const& m) {
      m_default_properties.m_margin = m;
    }

    /**
     * \return the default margins
     **/
    inline margins manager::get_default_margins() const {
      return m_default_properties.m_margin;
    }

    /**
     * \param[in] v the new default visibility
     **/
    inline void manager::set_default_visibility(visibility const& v) {
      m_default_properties.m_visible = v;
    }

    inline visibility manager::get_default_visibility() const {
      return m_default_properties.m_visible;
    }

    template < typename A >
    inline void box< A >::clear() {
      m_children.clear();
    }

    /**
     * \param[in] e a pointer to a layout element
     **/
    template < typename A >
    inline element& box< A >::add(element& e) {
      return add(e, manager::m_default_properties);
    }

    /**
     * \param[in] e a pointer to a layout element
     * \param[in] c the layout properties to observe
     **/
    template < typename A >
    inline element& box< A >::add(element& e, properties const& c) {
      properties cn(c);

      cn.set_growth(grows(e));

      m_children.push_back(layout_descriptor(&e,cn));

      return e;
    }

    /**
     * \param[in] e the element
     * \return *this
     **/
    template < typename A >
    template < typename T >
    inline box< A >& box< A >::append(T& e) {
      add(static_cast < element& > (e), m_default_properties);

      return *this;
    }

    /**
     * \param[in] e the element
     * \param[in] m the margins to use for this element
     * \return *this
     **/
    template < typename A >
    template < typename T >
    inline box< A >& box< A >::append(T& e, margins const& m) {
      add(static_cast < element& > (e), properties(manager::get_default_vertical_alignment(), manager::get_default_horizontal_alignment(), m, manager::get_default_visibility()));

      return *this;
    }

    /**
     * \param[in] e the element
     * \param[in] m the margins to use for this element
     * \param[in] v the visiblity for this element
     * \return *this
     **/
    template < typename A >
    template < typename T >
    inline box< A >& box< A >::append(T& e, margins const& m, visibility const& v) {
      add(static_cast < element& > (e), properties(manager::get_default_vertical_alignment(), manager::get_default_horizontal_alignment(), m, v));

      return *this;
    }

    /**
     * Adds a new element to the box
     * \param[in] e the element
     * \param[in] v the visiblity for this element
     * \return *this
     **/
    template < typename A >
    template < typename T >
    inline box< A >& box< A >::append(T& e, visibility const& v) {
      add(static_cast < element& > (e), properties(manager::get_default_vertical_alignment(), manager::get_default_horizontal_alignment(), manager::get_default_margins(), v));

      return *this;
    }

    /**
     * \param[in] e the element
     * \param[in] b whether it should be enabled
     **/
    template < typename A >
    inline void box< A >::enable(element* e, bool b) {
      for (children_list::iterator i = m_children.begin(); i != m_children.end(); ++i) {
        if (i->layout_element == e) {
          i->layout_properties.m_enabled = b;
        }
      }

      activate_handlers();
    }

    /**
     * \param[in] e the element
     **/
    template < typename A >
    inline void box< A >::disable(element* e) {
      enable(e, false);
    }

    /**
     * \param[in] e the element
     * \param[in] b whether it should be visible
     **/
    template < typename A >
    inline void box< A >::show(element* e, bool b) {
      for (children_list::iterator i = m_children.begin(); i != m_children.end(); ++i) {
        if (i->layout_element == e) {
          i->layout_properties.m_visible = b ? visible : hidden;
        }
      }

      activate_handlers();
    }

    /**
     * \param[in] e the element
     **/
    template < typename A >
    inline void box< A >::hide(element* e) {
      show(e, false);
    }

    /**
     * \param[in] a the new horizontal alignment
     *
     * \return *this
     **/
    template < typename A >
    inline box< A >& box< A >::set_default_alignment(A const& a) {
      manager::set_default_alignment(a);

      return *this;
    }

    /**
     * \param[in] m the new margins
     *
     * \return *this
     **/
    template < typename A >
    inline box< A >& box< A >::set_default_margins(margins const& m) {
      manager::set_default_margins(m);

      return *this;
    }

    /**
     * \param[in] v the new default visibility
     *
     * \return *this
     **/
    template < typename A >
    inline box< A >& box< A >::set_default_visibility(visibility const& v) {
      manager::set_default_visibility(v);

      return *this;
    }

    /**
     * \return the default horizontal alignment
     **/
    template < typename A >
    inline A box< A >::get_default_alignment() {
      return manager::get_default_alignment< A >();
    }

    /**
     * \param[in] e a pointer to a layout element
     * \param[in] a how the element should be aligned relative to the box
     * \return *this
     **/
    template < >
    template < typename T >
    inline box< horizontal_alignment >& box< horizontal_alignment >::append(T& e, horizontal_alignment const& a) {
      box::add(static_cast < element& > (e), properties(manager::get_default_vertical_alignment(), a, manager::get_default_margins(), manager::get_default_visibility()));

      return *this;
    }

    /**
     * \param[in] e a pointer to a layout element
     * \param[in] a how the element should be aligned relative to the box
     * \return *this
     **/
    template < >
    template < typename T >
    inline box< vertical_alignment >& box< vertical_alignment >::append(T& e, vertical_alignment const& a) {
      box::add(static_cast < element& > (e), properties(a, manager::get_default_horizontal_alignment(), manager::get_default_margins(), manager::get_default_visibility()));

      return *this;
    }

    /**
     * \param[in] e a pointer to a layout element
     * \param[in] a how the element should be aligned relative to the box
     * \param[in] m the margins of the element relative to other elements that occupy the box
     * \return *this
     **/
    template < >
    template < typename T >
    inline box< horizontal_alignment >& box< horizontal_alignment >::append(T& e, horizontal_alignment const& a, margins const& m) {
      box::add(static_cast < element& > (e), properties(manager::get_default_vertical_alignment(), a, m, manager::get_default_visibility()));

      return *this;
    }

    /**
     * \param[in] e a pointer to a layout element
     * \param[in] a how the element should be aligned relative to the box
     * \param[in] m the margins of the element relative to other elements that occupy the box
     * \return *this
     **/
    template < >
    template < typename T >
    inline box< vertical_alignment >& box< vertical_alignment >::append(T& e, vertical_alignment const& a, margins const& m) {
      box::add(static_cast < element& > (e), properties(a, manager::get_default_horizontal_alignment(), m, manager::get_default_visibility()));

      return *this;
    }

    /**
     * \param[in] e a reference to a layout element
     * \param[in] a how the element should be aligned relative to the box
     * \param[in] m the margins of the element relative to other elements that occupy the box
     * \param[in] v whether the element is visible and has an effect on other elements that occupy the box
     * \return *this
     **/
    template < >
    template < typename T >
    inline box< horizontal_alignment >& box< horizontal_alignment >::append(T& e, horizontal_alignment const& a, margins const& m, visibility const& v) {
      box::add(static_cast < element& > (e), properties(manager::get_default_vertical_alignment(), a, m, v));

      return *this;
    }

    /**
     * \param[in] e a reference to a layout element
     * \param[in] a how the element should be aligned relative to the box
     * \param[in] m the margins of the element relative to other elements that occupy the box
     * \param[in] v whether the element is visible and has an effect on other elements that occupy the box
     * \return *this
     **/
    template < >
    template < typename T >
    inline box< vertical_alignment >& box< vertical_alignment >::append(T& e, vertical_alignment const& a, margins const& m, visibility const& v) {
      box::add(static_cast < element& > (e), properties(a, manager::get_default_horizontal_alignment(), m, v));

      return *this;
    }

    /**
     * \param[in] e a reference to a layout element
     * \param[in] m the margins of the element relative to other elements that occupy the box
     * \return *this
     **/
    template < typename A >
    inline element& box< A >::add(element& e, margins const& m) {
      box::add(e, properties(manager::get_default_vertical_alignment(), manager::get_default_horizontal_alignment(), m, manager::get_default_visibility()));

      return *this;
    }

    /**
     * \param[in] e a pointer to a layout element
     * \param[in] m the margins of the element relative to other elements that occupy the box
     * \param[in] v whether the element is visible and has an effect on other elements that occupy the box
     * \return *this
     **/
    template < typename A >
    inline element& box< A >::add(element& e, margins const& m, visibility const& v) {
      box::add(e, properties(manager::get_default_vertical_alignment(), manager::get_default_horizontal_alignment(), m, v));

      return *this;
    }

    /**
     * \param[in] e a pointer to a layout element
     * \param[in] v whether the element is visible and has an effect on other elements that occupy the box
     * \return *this
     **/
    template < typename A >
    inline element& box< A >::add(element& e, visibility const& v) {
      box::add(e, properties(manager::get_default_vertical_alignment(), manager::get_default_horizontal_alignment(), manager::get_default_margins(), v));

      return *this;
    }
  }
}

#endif
