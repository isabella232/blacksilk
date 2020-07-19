
#include <libgraphics/fx/operations/basic/general/gl.hpp>
#include <libgraphics/fx/operations/helpers/gl_helpers.hpp>

#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

/// operation: blit
void blit_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    libgraphics::Rect32I sourceArea,
    libgraphics::Rect32I destArea
) {
    assert( destArea.x == sourceArea.x );
    assert( destArea.y == sourceArea.y );
    assert( destArea.width == sourceArea.width );
    assert( destArea.height == sourceArea.height );

    add_GL(
        device,
        destination,
        source,
        sourceArea,
        0.0f
    );
}

}
}
}
