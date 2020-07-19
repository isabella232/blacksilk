#include <libgraphics/fx/operations/helpers/cpu_helpers.hpp>
#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

/// helpers

/// returns the 1024x1024 tile count for the
/// specific rectangle
size_t getTileCountForRectangle(
    libgraphics::Rect32I   area,
    size_t _tileSize = 1024
);
size_t getVerticalTileCountForRectangle(
    libgraphics::Rect32I   area,
    size_t _tileSize = 1024
);
size_t getHorizontalTileCountForRectangle(
    libgraphics::Rect32I   area,
    size_t _tileSize = 1024
);
size_t getTileSize();

size_t calculateDefaultTileSize( size_t threads, size_t width, size_t height ) {
    const float ratio   = ( float )width / ( float )height;
    const float total   = ( width * height );
    const int    base   = std::ceil( sqrt( total ) ) / threads;
    return std::max<size_t>(
               base * ratio,
               base * ( 1.0f / ratio )
           );
}

void cpuExecuteTileBased(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   destination,
    libgraphics::backend::cpu::ImageObject*   source,
    libgraphics::Rect32I area,
    std::function<void( libgraphics::fxapi::ApiBackendDevice*, libgraphics::backend::cpu::ImageObject*, libgraphics::backend::cpu::ImageObject*, libgraphics::Rect32I )>   kernel,
    bool manualSync
) {
    typedef std::function<void( libgraphics::fxapi::ApiBackendDevice*, libgraphics::backend::cpu::ImageObject*, libgraphics::backend::cpu::ImageObject*, libgraphics::Rect32I )>  kernel_fn;

    libgraphics::backend::cpu::BackendDevice*   cpuDevice = static_cast <
            libgraphics::backend::cpu::BackendDevice *
            >( device );

    const size_t tileSize        = 1024; /*calculateDefaultTileSize(
        cpuDevice->threadPool()->maxThreadCount(),
        area.width,
        area.height
    );*/
    const size_t horizontalCount = getHorizontalTileCountForRectangle(
                                       area,
                                       tileSize
                                   );
    const size_t verticalCount = getVerticalTileCountForRectangle(
                                     area,
                                     tileSize
                                 );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    qDebug() << "TileCount: " << horizontalCount* verticalCount;
    qDebug() << "TileSize:  " << tileSize;
#endif

    const unsigned int baseTileX = area.x;
    const unsigned int baseTileY = area.y;

    unsigned int tileX = baseTileX;
    unsigned int tileY = baseTileY;

    for( size_t iterHoriz = 0; horizontalCount > iterHoriz; ++iterHoriz ) {
        for( size_t iterVert = 0; verticalCount > iterVert; ++iterVert ) {
            tileX = baseTileX + ( unsigned int )( iterHoriz * tileSize );
            tileY = baseTileY + ( unsigned int )( iterVert * tileSize );

            size_t tileWidth = std::min( ( size_t )tileSize, ( size_t )( area.width - ( iterHoriz * tileSize ) ) );
            size_t tileHeight = std::min( ( size_t )tileSize, ( size_t )( area.height - ( iterVert * tileSize ) ) );

            libgraphics::Rect32I tileArea(
                tileX,
                tileY,
                ( unsigned int )tileWidth,
                ( unsigned int )tileHeight
            );

            struct Job : QRunnable {
                Job( libgraphics::fxapi::ApiBackendDevice* _device,
                     libgraphics::backend::cpu::ImageObject*   _destination,
                     libgraphics::backend::cpu::ImageObject*   _source,
                     libgraphics::Rect32I _area,
                     kernel_fn& fn
                   ) : device( _device ), destination( _destination ),
                    source( _source ), area( _area ), kernel( fn ) {
                    setAutoDelete( true );
                }
                virtual ~Job() {}

                libgraphics::fxapi::ApiBackendDevice* device;
                libgraphics::backend::cpu::ImageObject*   destination;
                libgraphics::backend::cpu::ImageObject*   source;
                libgraphics::Rect32I area;
                kernel_fn& kernel;

                virtual void run() {
                    kernel(
                        this->device,
                        this->destination,
                        this->source,
                        this->area
                    );
                }
            };

#ifdef FXAPI_CPU_BACKEND_SINGLETHREADED
            kernel(
                device,
                destination,
                source,
                tileArea
            );
#else
            cpuDevice->threadPool()->start(
                new Job( device, destination, source, tileArea, kernel )
            );
#endif
        }
    }

    if( !manualSync ) {
        cpuDevice->threadPool()->waitForDone();
    }
}

}
}
}
