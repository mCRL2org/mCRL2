#ifndef SIP_OPTION_H
#define SIP_OPTION_H

#include <vector>
#include <string>
#include <ostream>

#include <boost/any.hpp>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include <sip/detail/basic_datatype.h>
#include <sip/detail/exception.h>

namespace sip {

  /** \brief Describes a single option (or option instance) the basic building block of a tool configuration */
  class option {
    friend class configuration;

    private:

      /** \brief Type for argument to value mapping */
      typedef std::pair < datatype::basic_datatype::sptr, std::string > type_value_pair;

      /** \brief Container for lists of arguments to lists of value mapping */
      typedef std::vector < type_value_pair >                           type_value_list;

    public:

      /** Datatype for the identifier of an option */
      typedef unsigned int                  identifier;

      /** \brief Convenience type to hide the shared pointer wrapping */
      typedef boost::shared_ptr < option >  sptr;

      /** \brief Iterator over the argument values */
      class argument_iterator {

        private:

          /** \brief the start of the sequence */
                type_value_list::const_iterator iterator;

          /** \brief the end of the sequence */
          const type_value_list::const_iterator end;

        public:

          /** \brief Constructor */
          inline argument_iterator(type_value_list::const_iterator, type_value_list::const_iterator);

          /** \brief Whether the iterator has moved past the end of the sequence */
          inline bool valid() const;

          /** \brief Advances to the next element */
          inline void operator++();

          /** \brief Returs a functor that, when invoked, returns a value */
          inline boost::any operator*() const;
      };

    private:

      /** \brief List of (type, default value) */
      type_value_list arguments;

      /** \brief Must uniquely identify the option in a configuration */
      identifier id;

    private:

      /** \brief Constructor (only accessible from class configuration) */
      inline option(const identifier);

    public:

      /** \brief Whether the option takes arguments */
      inline bool takes_arguments() const;

      /** \brief Returns the option's identifier */
      inline const identifier get_id() const;

      /** \brief Gets an iterator that in order of appearance returns the values for each argument */
      inline argument_iterator get_value_iterator() const;

      /** \brief Append to the type (option takes an additional argument of the specified type) */
      inline void append_type(datatype::basic_datatype::sptr);

      /** \brief Append to the type (option takes an additional argument of the specified type) */
      inline void append_type(datatype::basic_datatype::sptr&);

      /** \brief Append type and instance ... */
      template < typename T >
      inline void append_argument(datatype::basic_datatype::sptr, T);

      /** \brief Assigns a value to the n-th argument of the option */
      inline void bind_argument(const size_t n, std::string);

      /** \brief Generate XML representation */
      inline void write(std::ostream&) const;

      /** \brief Generate XML representation */
      inline static option::sptr read(xml2pp::text_reader&);
  };

  /**
   * @param b the iterator from which to start
   * @param e the iterator with which to end
   **/
  inline option::argument_iterator::argument_iterator
          (type_value_list::const_iterator b, type_value_list::const_iterator e) : iterator(b), end(e) {
  }

  inline bool option::argument_iterator::valid() const {
    return (iterator != end);
  }

  inline void option::argument_iterator::operator++() {
    ++iterator;
  }

  inline boost::any option::argument_iterator::operator*() const {
    sip::datatype::basic_datatype* p = (*iterator).first.get();

    return (p->evaluate((*iterator).second));
  }

  inline option::option(const identifier i) : id(i) {
  }

  inline bool option::takes_arguments() const {
    return (arguments.size() != 0);
  }

  inline const option::identifier option::get_id() const {
    return (id);
  }

  inline option::argument_iterator option::get_value_iterator() const {
    return (argument_iterator(arguments.begin(), arguments.end()));
  }

  inline void option::append_type(datatype::basic_datatype::sptr& t) {
    assert(t.get() != 0);

    arguments.push_back(std::make_pair(t, ""));
  }

  inline void option::append_type(datatype::basic_datatype::sptr t) {
    assert(t.get() != 0);

    arguments.push_back(std::make_pair(t, ""));
  }

  template < typename T >
  inline void option::append_argument(datatype::basic_datatype::sptr t, T d) {
    assert(t.get() != 0);

    arguments.push_back(std::make_pair(t, t.get()->convert(d)));
  }

  /**
   * @param[in] t pointer to the data type definition
   * @param[in] d data that is valid w.r.t. the data type
   **/
  template < >
  inline void option::append_argument(datatype::basic_datatype::sptr t, std::string d) {
    assert(t.get() != 0);

    arguments.push_back(std::make_pair(t, d));
  }

  inline void option::bind_argument(const size_t n, std::string s) {
    // TODO validate whether string can be interpreted as type: types[instances.size()]
    type_value_list::iterator i = arguments.begin();

    boost::next(i, n);

    assert(i != arguments.end());

    (*i).second = s;
  }

  inline void option::write(std::ostream& output) const {
    using sip::exception;

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

  inline option::sptr option::read(xml2pp::text_reader& r) {
    using sip::exception;

    option::identifier id = 0;

    assert(r.is_element("option"));

    if (!r.get_attribute(&id, "id")) {
      throw (exception(sip::message_missing_required_attribute, "id", "option"));
    }
    else {
      option::sptr o(new option(id));

      if (!r.is_empty_element()) {
        r.next_element();
     
        while (!r.is_end_element()) {
          using namespace sip::datatype;
       
          /* The current element must be a datatype specification */
          o->arguments.push_back(basic_datatype::read(r));
        }
      }

      r.next_element();

      return (o);
    }
  }
}

#endif
