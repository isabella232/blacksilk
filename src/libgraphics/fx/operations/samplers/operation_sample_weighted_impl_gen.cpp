
#include <assert.h>
#include <QDebug>
#include <math.h>

#include <libgraphics/image.hpp>

#include <libgraphics/fx/operations/basic.hpp>
#include <libgraphics/fx/operations/samplers.hpp>
#include <libgraphics/fx/operations/samplers/gen.hpp>
#include <libgraphics/fx/operations/samplers/cpu.hpp>

#include <libgraphics/fx/operations/helpers/cpu_helpers.hpp>
#include <libgraphics/backend/cpu/cpu_backenddevice.hpp>
#include <libgraphics/backend/cpu/cpu_imageobject.hpp>

namespace libgraphics {
namespace fx {
namespace operations {

typedef void( *SampleFn )(
    libgraphics::fxapi::ApiBackendDevice*,
    libgraphics::fxapi::ApiImageObject*,
    libgraphics::fxapi::ApiImageObject*,
    float,
    float*
);

void sampleWeightedSumTemplate(
    SampleFn    fn,
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    assert( fn != nullptr );
    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src != nullptr );
    assert( weights != nullptr );
    assert( factor > 0.0f );

    libgraphics::backend::cpu::ImageObject  clonedSource(
        device->allocator().get(),
        src->format(),
        src->width(),
        src->height()
    );

    const auto data = clonedSource.allocator()->alloc(
                          src->width() * src->height() * libgraphics::fxapi::EPixelFormat::getPixelSize( clonedSource.format() )
                      );
    void* buffer = data->data;
    assert( buffer != nullptr );

    const auto successfullyRetrievedData = src->retrieve(
            buffer
                                           );
    assert( successfullyRetrievedData );

    if( !successfullyRetrievedData ) {
        qDebug() << "Error: Failed to retrieve data from source image. Downsampling failed.";
        return;
    }

    libgraphics::backend::cpu::ImageObject  clonedDestination(
        device->allocator().get(),
        dst->format(),
        dst->width(),
        dst->height()
    );

    libgraphics::fx::operations::sampleWeightedSum2x2_CPU(
        device,
        ( fxapi::ApiImageObject* )&clonedDestination,
        ( fxapi::ApiImageObject* )&clonedSource,
        factor,
        weights
    );

    const auto dstData = clonedSource.allocator()->alloc(
                             clonedDestination.width() * clonedDestination.height() * libgraphics::fxapi::EPixelFormat::getPixelSize( clonedDestination.format() )
                         );
    assert( dstData->data != nullptr );

    const auto retrievedSampledData = clonedDestination.retrieve(
                                          dstData->data
                                      );
    assert( retrievedSampledData );

    if( !retrievedSampledData ) {
        qDebug() << "Error: Failed to retrieve sampled destination data. Downsampling failed.";
        return;
    }

    const auto successfullyResetDestination = dst->reset(
                clonedDestination.format(),
                clonedDestination.width(),
                clonedDestination.height()
            );
    assert( successfullyResetDestination );

    if( !successfullyResetDestination ) {
        qDebug() << "Error: Failed to reset destination buffer.";
        return;
    }

    const auto successfullyCopiedData = dst->copy(
                                            data->data,
                                            libgraphics::Rect32I( clonedDestination.width(), clonedDestination.height() ),
                                            0,
                                            0
                                        );
    assert( successfullyCopiedData );

    if( !successfullyCopiedData ) {
        qDebug() << "Error: Failed to copy sampled data to destination.";
        return;
    }
}

void sampleWeightedSum2x2_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    sampleWeightedSumTemplate(
        &fx::operations::sampleWeightedSum2x2_CPU,
        device,
        dst,
        src,
        factor,
        weights
    );
}

void sampleWeightedSum3x3_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    sampleWeightedSumTemplate(
        &fx::operations::sampleWeightedSum3x3_CPU,
        device,
        dst,
        src,
        factor,
        weights
    );
}

void sampleWeightedSum4x4_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    sampleWeightedSumTemplate(
        &fx::operations::sampleWeightedSum4x4_CPU,
        device,
        dst,
        src,
        factor,
        weights
    );
}

void sampleWeightedSum5x5_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    sampleWeightedSumTemplate(
        &fx::operations::sampleWeightedSum5x5_CPU,
        device,
        dst,
        src,
        factor,
        weights
    );
}

void sampleWeightedSum6x6_GEN(
    libgraphics::fxapi::ApiBackendDevice* device,
    ImageLayer* dst,
    ImageLayer* src,
    float       factor, /// between 0.001 and 0.999
    float*      weights
) {
    sampleWeightedSumTemplate(
        &fx::operations::sampleWeightedSum6x6_CPU,
        device,
        dst,
        src,
        factor,
        weights
    );
}

}
}
}
