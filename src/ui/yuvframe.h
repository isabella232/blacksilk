#ifndef YUVFRAME_H
#define YUVFRAME_H

#include <QFrame>
#include <QTimer>

#include "utils/colorspaces.hpp"

//! \brief Frame to show and select colors in the YUV color space
class YUVFrame : public QFrame {
        Q_OBJECT
    public:
        explicit YUVFrame( QWidget* parent = 0 );
        enum EnumColorSpace {
            EnumColorSpaceRGB,
            EnumColorSpaceSuperRGB,
            EnumColorSpaceUltraRGB
        };
        void setColorSpace( EnumColorSpace space );
        QString toString();

    signals:
        void signalWeights( float r, float g, float b );
        void signalMouseReleased();

    public slots:
        void slotSetRGB( blacksilk::graphics::ColorRGB<float> rgb );
        void slotSetLuma( float luma );

    private:
        QSizeF scale;
        bool mousePressed;
        QPointF mousePos;
        QImage background;
        QString filter;

        EnumColorSpace mColorSpace;

        /* the YUV */
        blacksilk::graphics::YUV mYUV;

        float Umin;
        float Umax;
        float Vmin;
        float Vmax;

        virtual void paintEvent( QPaintEvent* );
        virtual void mouseReleaseEvent( QMouseEvent* );
        virtual void mouseMoveEvent( QMouseEvent* );
        virtual void mousePressEvent( QMouseEvent* );
        virtual void resizeEvent( QResizeEvent* );
        virtual void wheelEvent( QWheelEvent* );
        void drawContents( QPainter* painter );
        void emitAll();
        void renderBackground();

        QTimer m_WheelTimer;
};

#endif // YUVFRAME_H
