// Copyright (c) 2020-2020 Alexander Gaev
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/gc/hp.h>
#include <map>

namespace cds {
    namespace gc {
        namespace hp {
            namespace details {
                thread_local thread_data *tls_ = nullptr;
                thread_local std::pair<thread_data *, thread_data *> *tls2_ = new std::pair<thread_data *, thread_data *>(
                        nullptr,
                        nullptr);

                /*static*/ CDS_EXPORT_API thread_data *DefaultTLSManager::getTLS() {
                    return tls_;
                }

                /*static*/ CDS_EXPORT_API void DefaultTLSManager::setTLS(thread_data *new_tls) {
                    tls_ = new_tls;
                }
#if defined(CDS_THREADING_HPX)
                /*static*/ CDS_EXPORT_API thread_data *HPXTLSManager::getTLS() {
                    cds::OS::posix::ThreadId thread_id = cds::OS::get_current_thread_id();
                    std::size_t hpx_tls_data = hpx::threads::get_libcds_hazard_pointer_data(thread_id);
                    return reinterpret_cast<thread_data*> (hpx_tls_data);
                }

                /*static*/ CDS_EXPORT_API void HPXTLSManager::setTLS(thread_data *new_tls) {
                    cds::OS::posix::ThreadId thread_id = cds::OS::get_current_thread_id();
                    std::size_t hp_tls_data = reinterpret_cast<std::size_t>(new_tls);
                    hpx::threads::set_libcds_hazard_pointer_data(thread_id, hp_tls_data);
                }
#else
                const size_t MAXIMUM_THREAD_ID = 10000000;
                static thread_data *heap_tls_[MAXIMUM_THREAD_ID];

                /*static*/ CDS_EXPORT_API thread_data *StrangeTLSManager::getTLS() {
                    if (cds::OS::get_current_thread_id() % 2 == 0) { // % 2 with ThreadId structure?
                        return tls2_->second;
                    } else {
                        return tls2_->first;
                    }
                }

                /*static*/ CDS_EXPORT_API void StrangeTLSManager::setTLS(thread_data *new_tls) {
                    if (cds::OS::get_current_thread_id() % 2 == 0) { // % 2 with ThreadId structure?
                        tls2_->second = new_tls;
                    } else {
                        tls2_->first = new_tls;
                    }
                }

                /*static*/ CDS_EXPORT_API thread_data *HeapTLSManager::getTLS() {
                    cds::OS::posix::ThreadId thread_id = cds::OS::get_current_thread_id();
                    return heap_tls_[thread_id % MAXIMUM_THREAD_ID];
                }

                /*static*/ CDS_EXPORT_API void HeapTLSManager::setTLS(thread_data *new_tls) {
                    cds::OS::posix::ThreadId thread_id = cds::OS::get_current_thread_id();
                    heap_tls_[thread_id % MAXIMUM_THREAD_ID] = new_tls;
                }
#endif
            }
        }
    }
}
