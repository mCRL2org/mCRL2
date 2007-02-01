#ifndef SIP_REPORT_H
#define SIP_REPORT_H

#include <string>

#include <boost/shared_ptr.hpp>

#include <utility/visitor.h>

#include <sip/configuration.h>

namespace sip {

  class report;

  /** \brief Describes a report of tool operation */
  class report : public utility::visitable < report > {
    friend class sip::store_visitor_impl;
    friend class sip::restore_visitor_impl;

    public:

      /** \brief Type alias for convenience */
      typedef boost::shared_ptr < report > sptr;

      /** \brief The message class */
      enum type {
        notice,  //< like a warning but even less urgent
        warning, //< warning of some kind
        error    //< fatal error occurred
      };

    private:

      /** \brief The type of the report */
      type        m_report_type;

      /** \brief Room for errors (any error here implies unsuccessful termination) */
      std::string description;

    public:

      /** \brief Constructor */
      inline report(type const& = notice, std::string const& = "");

      /** \brief Gets the description */
      inline std::string get_description();
  };

  /**
   * \param[in] t the type of the report
   * \param[in] d a description
   **/
  inline report::report(type const& t, std::string const& d) : m_report_type(t), description(d) {
  }

  inline std::string report::get_description() {
    return (description);
  }
}

#endif
