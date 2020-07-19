#ifndef CURVEFRAME_H
#define CURVEFRAME_H

#include <QFrame>
#include <QTextStream>
#include <QPointF>

#include <memory>
#include <functional>

#define HISTOGRAM_SIZE 256

//! \brief QFrame class to hold and render the convolution curves
class CurveFrame : public QFrame {
        Q_OBJECT
        typedef std::unique_ptr<int, std::function<void ( int* )>> painter_guard;

    public:
        explicit  CurveFrame( QWidget* parent = 0 );
        enum frameType {
            frameTypeVertical,
            frameTypeHorizontal
        };
        enum interpolationType {
            interpolationTypeNone,
            interpolationTypeBezier
        };
        const static int mMargin;

        void getCoordinates( QVector<float>& mXs,  QVector<float>& mYs ) const;
        void setCoordinates( const QVector<float>& mXs, const QVector<float>& mYs );
        void setHistogram( const QVector<float>& values );
        void removeActiveHandle();
        bool isActive() const;
        void setframeType( const frameType mFrameType );
        QVector<float> getSpline( int count = HISTOGRAM_SIZE );
        QVector<float> getOutHistogram();
        void resetHandles();

        /* QWidgets from ui are usually pointer */
        friend QTextStream& operator << ( QTextStream& stream, const CurveFrame* const frame );
        friend QTextStream& operator >> ( QTextStream& stream, CurveFrame* const frame );

    signals:
        void signalPressed();
        void signalMoved( float, float );
        void signalStep(); // for undo

    public slots:
        void slotSetInterpolation( CurveFrame::interpolationType );
        void slotSetInactive();

    protected:
        virtual void paintEvent( QPaintEvent* );
        virtual void mousePressEvent( QMouseEvent* );
        virtual void mouseMoveEvent( QMouseEvent* );
        virtual void mouseReleaseEvent( QMouseEvent* );

    private:
        /* local helper function */
        float clamp( float var, float min, float max ) const;

        /* handles */
        QVector<float> mXs;
        QVector<float> mYs;

        /* histogram */
        QVector< float> mHistogram;
        QVector<float> mOutHistogram;
        QVector<float> mSpline;

        /* index of selected handle, -1 means no selection */
        int mSelectedHandle;

        bool mousePressed;
        frameType mFrameType;
        interpolationType mInterpolationType;

    private:
        void drawHistogram( QPainter* painter );
        void drawContents( QPainter* painter );
        void drawGrid( QPainter* painter );
        void drawRoundEdges( QPainter* painter );
        void drawBackground( QPainter* painter );
};

//! \brief Event filter class to forward key presses to the frames
class CurveFrameEventFilter : public QObject {
        Q_OBJECT

    public:
        //! \param f The CurveFrame* to be filtered
        explicit CurveFrameEventFilter( CurveFrame* f ): frame( f ) {}

    protected:
        bool eventFilter( QObject* obj, QEvent* event );

    private:
        CurveFrame* frame;
};

#endif // CURVEFRAME_H
