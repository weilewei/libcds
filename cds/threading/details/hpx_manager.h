// Copyright (c) 2020 Weile Wei
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_THREADING_DETAILS_HPX_MANAGER_H
#define CDSLIB_THREADING_DETAILS_HPX_MANAGER_H

#include <system_error>
#include <stdio.h>
#include <cds/threading/details/_common.h>
#include <cds/details/throw_exception.h>

#include <hpx/include/threads.hpp>

#include <iostream>

//@cond
namespace cds { namespace threading {

    /// cds::threading::Manager implementation based on hpxthread thread-specific data functions
    inline namespace hpxthread {

        /// Thread-specific data manager based on hpxthread thread-specific data functions
        /**
            Manager throws an exception of Manager::hpxthread_exception class if an error occurs
        */
        class Manager {
        public:
            /// Initialize manager
            /**
                This function is automatically called by cds::Initialize
            */
            static void init()
            {}

            /// Terminate manager
            /**
                This function is automatically called by cds::Terminate
            */
            static void fini()
            {}

            /// Checks whether current thread is attached to \p libcds feature or not.
            static bool isThreadAttached()
            {
                ThreadData * pData = reinterpret_cast<ThreadData*> (hpx::threads::get_thread_data(hpx::threads::get_self_id()));
                return pData != nullptr;
            }

            /// This method must be called in beginning of thread execution
            static void attachThread()
            {
                std::stringstream temp;
                temp << "\n attaching thread and my thread id: "<< hpx::threads::get_self_id();
                std::cout << temp.str();

                ThreadData * pData = reinterpret_cast<ThreadData*> (hpx::threads::get_thread_data(hpx::threads::get_self_id()));
                if(pData == nullptr)
                {
                    pData = new ThreadData;
                    hpx::threads::set_thread_data(hpx::threads::get_self_id(), reinterpret_cast<std::size_t>(pData));
                }
                assert( pData );
                pData->init();
            }

            /// This method must be called in end of thread execution
            static void detachThread()
            {
                ThreadData * pData = reinterpret_cast<ThreadData*> (hpx::threads::get_thread_data(hpx::threads::get_self_id()));
                assert( pData );

                if ( pData->fini())
                    hpx::threads::set_thread_data(hpx::threads::get_self_id(), reinterpret_cast<std::size_t>(nullptr));
            }

            /// Returns ThreadData pointer for the current thread
            static ThreadData * thread_data()
            {
                return reinterpret_cast<ThreadData*> (hpx::threads::get_thread_data(hpx::threads::get_self_id()));

            }

            //@cond
            static size_t fake_current_processor()
            {
                ThreadData * pData = reinterpret_cast<ThreadData*> (hpx::threads::get_thread_data(hpx::threads::get_self_id()));
                return pData->fake_current_processor();
            }
            //@endcond

        };

    } // namespace pthread
}} // namespace cds::threading
//@endcond

#endif // #ifndef CDSLIB_THREADING_DETAILS_HPX_MANAGER_H
