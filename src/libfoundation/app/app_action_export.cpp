
#include <libfoundation/app/application.hpp>
#include <libgraphics/image.hpp>
#include <libgraphics/backend/common/formats.hpp>
#include <libgraphics/filterplugin.hpp>
#include <libgraphics/filterpluginloader.hpp>
#include <libgraphics/filter.hpp>
#include <libgraphics/filtercollection.hpp>
#include <libgraphics/filterstack.hpp>
#include <libgraphics/filterpreset.hpp>
#include <libgraphics/filterpresetcollection.hpp>

#include <libgraphics/fx/filters/cascadedsharpen.hpp>
#include <libgraphics/fx/filters/filmgrain.hpp>

#include <libgraphics/io/pipeline.hpp>
#include <libgraphics/io/pipelineexporter.hpp>
#include <libgraphics/io/pipelineimporter.hpp>

#include <libgraphics/backend/gl/gl_backenddevice.hpp>
#include <libgraphics/backend/cpu/cpu_backenddevice.hpp>

#include <libgraphics/fx/operations/basic.hpp>

#include <log/log.hpp>
#include <QDebug>

namespace libfoundation {
namespace app {

struct ApplicationActionExport::Private : libcommon::PimplPrivate {
    ApplicationSession*         session;
    const std::string           path;
    const EImageFormat::t       format;
    libgraphics::ImageLayer*    layer;
    libgraphics::Bitmap*        seperateAlphaChannel;
    const unsigned long         initialThreadId;
    bool                        rendered;
    bool                        alreadyRendered;
    bool                        useStreamlinedGpuRendering;
    void*                       outputBuffer;

    Private( ApplicationSession* _session,
             const std::string& _path,
             const EImageFormat::t& _format,
             libgraphics::ImageLayer* _layer ) : session( _session ), path( _path ), format( _format ), layer( _layer ), initialThreadId( libcommon::getCurrentThreadId() ),
        rendered( false ), alreadyRendered( false ), useStreamlinedGpuRendering( false ), seperateAlphaChannel( nullptr ), outputBuffer( nullptr ) {
        assert( _session );
        assert( _format != EImageFormat::Unknown );
    }
};

ApplicationActionExport::ApplicationActionExport(
    ApplicationSession* session,
    const std::string& path,
    const EImageFormat::t& format,
    libgraphics::ImageLayer* imageLayer,
    bool alreadyRendered,
    bool useStreamlinedGpuRendering,
    libgraphics::Bitmap* seperateAlphaLayer,
    void* outputBuffer
) : d( new Private( session, path, format, imageLayer ) ), SessionCommitableApplicationAction( session ) {
    d->alreadyRendered              = alreadyRendered;
    d->useStreamlinedGpuRendering   = useStreamlinedGpuRendering;
    d->seperateAlphaChannel         = seperateAlphaLayer;
    d->outputBuffer                 = outputBuffer;
}

bool ApplicationActionExport::commit() {
    /** nothing to do here. An ApplicationActionExport instances does not alter the original
        application session. At a later stage this method should signal the completion of the
        rendering/writing process. **/
    return true;
}


bool ApplicationActionExport::applyAlphaChannel( const ApplicationBackend* currentSessionBackend, libgraphics::ImageLayer* destinationLayer, libgraphics::Bitmap* alphaLayer ) {
    assert( currentSessionBackend != nullptr );
    assert( destinationLayer != nullptr );
    assert( alphaLayer != nullptr );

    size_t  channelIndex( 0 );
    bool    noActiveAlphaChannel( false );

    switch( destinationLayer->format() ) {
        case libgraphics::fxapi::EPixelFormat::RGBA8:
        case libgraphics::fxapi::EPixelFormat::RGBA16:
        case libgraphics::fxapi::EPixelFormat::RGBA16S:
        case libgraphics::fxapi::EPixelFormat::RGBA32F:
            channelIndex = 3;
            break;

        case libgraphics::fxapi::EPixelFormat::RGB8:
        case libgraphics::fxapi::EPixelFormat::RGB16:
        case libgraphics::fxapi::EPixelFormat::RGB16S:
        case libgraphics::fxapi::EPixelFormat::RGB32F:
            channelIndex            = 3;
            noActiveAlphaChannel    = true;
            break;

        default:
            return false;
    }

    if( ( alphaLayer->format() == libgraphics::formats::Mono8::toFormat() )  ||
            ( alphaLayer->format() == libgraphics::formats::Mono16::toFormat() ) ||
            ( alphaLayer->format() == libgraphics::formats::Mono16S::toFormat() ) ) {

        const auto successfullyAppendedAlphaChannel = destinationLayer->addAlphaChannel();
        assert( successfullyAppendedAlphaChannel );

        if( !successfullyAppendedAlphaChannel ) {
#if LIBFOUNDATION_DEBUG_OUTPUT
            qDebug() << "Error: Failed to append alpha channel to destination layer.";
#endif
            return false;
        }

        const auto successfullyCopiedAlphaChannel = destinationLayer->copyChannel(
                    0, /// Mono
                    channelIndex, /// Alpha Plane
                    alphaLayer
                );
        assert( successfullyCopiedAlphaChannel );

        if( !successfullyCopiedAlphaChannel ) {
#if LIBFOUNDATION_DEBUG_OUTPUT
            qDebug() << "Error: Failed to copy alpha layer to destination layer.";
#endif
            return false;
        }

        qDebug() << "Successfully re-applied alpha channel to destination layer.";
        return true;
    }

    assert( false );
    return false;
}

bool ApplicationActionExport::doStreamlinedRendering( const ApplicationBackend* currentSessionBackend, libgraphics::Bitmap& outBitmap ) {
    const auto gpuBackend = currentSessionBackend->gpuBackend();
    assert( gpuBackend != nullptr );

    const auto cpuBackend = currentSessionBackend->cpuBackend();
    assert( cpuBackend != nullptr );

    ( ( libgraphics::backend::gl::BackendDevice* )gpuBackend )->switchRenderingMode(
        libgraphics::backend::gl::BackendDevice::ERenderingMode::StreamLined
    );

    /// create new render action
    if( !d->layer->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        d->layer->updateDataForBackend(
            gpuBackend,
            FXAPI_BACKEND_OPENGL
        );
    }

    std::unique_ptr<libgraphics::ImageLayer> destinationLayer( d->layer->duplicate() );
    assert( !destinationLayer->empty() );
    assert( destinationLayer->containsDataForBackend( FXAPI_BACKEND_OPENGL ) );

    if( destinationLayer->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        destinationLayer->deleteDataForBackend( FXAPI_BACKEND_CPU );
    }

    if( destinationLayer.get() == nullptr ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionExport::process(): Failed to duplicate destination layer.";
#endif
        d->rendered = false;

        return false;
    }

    std::unique_ptr<libfoundation::app::ApplicationSession> clonedSession( d->session->clone() );
    std::unique_ptr<libfoundation::app::ApplicationActionRenderPreview> renderAction( new libfoundation::app::ApplicationActionRenderPreview(
                clonedSession.get(),
                gpuBackend,
                destinationLayer.get(),
                d->layer,
                const_cast<libgraphics::FilterStack*>( clonedSession->filterStack() )
            ) );
    assert( renderAction.get() );

    const bool successfullyRenderedCurrentState = renderAction->process();
    assert( successfullyRenderedCurrentState );

    if( !successfullyRenderedCurrentState ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionExport::process(): Failed to render final image.";
#endif
        d->rendered = false;

        ( ( libgraphics::backend::gl::BackendDevice* )gpuBackend )->switchRenderingMode(
            libgraphics::backend::gl::BackendDevice::ERenderingMode::Realtime
        );
        return false;
    }

    const auto successfullyCommitted = renderAction->commit();
    assert( successfullyCommitted != false );

    /// apply alpha plane
    if( ( this->d->seperateAlphaChannel != nullptr ) && ( !this->d->seperateAlphaChannel->empty() ) ) {
        const auto successfullyAppliedAlphaPlane = this->applyAlphaChannel(
                    currentSessionBackend,
                    destinationLayer.get(),
                    this->d->seperateAlphaChannel
                );
        assert( successfullyAppliedAlphaPlane );

        if( !successfullyAppliedAlphaPlane ) {
#if LIBFOUNDATION_DEBUG_OUTPUT
            qDebug() << "Failed to apply alpha plane to destination layer. This error may result in loss of data.";
#endif
        }
    }

    /// now export current destination layer.
    const auto sucessfullyInitializedBitmap = outBitmap.reset(
                d->session->backend()->allocator().get(),
                libgraphics::backend::fromCompatibleFormat( destinationLayer->format() ),
                destinationLayer->width(),
                destinationLayer->height()
            );

    assert( sucessfullyInitializedBitmap );

    if( !sucessfullyInitializedBitmap ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionExport::process(): Failed to create temporary bitmap from destination layer.";
#endif
        d->rendered = false;

        ( ( libgraphics::backend::gl::BackendDevice* )gpuBackend )->switchRenderingMode(
            libgraphics::backend::gl::BackendDevice::ERenderingMode::Realtime
        );

        return false;
    }

    const auto successfullyRetrievedData = destinationLayer->retrieve( &outBitmap );

    assert( successfullyRetrievedData );

    if( !successfullyRetrievedData ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionExport::process(): Failed to retrieve rendered image.";
#endif
        d->rendered = false;

        ( ( libgraphics::backend::gl::BackendDevice* )gpuBackend )->switchRenderingMode(
            libgraphics::backend::gl::BackendDevice::ERenderingMode::Realtime
        );

        return false;
    }

    /// reset sharpen filter
    {
        auto filter = clonedSession->filters()->byName(
                          "CascadedSharpen"
                      );

        if( ( filter.get() != nullptr ) ) {
            libgraphics::fx::filters::CascadedSharpen* cs = ( libgraphics::fx::filters::CascadedSharpen* )filter.get();
            cs->deleteBlurBuffersForBackend( FXAPI_BACKEND_CPU );
        }
    }
    {
        auto filter = clonedSession->filters()->byName(
                          "FilmGrain"
                      );

        if( filter.get() != nullptr ) {
            libgraphics::fx::filters::FilmGrain* fg = ( libgraphics::fx::filters::FilmGrain* )filter.get();
            fg->deleteGrainForBackend( FXAPI_BACKEND_CPU );
        }
    }

    ( ( libgraphics::backend::gl::BackendDevice* )gpuBackend )->switchRenderingMode(
        libgraphics::backend::gl::BackendDevice::ERenderingMode::Realtime
    );

    return true;
}

bool ApplicationActionExport::doCpuRendering( const ApplicationBackend* currentSessionBackend, libgraphics::Bitmap& outBitmap ) {
    const auto cpuBackend = currentSessionBackend->cpuBackend();
    assert( cpuBackend != nullptr );

    /// create new render action
    std::unique_ptr<libgraphics::ImageLayer> destinationLayer( d->layer->duplicate() );
    assert( !destinationLayer->empty() );

    if( destinationLayer.get() == nullptr ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionExport::process(): Failed to duplicate destination layer.";
#endif
        d->rendered = false;

        return false;
    }

    std::unique_ptr<libfoundation::app::ApplicationSession> clonedSession( d->session->clone() );
    std::unique_ptr<libfoundation::app::ApplicationActionRenderPreview> renderAction( new libfoundation::app::ApplicationActionRenderPreview(
                clonedSession.get(),
                cpuBackend,
                destinationLayer.get(),
                d->layer,
                const_cast<libgraphics::FilterStack*>( clonedSession->filterStack() )
            ) );
    assert( renderAction.get() );

    const bool successfullyRenderedCurrentState = renderAction->process();
    assert( successfullyRenderedCurrentState );

    if( !successfullyRenderedCurrentState ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionExport::process(): Failed to render final image.";
#endif
        d->rendered = false;

        return false;
    }

    const auto successfullyCommitted = renderAction->commit();
    assert( successfullyCommitted != false );

    /// now export current destination layer.
    const auto sucessfullyInitializedBitmap = outBitmap.reset(
                clonedSession->backend()->allocator().get(),
                libgraphics::backend::fromCompatibleFormat( destinationLayer->format() ),
                destinationLayer->width(),
                destinationLayer->height()
            );

    assert( sucessfullyInitializedBitmap );

    if( !sucessfullyInitializedBitmap ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionExport::process(): Failed to create temporary bitmap from destination layer.";
#endif
        d->rendered = false;

        return false;
    }

    /// apply alpha plane
    if( ( this->d->seperateAlphaChannel != nullptr ) && ( !this->d->seperateAlphaChannel->empty() ) ) {
        const auto successfullyAppliedAlphaPlane = this->applyAlphaChannel(
                    currentSessionBackend,
                    destinationLayer.get(),
                    this->d->seperateAlphaChannel
                );
        assert( successfullyAppliedAlphaPlane );

        if( !successfullyAppliedAlphaPlane ) {
#if LIBFOUNDATION_DEBUG_OUTPUT
            qDebug() << "Failed to apply alpha plane to destination layer. This error may result in loss of data.";
#endif
        }
    }

    const auto successfullyRetrievedData = destinationLayer->retrieve( &outBitmap );

    assert( successfullyRetrievedData );

    if( !successfullyRetrievedData ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionExport::process(): Failed to retrieve rendered image.";
#endif
        d->rendered = false;

        return false;
    }

    /// reset sharpen filter
    {
        auto filter = clonedSession->filters()->byName(
                          "CascadedSharpen"
                      );

        if( ( filter.get() != nullptr ) ) {
            libgraphics::fx::filters::CascadedSharpen* cs = ( libgraphics::fx::filters::CascadedSharpen* )filter.get();
            cs->deleteBlurBuffersForBackend( FXAPI_BACKEND_CPU );
        }
    }
    {
        auto filter = clonedSession->filters()->byName(
                          "FilmGrain"
                      );

        if( filter.get() != nullptr ) {
            libgraphics::fx::filters::FilmGrain* fg = ( libgraphics::fx::filters::FilmGrain* )filter.get();
            fg->deleteGrainForBackend( FXAPI_BACKEND_CPU );
        }
    }

    return true;
}

bool ApplicationActionExport::process() {
    if( d->rendered ) {
        LOG_DEBUG( "Image already processed" );
        return true; /** already processed **/
    }

    const auto currentThreadId = libcommon::getCurrentThreadId();
    ( void )currentThreadId;

    const auto currentSessionBackend = d->session->backend();

    assert( currentSessionBackend != nullptr );

    if( !currentSessionBackend ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionExport::process(): Failed export image with invalid session backend.";
#endif
        d->rendered = false;

        return false;
    }

    assert( currentSessionBackend->cpuInitialized() );

    if( !currentSessionBackend->cpuInitialized() ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionExport::process(): Failed to export image using uninitialized cpu-backend.";
#endif
        d->rendered = false;

        return false;
    }

    libgraphics::Bitmap outBitmap;

    if( !d->alreadyRendered ) {
        if( d->useStreamlinedGpuRendering ) {
            if( !this->doStreamlinedRendering( currentSessionBackend, outBitmap ) ) {
                return false;
            }
        } else {
            if( !this->doCpuRendering( currentSessionBackend, outBitmap ) ) {
                return false;
            }
        }

        assert( !outBitmap.empty() );
    } else {

        /// now export current destination layer.
        libgraphics::Bitmap     temporaryBitmapObj;
        libgraphics::Bitmap*    temporaryBitmap(
            ( this->d->seperateAlphaChannel != nullptr ) ? &temporaryBitmapObj : &outBitmap
        );

        const auto successfullyInitializedBitmap = temporaryBitmap->reset(
                    d->session->backend()->allocator().get(),
                    libgraphics::backend::fromCompatibleFormat( d->layer->format() ),
                    d->layer->width(),
                    d->layer->height()
                );

        assert( successfullyInitializedBitmap );

        if( !successfullyInitializedBitmap ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
            qDebug() << "ApplicationActionExport::process(): Failed to create temporary bitmap from destination layer.";
#endif
            d->rendered = false;

            return false;
        }

        const auto successfullyRetrievedData = d->layer->retrieve( temporaryBitmap );
        assert( successfullyRetrievedData );

        if( !successfullyRetrievedData ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
            qDebug() << "ApplicationActionExport::process(): Failed to retrieve preview image.";
#endif
            d->rendered = false;

            return false;
        }

        if( d->seperateAlphaChannel != nullptr ) {
            const auto alphaFormat                  =
                libgraphics::backend::fromCompatibleFormat( libgraphics::fxapi::EPixelFormat::getAssociatedAlphaFormat( d->layer->format() ) );
            const auto successfullyInitializedBitmap = outBitmap.reset(
                        d->session->backend()->allocator().get(),
                        alphaFormat,
                        d->layer->width(),
                        d->layer->height()
                    );
            LOGB_ASSERT( successfullyInitializedBitmap, "Bitmap not initialized" );

            const auto successfullyCopied = outBitmap.copy(
                                                temporaryBitmap,
                                                temporaryBitmap->size(),
                                                0,
                                                0
                                            );
            LOGB_ASSERT( successfullyCopied, "Bitmap not copied" );

            const auto successfullyCopiedChannel = outBitmap.copyChannel(
                    0, /// Mono
                    3, /// Alpha plane
                    this->d->seperateAlphaChannel,
                    d->seperateAlphaChannel->size(),
                    0,
                    0
                                                   );
            LOGB_ASSERT( successfullyCopiedChannel, "Channel not copied" );
        }
    }

    if( !d->path.empty() ) {
        /// now export the rendered image
        auto ioPipeline = const_cast<libgraphics::io::Pipeline*>( d->session->pipeline() );
        assert( ioPipeline );

        const auto successfullyExported = ioPipeline->exportToPath(
                                              EImageFormat::toString( d->format ).c_str(),
                                              d->path.c_str(),
                                              &outBitmap
                                          );

        assert( successfullyExported );

        if( !successfullyExported ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
            qDebug() << "ApplicationActionExport::process(): Failed to export image to specified path '" << d->path.c_str() << "'.";
#endif
            d->rendered = false;

            return false;
        }

#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionExport::process(): Successfully exported image to '" << d->path.c_str() << "'.";
#endif
    } else {
        qDebug() << "Filename is empty...";
    }

    if( d->outputBuffer != nullptr ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Copying to seperate output buffer...";
#endif

        memcpy( ( void* )d->outputBuffer, ( const void* )outBitmap.buffer(), outBitmap.width() * outBitmap.height() * outBitmap.formatByteSize() );
    }

    d->rendered = true;

    return true;
}

bool ApplicationActionExport::finished() {
    return d->rendered;
}


}
}
