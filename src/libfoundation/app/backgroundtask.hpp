#pragma once

#include <string>

#include <libcommon/noncopyable.hpp>
#include <libcommon/scopedptr.hpp>

namespace libfoundation {
namespace app {

class BackgroundTask;
class BackgroundTaskListener;

class ApplicationSession;


/**
    \class BackgroundTask
    \since 1.0
    \brief The BackgroundTask class is the base class for asynchronously executed
        background tasks. The same BackgroundTask instance should be able to run
        multiple times.
*/
class BackgroundTask : libcommon::INonCopyable {
    public:
        BackgroundTask( std::string _name );
        virtual ~BackgroundTask() {}

        /**
            \fn onStart
            \since 1.0
            \brief Called by the main thread before asynchronously executing the
                run() method. If onStart returns false, the associated ApplicationSession object
                won't call the run() method and discard the current task object.

                onStart() should clean-up the internal state.
        */
        virtual bool onStart(
            libfoundation::app::ApplicationSession* session
        ) = 0;

        /**
            \fn run
            \since 1.0
            \brief The asynchronously executed method. Called by a worker thread or an ApplicationSession
                instance. The BackgroundTask object should report status messages to the specified BackgroundTaskListener
                instance.
        */
        virtual void run(
            BackgroundTaskListener* listener
        ) = 0;

        /**
            \fn running
            \since 1.0
            \brief Returns true, if the current BackgroundTask object is currently
                executed.
        */
        virtual bool running() = 0;

        /**
            \fn name
            \since 1.0
            \brief Returns the current BackgroundTask's name.
        */
        std::string& name();
        const std::string& name() const;

        void setName( const std::string& value );
    private:
        std::string m_Name;
};

/**
    \class AsyncBackgroundTask
    \since 1.0
    \brief Base class for threaded BackgroundTask classes.
*/
class AsyncBackgroundTask : public BackgroundTask {
    public:
        AsyncBackgroundTask( const std::string& _name ) : BackgroundTask( _name ), m_Running( 0 ) {}
        virtual ~AsyncBackgroundTask() {}

        /**
            \fn running
            \since 1.0
            \brief Returns true, if the current BackgroundTask object is currently
                executed.
        */
        virtual bool running();
    protected:
        /**
            \fn beginExecution
            \since 1.0
            \brief Marks the current BackgroundTask instance as running.
        */
        void beginExecution();

        /**
            \fn endExecution
            \since 1.0
            \brief Marks the current BackgroundTask instance as stopped.
        */
        void endExecution();
    private:
        libcommon::Int64    m_Running;
};

}
}
