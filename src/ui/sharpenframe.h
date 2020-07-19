#ifndef SharpenFrame_H
#define SharpenFrame_H

#include <QFrame>

#include "Graphics/ColorSpaces.h"

//! \brief Frame to show and select colors in the YUV color space
class SharpenFrame : public QFrame {
        Q_OBJECT
    public:
        explicit SharpenFrame( QWidget* parent = 0 );
        enum EnumColorSpace {
            EnumColorSpaceRGB,
            EnumColorSpaceSuperRGB,
            EnumColorSpaceUltraRGB
        };
        void setColorSpace( EnumColorSpace space );
        QString toString();

    signals:
        void signalWeights( float r, float g, float b );

    public slots:
        void slotSetRGB( blacksilk::graphics::ColorRGB<float> rgb );
        void slotSetLuma( float luma );

    private:
        QSizeF scale;
        bool mousePressed;
        QPointF mousePos;
        QImage background;

        EnumColorSpace mColorSpace;

        /* the YUV */
        blacksilk::graphics::YUV mYUV;

        int Umin;
        int Umax;
        int Vmin;
        int Vmax;

        virtual void paintEvent( QPaintEvent* );
        virtual void mouseReleaseEvent( QMouseEvent* );
        virtual void mouseMoveEvent( QMouseEvent* );
        virtual void mousePressEvent( QMouseEvent* );
        virtual void resizeEvent( QResizeEvent* );
        virtual void wheelEvent( QWheelEvent* );
        void drawContents( QPainter* painter );
        void emitAll();
        void renderBackground();
};

#endif // SharpenFrame_H
