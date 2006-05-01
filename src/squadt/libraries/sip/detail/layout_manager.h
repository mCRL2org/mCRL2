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

    /** \brief The directional alignment of layout elements perpendicular to the orientation of the box */
    enum vertical_alignment {
      left,       ///< element is put as much to the left as possible
      center,     ///< element is centered
      right       ///< element is put as much to the right as possible
    };

    /** \brief The directional alignment of layout elements perpendicular to the orientation of the box */
    enum horizontal_alignment {
      top,        ///< element is put as much to the top as possible
      middle,     ///< element is centered
      bottom      ///< element is put as much to the bottom as possible
    };

    /** \brief Base class for layout constraint containers */
    class constraints {
      public:
        horizontal_alignment align_horizontal; ///< how the element is aligned horizontally
        vertical_alignment   align_vertical;   ///< how the element is aligned vertically
        margins              margin;           ///< the margins that should be observed around the element
        visibility           visible;          ///< whether the element affects layout and is visible
       
        /** \brief Constructor */
        inline constraints(vertical_alignment const&, horizontal_alignment const&, margins const&, visibility const&);
       
        /** \brief Constructor, for when horizontal alignment does not matter */
        inline constraints(vertical_alignment const&, margins const&, visibility const&);
       
        /** \brief Constructor, for when vertical alignment does not matter */
        inline constraints(horizontal_alignment const&, margins const&, visibility const&);
    };

    /** \brief Abstract base class for layout managers */
    class manager : public element {
      friend class sip::layout::element;

      public:

        /** \brief Convenience type for hiding auto pointer implementation */
        typedef std::auto_ptr < manager > aptr;

      public:

        /** \brief The default margins between elements */
        static margins     default_margins;

        /** \brief The default visibility of elements */
        static visibility  default_visibility;

        /** \brief Default constraints */
        static constraints default_constraints;

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

    /**
     * \brief Box layout manager
     *
     * Elements are laid out horizontally or vertically, according to the chosen box variant.
     **/
    class box : public manager {

      protected:

        /** \brief The type of the list with the element managed by this manager */
        typedef std::vector < std::pair < element*, constraints > > children_list;

      protected:

        /** \brief The layout elements directly contained in this box */
        children_list children;

        /** \brief Resets private members to defaults */
        inline void clear();

        /** \brief Instantiate a layout element, through a mediator */
        inline mediator::wrapper_aptr instantiate(layout::mediator::aptr) const;

      private:

        /** \brief Read back a layout structure in XML format */
        inline void read_structure(xml2pp::text_reader& r);

      public:

        /** \brief Constructor */
        inline box();

        /** Adds a new element to the box */
        inline void add(element*);

        /** Adds a new element to the box */
        inline void add(element*, constraints const&);

        /** \brief Instantiate a layout element, through a mediator */
        virtual mediator::wrapper_aptr instantiate(layout::mediator*) const = 0;

        /** \brief Destructor */
        virtual ~box() = 0;
    };

    /**
     * \brief Vertical box layout manager
     *
     * Elements are laid out vertically
     **/
    class vertical_box : public box {
      friend class sip::layout::manager;
      friend class sip::layout::element;

      public:

        typedef vertical_alignment alignment;

      public:

        /** \brief Default alignment */
        static alignment   default_alignment;

      public:

        /** \brief Constructor */
        inline vertical_box();

        /** \brief Instantiates a layout manager and returns a shared pointer */
        inline static manager::aptr create();

        /** Adds a new element to the box */
        inline void add(element*, alignment const&,
                                  margins const& = manager::default_margins,
                                  visibility const& = manager::default_visibility);

        /** \brief Write out the layout structure in XML format */
        inline void write_structure(std::ostream&);

        /** \brief Instantiate a layout element, through a mediator */
        inline mediator::wrapper_aptr instantiate(layout::mediator*) const;
    };

    /**
     * \brief Vertical box layout manager
     *
     * Elements are laid out vertically
     **/
    class horizontal_box : public box {
      friend class sip::layout::manager;
      friend class sip::layout::element;

      public:

        typedef horizontal_alignment alignment;

      public:

        /** \brief Default alignment */
        static alignment   default_alignment;

      public:

        /** \brief Constructor */
        inline horizontal_box();

        /** \brief Instantiates a layout manager and returns a shared pointer */
        inline static manager::aptr create();

        /** Adds a new element to the box */
        inline void add(element*, alignment const&,
                                  margins const& = manager::default_margins,
                                  visibility const& = manager::default_visibility);

        /** \brief Write out the layout structure in XML format */
        inline void write_structure(std::ostream&);

        /** \brief Instantiate a layout element, through a mediator */
        inline mediator::wrapper_aptr instantiate(layout::mediator*) const;
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

    constraints::constraints(vertical_alignment const& av, horizontal_alignment const& ah, margins const& m, visibility const& v) :
                                                align_horizontal(ah), align_vertical(av), margin(m), visible(v) {
    }

    constraints::constraints(vertical_alignment const& av, margins const& m, visibility const& v) :
                                                align_horizontal(middle), align_vertical(av), margin(m), visible(v) {
    }

    constraints::constraints(horizontal_alignment const& ah, margins const& m, visibility const& v) :
                                                align_horizontal(ah), align_vertical(center), margin(m), visible(v) {
    }

    inline box::box() {
    }

    inline vertical_box::vertical_box() {
    }

    inline horizontal_box::horizontal_box() {
    }

    inline manager::aptr vertical_box::create() {
      return (manager::aptr(new vertical_box()));
    }

    inline manager::aptr horizontal_box::create() {
      return (manager::aptr(new horizontal_box()));
    }

    inline void box::clear() {
      element::clear();

      children.clear();
    }

    /**
     * @param[in] e a pointer to a layout element
     **/
    inline void box::add(element* e) {
      children.push_back(children_list::value_type(e, default_constraints));
    }

    /**
     * @param[in] e a pointer to a layout element
     * @param[in] c the layout constraints to observe
     **/
    inline void box::add(element* e, constraints const& c) {
      children.push_back(children_list::value_type(e, c));
    }

    /**
     * @param[in] e a pointer to a layout element
     * @param[in] a how the element should be aligned relative to the box
     * @param[in] m the margins of the element relative to other elements that occupy the box
     * @param[in] v whether the element is visible and has an effect on other elements that occupy the box
     **/
    inline void vertical_box::add(element* e, alignment const& a, margins const& m, visibility const& v) {
      children.push_back(children_list::value_type(e, constraints(a, m, v)));
    }

    /**
     * @param[in] e a pointer to a layout element
     * @param[in] a how the element should be aligned relative to the box
     * @param[in] m the margins of the element relative to other elements that occupy the box
     * @param[in] v whether the element is visible and has an effect on other elements that occupy the box
     **/
    inline void horizontal_box::add(element* e, alignment const& a, margins const& m, visibility const& v) {
      children.push_back(children_list::value_type(e, constraints(a, m, v)));
    }

    /**
     * @param[out] o the stream to which to write the result
     * \todo alignment, margins and visibility
     **/
    inline void vertical_box::write_structure(std::ostream& o) {
      o << "<box-layout-manager type=\"vertical\"";

      element::write_attributes(o);

      for (children_list::const_iterator i = children.begin(); i != children.end(); ++i) {
        (*i).first->write_structure(o);
      }

      o << "</box-layout-manager>";
    }

    /**
     * @param[out] o the stream to which to write the result
     * \todo alignment, margins and visibility
     **/
    inline void horizontal_box::write_structure(std::ostream& o) {
      o << "<box-layout-manager type=\"horizontal\"";
      
      element::write_attributes(o);

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
    inline void box::read_structure(xml2pp::text_reader& r) {
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
    inline layout::mediator::wrapper_aptr box::instantiate(layout::mediator::aptr m) const {
      layout::mediator* n = m.get();

      for (children_list::const_iterator i = children.begin(); i != children.end(); ++i) {
        attach(n, (*i).first->instantiate(n), dynamic_cast < const layout::constraints* > (&(*i).second));
      }

      return (n->get_data());
    }

    /**
     * @param m the mediator object to use
     **/
    inline layout::mediator::wrapper_aptr vertical_box::instantiate(layout::mediator* m) const {
      return (box::instantiate(m->build_vertical_box()));
    }

    /**
     * @param m the mediator object to use
     **/
    inline layout::mediator::wrapper_aptr horizontal_box::instantiate(layout::mediator* m) const {
      return (box::instantiate(m->build_horizontal_box()));
    }

    inline box::~box() {
      for (children_list::const_iterator i = children.begin(); i != children.end(); ++i) {
        delete (*i).first;
      }
    }
  }
}

#endif
