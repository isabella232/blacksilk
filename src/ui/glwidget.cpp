#include <utils/app.hpp>

#include "glwidget.h"
#include <utils/hostmachine.hpp>
#include <QDebug>
#include <QImage>
#include <cmath>
#include <istream>
#include <fstream>
#include <iostream>

#include <libcommon/metrics.hpp>

#include <libgraphics/debug.hpp>
#include <libgraphics/image.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/backend/gl/gl_backenddevice.hpp>

/// \begin
/// \brief  namespaces
using namespace std;
using namespace blacksilk::ui;
// \end

GLWidget::GLWidget( QWidget* parent ) : QGLWidget( parent ) {
    /* make frame sensible for all events */
    setFocusPolicy( Qt::StrongFocus );

    this->m_FilterBWMixer           = nullptr;
    this->m_FilterCascadedSharpen   = nullptr;
    this->m_FilterCurves            = nullptr;
    this->m_FilterFilmGrain         = nullptr;
    this->m_FilterSplitTone         = nullptr;
    this->m_FilterVignette          = nullptr;

    /* mouse events */
    this->m_FitTo              = GLWidget::enumFitToAutomatic;
    this->m_MousePressed       = false;
    this->m_MousePosition      = QPointF();
    connect( &m_ClickTimer,  SIGNAL( timeout() ), SLOT( singleClick() ) );
    connect( &m_ResizeTimer, SIGNAL( timeout() ), SLOT( resize() ) );
}


/// \brief      Destructor, Frees all ressources
GLWidget::~GLWidget() {}

void GLWidget::wheelEvent( QWheelEvent* ev ) {
    static const int maxDeltaValue = 240;

    const int   initialDelta    = ev->delta();
    const int   delta           = std::min( std::max( initialDelta, -maxDeltaValue ), maxDeltaValue );
    const float amount          = ( float )delta / 120.0f;

    theApp()->view->setZoom( amount );
    this->update();
}

void GLWidget::slotZoomIn() {
    theApp()->view->zoomIn();
    this->update();
}

void GLWidget::slotZoomOut() {
    theApp()->view->zoomOut();
    this->update();
}

//! \brief Fit image into GLWidget
void GLWidget::mouseDoubleClickEvent( QMouseEvent* ev ) {
    Q_ASSERT( ev );
    m_ClickTimer.stop();

    if( ev->button() == Qt::RightButton ) {
        m_FitTo = GLWidget::enumFitToHeight;
        this->slotFitTo( GLWidget::enumFitToHeight );
    } else {
        m_FitTo = GLWidget::enumFitToWidth;
        this->slotFitTo( GLWidget::enumFitToWidth );
    }
}

//! \brief Shows original image
void GLWidget::singleClick() {
    theApp()->view->showOriginalImage() = true;
    update();
}

//! \brief Starts the double click timer
void GLWidget::mousePressEvent( QMouseEvent* event ) {
    m_MousePressed = true;
    m_MousePosition = event->pos();

    int timeBetweenDoubleClick = 250;
    m_ClickTimer.start( timeBetweenDoubleClick );

    update();
}

//! \brief Shows filtered image
void GLWidget::mouseReleaseEvent( QMouseEvent* ) {
    m_MousePressed = false;

    m_ClickTimer.stop();
    theApp()->view->showOriginalImage() = false;

    update();
}


void GLWidget::mouseMoveEvent( QMouseEvent* event ) {
    if( m_MousePressed ) {
        m_ClickTimer.stop();

        QPointF position = event->pos();
        float x = theApp()->view->center().x;
        float y = theApp()->view->center().y;

        x += ( position.x() - m_MousePosition.x() ) / ( theApp()->view->zoom() );
        y += ( position.y() - m_MousePosition.y() ) / ( theApp()->view->zoom() );

        theApp()->view->center().x = x;
        theApp()->view->center().y = y;

        m_MousePosition = position;

        update();
    }
}

void GLWidget::resize() {
    theApp()->view->resetViewport(
        libgraphics::Rect32UI(
            this->contentsRect().width(),
            this->contentsRect().height()
        )
    );
    theApp()->view->resetCenter();

    this->slotFitTo( m_FitTo );
    m_ResizeTimer.stop();
}

void GLWidget::resizeEvent( QResizeEvent* ) {
    m_ResizeTimer.start( 100 );
}

void GLWidget::slotFitTo( GLWidget::enumFitTo fit ) {

    if( !theApp()->hasImage() ) {
        return;
    }

    m_FitTo = fit;

    switch( m_FitTo ) {
        case GLWidget::enumFitToActualPixels:
            theApp()->view->fitImage( GraphicsView::EDimension::Pixels );
            break;

        case GLWidget::enumFitToHeight:
            theApp()->view->fitImage( GraphicsView::EDimension::Vertical );
            break;

        case GLWidget::enumFitToWidth:
            theApp()->view->fitImage( GraphicsView::EDimension::Horizontal );
            break;

        case GLWidget::enumFitToAutomatic:
            // TODO: Naming, GraphicsView::EDimension::Horizontal is actually automatic in both directions
            theApp()->view->fitImage( GraphicsView::EDimension::Horizontal );
            break;
    }

    update();
}

void GLWidget::renderMemoryInfo() {
    auto infoBlob = libgraphics::MemoryLog::global().sample();

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(
        0.0f,
        ( float )this->contentsRect().width(),
        0.0f,
        ( float )this->contentsRect().height(),
        -1.0f,
        1.0f
    );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    static const size_t     memoryInfoMemberCount = 16;

    QFont       renderFont( "Courier New", 12 );
    QString     stringList[memoryInfoMemberCount];
    {
        stringList[0] =     "allocatedCPUObjects:           " + QString::number( infoBlob.allocatedCPUObjects );
        stringList[1] =     "allocatedGPUObjects:           " + QString::number( infoBlob.allocatedGPUObjects );
        stringList[2] =     "totalCPUObjects:               " + QString::number( infoBlob.totalCPUObjects );
        stringList[3] =     "totalGPUObjects:               " + QString::number( infoBlob.totalGPUObjects );

        stringList[4] =     "allocatedGPUMemory:            " + QString::number( ( float )infoBlob.allocatedGPUMemory / ( 1024.0f * 1024.0f ) ) + " Mb";
        stringList[5] =     "totalGPUMemory:                " + QString::number( ( float )infoBlob.totalGPUMemory / ( 1024.0f * 1024.0f ) ) + " Mb";
        stringList[6] =     "allocatedCPUMemory:            " + QString::number( ( float )infoBlob.allocatedCPUMemory / ( 1024.0f * 1024.0f ) ) + " Mb";
        stringList[7] =     "totalCPUMemory:                " + QString::number( ( float )infoBlob.totalCPUMemory / ( 1024.0f * 1024.0f ) ) + " Mb";

        stringList[8] =     "gpuPoolSize:                   " + QString::number( infoBlob.gpuPoolSize );
        stringList[9] =     "gpuPoolCapacity:               " + QString::number( infoBlob.gpuPoolCapacity );
        stringList[10] =    "gpuAllocatedTextures:          " + QString::number( infoBlob.gpuAllocatedTextures );
        stringList[11] =    "gpuAllocatedEffects:           " + QString::number( infoBlob.gpuAllocatedEffects );
        stringList[12] =    "gpuAllocatedRenderTargets:     " + QString::number( infoBlob.gpuAllocatedRenderTargets );
        stringList[13] =    "imageLayers:                   " + QString::number( infoBlob.imageLayers );
        stringList[14] =    "cpuImageObjects:               " + QString::number( infoBlob.cpuImageObjects );
        stringList[15] =    "gpuImageObjects:               " + QString::number( infoBlob.gpuImageObjects );
    }

    glColor3f(
        1.0,
        0.0,
        0.0
    );

    float offsetY( 25.0f );

    for( size_t i = 0; memoryInfoMemberCount > i; ++i ) {
        // this->renderText(20.0f, this->contentsRect().height() - offsetY, 0.0f, stringList[i], renderFont );
        offsetY += 15.0f;
    }
}

void GLWidget::renderStats( size_t cpuBackendConsumption, size_t gpuBackendConsumption, size_t allocCapacity,
                            size_t allocSize, size_t renderTime, size_t glTiles, size_t glTilesUsed ) {
    /**
     * Mac: QGLWidget::renderText() produces wrong output under Mac. This won't be fixed in the near future,
     *  because it's a QT issue.
     */


    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(
        0.0f,
        ( float )this->contentsRect().width(),
        0.0f,
        ( float )this->contentsRect().height(),
        -1.0f,
        1.0f
    );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    QString cpuStr = "cpu backend memory:     " + QString::number( cpuBackendConsumption / ( 1024 * 1024 ) ) + "Mb";
    QString gpuStr = "gpu backend memory:     " + QString::number( gpuBackendConsumption / ( 1024 * 1024 ) ) + "Mb -- TexturePool: (" + QString::number( glTiles ) + "/" + QString::number( glTilesUsed ) + ")";
    QString asStr =  "allocator consumption:  " + QString::number( allocSize / ( 1024 * 1024 ) ) + "Mb(" + QString::number( ( ( float )allocSize / ( float )allocCapacity ) * 100.0f ) + "%)";
    QString acStr =  "allocator capacity:     " + QString::number( allocCapacity / ( 1024 * 1024 ) ) + "Mb";
    QString tStr  =  "frame time:             " + QString::number( renderTime ) + "ms @" + QString::number( 1000.0f / ( float )renderTime ) + " fps";
    QString pStr  =  "preview:                @" + QString::number( theApp()->preview.qualityFactor * 100.0f ) + "% quality, " + QString::number( theApp()->preview.maximalImageSize ) + " Max ImageSize";

    pStr += QString( ", ScaledDown: " ) + ( theApp()->preview.isScaledDown ? "yes" : "no" );

    QString infoStr = "preview-image:          " + QString::number( theApp()->currentSession->previewImage()->width() ) + "x" + QString::number( theApp()->currentSession->previewImage()->height() ) + "@" +
                      QString::number( theApp()->preview.currentImageSize ) + "Mb";

    const float imageSizeInMB = ( theApp()->currentSession->previewImage()->width() * theApp()->currentSession->previewImage()->height() *
                                  libgraphics::fxapi::EPixelFormat::getPixelSize( theApp()->currentSession->previewImage()->format() ) ) / ( 1000 * 1000 );
    infoStr += ", image-buffer-size:" + QString::number( imageSizeInMB ) + "Mb";

    QFont myFont( "Courier New", 12 );

    glColor3f(
        1.0,
        0.0,
        0.0
    );

    // this->renderText(20.0f,20.0f,0.0f,cpuStr,myFont);
    // this->renderText(20.0f,35.0f,0.0f,gpuStr,myFont);
    // this->renderText(20.0f,50.0f,0.0f,asStr,myFont);
    // this->renderText(20.0f,65.0f,0.0f,acStr,myFont);
    // this->renderText(20.0f,80.0f,0.0f,tStr,myFont);
    // this->renderText(20.0f,95.0f,0.0f,pStr,myFont);
    // this->renderText(20.0f,110.0f,0.0f,infoStr,myFont);
}

static size_t staticScheduledUpdateCounter = 0;
void GLWidget::slotUpdatePreview() {
    --staticScheduledUpdateCounter;
    this->update();
}

static size_t lastFrameElapsed = 0;
void GLWidget::paintGL() {
    this->makeCurrent();

    QElapsedTimer   t;
    size_t          elapsed( 0 );

    glColor3f( 0.0f, 0.0f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if( theApp()->view->valid() ) {
        t.start();
        const auto successfullyUpdatedPreview = theApp()->updatePreview();
        elapsed = t.elapsed();

        if( elapsed == 0 ) {
            elapsed = lastFrameElapsed;
        }

        glColor3f( 1.0, 1.0, 1.0 );

        if( !successfullyUpdatedPreview && theApp()->shouldRender ) {
            if( staticScheduledUpdateCounter == 0 ) {
                staticScheduledUpdateCounter++;

                QTimer::singleShot(
                    ( 1000 / theApp()->maxFPS() ) + 10,
                    Qt::PreciseTimer,
                    this,
                    SLOT( slotUpdatePreview() )
                );
            }
        }

        theApp()->view->update();
        theApp()->view->render();

        libgraphics::backend::gl::BackendDevice* glBackend = ( libgraphics::backend::gl::BackendDevice* )theApp()->appBackend->gpuBackend();
        assert( glBackend != nullptr );

        if( theApp()->preview.shouldRenderStats ) {
            const auto numberOfReservedTiles = glBackend->texturePool()->size();
            const auto numberOfUsedTiles     = glBackend->texturePool()->countAcquired();
            renderStats(
                theApp()->appBackend->cpuBackend()->queryBackendMemoryConsumption(),
                theApp()->appBackend->gpuBackend()->queryBackendMemoryConsumption(),
                theApp()->appBackend->allocator()->queryMemoryCapacity(),
                theApp()->appBackend->allocator()->queryMemoryConsumption(),
                elapsed,
                numberOfReservedTiles,
                numberOfUsedTiles
            );
        }

#if LIBGRAPHICS_MEMORY_LOG
        this->renderMemoryInfo();
#endif

        lastFrameElapsed = elapsed;
    }
}

void GLWidget::slotChangeMixer( float weight, float r1, float g1, float b1, float r2, float g2, float b2 ) {
    bool triggerRendering( false );

    if( theApp()->filterBWMixer->balance() != weight ) {
        theApp()->filterBWMixer->setBalance( weight );
        triggerRendering = true;
    }

    if( theApp()->filterBWMixer->highlightWeights().Values[0] != r1 ) {
        theApp()->filterBWMixer->highlightWeights().Values[0] = r1;
        triggerRendering = true;
    }

    if( theApp()->filterBWMixer->highlightWeights().Values[1] != g1 ) {
        theApp()->filterBWMixer->highlightWeights().Values[1] = g1;
        triggerRendering = true;
    }

    if( theApp()->filterBWMixer->highlightWeights().Values[2] != b1 ) {
        theApp()->filterBWMixer->highlightWeights().Values[2] = b1;
        triggerRendering = true;
    }

    if( theApp()->filterBWMixer->shadowWeights().Values[0] != r2 ) {
        theApp()->filterBWMixer->shadowWeights().Values[0] = r2;
        triggerRendering = true;
    }

    if( theApp()->filterBWMixer->shadowWeights().Values[1] != g2 ) {
        theApp()->filterBWMixer->shadowWeights().Values[1] = g2;
        triggerRendering = true;
    }

    if( theApp()->filterBWMixer->shadowWeights().Values[2] != b2 ) {
        theApp()->filterBWMixer->shadowWeights().Values[2] = b2;
        triggerRendering = true;
    }

    if( triggerRendering ) {
        theApp()->triggerRendering();
    }

    this->update();
}

void GLWidget::slotChangeVignette( float x, float y, float radius, float strength, bool extraBlur ) {
    ( void )extraBlur;

    if( theApp()->filterVignette != nullptr ) {
        bool triggerRendering( false );

        if( theApp()->filterVignette->radius() != radius ) {
            theApp()->filterVignette->setRadius( radius );
            triggerRendering = true;
        }

        if( theApp()->filterVignette->strength() != strength ) {
            theApp()->filterVignette->setStrength( strength );
            triggerRendering = true;
        }

        if( theApp()->filterVignette->center().x != x ) {
            theApp()->filterVignette->center().x = x;
            triggerRendering = true;
        }

        if( theApp()->filterVignette->center().y != y ) {
            theApp()->filterVignette->center().y = y;
            triggerRendering = true;
        }

        if( triggerRendering ) {
            theApp()->triggerRendering();
        }
    }


    this->update();
}

void GLWidget::slotChangeSharpen( QVector<float> s, float t ) {
    /// set threshold
    theApp()->filterCascadedSharpen->setThreshold(
        t
    );

    /// set strengths
    for( int i = 0; s.size() > i; ++i ) {
        theApp()->filterCascadedSharpen->setCascadeStrength( i, s[i] );
    }

    theApp()->triggerRendering();
    this->update();
}

void GLWidget::slotChangeSplit( float weight, float r1, float g1, float b1, float r2, float g2, float b2 ) {

    if( theApp()->filterSplitTone != nullptr ) {
        bool triggerRendering( false );

        if( theApp()->filterSplitTone->balance() != weight ) {
            theApp()->filterSplitTone->setBalance( weight );
            triggerRendering = true;
        }

        if( theApp()->filterSplitTone->highlights().Values[0] != r1 ) {
            theApp()->filterSplitTone->highlights().Values[0] = r1;
            triggerRendering = true;
        }

        if( theApp()->filterSplitTone->highlights().Values[1] != g1 ) {
            theApp()->filterSplitTone->highlights().Values[1] = g1;
            triggerRendering = true;
        }

        if( theApp()->filterSplitTone->highlights().Values[2] != b1 ) {
            theApp()->filterSplitTone->highlights().Values[2] = b1;
            triggerRendering = true;
        }

        if( theApp()->filterSplitTone->shadows().Values[0] != r2 ) {
            theApp()->filterSplitTone->shadows().Values[0] = r2;
            triggerRendering = true;
        }

        if( theApp()->filterSplitTone->shadows().Values[1] != g2 ) {
            theApp()->filterSplitTone->shadows().Values[1] = g2;
            triggerRendering = true;
        }

        if( theApp()->filterSplitTone->shadows().Values[2] != b2 ) {
            theApp()->filterSplitTone->shadows().Values[2] = b2;
            triggerRendering = true;
        }

        if( triggerRendering ) {
            theApp()->triggerRendering();
        }
    }

    this->update();
}

void GLWidget::slotChangeGrain( float radius, QVector<float> xs, QVector<float> ys ) {

    if( this->m_FilterFilmGrain != nullptr ) {
        m_FilterFilmGrain->resetCurve();

        for( int i = 0; xs.size() > i; ++i ) {
            libgraphics::Point32F point( xs[i], ys[i] );
            m_FilterFilmGrain->addCurvePoint( point );
        }

        m_FilterFilmGrain->setGrainBlurRadius( radius );
    }

    theApp()->triggerRendering();
    this->update();
}

void GLWidget::slotChangeCurves( QVector<float> xs, QVector<float> ys ) {

    if( this->m_FilterCurves != nullptr ) {
        m_FilterCurves->resetCurve();

        for( int i = 0; xs.size() > i; ++i ) {
            libgraphics::Point32F point( xs[i], ys[i] );
            m_FilterCurves->addCurvePoint( point );
        }
    }

    theApp()->triggerRendering();
    this->update();
}

//! \brief Reset all filters except mixer
void GLWidget::slotReset() {
    theApp()->currentSession->disableAllFilters();

    this->update();
}

void GLWidget::debugMinMax( void* data, int width, int height, bool is16Bit, bool isRgba ) {
    int mod = 3;

    if( isRgba ) {
        mod = 4;
    }

    /* debug */
    int vmax = 0;
    int vmin = 65535;

    if( !is16Bit ) {
        unsigned char* cdata = ( unsigned char* ) data;

        for( int i = 0; width - 1 >= i; i++ ) {
            for( int j = 0; height - 1 >= j; j++ ) {
                unsigned char r = cdata[( ( j * width )  + i ) * mod + 0];
                vmax = qMax( vmax, ( int )r );
                vmin = qMin( vmin, ( int )r );
            }
        }
    } else {
        unsigned short* sdata = ( unsigned short* ) data;

        for( int i = 0; i < width; i++ ) {
            for( int j = 0; height - 1 >= j; j++ ) {
                unsigned short r = sdata[( ( j * width )  + i ) * mod + 0];
                vmax = qMax( vmax, ( int )r );
                vmin = qMin( vmin, ( int )r );
            }
        }
    }

    qDebug() << "Final vmin: " << vmin;
    qDebug() << "Final vmax: " << vmax;
}

void GLWidget::setupPreview() {
    this->makeCurrent();
    initializeGL();

    this->m_FilterBWMixer = theApp()->filterBWMixer;
    this->m_FilterCascadedSharpen = theApp()->filterCascadedSharpen;
    this->m_FilterCurves = theApp()->filterCurves;
    this->m_FilterFilmGrain = theApp()->filterFilmGrain;
    this->m_FilterSplitTone = theApp()->filterSplitTone;
    this->m_FilterVignette = theApp()->filterVignette;

    theApp()->filterCascadedSharpen->deleteBlurBuffersForBackend( FXAPI_BACKEND_CPU );
    theApp()->filterCascadedSharpen->deleteBlurBuffersForBackend( FXAPI_BACKEND_OPENGL );

    theApp()->filterFilmGrain->resetGrain();
    theApp()->filterFilmGrain->resetCurve();

    theApp()->view->resetViewport(
        libgraphics::Rect32UI(
            this->contentsRect().width(), this->contentsRect().height()
        )
    );
    theApp()->view->resetCenter();
    theApp()->view->resetZoom();
    theApp()->view->showOriginalImage() = false;
    theApp()->triggerRendering();

    m_FitTo = GLWidget::enumFitToAutomatic;
    this->slotFitTo( m_FitTo );
}

void GLWidget::initializeGL() {
    this->makeCurrent();
    ( void ) theApp()->initializeGraphicsPreview();

    /// initialize backends and graphics
    assert( theApp() != nullptr );
}
