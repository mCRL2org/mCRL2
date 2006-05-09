#ifndef LAYOUT_MEDIATOR_H
#define LAYOUT_MEDIATOR_H

#include <memory>

#include <boost/function.hpp>

namespace sip {
  namespace layout {

    class manager;
    class element;
    class constraints;

    /** \brief Abstract base class for interaction with a layout element */
    class mediator {
      friend class sip::layout::manager;

      public:

        /** \brief Wrapper for data that needs to be passed between build and attach members */
        class wrapper {
        };

        /** \brief Convenience type for use of auto pointers */
        typedef std::auto_ptr < mediator >                                           aptr;

        /** \brief Convenience type for use of auto pointers */
        typedef std::auto_ptr < wrapper >                                            wrapper_aptr;

        /** \brief Type for layout manager build functions */
        typedef std::auto_ptr < std::pair < aptr, wrapper_aptr > >                   connector_pair;

        /** \brief Function type for wrapping the attach methods */
        typedef boost::function < void (wrapper_aptr, layout::constraints const*) >  attach_function;

      protected:

        /** \brief A function object provided by the layout manager that is used to attach elements */
        attach_function attach;

        /** \brief Data used by derived classes to build a GUI */
        wrapper_aptr    data;

      public:

        /** \brief Constructor */
        inline mediator(wrapper_aptr d);

        /** \brief Get the associated data */
        inline wrapper_aptr get_data();

        /** \brief Set the attach function */
        inline void set_attach(attach_function);

        /** \brief Instantiates a vertically aligned box layout manager */
        virtual aptr build_vertical_box() = 0;

        /** \brief Instantiates a horizonally aligned box layout manager */
        virtual aptr build_horizontal_box() = 0;

        /** \brief Instantiates a label (static text) */
        virtual wrapper_aptr build_label(layout::element*, std::string const&) = 0;

        /** \brief Instantiates a label */
        virtual wrapper_aptr build_button(layout::element*, std::string const&) = 0;

        /** \brief Instantiates a single radio button */
        virtual wrapper_aptr build_radio_button(layout::element*, std::string const&) = 0;

        /** \brief Instantiates a single radio button */
        virtual wrapper_aptr build_checkbox(layout::element*, std::string const&, const bool status) = 0;

        /** \brief Instantiates a progress bar */
        virtual wrapper_aptr build_progress_bar(layout::element*, unsigned int const&, unsigned int const&, unsigned int const&) = 0;

        /** \brief Instantiates a single line text input control */
        virtual wrapper_aptr build_text_field(layout::element*, std::string const&) = 0;

        /** \brief Destructor */
        virtual ~mediator() = 0;
    };

    inline mediator::mediator(wrapper_aptr d) : data(d) {
    }

    inline mediator::~mediator() {
    }

    inline mediator::wrapper_aptr mediator::get_data() {
      return (data);
    }

    /**
     * @param[in] h the function to set
     **/
    inline void mediator::set_attach(attach_function h) {
      attach = h;
    }
  }
}

#endif
