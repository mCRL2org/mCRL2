#ifndef LAYOUT_MANAGER_H
#define LAYOUT_MANAGER_H

#include <utility>
#include <vector>
#include <memory>

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
      friend class sip::layout::element;

      public:

        /** \brief Convenience type for hiding auto pointer implementation */
        typedef std::auto_ptr < manager > aptr;

      public:

        /** \brief The default margins between elements */
        static margins    default_margins;

        /** \brief The default visibility of elements */
        static visibility default_visibility;

      protected:

        /** \brief Attaches a layout element to a manager, using layout constraints */
        inline void attach(layout::mediator*, mediator::wrapper_aptr, constraints const*) const;

      public:

        /** \brief Adds a new element to the box */
        virtual void add(element*) = 0;

        /** \brief Recursively builds the state of the object */
        static aptr static_read_structure(xml2pp::text_reader&); 

        /** \brief Instantiate a layout element, through a mediator */
        virtual mediator::wrapper_aptr instantiate(layout::mediator*) const = 0;

        /** \brief Destructor */
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

    /** \brief Convenience type for vertically oriented box layout manager */
    typedef box < sip::layout::vertical >   box_vertical;

    /** \brief Convenience type for horizontally oriented box layout manager */
    typedef box < sip::layout::horizontal > box_horizontal;

    /**
     * \brief Vertical box layout manager
     *
     * Elements are laid out vertically
     **/
    template < >
    class box< vertical > : public manager {
      friend class sip::layout::manager;
      friend class sip::layout::element;

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
            inline constraints(alignment const&, margins const&, visibility const&);
        };

        /** \brief Default alignment */
        static alignment   default_alignment;

        /** \brief Default constraints */
        static constraints default_constraints;

      private:

        /** \brief The type of the list with the element managed by this manager */
        typedef std::vector < std::pair < element*, constraints > > children_list;

      private:

        /** \brief The layout elements directly contained in this box */
        children_list children;

      private:

        /** \brief Read back a layout structure in XML format */
        inline void read_structure(xml2pp::text_reader& r);

        /** \brief Resets private members to defaults */
        inline void clear();

      public:

        /** \brief Constructor */
        inline box();

        /** \brief Instantiates a layout manager and returns a shared pointer */
        inline static manager::aptr create();

        /** Adds a new element to the box */
        inline void add(element*);

        /** Adds a new element to the box */
        inline void add(element*, constraints const&);

        /** Adds a new element to the box */
        inline void add(element*, alignment const&,
                                  margins const& = manager::default_margins,
                                  visibility const& = manager::default_visibility);

        /** \brief Instantiate a layout element, through a mediator */
        inline mediator::wrapper_aptr instantiate(layout::mediator*) const;

        /** \brief Write out the layout structure in XML format */
        inline void write_structure(std::ostream&);

        /** \brief Destructor */
        inline ~box();
    };

    /**
     * \brief Horizontal box layout manager
     *
     * Elements are laid out horizontally
     **/
    template < >
    class box< horizontal > : public manager {
      friend class sip::layout::manager;
      friend class sip::layout::element;

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
            inline constraints(alignment const&, margins const&, visibility const&);
        };

        /** \brief Default alignment */
        static alignment   default_alignment;

        /** \brief Default constraints */
        static constraints default_constraints;

      private:

        /** \brief The type of the list with the element managed by this manager */
        typedef std::vector < std::pair < element*, constraints > > children_list;

      private:

        /** \brief The layout elements directly contained in this box */
        children_list children;

      private:

        /** \brief Read back a layout structure in XML format */
        inline void read_structure(xml2pp::text_reader& r);

        /** \brief Resets private members to defaults */
        inline void clear();

      public:

        /** \brief Constructor */
        inline box();

        /** \brief Instantiates a layout manager and returns a shared pointer */
        inline static manager::aptr create();

        /** Adds a new element to the box */
        inline void add(element*);

        /** Adds a new element to the box */
        inline void add(element*, constraints const&);

        /** Adds a new element to the box */
        inline void add(element*, alignment const&,
                                  margins const& = default_margins,
                                  visibility const& = default_visibility);

        /** \brief Instantiate a layout element, through a mediator */
        inline mediator::wrapper_aptr instantiate(layout::mediator*) const;

        /** \brief Write out the layout structure in XML format */
        inline void write_structure(std::ostream&);

        /** \brief Destructor */
        inline ~box();
    };

    /**
     * @param m the mediator object to use
     * @param d the data needed
     * @param c the layout constraints
     **/
    inline void manager::attach(layout::mediator* m, mediator::wrapper_aptr d, constraints const* c) const {
      m->attach(d, c);
    }

    inline manager::~manager() {
    }

    box< vertical >::constraints::constraints(alignment const& a, margins const& m, visibility const& v) :
                                                align(a), margin(m), visible(v) {
    }

    box< horizontal >::constraints::constraints(alignment const& a, margins const& m, visibility const& v) :
                                                align(a), margin(m), visible(v) {
    }

    inline box< vertical >::box() {
    }

    inline box< horizontal >::box() {
    }

    inline manager::aptr box< vertical >::create() {
      return (manager::aptr(new box< vertical >::box()));
    }

    inline manager::aptr box< horizontal >::create() {
      return (manager::aptr(new box< horizontal >::box()));
    }

    inline void box< vertical >::clear() {
      element::clear();

      children.clear();
    }

    inline void box< horizontal >::clear() {
      element::clear();

      children.clear();
    }

    /**
     * @param[in] e a pointer to a layout element
     **/
    inline void box< vertical >::add(element* e) {
      children.push_back(children_list::value_type(e, default_constraints));
    }

    /**
     * @param[in] e a pointer to a layout element
     **/
    inline void box< horizontal >::add(element* e) {
      children_list::value_type c(e, default_constraints);
      children.push_back(c);
    }

    /**
     * @param[in] e a pointer to a layout element
     * @param[in] c the layout constraints to observe
     **/
    inline void box< vertical >::add(element* e, constraints const& c) {
      children.push_back(children_list::value_type(e, c));
    }

    /**
     * @param[in] e a pointer to a layout element
     * @param[in] c the layout constraints to observe
     **/
    inline void box< horizontal >::add(element* e, constraints const& c) {
      children.push_back(children_list::value_type(e, c));
    }

    /**
     * @param[in] e a pointer to a layout element
     * @param[in] a how the element should be aligned relative to the box
     * @param[in] m the margins of the element relative to other elements that occupy the box
     * @param[in] v whether the element is visible and has an effect on other elements that occupy the box
     **/
    inline void box< vertical >::add(element* e, alignment const& a, margins const& m, visibility const& v) {
      children.push_back(children_list::value_type(e, constraints(a, m, v)));
    }

    /**
     * @param[in] e a pointer to a layout element
     * @param[in] a how the element should be aligned relative to the box
     * @param[in] m the margins of the element relative to other elements that occupy the box
     * @param[in] v whether the element is visible and has an effect on other elements that occupy the box
     **/
    inline void box< horizontal >::add(element* e, alignment const& a, margins const& m, visibility const& v) {
      children.push_back(children_list::value_type(e, constraints(a, m, v)));
    }

    /**
     * @param[out] o the stream to which to write the result
     * \todo alignment, margins and visibility
     **/
    inline void box< vertical >::write_structure(std::ostream& o) {
      o << "<box-layout-manager id=\"" << id << "\" type=\"vertical\">";

      for (children_list::const_iterator i = children.begin(); i != children.end(); ++i) {
        (*i).first->write_structure(o);
      }

      o << "</box-layout-manager>";
    }

    /**
     * @param[out] o the stream to which to write the result
     * \todo alignment, margins and visibility
     **/
    inline void box< horizontal >::write_structure(std::ostream& o) {
      o << "<box-layout-manager id=\"" << id << "\" type=\"horizontal\">";

      for (children_list::const_iterator i = children.begin(); i != children.end(); ++i) {
        (*i).first->write_structure(o);
      }

      o << "</box-layout-manager>";
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre reader should point to a box-layout-manager element (of type horizontal)
     * \post reader points to after the associated end tag of the box
     * \todo alignment, margins and visibility
     **/
    inline void box< vertical >::read_structure(xml2pp::text_reader& r) {
      clear();

      if (!r.is_empty_element()) {
        r.read();

        while (!r.is_end_element("box-layout-manager")) {
          children.push_back(children_list::value_type(element::static_read_structure(r).release(), default_constraints));
        }
      }

      r.read();
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre reader should point to a box-layout-manager element (of type horizontal)
     * \post reader points to after the associated end tag of the box
     * \todo alignment, margins and visibility
     **/
    inline void box< horizontal >::read_structure(xml2pp::text_reader& r) {
      clear();

      if (!r.is_empty_element()) {
        r.read();

        while (!r.is_end_element("box-layout-manager")) {
          children.push_back(children_list::value_type(element::static_read_structure(r).release(), default_constraints));
        }
      }

      r.read();
    }

    /**
     * @param m the mediator object to use
     **/
    inline layout::mediator::wrapper_aptr box< vertical >::instantiate(layout::mediator* m) const {
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
    inline layout::mediator::wrapper_aptr box< horizontal >::instantiate(layout::mediator* m) const {
      layout::mediator::aptr n = m->build_horizontal_box();

      m = n.get();

      for (children_list::const_iterator i = children.begin(); i != children.end(); ++i) {
        attach(m, (*i).first->instantiate(m), dynamic_cast < const layout::constraints* > (&(*i).second));
      }

      return (n->get_data());
    }

    inline box< vertical >::~box() {
      for (children_list::const_iterator i = children.begin(); i != children.end(); ++i) {
        delete (*i).first;
      }
    }

    inline box< horizontal >::~box() {
      for (children_list::const_iterator i = children.begin(); i != children.end(); ++i) {
        delete (*i).first;
      }
    }
  }
}

#endif
