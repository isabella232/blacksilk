
#include <QDebug>
#include <libgraphics/fx/operations/basic.hpp>
#include <libgraphics/fx/operations/complex.hpp>
#include <libgraphics/fx/filters/filmgrain.hpp>
#include <libgraphics/bezier.hpp>
#include <sstream>

namespace libgraphics {
namespace fx {
namespace filters {

FilmGrain::FilmGrain( fxapi::ApiBackendDevice* _device ) : Filter( "FilmGrain", _device ), m_ModifiedCurve( true ), m_MonoGrain( true ), m_GrainBlurRadius( 1.0f ) {}

bool FilmGrain::process(
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {
    return process(
               this->backend(),
               destination,
               source
           );
}

bool FilmGrain::process(
    fxapi::ApiBackendDevice*    device,
    libgraphics::ImageLayer*    destination,
    libgraphics::ImageLayer*    source
) {

    assert( device != nullptr );
    assert( destination != nullptr );
    assert( source != nullptr );

    if( device == nullptr ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to process FilmGrain filter using invalid backend device.";
#endif
        return false;
    }

    if( destination == nullptr ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to process FilmGrain filter using invalid destination buffer as target.";
#endif
        return false;
    }

    if( source == nullptr ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to process FilmGrain filter using invalid source buffer.";
#endif
        return false;
    }

    const size_t pixelMax = libgraphics::fxapi::EPixelFormat::getPixelMax( source->format() );

    if( this->m_ModifiedCurve || ( this->m_CurveData.size() != ( pixelMax + 1 ) ) ) {
        this->m_ModifiedCurve = true; /** ensure its actually set to true **/
        this->updateCurveData(
            pixelMax + 1
        );
    }

    libgraphics::fxapi::EPixelFormat::t pfFormat( destination->format() );

    if( this->m_MonoGrain && ( device->backendId() == FXAPI_BACKEND_OPENGL ) ) {
        const auto channelLen = libgraphics::fxapi::EPixelFormat::getPixelSize( pfFormat ) /
                                libgraphics::fxapi::EPixelFormat::getChannelCount( pfFormat );
        assert( channelLen != 0 );

        switch( channelLen ) {
            case 1:
                pfFormat = libgraphics::fxapi::EPixelFormat::Mono8;
                break;

            case 2:
                pfFormat = libgraphics::fxapi::EPixelFormat::Mono16;
                break;

            case 4:
                pfFormat = libgraphics::fxapi::EPixelFormat::Mono32F;
                break;

            default:
                pfFormat = libgraphics::fxapi::EPixelFormat::Empty;
        }

        assert( pfFormat != libgraphics::fxapi::EPixelFormat::Empty );
    }

    bool isCompatibleGrainImage( true );

    if( ( this->m_GrainLayer.get() == nullptr ) || ( this->m_GrainLayer->empty() ) ) {
        isCompatibleGrainImage = false;
    } else {
        if( ( this->m_GrainLayer->width() != destination->width() ) ||
                ( this->m_GrainLayer->height() != destination->height() ) ||
                ( ( this->m_GrainLayer->format() != destination->format() ) && ( this->m_GrainLayer->format() != pfFormat ) ) ||
                !( this->m_GrainLayer->containsDataForDevice( device ) ) ) {
            isCompatibleGrainImage = false;
        }
    }

    if( !isCompatibleGrainImage ) {
        this->resetGrain(
            device,
            pfFormat,
            destination->width(),
            destination->height()
        );
    }

    libcommon::ScopedPtr<ImageLayer> blurredGrainLayer( makeImageLayer( device, destination ) );

    if( ( ( this->m_GrainBlurRadius >= 0.05f ) ) ||
            ( ( this->m_GrainBlurRadius >= 0.05f ) && !isCompatibleGrainImage ) ) {
        libgraphics::fx::operations::gaussianBlur(
            device,
            blurredGrainLayer,
            this->m_GrainLayer,
            this->m_GrainLayer->size(),
            this->m_GrainBlurRadius
        );
    } else {
        libgraphics::fx::operations::blit(
            blurredGrainLayer,
            m_GrainLayer,
            m_GrainLayer->size(),
            m_GrainLayer->size()
        );
    }


    libgraphics::fx::operations::filmgrain(
        device,
        destination,
        source,
        source->size(),
        blurredGrainLayer,
        this->m_CurveData,
        this->m_MonoGrain
    );

    return true;
}

FilterPreset FilmGrain::toPreset() const {
    FilterPreset preset;

    // TODO: Needed?
    preset.ints()["Length"].value = ( int )this->m_CurveData.size();

    size_t index( 0 );

    for( auto it = this->m_CurvePoints.begin(); it != this->m_CurvePoints.end(); ++it ) {
        std::stringstream ss;
        ss << "Point";
        ss << index;

        preset.points()[ss.str()].value = ( *it );

        ++index;
    }

    preset.ints()["MonoGrain"].value    = this->monoGrain();
    preset.floats()["GrainBlurRadius"].value = this->m_GrainBlurRadius;

    preset.filterName() = this->m_Name;
    preset.name()       = "Current";

    return preset;
}

bool FilmGrain::fromPreset( const FilterPreset& preset ) {

    bool success( false );

    // TODO "Length" needed in preset?
    // if ( lengthIt->second.value > 0 )
    if( !preset.points().empty() ) {
        this->m_CurvePoints.clear();
        const auto points = preset.points();

        this->m_CurvePoints.resize( points.size() );

        std::transform(
            points.begin(),
            points.end(),
            m_CurvePoints.begin(),
        []( const std::pair<std::string, FilterPresetValuePoint>& p ) {
            return p.second.value;
        }
        );
        std::sort(
            this->m_CurvePoints.begin(),
            this->m_CurvePoints.end(),
        []( const Point32F & first, const Point32F & second ) {
            return ( first.x < second.x );
        }
        );

        this->m_ModifiedCurve = true;
    }

    if( preset.containsValueWithName( "MonoGrain" ) ) {
        const auto valueIt = preset.ints().find(
                                 "MonoGrain"
                             );

        if( valueIt != preset.ints().end() ) {
            const auto value    = ( *valueIt ).second.value;
            this->m_MonoGrain   = ( value == 1 );

            success = true;
        }
    }

    if( preset.containsValueWithName( "GrainBlurRadius" ) ) {
        const auto valueIt = preset.floats().find(
                                 "GrainBlurRadius"
                             );

        if( valueIt != preset.floats().end() ) {
            this->m_GrainBlurRadius = ( *valueIt ).second.value;

            success = true;
        }
    }

    return success;
}

/// random number generator
template < class _t_any >
struct rgen {
    typedef _t_any  ValueType;

    rgen(
        const ValueType& _min,
        const ValueType& _max
    ) : min( _min ), max( _max ), range( _max - _min ) { assert( _max > _min ); }

    inline ValueType gen() const {
        return ( ::rand() % range ) + min;
    }

    const ValueType     min;
    const ValueType     max;
    const ValueType     range;
};
template <>
struct rgen<float> {
    typedef float           ValueType;
    typedef unsigned int    RandomBaseType;

    rgen(
        const RandomBaseType& _min,
        const RandomBaseType& _max
    ) : min( _min ), max( _max ), range( _max - _min ) { assert( _max > _min ); }

    inline ValueType gen() const {
        const auto randomVal = ( ::rand() % range );
        return ( ( float )randomVal / ( float )range );
    }

    const RandomBaseType     min;
    const RandomBaseType     max;
    const RandomBaseType     range;
};
template < class _t_gen, class _t_value >
struct grain_render {
    typedef _t_value            ValueType;
    typedef _t_gen              GenType;

    static void render(
        const GenType& rgen,
        void* data,
        size_t channelCount,
        size_t grainWidth,
        size_t grainHeight,
        bool monoGrain
    ) {
        if( monoGrain ) {
            for( size_t x = 0; grainWidth > x; ++x ) {
                for( size_t y = 0; grainHeight > y; ++y ) {
                    ValueType* currentPixel     = ( ( ValueType* )data + ( ( ( y * grainWidth ) + x ) * channelCount ) );
                    const auto randomValue      = rgen.gen();

                    for( size_t i = 0; channelCount > i; ++i ) {
                        *( currentPixel + i ) = randomValue;
                    }
                }
            }
        } else {
            for( size_t x = 0; grainWidth > x; ++x ) {
                for( size_t y = 0; grainHeight > y; ++y ) {
                    ValueType* currentPixel     = ( ( ValueType* )data + ( ( ( y * grainWidth ) + x ) * channelCount ) );

                    for( size_t i = 0; channelCount > i; ++i ) {
                        const auto randomValue  = rgen.gen();
                        *( currentPixel + i ) = randomValue;
                    }
                }
            }
        }
    }
};

void FilmGrain::calculateGrainImage() {

    if( !this->m_GrainLayer.empty() ) {

        const auto grainFormat  = this->m_GrainLayer->format();
        const auto grainWidth   = this->m_GrainLayer->width();
        const auto grainHeight  = this->m_GrainLayer->height();
        const auto channelCount = fxapi::EPixelFormat::getChannelCount( grainFormat );

        assert( channelCount > 0 );
        assert( grainFormat != fxapi::EPixelFormat::Empty );
        assert( grainWidth * grainHeight != 0 );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

        if( grainFormat == fxapi::EPixelFormat::Empty ) {
            qDebug() << "Not able to recalculate grain image - Invalid grain format.";
            return;
        }

        if( ( grainWidth * grainHeight ) == 0 ) {
            qDebug() << "Not able to recalculate grain image - Grain image is of invalid size.";
            return;
        }

#endif
        typedef rgen<int>               rgen_int;
        typedef rgen<float>             rgen_float;


        const auto isFloatingPointFormat( fxapi::EPixelFormat::isFloatingPointFormat( grainFormat ) );
        const auto formatSize( fxapi::EPixelFormat::getPixelSize( grainFormat ) );
        const auto grainBufferSize( grainWidth * grainHeight * formatSize );

        assert( formatSize > 0 );
        assert( grainBufferSize > 0 );

        bool grainBufferWasLocallyAllocated( false );
        void* grainBuffer( nullptr );
        std::shared_ptr<libgraphics::StdDynamicPoolAllocator::Blob> grainBufferAllocBlob;

        if( m_GrainLayer->containsDataForBackend( FXAPI_BACKEND_CPU ) ) {
            grainBufferAllocBlob = m_GrainLayer->internalDeviceForBackend( FXAPI_BACKEND_CPU )->allocator()->alloc(
                                       grainBufferSize
                                   );
            assert( grainBufferAllocBlob.empty() == false );

            grainBuffer = grainBufferAllocBlob->data;
        }

        if( !grainBuffer ) {
            grainBuffer = ( void* ) new char[
                   grainBufferSize
               ];
            assert( grainBuffer != nullptr );

            grainBufferWasLocallyAllocated = true;
        }

        if( isFloatingPointFormat ) {
            const rgen_float rgen(
                fxapi::EPixelFormat::getPixelMin( fxapi::EPixelFormat::RGB16 ),
                fxapi::EPixelFormat::getPixelMax( fxapi::EPixelFormat::RGB16 )
            );

            grain_render <
            rgen_float,
            float
            >::render(
                rgen,
                grainBuffer,
                channelCount,
                grainWidth,
                grainHeight,
                this->monoGrain()
            );
        } else {
            const rgen_int rgen(
                fxapi::EPixelFormat::getPixelMin( grainFormat ),
                fxapi::EPixelFormat::getPixelMax( grainFormat )
            );

            switch( grainFormat ) {
                /// unsigned integer
                /// formats
                case fxapi::EPixelFormat::Mono8:
                case fxapi::EPixelFormat::RGB8:
                case fxapi::EPixelFormat::RGBA8:
                    grain_render <
                    rgen_int,
                    unsigned char
                    >::render(
                        rgen,
                        grainBuffer,
                        channelCount,
                        grainWidth,
                        grainHeight,
                        this->monoGrain()
                    );
                    break;

                case fxapi::EPixelFormat::Mono16:
                case fxapi::EPixelFormat::RGB16:
                case fxapi::EPixelFormat::RGBA16:
                    grain_render <
                    rgen_int,
                    unsigned short
                    >::render(
                        rgen,
                        grainBuffer,
                        channelCount,
                        grainWidth,
                        grainHeight,
                        this->monoGrain()
                    );
                    break;

                /// signed integer
                /// formats
                case fxapi::EPixelFormat::Mono16S:
                case fxapi::EPixelFormat::RGB16S:
                case fxapi::EPixelFormat::RGBA16S:
                    grain_render <
                    rgen_int,
                    signed short
                    >::render(
                        rgen,
                        grainBuffer,
                        channelCount,
                        grainWidth,
                        grainHeight,
                        this->monoGrain()
                    );
                    break;

                /// err
                default:
                    assert( false );
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                    qDebug() << "Error: Failed to generate grain - invalid format.";
#endif
                    break;
            }
        }

        const auto successfullyCopied = m_GrainLayer->copy(
                                            grainBuffer,
                                            m_GrainLayer->size(),
                                            0,
                                            0
                                        );
        assert( successfullyCopied );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT

        if( !successfullyCopied ) {
            qDebug() << "Failed to copy generated grain data to internal grain layer.";
        }

#endif

        if( grainBufferWasLocallyAllocated ) {
            delete []( char* )grainBuffer;
        }

    }
}

bool FilmGrain::loadGrainFromData(
    fxapi::ApiBackendDevice* device,
    fxapi::EPixelFormat::t format,
    size_t width,
    size_t height,
    void* data
) {
    assert( format != fxapi::EPixelFormat::Empty );
    assert( width * height >= 0 );
    assert( data != nullptr );

    if( device == nullptr || data == nullptr || ( width * height == 0 ) ) {
        return false;
    }

    resetGrain();

    this->m_GrainLayer.reset(
        makeImageLayer(
            device,
            "default",
            format,
            width,
            height,
            data
        )
    );
    assert( this->m_GrainLayer.get() != nullptr );

    return true;
}

void FilmGrain::resetGrain() {
    this->m_GrainLayer.reset();
}

void FilmGrain::resetGrain(
    fxapi::ApiBackendDevice* device,
    fxapi::EPixelFormat::t format,
    size_t width,
    size_t height
) {
    assert( format != fxapi::EPixelFormat::Empty );
    assert( width * height >= 0 );

    if( device == nullptr || ( width * height == 0 ) ) {
        return;
    }

    resetGrain();

    this->m_GrainLayer.reset(
        makeImageLayer(
            device,
            "default",
            format,
            width,
            height
        )
    );
    assert( this->m_GrainLayer.get() != nullptr );

    /*
    libgraphics::fx::operations::fill(
        this->m_GrainLayer.get(),
        this->m_GrainLayer->size(),
        128
    );*/
    this->calculateGrainImage();

    return;
}

void FilmGrain::resetGrain(
    libcommon::ScopedPtr<libgraphics::ImageLayer>& rhs
) {
    this->m_GrainLayer.assign( rhs );
}

libcommon::ScopedPtr<libgraphics::ImageLayer>&  FilmGrain::grainLayer() {
    return this->m_GrainLayer;
}

const libcommon::ScopedPtr<libgraphics::ImageLayer>&   FilmGrain::grainLayer() const {
    return this->m_GrainLayer;
}


void FilmGrain::assignCurve( const std::vector<libgraphics::Point32F>& points ) {
    this->m_ModifiedCurve = true;
    this->m_CurvePoints   = points;
}

void FilmGrain::resetCurve() {
    this->m_ModifiedCurve = true;

    this->m_CurveData.clear();
    this->m_CurvePoints.clear();
}

const size_t                    FilmGrain::curveLength() const {
    return this->m_CurvePoints.size();
}

libgraphics::Point32F&          FilmGrain::curvePointAtIndex( size_t index ) {
    assert( this->m_CurvePoints.size() >= index );
    this->m_ModifiedCurve = true;

    return *( this->m_CurvePoints.begin() + index );
}

const libgraphics::Point32F&    FilmGrain::curvePointAtIndex( size_t index ) const {
    assert( this->m_CurvePoints.size() >= index );

    return *( this->m_CurvePoints.begin() + index );
}

const std::vector<libgraphics::Point32F>&   FilmGrain::curve() const {
    return this->m_CurvePoints;
}

const std::vector<float>&                   FilmGrain::curveData() const {
    return this->m_CurveData;
}

void FilmGrain::addCurvePoint( const libgraphics::Point32F& curvePoint ) {
    this->m_ModifiedCurve = true;

    this->m_CurvePoints.push_back( curvePoint );
    std::sort(
        this->m_CurvePoints.begin(),
        this->m_CurvePoints.end(),
    []( const Point32F & first, const Point32F & second ) {
        return ( first.x < second.x );
    }
    );
}

bool FilmGrain::insertCurvePoint( std::vector<libgraphics::Point32F>::iterator where, const libgraphics::Point32F& curvePoint ) {
    assert( where != m_CurvePoints.end() );

    if( where != m_CurvePoints.end() ) {
        m_CurvePoints.insert( where, curvePoint );
        m_ModifiedCurve = true;

        return true;
    }

    return false;
}

bool FilmGrain::removeCurvePoint( std::vector<libgraphics::Point32F>::iterator which ) {
    assert( which != m_CurvePoints.end() );

    if( which != m_CurvePoints.end() ) {
        m_CurvePoints.erase( which );
        m_ModifiedCurve = true;

        return true;
    }

    return false;
}

void FilmGrain::updateCurveData( size_t dataLen ) {
    assert( !m_CurvePoints.empty() );
    assert( dataLen != 0 );

    if( this->m_ModifiedCurve ) {
        this->m_ModifiedCurve = false;

        this->m_CurveData.clear();

        std::vector<Point32F> plottedCurve;

        if( m_CurvePoints.size() == 2 ) {
            plottedCurve = libgraphics::calcLinear( this->m_CurvePoints, dataLen );
        } else {
            plottedCurve = libgraphics::calcBezier( this->m_CurvePoints, dataLen );
        }

        assert( plottedCurve.size() == dataLen );

        this->m_CurveData.resize( dataLen );

        std::transform(
            plottedCurve.begin(),
            plottedCurve.end(),
            m_CurveData.begin(),
        []( const libgraphics::Point32F & p ) {
            return p.y;
        }
        );
    }
}


bool& FilmGrain::monoGrain() {
    return this->m_MonoGrain;
}

const bool& FilmGrain::monoGrain() const {
    return this->m_MonoGrain;
}

void FilmGrain::setMonoGrain( bool value ) {
    this->m_MonoGrain = value;
}


float& FilmGrain::grainBlurRadius() {
    return this->m_GrainBlurRadius;
}

const float& FilmGrain::grainBlurRadius() const {
    return this->m_GrainBlurRadius;
}

void FilmGrain::setGrainBlurRadius( float radius ) {
    this->m_GrainBlurRadius = radius;
}

void FilmGrain::deleteGrainForBackend( int backendId ) {
    if( !this->m_GrainLayer.empty() ) {
        this->m_GrainLayer->deleteDataForBackend( backendId );

        this->m_ModifiedCurve   = true;
    }
}


Filter* FilmGrain::clone() {
    FilmGrain* clonedFilter = new FilmGrain(
        this->m_Device
    );
    clonedFilter->m_Device      = m_Device;
    clonedFilter->m_Name        = m_Name;

    clonedFilter->m_CurveData       = m_CurveData;
    clonedFilter->m_CurvePoints     = m_CurvePoints;
    clonedFilter->m_ModifiedCurve   = true;
    clonedFilter->m_GrainBlurRadius = m_GrainBlurRadius;
    clonedFilter->m_MonoGrain = m_MonoGrain;

    return ( Filter* )clonedFilter;
}

}
}
}
