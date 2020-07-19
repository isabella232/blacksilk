
#include <libgraphics/fx/operations/samplers.hpp>
#include <libgraphics/fx/operations/samplers/gen.hpp>
#include <libgraphics/fx/operations/samplers/cpu.hpp>

#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

void sampleWeightedSum2x2(
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    assert( dst );
    assert( src );
    assert( !dst->empty() );
    assert( !src->empty() );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        sampleWeightedSum2x2_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            factor,
            weights
        );
        dst->updateInternalState( FXAPI_BACKEND_CPU );

        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        sampleWeightedSum2x2_GEN(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst,
            src,
            factor,
            weights
        );
        dst->updateInternalState( FXAPI_BACKEND_OPENGL );

        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void sampleWeightedSum3x3(
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    assert( dst );
    assert( src );
    assert( !dst->empty() );
    assert( !src->empty() );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        sampleWeightedSum3x3_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            factor,
            weights
        );
        dst->updateInternalState( FXAPI_BACKEND_CPU );

        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        sampleWeightedSum3x3_GEN(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst,
            src,
            factor,
            weights
        );
        dst->updateInternalState( FXAPI_BACKEND_OPENGL );

        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void sampleWeightedSum4x4(
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    assert( dst );
    assert( src );
    assert( !dst->empty() );
    assert( !src->empty() );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        sampleWeightedSum4x4_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            factor,
            weights
        );
        dst->updateInternalState( FXAPI_BACKEND_CPU );

        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        sampleWeightedSum4x4_GEN(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst,
            src,
            factor,
            weights
        );
        dst->updateInternalState( FXAPI_BACKEND_OPENGL );

        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void sampleWeightedSum5x5(
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    assert( dst );
    assert( src );
    assert( !dst->empty() );
    assert( !src->empty() );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        sampleWeightedSum5x5_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            factor,
            weights
        );
        dst->updateInternalState( FXAPI_BACKEND_CPU );

        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        sampleWeightedSum5x5_GEN(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst,
            src,
            factor,
            weights
        );
        dst->updateInternalState( FXAPI_BACKEND_OPENGL );

        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}

void sampleWeightedSum6x6(
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    assert( dst );
    assert( src );
    assert( !dst->empty() );
    assert( !src->empty() );

    bool rendered( false );

    if( dst->containsDataForBackend( FXAPI_BACKEND_CPU ) && src->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
        sampleWeightedSum6x6_CPU(
            dst->internalDeviceForBackend( FXAPI_BACKEND_CPU ),
            dst->internalImageForBackend( FXAPI_BACKEND_CPU ),
            src->internalImageForBackend( FXAPI_BACKEND_CPU ),
            factor,
            weights
        );
        dst->updateInternalState( FXAPI_BACKEND_CPU );

        rendered = true;
    }

    if( dst->containsDataForBackend( FXAPI_BACKEND_OPENGL ) && src->containsDataForBackend( FXAPI_BACKEND_OPENGL ) ) {
        sampleWeightedSum6x6_GEN(
            dst->internalDeviceForBackend( FXAPI_BACKEND_OPENGL ),
            dst,
            src,
            factor,
            weights
        );
        dst->updateInternalState( FXAPI_BACKEND_OPENGL );

        rendered = true;
    }

    assert( rendered );
    ( void ) rendered;
}




}
}
}
