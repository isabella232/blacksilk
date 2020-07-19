#pragma once

#include <string>

#include <libcommon/mutex.hpp>
#include <libcommon/lockable.hpp>
#include <libcommon/noncopyable.hpp>
#include <libcommon/scopedptr.hpp>


class QThreadPool;


namespace libfoundation {
namespace app {

class BackgroundTask;
class BackgroundTaskListener;
class ApplicationSession;

/**
    \class BackgroundTaskGroup
    \since 1.0
    \brief Manages the execution of BackgroundTask instances.
*/
class BackgroundTaskGroup : public libcommon::INonCopyable {
    public:
        static const size_t defaultMinimalThreadCount = 1;

        BackgroundTaskGroup(
            size_t _maximalThreadCount = defaultMinimalThreadCount
        );
        virtual ~BackgroundTaskGroup();

        /**
            \fn enqueTask
            \since 1.0
            \brief Enques a new background task. The BackgroundTaskGroup instance deletes
                the specified task object after finishing execution.
        */
        void enqueTask(
            BackgroundTask* task,
            ApplicationSession* session,
            BackgroundTaskListener* listener
        );

        /**
            \fn waitForAll
            \since 1.0
            \brief Waits for all tasks to complete. Blocks the calling
                thread.
        */
        void waitForAll();

        /**
            \fn threadPool
            \since 1.0
            \brief Returns the internal thread pool.
        */
        libcommon::ScopedPtr<QThreadPool>& threadPool();
        const libcommon::ScopedPtr<QThreadPool>& threadPool() const;

        /**
            \fn setMaximalThreadCount
            \since 1.0
            \brief Sets the maximal number of concurrently executed
                tasks.
        */
        void setMaximalThreadCount( size_t count );

        /**
            \fn maximalThreadCount
            \since 1.0
            \brief Returns the number of maximal concurrently
                executed tasks.
        */
        size_t maximalThreadCount() const;

        /**
            \fn threadCount
            \since 1.0
            \brief The number of currently running
                threads(=tasks).
        */
        size_t threadCount() const;
    private:
        libcommon::ScopedPtr<QThreadPool> m_ThreadPool;
};

}
}
