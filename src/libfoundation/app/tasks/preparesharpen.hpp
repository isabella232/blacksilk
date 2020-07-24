#pragma once

#include <vector>
#include <string>

#include <libcommon/noncopyable.hpp>

#include <libfoundation/app/backgroundtask.hpp>

namespace libfoundation {
namespace app {
namespace tasks {

/**
    \class AsyncBackgroundTask
    \since 1.0
    \brief Base class for threaded BackgroundTask classes.
*/
class PrepareSharpen : public libfoundation::app::AsyncBackgroundTask {
    public:
        PrepareSharpen( const std::vector<float>& cascades ) : libfoundation::app::AsyncBackgroundTask( "PrepareSharpen" ) {
            configure( cascades );
        }
        virtual ~PrepareSharpen() {}

        /**
            \fn configure
            \since 1.0
            \brief Configures the PrepareSharpen-task(number of sharpen
                cascades).
        */
        void configure(
            const std::vector<float>&   cascades
        );

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
        );

        /**
            \fn run
            \since 1.0
            \brief The asynchronously executed method. Called by a worker thread or an ApplicationSession
                instance. The BackgroundTask object should report status messages to the specified BackgroundTaskListener
                instance.
        */
        virtual void run(
            BackgroundTaskListener* listener
        );
    private:
        libfoundation::app::ApplicationSession*         m_Session;
        libfoundation::app::BackgroundTaskListener*     m_Listener;
        std::vector<float>                              m_Cascades;
};

}
}
}
