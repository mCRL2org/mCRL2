#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/convenience.hpp>

#include <sip/configuration.h>
#include <sip/visitors.h>

namespace sip {

  configuration::const_iterator_option_range configuration::get_options() const {
    return (boost::make_iterator_range(
                boost::make_filter_iterator(type_filter_predicate(boost::bind(&configuration::is_option, this, _1)),
                        boost::make_indirect_iterator(m_positions.begin()),
                        boost::make_indirect_iterator(m_positions.end())),
                        const_option_iterator(boost::make_indirect_iterator(m_positions.end()),
                                              boost::make_indirect_iterator(m_positions.end()))));
  }

  configuration::iterator_option_range configuration::get_options() {
    return (boost::make_iterator_range(
                boost::make_filter_iterator(type_filter_predicate(boost::bind(&configuration::is_option, this, _1)),
                        boost::make_indirect_iterator(m_positions.begin()),
                        boost::make_indirect_iterator(m_positions.end())),
                        option_iterator(boost::make_indirect_iterator(m_positions.end()),
                                        boost::make_indirect_iterator(m_positions.end()))));
  }

  configuration::const_iterator_input_range configuration::get_input_objects() const {
    return (boost::make_iterator_range(
                boost::make_filter_iterator(type_filter_predicate(boost::bind(&configuration::is_input, this, _1)),
                        boost::make_indirect_iterator(m_positions.begin()),
                        boost::make_indirect_iterator(m_positions.end())),
                        const_input_iterator(boost::make_indirect_iterator(m_positions.end()),
                                             boost::make_indirect_iterator(m_positions.end()))));
  }

  configuration::iterator_input_range configuration::get_input_objects() {
    return (boost::make_iterator_range(
                boost::make_filter_iterator(type_filter_predicate(boost::bind(&configuration::is_input, this, _1)),
                        boost::make_indirect_iterator(m_positions.begin()),
                        boost::make_indirect_iterator(m_positions.end())),
                        input_iterator(boost::make_indirect_iterator(m_positions.end()),
                                       boost::make_indirect_iterator(m_positions.end()))));
  }

  configuration::const_iterator_output_range configuration::get_output_objects() const {
    return (boost::make_iterator_range(
                const_output_iterator(type_filter_predicate(boost::bind(&configuration::is_output, this, _1)),
                        boost::make_indirect_iterator(m_positions.begin()),
                        boost::make_indirect_iterator(m_positions.end())),
                        const_output_iterator(boost::make_indirect_iterator(m_positions.end()),
                                              boost::make_indirect_iterator(m_positions.end()))));
  }

  configuration::iterator_output_range configuration::get_output_objects() {
    return (boost::make_iterator_range(
                output_iterator(type_filter_predicate(boost::bind(&configuration::is_output, this, _1)),
                        boost::make_indirect_iterator(m_positions.begin()),
                        boost::make_indirect_iterator(m_positions.end())),
                        output_iterator(boost::make_indirect_iterator(m_positions.end()),
                                              boost::make_indirect_iterator(m_positions.end()))));
  }

  /**
   * \param[in] o the parameter for which to find the identifier
   *
   * Throws an exception when the option is not part of the configuration
   **/
  std::string configuration::get_identifier(parameter const& o) const {
    position_list::const_iterator i = std::find_if(m_positions.begin(), m_positions.end(),
                boost::bind(std::equal_to< parameter const* >(), &o, boost::bind(&position_list::value_type::get, _1)));

    if (i == m_positions.end()) {
      throw (false);
    }

    id_parameter_map::const_iterator j = std::find_if(m_parameter_by_id.begin(), m_parameter_by_id.end(),
            boost::bind(std::equal_to< size_t >(), i - m_positions.begin(), boost::bind(&id_parameter_map::value_type::second, _1)));
    
    if (j == m_parameter_by_id.end()) {
      throw (false);
    }

    return ((*j).first);
  }

  /**
   * \param[in] o the sip::option object for which to find the identifier
   *
   * Throws an exception when the option is not part of the configuration
   **/
  std::string configuration::get_identifier(sip::option const& o) const {
    return (get_identifier(static_cast < parameter const& > (o)));
  }

  /**
   * \param[in] o the sip::object object for which to find the identifier
   *
   * Throws an exception when the object is not part of the configuration
   **/
  std::string configuration::get_identifier(sip::object const& o) const {
    return (get_identifier(static_cast < parameter const& > (o)));
  }

  /**
   * \brief Operator for writing to stream
   *
   * \param s stream to write to
   * \param c the configuration object to write out
   **/
  std::ostream& operator << (std::ostream& s, const configuration& c) {
    visitors::store(c, s);

    return (s);
  }

  /**
   * \param[in] n suffix of the name
   **/
  std::string configuration::get_input_name(std::string const& n) const {
    for (position_list_partition::const_iterator i = m_input_objects.begin(); i != m_input_objects.end(); ++i) {
      return(boost::filesystem::basename(boost::filesystem::path((static_cast< const object* > (*i))->get_location())) + n);
    }

    return (m_output_prefix + n);
  }

  /**
   * \param[in] n suffix of the name
   **/
  std::string configuration::get_output_name(std::string const& n) const {
    return (m_output_prefix + n);
  }

  /**
   * \param id an identifier for the option
   * \param r whether or not to replace an existing option with the same id
   **/
  option& configuration::add_option(std::string const& id, bool r) {
    assert(m_parameter_by_id.count(id) == 0);

    boost::shared_ptr < option > new_option(new sip::option);

    if (m_parameter_by_id.count(id) == 0) {
      m_parameter_by_id[id] = m_positions.size();
      m_positions.push_back(new_option);
      m_options.insert(m_positions.back().get());
    }
    else if (r) {
      m_positions[m_parameter_by_id[id]] = new_option;
    }

    return (*new_option);
  }

  /**
   * \param id an identifier for the option
   * \param o the option object to add
   * \param r whether or not to replace an existing option with the same id
   **/
  option& configuration::add_option(std::string const& id, boost::shared_ptr < option >& o, bool r) {
    assert(m_parameter_by_id.count(id) == 0);

    if (m_parameter_by_id.count(id) == 0) {
      m_parameter_by_id[id] = m_positions.size();
      m_positions.push_back(o);
      m_options.insert(m_positions.back().get());
    }
    else if (r) {
      m_positions[m_parameter_by_id[id]] = o;
    }

    return (*o);
  }

  /**
   * \param id an identifier for the option
   **/
  void configuration::remove_option(std::string const& id) {
    if (m_parameter_by_id.count(id) != 0) {
      size_t position = m_parameter_by_id[id];

      m_options.erase(m_positions[position].get());
      m_positions.erase(m_positions.begin() + position);
      m_parameter_by_id.erase(id);
    }
  }

  /**
   * \param[in] id position in the list of output objects
   **/
  inline void configuration::remove_input(std::string const& id) {
    if (0 < m_parameter_by_id.count(id)) {
      size_t position = m_parameter_by_id[id];

      m_input_objects.erase(m_positions[position].get());
      m_positions.erase(m_positions.begin() + position);
      m_parameter_by_id.erase(id);
    }
  }

  /**
   * \param[in] p position in the list of input objects
   **/
  void configuration::remove_input(size_t p) {
    for (position_list::iterator i = m_positions.begin(); i != m_positions.end(); ++i) {
      if (m_input_objects.count((*i).get()) != 0) {
        if (--p == 0) {
          m_input_objects.erase((*i).get());

          m_parameter_by_id.erase(std::find_if(m_parameter_by_id.begin(), m_parameter_by_id.end(),
             boost::bind(std::equal_to< id_parameter_map::value_type::second_type >(),
                boost::bind(&id_parameter_map::value_type::second, _1), i - m_positions.begin())));

          m_positions.erase(i);
        }
      }
    }
  }

  /**
   * \param[in] id position in the list of output objects
   **/
  inline void configuration::remove_output(std::string const& id) {
    if (0 < m_parameter_by_id.count(id)) {
      size_t position = m_parameter_by_id[id];

      m_output_objects.erase(m_positions[position].get());
      m_positions.erase(m_positions.begin() + position);
      m_parameter_by_id.erase(id);
    }
  }

  /**
   * \param[in] p position in the list of output objects
   **/
  void configuration::remove_output(size_t p) {
    for (position_list::iterator i = m_positions.begin(); i != m_positions.end(); ++i) {
      if (m_output_objects.count((*i).get()) != 0) {
        if (--p == 0) {
          m_output_objects.erase((*i).get());

          m_parameter_by_id.erase(std::find_if(m_parameter_by_id.begin(), m_parameter_by_id.end(),
             boost::bind(std::equal_to< id_parameter_map::value_type::second_type >(),
                boost::bind(&id_parameter_map::value_type::second, _1), i - m_positions.begin())));

          m_positions.erase(i);
        }
      }
    }
  }

  /**
   * \param[in] id an identifier for the option
   * \pre option with this identifier must be part of the configuration, use
   *      option_exists member to establish this
   **/
  option const& configuration::get_option(std::string const& id) const {
    assert(m_parameter_by_id.count(id) != 0);

    return (* boost::static_pointer_cast < const option > (
        const_cast < position_list& > (m_positions)[(*m_parameter_by_id.find(id)).second]));
  }

  /**
   * \param[in] id an identifier for the option
   * \pre option with this identifier must be part of the configuration, use
   *      option_exists member to establish this
   **/
  option& configuration::get_option(std::string const& id) {
    assert(m_parameter_by_id.count(id) != 0);

    return (*boost::static_pointer_cast < option > (m_positions[(*m_parameter_by_id.find(id)).second]));
  }

  /**
   * \param[in] id an identifier for the option
   * \param[in] n an optional identifier for the option
   * \pre option must take at least one argument and n must be smaller than the number of arguments
   * \pre option with this identifier must be part of the configuration, use
   *      option_exists member to establish this
   **/
  boost::any configuration::get_option_argument(std::string const& id, size_t const& n) const {
    assert(m_parameter_by_id.count(id) != 0);

    return (boost::static_pointer_cast < const option > (
        const_cast < position_list& > (m_positions)[(*m_parameter_by_id.find(id)).second])->get_value(n));
  }
}
