#include <libgraphics/io/pipelineimporter.hpp>

namespace libgraphics {
namespace io {

struct GenericPipelineImporter : public libgraphics::io::PipelineImporter {
        GenericPipelineImporter( const char* name, const char* extension ) : m_Name( name ), m_Extension( extension ) {}
        virtual ~GenericPipelineImporter() {}

        virtual void lock()  {
            while( !tryLock() ) {
                libcommon::sleep( 1 );
            }
        }
        virtual bool tryLock() {
            const auto threadId = libcommon::getCurrentThreadId();

            if( ( libcommon::atomics::exchange32( &m_Atomic, 0, threadId ) == 0 ) ||
                    ( libcommon::atomics::equal32( &m_Atomic, threadId ) ) ) {
                return true;
            }

            return false;
        }
        virtual void unlock() {
            if( libcommon::atomics::equal32( &m_Atomic, 0 ) ) {
                return;
            }

            const auto threadId = libcommon::getCurrentThreadId();

            while( !( libcommon::atomics::exchange32( &m_Atomic, threadId, 0 ) == threadId ) ) {
                libcommon::sleep( 1 );
            }
        }
        virtual const char* name() {
            return m_Name.c_str();
        }
        virtual bool supportsExtension(
            const char* extension
        ) {
            return ( m_Extension == extension );
        }
        virtual const char* mainExtension() {
            return m_Name.c_str();
        }
    private:
        std::string m_Name;
        std::string m_Extension;
        libcommon::atomics::type32  m_Atomic;
};

struct CallbackPipelineImporter : public GenericPipelineImporter {
        CallbackPipelineImporter(
            const char* name,
            const char* extension,
            CallbackImportFromData* fromData,
            CallbackImportFromPath* fromPath
        ) : GenericPipelineImporter( name, extension ), m_FromData( fromData ), m_FromPath( fromPath ) {}
        virtual ~CallbackPipelineImporter() {}

        virtual bool importFromData(
            void* data,
            size_t length,
            libgraphics::Bitmap* out
        ) {
            assert( m_FromData );

            return m_FromData( data, length, out );
        }

        virtual bool importFromPath(
            const char* path,
            libgraphics::Bitmap* out
        ) {
            assert( m_FromPath );

            return m_FromPath( path, out );
        }
        virtual bool supportsActionFromData() {
            return ( m_FromData != nullptr );
        }

        virtual bool supportsActionFromPath( const char* path ) {
            ( void )path;

            return ( m_FromPath != nullptr );
        }
    private:
        CallbackImportFromData*  m_FromData;
        CallbackImportFromPath*  m_FromPath;
};

PipelineImporter* makeImporterFromCallbacks(
    const char* name,
    const char* extension,
    CallbackImportFromData* fromDataCallback,
    CallbackImportFromPath* fromPathCallback
) {
    return ( PipelineImporter* )new CallbackPipelineImporter(
               name,
               extension,
               fromDataCallback,
               fromPathCallback
           );
}

struct FunctionPipelineImporter : public GenericPipelineImporter {
        FunctionPipelineImporter(
            const char* name,
            const char* extension,
            const std::function<CallbackImportFromData>& fromData,
            const std::function<CallbackImportFromPath>& fromPath
        ) : GenericPipelineImporter( name, extension ), m_FromData( fromData ), m_FromPath( fromPath ) {}
        virtual ~FunctionPipelineImporter() {}

        virtual bool importFromData(
            void* data,
            size_t length,
            libgraphics::Bitmap* out
        ) {
            assert( m_FromData );

            return m_FromData( data, length, out );
        }

        virtual bool importFromPath(
            const char* path,
            libgraphics::Bitmap* out
        ) {
            assert( m_FromPath );

            return m_FromPath( path, out );
        }
        virtual bool supportsActionFromData() {
            return ( m_FromData != nullptr );
        }

        virtual bool supportsActionFromPath( const char* path ) {
            ( void )path;

            return ( m_FromPath != nullptr );
        }
    private:
        std::function<CallbackImportFromData>  m_FromData;
        std::function<CallbackImportFromPath>  m_FromPath;
};


PipelineImporter* makeImporterFromFunctions(
    const char* name,
    const char* extension,
    std::function<CallbackImportFromData>   fromDataFn,
    std::function<CallbackImportFromPath>   fromPathFn
) {
    return ( PipelineImporter* )new FunctionPipelineImporter(
               name,
               extension,
               fromDataFn,
               fromPathFn
           );
}


}
}
