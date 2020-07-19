
#include "wineventfilter.hpp"

#include <QApplication>
#include <QDebug>

#include "Windows.h"

WinEventFilter::WinEventFilter() {
}

bool WinEventFilter::nativeEventFilter( const QByteArray& eventType, void* message, long* /*result*/ ) {
    static const char WindowsGenericMsg[] = "windows_generic_MSG";

    if( ( eventType == WindowsGenericMsg ) && message ) {
        MSG* msg = ( MSG* ) message;

        if( msg->message == WM_QUERYENDSESSION ) {
            qDebug() << "Received WM_QUERYENDSESSION, quitting...";
            qApp->quit();
            return true;
        }
    }

    return false;
}
