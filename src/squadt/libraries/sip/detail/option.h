#ifndef SIP_OPTION_H
#define SIP_OPTION_H

#include <vector>
#include <string>
#include <ostream>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include <sip/detail/basic_datatype.h>
#include <sip/detail/exception.h>

namespace sip {

  /** \brief Describes a single option (or option instance) the basic building block of a tool configuration */
  class option {
    friend class configuration;

    public:
      /** Datatype for the identifier of an option */
      typedef unsigned int                  identifier;

      /** \brief Convenience type to hide the shared pointer wrapping */
      typedef boost::shared_ptr < option >  ptr;

    private:

      /** \brief Type for argument to value mapping */
      typedef std::pair < datatype::basic_datatype*, std::string > type_value_pair;

      /** \brief Container for lists of arguments to lists of value mapping */
      typedef std::vector < type_value_pair >                      type_value_list;

      /** \brief List of (type, default value) */
      type_value_list arguments;

      /** \brief Must uniquely identify the option in a configuration */
      identifier id;

      /** \brief Constructor (only accessible from class configuration) */
      inline option(identifier);

    public:

      /** \brief Whether the option takes arguments */
      inline bool takes_arguments() const;

      /** \brief Returns the option's identifier */
      inline const identifier get_id() const;

      /** \brief Append to the type (option takes an additional argument of the specified type) */
      inline void append_type(datatype::basic_datatype&);

      /** \brief Append type and instance ... */
      inline void append_argument(datatype::basic_datatype&, std::string);

      /** \brief Assigns a value to the n-th argument of the option */
      inline void bind_argument(const size_t n, std::string);

      /** \brief Generate XML representation */
      inline void write(std::ostream&) const;

      /** \brief Generate XML representation */
      inline static option::ptr read(xml2pp::text_reader&);
  };

  inline option::option(identifier i) : id(i) {
  }

  inline bool option::takes_arguments() const {
    return (arguments.size() != 0);
  }

  inline const option::identifier option::get_id() const {
    return (id);
  }

  inline void option::append_type(datatype::basic_datatype& t) {
    arguments.push_back(type_value_pair(&t, ""));
  }

  inline void option::append_argument(datatype::basic_datatype& t, std::string d) {
    arguments.push_back(type_value_pair(&t, d));
  }

  inline void option::bind_argument(const size_t n, std::string s) {
    // TODO validate whether string can be interpretted as type: types[instances.size()]
    type_value_list::iterator i = arguments.begin();

    boost::next(i, n);

    assert(i != arguments.end());

    (*i).second = s;
  }

  inline void option::write(std::ostream& output) const {
    output << "<option id=\"" << id << "\"";

    if (takes_arguments()) {
            type_value_list::const_iterator i = arguments.begin();
      const type_value_list::const_iterator b = arguments.end();

      output << ">";

      while (i != b) {
        try {
          (*i).first->write(output, (*i).second);
        }
        catch (exception e) {
          /* Invalid datatype exception; substitute context */
          e.message() % boost::str(boost::format("option -> argument %u") % (i - arguments.begin()));
        }

        ++i;
      }

      output << "</option>";
    }
    else {
      output << "/>";
    }
  }

  inline option::ptr option::read(xml2pp::text_reader& r) {
    option::identifier id;

    assert(r.is_element("option"));

    r.get_attribute(&id, "id");

    option::ptr o(new option(id));

    if (!r.is_empty_element()) {
      r.read();

      while (!r.is_end_element()) {
        using namespace sip::datatype;
     
        /* The current element must be a datatype specification */
        type_value_pair new_argument;
     
        /* Set the type */
        new_argument.first = basic_datatype::read(r);

        /* The current element can be a value of the previously read type (element is optional) */
        if (r.is_element("value")) {
          r.read();

          if (!r.is_end_element()) {
            /* Read value */
            r.get_value(&new_argument.second);
     
            r.read();
          }
     
          /* Skip end tag */
          r.read();
        }
     
        o->arguments.push_back(new_argument);
      }
    }

    r.read();

    return (o);
  }
}

#endif
