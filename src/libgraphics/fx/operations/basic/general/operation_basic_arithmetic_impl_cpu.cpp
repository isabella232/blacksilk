
#include <QDebug>

#include <libgraphics/fx/operations/basic/general/cpu.hpp>
#include <libgraphics/fx/operations/helpers/cpu_helpers.hpp>
#include <libgraphics/backend/cpu/cpu_backenddevice.hpp>
#include <libgraphics/backend/cpu/cpu_imageobject.hpp>
#include <libgraphics/backend/cpu/cpu_imageoperation.hpp>
#include <utility>

namespace libgraphics {
namespace fx {
namespace operations {


#define OP_MIN_MAX(x) std::max<int>( 0, std::min<int>( std::numeric_limits<ValueType>::max(), x ) )

template < class _t_any >
struct op_base {
    typedef _t_any ValueType;

    op_base() : maxVal( std::numeric_limits<ValueType>::max() ),
        minVal( std::numeric_limits<ValueType>::min() ) {}

    const ValueType maxVal;
    const ValueType minVal;
};
template <>
struct op_base<float> {
    typedef float ValueType;

    op_base() : maxVal( 1.0f ),
        minVal( 0.0f ) {}

    const ValueType maxVal;
    const ValueType minVal;
};
template <>
struct op_base<double> {
    typedef double ValueType;

    op_base() : maxVal( 1.0 ),
        minVal( 0.0 ) {}

    const ValueType maxVal;
    const ValueType minVal;
};

template < class _t_any >
struct op_grain_multiply : op_base<_t_any> {
    typedef _t_any ValueType;

    op_grain_multiply() {}

    inline ValueType operator()( const ValueType& first, const ValueType& second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        const int basic_value0 = ( first - ( max_value / 2 ) );
        const int basic_value1 = ( second - ( max_value / 2 ) );

        return static_cast<_t_any>(
                   OP_MIN_MAX( ( basic_value0 * basic_value1 ) + ( max_value / 2 ) )
               );
    }
    inline ValueType operator()( const ValueType& first, const float& second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        const int basic_value0 = ( first - ( max_value / 2 ) );

        return static_cast<_t_any>(
                   OP_MIN_MAX( std::ceil( basic_value0 * second ) + ( max_value / 2 ) )
               );
    }
};
template <>
struct op_grain_multiply<float> : op_base<float> {
    typedef float ValueType;

    op_grain_multiply() {}

    inline ValueType operator()( float first, float second ) const {
        return ( first * second ) + 0.5f;
    }
};

template < class _t_any >
struct op_add : op_base<_t_any> {
    typedef _t_any ValueType;

    op_add() {}

    inline ValueType operator()( const ValueType& first, const ValueType& second ) const {
        return std::max( ( int )op_base<_t_any>::minVal, std::min( ( int )op_base<_t_any>::maxVal, ( int )first + ( int )second ) );
    }
    inline ValueType operator()( const ValueType& first, const float& second ) const {
        return std::max( ( int )op_base<_t_any>::minVal, std::min<int>( ( int )op_base<_t_any>::maxVal, ( int )first + std::ceil( second * op_base<_t_any>::maxVal ) ) );
    }
};
template <>
struct op_add<float> : op_base<float> {
    typedef float ValueType;

    op_add() {}
    inline float operator()( float first, float second ) const {
        return first + second;
    }
};

template < class _t_any >
struct op_sub : op_base<_t_any> {
    typedef _t_any ValueType;

    op_sub() {}

    inline ValueType operator()( const ValueType& first, const ValueType& second ) const {
        return std::max( ( int )op_base<_t_any>::minVal, std::min( ( int )op_base<_t_any>::maxVal, ( int )first - ( int )second ) );
    }
    inline ValueType operator()( const ValueType& first, const float& second ) const {
        return std::max( ( int )op_base<_t_any>::minVal, std::min<int>( ( int )op_base<_t_any>::maxVal, ( int )first - std::ceil( second * op_base<_t_any>::maxVal ) ) );
    }
};
template <>
struct op_sub<float> : op_base<float> {
    typedef float ValueType;

    op_sub() {}
    inline ValueType operator()( float first, float second ) const {
        return first - second;
    }
};

template < class _t_any >
struct op_mul : op_base<_t_any> {
    typedef _t_any ValueType;

    op_mul() {}

    inline ValueType operator()( const ValueType& first, const ValueType& second ) const {
        const auto relative = libgraphics::mapValueRangeToFloat( second, op_base<_t_any>::minVal, op_base<_t_any>::maxVal );
        return std::max( ( int )op_base<_t_any>::minVal, std::min( ( int )op_base<_t_any>::maxVal, ( int )std::ceil( first * relative ) ) );
    }
    inline ValueType operator()( const ValueType& first, const float& second ) const {
        return std::max( ( int )op_base<_t_any>::minVal, std::min<int>( ( int )op_base<_t_any>::maxVal, ( int )std::ceil( ( double )first * ( double )second ) ) );
    }
};
template <>
struct op_mul<float> : op_base<float> {
    typedef float ValueType;

    op_mul() {}

    inline float operator()( float first, float second ) const {
        return std::max( 0.0f, std::min<float>( 1.0f, first * second ) );
    }
};

template < class _t_any >
struct op_div : op_base<_t_any> {
    typedef _t_any ValueType;

    op_div() {}

    inline ValueType operator()( const ValueType& first, ValueType second ) const {
        second = ( second == 0 ) ? 1 : second;
        return std::max( ( int )op_base<_t_any>::minVal, std::min( ( int )op_base<_t_any>::maxVal, ( int )first / ( int )second ) );
    }
    inline ValueType operator()( const ValueType& first, float second ) const {
        return std::max( ( int )op_base<_t_any>::minVal, std::min<int>( ( int )op_base<_t_any>::maxVal, ( int )std::ceil( first / second ) ) );
    }
};
template <>
struct op_div<float> : op_base<float> {
    typedef float ValueType;

    op_div() {}
    inline float operator()( float first, float second ) const {
        assert( second != 0.0f );
        return ( second == 0.0f ) ? 1.0f : ( first / second );
    }
};

template < class _t_any >
struct op_negate : op_base<_t_any> {
    typedef _t_any ValueType;

    op_negate() {}

    inline ValueType operator()( const ValueType& first ) const {
        return ( op_base<_t_any>::maxVal - first );
    }
};
template <>
struct op_negate<float> : op_base<float> {
    typedef float ValueType;

    op_negate() {}
    inline float operator()( float first ) const {
        return ( 1.0f - first );
    }
};

template < class _t_any >
struct op_min : op_base<_t_any> {
    typedef _t_any ValueType;

    op_min() {}

    inline ValueType operator()( const ValueType& first, ValueType second ) const {
        return std::min<ValueType>(
                   first,
                   second
               );
    }
    inline ValueType operator()( const ValueType& first, float second ) const {
        return std::min<ValueType>(
                   first,
                   std::min<ValueType>( op_base<_t_any>::maxVal, std::max<ValueType>( op_base<_t_any>::minVal, ( ValueType )std::ceil( op_base<_t_any>::maxVal * second ) ) )
               );
    }
};
template <>
struct op_min<float> : op_base<float> {
    typedef float ValueType;

    op_min() {}

    inline float operator()( float first, float second ) const {
        return std::min<float>(
                   first,
                   second
               );
    }
};

template < class _t_any >
struct op_max : op_base<_t_any> {
    typedef _t_any ValueType;

    op_max() {}

    inline ValueType operator()( const ValueType& first, ValueType second ) const {
        return std::max<ValueType>(
                   first,
                   second
               );
    }
    inline ValueType operator()( const ValueType& first, float second ) const {
        return std::max<ValueType>(
                   first,
                   std::max<ValueType>( op_base<_t_any>::minVal, std::min<ValueType>( op_base<_t_any>::maxVal, ( ValueType )std::ceil( op_base<_t_any>::maxVal * second ) ) )
               );
    }
};
template <>
struct op_max<float> : op_base<float> {
    typedef float ValueType;

    op_max() {}
    inline ValueType operator()( float first, float second ) const {
        return std::max<float>(
                   first,
                   second
               );
    }
};

template < class _t_any >
struct op_min_threshold : op_base<_t_any> {
    typedef _t_any ValueType;

    op_min_threshold() {}

    inline ValueType operator()( const ValueType& first, ValueType second ) const {
        return ( first < second ) ? op_base<_t_any>::minVal : first;
    }
    inline ValueType operator()( const ValueType& first, float second ) const {
        const auto fvalue = std::max<ValueType>( op_base<_t_any>::minVal, std::min<ValueType>( op_base<_t_any>::maxVal, ( ValueType )std::ceil( op_base<_t_any>::maxVal * second ) ) );
        return ( first < fvalue ) ? op_base<_t_any>::minVal : first;
    }
};
template <>
struct op_min_threshold<float> : op_base<float> {
    typedef float ValueType;

    op_min_threshold() {}

    inline ValueType operator()( float first, float second ) const {
        return ( first < second ) ? second : first;
    }
};

template < class _t_any >
struct op_max_threshold : op_base<_t_any> {
    typedef _t_any ValueType;

    op_max_threshold() {}

    inline ValueType operator()( const ValueType& first, ValueType second ) const {
        return ( first > second ) ? op_base<_t_any>::minVal : first;
    }
    inline ValueType operator()( const ValueType& first, float second ) const {
        const auto fvalue = std::max<ValueType>( op_base<_t_any>::minVal, std::min<ValueType>( op_base<_t_any>::maxVal, ( ValueType )std::ceil( op_base<_t_any>::maxVal * second ) ) );
        return ( first > fvalue ) ? op_base<_t_any>::minVal : first;
    }
};
template <>
struct op_max_threshold<float> : op_base<float> {
    typedef float ValueType;

    op_max_threshold() {}

    inline ValueType operator()( float first, float second ) const {
        const auto fvalue = std::max<float>( 0.0f, std::min<float>( 1.0f, second ) );
        return ( first > fvalue ) ? 0.0f : first;
    }
};

template < class _t_any >
struct op_screen : op_base<_t_any> {
    typedef _t_any ValueType;

    op_screen() {}

    inline ValueType operator()( const ValueType& first, ValueType second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        return static_cast<ValueType>( OP_MIN_MAX( ( max_value ) - ( ( int )( max_value - first ) * ( int )( max_value - second ) * max_value ) ) );
    }
    inline ValueType operator()( const ValueType& first, float second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        const auto isecond   = std::ceil( max_value * second );
        return static_cast<ValueType>( OP_MIN_MAX( ( max_value ) - ( ( int )( max_value - first ) * ( int )( max_value - isecond ) * max_value ) ) );
    }
};
template <>
struct op_screen<float> : op_base<float> {
    typedef float ValueType;

    op_screen() {}

    inline ValueType operator()( float first, float second ) const {
        return static_cast<ValueType>( 1.0f - ( ( 1.0f - first ) * ( 1.0f - second ) ) );
    }
};

template < class _t_any >
struct op_overlay : op_base<_t_any> {
    typedef _t_any ValueType;

    op_overlay() {}

    inline ValueType operator()( const ValueType& first, ValueType second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        const int value = std::ceil(
                              ( float )( ( float )first / ( float )max_value ) *
                              ( float )( ( float )first + ( ( 2.0f * ( float )second ) / ( float )max_value ) * ( float )( max_value - first ) )
                          );
        return static_cast<ValueType>( OP_MIN_MAX( value ) );
    }
    inline ValueType operator()( const ValueType& first, float second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        const float isecond   = second * max_value;
        const int value = std::ceil(
                              ( float )( ( float )first / ( float )max_value ) *
                              ( float )( ( float )first + ( ( 2.0f * ( float )isecond ) / ( float )max_value ) * ( float )( max_value - first ) )
                          );
        return static_cast<ValueType>( OP_MIN_MAX( value ) );
    }
};
template <>
struct op_overlay<float> : op_base<float> {
    typedef float ValueType;

    op_overlay() {}

    inline ValueType operator()( float first, float second ) const {
        return static_cast<ValueType>(
                   first *
                   ( first + ( ( 2 * second ) ) * ( 1.0f - first ) )
               );
    }
};

template < class _t_any >
struct op_dodge : op_base<_t_any> {
    typedef _t_any ValueType;

    op_dodge() {}

    inline ValueType operator()( const ValueType& first, ValueType second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        return static_cast<ValueType>( OP_MIN_MAX( ( ( int )( max_value + 1 ) * first ) / ( ( max_value - second ) + 1 ) ) );
    }
    inline ValueType operator()( const ValueType& first, float second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        const auto isecond   = std::ceil( max_value * second );
        return static_cast<ValueType>( OP_MIN_MAX( ( ( int )( max_value + 1 ) * first ) / ( ( max_value - isecond ) + 1 ) ) );
    }
};
template <>
struct op_dodge<float> : op_base<float> {
    typedef float ValueType;

    op_dodge() {}

    inline ValueType operator()( float first, float second ) const {
        return static_cast<ValueType>( ( 1.0f * first ) / ( 1.0f - second ) );
    }
};

template < class _t_any >
struct op_grain_merge : op_base<_t_any> {
    typedef _t_any ValueType;

    op_grain_merge() {}

    inline ValueType operator()( const ValueType& first, ValueType second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        return OP_MIN_MAX( static_cast<ValueType>( ( int )first + ( int )second - ( max_value / 2 ) ) );
    }
    inline ValueType operator()( const ValueType& first, float second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        const auto isecond   = std::ceil( max_value * second );
        return OP_MIN_MAX( static_cast<ValueType>( ( int )first + ( int )isecond - ( max_value / 2 ) ) );
    }
};
template <>
struct op_grain_merge<float> : op_base<float> {
    typedef float ValueType;

    op_grain_merge() {}

    inline ValueType operator()( float first, float second ) const {
        return static_cast<ValueType>( first + second - ( 1.0 / 2 ) );
    }
};

template < class _t_any >
struct op_grain_extract : op_base<_t_any> {
    typedef _t_any ValueType;

    op_grain_extract() {}

    inline ValueType operator()( const ValueType& first, ValueType second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        return OP_MIN_MAX( ( ( int )first - ( int )second + ( max_value / 2 ) ) ); //std::min<int>( max_value, std::max<int>( 0, ( (int)first - (int)second + (max_value / 2) ) ) );
    }
    inline ValueType operator()( const ValueType& first, float second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        const auto isecond   = std::ceil( max_value * second );
        return OP_MIN_MAX( static_cast<ValueType>( ( int )first - ( int )isecond + ( max_value / 2 ) ) );
    }
};
template <>
struct op_grain_extract<float> : op_base<float> {
    typedef float ValueType;

    op_grain_extract() {}

    inline ValueType operator()( float first, float second ) const {
        return first - second + 0.5f;
    }
};

template < class _t_any >
struct op_burn : op_base<_t_any> {
    typedef _t_any ValueType;

    op_burn() {}

    inline ValueType operator()( const ValueType& first, ValueType second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        return static_cast<ValueType>( OP_MIN_MAX( max_value - ( ( ( max_value + 1 ) * ( max_value - first ) ) / ( second + 1 ) ) ) );
    }
    inline ValueType operator()( const ValueType& first, float second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        const auto isecond   = std::ceil( max_value * second );
        return static_cast<ValueType>( OP_MIN_MAX( max_value - ( ( ( max_value + 1 ) * ( max_value - first ) ) / ( isecond + 1 ) ) ) );
    }
};
template <>
struct op_burn<float> : op_base<float> {
    typedef float ValueType;

    op_burn() {}

    inline ValueType operator()( const ValueType& first, float second ) const {
        return static_cast<ValueType>( OP_MIN_MAX( 1.0 - ( ( 1.0 * ( 1 - first ) ) / second ) ) );
    }
};

template < class _t_any >
struct op_hard_light : op_base<_t_any> {
    typedef _t_any ValueType;

    op_hard_light() {}

    inline ValueType operator()( const ValueType& first, ValueType second ) const {
        const auto max_value = op_base<_t_any>::maxVal;

        if( first <= ( max_value / 2 ) ) {
            return OP_MIN_MAX( ( 2 * first * second ) / ( max_value + 1 ) );
        } else {
            return OP_MIN_MAX( max_value - ( ( ( ( max_value - 2 * ( second - ( max_value / 2 ) ) ) ) * ( max_value - first ) ) / ( max_value + 1 ) ) );
        }
    }
    inline ValueType operator()( const ValueType& first, float second ) const {
        const auto max_value = op_base<_t_any>::maxVal;
        const auto isecond   = std::ceil( max_value * second );

        if( first <= ( max_value / 2 ) ) {
            return OP_MIN_MAX( ( 2 * first * isecond ) / ( max_value + 1 ) );
        } else {
            return OP_MIN_MAX( max_value - ( ( ( ( max_value - 2 * ( isecond - ( max_value / 2 ) ) ) ) * ( max_value - first ) ) / ( max_value + 1 ) ) );
        }
    }
};
template <>
struct op_hard_light<float> : op_base<float> {
    typedef float ValueType;

    op_hard_light() {}
    inline ValueType operator()( float first, float second ) const {
        if( first <= 0.0f ) {
            return ( 2.0 * first * second ) / 1.0f;
        } else {
            return 1.0f - ( ( ( ( 1.0f - 2 * ( second - ( 1.0f / 2 ) ) ) ) * ( 1.0f - first ) ) / 1.0f );
        }
    }
};

template < class _t_any >
struct op_difference : public op_base<_t_any> {
    typedef _t_any ValueType;

    op_difference() {}

    inline ValueType operator()( const ValueType& first, ValueType second ) const {
        return static_cast<ValueType>( abs( ( int )first - ( int )second ) );
    }
    inline ValueType operator()( const ValueType& first, float second ) const {
        const auto isecond   = std::ceil( op_base<_t_any>::maxVal * second );
        return static_cast<ValueType>( abs( ( int )first - ( int )isecond ) );
    }
};
template <>
struct op_difference<float> : op_base<float> {
    typedef float ValueType;

    op_difference() {}

    inline ValueType operator()( const ValueType& first, ValueType second ) const {
        return static_cast<ValueType>( abs( ( int )( first - second ) ) );
    }
};

template < class _t_any >
struct op_apply_grain_subtract : op_base<_t_any> {
    typedef _t_any ValueType;

    op_apply_grain_subtract() {}

    inline ValueType operator()( ValueType first, ValueType second ) const {
        const auto max_value = op_base<_t_any>::maxVal;

        return static_cast<ValueType>(
                   OP_MIN_MAX( ( int )first - ( ( int )second - ( int )( max_value / 2 ) ) )
               );
    }
};
template <>
struct op_apply_grain_subtract<float> : op_base<float> {
    typedef float ValueType;

    op_apply_grain_subtract() {}

    inline ValueType operator()( float first, float second ) const {
        return static_cast<ValueType>(
                   first - ( second - 0.5f )
               );
    }
};
template < class _t_any >
struct op_apply_grain_add : op_base<_t_any> {
    typedef _t_any ValueType;

    op_apply_grain_add() {}

    inline ValueType operator()( ValueType first, ValueType second ) const {
        const auto max_value = op_base<_t_any>::maxVal;

        return static_cast<ValueType>(
                   OP_MIN_MAX( ( int )first + ( ( int )second - ( int )( max_value / 2 ) ) )
               );
    }
};
template <>
struct op_apply_grain_add<float> : op_base<float> {
    typedef float ValueType;

    op_apply_grain_add() {}

    inline ValueType operator()( float first, float second ) const {
        return static_cast<ValueType>(
                   first + ( second - 0.5f )
               );
    }
};

template < class _t_op, class _t_pixel_type >
void generic_operation_kernel_img(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   dst,
    libgraphics::backend::cpu::ImageObject*   src1,
    libgraphics::backend::cpu::ImageObject*   src2,
    libgraphics::Rect32I area
) {
    static const _t_op op;

    ( void )device;

    assert( dst );
    assert( device );
    assert( src1 );
    assert( src2 );

    void*   destinationBuffer = dst->data();
    void*   sourceBuffer0 = src1->data();
    void*   sourceBuffer1 = src2->data();

    size_t pixelLength = libgraphics::fxapi::EPixelFormat::getPixelSize(
                             dst->format()
                         );
    size_t channelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                              dst->format()
                          );

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );

        _t_pixel_type* dstPixel = ( _t_pixel_type* )( ( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * dst->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* srcPixel0 = ( _t_pixel_type* )( ( ( char* )sourceBuffer0 ) + ( ( ( area.y + y ) * src1->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* srcPixel1 = ( _t_pixel_type* )( ( ( char* )sourceBuffer1 ) + ( ( ( area.y + y ) * src2->width() ) + x + area.x ) * pixelLength );

        for( size_t i = 0; channelCount > i; ++i ) {
            *( dstPixel + i ) = op( *( srcPixel0 + i ), *( srcPixel1 + i ) );
        }
    }
}

template < class _t_op, class _t_pixel_type >
void generic_operation_kernel_img_single(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   dst,
    libgraphics::backend::cpu::ImageObject*   src0,
    libgraphics::Rect32I area
) {
    static const _t_op op;

    ( void )device;

    assert( dst );
    assert( device );
    assert( src0 );

    void*   destinationBuffer = dst->data();
    void*   sourceBuffer0 = src0->data();

    size_t pixelLength = libgraphics::fxapi::EPixelFormat::getPixelSize(
                             dst->format()
                         );
    size_t channelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                              dst->format()
                          );

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );
        _t_pixel_type* dstPixel = ( _t_pixel_type* )( ( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * dst->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* srcPixel0 = ( _t_pixel_type* )( ( ( char* )sourceBuffer0 ) + ( ( ( area.y + y ) * src0->width() ) + x + area.x ) * pixelLength );

        for( size_t i = 0; channelCount > i; ++i ) {
            *( dstPixel + i ) = op( *( srcPixel0 + i ) );
        }
    }
}

template < class _t_op, class _t_pixel_type >
void generic_operation_kernel_value(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   dst,
    libgraphics::backend::cpu::ImageObject*   src,
    libgraphics::Rect32I area,
    float value
) {
    static const _t_op op;

    ( void )device;

    assert( dst );
    assert( device );
    assert( src );

    void*   destinationBuffer = dst->data();
    void*   sourceBuffer0 = src->data();

    size_t pixelLength = libgraphics::fxapi::EPixelFormat::getPixelSize(
                             dst->format()
                         );
    size_t channelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                              dst->format()
                          );

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );
        _t_pixel_type* dstPixel = ( _t_pixel_type* )( ( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * dst->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* srcPixel0 = ( _t_pixel_type* )( ( ( char* )sourceBuffer0 ) + ( ( ( area.y + y ) * src->width() ) + x + area.x ) * pixelLength );

        for( size_t i = 0; channelCount > i; ++i ) {
            *( dstPixel + i ) = op( *( srcPixel0 + i ), value );
        }
    }

}

template < class _t_op, class _t_pixel_type >
void generic_operation_kernel_color(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   dst,
    libgraphics::backend::cpu::ImageObject*   src,
    libgraphics::Rect32I area,
    const libgraphics::formats::RGBA32F::t& color
) {
    static const _t_op op;

    ( void )device;

    assert( dst );
    assert( device );
    assert( src );

    void*   destinationBuffer = dst->data();
    void*   sourceBuffer0 = src->data();

    size_t pixelLength = libgraphics::fxapi::EPixelFormat::getPixelSize(
                             dst->format()
                         );
    size_t channelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                              dst->format()
                          );

    for( size_t p = 0; ( area.width * area.height ) > p; ++p ) {
        const size_t y = ( p - ( p % ( int )area.width ) ) / area.width;
        const size_t x = ( p - ( y * ( int )area.width ) );
        _t_pixel_type* dstPixel = ( _t_pixel_type* )( ( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * dst->width() ) + x + area.x ) * pixelLength );
        _t_pixel_type* srcPixel0 = ( _t_pixel_type* )( ( ( char* )sourceBuffer0 ) + ( ( ( area.y + y ) * src->width() ) + x + area.x ) * pixelLength );

        const auto iterMax = std::min<size_t>( channelCount, ( size_t )color.Count );

        for( size_t i = 0; iterMax > i; ++i ) {
            *( dstPixel + i ) = op( *( srcPixel0 + i ), color.Values[i] );
        }
    }

}

/// operation: maxThreshold
void maxThreshold_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float maximalValue
) {
    assert( dst->format() == src0->format() );

    switch( dst->format() ) {

        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_max_threshold<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    maximalValue
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_max_threshold<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    maximalValue
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_max_threshold<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    maximalValue
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_max_threshold<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    maximalValue
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}


/// operation: minThreshold
void minThreshold_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float minimalValue
) {
    assert( dst->format() == src0->format() );

    switch( dst->format() ) {

        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_min_threshold<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    minimalValue
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_min_threshold<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    minimalValue
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_min_threshold<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    minimalValue
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_min_threshold<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    minimalValue
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

/// operation: negate
void negate_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area
) {

    assert( dst->format() == src0->format() );

    switch( dst->format() ) {

        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img_single<op_negate<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img_single<op_negate<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img_single<op_negate<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img_single<op_negate<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

/// operation: min
void min_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {

    assert( dst->format() == src0->format() );

    switch( dst->format() ) {

        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_min<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_min<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_min<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_min<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void min_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I area,
    const float& minimalChannelValue
) {

    assert( dst->format() == src0->format() );

    switch( dst->format() ) {

        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_min<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    minimalChannelValue
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_min<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    minimalChannelValue
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_min<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    minimalChannelValue
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_min<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    minimalChannelValue
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

/// operation: max
void max_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {

    assert( dst->format() == src0->format() );

    switch( dst->format() ) {

        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_max<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_max<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_max<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_max<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void max_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I area,
    const float& maxChannelValue
) {

    assert( dst->format() == src0->format() );

    switch( dst->format() ) {

        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_max<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    maxChannelValue
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_max<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    maxChannelValue
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_max<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    maxChannelValue
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_max<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    maxChannelValue
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

/// operation: add
void add_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {

    assert( dst->format() == src0->format() );

    switch( dst->format() ) {

        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_add<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_add<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;


        case fxapi::EPixelFormat::Mono8:

            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_add<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_add<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_add<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_add<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_add<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_add<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void add_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
) {
    assert( dst->format() == src0->format() );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_add<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_add<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::Mono8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_add<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_add<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_add<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_add<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_add<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_add<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void add_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    assert( dst->format() == src0->format() );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_add<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_add<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::Mono8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_add<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_add<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_add<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_add<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_add<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_add<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

/// operation: subtract
void subtract_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {

    assert( dst->format() == src0->format() );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_sub<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_sub<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::Mono8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_sub<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_sub<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_sub<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_sub<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_sub<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_sub<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void subtract_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
) {
    assert( dst->format() == src0->format() );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_sub<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_sub<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::Mono8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_sub<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_sub<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_sub<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_sub<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_sub<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_sub<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void subtract_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    assert( dst->format() == src0->format() );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_sub<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_sub<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::Mono8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_sub<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_sub<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_sub<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_sub<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_sub<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_sub<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}


/// operation: multiply
void multiply_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {

    assert( dst->format() == src0->format() );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_mul<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_mul<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::Mono8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_mul<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_mul<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_mul<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_mul<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_mul<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_mul<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void multiply_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
) {
    assert( dst->format() == src0->format() );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_mul<short>, short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_mul<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::Mono8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_mul<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_mul<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_mul<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_mul<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_mul<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_mul<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void multiply_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    assert( dst->format() == src0->format() );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_mul<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_mul<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::Mono8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_mul<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_mul<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_mul<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_mul<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_mul<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_mul<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

/// operation: divide
void divide_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {

    assert( dst->format() == src0->format() );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_div<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_div<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::Mono8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_div<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_div<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_div<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_div<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_div<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_div<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void divide_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
) {
    assert( dst->format() == src0->format() );

    switch( dst->format() ) {

        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_div<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_div<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;


        case fxapi::EPixelFormat::Mono8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_div<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_div<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_div<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_div<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_div<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_div<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void divide_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    assert( dst->format() == src0->format() );

    switch( dst->format() ) {

        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_div<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_div<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::Mono8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_div<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_div<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_div<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_div<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_div<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_div<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

template < class _t_pixel_type >
void kernel_normalize(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::backend::cpu::ImageObject*   dst,
    libgraphics::backend::cpu::ImageObject*   src,
    libgraphics::Rect32I area
) {
    ( void )device;

    assert( dst );
    assert( device );
    assert( src );

    void*   destinationBuffer = dst->data();
    void*   sourceBuffer0 = src->data();

    size_t pixelLength = libgraphics::fxapi::EPixelFormat::getPixelSize(
                             dst->format()
                         );
    size_t channelCount = libgraphics::fxapi::EPixelFormat::getChannelCount(
                              dst->format()
                          );

    for( size_t x = 0; area.width > x; ++x ) {
        for( size_t y = 0; area.height > y; ++y ) {
            _t_pixel_type* dstPixel = ( _t_pixel_type* )( ( char* )destinationBuffer ) + ( ( ( area.y + y ) * dst->width() ) + x + area.x ) * pixelLength;
            _t_pixel_type* srcPixel0 = ( _t_pixel_type* )( ( char* )sourceBuffer0 ) + ( ( ( area.y + y ) * src->width() ) + x + area.x ) * pixelLength;

            const auto max_value = std::numeric_limits<_t_pixel_type>::max();

            unsigned long long totalValue( 0 );

            for( size_t i = 0; channelCount > i; ++i ) {
                totalValue += ( *( srcPixel0 + i ) * *( srcPixel0 + i ) );
            }

            const _t_pixel_type normalizedValue(
                ( _t_pixel_type )std::min<unsigned long long>(
                    max_value,
                    sqrt( totalValue )
                )
            );

            for( size_t i = 0; channelCount > i; ++i ) {
                *( dstPixel + i ) = normalizedValue;
            }
        }
    }
}

void normalize_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src,
    const libgraphics::Rect32I& area
) {
    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src != nullptr );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src,
                area,
                std::bind(
                    &kernel_normalize<signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src,
                area,
                std::bind(
                    &kernel_normalize<float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src,
                area,
                std::bind(
                    &kernel_normalize<unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src,
                area,
                std::bind(
                    &kernel_normalize<unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}



/// operation: gimp layer modes

void grainMerge_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_merge<short>, short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_merge<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;


        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_merge<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_merge<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void alphaBlend_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area,
    float opacity
) {
    assert( false );
}

void alphaBlend_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    assert( false );
}

void screen_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_screen<short>, short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_screen<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;


        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_screen<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_screen<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void overlay_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    const libgraphics::formats::RGBA32F::t& color
) {
    assert( dst->format() == src0->format() );

    switch( dst->format() ) {

        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_overlay<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_overlay<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_overlay<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_color<op_overlay<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    color
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void overlay_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_overlay<short>, short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_overlay<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;


        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_overlay<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_overlay<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void dodge_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_dodge<short>, short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_dodge<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;


        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_dodge<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_dodge<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void burn_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {
    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_burn<short>, short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_burn<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;


        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_burn<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_burn<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void hardLight_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {

    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_hard_light<short>, short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_hard_light<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;


        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_hard_light<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_hard_light<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void grainExtract_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {

    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_extract<short>, short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_extract<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;


        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_extract<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_extract<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void grainMultiply_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {

    assert( dst->format() == src0->format() );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_multiply<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_multiply<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::Mono8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_multiply<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::Mono16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_multiply<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_multiply<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_multiply<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA8:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_multiply<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGBA16:


            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_grain_multiply<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void grainMultiply_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    const libgraphics::Rect32I& area,
    float value
) {

    assert( dst->format() == src0->format() );

    switch( dst->format() ) {

        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_grain_multiply<signed short>, signed short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_grain_multiply<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_grain_multiply<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_value<op_grain_multiply<unsigned short>, unsigned short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    std::placeholders::_4,
                    value
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

void difference_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {

    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_difference<short>, short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_difference<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;


        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_difference<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_difference<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}


/// operation: applyGrainSubtract
void applyGrainSubtract_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {

    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_apply_grain_subtract<short>, short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_apply_grain_subtract<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;


        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_apply_grain_subtract<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_apply_grain_subtract<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}

/// operation: applyGrainAdd
void applyGrainAdd_CPU(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject* dst,
    libgraphics::fxapi::ApiImageObject* src0,
    libgraphics::fxapi::ApiImageObject* src1,
    const libgraphics::Rect32I& area
) {

    assert( device != nullptr );
    assert( dst != nullptr );
    assert( src0 != nullptr );
    assert( src1 != nullptr );

    switch( dst->format() ) {
        case fxapi::EPixelFormat::RGB16S:
        case fxapi::EPixelFormat::RGBA16S:
        case fxapi::EPixelFormat::Mono16S:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_apply_grain_add<short>, short>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB32F:
        case fxapi::EPixelFormat::RGBA32F:
        case fxapi::EPixelFormat::Mono32F:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_apply_grain_add<float>, float>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;


        case fxapi::EPixelFormat::RGB8:
        case fxapi::EPixelFormat::RGBA8:
        case fxapi::EPixelFormat::Mono8:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_apply_grain_add<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        case fxapi::EPixelFormat::RGB16:
        case fxapi::EPixelFormat::RGBA16:
        case fxapi::EPixelFormat::Mono16:
            fx::operations::cpuExecuteTileBased(
                device,
                ( backend::cpu::ImageObject* )dst,
                ( backend::cpu::ImageObject* )src0,
                area,
                std::bind(
                    &generic_operation_kernel_img<op_apply_grain_add<unsigned char>, unsigned char>,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3,
                    ( libgraphics::backend::cpu::ImageObject* )src1,
                    std::placeholders::_4
                )
            );
            break;

        default:

            throw std::runtime_error(
                "Error: unknown or incompatible pixel format!"
            );
    }
}


}
}
}
