#include <iostream>
#include <utility>

#include <sip/detail/layout_manager.h>
#include <sip/detail/layout_elements.h>
#include <sip/detail/layout_tool_display.h>
#include <sip/detail/object.h>

#include <xml2pp/text_reader.h>

#include <sip/detail/event_handlers.h>

namespace sip {
  namespace layout {

    basic_event_handler element::global_event_handler;

    const margins       manager::default_margins;

    const visibility    manager::default_visibility = visible;

    const constraints   manager::default_constraints(middle, center, manager::default_margins, manager::default_visibility);

    /** \brief Maps alignment to a string */
    static const char* alignment_to_text[6] = {"top","middle","bottom","left","center","right"};

    /** \brief Maps visibility to a string */
    static const char* visibility_to_text[3] = {"visible","hidden","none"};

    /**
     * Blocks until the next change event or when the object is destroyed
     *
     * \return Whether there was a change, otherwise the object was destroyed
     **/
    void element::await_change() const {
      current_event_handler->await_change(this);
    }

    /**
     * @param[in] e event handler object that will dispatch events for this object
     **/
    void element::set_event_handler(basic_event_handler* e) {
      current_event_handler->transfer(*e, this);
    }

    /**
     * @param[in] identifier the identifier of the wanted element
     **/
    element* box::find(layout::element::identifier identifier) {
      element* r = (identifier == id) ? this : 0;

      for (children_list::const_iterator i = children.begin(); i != children.end() && r == 0; ++i) {
        r = (*i).first->find(identifier);
      }

      return (r);
    }

    /** \brief Finds a member of the visibility domain for a string */
    visibility text_to_visibility(std::string const& s) {
      if (s == "visible") {
        return (visible);
      }
      else if (s == "hidden") {
        return (hidden);
      }
      else { /* Assume none */
        return (none);
      }
    }

    /** \brief Finds a member of the visibility domain for a string */
    vertical_alignment text_to_vertical_alignment(std::string const& s) {
      if (s == "top") {
        return (top);
      }
      else if (s == "middle") {
        return (middle);
      }
      else { /* Assume bottom */
        return (bottom);
      }
    }

    /** \brief Finds a member of the visibility domain for a string */
    horizontal_alignment text_to_horizontal_alignment(std::string const& s) {
      if (s == "left") {
        return (left);
      }
      else if (s == "center") {
        return (center);
      }
      else { /* Assume right */
        return (right);
      }
    }

    manager::aptr manager::static_read_structure(element::read_context& r) {
      using sip::exception::exception;

      std::string name(r.reader.element_name());

      layout::manager::aptr new_element;

      if (name == "box-layout-manager") {
        std::string type;

        r.reader.get_attribute(&type, "type");

        if (type == "vertical") {
          new_element = manager::aptr(new vertical_box());
        }
        else {
          new_element = manager::aptr(new horizontal_box());
        }
      }
      else {
        throw (new exception(sip::exception::unknown_layout_element, name));
      }

      /* Read abstract element specific data */
      new_element->read(r);
      new_element->read_structure(r);

      return (new_element);
    }

    /**
     * @param[out] o the stream to which to write
     **/
    void constraints::write(std::ostream& o) const {
      o << "<layout-constraints "
        << "horizontal-alignment=\"" << alignment_to_text[alignment_horizontal]
        << "\" vertical-alignment=\"" << alignment_to_text[alignment_vertical]
        << "\" margin-top=\"" << margin.top
        << "\" margin-left=\"" << margin.left
        << "\" margin-bottom=\"" << margin.bottom
        << "\" margin-right=\"" << margin.right
        << "\" visibility=\"" << visibility_to_text[visible] << "\"/>";
    }

    /**
     * @param[in] r a xml2pp text reader
     **/
    void constraints::read(xml2pp::text_reader& r) {
      alignment_horizontal = text_to_horizontal_alignment(r.get_attribute_as_string("horizontal-alignment"));
      alignment_vertical   = text_to_vertical_alignment(r.get_attribute_as_string("vertical-alignment"));

      r.get_attribute(&margin.top, "margin-top");
      r.get_attribute(&margin.left, "margin-left");
      r.get_attribute(&margin.bottom, "margin-bottom");
      r.get_attribute(&margin.right, "margin-right");

      visible = text_to_visibility(r.get_attribute_as_string("visibility"));

      r.next_element();
    }

    /**
     * @param[out] o the stream to which to write the result
     **/
    void vertical_box::write_structure(std::ostream& o) const {
      constraints const* current_constraints = &manager::default_constraints;

      o << "<box-layout-manager type=\"vertical\"";

      element::write_attributes(o);

      o << ">";

      for (children_list::const_iterator i = children.begin(); i != children.end(); ++i) {
        if ((*i).second != *current_constraints) {
          current_constraints = &((*i).second);

          current_constraints->write(o);
        }

        (*i).first->write_structure(o);
      }

      o << "</box-layout-manager>";
    }

    /**
     * @param[out] o the stream to which to write the result
     **/
    void horizontal_box::write_structure(std::ostream& o) const {
      constraints const* current_constraints = &manager::default_constraints;

      o << "<box-layout-manager type=\"horizontal\"";
      
      element::write_attributes(o);

      o << ">";

      for (children_list::const_iterator i = children.begin(); i != children.end(); ++i) {
        if ((*i).second != *current_constraints) {
          current_constraints = &((*i).second);

          current_constraints->write(o);
        }

        (*i).first->write_structure(o);
      }

      o << "</box-layout-manager>";
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre reader should point to a box-layout-manager element (of type horizontal)
     * \post reader points to after the associated end tag of the box
     **/
    void box::read_structure(element::read_context& r) {
      constraints current_constraints = manager::default_constraints;

      clear();

      if (!r.reader.is_empty_element()) {
        r.reader.next_element();

        while (!r.reader.is_end_element("box-layout-manager")) {
          if (r.reader.is_element("layout-constraints")) {
            current_constraints.read(r.reader);
          }

          children.push_back(children_list::value_type(element::static_read_structure(r).release(), current_constraints));
        }
      }

      r.reader.next_element();
    }

    /**
     * @param[out] o the stream to which to write the result
     **/
    void tool_display::write(std::ostream& o) const {
      o << "<display-layout visible=\"" << visible << "\">";

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
    tool_display::sptr tool_display::read(xml2pp::text_reader& r) {
      tool_display::sptr display(new tool_display());

      display->visible = r.get_attribute("visible");

      r.next_element();

      if (!r.is_end_element("display-layout")) {
        element::read_context c(r);

        display->set_top_manager(layout::manager::static_read_structure(c));
      }

      return (display);
    }

    /**
     * @param[in] m a mediator to synchronise an element with the associated element in a (G)UI
     **/
    mediator::wrapper_aptr tool_display::instantiate(mediator* m) const {
      if (top_manager.get() != 0) {
        return (top_manager->instantiate(m));
      }

      return mediator::wrapper_aptr();
    }

    /**
     * @param[in] r an xml2pp text reader that should point to state descriptions
     *
     * Looks up an element by its id and calls the read_structure on this
     * member to read its new state from the text reader.
     *
     * \return vector of pointers to elements that have been updated
     **/
    void tool_display::update(xml2pp::text_reader& r, std::vector < sip::layout::element const* >& elements) {
      try {
        while (r.valid()) {
          sip::layout::element::identifier id = 0;

          r.get_attribute(&id, "id");

          /* Find the element that is to be changed */
          if (top_manager.get() == 0) {
            std::cerr << "Warning : update operation on empty layout!\n";

            break;
          }

          element* t = top_manager->find(id);

          if (t != 0) {
            sip::layout::element::read_context c(r);

            t->read_structure(c);

            elements.push_back(t);
          }
          else {
            std::string name = r.element_name();

            while (r.valid() && !r.is_end_element(name.c_str())) {
              r.next_element();
            }

            r.next_element();
          }
        }
      }
      catch (xml2pp::exception::exception* e) {
        std::cerr << e->what();
      }
    }
  }
}

