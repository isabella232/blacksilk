
#include <libgraphics/fx/operations/basic/general/cpu.hpp>
#include <libgraphics/fx/operations/helpers/cpu_helpers.hpp>

#include <libgraphics/backend/cpu/cpu_imageobject.hpp>
#include <libgraphics/backend/cpu/cpu_backenddevice.hpp>

#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

void blit_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    libgraphics::Rect32I sourceArea,
    libgraphics::Rect32I destArea
) {
    assert( device );
    assert( destination );
    assert( source );

    backend::cpu::ImageObject* dstObj = ( backend::cpu::ImageObject* )destination;
    backend::cpu::ImageObject* srcObj = ( backend::cpu::ImageObject* )source;

    assert( sourceArea.width == destArea.width );
    assert( sourceArea.height == destArea.height );

    const auto successfullyCopied = dstObj->copy(
                                        srcObj,
                                        sourceArea,
                                        destArea.x,
                                        destArea.y
                                    );
    assert( successfullyCopied );

    if( !successfullyCopied ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "blit_CPU(): Failed to copy from source image object.";
#endif
    }
}

}
}
}
