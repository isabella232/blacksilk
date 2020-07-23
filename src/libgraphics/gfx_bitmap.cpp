
#include <algorithm>
#include <vector>
#include <limits>
#include <functional>
#include <libgraphics/bitmap.hpp>
#include <log/log.hpp>

#include <fstream>
#include <iostream>

namespace libgraphics {

typedef std::pair<void*, const void*>  BufPair;
typedef std::function<bool( const BufPair&, const libgraphics::Rect32I&, const libgraphics::Rect32I&, size_t, size_t )>   TransformFn;
struct TransformFnId {
    libgraphics::Format from;
    libgraphics::Format to;

    TransformFnId(
        const libgraphics::Format& _from,
        const libgraphics::Format& _to
    ) : from( _from ), to( _to ) {}
};
typedef std::pair<TransformFnId, TransformFn>    SpecifiedTransformer;

template < class _t_format_type >
bool genericRGBAToARGB( const BufPair& buf, const libgraphics::Rect32I& dstArea, const libgraphics::Rect32I& srcArea, size_t dstWidth, size_t srcWidth ) {
    void* dst       = buf.first;
    const void* src = buf.second;

    if( !dst || !src || ( dstArea.area() == 0 ) || ( srcArea.area() == 0 ) ) {
        return false;
    }

    if( ( dstArea.width != srcArea.width ) || ( dstArea.height != srcArea.height ) ) {
        return false;
    }

    typedef libgraphics::BasicPixel< _t_format_type, 4 >    GenericPixelValue;


    const size_t cWidth     = dstArea.width;
    const size_t cHeight    = dstArea.height;

    GenericPixelValue*  srcPixelBuffer = ( GenericPixelValue* )src;
    GenericPixelValue*  dstPixelBuffer = ( GenericPixelValue* )dst;

    for( size_t sx = 0; cWidth > sx; ++sx ) {
        for( size_t sy = 0; cHeight > sy; ++sy ) {

            GenericPixelValue* currentDstPtr = ( dstPixelBuffer + ( ( sy + dstArea.y ) * dstWidth ) + ( sx + dstArea.x ) );
            GenericPixelValue* currentSrcPtr = ( srcPixelBuffer + ( ( sy + srcArea.y ) * srcWidth ) + ( sx + srcArea.x ) );

            currentDstPtr->Values[1] = currentSrcPtr->Values[0];
            currentDstPtr->Values[2] = currentSrcPtr->Values[1];
            currentDstPtr->Values[3] = currentSrcPtr->Values[2];

            currentDstPtr->Values[0] = currentSrcPtr->Values[3];
        }
    }

    return true;
}
template < class _t_format_type >
bool genericARGBToRGBA( const BufPair& buf, const libgraphics::Rect32I& dstArea, const libgraphics::Rect32I& srcArea, size_t dstWidth, size_t srcWidth ) {
    void* dst       = buf.first;
    const void* src = buf.second;

    if( !dst || !src || ( dstArea.area() == 0 ) || ( srcArea.area() == 0 ) ) {
        return false;
    }

    if( ( dstArea.width != srcArea.width ) || ( dstArea.height != srcArea.height ) ) {
        return false;
    }

    typedef libgraphics::BasicPixel< _t_format_type, 4 >    GenericPixelValue;

    const size_t cWidth     = dstArea.width;
    const size_t cHeight    = dstArea.height;

    GenericPixelValue*  srcPixelBuffer = ( GenericPixelValue* )src;
    GenericPixelValue*  dstPixelBuffer = ( GenericPixelValue* )dst;

    for( size_t sx = 0; cWidth > sx; ++sx ) {
        for( size_t sy = 0; cHeight > sy; ++sy ) {

            GenericPixelValue* currentDstPtr = ( dstPixelBuffer + ( ( sy + dstArea.y ) * dstWidth ) + ( sx + dstArea.x ) );
            GenericPixelValue* currentSrcPtr = ( srcPixelBuffer + ( ( sy + srcArea.y ) * srcWidth ) + ( sx + srcArea.x ) );

            currentDstPtr->Values[0] = currentSrcPtr->Values[1];
            currentDstPtr->Values[1] = currentSrcPtr->Values[2];
            currentDstPtr->Values[2] = currentSrcPtr->Values[3];

            currentDstPtr->Values[3] = currentSrcPtr->Values[0];
        }
    }

    return true;
}
template < class _t_format_type >
bool genericRGBToRGBA32( const BufPair& buf, const libgraphics::Rect32I& dstArea, const libgraphics::Rect32I& srcArea, size_t dstWidth, size_t srcWidth ) {
    void* dst       = buf.first;
    const void* src = buf.second;

    if( !dst || !src || ( dstArea.area() == 0 ) || ( srcArea.area() == 0 ) ) {
        return false;
    }

    if( ( dstArea.width != srcArea.width ) || ( dstArea.height != srcArea.height ) ) {
        return false;
    }

    typedef libgraphics::BasicPixel< unsigned int, 4 >               GenericPixelValue;
    typedef libgraphics::BasicPixel< _t_format_type, 3 >    SourcePixelValue;


    const size_t cWidth     = dstArea.width;
    const size_t cHeight    = dstArea.height;

    const auto srcMin       = std::numeric_limits<_t_format_type>::min();
    const auto srcMax       = std::numeric_limits<_t_format_type>::max();
    const auto dstMin       = std::numeric_limits<unsigned int>::min();
    const auto dstMax       = std::numeric_limits<unsigned int>::max();

    SourcePixelValue*  srcPixelBuffer   = ( SourcePixelValue* )src;
    GenericPixelValue*  dstPixelBuffer  = ( GenericPixelValue* )dst;

    for( size_t sx = 0; cWidth > sx; ++sx ) {
        for( size_t sy = 0; cHeight > sy; ++sy ) {

            GenericPixelValue* currentDstPtr    = ( dstPixelBuffer + ( ( sy + dstArea.y ) * dstWidth ) + ( sx + dstArea.x ) );
            SourcePixelValue* currentSrcPtr     = ( srcPixelBuffer + ( ( sy + srcArea.y ) * srcWidth ) + ( sx + srcArea.x ) );

            const auto val0 = mapRanges(
                                  currentSrcPtr->Values[0],
                                  srcMin, srcMax,
                                  dstMin, dstMax
                              );
            const auto val1 = mapRanges(
                                  currentSrcPtr->Values[1],
                                  srcMin, srcMax,
                                  dstMin, dstMax
                              );
            const auto val2 = mapRanges(
                                  currentSrcPtr->Values[2],
                                  srcMin, srcMax,
                                  dstMin, dstMax
                              );

            currentDstPtr->Values[0] = val0;
            currentDstPtr->Values[1] = val1;
            currentDstPtr->Values[2] = val2;

            currentDstPtr->Values[0] = 0;
        }
    }

    return true;
}
template < class _t_format_type >
bool genericMonoToRGBA32( const BufPair& buf, const libgraphics::Rect32I& dstArea, const libgraphics::Rect32I& srcArea, size_t dstWidth, size_t srcWidth ) {
    void* dst       = buf.first;
    const void* src = buf.second;

    if( !dst || !src || ( dstArea.area() == 0 ) || ( srcArea.area() == 0 ) ) {
        return false;
    }

    if( ( dstArea.width != srcArea.width ) || ( dstArea.height != srcArea.height ) ) {
        return false;
    }

    typedef libgraphics::BasicPixel< int, 4 >               GenericPixelValue;
    typedef libgraphics::BasicPixel< _t_format_type, 1 >    SourcePixelValue;


    const size_t cWidth     = dstArea.width;
    const size_t cHeight    = dstArea.height;

    const auto srcMin       = std::numeric_limits<_t_format_type>::min();
    const auto srcMax       = std::numeric_limits<_t_format_type>::max();
    const auto dstMin       = std::numeric_limits<int>::min();
    const auto dstMax       = std::numeric_limits<int>::max();

    SourcePixelValue*  srcPixelBuffer   = ( SourcePixelValue* )src;
    GenericPixelValue*  dstPixelBuffer  = ( GenericPixelValue* )dst;

    for( size_t sx = 0; cWidth > sx; ++sx ) {
        for( size_t sy = 0; cHeight > sy; ++sy ) {

            GenericPixelValue* currentDstPtr    = ( dstPixelBuffer + ( ( sy + dstArea.y ) * dstWidth ) + ( sx + dstArea.x ) );
            SourcePixelValue* currentSrcPtr     = ( srcPixelBuffer + ( ( sy + srcArea.y ) * srcWidth ) + ( sx + srcArea.x ) );

            const auto val0 = mapRanges(
                                  currentSrcPtr->Values[0],
                                  srcMin, srcMax,
                                  dstMin, dstMax
                              );
            currentDstPtr->Values[0] = val0;
            currentDstPtr->Values[1] = val0;
            currentDstPtr->Values[2] = val0;
            currentDstPtr->Values[3] = 0;
        }
    }

    return true;
}
template < class _t_format_type >
bool genericRGBAToRGBA32( const BufPair& buf, const libgraphics::Rect32I& dstArea, const libgraphics::Rect32I& srcArea, size_t dstWidth, size_t srcWidth ) {
    void* dst       = buf.first;
    const void* src = buf.second;

    if( !dst || !src || ( dstArea.area() == 0 ) || ( srcArea.area() == 0 ) ) {
        return false;
    }

    if( ( dstArea.width != srcArea.width ) || ( dstArea.height != srcArea.height ) ) {
        return false;
    }

    typedef libgraphics::BasicPixel< int, 4 >               GenericPixelValue;
    typedef libgraphics::BasicPixel< _t_format_type, 1 >    SourcePixelValue;


    const size_t cWidth     = dstArea.width;
    const size_t cHeight    = dstArea.height;

    const auto srcMin       = std::numeric_limits<_t_format_type>::min();
    const auto srcMax       = std::numeric_limits<_t_format_type>::max();
    const auto dstMin       = std::numeric_limits<int>::min();
    const auto dstMax       = std::numeric_limits<int>::max();

    SourcePixelValue*  srcPixelBuffer   = ( SourcePixelValue* )src;
    GenericPixelValue*  dstPixelBuffer  = ( GenericPixelValue* )dst;

    for( size_t sx = 0; cWidth > sx; ++sx ) {
        for( size_t sy = 0; cHeight > sy; ++sy ) {
            GenericPixelValue* currentDstPtr    = ( dstPixelBuffer + ( ( sy + dstArea.y ) * dstWidth ) + ( sx + dstArea.x ) );
            SourcePixelValue* currentSrcPtr     = ( srcPixelBuffer + ( ( sy + srcArea.y ) * srcWidth ) + ( sx + srcArea.x ) );


            const auto val0 = mapRanges(
                                  currentSrcPtr->Values[0],
                                  srcMin, srcMax,
                                  dstMin, dstMax
                              );
            const auto val1 = mapRanges(
                                  currentSrcPtr->Values[1],
                                  srcMin, srcMax,
                                  dstMin, dstMax
                              );
            const auto val2 = mapRanges(
                                  currentSrcPtr->Values[2],
                                  srcMin, srcMax,
                                  dstMin, dstMax
                              );
            const auto val3 = mapRanges(
                                  currentSrcPtr->Values[3],
                                  srcMin, srcMax,
                                  dstMin, dstMax
                              );

            currentDstPtr->Values[0] = val0;
            currentDstPtr->Values[1] = val1;
            currentDstPtr->Values[2] = val2;
            currentDstPtr->Values[3] = val3;
        }
    }

    return true;
}

static const SpecifiedTransformer   specifiedFormatTransformers[] = {
    /// RGBA to ARGB
    std::make_pair( TransformFnId( libgraphics::formats::RGBA8::toFormat(), libgraphics::formats::ARGB8::toFormat() ), &genericRGBAToARGB<unsigned char> ),
    std::make_pair( TransformFnId( libgraphics::formats::RGBA16::toFormat(), libgraphics::formats::ARGB16::toFormat() ), &genericRGBAToARGB<unsigned short> ),
    std::make_pair( TransformFnId( libgraphics::formats::RGBA16S::toFormat(), libgraphics::formats::ARGB16S::toFormat() ), &genericRGBAToARGB<short> ),
    std::make_pair( TransformFnId( libgraphics::formats::RGBA32::toFormat(), libgraphics::formats::ARGB32::toFormat() ), &genericRGBAToARGB<unsigned int> ),
    std::make_pair( TransformFnId( libgraphics::formats::RGBA32S::toFormat(), libgraphics::formats::ARGB32S::toFormat() ), &genericRGBAToARGB<int> ),

    /// ARGB to RGBA
    std::make_pair( TransformFnId( libgraphics::formats::ARGB8::toFormat(), libgraphics::formats::RGBA8::toFormat() ), &genericARGBToRGBA<unsigned char> ),
    std::make_pair( TransformFnId( libgraphics::formats::ARGB16::toFormat(), libgraphics::formats::RGBA16::toFormat() ), &genericARGBToRGBA<unsigned short> ),
    std::make_pair( TransformFnId( libgraphics::formats::ARGB16S::toFormat(), libgraphics::formats::RGBA16S::toFormat() ), &genericARGBToRGBA<short> ),
    std::make_pair( TransformFnId( libgraphics::formats::ARGB32::toFormat(), libgraphics::formats::RGBA32::toFormat() ), &genericARGBToRGBA<unsigned int> ),
    std::make_pair( TransformFnId( libgraphics::formats::ARGB32S::toFormat(), libgraphics::formats::RGBA32S::toFormat() ), &genericARGBToRGBA<int> ),

    /// x to rgba
    std::make_pair( TransformFnId( libgraphics::formats::RGB8::toFormat(), libgraphics::formats::RGBA32::toFormat() ), &genericRGBToRGBA32<unsigned char> ),
    std::make_pair( TransformFnId( libgraphics::formats::RGB16::toFormat(), libgraphics::formats::RGBA32::toFormat() ), &genericRGBToRGBA32<unsigned short> ),
    std::make_pair( TransformFnId( libgraphics::formats::RGB16S::toFormat(), libgraphics::formats::RGBA32::toFormat() ), &genericRGBToRGBA32<short> ),
    std::make_pair( TransformFnId( libgraphics::formats::RGB32::toFormat(), libgraphics::formats::RGBA32::toFormat() ), &genericRGBToRGBA32<unsigned int> ),
    std::make_pair( TransformFnId( libgraphics::formats::RGB32S::toFormat(), libgraphics::formats::RGBA32::toFormat() ), &genericRGBToRGBA32<int> ),

    std::make_pair( TransformFnId( libgraphics::formats::Mono8::toFormat(), libgraphics::formats::RGBA32::toFormat() ), &genericMonoToRGBA32<unsigned char> ),
    std::make_pair( TransformFnId( libgraphics::formats::Mono16::toFormat(), libgraphics::formats::RGBA32::toFormat() ), &genericMonoToRGBA32<unsigned short> ),
    std::make_pair( TransformFnId( libgraphics::formats::Mono16S::toFormat(), libgraphics::formats::RGBA32::toFormat() ), &genericMonoToRGBA32<short> ),

    std::make_pair( TransformFnId( libgraphics::formats::RGBA8::toFormat(), libgraphics::formats::RGBA32::toFormat() ), &genericRGBAToRGBA32<unsigned char> ),
    std::make_pair( TransformFnId( libgraphics::formats::RGBA16::toFormat(), libgraphics::formats::RGBA32::toFormat() ), &genericRGBAToRGBA32<unsigned short> ),
    std::make_pair( TransformFnId( libgraphics::formats::RGBA16S::toFormat(), libgraphics::formats::RGBA32::toFormat() ), &genericRGBAToRGBA32<short> ),
    std::make_pair( TransformFnId( libgraphics::formats::RGBA32S::toFormat(), libgraphics::formats::RGBA32::toFormat() ), &genericRGBAToRGBA32<int> )

};
static const size_t specifiedFormatTransformerCount = sizeof( specifiedFormatTransformers ) / sizeof( SpecifiedTransformer );
bool findSpecifiedTransformer( const libgraphics::Format& from, const libgraphics::Format& to, TransformFn& ret ) {
    for( size_t i = 0; specifiedFormatTransformerCount > i; ++i ) {
        if( ( specifiedFormatTransformers[i].first.from == from ) && ( specifiedFormatTransformers[i].first.to == to ) ) {
            ret = specifiedFormatTransformers[i].second;
            return true;
        }
    }

    return false;
}


/// conv
bool transformFormat(
    libgraphics::Bitmap* dst,
    libgraphics::Bitmap* src,
    const libgraphics::Format&  dstFormat
) {
    assert( dst != nullptr );
    assert( src != nullptr );

    if( !dst || !src || dst->empty() || src->empty() ) {
#if LIBGRAPHICS_DEBUG_OUTPUT
        LOG( "Failed to transform format: invalid src or destination." );
#endif
        return false;
    }

    assert( dst->size() == src->size() );

    if( dst->size() != src->size() ) {
#if LIBGRAPHICS_DEBUG_OUTPUT
        LOG( "Failed to transform format: incompatible bitmap planes." );
#endif
        return false;
    }

    return libgraphics::transformFormat(
               dst,
               src,
               dstFormat,
               dst->size(),
               src->size()
           );
}

bool transformFormat(
    libgraphics::Bitmap* dst,
    libgraphics::Bitmap* src,
    const libgraphics::Format&  dstFormat,
    const libgraphics::Rect32I& dstArea,
    const libgraphics::Rect32I& srcArea
) {
    assert( dst != nullptr );
    assert( src != nullptr );

    if( !dst || !src || dst->empty() || src->empty() ) {
#if LIBGRAPHICS_DEBUG_OUTPUT
        LOG( "Failed to transform format: invalid source or destination." );
#endif
        return false;
    }

    assert( dstArea.width == srcArea.width );
    assert( dstArea.height == srcArea.height );

    if( ( dstArea.width != srcArea.width ) || ( dstArea.height != dstArea.height ) ) {
#if LIBGRAPHICS_DEBUG_OUTPUT
        LOG( "Failed to transform format: incompatible source/destination planes." );
#endif
        return false;
    }

    assert( dst->size().contains( dstArea ) );
    assert( src->size().contains( srcArea ) );

    if( !dst->size().contains( dstArea ) || !src->size().contains( srcArea ) ) {
#if LIBGRAPHICS_DEBUG_OUTPUT
        LOG( "Failed to transform format: source/destination planes not part of original bitmap plane." );
#endif
        return false;
    }

    return transformFormat(
               dst->buffer(),
               src->buffer(),
               dstFormat,
               src->format(),
               dstArea,
               srcArea,
               dst->width(),
               src->width()
           );
}

bool transformFormat(
    void* dstBuf,
    const void* srcBuf,
    const libgraphics::Format& dstFormat,
    const libgraphics::Format& srcFormat,
    const libgraphics::Rect32I& dstArea,
    const libgraphics::Rect32I& srcArea,
    size_t dstWidth,
    size_t srcWidth
) {
    assert( dstBuf != nullptr );
    assert( srcBuf != nullptr );

    if( !dstBuf || !srcBuf ) {
#if LIBGRAPHICS_DEBUG_OUTPUT
        LOG( "Failed to transform format: invalid source/destination buffer." );
#endif
        return false;
    }

    assert( dstArea.area() != 0 );
    assert( srcArea.area() != 0 );
    assert( dstWidth != 0 );
    assert( srcWidth != 0 );

    if( ( dstArea.size() == 0 ) || ( srcArea.size() == 0 ) || ( dstWidth == 0 ) || ( srcWidth == 0 ) ) {
#if LIBGRAPHICS_DEBUG_OUTPUT
        LOG( "Failed to transform format: invalid buffer planes." );
#endif
        return false;
    }

    const size_t cWidth     = dstArea.width;
    const size_t cHeight    = dstArea.height;
    const size_t pixelSize  = dstFormat.byteSize;

    if( dstFormat == srcFormat ) {
        for( size_t sy = 0; cHeight > sy; ++sy ) {
            memcpy( ( void* )( ( char* )dstBuf + ( pixelSize * dstArea.x ) + ( pixelSize * dstWidth * ( dstArea.y + sy ) ) ),
                    ( void* )( ( char* )srcBuf + ( pixelSize * srcArea.x ) + ( pixelSize * srcWidth * ( srcArea.y + sy ) ) ),
                    cWidth * pixelSize );
        }

        return true;
    }

    TransformFn   formatTransformerFn;
    const bool successfullyFoundTransformerFn = findSpecifiedTransformer(
                srcFormat,
                dstFormat,
                formatTransformerFn
            );

    if( successfullyFoundTransformerFn ) {
        return formatTransformerFn(
                   BufPair( dstBuf, srcBuf ),
                   dstArea,
                   srcArea,
                   dstWidth,
                   srcWidth
               );
    }


    /// alternative strategy: look for ( source format -> RGBA32 )-transformer. Afterwards transform
    /// the source buffer to RGBA32 and look for a ( RGBA32 -> destination )-transformer.

    TransformFn   sourceFormatToRGBA32;
    const bool foundToRGBA32 = findSpecifiedTransformer(
                                   srcFormat,
                                   libgraphics::formats::RGBA32::toFormat(),
                                   sourceFormatToRGBA32
                               );

    TransformFn rgba32ToDestinationFormat;
    const bool foundFromRGBA32 = findSpecifiedTransformer(
                                     libgraphics::formats::RGBA32::toFormat(),
                                     dstFormat,
                                     rgba32ToDestinationFormat
                                 );

    assert( foundToRGBA32 && foundFromRGBA32 );

    if( !foundToRGBA32 ) {
#if LIBGRAPHICS_DEBUG_OUTPUT
        LOG( "Failed to transform format - Found neither specified format transformer nor generic to-rgba32 transformer." );
#endif
        return false;
    }

    if( !foundFromRGBA32 ) {
#if LIBGRAPHICS_DEBUG_OUTPUT
        LOG( "Failed to transform format - Found neither specified format transformer nor generic from-rgba32 transformer." );
#endif
        return false;
    }


    void*   temporaryBuffer = ( void* ) new char[
                               cWidth * cHeight * libgraphics::formats::RGBA32::ByteSize
                           ];

    if( !sourceFormatToRGBA32( BufPair( temporaryBuffer, srcBuf ), libgraphics::Rect32I( cWidth, cHeight ), srcArea, cWidth, srcWidth ) ) {
        delete []( char* )temporaryBuffer;
#if LIBGRAPHICS_DEBUG_OUTPUT
        LOG( "Failed to transform format - Generic to-rgba32 transformer failed." );
#endif

        return false;
    }

    if( !rgba32ToDestinationFormat( BufPair( dstBuf, temporaryBuffer ), dstArea, libgraphics::Rect32I( cWidth, cHeight ), dstWidth, cWidth ) ) {
        delete []( char* )temporaryBuffer;
#if LIBGRAPHICS_DEBUG_OUTPUT
        LOG( "Failed to transform format - Generic from-rgba32 transformer failed." );
#endif

        return false;
    }

    delete []( char* )temporaryBuffer;

    return true;
}

/// color-helpers
template < class _t_value >
libgraphics::BasicPixel<_t_value, 3> toRGB( const void* colorData, const libgraphics::Format& format ) {
    assert( colorData );

    typedef libgraphics::BasicPixel<_t_value, 3> ResultType;

    static const auto minValue = std::numeric_limits<_t_value>::min();
    static const auto maxValue = std::numeric_limits<_t_value>::max();

    static const auto byteSize = ResultType::Size;
    static const auto channelCount = 3;
    static const auto channelSize = byteSize / channelCount;

    assert( channelSize > 0 );
    ( void )channelSize;

    switch( format.family ) {
        /// convert from mono
        case formats::family::Mono:
            switch( format.byteSize ) {
                case formats::EPixelDepth<1>::b8: {
                    libgraphics::formats::Mono8::t value(
                        *( const libgraphics::formats::Mono8::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );

                    return result;
                }

                case formats::EPixelDepth<1>::b16: {

                    libgraphics::formats::Mono16::t value(
                        *( const libgraphics::formats::Mono16::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );

                    return result;
                }

                case formats::EPixelDepth<1>::b32: {
                    libgraphics::formats::Mono32::t value(
                        *( const libgraphics::formats::Mono32::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );

                    return result;
                }

                default:
                    assert( false ); /** failed to convert color. */
                    return ResultType();
            }

        /// convert from ARGB
        case formats::family::ARGB:
            switch( format.byteSize ) {
                case formats::EPixelDepth<4>::b8: {
                    libgraphics::formats::ARGB8::t value(
                        *( const libgraphics::formats::ARGB8::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[1], valMin, valMax, minValue, maxValue );
                    result.Values[1] = mapRanges( value.Values[2], valMin, valMax, minValue, maxValue );
                    result.Values[2] = mapRanges( value.Values[3], valMin, valMax, minValue, maxValue );

                    return result;
                }

                case formats::EPixelDepth<4>::b16: {

                    libgraphics::formats::ARGB16::t value(
                        *( const libgraphics::formats::ARGB16::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[1], valMin, valMax, minValue, maxValue );
                    result.Values[1] = mapRanges( value.Values[2], valMin, valMax, minValue, maxValue );
                    result.Values[2] = mapRanges( value.Values[3], valMin, valMax, minValue, maxValue );

                    return result;
                }

                case formats::EPixelDepth<4>::b32: {
                    libgraphics::formats::ARGB32::t value(
                        *( const libgraphics::formats::ARGB32::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[1], valMin, valMax, minValue, maxValue );
                    result.Values[1] = mapRanges( value.Values[2], valMin, valMax, minValue, maxValue );
                    result.Values[2] = mapRanges( value.Values[3], valMin, valMax, minValue, maxValue );

                    return result;
                }

                default:
                    assert( false ); /** failed to convert color. */
                    return ResultType();
            }

        /// convert from bgr
        case formats::family::BGR:
            switch( format.byteSize ) {
                case formats::EPixelDepth<3>::b8: {
                    libgraphics::formats::BGR8::t value(
                        *( const libgraphics::formats::BGR8::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[2], valMin, valMax, minValue, maxValue );
                    result.Values[1] = mapRanges( value.Values[1], valMin, valMax, minValue, maxValue );
                    result.Values[2] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );

                    return result;
                }

                case formats::EPixelDepth<3>::b16: {

                    libgraphics::formats::BGR16::t value(
                        *( const libgraphics::formats::BGR16::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[2], valMin, valMax, minValue, maxValue );
                    result.Values[1] = mapRanges( value.Values[1], valMin, valMax, minValue, maxValue );
                    result.Values[2] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );

                    return result;
                }

                case formats::EPixelDepth<3>::b32: {
                    libgraphics::formats::BGR32::t value(
                        *( const libgraphics::formats::BGR32::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[2], valMin, valMax, minValue, maxValue );
                    result.Values[1] = mapRanges( value.Values[1], valMin, valMax, minValue, maxValue );
                    result.Values[2] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );

                    return result;
                }

                default:
                    assert( false ); /** failed to convert color. */
                    return ResultType();
            }

        case formats::family::BGRA:
            assert( false ); // not implemented
            return ResultType();

        /// convert from rgba
        case formats::family::RGBA:
            switch( format.byteSize ) {
                case formats::EPixelDepth<4>::b8: {
                    libgraphics::formats::RGBA8::t value(
                        *( const libgraphics::formats::RGBA8::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );
                    result.Values[1] = mapRanges( value.Values[1], valMin, valMax, minValue, maxValue );
                    result.Values[2] = mapRanges( value.Values[2], valMin, valMax, minValue, maxValue );

                    return result;
                }

                case formats::EPixelDepth<4>::b16: {

                    libgraphics::formats::RGBA16::t value(
                        *( const libgraphics::formats::RGBA16::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );
                    result.Values[1] = mapRanges( value.Values[1], valMin, valMax, minValue, maxValue );
                    result.Values[2] = mapRanges( value.Values[2], valMin, valMax, minValue, maxValue );

                    return result;
                }

                case formats::EPixelDepth<4>::b32: {
                    libgraphics::formats::RGBA32::t value(
                        *( const libgraphics::formats::RGBA32::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );
                    result.Values[1] = mapRanges( value.Values[1], valMin, valMax, minValue, maxValue );
                    result.Values[2] = mapRanges( value.Values[2], valMin, valMax, minValue, maxValue );

                    return result;
                }

                default:
                    assert( false ); /** failed to convert color. */
                    return ResultType();
            }


        /// convert from rgb
        case formats::family::RGB: {
            if( byteSize == format.byteSize ) {
                return ResultType( *( const ResultType* )colorData );
            }

            switch( format.byteSize ) {
                case formats::EPixelDepth<3>::b8: {
                    libgraphics::formats::RGB8::t value(
                        *( const libgraphics::formats::RGB8::t* )colorData
                    );

                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );
                    result.Values[1] = mapRanges( value.Values[1], valMin, valMax, minValue, maxValue );
                    result.Values[2] = mapRanges( value.Values[2], valMin, valMax, minValue, maxValue );

                    return result;
                }

                case formats::EPixelDepth<3>::b16: {

                    libgraphics::formats::RGB16::t value(
                        *( const libgraphics::formats::RGB16::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );
                    result.Values[1] = mapRanges( value.Values[1], valMin, valMax, minValue, maxValue );
                    result.Values[2] = mapRanges( value.Values[2], valMin, valMax, minValue, maxValue );

                    return result;
                }

                case formats::EPixelDepth<3>::b32: {
                    libgraphics::formats::RGB32::t value(
                        *( const libgraphics::formats::RGB32::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    ResultType result;

                    result.Values[0] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );
                    result.Values[1] = mapRanges( value.Values[1], valMin, valMax, minValue, maxValue );
                    result.Values[2] = mapRanges( value.Values[2], valMin, valMax, minValue, maxValue );

                    return result;
                }

                default:
                    assert( false ); /** failed to convert color. */
                    return ResultType();
            }
        }

        case formats::family::Invalid:
            assert( false ); // bad
            return ResultType();
    }

    return ResultType();
}
template < class _t_value >
libgraphics::BasicPixel<_t_value, 4> toRGBA( const void* colorData, const libgraphics::Format& format ) {
    static const auto minValue = std::numeric_limits<_t_value>::min();
    static const auto maxValue = std::numeric_limits<_t_value>::max();

    const auto rgb = toRGB<_t_value>( colorData, format );
    libgraphics::BasicPixel<_t_value, 4> rgba;

    rgba.Values[0] = rgb.Values[0];
    rgba.Values[1] = rgb.Values[1];
    rgba.Values[2] = rgb.Values[2];

    switch( format.family ) {
        case formats::family::RGBA: {
            switch( format.byteSize ) {
                case formats::EPixelDepth<4>::b8: {
                    formats::RGBA8::t value(
                        *( const formats::RGBA8::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    static const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    static const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    rgba.Values[3] = mapRanges( value.Values[3], valMin, valMax, minValue, maxValue );
                    break;
                }

                case formats::EPixelDepth<4>::b16: {
                    formats::RGBA16::t value(
                        *( const formats::RGBA16::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    static const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    static const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    rgba.Values[3] = mapRanges( value.Values[3], valMin, valMax, minValue, maxValue );
                    break;
                }

                case formats::EPixelDepth<4>::b32: {
                    formats::RGBA32::t value(
                        *( const formats::RGBA32::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    static const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    static const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    rgba.Values[3] = mapRanges( value.Values[3], valMin, valMax, minValue, maxValue );
                    break;
                }
            }
        }

        case formats::family::ARGB: {
            switch( format.byteSize ) {
                case formats::EPixelDepth<4>::b8: {
                    formats::ARGB8::t value(
                        *( const formats::ARGB8::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    static const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    static const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    rgba.Values[3] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );
                    break;
                }

                case formats::EPixelDepth<4>::b16: {
                    formats::ARGB16::t value(
                        *( const formats::ARGB16::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    static const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    static const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    rgba.Values[3] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );
                    break;
                }

                case formats::EPixelDepth<4>::b32: {
                    formats::ARGB32::t value(
                        *( const formats::ARGB32::t* )colorData
                    );
                    typedef decltype( value ) IntermediatePixelType;
                    static const auto valMin = std::numeric_limits< IntermediatePixelType::PixelType >::min();
                    static const auto valMax = std::numeric_limits< IntermediatePixelType::PixelType >::max();

                    rgba.Values[3] = mapRanges( value.Values[0], valMin, valMax, minValue, maxValue );
                    break;
                }
            }
        }

        default:
            assert( false );
    }

    return rgba;
}

formats::RGB8::t toRGB8( const void* colorData, const libgraphics::Format& format ) {
    assert( colorData );

    if( formats::RGB8::toFormat() == format ) {
        return formats::RGB8::t(
                   *( ( const formats::RGB8::t* )colorData )
               );
    }

    return toRGB<formats::RGB8::t::PixelType>( colorData, format );
}

formats::RGB16::t toRGB16( const void* colorData, const libgraphics::Format& format ) {
    assert( colorData );

    if( formats::RGB16::toFormat() == format ) {
        return formats::RGB16::t(
                   *( ( const formats::RGB16::t* )colorData )
               );
    }

    return toRGB<formats::RGB16::t::PixelType>( colorData, format );
}

formats::RGB16S::t toRGB16S( const void* colorData, const libgraphics::Format& format ) {
    assert( colorData );

    if( formats::RGB16S::toFormat() == format ) {
        return formats::RGB16S::t(
                   *( ( const formats::RGB16S::t* )colorData )
               );
    }

    return toRGB<formats::RGB16S::t::PixelType>( colorData, format );
}

formats::RGB32::t toRGB32( const void* colorData, const libgraphics::Format& format ) {
    assert( colorData );

    if( formats::RGB32::toFormat() == format ) {
        return formats::RGB32::t(
                   *( ( const formats::RGB32::t* )colorData )
               );
    }

    return toRGB<formats::RGB32::t::PixelType>( colorData, format );
}

formats::RGB32S::t toRGB32S( const void* colorData, const libgraphics::Format& format ) {
    assert( colorData );

    if( formats::RGB32S::toFormat() == format ) {
        return formats::RGB32S::t(
                   *( ( const formats::RGB32S::t* )colorData )
               );
    }

    return toRGB<formats::RGB32S::t::PixelType>( colorData, format );
}

formats::RGB32F::t toRGB32F( const void* colorData, const libgraphics::Format& format ) {
    assert( colorData );

    if( formats::RGB32F::toFormat() == format ) {
        return formats::RGB32F::t(
                   *( ( const formats::RGB32F::t* )colorData )
               );
    }

    return toRGB<formats::RGB32F::t::PixelType>( colorData, format );
}

formats::RGBA8::t toRGBA8( const void* colorData, const libgraphics::Format& format ) {
    assert( colorData );

    if( formats::RGBA8::toFormat() == format ) {
        return formats::RGBA8::t(
                   *( ( const formats::RGBA8::t* )colorData )
               );
    }

    return toRGBA<formats::RGBA8::t::PixelType>( colorData, format );
}

formats::RGBA16::t toRGBA16( const void* colorData, const libgraphics::Format& format ) {
    assert( colorData );

    if( formats::RGBA16::toFormat() == format ) {
        return formats::RGBA16::t(
                   *( ( const formats::RGBA16::t* )colorData )
               );
    }

    return toRGBA<formats::RGBA16::t::PixelType>( colorData, format );
}

formats::RGBA16S::t toRGBA16S( const void* colorData, const libgraphics::Format& format ) {
    assert( colorData );

    if( formats::RGBA16S::toFormat() == format ) {
        return formats::RGBA16S::t(
                   *( ( const formats::RGBA16S::t* )colorData )
               );
    }

    return toRGBA<formats::RGBA16S::t::PixelType>( colorData, format );
}

formats::RGBA32::t toRGBA32( const void* colorData, const libgraphics::Format& format ) {
    assert( colorData );

    if( formats::RGBA32::toFormat() == format ) {
        return formats::RGBA32::t(
                   *( ( const formats::RGBA32::t* )colorData )
               );
    }

    return toRGBA<formats::RGBA32::t::PixelType>( colorData, format );
}

formats::RGBA32S::t toRGBA32S( const void* colorData, const libgraphics::Format& format ) {
    assert( colorData );

    if( formats::RGBA32S::toFormat() == format ) {
        return formats::RGBA32S::t(
                   *( ( const formats::RGBA32S::t* )colorData )
               );
    }

    return toRGBA<formats::RGBA32S::t::PixelType>( colorData, format );
}

formats::RGBA32F::t toRGBA32F( const void* colorData, const libgraphics::Format& format ) {
    assert( colorData );

    if( formats::RGBA32F::toFormat() == format ) {
        return formats::RGBA32F::t(
                   *( ( const formats::RGBA32F::t* )colorData )
               );
    }

    return toRGBA<formats::RGBA32F::t::PixelType>( colorData, format );
}

/** optimized memory management for
 *  bitmaps. **/
class BitmapMemoryContainer {
    public:
        explicit BitmapMemoryContainer( const libcommon::SizeType& length ) :
            m_Used( false ), m_Buffer( NULL ), m_Size( length ) {
            m_Buffer = ( void* )new char[ length ];
        }
        ~BitmapMemoryContainer() {
            reset();
        }
        void release() {
            m_Used = false;
        }

        void*   tryAcquire() {
            if( !used() ) {
                m_Used = true;

                return m_Buffer;
            }

            return NULL;
        }
        const libcommon::AddressType address() const {
            return ( libcommon::AddressType )m_Buffer;
        }
        const libcommon::SizeType   size() const {
            return m_Size;
        }
        const bool used() const {
            return m_Used;
        }
    protected:
        void reset() {
            delete []( char* )m_Buffer;

            m_Buffer    = NULL;
            m_Used      = false;
            m_Size      = 0;
        }

        bool                    m_Used;
        void*                   m_Buffer;
        libcommon::SizeType     m_Size;
};

class BitmapMemoryAllocator {
    public:
        typedef std::vector< libcommon::SharedPtr<BitmapMemoryContainer > >
        VectorType;

        bool free( void* buf ) {
            auto _g = libcommon::LockGuard( &m_Mutex );
            ( void )_g;

            for( auto it = m_Containers.begin(); it != m_Containers.end(); ++it ) {
                if( ( *it )->address() == ( libcommon::AddressType )buf ) {
                    ( *it )->release();

                    return true;
                }
            }

            return false;
        }

        void* find( const libcommon::SizeType& length, libcommon::SizeType allocationBase = 1024 ) {
            auto _g = libcommon::LockGuard( &m_Mutex );
            ( void )_g;

            for( auto it = m_Containers.begin(); it != m_Containers.end(); ++it ) {
                if( ( *it )->size() >= length && !( *it )->used() ) {
                    return ( *it )->tryAcquire();
                }
            }

            if( allocationBase != 0 ) {
                reserve( allocationBase - ( length % allocationBase ) );

                return find( length, 0 );
            }

            return NULL;
        }
        void freeUnused() {
            auto _g = libcommon::LockGuard( &m_Mutex );
            ( void )_g;

            VectorType ret;

            for( auto it = m_Containers.begin(); it != m_Containers.end(); ++it ) {
                if( !( *it )->used() ) {
                    ( *it ).reset();
                } else {
                    ret.push_back( ( *it ) );
                }
            }
        }
        void reserve( const libcommon::SizeType& length ) {
            auto _g = libcommon::LockGuard( &m_Mutex );
            ( void )_g;

            m_Containers.push_back(
                libcommon::SharedPtr<BitmapMemoryContainer>(
                    new BitmapMemoryContainer( length )
                )
            );

            std::sort(
                m_Containers.begin(),
                m_Containers.end(),
            []( const VectorType::value_type & first, const VectorType::value_type & second ) {
                return ( first->size() > second->size() );
            }
            );
        }
        libcommon::SizeType size() const {
            return ( libcommon::SizeType )m_Containers.size();
        }
    protected:
        libcommon::RecursiveMutex       m_Mutex;
        VectorType                      m_Containers;
};

static BitmapMemoryAllocator    _galloc;

//static BitmapMemoryAllocator&   galloc() {
//    return _galloc;
//}

/** static format instantiations **/

const libcommon::SizeType formats::Mono8::ByteSize      = formats::Mono8::t::Size;
const formats::family::t  formats::Mono8::Family        = formats::family::Mono;
const libcommon::SizeType formats::Mono8::Channels      = 1;

const libcommon::SizeType formats::Mono16::ByteSize     = formats::Mono16::t::Size;
const formats::family::t  formats::Mono16::Family       = formats::family::Mono;
const libcommon::SizeType formats::Mono16::Channels     = 1;

const libcommon::SizeType formats::Mono32::ByteSize     = formats::Mono32::t::Size;
const formats::family::t  formats::Mono32::Family       = formats::family::Mono;
const libcommon::SizeType formats::Mono32::Channels     = 1;

const libcommon::SizeType formats::RGB8::ByteSize       = formats::RGB8::t::Size;
const formats::family::t  formats::RGB8::Family         = formats::family::RGB;
const libcommon::SizeType formats::RGB8::Channels       = 3;

const libcommon::SizeType formats::RGB16::ByteSize      = formats::RGB16::t::Size;
const formats::family::t  formats::RGB16::Family        = formats::family::RGB;
const libcommon::SizeType formats::RGB16::Channels      = 3;

const libcommon::SizeType formats::RGB32::ByteSize      = formats::RGB32::t::Size;
const formats::family::t  formats::RGB32::Family        = formats::family::RGB;
const libcommon::SizeType formats::RGB32::Channels      = 3;

const libcommon::SizeType formats::RGB32F::ByteSize     = formats::RGB32F::t::Size;
const formats::family::t  formats::RGB32F::Family       = formats::family::RGB;
const libcommon::SizeType formats::RGB32F::Channels     = 3;

const libcommon::SizeType formats::RGBA8::ByteSize      = formats::RGBA8::t::Size;
const formats::family::t  formats::RGBA8::Family        = formats::family::RGBA;
const libcommon::SizeType formats::RGBA8::Channels      = 4;

const libcommon::SizeType formats::RGBA16::ByteSize     = formats::RGBA16::t::Size;
const formats::family::t  formats::RGBA16::Family       = formats::family::RGBA;
const libcommon::SizeType formats::RGBA16::Channels     = 4;

const libcommon::SizeType formats::RGBA32::ByteSize     = formats::RGBA32::t::Size;
const formats::family::t  formats::RGBA32::Family       = formats::family::RGBA;
const libcommon::SizeType formats::RGBA32::Channels     = 4;

const libcommon::SizeType formats::RGBA32F::ByteSize    = formats::RGBA32F::t::Size;
const formats::family::t  formats::RGBA32F::Family      = formats::family::RGBA;
const libcommon::SizeType formats::RGBA32F::Channels    = 4;

const libcommon::SizeType formats::ARGB8::ByteSize      = formats::ARGB8::t::Size;
const formats::family::t  formats::ARGB8::Family        = formats::family::ARGB;
const libcommon::SizeType formats::ARGB8::Channels      = 4;

const libcommon::SizeType formats::ARGB16::ByteSize     = formats::ARGB16::t::Size;
const formats::family::t  formats::ARGB16::Family       = formats::family::ARGB;
const libcommon::SizeType formats::ARGB16::Channels     = 4;

const libcommon::SizeType formats::ARGB16S::ByteSize     = formats::ARGB16S::t::Size;
const formats::family::t  formats::ARGB16S::Family       = formats::family::ARGB;
const libcommon::SizeType formats::ARGB16S::Channels     = 4;

const libcommon::SizeType formats::ARGB32::ByteSize     = formats::ARGB32::ByteSize;
const formats::family::t  formats::ARGB32::Family       = formats::family::ARGB;
const libcommon::SizeType formats::ARGB32::Channels     = 4;

const libcommon::SizeType formats::ARGB32S::ByteSize     = formats::ARGB32S::ByteSize;
const formats::family::t  formats::ARGB32S::Family       = formats::family::ARGB;
const libcommon::SizeType formats::ARGB32S::Channels     = 4;

const libcommon::SizeType formats::ARGB32F::ByteSize    = formats::ARGB32F::ByteSize;
const formats::family::t  formats::ARGB32F::Family      = formats::family::ARGB;
const libcommon::SizeType formats::ARGB32F::Channels    = 4;

const libcommon::SizeType formats::BGR8::ByteSize       = formats::BGR8::t::Size;
const formats::family::t  formats::BGR8::Family         = formats::family::BGR;
const libcommon::SizeType formats::BGR8::Channels       = 3;

const libcommon::SizeType formats::BGR16::ByteSize      = formats::BGR16::t::Size;
const formats::family::t  formats::BGR16::Family        = formats::family::BGR;
const libcommon::SizeType formats::BGR16::Channels      = 3;

const libcommon::SizeType formats::BGR32::ByteSize      = formats::BGR32::t::Size;
const formats::family::t  formats::BGR32::Family        = formats::family::BGR;
const libcommon::SizeType formats::BGR32::Channels      = 3;

const libcommon::SizeType formats::BGR32F::ByteSize     = formats::BGR32::t::Size;
const formats::family::t  formats::BGR32F::Family       = formats::family::BGR;
const libcommon::SizeType formats::BGR32F::Channels     = 3;

const libcommon::SizeType formats::RGB16S::ByteSize     = formats::RGB16S::t::Size;
const formats::family::t  formats::RGB16S::Family       = formats::family::RGB;
const libcommon::SizeType formats::RGB16S::Channels     = 3;

const libcommon::SizeType formats::RGB32S::ByteSize     = formats::RGB32S::t::Size;
const formats::family::t  formats::RGB32S::Family       = formats::family::RGB;
const libcommon::SizeType formats::RGB32S::Channels     = 3;


const libcommon::SizeType formats::RGBA16S::ByteSize     = formats::RGBA16S::t::Size;
const formats::family::t  formats::RGBA16S::Family       = formats::family::RGBA;
const libcommon::SizeType formats::RGBA16S::Channels     = 4;

const libcommon::SizeType formats::RGBA32S::ByteSize     = formats::RGBA32S::t::Size;
const formats::family::t  formats::RGBA32S::Family       = formats::family::RGBA;
const libcommon::SizeType formats::RGBA32S::Channels     = 4;


const libcommon::SizeType formats::Mono16S::ByteSize     = formats::Mono16S::t::Size;
const formats::family::t  formats::Mono16S::Family       = formats::family::Mono;
const libcommon::SizeType formats::Mono16S::Channels     = 1;

const libcommon::SizeType formats::Mono32S::ByteSize     = formats::Mono32S::t::Size;
const formats::family::t  formats::Mono32S::Family       = formats::family::Mono;
const libcommon::SizeType formats::Mono32S::Channels     = 1;

BitmapInfo::BitmapInfo( const libgraphics::Format& format, const libcommon::SizeType& width, const libcommon::SizeType& height ) : m_Format( format ), m_Width( width ), m_Height( height ) {}

BitmapInfo::BitmapInfo( const BitmapInfo& rhs ) : m_Format( rhs.m_Format ), m_Width( rhs.m_Width ), m_Height( rhs.m_Height ) {}

int BitmapInfo::width() const {
    return this->m_Width;
}

int BitmapInfo::height() const {
    return this->m_Height;
}

const Format& BitmapInfo::format() const {
    return this->m_Format;
}

const libcommon::SizeType& BitmapInfo::formatByteSize() const {
    return this->m_Format.byteSize;
}

libcommon::SizeType BitmapInfo::pixelCount() const {
    return m_Width * m_Height;
}

const libcommon::SizeType& BitmapInfo::channels() const {
    return m_Format.channels;
}

const libcommon::SizeType& BitmapInfo::pixelSize() const {
    return m_Format.byteSize;
}

void BitmapInfo::setFormat( const libgraphics::Format& format ) {
    m_Format = format;
}

void BitmapInfo::setWidth( const libcommon::SizeType& width ) {
    m_Width = width;
}

void BitmapInfo::setHeight( const libcommon::SizeType& height ) {
    m_Height = height;
}

void BitmapInfo::setSize( const libcommon::SizeType& width, const libcommon::SizeType& height ) {
    m_Width = width;
    m_Height = height;
}

void BitmapInfo::reset( const libgraphics::Format& format, const libcommon::SizeType& width, const libcommon::SizeType& height ) {
    m_Width = width;
    m_Height = height;
    m_Format = format;
}

Bitmap::Bitmap() : m_BitmapBuffer( NULL ), m_BitmapHeight( 0 ), m_BitmapWidth( 0 ), m_InternalAllocator( nullptr ) {}

Bitmap::Bitmap( const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height ) : m_BitmapBuffer( NULL ),
    m_BitmapHeight( 0 ), m_BitmapWidth( 0 ), m_InternalAllocator( nullptr ) {

    bool success = reset(
                       format,
                       width,
                       height
                   );

    ( void )success;
    assert( success );

    if( !success ) {
        reset();
    }
}
Bitmap::Bitmap( const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height, void* data ) : m_BitmapBuffer( NULL ),
    m_BitmapHeight( 0 ), m_BitmapWidth( 0 ), m_InternalAllocator( nullptr ) {

    bool success = reset(
                       format,
                       width,
                       height,
                       data
                   );

    ( void )success;
    assert( success );

    if( !success ) {
        reset();
    }
}
Bitmap::Bitmap( libgraphics::take_ownership_t, const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height, void* data ) : m_BitmapBuffer( NULL ),
    m_BitmapHeight( 0 ), m_BitmapWidth( 0 ), m_InternalAllocator( nullptr ) {

    this->m_BitmapBuffer        = data;
    this->m_BitmapWidth         = width;
    this->m_BitmapHeight        = height;
    this->m_Format              = format;
}

Bitmap::Bitmap( const BitmapInfo& info ) : m_BitmapBuffer( NULL ),
    m_BitmapHeight( 0 ), m_BitmapWidth( 0 ), m_InternalAllocator( nullptr ) {

    bool success = reset(
                       info
                   );

    ( void )success;
    assert( success );

    if( !success ) {
        reset();
    }
}

Bitmap::Bitmap( const Bitmap& other ) : m_BitmapBuffer( NULL ),
    m_BitmapHeight( 0 ), m_BitmapWidth( 0 ),  m_InternalAllocator( nullptr )  {

    bool success = reset(
                       other.format(),
                       other.width(),
                       other.height()
                   );

    assert( success );

    success = copy( &other, Rect32I( 0, 0, width(), height() ), 0, 0 );

    assert( success );

    if( !success ) {
        reset();
    }
}

Bitmap::Bitmap( libgraphics::StdDynamicPoolAllocator* allocator ) : m_BitmapBuffer( nullptr ),
    m_BitmapHeight( 0 ), m_BitmapWidth( 0 ), m_InternalAllocator( allocator ) {
    assert( allocator != nullptr );
}

Bitmap::Bitmap( libgraphics::StdDynamicPoolAllocator* allocator, const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height )  : m_BitmapBuffer( NULL ),
    m_BitmapHeight( 0 ), m_BitmapWidth( 0 ),  m_InternalAllocator( nullptr ) {
    const auto successfullyInitialized = reset(
            allocator,
            format,
            width,
            height
                                         );
    assert( successfullyInitialized );

#ifdef LIBGRAPHICS_DEBUG

    if( !successfullyInitialized ) {
        qDebug() << "Failed to initialize bitmap. width=" << width << ", height=" << height;
    }

#else
    ( void ) successfullyInitialized;
#endif // LIBGRAPHICS_DEBUG
}


Bitmap::Bitmap( libgraphics::StdDynamicPoolAllocator* allocator, const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height, void* data ) {
    const auto successfullyInitialized = reset(
            allocator,
            format,
            width,
            height,
            data
                                         );
    assert( successfullyInitialized );

#ifdef LIBGRAPHICS_DEBUG

    if( !successfullyInitialized ) {
        qDebug() << "Failed to initialize bitmap. width=" << width << ", height=" << height;
    }

#else
    ( void ) successfullyInitialized;
#endif // LIBGRAPHICS_DEBUG

}


template < class _t_any >
void any_swap( _t_any& first, _t_any& second ) {
    _t_any _temp( first );
    first = second;
    second = _temp;
}

void Bitmap::swap( Bitmap&& rhs ) {
    any_swap(
        this->m_BitmapBuffer, rhs.m_BitmapBuffer
    );
    any_swap(
        this->m_BitmapHeight, rhs.m_BitmapHeight
    );
    any_swap(
        this->m_BitmapWidth, rhs.m_BitmapWidth
    );
    any_swap(
        this->m_Format, rhs.m_Format
    );
    any_swap(
        this->m_InternalAllocator, rhs.m_InternalAllocator
    );
    m_InternalMemoryBlob.swap( rhs.m_InternalMemoryBlob );
}


Bitmap::~Bitmap() {
    reset();

    assert( empty() );
}

Bitmap& Bitmap::operator = ( const Bitmap& rhs ) {

    reset( rhs );

    return *this;

}

libcommon::SharedPtr<Bitmap>    Bitmap::clone() const {

    auto ptr = libcommon::SharedPtr<Bitmap>(
                   new Bitmap()
               );

    ptr->copy( this, Rect32I( 0, 0, width(), height() ), 0, 0 );

    return ptr;

}

libcommon::Boolean Bitmap::validPixel( const libcommon::UInt64 x, const libcommon::UInt64 y ) {

    if( empty() ) {
        return false;
    }

    return ( m_BitmapWidth >= x ) && ( m_BitmapHeight >= y );

}

bool Bitmap::operator == ( const BitmapInfo& info ) const {

    return (
               ( width() == info.width() ) &&
               ( height() == info.height() ) &&
               ( format() == info.format() )
           );

}

bool Bitmap::operator != ( const BitmapInfo& info ) const {

    return (
               ( width() != info.width() ) ||
               ( height() != info.height() ) ||
               ( format() != info.format() )
           );

}

bool Bitmap::operator == ( const Bitmap& info ) const {

    return (
               ( width() == info.width() ) &&
               ( height() == info.height() ) &&
               ( format() == info.format() ) &&
               ( buffer() == info.buffer() )
           );

}

bool Bitmap::operator != ( const Bitmap& info ) const {

    return (
               ( width() != info.width() ) ||
               ( height() != info.height() ) ||
               ( format() != info.format() ) ||
               ( buffer() != info.buffer() )
           );

}

bool Bitmap::copyChannel( size_t sourceChannelIndex, size_t destinationChannelIndex, const void* source, const int x, const int y, const int dwidth, const int dheight ) {

    assert( source );
    assert( this->format().channels > destinationChannelIndex );

    if( x + dwidth > width() ) {
        return false;
    }

    if( y + dheight > height() ) {
        return false;
    }

    if( this->format().channels <= destinationChannelIndex ) {
        return false;
    }

    void* destinationBuffer = buffer();
    const void* sourceBuffer = source;

    assert( destinationBuffer != nullptr );
    assert( sourceBuffer != nullptr );

    const auto channelSize = ( this->format().byteSize / this->format().channels );

    for( int i = 0; dheight > i; ++i ) {
        for( int j = 0; dwidth > j; ++j ) {
            memcpy( ( void* )( ( char* )destinationBuffer + ( ( ( i *  width() ) + ( y * width() ) + x + j ) * format().byteSize + ( destinationChannelIndex * channelSize ) ) ),
                    ( const void* )( ( const char* )sourceBuffer + ( ( ( i *  dwidth ) + ( y * dwidth ) + x + j ) * format().byteSize + ( sourceChannelIndex * channelSize ) ) ),
                    channelSize );
        }
    }


    return true;

}

bool Bitmap::copyChannel( size_t sourceChannelIndex, size_t destinationChannelIndex, const Bitmap* source, Rect32I sourceRect, const int destinationX, const int destinationY ) {

    assert( source );
    assert( this->format().channels > destinationChannelIndex );
    assert( source->format().channels > sourceChannelIndex );

    if( destinationX + sourceRect.width > width() ) {
        return false;
    }

    if( destinationY + sourceRect.height > height() ) {
        return false;
    }

    if( sourceRect.x > source->width() ) {
        return false;
    }

    if( sourceRect.y > source->height() ) {
        return false;
    }

    if( sourceRect.x + sourceRect.width > source->width() ) {
        return false;
    }

    if( sourceRect.y + sourceRect.height > source->height() ) {
        return false;
    }

    if( this->format().channels <= destinationChannelIndex ) {
        return false;
    }

    if( source->format().channels <= sourceChannelIndex ) {
        return false;
    }

    void*   destinationBuffer = buffer();
    const void* sourceBuffer  = source->buffer();

    const auto channelSize = ( this->format().byteSize / this->format().channels );

    for( int i = 0; sourceRect.height > i; ++i ) {
        for( int j = 0; sourceRect.width > j; ++j ) {
            memcpy( ( void* )( ( char* )destinationBuffer + ( ( i *  width() ) + ( destinationY * width() ) + destinationX + j ) * format().byteSize + ( destinationChannelIndex * channelSize ) ),
                    ( const void* )( ( const char* )sourceBuffer + ( ( i *  source->width() ) + ( sourceRect.y * source->width() ) + sourceRect.x + j ) * source->format().byteSize + ( sourceChannelIndex * channelSize ) ),
                    channelSize );
        }
    }

    return true;

}

bool Bitmap::copyChannel( size_t sourceChannelIndex, size_t destinationChannelIndex, const void* source, Rect32I sourceRect, const int destinationX, const int destinationY ) {

    assert( source );
    assert( this->format().channels > destinationChannelIndex );

    if( destinationX + sourceRect.width > width() ) {
        return false;
    }

    if( destinationY + sourceRect.height > height() ) {
        return false;
    }

    if( this->format().channels <= destinationChannelIndex ) {
        return false;
    }

    void*   destinationBuffer = buffer();
    const void* sourceBuffer  = source;

    const auto channelSize = ( this->format().byteSize / this->format().channels );

    for( int i = 0; sourceRect.height > i; ++i ) {
        for( int j = 0; sourceRect.width > j; ++j ) {
            memcpy( ( void* )( ( char* )destinationBuffer + ( ( i *  width() ) + ( destinationY * width() ) + destinationX + j ) * format().byteSize + ( destinationChannelIndex * channelSize ) ),
                    ( const void* )( ( const char* )sourceBuffer + ( ( i *  sourceRect.width ) + ( sourceRect.y * sourceRect.width + j ) + sourceRect.x ) * format().byteSize + ( sourceChannelIndex * channelSize ) ),
                    channelSize );
        }
    }

    return true;

}

bool Bitmap::copyChannel( size_t sourceChannelIndex, size_t destinationChannelIndex, const void* source, Rect32I sourceRect, size_t sourcePlaneWidth, size_t /*sourcePlaneHeight*/, const int destinationX, const int destinationY ) {

    assert( source );
    assert( this->format().channels > destinationChannelIndex );

    if( destinationX + sourceRect.width > width() ) {
        return false;
    }

    if( destinationY + sourceRect.height > height() ) {
        return false;
    }

    if( this->format().channels <= destinationChannelIndex ) {
        return false;
    }

    void*   destinationBuffer = buffer();
    const void* sourceBuffer  = source;

    const auto channelSize = ( this->format().byteSize / this->format().channels );

    for( int i = 0; sourceRect.height > i; ++i ) {
        for( int j = 0; sourceRect.width > j; ++j ) {
            memcpy( ( void* )( ( char* )destinationBuffer + ( ( i *  width() ) + ( destinationY * width() ) + destinationX ) * format().byteSize + ( destinationChannelIndex * channelSize ) ),
                    ( const void* )( ( const char* )sourceBuffer + ( ( i *  sourcePlaneWidth ) + ( sourceRect.y * sourcePlaneWidth ) + sourceRect.x ) * format().byteSize + ( sourceChannelIndex * channelSize ) ),
                    channelSize );
        }
    }

    return true;

}

bool Bitmap::copy( const void* source, const int x, const int y, const int dwidth, const int dheight ) {

    assert( source );

    if( x + dwidth > width() ) {
        return false;
    }

    if( y + dheight > height() ) {
        return false;
    }

    void* destinationBuffer = buffer();
    const void* sourceBuffer = source;

    for( int i = 0; dheight > i; ++i ) {
        memcpy( ( void* )( ( char* )destinationBuffer + ( ( ( i *  width() ) + ( y * width() ) + x ) * format().byteSize ) ),
                ( const void* )( ( const char* )sourceBuffer + ( ( ( i *  dwidth ) + ( y * dwidth ) + x ) * format().byteSize ) ),
                this->format().byteSize * dwidth );
    }


    return true;

}

bool Bitmap::copy( const Bitmap* source, Rect32I sourceRect, const int destinationX, const int destinationY ) {

    assert( source );

    if( destinationX + sourceRect.width > width() ) {
        return false;
    }

    if( destinationY + sourceRect.height > height() ) {
        return false;
    }

    if( sourceRect.x > source->width() ) {
        return false;
    }

    if( sourceRect.y > source->height() ) {
        return false;
    }

    if( sourceRect.x + sourceRect.width > source->width() ) {
        return false;
    }

    if( sourceRect.y + sourceRect.height > source->height() ) {
        return false;
    }

    if( ( source->format().byteSize / source->format().channels ) != ( this->format().byteSize / this->format().channels ) ) {
        return false;
    }

    void*   destinationBuffer   = buffer();
    const void* sourceBuffer    = source->buffer();
    const auto identityCopy     = ( source->format() == this->format() );

    if( identityCopy ) {
        for( int i = 0; sourceRect.height > i; ++i ) {
            memcpy( ( void* )( ( char* )destinationBuffer + ( ( i *  width() ) + ( destinationY * width() ) + destinationX ) * format().byteSize ),
                    ( const void* )( ( const char* )sourceBuffer + ( ( i *  source->width() ) + ( sourceRect.y * source->width() ) + sourceRect.x ) * format().byteSize ),
                    this->format().byteSize * sourceRect.width );
        }
    } else {
        const auto copySize = std::min( this->format().byteSize, source->format().byteSize );

        for( int i = 0; sourceRect.height > i; ++i ) {
            for( int j = 0; sourceRect.width > j; ++j ) {
                memcpy( ( void* )( ( char* )destinationBuffer + ( ( i *  width() ) + ( destinationY * width() ) + destinationX + j ) * format().byteSize ),
                        ( const void* )( ( const char* )sourceBuffer + ( ( i *  source->width() ) + ( sourceRect.y * source->width() ) + sourceRect.x + j ) * source->format().byteSize ),
                        copySize );
            }
        }
    }

    return true;

}

bool Bitmap::copy( const void* source, Rect32I sourceRect, const int destinationX, const int destinationY ) {

    assert( source );

    if( destinationX + sourceRect.width > width() ) {
        return false;
    }

    if( destinationY + sourceRect.height > height() ) {
        return false;
    }

    void*   destinationBuffer = buffer();
    const void* sourceBuffer  = source;

    for( int i = 0; sourceRect.height > i; ++i ) {
        memcpy( ( void* )( ( char* )destinationBuffer + ( ( i *  width() ) + ( destinationY * width() ) + destinationX ) * format().byteSize ),
                ( const void* )( ( const char* )sourceBuffer + ( ( i *  sourceRect.width ) + ( sourceRect.y * sourceRect.width ) + sourceRect.x ) * format().byteSize ),
                this->format().byteSize * sourceRect.width );
    }

    return true;

}

bool Bitmap::copy( const void* source, Rect32I sourceRect, size_t sourcePlaneWidth, size_t sourcePlaneHeight, const int destinationX, const int destinationY ) {

    assert( source );
    ( void )sourcePlaneHeight;

    if( destinationX + sourceRect.width > width() ) {
        return false;
    }

    if( destinationY + sourceRect.height > height() ) {
        return false;
    }

    void*   destinationBuffer = buffer();
    const void* sourceBuffer  = source;

    for( int i = 0; sourceRect.height > i; ++i ) {
        memcpy( ( void* )( ( char* )destinationBuffer + ( ( i *  width() ) + ( destinationY * width() ) + destinationX ) * format().byteSize ),
                ( const void* )( ( const char* )sourceBuffer + ( ( i *  sourcePlaneWidth ) + ( sourceRect.y * sourcePlaneWidth ) + sourceRect.x ) * format().byteSize ),
                this->format().byteSize * sourceRect.width );
    }

    return true;

}

bool Bitmap::write( Bitmap* destination, Rect32I sourceRect, const libcommon::SizeType& destinationX, const libcommon::SizeType& destinationY ) {
    assert( destination );

    return destination->copy(
               this,
               sourceRect,
               destinationX,
               destinationY
           );
}

bool Bitmap::write( void* destination, const int x, const int y, const int dwidth, const int dheight ) {

    assert( destination );

    if( x + dwidth > width() ) {
        return false;
    }

    if( y + dheight > height() ) {
        return false;
    }

    void* destinationBuffer = destination;
    const void* sourceBuffer = buffer();

    for( int i = 0; dheight > i; ++i ) {
        memcpy( ( void* )( ( char* )destinationBuffer + ( ( ( i *  dwidth ) + ( y * dwidth ) + x ) * format().byteSize ) ),
                ( const void* )( ( char* )sourceBuffer + ( ( ( i *  width() ) + ( y * width() ) + x ) * format().byteSize ) ),
                this->format().byteSize * dwidth );
    }


    return true;

}

bool Bitmap::write( void* destination, const int dstWidth, const int dstHeight, const int x, const int y, const int dwidth, const int dheight ) {
    assert( destination );
    assert( dstWidth > 0 );
    assert( dstHeight > 0 );

    ( void )dstHeight;

    if( x + dwidth > width() ) {
        return false;
    }

    if( y + dheight > height() ) {
        return false;
    }

    void* destinationBuffer = destination;
    const void* sourceBuffer = buffer();

    for( int i = 0; dheight > i; ++i ) {
        memcpy( ( void* )( ( char* )destinationBuffer + ( ( i *  dstWidth ) + ( y * dstWidth ) + x ) * format().byteSize ),
                ( const void* )( ( char* )sourceBuffer + ( ( i *  width() ) + ( y * width() ) + x ) * format().byteSize ),
                this->format().byteSize * dwidth );
    }


    return true;

}

bool Bitmap::write( void* destination, const Rect64UI& dstRect, const Rect64UI& srcRect ) {
    assert( destination );
    assert( dstRect.width == srcRect.width );
    assert( dstRect.height == srcRect.height );
    assert( width() >= srcRect.x + srcRect.width );
    assert( height() >= srcRect.y + srcRect.height );

    void* destinationBuffer = destination;
    const void* sourceBuffer = buffer();

    for( libcommon::UInt64 i = 0; dstRect.height > i; ++i ) {
        memcpy( ( void* )( ( char* )destinationBuffer + ( ( i *  dstRect.width ) + ( dstRect.y * dstRect.width ) + dstRect.x ) * format().byteSize ),
                ( const void* )( ( char* )sourceBuffer + ( ( i *  width() ) + ( srcRect.y * width() ) + srcRect.x ) * format().byteSize ),
                this->format().byteSize * dstRect.width );
    }


    return true;
}

bool Bitmap::write( void* destination, const Rect64UI& dstRect, size_t dstPlaneWidth, size_t dstPlaneHeight, const Rect64UI& srcRect ) {
    assert( destination );
    assert( dstRect.width == srcRect.width );
    assert( dstRect.height == srcRect.height );
    assert( width() >= srcRect.x + srcRect.width );
    assert( height() >= srcRect.y + srcRect.height );

    void* destinationBuffer = destination;
    const void* sourceBuffer = buffer();

    ( void )dstPlaneHeight;

    for( libcommon::UInt64 i = 0; dstRect.height > i; ++i ) {
        memcpy( ( void* )( ( char* )destinationBuffer + ( ( ( i *  dstPlaneWidth ) + ( dstRect.y * dstPlaneWidth ) + dstRect.x ) * format().byteSize ) ),
                ( const void* )( ( char* )sourceBuffer + ( ( ( i *  width() ) + ( srcRect.y * width() ) + srcRect.x ) * format().byteSize ) ),
                this->format().byteSize * dstRect.width );
    }


    return true;
}

libcommon::Maybe< libcommon::SharedPtr<Bitmap> > Bitmap::create( const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height ) {

    if( format.family != formats::family::Invalid && format.byteSize != 0 && format.channels != 0 && width != 0 && height != 0 ) {
        return libcommon::just( libcommon::SharedPtr<Bitmap>( new Bitmap( format, width, height ) ) );
    }

    return libcommon::nothing();

}

libcommon::Maybe< libcommon::SharedPtr<Bitmap> > Bitmap::create( const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height, void* data ) {

    if( format.family != formats::family::Invalid && format.byteSize != 0 && format.channels != 0 && width != 0 && height != 0 && data != 0 ) {
        return libcommon::just( libcommon::SharedPtr<Bitmap>( new Bitmap( format, width, height, data ) ) );
    }

    return libcommon::nothing();

}


libcommon::Maybe< libcommon::SharedPtr<Bitmap> > Bitmap::create( const BitmapInfo& info ) {

    return create( info.format(), info.width(), info.height() );

}

bool Bitmap::reset( const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height ) {
    assert( width * height != 0 );

    const libcommon::SizeType pixelSize     = format.byteSize;
    const libcommon::SizeType bitmapSize    = width * height * pixelSize;

    if( ( this->m_BitmapWidth == width ) && ( this->m_BitmapHeight == height ) && ( this->m_Format == format ) ) {
        ( void ) ::memset(
            this->m_BitmapBuffer,
            0,
            bitmapSize
        );

        return true;
    }

    if( this->m_InternalAllocator != nullptr ) {
        return reset( m_InternalAllocator, format, width, height );
    }

    if( !reset() ) {
        return false;
    }

    m_BitmapBuffer      = ( void* ) new char[ bitmapSize ];
    ( void ) ::memset(
        this->m_BitmapBuffer,
        0,
        bitmapSize
    );

    m_BitmapWidth   = width;
    m_BitmapHeight  = height;
    m_Format        = format;

    assert( m_BitmapBuffer );

    return true;

}

bool Bitmap::reset( const libgraphics::Format& format, const libcommon::UInt64& width, const libcommon::UInt64& height, void* data ) {

    if( ( this->m_BitmapWidth == width ) && ( this->m_BitmapHeight == height ) && ( this->m_Format == format ) ) {
        return this->copy(
                   data,
                   libgraphics::Rect32I( width, height ),
                   0,
                   0
               );
    }

    if( this->m_InternalAllocator != nullptr ) {
        return reset( m_InternalAllocator, format, width, height, data );
    }

    if( !reset() ) {
        return false;
    }

    const libcommon::SizeType pixelSize     = format.byteSize;
    const libcommon::SizeType bitmapSize    = width * height * pixelSize;

    m_BitmapBuffer      = ( void* ) new char[ bitmapSize ];

    m_BitmapWidth   = width;
    m_BitmapHeight  = height;
    m_Format        = format;

    ::memcpy( m_BitmapBuffer, ( const void* )data, m_BitmapWidth * m_BitmapHeight * m_Format.byteSize );

    assert( m_BitmapBuffer );

    return true;

}

bool Bitmap::reset( const BitmapInfo& info ) {
    return reset( info.format(), info.width(), info.height() );
}

bool Bitmap::reset( const Bitmap& other ) {

    if( ( other.m_BitmapHeight == other.m_BitmapHeight ) && ( other.m_BitmapWidth == other.m_BitmapWidth ) && ( other.format() == format() ) ) {
        return this->copy(
                   &other,
                   Rect32I( 0, 0, width(), height() ),
                   0,
                   0
               );
    }

    if( !reset() ) {
        return false;
    }

    if( !reset( other.m_Format, other.m_BitmapWidth, other.m_BitmapHeight ) ) {
        return false;
    }

    return this->copy(
               &other,
               Rect32I( 0, 0, width(), height() ),
               0,
               0
           );

}

bool Bitmap::reset() {

    if( empty() ) {
        return true;
    }

    if( m_InternalAllocator == nullptr ) {
        delete []( char* )m_BitmapBuffer;
    } else {
        m_InternalMemoryBlob.reset();
    }

    m_BitmapBuffer      = NULL;
    m_BitmapWidth       = 0;
    m_BitmapHeight      = 0;

    m_Format.byteSize   = 0;
    m_Format.family     = formats::family::Invalid;
    m_Format.channels   = 0;

    return true;

}


bool Bitmap::discardAlphaChannel() {
    libgraphics::Format format( this->format() );
    --format.channels;
    format.byteSize -= ( this->format().byteSize / this->format().channels );

    if( this->format().family == libgraphics::formats::family::ARGB ) {
        format.family    = libgraphics::formats::family::RGB;

        return this->discardChannel( 0, format );
    } else if( ( this->format().family == libgraphics::formats::family::BGRA ) ) {
        format.family    = libgraphics::formats::family::BGR;

        return this->discardChannel( 3, format );
    } else if( ( this->format().family == libgraphics::formats::family::RGBA ) ) {
        format.family    = libgraphics::formats::family::RGB;

        return this->discardChannel( 3, format );
    }

    return false;
}

bool Bitmap::discardChannel( size_t channelIndex, const libgraphics::Format& format ) {
    if( this->empty() ) {
        assert( false );
        return false;
    }

    if( channelIndex >= this->format().channels ) {
        assert( false );
        return false;
    }

    if( ( this->format().channels - 1 ) != format.channels ) {
        assert( false );
        return false;
    }

    if( ( this->format().byteSize - ( this->format().byteSize / this->format().channels ) ) != format.byteSize ) {
        assert( false );
        return false;
    }

    libgraphics::Bitmap newBitmap(
        this->allocator(),
        format,
        this->width(),
        this->height()
    );
    size_t destinationIndex( 0 );

    for( size_t i = 0; this->format().channels > i; ++i ) {
        if( i == channelIndex ) {
            continue;
        }

        const auto _successfullyCopied = newBitmap.copyChannel(
                                             i,
                                             destinationIndex,
                                             this,
                                             this->size(),
                                             0,
                                             0
                                         );

        if( !_successfullyCopied ) {
            assert( false );
            break;
        }

        ++destinationIndex;
    }

    this->swap(
        std::move( newBitmap )
    );

    return ( this->format() == format );
}

bool Bitmap::empty() const {

    return ( m_BitmapBuffer == NULL );

}

void* Bitmap::buffer() {

    return this->m_BitmapBuffer;

}

const void* Bitmap::buffer() const {

    return this->m_BitmapBuffer;

}

const libcommon::SizeType&  Bitmap::formatByteSize() const {

    return this->m_Format.byteSize;

}

libcommon::SizeType  Bitmap::pixelCount() const {

    return this->m_BitmapHeight * this->m_BitmapWidth;

}

const libcommon::SizeType&  Bitmap::channels() const {

    return this->m_Format.channels;

}

int  Bitmap::width() const {
    return this->m_BitmapWidth;
}

int  Bitmap::height() const {
    return this->m_BitmapHeight;
}

libgraphics::Rect32I    Bitmap::size() const {
    return libgraphics::Rect32I(
               this->width(), this->height()
           );
}

const libgraphics::Format&  Bitmap::format() const {

    return this->m_Format;

}

BitmapInfo Bitmap::info() const {
    return BitmapInfo(
               format(),
               width(),
               height()
           );
}

libcommon::LockGuard    Bitmap::manualLock() {

    return libcommon::LockGuard( &this->m_AccessLock );

}

void    Bitmap::synchronize() {
    volatile auto guard = manualLock();

    ( void )guard;
}


bool Bitmap::reset( libgraphics::StdDynamicPoolAllocator* allocator, const libgraphics::Format& format, const int width, const int height, void* data ) {
    assert( allocator != nullptr );
    assert( width * height > 0 );
    assert( data != nullptr );

    if( allocator == nullptr || data == nullptr ) {
        return false;
    }

    if( width * height <= 0 ) {
        return false;
    }

    if( ( this->format() == format ) && ( this->width() == width ) && ( this->height() == height ) && ( allocator == this->allocator() ) ) {
        ( void ) ::memcpy(
            this->m_BitmapBuffer,
            data,
            this->formatByteSize() * this->width() * this->height()
        );
        return true;
    }


    const auto successfullyResetted = this->reset();
    assert( successfullyResetted );

    if( !successfullyResetted ) {
#ifdef LIBGRAPHICS_DEBUG
        qDebug() << "Failed to reset bitmap. Aborting...";
#endif
        return false;
    }

    this->m_BitmapBuffer            = nullptr;
    this->m_BitmapHeight            = height;
    this->m_BitmapWidth             = width;
    this->m_InternalAllocator       = allocator;
    this->m_Format                  = format;


    const auto blob_size            = this->pixelCount() * this->formatByteSize();
    this->m_InternalMemoryBlob      = this->m_InternalAllocator->alloc(
                                          blob_size
                                      );
    assert( this->m_InternalMemoryBlob );
    assert( !this->m_InternalMemoryBlob->empty() );

    if( !this->m_InternalMemoryBlob || this->m_InternalMemoryBlob->empty() ) {
#ifdef LIBGRAPHICS_DEBUG
        qDebug() << "Failed to allocate memory blob of size=" << ( blob_size / 1024 ) << "kb";
#endif
        return false;
    }

    this->m_BitmapBuffer = this->m_InternalMemoryBlob->data;

    ( void ) ::memcpy(
        ( void* ) this->m_BitmapBuffer,
        ( const void* ) data,
        blob_size
    );

    return true;
}

bool Bitmap::reset( libgraphics::StdDynamicPoolAllocator* allocator, const libgraphics::Format& format, const int width, const int height ) {
    assert( allocator != nullptr );
    assert( width * height > 0 );

    if( allocator == nullptr ) {
        return false;
    }

    if( width * height <= 0 ) {
        return false;
    }

    if( ( this->format() == format ) && ( this->width() == width ) && ( this->height() == height ) && ( allocator == this->allocator() ) ) {
        const auto blob_size            = this->pixelCount() * this->formatByteSize();
        ( void ) ::memset(
            this->m_BitmapBuffer,
            0,
            blob_size
        );
        return true;
    }

    const auto successfullyResetted = this->reset();
    assert( successfullyResetted );

    if( !successfullyResetted ) {
#ifdef LIBGRAPHICS_DEBUG
        qDebug() << "Failed to reset bitmap. Aborting...";
#endif
        return false;
    }

    this->m_BitmapBuffer            = nullptr;
    this->m_BitmapHeight            = height;
    this->m_BitmapWidth             = width;
    this->m_InternalAllocator       = allocator;
    this->m_Format                  = format;

    const auto blob_size            = this->pixelCount() * this->formatByteSize();
    this->m_InternalMemoryBlob      = this->m_InternalAllocator->alloc(
                                          blob_size
                                      );
    assert( this->m_InternalMemoryBlob );
    assert( !this->m_InternalMemoryBlob->empty() );

    if( !this->m_InternalMemoryBlob || this->m_InternalMemoryBlob->empty() ) {
#ifdef LIBGRAPHICS_DEBUG
        qDebug() << "Failed to allocate memory blob of size=" << ( blob_size / 1024 ) << "kb";
#endif
        return false;
    }

    this->m_BitmapBuffer = this->m_InternalMemoryBlob->data;
    ( void ) ::memset(
        this->m_BitmapBuffer,
        0,
        blob_size
    );

    return true;
}

bool Bitmap::reset( libgraphics::StdDynamicPoolAllocator* allocator, const BitmapInfo& info ) {
    return reset(
               allocator,
               info.format(),
               info.width(),
               info.height()
           );
}

bool Bitmap::containsAllocator() const {
    return ( this->m_InternalAllocator != nullptr );
}

libgraphics::StdDynamicPoolAllocator*   Bitmap::allocator() const {
    return this->m_InternalAllocator;
}

bool Bitmap::assignAllocator(
    libgraphics::StdDynamicPoolAllocator* _allocator
) {
    assert( _allocator );

    /// relocate data from current to the newly assigned
    /// allocator
    if( this->m_BitmapBuffer != nullptr ) {
        assert( this->width() * this->height() > 0 );

        const auto blob_size            = this->formatByteSize() * this->pixelCount();
        auto new_allocator_blob         = _allocator->alloc(
                                              blob_size
                                          );
        assert( !new_allocator_blob || new_allocator_blob->empty() );

        if( !new_allocator_blob || new_allocator_blob->empty() ) {
#ifdef LIBGRAPHICS_DEBUG
            qDebug() << "Failed to assign allocator to bitmap. Couldn't allocate new shared memory blob.";
#endif
            return false;
        }

        ( void ) ::memcpy( ( void* )new_allocator_blob->data, ( const void* )this->m_BitmapBuffer, static_cast<size_t>( blob_size ) );

        if( this->m_InternalAllocator != nullptr ) {
            this->m_InternalMemoryBlob.reset();
            this->m_InternalAllocator = nullptr;
        } else {
            /// manually delete the internal bitmap buffer
            delete []( char* )this->m_BitmapBuffer;
        }

        this->m_InternalAllocator   = _allocator;
        this->m_InternalMemoryBlob  = new_allocator_blob;

        this->m_BitmapBuffer        = this->m_InternalMemoryBlob->data;
    } else {
        this->m_InternalAllocator = _allocator;
    }

    return true;
}

void Bitmap::clearAllocator() {
    if( this->m_InternalAllocator != nullptr ) {
        if( this->pixelCount() > 0 ) {
            const auto blob_size = this->formatByteSize() * this->pixelCount();
            void*      raw_blob  = ( void* ) new char[ blob_size ];
            assert( raw_blob != nullptr );

            ( void ) ::memcpy( ( void* )raw_blob, ( const void* )m_BitmapBuffer, blob_size );

            this->m_InternalMemoryBlob.reset();

            this->m_BitmapBuffer = raw_blob;
        }

        this->m_InternalAllocator = nullptr;
    }
}

bool Bitmap::saveRawData( const std::string& _path ) {
    assert( !_path.empty() );
    assert( !this->empty() );

    if( this->empty() || _path.empty() ) {
        return false;
    }

    std::ofstream file_out( _path.c_str(), std::ios_base::out | std::ios_base::binary );

    if( !file_out.is_open() ) {
        assert( false );
        return false;
    }

    file_out.write( ( const char* )this->m_BitmapBuffer, this->formatByteSize() * this->width() * this->height() );

    if( file_out.bad() ) {
        assert( false );
        return false;
    }

    return true;
}

bool Bitmap::loadRawData( const libgraphics::Format& _format, size_t width, size_t height, const std::string& _path )  {
    assert( !_path.empty() );
    assert( _format.byteSize != 0 );
    assert( _format.channels != 0 );
    assert( width != 0 );
    assert( height != 0 );

    if( _path.empty() || ( _format.byteSize == 0 ) || ( _format.channels == 0 ) ) {
        assert( false );
        return false;
    }

    std::ifstream file_in( _path.c_str(), std::ios_base::in | std::ios_base::binary | std::ios_base::ate );

    if( !file_in.is_open() ) {
        assert( false );
        return false;
    }

    const auto len = file_in.tellg();
    ( void ) file_in.seekg( 0, std::ifstream::beg );

    std::vector<char> buf;
    buf.reserve( len );
    assert( buf.size() == len );

    return this->reset( _format, width, height, buf.data() );
}

bool Bitmap::transformFormat( const libgraphics::Format& destinationFormat ) {
    const auto successfullyTransformed = libgraphics::transformFormat(
            this,
            this,
            destinationFormat
                                         );

    if( successfullyTransformed ) {
        this->m_Format = destinationFormat;
    }

    return successfullyTransformed;
}

libgraphics::Bitmap* Bitmap::toFormat(
    const libgraphics::Format& destinationFormat
) {
    return toFormat(
               destinationFormat,
               this->size()
           );
}

libgraphics::Bitmap* Bitmap::toFormat(
    const libgraphics::Format&  destinationFormat,
    const libgraphics::Rect32I& area
) {

    libgraphics::Bitmap* newBitmap = new Bitmap(
        this->allocator(),
        destinationFormat,
        area.width,
        area.height
    );
    assert( newBitmap != nullptr );

    if( newBitmap->empty() ) {
#if LIBGRAPHICS_DEBUG_OUTPUT
        LOG( "Failed to create new bitmap" );
#endif
        delete newBitmap;

        return nullptr;
    }

    const auto successfullyConverted = libgraphics::transformFormat(
                                           newBitmap,
                                           this,
                                           destinationFormat,
                                           newBitmap->size(),
                                           area
                                       );
    assert( successfullyConverted );

    if( !successfullyConverted ) {
#if LIBGRAPHICS_DEBUG_OUTPUT
        LOG( "Failed to convert to specified format." );
#endif
        delete newBitmap;

        return nullptr;
    }

    return newBitmap;
}

}
