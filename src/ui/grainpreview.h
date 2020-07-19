#ifndef GRAINPREVIEW_H
#define GRAINPREVIEW_H

#include <QFrame>

//! \brief Frame to show a grain preview on a b/w gradient
class GrainPreview : public QFrame {
        Q_OBJECT
    public:
        explicit GrainPreview( QWidget* parent = 0 );

    signals:

    public slots:
        void slotSetRadius( float radius );
        void slotAdjustWeights( const QVector<float> fulcrums );

    private:
        float mRadius;
        QVector<float> mFulcrums;
        QRect mRect;
        QImage mBlurredGrain;

        virtual void paintEvent( QPaintEvent* );
        void blur( QImage& in, float radius ) const;

};

#endif // GRAINPREVIEW_H
