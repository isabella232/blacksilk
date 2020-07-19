
#include <libgraphics/io/pipelineprocessor.hpp>

namespace libgraphics {
namespace io {

struct CallbackProcessor : PipelineProcessor {
        CallbackProcessor( ProcessCallback* callback ) : m_Fn( callback ) {}
        virtual ~CallbackProcessor() {}
        virtual bool process(
            libgraphics::Bitmap* bitmap
        ) {
            assert( m_Fn );
            return m_Fn( bitmap );
        }
    private:
        ProcessCallback* m_Fn;
};
PipelineProcessor* makePipelineProcessorFromCallback(
    ProcessCallback* callback
) {
    return ( PipelineProcessor* )new CallbackProcessor( callback );
}

struct FunctionProcessor : PipelineProcessor {
        FunctionProcessor( const std::function<ProcessCallback>& fn ) : m_Fn( fn ) {}
        virtual ~FunctionProcessor() {}
        virtual bool process(
            libgraphics::Bitmap* bitmap
        ) {
            assert( m_Fn );
            return m_Fn( bitmap );
        }
    private:
        std::function<ProcessCallback> m_Fn;
};
PipelineProcessor* makePipelineProcessorFromFunction(
    const std::function<ProcessCallback>& fn
) {
    return ( PipelineProcessor* )new FunctionProcessor( fn );
}

}
}
