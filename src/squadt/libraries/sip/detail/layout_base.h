#ifndef LAYOUT_BASE_H
#define LAYOUT_BASE_H

#include <boost/shared_ptr.hpp>

namespace sip {
  namespace layout {

    /** \brief Type for element visibility */
    enum visibility {
      visible,       ///< the element is visible
      hidden,        ///< the element is not visible but still has effect on the layout
      none           ///< the element is invisible and has no effect on the layout
    };

    /** \brief Type for the margins that should be observed around the element */
    struct margins {
      unsigned short top;    ///< top margin in pixels
      unsigned short right;  ///< right margin in pixels
      unsigned short bottom; ///< bottom margin in pixels
      unsigned short left;   ///< left margin in pixels
    };

    /** \brief Abstract base class for layout elements */
    class layout_element {
      private:
        /** The margins for this default {0,0,0,0} */
        margins margin;

      public:

        /** \brief Abstract destructor */
        virtual ~layout_element() = 0;

        /** \brief Writes the state of the element to stream */
        virtual void state_write() = 0;

        /** \brief Read state of the element with a reader */
        virtual void state_read();

        /** \brief Recursively serialises the state of the object to a stream */
        virtual void element_write() = 0; 

        /** \brief Recursively builds the state of the object */
        virtual void element_read(); 
    };

    /** \brief Abstract base class for layout managers */
    class layout_manager : public layout_element {
      public:
        virtual ~layout_manager() = 0;
    };

    /** Convenience type for hiding shared pointer implementation */
    typedef boost::shared_ptr < layout_element > layout_element_ptr;

    /** Convenience type for hiding shared pointer implementation */
    typedef boost::shared_ptr < layout_manager > layout_manager_ptr;
  }
}
#endif

