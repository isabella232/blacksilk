
#include <gl/glew.h>
#include <QGLWidget>

#include <utils/offscreencontext.hpp>
#include <log/log.hpp>

namespace blacksilk {

struct OffscreenContext::Private : libcommon::PimplPrivate {
    Private() : widget( nullptr ) {}
    virtual ~Private() {
        ( void )destroy();
    }

    bool initialize() {
        if( widget != nullptr ) {
            return false;
        }

        try {
            widget = new QGLWidget();
            widget->makeCurrent();
            widget->hide();
        } catch( ... ) {
            delete widget;
            widget = nullptr;

            return false;
        }

        return true;
    }
    bool destroy() {
        if( widget == nullptr ) {
            return false;
        }

        delete widget;
        widget = nullptr;
        return true;
    }

    QGLWidget*          widget;
};

OffscreenContext::OffscreenContext() : d( new Private() ) {
    const auto successfullyInitialized = d->initialize();
    LOGB_ASSERT( successfullyInitialized, "OffscreenContext was not initialized successfully" );
}

OffscreenContext::~OffscreenContext() {}

bool    OffscreenContext::valid() const {
    return ( d->widget != nullptr );
}

}
