#pragma once

#include <QAbstractNativeEventFilter>

class WinEventFilter : public QAbstractNativeEventFilter {
    public:
        WinEventFilter();
    public:
        virtual bool nativeEventFilter( const QByteArray& eventType, void* message, long* result );
};
