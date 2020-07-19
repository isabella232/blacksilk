
#include <libfoundation/ui/widget.hpp>

namespace libfoundation {
namespace ui {
/*
class ImagingWidgetBaseRenderImpl {
public:
    virtual ~ImagingWidgetBaseRenderImpl() {}

    virtual bool reset(
        QWidget* widget,
        libgraphics::ImageLayer* layer,
        libgraphics::fxapi::ApiBackendDevice* backendDevice
    ) = 0;
    virtual bool resize(
        int width,
        int height
    ) = 0;
    virtual bool draw( const libpresentation::View& view ) = 0;
};
class ImagingWidgetBaseRenderImplCPU : public ImagingWidgetBaseRenderImpl {
public:
    ImagingWidgetBaseRenderImplCPU() : m_BaseWidget(nullptr),
        m_Layer(nullptr), m_BackendDevice(nullptr), m_SceneItem(nullptr) {}
    virtual ~ImagingWidgetBaseRenderImplCPU() {}

    virtual bool reset(
        QWidget* widget,
        libgraphics::ImageLayer* layer,
        libgraphics::fxapi::ApiBackendDevice* backendDevice
    ) {
        this->m_BaseWidget = widget;
        assert(this->m_BaseWidget);

        this->m_Layer = layer;
        assert(this->m_Layer);

        this->m_BackendDevice = backendDevice;
        assert(this->m_BackendDevice);

        if ( m_InternalScene.get() == nullptr ) {
            m_InternalScene.reset( new QGraphicsScene() );

            this->m_SceneItem = m_InternalScene->addPixmap(m_SharedPixmap);
        }

        return true;
    }

    virtual bool resize(
        int width,
        int height
    ) {
        assert(m_BaseWidget);
        m_BaseWidget->resize(
            width,
            height
        );

        return true;
    }

    virtual bool draw( const libpresentation::View& view ) {
        QGraphicsView*  graphicsView = (QGraphicsView*)this->m_BaseWidget;


    }
protected:
    libcommon::ScopedPtr<QGraphicsScene>    m_InternalScene;
    QWidget*    m_BaseWidget;
    QGraphicsPixmapItem*    m_SceneItem;
    libgraphics::ImageLayer*    m_Layer;
    libgraphics::fxapi::ApiBackendDevice* m_BackendDevice;
    QPixmap m_SharedPixmap;
};

struct ImagingWidgetBase::Private : libcommon::PimplPrivate {
    libpresentation::View       view;
    ImagingWidgetBase::EWidgetBackend::t        backendType;
    QWidget*        baseWidget;
    app::ApplicationSession*        session;
    ImagingWidgetBaseRenderImpl*    impl;

    Private() : backendType(ImagingWidgetBase::EWidgetBackend::CPU), baseWidget(nullptr),
        session(nullptr) {}
};

ImagingWidgetBase::ImagingWidgetBase(
    app::ApplicationSession* session,
    QWidget* baseWidget,
    EWidgetBackend::t targetBackend,
    int width,
    int height
) {

}

ImagingWidgetBase::~ImagingWidgetBase() {}

/// properties
int ImagingWidgetBase::width() const {

}

int ImagingWidgetBase::height() const {

}

void ImagingWidgetBase::setView( const libpresentation::View& view ) {

}

libpresentation::View&  ImagingWidgetBase::view() const {

}

const libpresentation::View&  ImagingWidgetBase::view() {

}

app::ApplicationSession* ImagingWidgetBase::session() const {

}

QWidget* ImagingWidgetBase::baseWidget() const {

}

ImagingWidgetBase::EWidgetBackend::t ImagingWidgetBase::backend() const {

}

/// basic methods
bool ImagingWidgetBase::resize(
    int width,
    int height
) {

}

bool ImagingWidgetBase::draw() {

}*/

}
}
