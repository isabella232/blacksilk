#pragma once

#include <libgraphics/backend/gl/gl_resource.hpp>
#include <libgraphics/backend/gl/gl_resourcepool.hpp>
#include <libgraphics/utils/gl.hpp>

#include <libcommon/scopedptr.hpp>
#include <libcommon/atomics.hpp>

namespace libgraphics {
namespace backend {
namespace gl {

class Texture;
class Effect;
class PixelArray;

template < size_t len >
struct FloatArray {
    static const size_t count = len;
    FloatArray() {
        memset( ( void* )&data, 0, sizeof( float ) * len );
    }
    explicit FloatArray( float* _data ) {
        memcpy( ( void* )data, ( const void* )_data, sizeof( float ) * count );
    }
    FloatArray( const FloatArray& rhs ) {
        assign( rhs );
    }
    void assign( const FloatArray& rhs ) {
        memcpy( ( void* )data, ( const void* )rhs.data, sizeof( float ) * count );
    }

    float   data[count];
};
typedef FloatArray<2>   Vec2Data;
typedef FloatArray<3>   Vec3Data;
typedef FloatArray<4>   Vec4Data;

typedef FloatArray<9>   Mat9Data;
typedef FloatArray<16>  Mat16Data;

inline Vec4Data    createVec4Data( float r, float g, float b, float a ) {
    Vec4Data d;
    d.data[0] = r; d.data[1] = g; d.data[2] = b; d.data[3] = a;
    return d;
}
inline Vec3Data    createVec3Data( float r, float g, float b ) {
    Vec3Data d;
    d.data[0] = r; d.data[1] = g; d.data[2] = b;
    return d;
}
inline Vec2Data    createVec2Data( float r, float g ) {
    Vec2Data d;
    d.data[0] = r; d.data[1] = g;
    return d;
}

template < class _t_any >
struct EffectParameter {
        typedef _t_any  ValueType;

        inline EffectParameter() : m_ParameterId( -1 ), m_ParameterValue( ValueType() ), m_ParameterChanged( true ) {}
        inline explicit EffectParameter( const std::string& name ) : m_ParameterName( name ), m_ParameterId( -1 ),
            m_ParameterValue( ValueType() ), m_ParameterChanged( true ) {}
        inline EffectParameter( const std::string& name, const ValueType& val ) : m_ParameterName( name ), m_ParameterId( -1 ),
            m_ParameterValue( val ), m_ParameterChanged( true ) {}
        inline EffectParameter( const std::string& name, ValueType&& val ) : m_ParameterName( name ), m_ParameterId( -1 ), m_ParameterChanged( true ),
            m_ParameterValue( std::forward < ValueType && > ( val ) ) {}

        inline bool changed() const {
            return m_ParameterChanged;
        }
        inline int id() const {
            return m_ParameterId;
        }
        inline int& id() {
            return m_ParameterId;
        }
        inline void assignId( int cid ) {
            assert( ( cid != -1 ) );
            m_ParameterId = cid;
        }
        inline const ValueType& value() const {
            return m_ParameterValue;
        }
        inline ValueType& value() {
            return m_ParameterValue;
        }
        inline void reset( const ValueType& val ) {
            m_ParameterChanged = true;
            m_ParameterValue = val;
        }
        inline void reset( const std::string& name, const ValueType& val ) {
            m_ParameterChanged = true;
            m_ParameterValue = val;
            m_ParameterName = name;
        }
        inline const std::string& name() const {
            return m_ParameterName;
        }
        inline std::string& name() {
            return m_ParameterName;
        }
    private:
        std::string m_ParameterName;
        ValueType   m_ParameterValue;
        int         m_ParameterId;
        bool        m_ParameterChanged;
};
typedef EffectParameter<int>            EffectParameterInt;
typedef EffectParameter<short>          EffectParameterShort;
typedef EffectParameter<float>          EffectParameterFloat;
typedef EffectParameter<Vec2Data>       EffectParameterVec2;
typedef EffectParameter<Vec3Data>       EffectParameterVec3;
typedef EffectParameter<Vec4Data>       EffectParameterVec4;
typedef EffectParameter<Mat9Data>       EffectParameterMat9;
typedef EffectParameter<Mat16Data>      EffectParameterMat16;
typedef EffectParameter<Texture*>       EffectParameterTexture;
typedef EffectParameter<PixelArray*>    EffectParameterPixelArray;

void applyEffectParameterToEffect( Effect* effect, EffectParameterInt& param );
void applyEffectParameterToEffect( Effect* effect, EffectParameterShort& param );
void applyEffectParameterToEffect( Effect* effect, EffectParameterFloat& param );
void applyEffectParameterToEffect( Effect* effect, EffectParameterVec2& param );
void applyEffectParameterToEffect( Effect* effect, EffectParameterVec3& param );
void applyEffectParameterToEffect( Effect* effect, EffectParameterVec4& param );
void applyEffectParameterToEffect( Effect* effect, EffectParameterMat9& param );
void applyEffectParameterToEffect( Effect* effect, EffectParameterMat16& param );
void applyEffectParameterToEffect( Effect* effect, EffectParameterTexture& param );
void applyEffectParameterToEffect( Effect* effect, EffectParameterPixelArray& param );


class Effect : public backend::gl::Resource {
    public:
        struct Private;

        /// constr./destr.
        Effect();
        Effect( const char* name, const char* source );
        virtual ~Effect();

        virtual void reset();
        void reset( const char* name, const char* source );

        void bind();
        void updateTextureParameters();
        void unbind();

        bool empty() const;

        const char* name() const;

        bool containsParameter( const char* name );
        int getParameterId( const char* name );

        bool applyParameter( const int& id, int value );
        bool applyParameter( const int& id, short value );
        bool applyParameter( const int& id, float value );
        bool applyParameterVec2( const int& id, float* vals );
        bool applyParameterVec3( const int& id, float* vals );
        bool applyParameterVec4( const int& id, float* vals );
        bool applyParameterMat9( const int& id, float* vals );
        bool applyParameterMat16( const int& id, float* vals );
        bool applyParameterTexture( const int& id, Texture* tex );
        bool applyParameterPixelArray( const int& id, PixelArray* tex );

        /// from gl::Resource

        /// returns the gpu-side memory consumption of the
        /// current resource object.
        virtual size_t getGpuMemoryConsumption();

        /// returns the cpu-side memroy consumption of the
        /// current resource object.
        virtual size_t getCpuMemoryConsumption();

        /// returns true, if the current resource was already
        /// acquired.
        virtual bool acquired();

        /// tries to acquire the resource for the
        /// current thread.
        virtual bool tryAcquire();

        /// acquires the current resource.
        virtual void acquire();

        /// releases the resource from the
        /// current thread.
        virtual bool release();

        /// internals
        utils::GLProgram*   internalProgram() const;
    protected:
        libcommon::PimplPtr<Private>   d;
};


/// class: EffectPool
class EffectPool : public backend::gl::GenericResourcePool<Effect> {
    public:
        /// constr. / destr.
        EffectPool() {}
        virtual ~EffectPool() {}

        /// methods
        Effect* createEffect();
        Effect* createEffectFromSource( const char* name, const char* source );
        Effect* createEffectFromPath( const char* name, const char* path );
};

}
}
}
