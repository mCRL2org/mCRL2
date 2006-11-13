#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/convenience.hpp>

#include <sip/configuration.h>

namespace sip {

  /** The optional constraint, option is either not present or only present once */
  const configuration::option_constraint configuration::constrain_optional = {0,1};

  /** The required constraint, option (with possible arguments) must be present */
  const configuration::option_constraint configuration::constrain_required = {1,1};

  /**
   * \brief Operator for writing to stream
   *
   * @param s stream to write to
   * @param c the configuration object to write out
   **/
  std::ostream& operator << (std::ostream& s, const configuration& c) {
    c.write(s);

    return (s);
  }

  /**
   * @param id an identifier for the option
   * @param r whether or not to replace an existing option with the same id
   **/
  option& configuration::add_option(const option::identifier id, bool r) {
    using namespace std;
    using namespace boost;

    assert(r || !option_exists(id));

    option::sptr o;
    
    if (option_exists(id)) {
      o = get_option(id);
    }
    else {
      o.reset(new option(id));

      options[o] = constrain_optional;
    }

    return (*o);
  }

  /**
   * @param id an identifier for the option
   **/
  bool configuration::option_exists(const option::identifier id) const {
    using namespace std;
    using namespace boost;

    return (find_if(options.begin(), options.end(), bind(equal_to < option::identifier >(),
                    bind(&option::get_id,
                            bind(&option::sptr::get,
                                    bind(&option_list::value_type::first, _1))),id)) != options.end());
  }

  /**
   * @param id an identifier for the option
   **/
  void configuration::remove_option(const option::identifier id) {
    using namespace std;
    using namespace boost;

    option_list::iterator i = find_if(options.begin(), options.end(),
                    bind(equal_to < option::identifier >(),
                            bind(&option::get_id,
                                    bind(&option::sptr::get,
                                            bind(&option_list::value_type::first, _1))), id));

    assert(i != options.end());

    options.erase(i);
  }

  /**
   * @param[in] n suffix of the name
   **/
  std::string configuration::get_input_name(std::string const& n) {
    for (object_list::iterator i = objects.begin(); i != objects.end(); ++i) {
      if ((*i)->get_type() == object::input) {
        using namespace boost::filesystem;

        return(boost::filesystem::basename(path((*i)->get_location())) + n);
      }
    }

    return (output_prefix + n);
  }

  /**
   * @param id an identifier for the option
   **/
  option::sptr configuration::get_option(const option::identifier id) const {
    using namespace std;
    using namespace boost;

    option_list::const_iterator i = find_if(options.begin(), options.end(),
                    bind(equal_to < option::identifier >(),
                            bind(&option::get_id,
                                    bind(&option::sptr::get,
                                            bind(&option_list::value_type::first, _1))), id));

    assert(i != options.end());

    return ((*i).first);
  }

  /**
   * @param out the stream to which the output is written
   **/
  void configuration::write(std::ostream& out) const {
    out << "<configuration";

    if (fresh) {
      out << " fresh=\"true\"";
    }

    if (!output_prefix.empty()) {
      out << " output-prefix=\"" << output_prefix << "\"";
    }

    /* Add input combination */
    out << " category=\"" << category << "\">";

    for (option_list::const_iterator i = options.begin(); i != options.end(); ++i) {
        (*i).first->write(out);
    }

    for (object_list::const_iterator i = objects.begin(); i != objects.end(); ++i) {
        (*i)->write(out);
    }

    out << "</configuration>";
  }

  /**
   * @param id a unique identifier for the object
   * @param f the storage format the object uses
   * @param l the location for the object (optional)
   * @param t the object type
   **/
  void configuration::add_object(const object::identifier id, object::storage_format f, object::type t, object::uri l) {
    using namespace std;
    using namespace boost;

    assert(find_if(objects.begin(), objects.end(), bind(equal_to < object::identifier >(),
                    bind(&object::get_id,
                            bind(&object::sptr::get,_1)),id)) == objects.end());

    objects.push_back(object::sptr(new object(id, f, l, t)));
  }

  /**
   * @param o a pointer to an existing object
   **/
  void configuration::add_object(object::sptr o) {
    using namespace std;
    using namespace boost;

    assert(find_if(objects.begin(), objects.end(), bind(equal_to < object::identifier >(),
                    bind(&object::get_id,
                            bind(&object::sptr::get,_1)),o->get_id())) == objects.end());

    objects.push_back(o);
  }

  /**
   * @param id an identifier for the object
   **/
  void configuration::remove_object(const object::identifier id) {
    using namespace std;
    using namespace boost;

    object_list::iterator i = find_if(objects.begin(), objects.end(),
                    bind(equal_to < object::identifier >(),
                            bind(&object::get_id,
                                    bind(&object::sptr::get,_1)), id));

    assert(i != objects.end());

    objects.erase(i);
  }

  /**
   * @param id a unique identifier for the object
   **/
  bool configuration::object_exists(const object::identifier id) const {
    using namespace std;
    using namespace boost;

    return (find_if(objects.begin(), objects.end(), bind(equal_to < object::identifier >(),
                    bind(&object::get_id,
                            bind(&object::sptr::get,_1)),id)) != objects.end());
  }

  /**
   * @param id an identifier for the object
   **/
  object::sptr const configuration::get_object(const object::identifier id) const {
    using namespace std;
    using namespace boost;

    object::sptr o;

    BOOST_FOREACH(object::sptr const i, objects) {
      if (i->get_id() == id) {
        o = i;

        break;
      }
    }

    return (o);
  }

  /**
   * @param reader is a reference to a libXML 2 text reader instance
   * /pre the reader points to a \<configuration\> instance
   * /post the readers position is just past the configuration block
   **/
  configuration::sptr configuration::read(xml2pp::text_reader& reader) {
    configuration::sptr c(new configuration);

    assert(reader.is_element("configuration"));

    c->fresh         = reader.get_attribute("fresh");
    c->output_prefix = reader.get_attribute_as_string("output-prefix");

    reader.get_attribute(&c->category, "category");

    reader.next_element();

    while (!(reader.is_end_element() && reader.is_element("configuration"))) {
      /* Current element must be <option> */
      if (reader.is_element("option")) {
        c->options[option::read(reader)] = constrain_optional;
      }
      else if (reader.is_element("object")) {
        c->objects.push_back(object::read(reader));
      }
    }

    reader.skip_end_element("configuration");

    return (c);
  }
}
