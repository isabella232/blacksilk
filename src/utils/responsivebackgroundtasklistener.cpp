
#include <QDebug>
#include <QStatusBar>
#include <utils/responsivebackgroundtasklistener.hpp>

namespace blacksilk {

ResponsiveBackgroundTaskListener::ResponsiveBackgroundTaskListener( QStatusBar* statusBar ) : m_StatusBar( statusBar ) {
    QObject::connect(
        this,
        SIGNAL( setStatusMessage( QString ) ),
        m_StatusBar,
        SLOT( showMessage( QString ) )
    );
}

void ResponsiveBackgroundTaskListener::reportProgress(
    const std::string& subject,
    const size_t overallProgress /// from 0 to 100
) {
    ( void )overallProgress;

    const std::string _sharpen = "PrepareSharpen";
    const std::string _grain   = "RenderGrain";

    if( subject == _sharpen ) {
        /// report sharpen preperation
        emit setStatusMessage( "Preparing sharpen cascades..." );
    } else if( subject == _grain ) {
        emit setStatusMessage( "Generating grain..." );
    }
}

void ResponsiveBackgroundTaskListener::onFinish() {
    emit setStatusMessage( "" );
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    qDebug() << "Task finished...";
#endif
}

void ResponsiveBackgroundTaskListener::onStart() {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    qDebug() << "Starting to listen for task...";
#endif
}

void ResponsiveBackgroundTaskListener::reportError(
    const std::string& subject,
    const std::string& message
) {
    qDebug() << "Failed to execute background task - " << subject.c_str() << " - msg: " << message.c_str();
}


}
