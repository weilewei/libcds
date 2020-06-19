// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OS_POSIX_THREAD_H
#define CDSLIB_OS_POSIX_THREAD_H

#include <pthread.h>
#include <signal.h>

#include <hpx/config.hpp>
#include <hpx/config/defines.hpp>
#include <hpx/modules/threading.hpp>

namespace cds { namespace OS {
    /// posix-related wrappers
    inline namespace posix {

        /// Posix thread id type
        typedef hpx::threads::thread_id ThreadId;

        /// Get current thread id
        static inline ThreadId get_current_thread_id()
        {
//            return pthread_self();
            return hpx::threads::get_self_id();
        }
    }    // namespace posix

    //@cond
    constexpr const posix::ThreadId c_NullThreadId = hpx::threads::invalid_thread_id;
    //@endcond

}} // namespace cds::OS


#endif // #ifndef CDSLIB_OS_POSIX_THREAD_H
