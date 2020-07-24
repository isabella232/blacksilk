
#include <QDebug>
#include <QThread>
#include <QRunnable>
#include <QThreadPool>

#include <libfoundation/app/backgroundtask.hpp>
#include <libfoundation/app/backgroundtaskgroup.hpp>
#include <libfoundation/app/backgroundtasklistener.hpp>

namespace libfoundation {
namespace app {

BackgroundTaskGroup::BackgroundTaskGroup(
    size_t _maximalThreadCount
) : m_ThreadPool( new QThreadPool() ) {
    this->m_ThreadPool->setMaxThreadCount(
        _maximalThreadCount
    );
}

BackgroundTaskGroup::~BackgroundTaskGroup() {
    this->waitForAll();
}

void BackgroundTaskGroup::enqueTask(
    BackgroundTask* task,
    ApplicationSession* session,
    BackgroundTaskListener* listener
) {
    assert( task != nullptr );
    assert( session != nullptr );

    const auto shouldStart = task->onStart(
                                 session
                             );
    assert( shouldStart != false );

    if( !shouldStart ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "task->onStart() failed. Task not executed.";
#endif
        return;
    }

    listener->onStart();

    struct Runnable : QRunnable {
        Runnable(
            BackgroundTask* _task,
            BackgroundTaskListener* _listener
        ) : task( _task ), listener( _listener ) {}

        virtual void run() {
            task->run(
                listener
            );
            listener->onFinish();
        }

        BackgroundTask* task;
        BackgroundTaskListener* listener;
    };
    Runnable* runnable = new Runnable(
        task,
        listener
    );
    runnable->setAutoDelete( true );

    this->m_ThreadPool->start( runnable );
}

void BackgroundTaskGroup::waitForAll() {
    this->m_ThreadPool->waitForDone();
}

std::unique_ptr<QThreadPool>& BackgroundTaskGroup::threadPool() {
    return this->m_ThreadPool;
}

const std::unique_ptr<QThreadPool>& BackgroundTaskGroup::threadPool() const {
    return this->m_ThreadPool;
}

void BackgroundTaskGroup::setMaximalThreadCount( size_t count ) {
    this->m_ThreadPool->setMaxThreadCount( count );
}

size_t BackgroundTaskGroup::maximalThreadCount() const {
    return this->m_ThreadPool->maxThreadCount();
}

size_t BackgroundTaskGroup::threadCount() const {
    return this->m_ThreadPool->activeThreadCount();
}

}
}
