#include <libgraphics/io/pipelineexporter.hpp>

namespace libgraphics {
namespace io {

struct GenericPipelineExporter : public libgraphics::io::PipelineExporter {
        GenericPipelineExporter( const char* name, const char* extension ) : m_Name( name ), m_Extension( extension ) {}
        virtual ~GenericPipelineExporter() {}
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
};

struct CallbackPipelineExporter : public GenericPipelineExporter {
        CallbackPipelineExporter(
            const char* name,
            const char* extension,
            CallbackExportToStream* toStream,
            CallbackExportToPath* toPath
        ) : GenericPipelineExporter( name, extension ), m_ToStream( toStream ), m_ToPath( toPath ) {}
        virtual ~CallbackPipelineExporter() {}

        virtual bool exportToStream(
            void* data,
            size_t length,
            libgraphics::Bitmap* toSave
        ) {
            assert( m_ToStream );

            return m_ToStream( data, length, toSave );
        }

        virtual bool exportToPath(
            const char* path,
            libgraphics::Bitmap* toSave
        ) {
            assert( m_ToPath );

            return m_ToPath( path, toSave );
        }
        virtual bool supportsActionFromData() {
            return ( m_ToStream != nullptr );
        }

        virtual bool supportsActionFromPath( const char* path ) {
            ( void )path;

            return ( m_ToPath != nullptr );
        }
    private:
        CallbackExportToStream*  m_ToStream;
        CallbackExportToPath*  m_ToPath;
};

PipelineExporter* makeExporterFromCallbacks(
    const char* name,
    const char* extension,
    CallbackExportToStream* toStreamCallback,
    CallbackExportToPath* toPathCallback
) {
    return ( PipelineExporter* )new CallbackPipelineExporter(
               name,
               extension,
               toStreamCallback,
               toPathCallback
           );
}

struct FunctionPipelineExporter : public GenericPipelineExporter {
        FunctionPipelineExporter(
            const char* name,
            const char* extension,
            const std::function<CallbackExportToStream>& fromData,
            const std::function<CallbackExportToPath>& fromPath
        ) : GenericPipelineExporter( name, extension ), m_ToStream( fromData ), m_ToPath( fromPath ) {}
        virtual ~FunctionPipelineExporter() {}


        virtual bool exportToStream(
            void* data,
            size_t length,
            libgraphics::Bitmap* toSave
        ) {
            assert( m_ToStream );

            return m_ToStream( data, length, toSave );
        }

        virtual bool exportToPath(
            const char* path,
            libgraphics::Bitmap* toSave
        ) {
            assert( m_ToPath );

            return m_ToPath( path, toSave );
        }

        virtual bool supportsActionFromData() {
            return ( m_ToStream != nullptr );
        }

        virtual bool supportsActionFromPath( const char* path ) {
            ( void )path;

            return ( m_ToPath != nullptr );
        }
    private:
        std::function<CallbackExportToStream>  m_ToStream;
        std::function<CallbackExportToPath>  m_ToPath;
};


PipelineExporter* makeExporterFromFunctions(
    const char* name,
    const char* extension,
    std::function<CallbackExportToStream>   toStreamFn,
    std::function<CallbackExportToPath>   toPathFn
) {
    return ( PipelineExporter* )new FunctionPipelineExporter(
               name,
               extension,
               toStreamFn,
               toPathFn
           );
}


}
}
