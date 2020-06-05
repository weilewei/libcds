// Copyright (c) 2020 Weile Wei
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_THREADING_DETAILS_HPX_MANAGER_H
#define CDSLIB_THREADING_DETAILS_HPX_MANAGER_H

#include <system_error>
#include <stdio.h>
//#include <pthread.h>
#include <cds/threading/details/_common.h>
#include <cds/details/throw_exception.h>

#include <hpx/include/threads.hpp>

#include <iostream>

//@cond
namespace cds { namespace threading {

    //@cond
    struct hpx_internal {
        typedef unsigned char  ThreadDataPlaceholder[ sizeof(ThreadData) ];
        static ThreadDataPlaceholder s_threadData;
        static ThreadData * s_pThreadData;
    };
    //@endcond

    /// cds::threading::Manager implementation based on HPX threading
    inline namespace hpx {

        /// Thread-specific data manager
        class Manager {
        private :
            //@cond
            static ThreadData * _threadData()
            {
                return hpx_internal::s_pThreadData;
            }

            static ThreadData * create_thread_data()
            {
                if ( !hpx_internal::s_pThreadData ) {
                    hpx_internal::s_pThreadData = new (hpx_internal::s_threadData) ThreadData();
                }
                return hpx_internal::s_pThreadData;
            }

            static void destroy_thread_data()
            {
                if ( hpx_internal::s_pThreadData ) {
                    hpx_internal::s_pThreadData->ThreadData::~ThreadData();
                    hpx_internal::s_pThreadData = nullptr;
                }
            }

            //@endcond

        public:
            /// Initialize manager (empty function)
            /**
                This function is automatically called by cds::Initialize
            */
            static void init()
            {}

            /// Terminate manager (empty function)
            /**
                This function is automatically called by cds::Terminate
            */
            static void fini()
            {}

            /// Checks whether current thread is attached to \p libcds feature or not.
            static bool isThreadAttached()
            {
                ThreadData * pData = _threadData();
                return pData != nullptr;
            }

            /// This method must be called in beginning of thread execution
            static void attachThread()
            {
                create_thread_data()->init();
            }

            /// This method must be called in end of thread execution
            static void detachThread()
            {
                assert( _threadData());

                if ( _threadData()->fini())
                    destroy_thread_data();
            }

            /// Returns internal ThreadData pointer for the current thread
            static ThreadData * thread_data()
            {
                ThreadData * p = _threadData();
                assert( p );
                return p;
            }

            //@cond
            static size_t fake_current_processor()
            {
                return _threadData()->fake_current_processor();
            }
            //@endcond
        };

    } // namespace hpx
}} // namespace cds::threading
//@endcond

#endif // #ifndef CDSLIB_THREADING_DETAILS_HPX_MANAGER_H
