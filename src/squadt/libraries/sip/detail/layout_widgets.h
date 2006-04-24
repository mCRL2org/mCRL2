#ifndef LAYOUT_WIDGETS_H
#define LAYOUT_WIDGETS_H

namespace sip {
  namespace layout {

    using namespace sip::datatype;

    /** \brief A basic text label widget */
    class label : public layout_element {

      private:

        /** \brief The text to be displayed */
        std::string                     text;

      public:

        /** \brief Constructor for a label */
        inline label(std::string);

        /** \brief Instantiate a layout element, through a mediator */
        inline void instantiate(layout::mediator*);
    };

    inline label::label(std::string c) : text(c) {
    }

    /**
     * @param m the mediator object to use
     **/
    inline void label::instantiate(layout::mediator* m) {
    }

    /** \brief A basic button widget */
    class button : public layout_element {
      private:
        /** The caption */
        std::string label;

      public:

        /** \brief Constructor for a button */
        inline button(std::string);

        /** \brief Instantiate a layout element, through a mediator */
        inline void instantiate(layout::mediator*);
    };

    inline button::button(std::string c) : label(c) {
    }

    /**
     * @param m the mediator object to use
     **/
    inline void button::instantiate(layout::mediator* m) {
    }

    /**
     * \brief A basic radio button widget
     *
     * Connected radio buttons behave mutually exclusive. That is only one of
     * the connected buttons can be depressed at any time. A radio button p
     * is connected to a radio button q if and only if :
     *  - q is connected to p, or
     *  - p has a connection reference to q, or
     *  - there exists a radio button r such that :
     *    - p is connected to r and
     *    - r is connected to q
     **/
    class radio_button : public layout_element {
      private:
        /** The caption */
        std::string          label;

        /** The connection reference */
        const radio_button*  connection;

      public:

        /** \brief Constructor for a button */
        inline radio_button(std::string);

        /** \brief Alternative constructor for a button */
        inline radio_button(std::string, radio_button*);

        /** \brief Instantiate a layout element, through a mediator */
        inline void instantiate(layout::mediator*);
    };

    inline radio_button::radio_button(std::string c, radio_button* r) : label(c), connection(r) {
    }

    /**
     * @param m the mediator object to use
     **/
    inline void radio_button::instantiate(layout::mediator* m) {
    }

    /** \brief A basic button widget */
    class progress_bar : public layout_element {

      private:
        /** The minimum value */
        unsigned int minimum;

        /** The maximum value */
        unsigned int maximum;

        /** The current value */
        unsigned int current;

      public:

        /** \brief Constructor for a button */
        inline progress_bar(const unsigned int, const unsigned int, const unsigned int);

        /** \brief Instantiate a layout element, through a mediator */
        inline void instantiate(layout::mediator*);
    };

    inline progress_bar::progress_bar(const unsigned int min, const unsigned int max, const unsigned int cur)
            : minimum(min), maximum(max), current(cur) {
    }

    /**
     * @param m the mediator object to use
     **/
    inline void progress_bar::instantiate(layout::mediator* m) {
    }

    /**
     * \brief A text input field
     *
     * A datatype derived from basic_datatype can be specified for validation
     * purposes. By default any string is accepted.
     **/
    class text_field : public layout_element {
      private:
        /** Type for validation purposes */
        basic_datatype* type;

      public:

        /** Constructor */
        inline text_field(const std::string& i, basic_datatype* = &standard_string);

        /** \brief Instantiate a layout element, through a mediator */
        inline void instantiate(layout::mediator*);
    };

    /**
     * @param[in] i the initial content of the text control
     * @param[in] t the a type description object for validation purposes
     **/
    inline text_field::text_field(const std::string& i, basic_datatype* t) : type(t) {
    }

    /**
     * @param m the mediator object to use
     **/
    inline void text_field::instantiate(layout::mediator* m) {
    }
  }
}

#endif
