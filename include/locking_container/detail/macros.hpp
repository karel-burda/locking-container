#pragma once

#include <mutex>
#include <shared_mutex>
#include <utility>

#define BURDA_LC_IMPLEMENT_SHARED_ACCESS(method)\
            _BURDA_LC_IMPLEMENT_METHOD(method ## _read_lock,\
                                       _BURDA_LC_IMPLEMENT_METHOD_SHARED_ACCESS\
                                       (\
                                           T::method, std::forward<Args>(arguments)...)\
                                       )

#define BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(method)\
            _BURDA_LC_IMPLEMENT_METHOD(method ## _write_lock,\
                                       _BURDA_LC_IMPLEMENT_METHOD_EXCLUSIVE_ACCESS\
                                       (\
                                           T::method, std::forward<Args>(arguments)...)\
                                       )

#define BURDA_LC_IMPLEMENT_ITERATOR(method)\
            _BURDA_LC_IMPLEMENT_ITERATOR(method)

#define BURDA_LC_IMPLEMENT_CITERATOR(method)\
            _BURDA_LC_IMPLEMENT_CITERATOR(method)

#define BURDA_LC_LOCK_SHARED\
            std::shared_lock<decltype(locking_container_basic<T>::read_write_mutex)> read_lock(locking_container_basic<T>::read_write_mutex);

#define BURDA_LC_LOCK_EXCLUSIVE\
            std::unique_lock<decltype(locking_container_basic<T>::read_write_mutex)> write_lock(locking_container_basic<T>::read_write_mutex);

#define _BURDA_LC_IMPLEMENT_METHOD_SHARED_ACCESS(method, args)\
            BURDA_LC_LOCK_SHARED\
            return method(args);

#define _BURDA_LC_IMPLEMENT_METHOD_EXCLUSIVE_ACCESS(method, args)\
            BURDA_LC_LOCK_EXCLUSIVE\
            return method(args);


#define _BURDA_LS_METHOD_SIGNATURE(name)\
            template <typename ... Args>\
            constexpr auto name(Args && ... arguments)

#define _BURDA_LS_METHOD_SIGNATURE_CONST(name)\
            _BURDA_LS_METHOD_SIGNATURE(name) const

#define _BURDA_LC_IMPLEMENT_METHOD(name, body)\
            _BURDA_LS_METHOD_SIGNATURE(name)\
            {\
                body\
            }\
            \
            _BURDA_LS_METHOD_SIGNATURE_CONST(name)\
            {\
                body\
            }

#define _BURDA_LC_IMPLEMENT_ITERATOR(name)\
            typename T::iterator name ## _no_lock() noexcept\
            {\
                return T::name();\
            }\
            \
            typename T::const_iterator name ## _no_lock() const noexcept\
            {\
                return T::name();\
            }\
            \
            typename T::iterator name()\
            {\
                BURDA_LC_LOCK_EXCLUSIVE\
                return name ## _no_lock();\
            }\
            \
            typename T::const_iterator name() const\
            {\
                BURDA_LC_LOCK_SHARED\
                return name ## _no_lock();\
            }

#define _BURDA_LC_IMPLEMENT_CITERATOR(name)\
            typename T::const_iterator name ## _no_lock() const noexcept\
            {\
                return T::name();\
            }\
            \
            typename T::const_iterator name() const\
            {\
                BURDA_LC_LOCK_SHARED\
                return name ## _no_lock();\
            }
