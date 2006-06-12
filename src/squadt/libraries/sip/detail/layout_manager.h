#ifndef LAYOUT_MANAGER_H
#define LAYOUT_MANAGER_H

#include <utility>
#include <vector>
#include <map>
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

      /** \brief Constructor */
      inline margins(const unsigned short = 0, const unsigned short = 0, const unsigned short = 0, const unsigned short = 0);

      /** \brief Compares for equality */
      inline bool operator==(margins const&) const;

      /** \brief Compares for inequality */
      inline bool operator!=(margins const&) const;
    };

    /** \brief Type for element visibility */
    enum visibility {
      visible,       ///< the element is visible
      hidden,        ///< the element is not visible but still has effect on the layout
      none           ///< the element is invisible and has no effect on the layout
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

    /** \brief Base class for layout constraint containers */
    class constraints {
      public:
        horizontal_alignment alignment_horizontal; ///< how the element is aligned horizontally
        vertical_alignment   alignment_vertical;   ///< how the element is aligned vertically
        margins              margin;               ///< the margins that should be observed around the element
        visibility           visible;              ///< whether the element affects layout and is visible
       
        /** \brief Constructor */
        inline constraints(vertical_alignment const&, horizontal_alignment const&, margins const&, visibility const&);
       
        /** \brief Constructor, for when horizontal alignment does not matter */
        inline constraints(vertical_alignment const&, margins const&, visibility const&);
       
        /** \brief Constructor, for when vertical alignment does not matter */
        inline constraints(horizontal_alignment const&, margins const&, visibility const&);

        /** \brief Write out the layout constraint in XML format */
        void write(std::ostream&) const;

        /** \brief Read back a layout constraint, in XML format */
        void read(xml2pp::text_reader&);

        /** \brief Compares for equality */
        inline bool operator==(constraints const&) const;

        /** \brief Compares for inequality */
        inline bool operator!=(constraints const&) const;
    };

    /** \brief Abstract base class for layout managers */
    class manager : public element {
      friend class sip::layout::element;

      public:

        /** \brief Convenience type for hiding auto pointer implementation */
        typedef std::auto_ptr < manager > aptr;

      public:

        /** \brief The default margins between elements */
        static const margins     default_margins;

        /** \brief The default visibility of elements */
        static const visibility  default_visibility;

        /** \brief Default constraints */
        static const constraints default_constraints;

      protected:

        /** \brief Attaches a layout element to a manager, using layout constraints */
        inline void attach(layout::mediator*, mediator::wrapper_aptr, constraints const*) const;

      public:

        /** \brief Adds a new element to the box */
        virtual void add(element*) = 0;

        /** Adds a new element to the box */
        virtual void add(element*, margins const&,
                                  visibility const& = manager::default_visibility) = 0;

        /** Adds a new element to the box */
        virtual void add(element*, visibility const&) = 0;

        /** \brief Recursively builds the state of the object */
        static aptr static_read_structure(read_context&); 

        /** \brief Instantiate a layout element, through a mediator */
        virtual mediator::wrapper_aptr instantiate(layout::mediator*) = 0;

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
        void read_structure(read_context& r);

      public:

        /** \brief Constructor */
        inline box();

        /** Adds a new element to the box */
        inline void add(element*);

        /** Adds a new element to the box */
        inline void add(element*, constraints const&);

        /** Adds a new element to the box */
        inline void add(element*, margins const&,
                                  visibility const& = manager::default_visibility);

        /** Adds a new element to the box */
        inline void add(element*, visibility const&);

        /** \brief Instantiate a layout element, through a mediator */
        virtual mediator::wrapper_aptr instantiate(layout::mediator*) = 0;

        /** \brief Recursively traverses layout structure to find an element by its id */
        element* find(element::identifier);

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

        /** \brief Element positioning is subject only to horizontal alignment */
        typedef horizontal_alignment alignment;

      public:

        /** \brief Default alignment */
        static const alignment   default_alignment;

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
        void write_structure(std::ostream&) const;

        /** \brief Instantiate a layout element, through a mediator */
        inline mediator::wrapper_aptr instantiate(layout::mediator*);
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

        /** \brief Element positioning is subject only to vertical alignment */
        typedef vertical_alignment alignment;

      public:

        /** \brief Default alignment */
        static const alignment   default_alignment;

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
        void write_structure(std::ostream&) const;

        /** \brief Instantiate a layout element, through a mediator */
        inline mediator::wrapper_aptr instantiate(layout::mediator*);
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

    inline constraints::constraints(vertical_alignment const& av, horizontal_alignment const& ah, margins const& m, visibility const& v) :
                                                alignment_horizontal(ah), alignment_vertical(av), margin(m), visible(v) {
    }

    inline constraints::constraints(vertical_alignment const& av, margins const& m, visibility const& v) :
                                                alignment_horizontal(center), alignment_vertical(av), margin(m), visible(v) {
    }

    inline constraints::constraints(horizontal_alignment const& ah, margins const& m, visibility const& v) :
                                                alignment_horizontal(ah), alignment_vertical(middle), margin(m), visible(v) {
    }

    /**
     * @param[in] t the top margin
     * @param[in] r the right margin
     * @param[in] b the bottom margin
     * @param[in] l the left margin
     **/
    inline margins::margins(const unsigned short t, const unsigned short r, const unsigned short b, const unsigned short l) :
                                                                top(t), right(r), bottom(b), left(l) {
    }

    /**
     * @param[in] m the margins to compare agains
     **/
    inline bool margins::operator==(margins const& m) const {
      return (top == m.top && left == m.left && bottom == m.bottom && right == m.right);
    }

    /**
     * @param[in] m the margins to compare agains
     **/
    inline bool margins::operator!=(margins const& m) const {
      return (top != m.top || left != m.left || bottom != m.bottom || right != m.right);
    }

    /**
     * @param[in] c the constraints object to compare against
     **/
    inline bool constraints::operator==(constraints const& c) const {
      return (alignment_horizontal == c.alignment_horizontal && alignment_vertical == c.alignment_vertical && margin == c.margin && visible == c.visible);
    }

    /**
     * @param[in] c the constraints object to compare against
     **/
    inline bool constraints::operator!=(constraints const& c) const {
      return (alignment_horizontal != c.alignment_horizontal || alignment_vertical != c.alignment_vertical || margin != c.margin || visible != c.visible);
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
      children.push_back(children_list::value_type(e, constraints(middle, a, m, v)));
    }

    /**
     * @param[in] e a pointer to a layout element
     * @param[in] a how the element should be aligned relative to the box
     * @param[in] m the margins of the element relative to other elements that occupy the box
     * @param[in] v whether the element is visible and has an effect on other elements that occupy the box
     **/
    inline void horizontal_box::add(element* e, alignment const& a, margins const& m, visibility const& v) {
      children.push_back(children_list::value_type(e, constraints(a, center, m, v)));
    }

    /**
     * @param[in] e a pointer to a layout element
     * @param[in] m the margins of the element relative to other elements that occupy the box
     * @param[in] v whether the element is visible and has an effect on other elements that occupy the box
     **/
    inline void box::add(element* e, margins const& m, visibility const& v) {
      children.push_back(children_list::value_type(e, constraints(middle, center, m, v)));
    }

    /**
     * @param[in] e a pointer to a layout element
     * @param[in] v whether the element is visible and has an effect on other elements that occupy the box
     **/
    inline void box::add(element* e, visibility const& v) {
      children.push_back(children_list::value_type(e, constraints(middle, center, manager::default_margins, v)));
    }

    /**
     * @param m the mediator object to use
     **/
    inline layout::mediator::wrapper_aptr box::instantiate(layout::mediator::aptr m) const {
      layout::mediator* n = m.get();

      for (children_list::const_iterator i = children.begin(); i != children.end(); ++i) {
        attach(n, (*i).first->instantiate(n), dynamic_cast < const layout::constraints* > (&(*i).second));
      }

      return (n->extract_data());
    }

    /**
     * @param m the mediator object to use
     **/
    inline layout::mediator::wrapper_aptr vertical_box::instantiate(layout::mediator* m) {
      return (box::instantiate(m->build_vertical_box()));
    }

    /**
     * @param m the mediator object to use
     **/
    inline layout::mediator::wrapper_aptr horizontal_box::instantiate(layout::mediator* m) {
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
