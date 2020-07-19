#ifndef VFRAME_H
#define VFRAME_H

#include <QFrame>
#include <QTimer>

//! \brief Frame to show and adjust the vignette
class VFrame : public QFrame {
        Q_OBJECT
    public:
        explicit VFrame( QWidget* parent = 0 );

    signals:
        void signalPosition( QPointF position );
        void signalRadius( float radius );
        void signalStrength( float strength );
        void signalMouseReleased();

    protected:
        virtual void paintEvent( QPaintEvent* );
        void drawContents( QPainter* painter );
        virtual void mousePressEvent( QMouseEvent* );
        virtual void mouseMoveEvent( QMouseEvent* );
        virtual void mouseReleaseEvent( QMouseEvent* );
        virtual void wheelEvent( QWheelEvent* );

    public slots:
        void slotSetRadius( float radius );
        void slotSetStrength( float strength );
        void slotSetX( float x );
        void slotSetY( float y );

    private:
        bool mMousePressed;
        QPointF mMousePosition;
        float mRadius;
        float mStrength;

        QTimer m_WheelTimer;
};

#endif // VFRAME_H
