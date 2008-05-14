//  Author(s): Jeroen van der Wulp
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  LICENSE_1_0.txt)
//
/// \file tipi/detail/layout_mediator.hpp

#ifndef LAYOUT_MEDIATOR_H
#define LAYOUT_MEDIATOR_H

#include <memory>

#include <boost/function.hpp>

#include <tipi/detail/layout_mediator.hpp>

/// \cond INTERNAL_DOCS
namespace tipi {
  namespace layout {
    namespace elements {
      class label;
      class button;
      class checkbox;
      class radio_button;
      class progress_bar;
      class text_field;
    }

    class manager;
    class element;
    class properties;

    /**
     * \brief Abstract base class for interaction with a structure of connected
     * layout elements
     *
     * This class provides an interface for classes that can convert a
     * structure of connected layout elements to a (G)UI. It also provides the
     * means to update that (G)UI once it has been generated and the state of
     * the layout elements has changed.
     **/
    class mediator {
      friend class tipi::layout::manager;

      public:

        /** \internal
         * \brief Wrapper for data that needs to be passed between build and attach members
         **/
        class wrapper {
        };

        /** \brief Convenience type for use of auto pointers */
        typedef std::auto_ptr < mediator >                                           aptr;

        /** \brief Convenience type for use of auto pointers */
        typedef std::auto_ptr < wrapper >                                            wrapper_aptr;

        /** \brief Type for layout manager build functions */
        typedef std::auto_ptr < std::pair < aptr, wrapper_aptr > >                   connector_pair;

        /** \brief Function type for wrapping the attach methods */
        typedef boost::function < void (wrapper_aptr, layout::properties const*) >  attach_function;

      protected:

        /** \brief A function object provided by the layout manager that is used to attach elements */
        attach_function attach;

        /** \brief Data used by derived classes to build a GUI */
        wrapper_aptr    data;

      public:

        /** \brief Constructor */
        inline mediator();

        /** \brief Constructor */
        inline mediator(wrapper_aptr d);

        /** \brief Get the associated data */
        inline wrapper_aptr extract_data();

        /** \brief Set the attach function */
        inline void set_attach(attach_function);

        /** \brief Instantiates a vertically aligned box layout manager */
        virtual aptr build_vertical_box() = 0;

        /** \brief Instantiates a horizonally aligned box layout manager */
        virtual aptr build_horizontal_box() = 0;

        /** \brief Instantiates a label (static text) */
        virtual wrapper_aptr build(layout::elements::label const&) = 0;

        /** \brief Instantiates a label */
        virtual wrapper_aptr build(layout::elements::button const&) = 0;

        /** \brief Instantiates a single radio button */
        virtual wrapper_aptr build(layout::elements::radio_button const&) = 0;

        /** \brief Instantiates a single radio button */
        virtual wrapper_aptr build(layout::elements::checkbox const&) = 0;

        /** \brief Instantiates a progress bar */
        virtual wrapper_aptr build(layout::elements::progress_bar const&) = 0;

        /** \brief Instantiates a single line text input control */
        virtual wrapper_aptr build(layout::elements::text_field const&) = 0;

        /** \brief Destructor */
        virtual ~mediator() = 0;
    };

    inline mediator::mediator() {
    }

    inline mediator::mediator(wrapper_aptr d) : data(d) {
    }

    inline mediator::~mediator() {
    }

    inline mediator::wrapper_aptr mediator::extract_data() {
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
/// \endcond

#endif
