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

    /// cds::threading::Manager implementation based on hpxthread thread-specific data functions
    inline namespace hpxthread {

        /// Thread-specific data manager based on hpxthread thread-specific data functions
        /**
            Manager throws an exception of Manager::hpxthread_exception class if an error occurs
        */
        class Manager {
        private :
            /// hpxthread error code type
            typedef int hpxthread_error_code;

            /// hpxthread exception
            class hpxthread_exception: public std::system_error
            {
            public:
                /// Exception constructor
                hpxthread_exception( int nCode, const char * pszFunction )
                    : std::system_error( nCode, std::system_category(), pszFunction )
                {}
            };

            /// hpxthread TLS key holder
            struct Holder {
            //@cond
//                static pthread_key_t       m_key;
                static hpx::thread::id m_key;

                static void key_destructor(void * p)
                {
                    if ( p ) {
                        reinterpret_cast<ThreadData *>(p)->fini();
                        delete reinterpret_cast<ThreadData *>(p);
                    }
                }

                static void init()
                {
//                    hpxthread_error_code  nErr;
                    m_key = hpx::this_thread::get_id();
//                    if ( ( nErr = pthread_key_create( &m_key, key_destructor )) != 0 )
//                    if(m_key == hpx::threads::invalid_thread_id)
//                    {
//                        CDS_THROW_EXCEPTION( hpxthread_exception( nErr, "hpxthread_key_create" ));
//                    }
                }

                static void fini()
                {
//                    hpxthread_error_code  nErr;
//                    if ( ( nErr = pthread_key_delete( m_key )) != 0 )
//                        CDS_THROW_EXCEPTION( hpxthread_exception( nErr, "hpxthread_key_delete" ));
                     m_key = hpx::thread::id();
                }

                static ThreadData *    get()
                {
//                    return reinterpret_cast<ThreadData *>( pthread_getspecific( m_key ));
                    return reinterpret_cast<ThreadData *>( hpx::this_thread::get_thread_data());
                }

                static void alloc()
                {
//                    hpxthread_error_code  nErr;
//                    ThreadData * pData = new ThreadData;
//                    if ( ( nErr = pthread_setspecific( m_key, pData )) != 0 )
                    ThreadData* pData = new ThreadData();
                    hpx::this_thread::set_thread_data(reinterpret_cast<std::size_t>(pData) );
//                    {
//                        CDS_THROW_EXCEPTION( hpxthread_exception( nErr, "hpxthread_setspecific" ));
//                    }
                }
                static void free()
                {
                    ThreadData * p = get();
                    hpx::this_thread::set_thread_data(reinterpret_cast<std::size_t>(nullptr) );
//                    pthread_setspecific( m_key, nullptr );
                    m_key = hpx::thread::id();
                    delete p;
                }
            //@endcond
            };

            //@cond
            enum EThreadAction {
                do_getData,
                do_attachThread,
                do_detachThread,
                do_checkData,
                init_holder,
                fini_holder
            };
            //@endcond

            //@cond
            static ThreadData * _threadData( EThreadAction nAction )
            {
                switch ( nAction ) {
                    case do_getData:
                        return Holder::get();
                    case do_checkData:
                        return Holder::get();
                    case do_attachThread:
                        if ( Holder::get() == nullptr )
                            Holder::alloc();
                        return Holder::get();
                    case do_detachThread:
                        Holder::free();
                        return nullptr;
                    case init_holder:
                    case fini_holder:
                        break;
                    default:
                        assert( false ) ;   // anything forgotten?..
                }
                assert(false)   ;   // how did we get here?
                return nullptr;
            }
            //@endcond

        public:
            /// Initialize manager
            /**
                This function is automatically called by cds::Initialize
            */
            static void init()
            {
                std::cout << "\n********** HPX **************\n";
                Holder::init();
            }

            /// Terminate manager
            /**
                This function is automatically called by cds::Terminate
            */
            static void fini()
            {
                Holder::fini();
            }

            /// Checks whether current thread is attached to \p libcds feature or not.
            static bool isThreadAttached()
            {
                return _threadData( do_checkData ) != nullptr;
            }

            /// This method must be called in beginning of thread execution
            /**
                If TLS pointer to manager's data is \p nullptr, hpxthread_exception is thrown
                with code = -1.
                If an error occurs in call of hpxthread API function, hpxthread_exception is thrown
                with hpxthread error code.
            */
            static void attachThread()
            {
                std::cout << "\n ------- attaching thread!!!! --------\n";
                ThreadData * pData = _threadData( do_attachThread );
                assert( pData );

                if ( pData ) {
                    pData->init();
                }
                else
                    CDS_THROW_EXCEPTION( hpxthread_exception( -1, "cds::threading::hpxthread::Manager::attachThread" ));
            }

            /// This method must be called in end of thread execution
            /**
                If TLS pointer to manager's data is \p nullptr, hpxthread_exception is thrown
                with code = -1.
                If an error occurs in call of hpxthread API function, hpxthread_exception is thrown
                with hpxthread error code.
            */
            static void detachThread()
            {
                ThreadData * pData = _threadData( do_getData );
                assert( pData );

                if ( pData ) {
                    if ( pData->fini())
                        _threadData( do_detachThread );
                }
                else
                    CDS_THROW_EXCEPTION( hpxthread_exception( -1, "cds::threading::hpxthread::Manager::detachThread" ));
            }

            /// Returns ThreadData pointer for the current thread
            static ThreadData * thread_data()
            {
                return _threadData( do_getData );
            }

            //@cond
            static size_t fake_current_processor()
            {
                return _threadData( do_getData )->fake_current_processor();
            }
            //@endcond

        };

    } // namespace hpxthread
}} // namespace cds::threading
//@endcond

#endif // #ifndef CDSLIB_THREADING_DETAILS_HPX_MANAGER_H
