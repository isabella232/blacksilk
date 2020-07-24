#pragma once

#include <string>

#include <libcommon/noncopyable.hpp>
#include <libcommon/scopedptr.hpp>

namespace libfoundation {
namespace app {

/**
    \class BackgroundTaskListener
    \since 1.0
    \brief Listens and reports a BackgroundTask's progress
        and results.
*/
class BackgroundTaskListener : public libcommon::INonCopyable {
    public:
        virtual ~BackgroundTaskListener() {}

        /**
            \fn reportProgress
            \since 1.0
            \brief Reports the current progress of the
                running task.
        */
        virtual void reportProgress(
            const std::string& subject,
            const size_t overallProgress /// from 0 to 100
        ) = 0;

        /**
            \fn onFinish
            \since 1.0
            \brief Called after the task finished
                execution.
        */
        virtual void onFinish() = 0;

        /**
            \fn onStart
            \since 1.0
            \brief Called before the task starts
                execution.
        */
        virtual void onStart() = 0;

        /**
            \fn reportError
            \since 1.0
            \brief Reports an error.
        */
        virtual void reportError(
            const std::string& subject,
            const std::string& message
        ) = 0;
};

}
}
