
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

#include <libgraphics/io/pipeline.hpp>
#include <libgraphics/io/pipelineexporter.hpp>
#include <libgraphics/io/pipelineimporter.hpp>

#include <log/log.hpp>
#include <QDebug>

namespace libfoundation {
namespace app {

struct ApplicationActionImport::Private : libcommon::PimplPrivate {
    const EImageFormat::t format;
    ApplicationSession* session;
    libgraphics::fxapi::ApiBackendDevice* backend;
    const std::string path;

    libgraphics::Bitmap     bitmapIn;

    Private(
        ApplicationSession* _session,
        libgraphics::fxapi::ApiBackendDevice* _device,
        const std::string& _path,
        const EImageFormat::t _format
    ) : session( _session ), backend( _device ),
        path( _path ), format( _format ), bitmapIn( _device->allocator().get() ) {}
};

ApplicationActionImport::ApplicationActionImport(
    ApplicationSession* session,
    libgraphics::fxapi::ApiBackendDevice* backendDevice,
    const std::string& path
) : d( new Private( session, backendDevice, path, EImageFormat::Unknown ) ), SessionCommitableApplicationAction( session ) {
    assert( session != nullptr );
    assert( backendDevice != nullptr );
    assert( !path.empty() );
}

bool ApplicationActionImport::commit() {
    if( ( d->bitmapIn.width() == 0 ) || ( d->bitmapIn.height() == 0 ) ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionImport::commit(): Failed to commit corrupted image.";
#endif
        return false;
    }

    if( d->bitmapIn.format().family == libgraphics::formats::ARGB8::Family ) {
        libgraphics::Format dstFormat( d->bitmapIn.format() );
        dstFormat.family = libgraphics::formats::family::RGBA;

        if( !d->bitmapIn.transformFormat( dstFormat ) ) {
#if LIBFOUNDATION_DEBUG_OUTPUT
            qDebug() << "Failed to import image - invalid color format.";
#endif
            return false;
        }

    }


    const auto compatibleFormat = libgraphics::backend::toCompatibleFormat( d->bitmapIn.format() );
    assert( compatibleFormat != libgraphics::fxapi::EPixelFormat::Empty );

    if( compatibleFormat == libgraphics::fxapi::EPixelFormat::Empty ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionImport::commit(): Failed to commit image of unknown format.";
#endif
        return false;
    }

    libgraphics::Image*     originalImage = new libgraphics::Image(
        this->d->backend,
        compatibleFormat,
        this->d->bitmapIn.width(),
        this->d->bitmapIn.height(),
        this->d->bitmapIn.buffer()
    );
    assert( originalImage );

    if( originalImage->empty() ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionImport::commit(): Failed to create image objects from bitmap.";
#endif
        return false;
    }

    this->d->session->resetImageState(
        nullptr,
        originalImage,
        this->d->path
    );

    return true;
}

bool ApplicationActionImport::process() {
    libgraphics::io::Pipeline*  ioPipeline = const_cast<libgraphics::io::Pipeline*>( this->d->session->pipeline() );

    assert( ioPipeline != nullptr );

    if( ioPipeline == nullptr ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionImport::process(): Failed to import image using invalid pipeline object.";
#endif
        return false;
    }

    const bool successfullyImportedBitmap = ioPipeline->importFromPath(
            d->path.c_str(),
            &d->bitmapIn
                                            );

    assert( successfullyImportedBitmap );

    if( !successfullyImportedBitmap ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionImport::process(): Failed to import bitmap. Pipeline couldn't read image from path.";
#endif
        d->bitmapIn.reset();

        return false;
    }

    if( ( d->bitmapIn.width() == 0 ) || ( d->bitmapIn.height() == 0 ) ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionImport::process(): The imported bitmap is corrupted.";
#endif
        return false;
    }

    return true;
}

bool ApplicationActionImport::finished() {
    return this->m_FinishedMutex.try_lock();
}

}
}
