#pragma once

#include <string>
#include <libcommon/noncopyable.hpp>

namespace libgraphics {

/// predecl
namespace backend {
namespace gl {
class       Effect;
class       Texture;
class       RenderTarget;
class       ImageObject;
}
namespace cpu {
class       ImageObject;
}
}
class Bitmap;
class ImageLayer;


/// Global memory log class.
class MemoryLog :  libcommon::INonCopyable {
    public:
        struct MemoryInfo {
            long long              allocatedCPUObjects;
            long long              allocatedGPUObjects;
            long long              totalCPUObjects;
            long long              totalGPUObjects;

            long long              allocatedGPUMemory;
            long long              totalGPUMemory;

            long long              allocatedCPUMemory;
            long long              totalCPUMemory;

            long long              gpuPoolSize;
            long long              gpuPoolCapacity;

            long long              gpuAllocatedTextures;
            long long              gpuAllocatedEffects;
            long long              gpuAllocatedRenderTargets;

            long long              imageLayers;
            long long              cpuImageObjects;
            long long              gpuImageObjects;

            MemoryInfo() :
                allocatedCPUObjects( 0 ),
                allocatedGPUObjects( 0 ),
                totalCPUObjects( 0 ),
                totalGPUObjects( 0 ),
                allocatedGPUMemory( 0 ),
                totalGPUMemory( 0 ),
                allocatedCPUMemory( 0 ),
                totalCPUMemory( 0 ),
                gpuPoolSize( 0 ),
                gpuPoolCapacity( 0 ),
                gpuAllocatedTextures( 0 ),
                gpuAllocatedEffects( 0 ),
                gpuAllocatedRenderTargets( 0 ),
                imageLayers( 0 ),
                cpuImageObjects( 0 ),
                gpuImageObjects( 0 ) {}
        };

        MemoryLog( bool _continious = false, bool _pedantic = false );

        /// Signals a newly allocated object.
        void        signalAllocation( size_t cpuMemorySize );
        void        signalAllocation( backend::gl::Effect* effect );
        void        signalAllocation( backend::gl::Texture* texture );
        void        signalAllocation( backend::gl::RenderTarget* target );
        void        signalAllocation( backend::gl::ImageObject* imageObject );
        void        signalAllocation( backend::cpu::ImageObject* imageObject );
        void        signalAllocation( Bitmap* bitmap );
        void        signalAllocation( ImageLayer* layer );

        void        signalDeallocation( size_t memorySize );
        void        signalDeallocation( backend::gl::Effect* effect );
        void        signalDeallocation( backend::gl::Texture* texture );
        void        signalDeallocation( backend::gl::RenderTarget* target );
        void        signalDeallocation( backend::gl::ImageObject* imageObject );
        void        signalDeallocation( backend::cpu::ImageObject* imageObject );
        void        signalDeallocation( Bitmap* bitmap );
        void        signalDeallocation( ImageLayer* layer );

        void        signalAcquire( backend::gl::Effect* effect );
        void        signalAcquire( backend::gl::Texture* texture );
        void        signalAcquire( backend::gl::RenderTarget* target );

        void        signalRelease( backend::gl::Effect* effect );
        void        signalRelease( backend::gl::Texture* texture );
        void        signalRelease( backend::gl::RenderTarget* target );


        /// Property: If true, every single allocation is written to the
        /// log.
        bool&       continiousLog();
        const bool& continiousLog() const;

        /// Property: If true, every release/acquire is written to the
        /// log.
        bool&       pedantic();
        const bool& pedantic() const;

        /// Samples the current memory info state and returns
        /// a copy.
        MemoryInfo sample();

        /// Automatically prints a memory-usage summary to the
        /// global log-file.
        void printToLog();

        /// Prints the total log information to a string.
        static std::string      memoryInfoToString( const MemoryInfo& info );

        /// Returns the global memory log object.
        static MemoryLog& global();
    protected:
        MemoryInfo          m_Info;
        bool                m_ContiniousLog;
        bool                m_Pedantic;
};
}

#if LIBGRAPHICS_MEMORY_LOG
#   define LIBGRAPHICS_MEMORY_LOG_PRINT libgraphics::MemoryLog::global().printToLog();
#   define LIBGRAPHICS_MEMORY_LOG_ALLOCATE(x) libgraphics::MemoryLog::global().signalAllocation(x);
#   define LIBGRAPHICS_MEMORY_LOG_DEALLOCATE(x) libgraphics::MemoryLog::global().signalDeallocation(x);
#   define LIBGRAPHICS_MEMORY_LOG_ACQUIRE(x) libgraphics::MemoryLog::global().signalAcquire(x);
#   define LIBGRAPHICS_MEMORY_LOG_RELEASE(x) libgraphics::MemoryLog::global().signalRelease(x);
#   define LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET(x) libgraphics::__MemoryInfoScopedResetLog< decltype( x ) >   __p(x); (void)__p;
#else
#   define LIBGRAPHICS_MEMORY_LOG_PRINT
#   define LIBGRAPHICS_MEMORY_LOG_ALLOCATE(x)
#   define LIBGRAPHICS_MEMORY_LOG_DEALLOCATE(x)
#   define LIBGRAPHICS_MEMORY_LOG_ACQUIRE(x)
#   define LIBGRAPHICS_MEMORY_LOG_RELEASE(x)
#   define LIBGRAPHICS_MEMORY_LOG_SCOPED_RESET(x)
#endif


namespace libgraphics {

template < class _t_any >
struct __MemoryInfoScopedResetLog {
    __MemoryInfoScopedResetLog( _t_any _p ) : p( _p ) {
        assert( p != nullptr );
        LIBGRAPHICS_MEMORY_LOG_DEALLOCATE( p );
    }
    ~__MemoryInfoScopedResetLog() {
        assert( p != nullptr );
        LIBGRAPHICS_MEMORY_LOG_ALLOCATE( p );
    }
    _t_any p;
};

}
