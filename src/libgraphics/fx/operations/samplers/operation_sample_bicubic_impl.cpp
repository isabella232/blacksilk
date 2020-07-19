
#include <libgraphics/fx/operations/samplers.hpp>
#include <libgraphics/fx/operations/samplers/gen.hpp>
#include <libgraphics/fx/operations/samplers/cpu.hpp>

namespace libgraphics {
namespace fx {
namespace operations {


void areaSample2x2(
    ImageLayer* dst,
    ImageLayer* src,
    float       factor
) {
    static const float averageValue = 1.0f / 4.0f;
    static const float matrix[] = {
        averageValue, averageValue,
        averageValue, averageValue
    };

    fx::operations::sampleWeightedSum2x2(
        dst,
        src,
        factor,
        ( float* )matrix
    );
}

void areaSample3x3(
    ImageLayer* dst,
    ImageLayer* src,
    float       factor
) {
    static const float averageValue = 1.0f / 9.0f;
    static const float matrix[] = {
        averageValue, averageValue, averageValue,
        averageValue, averageValue, averageValue,
        averageValue, averageValue, averageValue
    };

    fx::operations::sampleWeightedSum3x3(
        dst,
        src,
        factor,
        ( float* )matrix
    );
}

void areaSample4x4(
    ImageLayer* dst,
    ImageLayer* src,
    float       factor
) {
    static const float averageValue = 1.0f / 16.0f;
    static const float matrix[] = {
        averageValue, averageValue, averageValue, averageValue,
        averageValue, averageValue, averageValue, averageValue,
        averageValue, averageValue, averageValue, averageValue,
        averageValue, averageValue, averageValue, averageValue
    };

    fx::operations::sampleWeightedSum4x4(
        dst,
        src,
        factor,
        ( float* )matrix
    );
}


}
}
}
