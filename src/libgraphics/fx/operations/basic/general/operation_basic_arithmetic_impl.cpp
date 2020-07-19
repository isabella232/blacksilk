
#include <libgraphics/fx/operations/basic.hpp>
#include <libgraphics/fx/operations/basic/general/cpu.hpp>
#include <libgraphics/fx/operations/basic/general/gl.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

void normalize(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area
) {
    assert( dst != nullptr );
    assert( src != nullptr );
    assert( dst->format() == src->format() );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        normalize_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        normalize_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;

}

void maxThreshold(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    float maximalValue
) {
    assert( dst );
    assert( src );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        maxThreshold_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            maximalValue
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        maxThreshold_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            maximalValue
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void minThreshold(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    float minimalValue
) {
    assert( dst );
    assert( src );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        minThreshold_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            minimalValue
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        minThreshold_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            minimalValue
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void negate(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area
) {
    assert( dst );
    assert( src );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        negate_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        negate_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;

}

void min(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst );
    assert( src0 );
    assert( src1 );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        min_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        min_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void min(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I area,
    const float& minimalChannelValue
) {
    assert( dst );
    assert( src );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        min_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            minimalChannelValue
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        min_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            minimalChannelValue
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void max(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst );
    assert( src0 );
    assert( src1 );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        max_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        max_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void max(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I area,
    const float& maxChannelValue
) {
    assert( dst );
    assert( src );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        max_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            maxChannelValue
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        max_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            maxChannelValue
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void add(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst );
    assert( src0 );
    assert( src1 );
    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        add_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        add_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void add(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    float value
) {
    assert( dst );
    assert( src );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        add_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            value
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        add_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            value
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void add(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    assert( dst );
    assert( src );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        add_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            color
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        add_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            color
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void subtract(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst );
    assert( src0 );
    assert( src1 );
    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        subtract_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        subtract_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void subtract(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    float value
) {
    assert( dst );
    assert( src );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        subtract_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            value
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        subtract_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            value
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void subtract(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    assert( dst );
    assert( src );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        subtract_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            color
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        subtract_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            color
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void multiply(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst );
    assert( src0 );
    assert( src1 );
    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        multiply_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        multiply_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void multiply(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    float value
) {
    assert( dst );
    assert( src );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        multiply_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            value
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        multiply_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            value
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void multiply(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    assert( dst );
    assert( src );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        multiply_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            color
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        multiply_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            color
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void divide(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst );
    assert( src0 );
    assert( src1 );
    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        divide_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        divide_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void divide(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    float value
) {
    assert( dst );
    assert( src );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        divide_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            value
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        divide_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            value
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void divide(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    assert( dst );
    assert( src );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        divide_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            color
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        divide_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            color
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void grainMultiply(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst );
    assert( src0 );
    assert( src1 );
    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        grainMultiply_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        grainMultiply_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void grainMultiply(
    ImageLayer* dst,
    ImageLayer* src0,
    const libgraphics::Rect32I& area,
    float value
) {
    assert( dst != nullptr );
    assert( src0 != nullptr );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        grainMultiply_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            value
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        grainMultiply_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            value
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void applyGrainSubtract(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        applyGrainSubtract_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL )  && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        applyGrainSubtract_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void applyGrainAdd(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        applyGrainAdd_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL )  && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        applyGrainAdd_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void grainMerge(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        grainMerge_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL )  && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        grainMerge_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void alphaBlend(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area,
    float opacity
) {
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        alphaBlend_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            opacity
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        alphaBlend_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            opacity
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void alphaBlend(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        alphaBlend_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL )  && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        alphaBlend_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void screen(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        screen_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL )  && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        screen_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void overlay(
    ImageLayer* dst,
    ImageLayer* src,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    assert( dst != nullptr );
    assert( src != nullptr );
    assert( dst != nullptr );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        overlay_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area,
            color
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        overlay_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area,
            color
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void overlay(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        overlay_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        overlay_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void dodge(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        dodge_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL )  && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        dodge_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void burn(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        burn_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        burn_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void hardLight(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        hardLight_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        hardLight_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}


void grainExtract(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        grainExtract_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        grainExtract_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void difference(
    ImageLayer* dst,
    ImageLayer* src0,
    ImageLayer* src1,
    const libgraphics::Rect32I& area
) {
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src0->containsDataForBackend( FXAPI_BACKEND_CPU ) && src1->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        difference_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src0->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src1->internalImageForBackend( FXAPI_BACKEND_CPU ),
            area
        );
        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src0->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src1->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        difference_GL(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src0->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            src1->internalImageForBackend( FXAPI_BACKEND_OPENGL ),
            area
        );
        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

}
}
}
