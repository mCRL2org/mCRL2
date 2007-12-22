// Author(s): Jeroen van der Wulp
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file extractor.cpp
/// \brief Add your file description here.

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "tipi/visitors.hpp"
#include "extractor.hpp"

namespace squadt {

  /**
   * \param[in] e pointer to this element or lifetime management
   * \param[in] t reference to the tool object to use for storage
   *
   * \pre get_process().get() is not 0 and e.get() == this
   **/
  bool extractor::extract(boost::weak_ptr < extractor > const& e, boost::shared_ptr < tool > const& t) {
    struct local {
      static void store_capabilities(boost::weak_ptr < extractor > e, boost::shared_ptr< const tipi::message >& m, boost::shared_ptr < tool > t) {
        boost::shared_ptr < extractor > guard(e.lock());

        if (guard) {
          t->m_capabilities.reset(new tipi::tool::capabilities);

          tipi::visitors::restore(*t->m_capabilities, m->to_string());
        }
      }
    };

    bool return_value = false;

    boost::shared_ptr < extractor > guard(e.lock());
    
    if (guard) {
      add_handler(tipi::message_capabilities, bind(&local::store_capabilities, e, _1, t));

      /* Await connection */
      if (await_connection(5)) {
        request_tool_capabilities();
    
        return_value = await_message(tipi::message_capabilities, 1).get() != 0;
      }
    
      finish();
    }

    return return_value;
  }
}
