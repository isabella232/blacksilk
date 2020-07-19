#include <libgraphics/debug.hpp>
#include <libgraphics/image.hpp>
#include <libgraphics/backend/gl/gl_effect.hpp>
#include <libgraphics/backend/gl/gl_rendertarget.hpp>
#include <libgraphics/backend/gl/gl_texture.hpp>
#include <libgraphics/backend/gl/gl_imageobject.hpp>
#include <libgraphics/backend/cpu/cpu_imageobject.hpp>
#include <libgraphics/bitmap.hpp>
#include <QDebug>
#include <sstream>

namespace libgraphics {

void writeSummaryCPU( size_t mem, size_t obj ) {
    qDebug() << "MemoryLog:                 * allocatedCPUMemory:           " << ( ( float )mem / ( 1000.0f * 1000.0f ) ) << "Mb";
    qDebug() << "MemoryLog:                 * allocatedCPUObjects:          " << obj;
}
void writeSummaryGPU( size_t mem, size_t obj ) {
    qDebug() << "MemoryLog:                 * allocatedGPUMemory:           " << ( ( float )mem / ( 1000.0f * 1000.0f ) ) << "Mb";
    qDebug() << "MemoryLog:                 * allocatedGPUObjects:          " << obj;
}

MemoryLog::MemoryLog( bool _continious, bool _pedantic ) : m_ContiniousLog( _continious ),
    m_Pedantic( _pedantic ) {}

void        MemoryLog::signalAllocation( size_t cpuMemorySize ) {
    this->m_Info.allocatedCPUMemory     += cpuMemorySize;
    this->m_Info.allocatedCPUObjects++;
    this->m_Info.totalCPUMemory         += cpuMemorySize;
    this->m_Info.totalCPUObjects++;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Allocation - (CPU) - Arbitary Memory          # " << ( ( float )cpuMemorySize / ( 1000.0f * 1000.0f ) ) << "Mb";
        //writeSummaryCPU(this->m_Info.allocatedCPUMemory,this->m_Info.allocatedCPUObjects);
    }
}

void        MemoryLog::signalAllocation( backend::gl::Effect* effect ) {
    assert( effect != nullptr );
    const auto memSize = effect->getGpuMemoryConsumption();

    this->m_Info.allocatedGPUMemory     += memSize;
    this->m_Info.allocatedGPUObjects++;
    this->m_Info.totalGPUMemory         += memSize;
    this->m_Info.totalGPUObjects++;
    this->m_Info.gpuPoolCapacity++;

    this->m_Info.gpuAllocatedEffects++;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Allocation - (GPU) - Effect                   # " << ( ( float )memSize / ( 1000.0f * 1000.0f ) ) << "Mb";
        //writeSummaryGPU(this->m_Info.allocatedGPUMemory,this->m_Info.allocatedGPUObjects);
    }
}

void        MemoryLog::signalAllocation( backend::gl::Texture* texture ) {
    assert( texture != nullptr );
    const auto memSize = texture->getGpuMemoryConsumption();

    this->m_Info.allocatedGPUMemory     += memSize;
    this->m_Info.allocatedGPUObjects++;
    this->m_Info.totalGPUMemory         += memSize;
    this->m_Info.totalGPUObjects++;
    this->m_Info.gpuPoolCapacity++;

    this->m_Info.gpuAllocatedTextures++;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Allocation - (GPU) - Texture                  # " << ( ( float )memSize / ( 1000.0f * 1000.0f ) ) << "Mb";
        //writeSummaryGPU(this->m_Info.allocatedGPUMemory,this->m_Info.allocatedGPUObjects);
    }
}

void        MemoryLog::signalAllocation( backend::gl::RenderTarget* target ) {
    assert( target != nullptr );
    const auto memSize = target->getGpuMemoryConsumption();

    this->m_Info.allocatedGPUMemory     += memSize;
    this->m_Info.allocatedGPUObjects++;
    this->m_Info.totalGPUMemory         += memSize;
    this->m_Info.totalGPUObjects++;
    this->m_Info.gpuPoolCapacity++;

    this->m_Info.gpuAllocatedRenderTargets++;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Allocation - (GPU) - RenderTarget             # " << ( ( float )memSize / ( 1000.0f * 1000.0f ) ) << "Mb";
        //writeSummaryGPU(this->m_Info.allocatedGPUMemory,this->m_Info.allocatedGPUObjects);
    }
}

void        MemoryLog::signalAllocation( Bitmap* bitmap ) {
    assert( bitmap != nullptr );
    const auto cpuMemorySize = bitmap->width() * bitmap->height() * bitmap->formatByteSize();

    this->m_Info.allocatedCPUMemory     += cpuMemorySize;
    this->m_Info.allocatedCPUObjects++;
    this->m_Info.totalCPUMemory         += cpuMemorySize;
    this->m_Info.totalCPUObjects++;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Allocation - (CPU) - Bitmap                   # " << ( ( float )cpuMemorySize / ( 1000.0f * 1000.0f ) ) << "Mb";
        //writeSummaryCPU(this->m_Info.allocatedCPUMemory,this->m_Info.allocatedCPUObjects);
    }
}

void        MemoryLog::signalAllocation( ImageLayer* layer ) {
    assert( layer != nullptr );
    this->m_Info.imageLayers++;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Allocation - (Generic) - ImageLayer           # " << layer->width() << "x" << layer->height() << "@" << libgraphics::fxapi::EPixelFormat::toString( layer->format() );
    }
}

void        MemoryLog::signalAllocation( backend::gl::ImageObject* layer ) {
    assert( layer != nullptr );
    this->m_Info.gpuImageObjects++;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Allocation - (GPU) - ImageObject              # " << layer->width() << "x" << layer->height() << "@" << libgraphics::fxapi::EPixelFormat::toString( layer->format() );

    }
}

void        MemoryLog::signalAllocation( backend::cpu::ImageObject* layer ) {
    assert( layer != nullptr );
    this->m_Info.cpuImageObjects++;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Allocation - (CPU) - ImageObject              # " << layer->width() << "x" << layer->height() << "@" << libgraphics::fxapi::EPixelFormat::toString( layer->format() );

    }
}

void        MemoryLog::signalDeallocation( size_t memorySize ) {
    this->m_Info.allocatedCPUMemory     -= memorySize;
    this->m_Info.allocatedCPUObjects--;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Deallocation - (CPU) - Arbitary Memory        # " << ( ( float )memorySize / ( 1000.0f * 1000.0f ) ) << "Mb";
        //writeSummaryCPU(this->m_Info.allocatedCPUMemory,this->m_Info.allocatedCPUObjects);
    }
}

void        MemoryLog::signalDeallocation( backend::gl::Effect* obj ) {
    assert( obj != nullptr );
    const auto memSize = obj->getGpuMemoryConsumption();

    this->m_Info.allocatedGPUMemory     -= memSize;
    this->m_Info.allocatedGPUObjects--;
    this->m_Info.gpuPoolCapacity--;

    this->m_Info.gpuAllocatedEffects--;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Deallocation - (GPU) - Effect                 # " << ( ( float )memSize / ( 1000.0f * 1000.0f ) ) << "Mb";
        //writeSummaryGPU(this->m_Info.allocatedGPUMemory,this->m_Info.allocatedGPUObjects);
    }
}

void        MemoryLog::signalDeallocation( backend::gl::Texture* obj ) {
    assert( obj != nullptr );
    const auto memSize = obj->getGpuMemoryConsumption();

    this->m_Info.allocatedGPUMemory     -= memSize;
    this->m_Info.allocatedGPUObjects--;
    this->m_Info.gpuPoolCapacity--;

    this->m_Info.gpuAllocatedTextures--;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Deallocation - (GPU) - Texture                # " << ( ( float )memSize / ( 1000.0f * 1000.0f ) ) << "Mb";
        //writeSummaryGPU(this->m_Info.allocatedGPUMemory,this->m_Info.allocatedGPUObjects);
    }
}

void        MemoryLog::signalDeallocation( backend::gl::RenderTarget* obj ) {
    assert( obj != nullptr );
    const auto memSize = obj->getGpuMemoryConsumption();

    this->m_Info.allocatedGPUMemory     -= memSize;
    this->m_Info.allocatedGPUObjects--;
    this->m_Info.gpuPoolCapacity--;

    this->m_Info.gpuAllocatedRenderTargets--;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Deallocation - (GPU) - RenderTarget           # " << ( ( float )memSize / ( 1000.0f * 1000.0f ) ) << "Mb";
        //writeSummaryGPU(this->m_Info.allocatedGPUMemory,this->m_Info.allocatedGPUObjects);
    }
}

void        MemoryLog::signalDeallocation( Bitmap* bitmap ) {
    assert( bitmap != nullptr );
    const auto cpuMemorySize = bitmap->width() * bitmap->height() * bitmap->formatByteSize();

    this->m_Info.allocatedCPUMemory     -= cpuMemorySize;
    this->m_Info.allocatedCPUObjects--;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Deallocation - (CPU) - Bitmap                 # " << ( ( float )cpuMemorySize / ( 1000.0f * 1000.0f ) ) << "Mb";
        //writeSummaryCPU(this->m_Info.allocatedCPUMemory,this->m_Info.allocatedCPUObjects);
    }
}

void        MemoryLog::signalDeallocation( ImageLayer* layer ) {
    assert( layer != nullptr );
    this->m_Info.imageLayers--;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Deallocation - (Generic) - ImageObject        # " << layer->width() << "x" << layer->height() << "@" << libgraphics::fxapi::EPixelFormat::toString( layer->format() );
    }
}

void        MemoryLog::signalDeallocation( backend::cpu::ImageObject* layer ) {
    assert( layer != nullptr );
    this->m_Info.cpuImageObjects--;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Deallocation - (CPU) - ImageObject            # " << layer->width() << "x" << layer->height() << "@" << libgraphics::fxapi::EPixelFormat::toString( layer->format() );
    }
}

void        MemoryLog::signalDeallocation( backend::gl::ImageObject* layer ) {
    assert( layer != nullptr );
    this->m_Info.gpuImageObjects--;

    if( this->m_ContiniousLog ) {
        qDebug() << "MemoryLog: " << "Deallocation - (GPU) - ImageObject            # " << layer->width() << "x" << layer->height() << "@" << libgraphics::fxapi::EPixelFormat::toString( layer->format() );
    }
}


void        MemoryLog::signalAcquire( backend::gl::Effect* obj ) {
    assert( obj != nullptr );
    const auto memSize = obj->getGpuMemoryConsumption();

    this->m_Info.gpuPoolSize++;

    if( this->m_ContiniousLog && this->m_Pedantic ) {
        qDebug() << "MemoryLog: " << "Acquire - (GPU) - Effect                      # " << ( ( float )memSize / ( 1000.0f * 1000.0f ) ) << "Mb";
    }
}

void        MemoryLog::signalAcquire( backend::gl::Texture* obj ) {
    assert( obj != nullptr );
    const auto memSize = obj->getGpuMemoryConsumption();

    this->m_Info.gpuPoolSize++;

    if( this->m_ContiniousLog && this->m_Pedantic ) {
        qDebug() << "MemoryLog: " << "Acquire - (GPU) - Texture                     # " << ( ( float )memSize / ( 1000.0f * 1000.0f ) ) << "Mb";
    }
}

void        MemoryLog::signalAcquire( backend::gl::RenderTarget* obj ) {
    assert( obj != nullptr );
    const auto memSize = obj->getGpuMemoryConsumption();

    this->m_Info.gpuPoolSize++;

    if( this->m_ContiniousLog && this->m_Pedantic ) {
        qDebug() << "MemoryLog: " << "Acquire - (GPU) - RenderTarget                # " << ( ( float )memSize / ( 1000.0f * 1000.0f ) ) << "Mb";
    }
}

void        MemoryLog::signalRelease( backend::gl::Effect* obj ) {
    assert( obj != nullptr );
    const auto memSize = obj->getGpuMemoryConsumption();

    this->m_Info.gpuPoolSize--;

    if( this->m_ContiniousLog && this->m_Pedantic ) {
        qDebug() << "MemoryLog: " << "Release - (GPU) - Effect                      # " << ( ( float )memSize / ( 1000.0f * 1000.0f ) ) << "Mb";
    }
}

void        MemoryLog::signalRelease( backend::gl::Texture* obj ) {
    assert( obj != nullptr );
    const auto memSize = obj->getGpuMemoryConsumption();

    this->m_Info.gpuPoolSize--;

    if( this->m_ContiniousLog && this->m_Pedantic ) {
        qDebug() << "MemoryLog: " << "Release - (GPU) - Texture                     # " << ( ( float )memSize / ( 1000.0f * 1000.0f ) ) << "Mb";
    }
}

void        MemoryLog::signalRelease( backend::gl::RenderTarget* obj ) {
    assert( obj != nullptr );
    const auto memSize = obj->getGpuMemoryConsumption();

    this->m_Info.gpuPoolSize--;

    if( this->m_ContiniousLog && this->m_Pedantic ) {
        qDebug() << "MemoryLog: " << "Release - (GPU) - RenderTarget                # " << ( ( float )memSize / ( 1000.0f * 1000.0f ) ) << "Mb";
    }
}

bool&       MemoryLog::continiousLog() {
    return this->m_ContiniousLog;
}

const bool& MemoryLog::continiousLog() const {
    return this->m_ContiniousLog;
}

bool&       MemoryLog::pedantic() {
    return this->m_Pedantic;
}

const bool& MemoryLog::pedantic() const {
    return this->m_Pedantic;
}

MemoryLog::MemoryInfo MemoryLog::sample() {
    return this->m_Info;
}

void MemoryLog::printToLog() {
    const std::string str = MemoryLog::memoryInfoToString( sample() );
    qDebug() << str.c_str();
}

std::string MemoryLog::memoryInfoToString( const MemoryInfo& info ) {
    std::stringstream ss;

    ss << "\n MemoryInfo: " << std::endl;
    ss <<       " - allocatedCPUObjects:        " << info.allocatedCPUObjects << std::endl;
    ss <<       " - allocatedGPUObjects:        " << info.allocatedGPUObjects << std::endl;
    ss <<       " - totalCPUObjects:            " << info.totalCPUObjects << std::endl;
    ss <<       " - totalGPUObjects:            " << info.totalGPUObjects << std::endl;
    ss << std::endl;
    ss <<       " - allocatedGPUMemory:         " << info.allocatedGPUMemory << std::endl;
    ss <<       " - totalGPUMemory:             " << info.totalGPUMemory << std::endl;
    ss << std::endl;
    ss <<       " - allocatedCPUMemory:         " << info.allocatedCPUMemory << std::endl;
    ss <<       " - totalCPUMemory:             " << info.totalCPUMemory << std::endl;
    ss << std::endl;
    ss <<       " - gpuPoolSize:                " << info.gpuPoolSize << std::endl;
    ss <<       " - gpuPoolCapacity:            " << info.gpuPoolCapacity << std::endl;
    ss << std::endl;
    ss <<       " - gpuAllocatedEffects:        " << info.gpuAllocatedEffects << std::endl;
    ss <<       " - gpuAllocatedRenderTargets:  " << info.gpuAllocatedRenderTargets << std::endl;
    ss <<       " - gpuAllocatedTextures:       " << info.gpuAllocatedTextures << std::endl;
    ss << std::endl;
    ss <<       " - imageLayers:                " << info.imageLayers << std::endl;
    ss <<       " - cpuImageObjects:            " << info.cpuImageObjects << std::endl;
    ss <<       " - gpuImageObjects:            " << info.gpuImageObjects << std::endl;
    ss << std::endl;

    return ss.str();
}

MemoryLog& MemoryLog::global() {
#if LIBGRAPHICS_MEMORY_LOG_CONTINIOUS
    static const bool continiousLog = true;
#else
    static const bool continiousLog = false;
#endif

#if LIBGRAPHICS_MEMORY_LOG_PEDANTIC
    static const bool pedantic      = true;
#else
    static const bool pedantic      = false;
#endif

    static MemoryLog log( continiousLog, pedantic );

    return log;
}


}
