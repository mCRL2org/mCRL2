#ifndef SIP_CONTROLLER_CAPABILITIES_H
#define SIP_CONTROLLER_CAPABILITIES_H

#include <ostream>
#include <sstream>

#include <sip/detail/common.h>

#include <utility/visitor.h>

namespace sip {
  namespace tool {
    class communicator;
  }

  namespace controller {

    class communicator;
    class capabilities;

    /**
     * \brief Describes some controller capabilities (e.g. supported protocol version)
     *
     * Objects of this type contain information about the capabilities of a controller:
     *
     *  - the amount of display space that is reserved for a tool (that makes a request)
     *  - what version of the protocol the controller uses
     *  - ...
     *
     * As well as any information about the controller that might be interesting
     * for a tool developer.
     **/
    class capabilities : public utility::visitable < controller::capabilities > {
      friend class sip::tool::communicator;
      friend class sip::controller::communicator;
      friend class sip::restore_visitor_impl;
      friend class sip::store_visitor_impl;
 
      public:

        /** \brief Type for display dimensions */
        struct display_dimensions {
          unsigned short x; ///< \brief Horizontal dimension
          unsigned short y; ///< \brief Vertical dimension
          unsigned short z; ///< \brief Unused for the moment
        };
 
      private:
 
        /** \brief The protocol version */
        version            m_protocol_version;
 
        /** \brief The dimensions of the screen that are currently reserved for this tool */
        display_dimensions m_dimensions;
 
      private:
 
        /** \brief Constructor */
        capabilities(const version = default_protocol_version);
 
      public:
 
        /** \brief Get the protocol version */
        version get_version() const;
 
        /** \brief Set display dimensions */
        void set_display_dimensions(const unsigned short x, const unsigned short y, const unsigned short z);
 
        /** \brief Get the dimensions of the part of the display that is reserved for this tool */
        display_dimensions get_display_dimensions() const;
    };
 
     inline controller::capabilities::capabilities(const version v) : m_protocol_version(v) {
      m_dimensions.x = 0;
      m_dimensions.y = 0;
      m_dimensions.z = 0;
    }
 
    inline version controller::capabilities::get_version() const {
      return (m_protocol_version);
    }
 
    inline void controller::capabilities::set_display_dimensions(const unsigned short x, const unsigned short y, const unsigned short z) {
      m_dimensions.x = x;
      m_dimensions.y = y;
      m_dimensions.z = z;
    }
 
    inline controller::capabilities::display_dimensions controller::capabilities::get_display_dimensions() const {
      return (m_dimensions);
    }
  }
}

#endif

