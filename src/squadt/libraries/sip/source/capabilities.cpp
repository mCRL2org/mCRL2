#include <ostream>
#include <sstream>
#include <utility>

#include <boost/bind.hpp>

#include <sip/controller/capabilities.h>
#include <sip/tool/capabilities.h>
#include <sip/visitors.h>

namespace sip {
  namespace controller {
     /**
     * \brief Operator for writing to stream
     *
     * \param[in,out] s stream to write to
     * \param[in] c the capabilities object to write out
     **/
    inline std::ostream& operator << (std::ostream& s, controller::capabilities const& c) {
      visitors::store(c, s);
 
      return (s);
    }
  }
  namespace tool {
    /**
     * \brief Operator for writing to stream
     *
     * \param[in,out] s stream to write to
     * \param[in] t the capabilities object to write out
     **/
    std::ostream& operator << (std::ostream& s, tool::capabilities const& t) {
      visitors::store(t, s);
 
      return (s);
    }

    capabilities::capabilities(const version v) : m_protocol_version(v) {
    }
 
    /**
     * \param[in] id a unique identifier for the input object
     * \param[in] f mime-type for the object
     * \param[in] c category to which the functionality of the tool must be counted
     **/
    void capabilities::add_input_combination(std::string const& id, mime_type const& f, tool::category const& c) {
      input_combination ic(c, f, id);
 
      m_input_combinations.insert(ic);
    }
 
    /**
     * \param[in] id a unique identifier for the output object
     * \param[in] f mime-type for the object
     * \param[in] c category to which the functionality of the tool must be counted
     **/
    void capabilities::add_output_combination(std::string const& id, mime_type const& f) {
      output_combination oc(f, id);
 
      m_output_combinations.insert(oc);
    }
 
    version capabilities::get_version() const {
      return (m_protocol_version);
    }
 
    capabilities::input_combination_range capabilities::get_input_combinations() const {
      return (boost::make_iterator_range(m_input_combinations));
    }
 
    capabilities::output_combination_range capabilities::get_output_combinations() const {
      return (boost::make_iterator_range(m_output_combinations));
    }

    /**
     * \param[in] f the storage format
     * \param[in] t the category in which the tool operates
     **/
    capabilities::input_combination const*
              capabilities::find_input_combination(const mime_type& f, const tool::category& t) const {
 
      input_combination p(t, f, "");

      input_combination_list::const_iterator i = std::find_if(m_input_combinations.begin(),
                      m_input_combinations.end(), boost::bind(&input_combination::equal, _1, p));

      if (i == m_input_combinations.end()) {
        return (0);
      }
      else {
        return (&(*i));
      }
    }
  }
}
