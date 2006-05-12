#ifndef LAYOUT_TOOL_DISPLAY
#define LAYOUT_TOOL_DISPLAY

#include <iosfwd>

#include "sip/detail/basic_datatype.h"
#include "sip/detail/layout_base.h"
#include "sip/detail/layout_manager.h"
#include "sip/detail/layout_elements.h"

namespace sip {
  namespace layout {

    /** \brief Basic container class for controller-side layout definitions */
    class tool_display {
      public:

        /** \brief Convenience type alias to hide the shared pointer implementation */
        typedef boost::shared_ptr < tool_display >          sptr;

        /** \brief Convenience type for passing and returning a sequence of layout elements */
        typedef std::vector < sip::layout::element const* > constant_elements;

      private:

        /** \brief the layout manager that contains all widgits for this display */
        layout::manager::aptr top_manager;

      public:

        /** \brief Get the layout manager that contains all widgits for this display */
        inline layout::manager const* get_top_manager() const;

        /** \brief Set the layout manager that contains all widgits for this display */
        inline void set_top_manager(layout::manager::aptr);

        /** \brief Write out the layout structure in XML format */
        std::string write() const;

        /** \brief Write out the layout structure in XML format */
        void write(std::ostream&) const;

        /** \brief Read the layout structure from an xml2pp text reader */
        static tool_display::sptr read(xml2pp::text_reader&);

        /** \brief Creates a (G)UI for this tool_display */
        mediator::wrapper_aptr instantiate(mediator* m) const;

        /** \brief Update (part of) the layout structure based on data read from an xml2pp text reader */
        constant_elements update(xml2pp::text_reader&);
    };

    inline layout::manager const* tool_display::get_top_manager() const {
      return (top_manager.get());
    }

    inline void tool_display::set_top_manager(layout::manager::aptr m) {
      top_manager = m;
    }
    
    inline std::string tool_display::write() const {
      std::ostringstream output;
 
      write(output);

      return (output.str());
    }

    /**
     * @param[out] o the stream to which to write the result
     **/
    inline void tool_display::write(std::ostream& o) const {
      o << "<display-layout>";

      if (top_manager.get() != 0) {
        top_manager->write_structure(o);
      }

      o << "</display-layout>";
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre reader should point to a display-layout element
     * \post reader points to after the associated end tag of the box
     **/
    inline tool_display::sptr tool_display::read(xml2pp::text_reader& r) {
      tool_display::sptr display(new tool_display());

      r.read();

      if (!r.is_end_element("display-layout")) {
        display->set_top_manager(layout::manager::static_read_structure(r));
      }

      return (display);
    }
  }
}

#endif
