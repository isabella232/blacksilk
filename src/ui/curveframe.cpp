
#include "curveframe.h"

#include <cmath>
#include <algorithm>

#include <utils/curve.hpp>
#include <libgraphics/bezier.hpp>

#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>

const int CurveFrame::mMargin = 10;

//! \brief  Filters key action for suitable ones for the frame\n
//!         DEL removes the frame's active handle
//! \param obj Unused
//! \param event The key press
//! \see void CurveFrame::removeActiveHandle()
bool CurveFrameEventFilter::eventFilter( QObject* obj, QEvent* event ) {
    if( !frame ) {
        return QObject::eventFilter( obj, event );
    }

    if( !frame->isActive() ) {
        return QObject::eventFilter( obj, event );
    }

    if( !frame->isVisible() ) {
        return QObject::eventFilter( obj, event );
    }

    if( event->type() != QEvent::KeyPress ) {
        return QObject::eventFilter( obj, event );
    }

    QKeyEvent* keyEvent = static_cast<QKeyEvent*>( event );

    if( !keyEvent ) {
        return QObject::eventFilter( obj, event );
    }

    if( keyEvent->key() == Qt::Key_Delete ) {
        frame->removeActiveHandle();
        return true;
    } else {
        return QObject::eventFilter( obj, event );
    }
}

QTextStream& operator << ( QTextStream& out, const CurveFrame* const frame ) {
    QVector< float > xs;
    QVector< float > ys;

    if( frame->mFrameType == CurveFrame::frameTypeVertical ) {
        out << "# Vertical: \n";
    } else {
        out << "# Horizontal: \n";
    }

    frame->getCoordinates( xs, ys );

    foreach( float val, xs ) {
        out << val << " ";
    }

    out << "\n";

    foreach( float val, ys ) {
        out << val << " ";
    }

    out << "\n";
    return out;
}

//! \brief  Reading from a QTextStream;\n
//!         The stream is not const, because it advances two lines
QTextStream& operator >> ( QTextStream& in, CurveFrame* const frame ) {
    QVector< QVector< float > > all;
    int counter = 0;

    while( ! in.atEnd() && counter < 2 ) {
        QString line  = in.readLine();

        /* checks */
        if( line.size() < 0 ) {
            continue;
        }

        if( line.at( 0 ) == '#' ) {
            continue;
        }

        /* fill up data points */
        QVector< float > one;
        QStringList lines = line.split( " ", QString::SkipEmptyParts );

        foreach( QString element, lines ) {
            bool ok = false;
            float num = element.toFloat( &ok );

            if( ok ) {
                one.push_back( num );
            }
        }

        all.push_back( one );

        /* only increment counter, if we have some valid data */
        counter++;
    }

    frame->setCoordinates( all[0], all[1] );
    return in;
}

CurveFrame::CurveFrame( QWidget* parent ) :
    QFrame( parent ),
    mousePressed( false ),
    mFrameType( frameTypeVertical ),
    mInterpolationType( CurveFrame::interpolationTypeBezier ) {

    /* modify the frame's look */
    setFrameStyle( QFrame::Box | QFrame::Plain );

    /* set initial values */
    mSelectedHandle = -1;
    mXs.push_back( 0 );
    mXs.push_back( 0.5 );
    mXs.push_back( 1 );
    mYs.push_back( 1.0 );
    mYs.push_back( 0.5 );
    mYs.push_back( 0.0 );

    mHistogram = QVector<float>( HISTOGRAM_SIZE, 0.3 );
    mOutHistogram = QVector<float>( HISTOGRAM_SIZE, 0.3 );

    // update();
}

void CurveFrame::resetHandles() {
    mXs.clear();
    mYs.clear();

    /* set initial values */
    mSelectedHandle = -1;
    mXs.push_back( 0 );
    mXs.push_back( 1 );
    mYs.push_back( 0.5 );
    mYs.push_back( 0.5 );

    update();
}


//! \brief Remove the active handle, if there are more than two handles left
//! \see bool CurveFrameEventFilter::eventFilter( QObject *obj, QEvent *event )
void CurveFrame::removeActiveHandle() {
    /* checks */
    if( mSelectedHandle < 1 ) {
        return;
    }

    if( !( mXs.size() > 2 ) ) {
        return;
    }

    if( !( mYs.size() > 2 ) ) {
        return;
    }

    /* removal */
    if( mSelectedHandle < mXs.size() - 1 && mSelectedHandle < mYs.size() - 1 ) {
        mXs.remove( mSelectedHandle );
        mYs.remove( mSelectedHandle );
        mSelectedHandle--;

        emit signalMoved( mXs[mSelectedHandle], mYs[mSelectedHandle] );
        emit signalStep();
        update();
    }
}

//! \return True, if the selected handle is not -1
bool CurveFrame::isActive() const {
    return mSelectedHandle != -1;
}

//! \return min < var < max
float CurveFrame::clamp( float var, float min, float max ) const {
    if( var < min ) {
        return min;
    }

    if( var > max ) {
        return max;
    } else {
        return var;
    }
}

void CurveFrame::mouseMoveEvent( QMouseEvent* event ) {
    /* checks */
    if( !mousePressed ) {
        return;
    }

    if( !event ) {
        qWarning() << "ERROR: Invalid event";
        return;
    }

    /* env values */
    QRect rect = contentsRect();
    rect -= QMargins( CurveFrame::mMargin, CurveFrame::mMargin, CurveFrame::mMargin, CurveFrame::mMargin );
    float x = float( event->pos().x() - CurveFrame::mMargin ) / rect.width();
    float y = float( event->pos().y() - CurveFrame::mMargin ) / rect.height();

    /* set new position for handle */
    if( mSelectedHandle != -1 ) {
        mYs[mSelectedHandle] = clamp( y, 0, 1 );

        /* do not move first or last handle in x direction */
        if( mSelectedHandle != 0 && mSelectedHandle != ( mXs.size() - 1 ) ) {
            float dxPre  = x - mXs[mSelectedHandle - 1];
            float dxPost = mXs[mSelectedHandle + 1] - x;

            float epsilon = 0.02;

            if( dxPre > epsilon && dxPost > epsilon ) {
                mXs[mSelectedHandle] = x;
            }
        }
    }

    /* tell others where we are*/
    emit signalMoved( mXs[mSelectedHandle], mYs[mSelectedHandle] );

    /* yeah */
    update();
}

void CurveFrame::mouseReleaseEvent( QMouseEvent* event ) {
    Q_UNUSED( event )
    mousePressed = false;

    /* tell others where we are*/
    emit signalMoved( mXs[mSelectedHandle], mYs[mSelectedHandle] );
    emit signalStep();
}

//! \brief Sets the selected handle to -1 and refreshs the window
void CurveFrame::slotSetInactive() {
    mSelectedHandle = -1;
    update();
}

void CurveFrame::mousePressEvent( QMouseEvent* event ) {
    mousePressed = true;

    QRect rect = contentsRect();
    rect -= QMargins( CurveFrame::mMargin, CurveFrame::mMargin, CurveFrame::mMargin, CurveFrame::mMargin );
    float x = float( event->pos().x() - CurveFrame::mMargin ) / rect.width();
    float y = float( event->pos().y() - CurveFrame::mMargin ) / rect.height();

    float epsilon = 0.02;
    float dxPre   = 0.0;
    float dxPost  = 0.0;

    /* choose handle */
    size_t size = mXs.size();

    for( size_t i = 0; i < size - 1; ++i ) {
        dxPre  = fabs( x - mXs[i] );
        dxPost = fabs( mXs[i + 1] - x );

        // qDebug() << "dxPre: " << dxPre;
        // qDebug() << "dxPost: " << dxPost;

        if( dxPre < epsilon || dxPost < epsilon ) {
            if( dxPost < dxPre ) {
                mSelectedHandle = i + 1;
            } else {
                mSelectedHandle = i;
            }

            /* tell others */
            emit signalPressed();

            update();
            return;
        }
    }

    /* else add handle */
    for( size_t i = 0; i < size; ++i ) {
        if( mXs[i] > x ) {
            {
                mXs.insert( mXs.begin() + i, x );
                mYs.insert( mYs.begin() + i, y );
            }
            mSelectedHandle = i;

            /* tell others */
            emit signalPressed();

            update();
            return;
        }
    }
}

void CurveFrame::slotSetInterpolation( CurveFrame::interpolationType type ) {
    if( mInterpolationType != type ) {
        mInterpolationType = type;
        update();
    }
}

//! \brief Returns the output histogram
//! \param count The histogram width
QVector<float> CurveFrame::getOutHistogram( ) {
    /* checks */
    if( mSpline.size() != HISTOGRAM_SIZE ) {
        return mHistogram;
    }

    /* reset */
    mOutHistogram.fill( 0 );

    /* insert new values */
    for( int i = 0; i < HISTOGRAM_SIZE; i++ ) {
        float scaling = mSpline[i] / ( ( float )i / HISTOGRAM_SIZE );

        if( scaling * i < mHistogram.size() ) {
            mOutHistogram[scaling * i] += mHistogram[i];
        }
    }

    return mOutHistogram;
}

//! \brief Returns the spline points
//! \param count The count of fourier parameters, e.g. half width or height
QVector<float> CurveFrame::getSpline( int count ) {
    QVector<float> fulcrums;
    fulcrums.reserve( count );

    if( mXs.size() == 2 || mInterpolationType == CurveFrame::interpolationTypeNone ) {
        /* linear interpolation */
        for( int i = 0; i < count; i++ ) {
            float x = ( float )( i ) / count;
            int lower  = 0;
            int higher = 1;

            while( x > mXs[higher] ) {
                lower ++;
                higher++;
            }

            float part = ( x - mXs[lower] ) / ( mXs[higher] - mXs[lower] );
            float y = 1.0 - 1.0 * ( mYs[higher] * part + mYs[lower] * ( 1 - part ) );
            y = clamp( y, 0.0, 1.0 );
            fulcrums.push_back( y );
        }
    } else {
        /* bezier */
        QVector<QPointF> points = blacksilk::calcBezier( mXs, mYs, count );

        foreach( QPointF point, points ) {
            float y = 1.0 - point.y();
            y = clamp( y, 0.0, 1.0 );
            fulcrums.push_back( y );
        }
    }

    mSpline = fulcrums;

    return mSpline;
}

//! \brief Draw curves and handles
void CurveFrame::drawContents( QPainter* painter ) {
    /* check */
    if( !painter ) {
        qWarning() << "Invalid painter";
        return;
    }

    /* move painter down right while painting handles */
    painter->translate( CurveFrame::mMargin, CurveFrame::mMargin );
    painter_guard guard( ( int* )1, [ = ]( int* ) {
        painter->translate( -CurveFrame::mMargin, -CurveFrame::mMargin );
    } );

    /* env corrected by margin*/
    QRect r = contentsRect();
    r -= QMargins( CurveFrame::mMargin, CurveFrame::mMargin, CurveFrame::mMargin, CurveFrame::mMargin );

    Qt::GlobalColor color;

    if( isEnabled() ) {
        color = Qt::black;
    } else {
        color = Qt::gray;
    }

    painter->setPen( QPen( color, 1 ) );

    if( !( mInterpolationType == CurveFrame::interpolationTypeBezier ) ) {
        /* draw lines */
        for( int i = 1; i < mXs.size(); i++ ) {
            painter->drawLine( r.width() * mXs[i - 1], r.height() * mYs[i - 1], r.width() * mXs[i], r.height() * mYs[i] );
        }
    } else {
        QVector< blacksilk::Quadruple<QPointF> > controls = blacksilk::calcControlPoints( mXs, mYs );

        if( ! controls.empty() ) {

            QPainterPath path( QPointF( controls[0].A.x() * r.width(), controls[0].A.y() * r.height() ) );

            for( int i = 0; i < controls.size() - 1 ; ++i ) {
                /*
                painter->setPen( QPen( Qt::green, 8 ) );

                painter->drawPoint( r.width() * controls[i].B.x(), r.height() * controls[i].B.y() );
                painter->drawPoint( r.width() * controls[i].C.x(), r.height() * controls[i].C.y() );
                */

                path.cubicTo( controls[i].B.x() * r.width(), controls[i].B.y() * r.height(),
                              controls[i].C.x() * r.width(), controls[i].C.y() * r.height(),
                              controls[i].D.x() * r.width(), controls[i].D.y() * r.height() );
            }

            painter->setPen( QPen( Qt::black, 1 ) );
            painter->drawPath( path );
        } else {
            painter->drawLine( mXs[0] * r.width(), mYs[0] * r.height(),
                               mXs[1] * r.width(), mYs[1] * r.height() );
        }

        /*
        QVector<QPointF> points = calcBezier( mXs, mYs, 255 );
        painter->setPen( QPen( Qt::black, 2 ) );

        int i = 0;
        foreach( QPointF point, points ) {
            painter->drawPoint( r.width() * (float)i/255.0, r.height() * point.y() );
            // painter->drawPoint( r.width() * point.x(), r.height() * point.y() );
            i++;
        }
        */

    }

    /* draw handles */
    QPen pen( color );
    pen.setCapStyle( Qt::RoundCap );

    for( int i = 0; i < mXs.size(); i++ ) {
        pen.setWidth( 10 );
        pen.setColor( color );
        painter->setPen( pen );
        painter->drawPoint( r.width() * mXs[i], r.height() * mYs[i] );
        pen.setWidth( 8 );
        pen.setColor( Qt::red );
        painter->setPen( pen );
        painter->drawPoint( r.width() * mXs[i], r.height() * mYs[i] );
    }

    /* draw selected handle */
    if( mSelectedHandle != -1 ) {
        pen.setWidth( 10 );
        pen.setColor( color );
        painter->setPen( pen );
        painter->drawPoint( r.width() * mXs[mSelectedHandle], r.height() * mYs[mSelectedHandle] );
        pen.setWidth( 8 );
        pen.setColor( Qt::yellow );
        painter->setPen( pen );
        painter->drawPoint( r.width() * mXs[mSelectedHandle], r.height() * mYs[mSelectedHandle] );
    }
}

void CurveFrame::drawGrid( QPainter* painter ) {
    QRect r = contentsRect();
    r -= QMargins( CurveFrame::mMargin, CurveFrame::mMargin, CurveFrame::mMargin, CurveFrame::mMargin );
    painter->setPen( QPen( Qt::black, 1 ) );
    painter->drawRect( r );

    /* thinner inside */
    painter->setPen( QPen( Qt::black, 0.5 ) );

    /* vertical quarters */
    QRect vertical = r;
    vertical -= QMargins( r.width() / 4, 0, r.width() / 4, 0 );
    painter->drawLine( vertical.topLeft(), vertical.bottomLeft() );
    painter->drawLine( vertical.topRight(), vertical.bottomRight() );

    /* vertical center */
    vertical -= QMargins( r.width() / 4, 0, 0, 0 );
    painter->drawLine( vertical.topLeft(), vertical.bottomLeft() );

    /* horizontal quarters */
    QRect horizontal = r;
    horizontal -= QMargins( 0, r.height() / 4, 0, r.height() / 4 );
    painter->drawLine( horizontal.topLeft(), horizontal.topRight() );
    painter->drawLine( horizontal.bottomLeft(), horizontal.bottomRight() );

    /* horizontal center */
    horizontal -= QMargins( 0, r.height() / 4, 0, 0 );
    painter->drawLine( horizontal.topLeft(), horizontal.topRight() );
}

void CurveFrame::drawRoundEdges( QPainter* painter ) {
    QRectF rectangle( -1, -1, width() + 1, height() + 1 );
    painter->setPen( QPen( palette().window(), 5 ) );
    painter->drawRoundedRect( rectangle, 6.0, 6.0 );
}

void CurveFrame::drawBackground( QPainter* painter ) {
    QRadialGradient gradient;
    gradient.setCenter( width(), 0 );
    gradient.setFocalPoint( width(), 0 );
    gradient.setRadius( qMax( width(), height() ) );
    gradient.setSpread( QGradient::PadSpread );
    gradient.setColorAt( 1.0, QColor( 160, 160, 160 ) );
    gradient.setColorAt( 0.0, QColor( 230, 230, 230 ) );
    painter->fillRect( contentsRect(), gradient );
}

//! \brief Draw histogram
void CurveFrame::drawHistogram( QPainter* painter ) {
    /* check */
    if( !painter ) {
        qWarning() << "Invalid painter";
        return;
    }

    if( mHistogram.size() == 0 ) {
        qWarning() << "No histogram set";
        return;
    }

    /* env */
    QRect r = contentsRect();
    r -= QMargins( CurveFrame::mMargin, CurveFrame::mMargin, CurveFrame::mMargin, CurveFrame::mMargin );

    Qt::GlobalColor color;

    if( isEnabled() ) {
        color = Qt::darkGray;
    } else {
        color = Qt::gray;
    }

    /* draw lines */
    painter->setPen( QPen( color, 1 ) );

    float factor = ( float )r.width() / mHistogram.size();

#ifdef WIN32

    for( int i = 0; i < mHistogram.size(); i++ ) {
        int height = mHistogram[ i ] * r.height();
        QLine line( factor * i, r.height(), factor * i, r.height() - height );
        line.translate( CurveFrame::mMargin, CurveFrame::mMargin );
        painter->drawLine( line );
    }

#else
    QPainterPath path( r.bottomLeft() );

    for( int i = 0; i < mHistogram.size(); i++ ) {
        int height = fmin( mHistogram[ i ], 1.f ) * r.height();
        path.lineTo( CurveFrame::mMargin + factor * i, CurveFrame::mMargin + r.height() - height );
    }

    path.lineTo( r.bottomRight() );
    path.lineTo( r.bottomLeft() );
    painter->fillPath( path, QColor( 134, 144, 156 ) );
#endif // WIN32

}

void CurveFrame::paintEvent( QPaintEvent* event ) {
    Q_UNUSED( event );

    /* init painter */
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setClipRect( contentsRect() );

    /* render radial gradient */
    drawBackground( &painter );

    /* rounded edges, looks nicer */
    drawRoundEdges( &painter );

    /* draw histogram */
    drawHistogram( &painter );

    /* draw grid */
    drawGrid( &painter );

    /* draw curves and handles */
    drawContents( &painter );
}

//! \return The handle coordinates as two QVectors of same length
void CurveFrame::getCoordinates( QVector<float>& xsArg,  QVector<float>& ysArg ) const {
    xsArg = mXs;

    ysArg = mYs;

    // reverse direction
    for( QVector<float>::iterator it = ysArg.begin(); it != ysArg.end(); ++it ) {
        *it = 1.0 - *it;
    }
}

void CurveFrame::setCoordinates( const QVector<float>& xsArg, const QVector<float>& ysArg ) {
    mXs = xsArg;
    mYs = ysArg;

    /* checks */
    for( int i = 0; i < mXs.size(); i++ ) {
        mXs[i] = clamp( mXs[i], 0, 1 );
    }

    for( int i = 0; i < mYs.size(); i++ ) {
        mYs[i] = clamp( 1.0 - mYs[i], 0, 1 );
    }

    if( mXs.size() != mYs.size() ) {
        qWarning() << "xs.size() != ys.size()";
        mYs.resize( mXs.size() );
    }

    mSelectedHandle = -1;
    update();
}

//! \brief (Re-)Sets the histogram
void CurveFrame::setHistogram( const QVector< float >& values ) {
    if( values.size() != HISTOGRAM_SIZE ) {
        qWarning() << "curves: wrong histogram size";
        return;
    }

    mHistogram = values;
    update();
}

//! \brief Sets the frame type to horizontal or vertical
//! \param type frameTypeVertical or frameTypeHorizontal
void CurveFrame::setframeType( const frameType type ) {
    this->mFrameType = type;
}





