#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/image.hpp>

#include <libgraphics/backend/gl/gl_effect.hpp>
#include <libgraphics/backend/gl/gl_imageobject.hpp>
#include <libgraphics/backend/gl/gl_imageoperation.hpp>
#include <libgraphics/backend/gl/gl_rendertarget.hpp>
#include <libgraphics/backend/gl/gl_backenddevice.hpp>

#include <QDebug>

namespace libgraphics {
namespace fx {
namespace operations {

class GLImgOp : public backend::gl::ImageOperation {
    public:
        GLImgOp();
        virtual ~GLImgOp() {}

        virtual backend::gl::Effect*        getEffect();
        virtual bool                        initialize( libgraphics::fxapi::ApiBackendDevice* device );

        virtual std::string                 getKernelSource() = 0;
        virtual bool                        initializeParameters( backend::gl::Effect* effect ) = 0;
    protected:
        backend::gl::Effect*                            m_Effect;
        bool                                            m_Initialized;
};

bool renderTiledImageOperation(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source,
    libgraphics::Rect32I   area,
    backend::gl::ImageOperation* operation
);
bool renderTiledImageMergeOperation(
    libgraphics::fxapi::ApiBackendDevice* device,
    libgraphics::fxapi::ApiImageObject*     destination,
    libgraphics::fxapi::ApiImageObject*     source0,
    libgraphics::fxapi::ApiImageObject*     source1,
    libgraphics::Rect32I   area,
    backend::gl::ImageOperation* operation
);

bool replaceDirectives(
    std::vector<std::pair<std::string, std::string> >    directives,
    std::string&                                        source
);

struct GenericImgOpFromValue : public GLImgOp {
    virtual ~GenericImgOpFromValue() {}

    struct Params {
        backend::gl::EffectParameterFloat       value;
    };

    Params                              params;
    float                               value;

    std::pair<std::string, std::string>  opDirective;
    std::string                         source;

    GenericImgOpFromValue( const std::pair<std::string, std::string >& dir );
    virtual std::string                 getKernelSource();
    virtual bool                        initializeParameters( backend::gl::Effect* effect );
    virtual void submitImageMergeParameters( libgraphics::fxapi::ApiBackendDevice* device, libgraphics::fxapi::ApiImageObject* destination, libgraphics::fxapi::ApiImageObject* source, libgraphics::Rect32I area,
            backend::gl::Texture* texture, backend::gl::Texture* otherTexture, size_t tx, size_t ty );
    virtual void submitParameters( libgraphics::fxapi::ApiBackendDevice* device,
                                   libgraphics::fxapi::ApiImageObject* destination,
                                   libgraphics::fxapi::ApiImageObject* source,
                                   libgraphics::Rect32I   area,
                                   backend::gl::Texture* texture, size_t tx, size_t ty );
};

struct GenericImgOpChannelValue : public GLImgOp {
    virtual ~GenericImgOpChannelValue() {}

    struct Params {
        backend::gl::EffectParameterTexture     mainTexture;
        backend::gl::EffectParameterInt         width;
        backend::gl::EffectParameterInt         height;

        backend::gl::EffectParameterFloat       value;
    };

    Params                              params;
    float                               value;

    std::pair<std::string, std::string>  opDirective;
    std::string                         source;

    GenericImgOpChannelValue( const std::pair<std::string, std::string >& dir );
    virtual std::string                 getKernelSource();
    virtual bool                        initializeParameters( backend::gl::Effect* effect );
    virtual void submitImageMergeParameters( libgraphics::fxapi::ApiBackendDevice* device, libgraphics::fxapi::ApiImageObject* destination, libgraphics::fxapi::ApiImageObject* source, libgraphics::Rect32I area,
            backend::gl::Texture* texture, backend::gl::Texture* otherTexture, size_t tx, size_t ty );
    virtual void submitParameters( libgraphics::fxapi::ApiBackendDevice* device,
                                   libgraphics::fxapi::ApiImageObject* destination,
                                   libgraphics::fxapi::ApiImageObject* source,
                                   libgraphics::Rect32I   area,
                                   backend::gl::Texture* texture, size_t tx, size_t ty );
};


struct GenericImgOpColorValue : public GLImgOp {
    virtual ~GenericImgOpColorValue() {}

    struct Params {
        backend::gl::EffectParameterTexture     mainTexture;
        backend::gl::EffectParameterInt         width;
        backend::gl::EffectParameterInt         height;

        backend::gl::EffectParameterVec4        value;
    };

    Params                              params;
    libgraphics::backend::gl::Vec4Data  value;

    std::pair<std::string, std::string>  opDirective;
    std::string                         source;

    GenericImgOpColorValue( const std::pair<std::string, std::string >& dir );

    virtual std::string                 getKernelSource();
    virtual bool                        initializeParameters( backend::gl::Effect* effect );
    virtual void submitImageMergeParameters( libgraphics::fxapi::ApiBackendDevice* device, libgraphics::fxapi::ApiImageObject* destination, libgraphics::fxapi::ApiImageObject* source, libgraphics::Rect32I area,
            backend::gl::Texture* texture, backend::gl::Texture* otherTexture, size_t tx, size_t ty );
    virtual void submitParameters( libgraphics::fxapi::ApiBackendDevice* device,
                                   libgraphics::fxapi::ApiImageObject* destination,
                                   libgraphics::fxapi::ApiImageObject* source,
                                   libgraphics::Rect32I   area,
                                   backend::gl::Texture* texture, size_t tx, size_t ty );
};

struct GenericImgOpImageValue : public GLImgOp {
    virtual ~GenericImgOpImageValue() {}

    struct Params {
        backend::gl::EffectParameterTexture     mainTexture;
        backend::gl::EffectParameterInt         width;
        backend::gl::EffectParameterInt         height;

        backend::gl::EffectParameterTexture     value;
    };

    Params                              params;

    std::pair<std::string, std::string>  opDirective;
    std::string                         source;

    GenericImgOpImageValue( const std::pair<std::string, std::string >& dir );

    virtual std::string                 getKernelSource();
    virtual bool                        initializeParameters( backend::gl::Effect* effect );
    virtual void submitImageMergeParameters( libgraphics::fxapi::ApiBackendDevice* device, libgraphics::fxapi::ApiImageObject* destination, libgraphics::fxapi::ApiImageObject* source, libgraphics::Rect32I area,
            backend::gl::Texture* texture, backend::gl::Texture* otherTexture, size_t tx, size_t ty );

    virtual void submitParameters( libgraphics::fxapi::ApiBackendDevice* device,
                                   libgraphics::fxapi::ApiImageObject* destination,
                                   libgraphics::fxapi::ApiImageObject* source,
                                   libgraphics::Rect32I   area,
                                   backend::gl::Texture* texture, size_t tx, size_t ty );
};


std::pair<std::string, std::string> makeOpDirective( const std::string& operation );

}
}
}


#define LIBGRAPHICS_FILTER_INSTANCE(_type,_device,_name) \
    static std::unique_ptr< _type > _name; \
    static libgraphics::fxapi::ApiBackendDevice* _pf_static_device_ ##_name (nullptr); \
    if ( !_name || (_pf_static_device_ ##_name  != _device ) ) { \
        _name.reset( new _type() ); \
        (void) _name->initialize( _device ); \
        _pf_static_device_ ##_name = _device; \
    }

#define LIBGRAPHICS_FILTER_INSTANCE_EX(_type,_device,_name,_param) \
    static std::unique_ptr< _type > _name; \
    static libgraphics::fxapi::ApiBackendDevice* _pf_static_device_ ##_name (nullptr); \
    if ( !_name || (_pf_static_device_ ##_name  != _device ) ) { \
        _name.reset( new _type( _param ) ); \
        (void) _name->initialize( _device ); \
        _pf_static_device_ ##_name  = _device; \
    }
