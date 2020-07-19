#pragma once

#include <QtGui>
#include <QWidget>
#include <QGraphicsView>

#include <libcommon/scopedptr.hpp>
#include <libcommon/noncopyable.hpp>

#include <libfoundation/app/application.hpp>

namespace libfoundation {
namespace ui {

/*
class ImagingWidgetBaseRenderImpl;
class ImagingWidgetBase : public QObject, public libcommon::INonCopyable {
    struct Private;
public:
    Q_OBJECT

    struct EWidgetBackend {
        enum t {
            CPU,
            GL
        };
    };

    /// basic widget implementation
    ImagingWidgetBase(
        app::ApplicationSession* session,
        QWidget* baseWidget,
        EWidgetBackend::t targetBackend,
        int width,
        int height
    );
    virtual ~ImagingWidgetBase();

    /// properties
    int width() const;
    int height() const;

    void setView( const libpresentation::View& view );
    libpresentation::View&  view() const;
    const libpresentation::View&  view();

    app::ApplicationSession* session() const;
    QWidget* baseWidget() const;
    EWidgetBackend::t backend() const;

    /// basic methods
    virtual bool resize(
        int width,
        int height
    );
    virtual bool draw();

public slots:
    virtual void wheelEvent( QWheelEvent* ev );
    virtual void mouseDoubleClickEvent( QMouseEvent* );
    virtual void mousePressEvent( QMouseEvent* );
    virtual void mouseReleaseEvent( QMouseEvent* );
    virtual void mouseMoveEvent( QMouseEvent* );
    virtual void resizeEvent( QResizeEvent* );

private:
    libcommon::PimplPtr<Private>   d;
};*/

}
}
