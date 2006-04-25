#ifndef LAYOUT_MANAGER_H
#define LAYOUT_MANAGER_H

#include <utility>
#include <vector>

#include <sip/detail/layout_base.h>
#include <sip/detail/layout_mediator.h>

namespace sip {
  namespace layout {

    /** \brief Type for the margins that should be observed around the element */
    struct margins {
      unsigned short top;    ///< top margin in pixels
      unsigned short right;  ///< right margin in pixels
      unsigned short bottom; ///< bottom margin in pixels
      unsigned short left;   ///< left margin in pixels
    };

    /** \brief Type for element visibility */
    enum visibility {
      visible,       ///< the element is visible
      hidden,        ///< the element is not visible but still has effect on the layout
      none           ///< the element is invisible and has no effect on the layout
    };

    /** \brief Base class for layout constraint containers */
    class constraints {
    };

    /** \brief Abstract base class for layout managers */
    class manager : public element {
      public:

        /** \brief Convenience type for hiding shared pointer implementation */
        typedef boost::shared_ptr < manager > sptr;

      protected:

        /** \brief Attaches a layout element to a manager, using layout constraints */
        inline void attach(layout::mediator*, mediator::wrapper_aptr, constraints const*);

      public:

        /* \brief Destructor */
        virtual ~manager() = 0;
    };

    /** \brief The variants for the box, a basic layout manager */
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
    class box : public manager {
      public:
        /** Constructor */
        inline box();
    };

    template < box_variant T >
    inline box< T >::box() {
    }

    /**
     * \brief Vertical box layout manager
     *
     * Elements are laid out vertically
     **/
    template < >
    class box< vertical > : public manager {
      public:

        /** \brief The directional alignment of layout elements perpendicular to the orientation of the box */
        enum alignment {
          left   = 0, ///< element is put as much to the left as possible
          center = 1, ///< element is centered
          right  = 2  ///< element is put as much to the right as possible
        };

        class constraints : public layout::constraints {
          public:
            alignment   align;   ///< how the element is aligned
            margins     margin;  ///< the margins that should be observed around the element
            visibility  visible; ///< whether the element affects layout and is visible

            /** \brief Constructor */
            inline constraints(alignment&, margins&, visibility&);
        };

      private:

        /** \brief The type of the list with the element managed by this manager */
        typedef std::vector < std::pair < element::sptr, constraints > > children_list;

      private:

        /** \brief The layout elements directly contained in this box */
        children_list children;

      public:

        /** Adds a new element to the box */
        inline void add(element::sptr&, alignment& a, margins& m, visibility& v);

        /** \brief Instantiate a layout element, through a mediator */
        inline mediator::wrapper_aptr instantiate(layout::mediator*);
    };

    /**
     * \brief Horizontal box layout manager
     *
     * Elements are laid out horizontally
     **/
    template < >
    class box< horizontal > : public manager {
      public:

        /** \brief The directional alignment of layout elements perpendicular to the orientation of the box */
        enum alignment {
          top    = 0, ///< element is put as much to the top as possible
          center = 1, ///< element is centered
          bottom = 2  ///< element is put as much to the bottom as possible
        };

        class constraints : public layout::constraints {
          public:
            alignment   align;   ///< how the element is aligned
            margins     margin;  ///< the margins that should be observed around the element
            visibility  visible; ///< whether the element affects layout and is visible

            /** \brief Constructor */
            inline constraints(alignment&, margins&, visibility&);
        };

      private:

        /** \brief The type of the list with the element managed by this manager */
        typedef std::vector < std::pair < element::sptr, constraints > > children_list;

      private:

        /** \brief The layout elements directly contained in this box */
        children_list children;

      public:

        /** Adds a new element to the box */
        inline void add(element::sptr&, alignment& a, margins& m, visibility& v);

        /** \brief Instantiate a layout element, through a mediator */
        inline mediator::wrapper_aptr instantiate(layout::mediator*);
    };

    /**
     * @param m the mediator object to use
     * @param d the data needed
     * @param c the layout constraints
     **/
    inline void manager::attach(layout::mediator* m, mediator::wrapper_aptr d, constraints const* c) {
      m->attach(d, c);
    }

    box< vertical >::constraints::constraints(alignment& a, margins& m, visibility& v) :
                                                align(a), margin(m), visible(v) {
    }

    box< horizontal >::constraints::constraints(alignment& a, margins& m, visibility& v) :
                                                align(a), margin(m), visible(v) {
    }

    inline void box< vertical >::add(element::sptr& e, alignment& a, margins& m, visibility& v) {
      children.push_back(children_list::value_type(e, constraints(a, m, v)));
    }

    inline void box< horizontal >::add(element::sptr& e, alignment& a, margins& m, visibility& v) {
      children.push_back(children_list::value_type(e, constraints(a, m, v)));
    }

    /**
     * @param m the mediator object to use
     **/
    inline layout::mediator::wrapper_aptr box< vertical >::instantiate(layout::mediator* m) {
      layout::mediator::aptr n = m->build_vertical_box();

      m = n.get();

      for (children_list::const_iterator i = children.begin(); i != children.end(); ++i) {
        attach(m, (*i).first->instantiate(m), dynamic_cast < const layout::constraints* > (&(*i).second));
      }

      return (n->get_data());
    }

    /**
     * @param m the mediator object to use
     **/
    inline layout::mediator::wrapper_aptr box< horizontal >::instantiate(layout::mediator* m) {
      layout::mediator::aptr n = m->build_horizontal_box();

      m = n.get();

      for (children_list::const_iterator i = children.begin(); i != children.end(); ++i) {
        attach(m, (*i).first->instantiate(m), dynamic_cast < const layout::constraints* > (&(*i).second));
      }

      return (n->get_data());
    }
  }
}

#endif
