#pragma once

#include <assert.h>

#include <libcommon/scopedptr.hpp>

#include <libgraphics/fxapi.hpp>

namespace libgraphics {
namespace backend {
namespace gl {

/// forward
class Resource;

/// impl: Resource
class Resource : public libgraphics::fxapi::ApiResource,
    public libcommon::INonCopyable {
    public:
        struct Private;

        Resource();
        virtual ~Resource();

        /// returns the gpu-side memory consumption of the
        /// current resource object.
        virtual size_t getGpuMemoryConsumption() = 0;

        /// returns the cpu-side memroy consumption of the
        /// current resource object.
        virtual size_t getCpuMemoryConsumption() = 0;

        /// returns true, if the current resource was already
        /// acquired.
        virtual bool acquired();

        /// tries to acquire the resource for the
        /// current thread.
        virtual bool tryAcquire();

        /// acquires the current resource.
        virtual void acquire();

        /// releases the resource from the
        /// current thread.
        virtual bool release();

        /// forces the release of the
        /// current resource.
        virtual void forceRelease();

    private:
        libcommon::PimplPtr<Private>   d;
};

/// impl: ScopedResourceGuard
template < class _t_resource >
struct ScopedResourceGuard {
        typedef _t_resource ResourceType;
        struct _guard_impl {
            _guard_impl( ResourceType* _resource ) : resource( _resource ) {
                assert( resource );
                resource->acquire();
            }
            ~_guard_impl() {
                resource->release();
            }

            ResourceType*   resource;
        };
        explicit ScopedResourceGuard( ResourceType* resource ) : d( new _guard_impl( resource ) ) {}
    private:
        libcommon::ScopedPtr< _guard_impl > d;
};
typedef ScopedResourceGuard<backend::gl::Resource>  BaseScopedResourceGuard;

/// impl: SharedResourceGuard
template < class _t_resource >
struct SharedResourceGuard {
        typedef _t_resource ResourceType;
        struct _guard_impl {
            _guard_impl( ResourceType* _resource ) : resource( _resource ) {
                assert( resource );
                resource->acquire();
            }
            ~_guard_impl() {
                resource->release();
            }

            ResourceType*   resource;
        };
        explicit SharedResourceGuard( ResourceType* resource ) : d( new _guard_impl( resource ) ) {}
    private:
        libcommon::SharedPtr< _guard_impl > d;
};
typedef SharedResourceGuard<backend::gl::Resource>  BaseSharedResourceGuard;

}
}
}
