#include <libgraphics/backend/common/formats.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/base.hpp>

namespace libgraphics {
namespace backend {

libgraphics::Format    fromCompatibleFormat( fxapi::EPixelFormat::t format ) {
    switch( format ) {
        case fxapi::EPixelFormat::Mono8:
            return libgraphics::formats::Mono8::toFormat();

        case fxapi::EPixelFormat::Mono16:
            return libgraphics::formats::Mono16::toFormat();

        case fxapi::EPixelFormat::RGB8:
            return libgraphics::formats::RGB8::toFormat();

        case fxapi::EPixelFormat::RGB16:
            return libgraphics::formats::RGB16::toFormat();

        case fxapi::EPixelFormat::RGBA8:
            return libgraphics::formats::RGBA8::toFormat();

        case fxapi::EPixelFormat::RGBA16:
            return libgraphics::formats::RGBA16::toFormat();

        default:
            assert( false );
            return libgraphics::Format();
    }
}

fxapi::EPixelFormat::t toCompatibleFormat( libgraphics::Format format ) {
    switch( format.family ) {
        case formats::family::Mono:
            switch( format.byteSize ) {
                case 1:
                    return fxapi::EPixelFormat::Mono8;

                case 2:
                    return fxapi::EPixelFormat::Mono16;

                default:
                    return fxapi::EPixelFormat::Empty;
            }

        case formats::family::RGB:
            switch( format.byteSize ) {
                case 3:
                    return fxapi::EPixelFormat::RGB8;

                case 6:
                    return fxapi::EPixelFormat::RGB16;

                default:
                    return fxapi::EPixelFormat::Empty;
            }

        case formats::family::RGBA:
            switch( format.byteSize ) {
                case 4:
                    return fxapi::EPixelFormat::RGBA8;

                case 8:
                    return fxapi::EPixelFormat::RGBA16;

                default:
                    return fxapi::EPixelFormat::Empty;
            }

        default:
            return fxapi::EPixelFormat::Empty;
    }
}

bool isCompatibleFormat( libgraphics::Format format ) {
    bool compatible = false;

    switch( format.channels ) {
        case 1:
            switch( format.family ) {
                case formats::family::Mono:
                    compatible = true;
                    break;

                default:
                    compatible = false;
                    break;
            }

            break;

        case 3:
            switch( format.family ) {
                case formats::family::RGB:
                    compatible = true;
                    break;

                default:
                    compatible = false;
                    break;
            }

            break;

        case 4:
            switch( format.family ) {
                case formats::family::RGBA:
                    compatible = true;
                    break;

                default:
                    compatible = false;
                    break;
            }

            break;

        default:
            compatible = false;
            break;
    }

    if( !compatible ) {
        return false;
    }

    switch( format.byteSize ) {
        case 3:
        case 4:
            compatible = true;
            break;

        default:
            compatible = false;
            break;
    }

    if( !compatible ) {
        return false;
    }

    return true;
}
bool isCompatibleFormat( libgraphics::Bitmap* bitmap ) {
    return isCompatibleFormat(
               bitmap->format()
           );
}

}
}
