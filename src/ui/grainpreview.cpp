#include <QPainter>
#include <QDebug>

#include "grainpreview.h"
#include <cmath>

GrainPreview::GrainPreview( QWidget* parent ) :
    QFrame( parent ),
    mRadius( 0.0 ) {
    /* needed to set magic numbers, because rect() is not initialized yet
    *  values are from grain.ui */
    mBlurredGrain = QImage( QSize( 400, 60 ), QImage::Format_ARGB32 );
    slotSetRadius( 1.5 );
}

void GrainPreview::slotSetRadius( float radius ) {
    if( ! qFuzzyCompare( radius, mRadius ) ) {
        mRadius = radius;

        if( ! mBlurredGrain.isNull() ) {
            /* grain */
            QPainter painter( &mBlurredGrain );
            painter.setRenderHint( QPainter::Antialiasing, true );

            int first = floor( mRadius );
            int second = floor( ( mRadius - first ) * 10 ) < 5 ? 0 : 5;
            QString filename = QString( ":/Prerender/Icons/Blur/blur_%1_%2.png" ).arg( first ).arg( second );
            QImage grain( filename );

            if( ! grain.isNull() ) {
                painter.drawImage( 0, 0, grain );
            } else {
                qWarning() << "Cannot find preblurred map: " << filename;
            }
        }

        /* just reset alpha */
        slotAdjustWeights( mFulcrums );

        update();
    }
}

void GrainPreview::slotAdjustWeights( const QVector<float> fulcrums ) {
    if( fulcrums.size() > 0 ) {
        mFulcrums = fulcrums;
    }

    if( mFulcrums.size() == 0 ) {
        return;
    }

    int width = mRect.width();
    float factor = 255.0 / width;

    if( ! mBlurredGrain.isNull() ) {
        int bpl = mBlurredGrain.bytesPerLine();

        if( width < mBlurredGrain.width() ) {
            for( int x = 0; x < width; x++ ) {
                uchar* bits = mBlurredGrain.bits() + 4 * x + 3;

                int pos = ( int )( factor * x );

                if( pos < fulcrums.size() ) {
                    int alpha = 255.0 * fulcrums[pos];

                    for( int y = 0; y < 60; y++ ) {

                        *bits = alpha;
                        bits += bpl;
                    }
                } else {
                    // qWarning() << "QVector too small.";
                }
            }
        } else {
            qWarning() << "Prerendered map is too small.";
        }
    }

    update();
}

void GrainPreview::paintEvent( QPaintEvent* ) {

    mRect = rect();
    QSize vsize = mRect.size();

    QImage image( vsize, QImage::Format_ARGB32 );

    if( ! image.isNull() ) {
        QPainter painter( &image );

        /* render linear b/w gradient */
        QLinearGradient gradient;
        gradient.setStart( 0, 0 );

        gradient.setFinalStop( width(), 0 );

        gradient.setColorAt( 0.0, QColor( 0, 0, 0 ) );
        gradient.setColorAt( 1.0, QColor( 255, 255, 255 ) );
        painter.fillRect( mRect, gradient );

        painter.setCompositionMode( QPainter::CompositionMode_Overlay );

        if( ! mBlurredGrain.isNull() ) {
            painter.drawImage( 0, 0, mBlurredGrain );
        }
    }

    /* draw background */
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setClipRect( contentsRect() );
    painter.drawImage( 0, 0, image );

    /* rounded edges, looks nicer */
    QRectF rectangle( -1, -1, width() + 1, height() + 1 );
    painter.setPen( QPen( palette().window(), 5 ) );
    painter.drawRoundedRect( rectangle, 6.0, 6.0 );
}
