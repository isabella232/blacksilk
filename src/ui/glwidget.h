#pragma once

#include <QVector>

/// basic graphics stuff
#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>

/// filters
#include <libgraphics/fx/filters/bwmixer.hpp>
#include <libgraphics/fx/filters/bwadaptivemixer.hpp>
#include <libgraphics/fx/filters/curves.hpp>
#include <libgraphics/fx/filters/cascadedsharpen.hpp>
#include <libgraphics/fx/filters/filmgrain.hpp>
#include <libgraphics/fx/filters/splittone.hpp>
#include <libgraphics/fx/filters/unsharpmask.hpp>
#include <libgraphics/fx/filters/vignette.hpp>

/// libfoundation stuff
#include <libfoundation/app/application.hpp>

/// qt stuff
#include <QtGui>
#include <QGLWidget>

namespace blacksilk {
namespace ui {

class GLWidget : public QGLWidget {
        Q_OBJECT
    public:
        enum enumFitTo { enumFitToHeight, enumFitToWidth, enumFitToActualPixels, enumFitToAutomatic };

        /// new vars
        std::unique_ptr<libfoundation::app::Application>           m_Application;
        std::unique_ptr<libfoundation::app::ApplicationBackend>    m_AppBackend;
        libfoundation::app::ApplicationSession*                         m_CurrentSession;

        libgraphics::fx::filters::BWAdaptiveMixer*                      m_FilterBWMixer;
        libgraphics::fx::filters::Curves*                               m_FilterCurves;
        libgraphics::fx::filters::CascadedSharpen*                      m_FilterCascadedSharpen;
        libgraphics::fx::filters::Vignette*                             m_FilterVignette;
        libgraphics::fx::filters::SplitTone*                            m_FilterSplitTone;
        libgraphics::fx::filters::FilmGrain*                            m_FilterFilmGrain;
        QTimer                                                          m_Timer;
    public:
        explicit GLWidget( QWidget* parent = 0 );

        virtual ~GLWidget();

        void initializeGL();
        void setupPreview();
        void paintGL();

        void renderStats( size_t cpuBackendConsumption, size_t gpuBackendConsumption, size_t allocCapacity,
                          size_t allocSize, size_t renderTime, size_t glTiles, size_t glTilesUsed );
        void renderMemoryInfo();

        /// \brief Debug min/max pixels of a buffer
        static void debugMinMax( void* data, int width, int height, bool is16Bit, bool isRgba );
    public slots:

        virtual void wheelEvent( QWheelEvent* ev );
        virtual void mouseDoubleClickEvent( QMouseEvent* );
        virtual void mousePressEvent( QMouseEvent* );
        virtual void mouseReleaseEvent( QMouseEvent* );
        virtual void mouseMoveEvent( QMouseEvent* );
        virtual void resizeEvent( QResizeEvent* );

        /// \brief      Renders the current image with all attached filters
        void slotZoomIn();
        void slotZoomOut();
        void slotFitTo( GLWidget::enumFitTo );
        void slotUpdatePreview();

        void slotChangeMixer( float weight, float r1, float g1, float b1, float r2, float g2, float b2 );
        void slotChangeVignette( float x, float y, float r, float s, bool e );
        void slotChangeSharpen( QVector<float> s, float t );
        void slotChangeSplit( float weight, float r1, float g1, float b1, float r2, float g2, float b2 );
        void slotChangeGrain( float radius, QVector<float> xs, QVector<float> ys );
        void slotChangeCurves( QVector<float> xs, QVector<float> ys );
        void slotReset();

    private:
        enumFitTo m_FitTo;
        QTimer m_ClickTimer;
        QTimer m_ResizeTimer;
        bool m_MousePressed;
        QPointF m_MousePosition;
    private slots:
        void resize();
        void singleClick();
};
}
}
