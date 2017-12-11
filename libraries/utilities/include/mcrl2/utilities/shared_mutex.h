// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/shared_mutex_h
/// \brief This file contains a shared mutex as available on c++17.
///        This code is temporary and should be removed when the 
///        toolset is using c++17. 

#ifndef MCRL2_UTILITIES_SHARED_MUTEX_H
#define MCRL2_UTILITIES_SHARED_MUTEX_H

#include <mutex>
#include <cassert>

namespace std
{

class shared_mutex
{
  protected:
     std::mutex administration_mutex;      // This mutex is used to access the administration of this class.
     std::mutex access_mutex;              // This mutex is used to protect the critical resource.
     size_t number_of_shared_users;        // This is the number of shared_users of the critical resource.

  public:
     /// \brief Constructor. 
     shared_mutex()
      : number_of_shared_users(0)
     {}

     shared_mutex( const shared_mutex& ) = delete;

     /// \brief Lock the resource for exclusive access.
     void lock()
     {
       access_mutex.lock();
     }

     /// \brief Unlock exclusive access to the critical resource.
     void unlock()
     {
       access_mutex.unlock();
     }

     /// \brief Lock the resource for shared access. As long as shared access
     ///        is allowed, no exclusive access is possible, but multiple 
     ///        processes can use the critical resource in shared access mode. 
     void lock_shared()
     {
       administration_mutex.lock();
       number_of_shared_users++;
       if (number_of_shared_users==1)
       {
         access_mutex.lock();
       }
       administration_mutex.unlock();
     }

     /// \brief Unlock the critical resource for shared access. 
     void unlock_shared()
     {
       administration_mutex.lock();
       assert(number_of_shared_users>0);
       number_of_shared_users--;
       if (number_of_shared_users==0)
       {
         access_mutex.unlock();
       }
       administration_mutex.unlock();
     }
};

} // namespace std

#endif // MCRL2_UTILITIES_SHARED_MUTEX_H
