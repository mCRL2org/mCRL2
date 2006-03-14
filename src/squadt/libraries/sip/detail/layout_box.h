#ifndef LAYOUT_BOX_H
#define LAYOUT_BOX_H

#include <vector>
#include <utility>

#include <sip/detail/layout_base.h>

namespace sip {
  namespace layout {

    /** The variants for the box, a basic layout manager */
    enum box_variant {
      vertical,    ///< to lay out elements above one another
      horizontal   ///< to lay out elements beside one another
    };

    /**
     * \brief Box layout manager
     *
     * Elements are laid out horizontally or vertically, according to the chosen box variant.
     **/
    template < box_variant T = vertical >
    class box : public layout_manager {
      public:
        /** Constructor */
        box();
    };

    template < box_variant T >
    box< T >::box() {
    }

    template < >
    class box< vertical > : public layout_manager {
      public:
        /** The horizontal alignment of layout elements with respect to the containing element */
        enum alignment {
          left   = 0, ///< element is put as much to the left as possible
          center = 1, ///< element is centered
          right  = 2  ///< element is put as much to the right as possible
        };

      private:
        /** \brief The layout elements directly contained in this box */
        std::vector < std::pair < layout_element_ptr, alignment > > children;

      public:
        /** Adds a new element to the box */
        inline void add(layout_element_ptr&, alignment);
    };

    inline void box< vertical >::add(layout_element_ptr& e, alignment a) {
      children.push_back(std::pair < layout_element_ptr, alignment >(e, a));
    }

    template < >
    class box< horizontal > : public layout_manager {
      public:
        /** The vertical alignment of layout elements with respect to the containing element */
        enum alignment {
          top    = 0, ///< element is put as much to the top as possible
          center = 1, ///< element is centered
          bottom = 2  ///< element is put as much to the bottom as possible
        };

      private:
        /** \brief The layout elements directly contained in this box */
        std::vector < std::pair < layout_element_ptr, alignment > > children;

      public:
        /** Adds a new element to the box */
        inline void add(layout_element_ptr&, alignment);
    };

    inline void box< horizontal >::add(layout_element_ptr& e, alignment a) {
      children.push_back(std::pair < layout_element_ptr, alignment >(e, a));
    }
  }
}

#endif
