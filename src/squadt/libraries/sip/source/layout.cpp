#include <sip/detail/layout_manager.h>
#include <sip/detail/layout_elements.h>
#include <sip/detail/object.h>

#include <xml2pp/text_reader.h>

namespace sip {
  namespace layout {

    const margins       manager::default_margins;

    const visibility    manager::default_visibility = visible;

    const constraints   manager::default_constraints(middle, center, manager::default_margins, manager::default_visibility);

    /** \brief Maps alignment to a string */
    static const char* alignment_to_text[6] = {"top","middle","bottom","left","center","right"};

    /** \brief Maps visibility to a string */
    static const char* visibility_to_text[3] = {"visible","hidden","none"};

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

    element::aptr element::static_read_structure(std::string& input) {
      xml2pp::text_reader r(input);

      r.read();

      /* Read structure data */
      return (static_read_structure(r));
    }

    element::aptr element::static_read_structure(xml2pp::text_reader& r) {
      using namespace sip::layout::elements;

      std::string name(r.element_name());

      layout::element::aptr new_element;

      if (name == "label") {
        new_element = element::aptr(new label());
      }
      else if (name == "button") {
        new_element = element::aptr(new button());
      }
      else if (name == "radio-button") {
        new_element = element::aptr(new radio_button());
      }
      else if (name == "progress-bar") {
        new_element = element::aptr(new progress_bar());
      }
      else if (name == "text-field") {
        new_element = element::aptr(new text_field());
      }

      if (new_element.get() != 0) {
        /* Read abstract element specific data */
        new_element->read(r);
        new_element->read_structure(r);
      }
      else { /* Assume layout manager */
        new_element = manager::static_read_structure(r);
      }

      return (new_element);
    }

    manager::aptr manager::static_read_structure(xml2pp::text_reader& r) {
      std::string name(r.element_name());

      layout::manager::aptr new_element;

      if (name == "box-layout-manager") {
        std::string type;

        r.get_attribute(&type, "type");

        if (type == "vertical") {
          new_element = manager::aptr(new vertical_box());
        }
        else {
          new_element = manager::aptr(new horizontal_box());
        }
      }
      else {
        throw (new sip::exception(exception_identifier::unknown_layout_element, name));
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
      alignment_horizontal = text_to_horizontal_alignment(r.get_attribute_string_value("horizontal-alignment"));
      alignment_vertical   = text_to_vertical_alignment(r.get_attribute_string_value("vertical-alignment"));

      r.get_attribute(&margin.top, "margin-top");
      r.get_attribute(&margin.left, "margin-left");
      r.get_attribute(&margin.bottom, "margin-bottom");
      r.get_attribute(&margin.right, "margin-right");

      visible = text_to_visibility(r.get_attribute_string_value("visibility"));

      r.read();
    }

    /**
     * @param[out] o the stream to which to write the result
     **/
    void vertical_box::write_structure(std::ostream& o) {
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
    void horizontal_box::write_structure(std::ostream& o) {
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
    void box::read_structure(xml2pp::text_reader& r) {
      constraints current_constraints = manager::default_constraints;

      clear();

      if (!r.is_empty_element()) {
        r.read();

        while (!r.is_end_element("box-layout-manager")) {
          if (r.is_element("layout-constraints")) {
            current_constraints.read(r);
          }

          children.push_back(children_list::value_type(element::static_read_structure(r).release(), current_constraints));
        }
      }

      r.read();
    }
  }
}

