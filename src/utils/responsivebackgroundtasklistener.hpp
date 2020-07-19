#pragma once

#include <libfoundation/app/application.hpp>
#include <libfoundation/app/backgroundtasklistener.hpp>

#include <QStatusBar>
#include <QObject>
#include <QString>

namespace blacksilk {


class ResponsiveBackgroundTaskListener : public QObject, public libfoundation::app::BackgroundTaskListener {
        Q_OBJECT

    public:
        ResponsiveBackgroundTaskListener( QStatusBar* statusBar );
        virtual ~ResponsiveBackgroundTaskListener() {}

        virtual void reportProgress(
            const std::string& subject,
            const size_t overallProgress /// from 0 to 100
        );
        virtual void onFinish();
        virtual void onStart();
        virtual void reportError(
            const std::string& subject,
            const std::string& message
        );

    signals:
        void setStatusMessage( QString string );

    private:
        QStatusBar* m_StatusBar;
};

}
