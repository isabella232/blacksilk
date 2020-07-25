
#include <libgraphics/fx/operations/basic/general/gl.hpp>
#include <libgraphics/fx/operations/helpers/gl_helpers.hpp>
#include <QDebug>
#include <QElapsedTimer>

namespace libgraphics {
namespace fx {
namespace operations {

namespace ColorOrImageOperations {
static const char opAdd[] =
    " return vec4(color.r + InputColor.r,color.g + InputColor.g,color.b + InputColor.b,color.a + InputColor.a); ";
static const char opSub[] =
    " return vec4(color.r - InputColor.r,color.g - InputColor.g,color.b - InputColor.b,color.a - InputColor.a); ";
static const char opMul[] =
    " return vec4(color.r * InputColor.r,color.g * InputColor.g,color.b * InputColor.b,color.a * InputColor.a); ";
static const char opDiv[] =
    " return vec4(color.r / InputColor.r,color.g / InputColor.g,color.b / InputColor.b,color.a / InputColor.a); ";
static const char opMin[] =
    " return vec4( min(color.r,InputColor.r), min(color.g,InputColor.g), min(color.b,InputColor.b), min(color.a,InputColor.a) );";
static const char opMax[] =
    " return vec4( max(color.r,InputColor.r), max(color.g,InputColor.g), max(color.b,InputColor.b), max(color.a,InputColor.a) );";
static const char opGrainSubtract[] =
    " return vec4(color.r - ( InputColor.r - 0.5 ),color.g - ( InputColor.g - 0.5 ),color.b - ( InputColor.b - 0.5 ),color.a - ( InputColor.a - 0.5 ) ); ";
static const char opGrainExtract[] =
    " return vec4(color.r - InputColor.r + 0.5,color.g - InputColor.g + 0.5,color.b - InputColor.b + 0.5,color.a - InputColor.a + 0.5); ";
static const char opGrainMerge[] =
    " return vec4(color.r + InputColor.r - 0.5,color.g + InputColor.g - 0.5,color.b + InputColor.b - 0.5,color.a + InputColor.a - 0.5); ";
static const char opGrainAdd[] =
    " return vec4(color.r + InputColor.r - 0.5,color.g + InputColor.g - 0.5,color.b + InputColor.b - 0.5,color.a + InputColor.a - 0.5); ";
static const char opGrainMultiply[] =
    " return vec4( (color.r - 0.5)  * (InputColor.r - 0.5) + 0.5,(color.g - 0.5) * (InputColor.g - 0.5) + 0.5,(color.b - 0.5) * (InputColor.b - 0.5) + 0.5,(color.a - 0.5) * (InputColor.a - 0.5) + 0.5); ";
static const char opNegate[] =
    " return vec4(1.0 - color.r,1.0 - color.g,1.0 - color.b,1.0 - color.a);";
static const char opMinThreshold[] =
    "   float tr = ( color.r < InputColor.r ) ? 0.0 : color.r; "
    "   float tg = ( color.g < InputColor.g ) ? 0.0 : color.g; "
    "   float tb = ( color.b < InputColor.b ) ? 0.0 : color.b; "
    "   float ta = ( color.a < InputColor.a ) ? 0.0 : color.a; "
    "   return vec4(tr,tg,tb,ta);";
static const char opMaxThreshold[] =
    "   float tr = ( color.r > InputColor.r ) ? 0.0 : color.r; "
    "   float tg = ( color.g > InputColor.g ) ? 0.0 : color.g; "
    "   float tb = ( color.b > InputColor.b ) ? 0.0 : color.b; "
    "   float ta = ( color.a > InputColor.a ) ? 0.0 : color.a; "
    "   return vec4(tr,tg,tb,ta);";
static const char opScreen[] =
    "   float tr = 1.0 - ( ( 1.0 - color.r ) * (1.0 - InputColor.r) ); "
    "   float tg = 1.0 - ( ( 1.0 - color.g ) * (1.0 - InputColor.g) ); "
    "   float tb = 1.0 - ( ( 1.0 - color.b ) * (1.0 - InputColor.b) ); "
    "   float ta = 1.0 - ( ( 1.0 - color.a ) * (1.0 - InputColor.a) ); "
    "   return vec4(tr,tg,tb,ta);";
static const char opOverlay[] =
    "   float tr = color.r * ( color.r + ( (2.0 * InputColor.r) * (1.0 - color.r) ) ); "
    "   float tg = color.g * ( color.g + ( (2.0 * InputColor.g) * (1.0 - color.g) ) ); "
    "   float tb = color.b * ( color.b + ( (2.0 * InputColor.b) * (1.0 - color.b) ) ); "
    "   float ta = color.a * ( color.a + ( (2.0 * InputColor.a) * (1.0 - color.a) ) ); "
    "   return vec4(tr,tg,tb,ta);";
static const char opDodge[] =
    "   float tr = color.r / ( 1.0 - InputColor.r );"
    "   float tg = color.g / ( 1.0 - InputColor.g );"
    "   float tb = color.b / ( 1.0 - InputColor.b );"
    "   float ta = color.a / ( 1.0 - InputColor.a );"
    "   return vec4(tr,tg,tb,ta);";
static const char opBurn[] =
    "   float tr = 1.0 - ( ( 1.0 * ( 1.0 - color.r ) ) / InputColor.r );"
    "   float tg = 1.0 - ( ( 1.0 * ( 1.0 - color.g ) ) / InputColor.g );"
    "   float tb = 1.0 - ( ( 1.0 * ( 1.0 - color.b ) ) / InputColor.b );"
    "   float ta = 1.0 - ( ( 1.0 * ( 1.0 - color.a ) ) / InputColor.a );"
    "   return vec4(tr,tg,tb,ta);";
static const char opHardLight[] =
    "   float tr = ( color.r <= 0.0 ) ? ( 2.0 * color.r * InputColor.r ) : ( 1.0f - ( ( ( ( 1.0f - 2 * (InputColor.r - (1.0f / 2) ) ) ) * (1.0f - color.r) ) / 1.0f ) ); "
    "   float tg = ( color.g <= 0.0 ) ? ( 2.0 * color.g * InputColor.g ) : ( 1.0f - ( ( ( ( 1.0f - 2 * (InputColor.g - (1.0f / 2) ) ) ) * (1.0f - color.g) ) / 1.0f ) ); "
    "   float tb = ( color.b <= 0.0 ) ? ( 2.0 * color.b * InputColor.b ) : ( 1.0f - ( ( ( ( 1.0f - 2 * (InputColor.b - (1.0f / 2) ) ) ) * (1.0f - color.b) ) / 1.0f ) ); "
    "   float ta = ( color.a <= 0.0 ) ? ( 2.0 * color.a * InputColor.a ) : ( 1.0f - ( ( ( ( 1.0f - 2 * (InputColor.a - (1.0f / 2) ) ) ) * (1.0f - color.a) ) / 1.0f ) ); "
    "   return vec4(tr,tg,tb,ta);";
static const char opDifference[] =
    " return vec4( abs(color.r - InputColor.r), abs(color.g - InputColor.g), abs(color.b - InputColor.b), abs(color.a - InputColor.a) ; ";
}

namespace ChannelOperations {
static const char opAdd[] =
    " return vec4(color.r + Value,color.g + Value,color.b + Value,color.a + Value); ";
static const char opSub[] =
    " return vec4(color.r - Value,color.g - Value,color.b - Value,color.a - Value); ";
static const char opMul[] =
    " return vec4(color.r * Value,color.g * Value,color.b * Value,color.a * Value); ";
static const char opDiv[] =
    " return vec4(color.r / Value,color.g / Value,color.b / Value,color.a / Value); ";
static const char opMin[] =
    " return vec4( min(color.r,Value), min(color.g,Value), min(color.b,Value), min(color.a,Value) );";
static const char opMax[] =
    " return vec4( max(color.r,Value), max(color.g,Value), max(color.b,Value), max(color.a,Value) );";
static const char opGrainSubtract[] =
    " return vec4(color.r - ( Value - 0.5 ),color.g - ( Value - 0.5 ),color.b - ( Value - 0.5 ),color.a - ( Value - 0.5 ) ); ";
static const char opGrainExtract[] =
    " return vec4(color.r - Value + 0.5,color.g - Value + 0.5,color.b - Value + 0.5,color.a - Value + 0.5); ";
static const char opGrainMerge[] =
    " return vec4(color.r + Value - 0.5,color.g + Value - 0.5,color.b + Value - 0.5,color.a + Value - 0.5); ";
static const char opGrainAdd[] =
    " return vec4(color.r + Value - 0.5,color.g + Value - 0.5,color.b + Value - 0.5,color.a + Value - 0.5); ";
static const char opGrainMultiply[] =
    " return vec4( (color.r - 0.5) * (Value - 0.5) + 0.5,(color.g - 0.5) * (Value - 0.5) + 0.5,(color.b - 0.5) * (Value - 0.5) + 0.5,(color.a - 0.5) * Value + 0.5); ";
static const char opNegate[] =
    " return vec4(Value - color.r,Value - color.g,Value - color.b,Value - color.a);";
static const char opMinThreshold[] =
    "   float tr = ( color.r < Value ) ? 0.0 : color.r; "
    "   float tg = ( color.g < Value ) ? 0.0 : color.g; "
    "   float tb = ( color.b < Value ) ? 0.0 : color.b; "
    "   float ta = ( color.a < Value ) ? 0.0 : color.a; "
    "   return vec4(tr,tg,tb,ta);";
static const char opMaxThreshold[] =
    "   float tr = ( color.r > Value ) ? 0.0 : color.r; "
    "   float tg = ( color.g > Value ) ? 0.0 : color.g; "
    "   float tb = ( color.b > Value ) ? 0.0 : color.b; "
    "   float ta = ( color.a > Value ) ? 0.0 : color.a; "
    "   return vec4(tr,tg,tb,ta);";
static const char opScreen[] =
    "   float tr = 1.0 - ( ( 1.0 - color.r ) * (1.0 - Value) ); "
    "   float tg = 1.0 - ( ( 1.0 - color.g ) * (1.0 - Value) ); "
    "   float tb = 1.0 - ( ( 1.0 - color.b ) * (1.0 - Value) ); "
    "   float ta = 1.0 - ( ( 1.0 - color.a ) * (1.0 - Value) ); "
    "   return vec4(tr,tg,tb,ta);";
static const char opOverlay[] =
    "   float tr = color.r * ( color.r + ( (2.0 * Value) * (1.0 - color.r) ) ); "
    "   float tg = color.g * ( color.g + ( (2.0 * Value) * (1.0 - color.g) ) ); "
    "   float tb = color.b * ( color.b + ( (2.0 * Value) * (1.0 - color.b) ) ); "
    "   float ta = color.a * ( color.a + ( (2.0 * Value) * (1.0 - color.a) ) ); "
    "   return vec4(tr,tg,tb,ta);";
static const char opDodge[] =
    "   float tr = color.r / ( 1.0 - Value );"
    "   float tg = color.g / ( 1.0 - Value );"
    "   float tb = color.b / ( 1.0 - Value );"
    "   float ta = color.a / ( 1.0 - Value );"
    "   return vec4(tr,tg,tb,ta);";
static const char opBurn[] =
    "   float tr = 1.0 - ( ( 1.0 * ( 1.0 - color.r ) ) / Value );"
    "   float tg = 1.0 - ( ( 1.0 * ( 1.0 - color.g ) ) / Value );"
    "   float tb = 1.0 - ( ( 1.0 * ( 1.0 - color.b ) ) / Value );"
    "   float ta = 1.0 - ( ( 1.0 * ( 1.0 - color.a ) ) / Value );"
    "   return vec4(tr,tg,tb,ta);";
static const char opHardLight[] =
    "   float tr = ( color.r <= 0.0 ) ? ( 2.0 * color.r * Value ) : ( 1.0f - ( ( ( ( 1.0f - 2 * (Value - (1.0f / 2) ) ) ) * (1.0f - color.r) ) / 1.0f ) ); "
    "   float tg = ( color.g <= 0.0 ) ? ( 2.0 * color.g * Value ) : ( 1.0f - ( ( ( ( 1.0f - 2 * (Value - (1.0f / 2) ) ) ) * (1.0f - color.g) ) / 1.0f ) ); "
    "   float tb = ( color.b <= 0.0 ) ? ( 2.0 * color.b * Value ) : ( 1.0f - ( ( ( ( 1.0f - 2 * (Value - (1.0f / 2) ) ) ) * (1.0f - color.b) ) / 1.0f ) ); "
    "   float ta = ( color.a <= 0.0 ) ? ( 2.0 * color.a * Value ) : ( 1.0f - ( ( ( ( 1.0f - 2 * (Value - (1.0f / 2) ) ) ) * (1.0f - color.a) ) / 1.0f ) ); "
    "   return vec4(tr,tg,tb,ta);";

static const char opNormalize[] =
    "   float total = (color.r * color.r) + (color.g * color.g) + (color.b * color.b);"
    "   float val   = sqrt(total);"
    "   return vec4(val,val,val,val);";
static const char opDifference[] =
    " return vec4( abs(color.r - Value), abs(color.g - Value), abs(color.b - Value), abs(color.a - Value) ; ";
}


/// operation: applyGrainSubtract
void applyGrainSubtract_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opGrainSubtract ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

/// operation: applyGrainAdd
void applyGrainAdd_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opGrainAdd ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

/// operation: grain multiply
void grainMultiply_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpChannelValue, device, filter, makeOpDirective( ChannelOperations::opGrainMultiply ) );

    filter->value       = value;

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void grainMultiply_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opGrainMultiply ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

/// operation: normalize
void normalize_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpChannelValue, device, filter, makeOpDirective( ChannelOperations::opNormalize ) );

    filter->value       = 1.0f;

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

/// operation: minThreshold
void minThreshold_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float minimalValue
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpChannelValue, device, filter, makeOpDirective( ChannelOperations::opMinThreshold ) );

    filter->value       = minimalValue;

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

/// operation: maxThreshold
void maxThreshold_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float maximalValue
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpChannelValue, device, filter, makeOpDirective( ChannelOperations::opMaxThreshold ) );

    filter->value       = maximalValue;

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

/// operation: negate
void negate_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpChannelValue, device, filter, makeOpDirective( ChannelOperations::opNegate ) );

    filter->value       = 1.0f;

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

/// operation: min
void min_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opMin ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void min_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I area,
    const float& minimalChannelValue
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpChannelValue, device, filter, makeOpDirective( ChannelOperations::opMin ) );

    filter->value       = minimalChannelValue;

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

/// operation: max
void max_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opMax ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void max_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I area,
    const float& maxChannelValue
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpChannelValue, device, filter, makeOpDirective( ChannelOperations::opMax ) );

    filter->value       = maxChannelValue;

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

/// operation: add
void add_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opAdd ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void add_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpChannelValue, device, filter, makeOpDirective( ChannelOperations::opAdd ) );

    filter->value       = value;

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void add_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpColorValue, device, filter, makeOpDirective( ColorOrImageOperations::opAdd ) );

    filter->value.data[0]       = color.Values[0];
    filter->value.data[1]       = color.Values[1];
    filter->value.data[2]       = color.Values[2];
    filter->value.data[3]       = color.Values[3];

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

/// operation: subtract
void subtract_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opSub ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void subtract_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpChannelValue, device, filter, makeOpDirective( ChannelOperations::opSub ) );

    filter->value       = value;

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void subtract_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpColorValue, device, filter, makeOpDirective( ColorOrImageOperations::opSub ) );

    filter->value.data[0]       = color.Values[0];
    filter->value.data[1]       = color.Values[1];
    filter->value.data[2]       = color.Values[2];
    filter->value.data[3]       = color.Values[3];

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

/// operation: multiply
void multiply_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opMul ) );


    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void multiply_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpChannelValue, device, filter, makeOpDirective( ChannelOperations::opMul ) );

    filter->value       = value;

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void multiply_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpColorValue, device, filter, makeOpDirective( ColorOrImageOperations::opMul ) );

    filter->value.data[0]       = color.Values[0];
    filter->value.data[1]       = color.Values[1];
    filter->value.data[2]       = color.Values[2];
    filter->value.data[3]       = color.Values[3];


    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

/// operation: divide
void divide_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opDiv ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void divide_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpChannelValue, device, filter, makeOpDirective( ChannelOperations::opDiv ) );

    filter->value       = value;

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void divide_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpColorValue, device, filter, makeOpDirective( ColorOrImageOperations::opDiv ) );

    filter->value.data[0]       = color.Values[0];
    filter->value.data[1]       = color.Values[1];
    filter->value.data[2]       = color.Values[2];
    filter->value.data[3]       = color.Values[3];

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void grainMerge_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opGrainMerge ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void alphaBlend_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area,
    float opacity
) {
    assert( false );
}

void alphaBlend_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    assert( false );
}

void screen_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opScreen ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void overlay_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opOverlay ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void overlay_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpColorValue, device, filter, makeOpDirective( ColorOrImageOperations::opOverlay ) );

    filter->value.data[0]       = color.Values[0];
    filter->value.data[1]       = color.Values[1];
    filter->value.data[2]       = color.Values[2];
    filter->value.data[3]       = color.Values[3];

    const auto rendered = renderTiledImageOperation(
                              device,
                              dst,
                              src0,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void dodge_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opDodge ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void burn_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opBurn ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void hardLight_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opHardLight ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void grainExtract_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opGrainExtract ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

void difference_GL(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    LIBGRAPHICS_FILTER_INSTANCE_EX( GenericImgOpImageValue, device, filter, makeOpDirective( ColorOrImageOperations::opDifference ) );

    const auto rendered = renderTiledImageMergeOperation(
                              device,
                              dst,
                              src0,
                              src1,
                              area,
                              filter.get()
                          );
    assert( rendered );

    if( !rendered ) {
        qDebug() << "add_GL: Failed to render operation.";
    }
}

}
}
}
