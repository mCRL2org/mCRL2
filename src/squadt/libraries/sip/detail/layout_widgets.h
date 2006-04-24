#ifndef LAYOUT_WIDGETS_H
#define LAYOUT_WIDGETS_H

namespace sip {
  namespace layout {

    using namespace sip::datatype;

    /** \brief A basic text label widget */
    class label : public layout_element {
      private:
        /** The text to be displayed */
        std::string text;

      public:

        /** \brief Constructor for a label */
        inline label(std::string);
    };

    inline label::label(std::string c) : text(c) {
    }

    /** \brief A basic button widget */
    class button : public layout_element {
      private:
        /** The caption */
        std::string label;

      public:

        /** \brief Constructor for a button */
        inline button(std::string);
    };

    inline button::button(std::string c) : label(c) {
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
    };

    inline radio_button::radio_button(std::string c, radio_button* r) : label(c), connection(r) {
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
        inline progress_bar(unsigned int, unsigned int, unsigned int);
    };

    inline progress_bar::progress_bar(unsigned int min, unsigned int max, unsigned int cur)
            : minimum(min), maximum(max), current(cur) {
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
        inline text_field(basic_datatype* = &standard_string);
    };

    inline text_field::text_field(basic_datatype* t) : type(t) {
    }
  }
}

#endif
