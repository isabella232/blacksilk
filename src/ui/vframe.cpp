#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

#include "vframe.h"

VFrame::VFrame( QWidget* parent ) :
    QFrame( parent ),
    mMousePressed( false ),
    mMousePosition( 0.5, 0.5 ),
    mRadius( 0.5 ),
    mStrength( 0.5 ) {
    /* make frame sensible for all events */
    setFocusPolicy( Qt::StrongFocus );

    m_WheelTimer.setSingleShot( true );
    connect( &m_WheelTimer, &QTimer::timeout, this, [ = ] { emit signalMouseReleased(); } );
}

void VFrame::slotSetRadius( float radius ) {
    /* checks */
    if( radius < 0.0 ) {
        return;
    }

    if( radius > 1.0 ) {
        return;
    }

    if( qFuzzyCompare( radius, mRadius ) ) {
        return;
    }

    mRadius = radius;
    emit signalRadius( radius );
    update();
}

void VFrame::slotSetStrength( float strength ) {
    /* checks */
    if( strength < -1.0 ) {
        return;
    }

    if( strength > 1.0 ) {
        return;
    }

    if( qFuzzyCompare( strength, mStrength ) ) {
        return;
    }

    mStrength = strength;
    emit signalStrength( strength );
    update();
}

void VFrame::slotSetX( float x ) {
    mMousePosition.setX( x );
    update();
}

void VFrame::slotSetY( float y ) {
    mMousePosition.setY( y );
    update();
}

void VFrame::paintEvent( QPaintEvent* ) {
    /* init painter */
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setClipRect( contentsRect() );

    /* render radial gradient */
    QRadialGradient gradient;
    gradient.setCenter( width() / 2, height() / 2 );
    gradient.setFocalPoint( width() / 2, height() / 2 );
    gradient.setRadius( qMax( width(), height() ) );
    gradient.setSpread( QGradient::PadSpread );

    gradient.setColorAt( 1.0, QColor( 160, 160, 160 ) );
    gradient.setColorAt( 0.0, QColor( 230, 230, 230 ) );

    // gradient.setColorAt( 0.0, QColor( 160, 160, 160 ) );
    painter.fillRect( contentsRect(), gradient );

    drawContents( &painter );

    // painter.setPen( QPen( Qt::gray, 1 ) );
    // painter.drawRect( contentsRect() );

    /* rounded edges, looks nicer */
    QRectF rectangle( -1, -1, width() + 1, height() + 1 );
    painter.setPen( QPen( palette().background(), 5 ) );
    painter.drawRoundedRect( rectangle, 6.0, 6.0 );
}

void VFrame::drawContents( QPainter* painter ) {
    Q_ASSERT( painter );

    /* env */
    QRect rect = contentsRect();
    QPointF center = QPointF( rect.width() * mMousePosition.x(), rect.height() * mMousePosition.y() );
    float radius = ( ( float )rect.width() + ( float )rect.height() ) / 2.0 * mRadius;

    painter->setPen( QPen( Qt::black, 10 ) );
    painter->drawPoint( center );

    if( mStrength < 0 ) {
        painter->setPen( QPen( Qt::cyan, -mStrength * 10 ) );
    } else {
        painter->setPen( QPen( Qt::yellow, mStrength * 10 ) );
    }

    painter->drawEllipse( center, radius, radius );
}

void VFrame::mousePressEvent( QMouseEvent* event ) {
    Q_ASSERT( event );

    mMousePressed = true;

    /* env values */
    QRect rect = contentsRect();
    mMousePosition.setX( float( event->pos().x() ) / rect.width() );
    mMousePosition.setY( float( event->pos().y() ) / rect.height() );

    emit signalPosition( mMousePosition );

    update();
}

void VFrame::mouseMoveEvent( QMouseEvent* event ) {
    Q_ASSERT( event );

    /* checks */
    if( !mMousePressed ) {
        return;
    }

    /* env values */
    QRect rect = contentsRect();
    mMousePosition.setX( float( event->pos().x() ) / rect.width() );
    mMousePosition.setY( float( event->pos().y() ) / rect.height() );

    emit signalPosition( mMousePosition );

    /* yeah */
    update();
}

void VFrame::mouseReleaseEvent( QMouseEvent* ) {
    emit signalMouseReleased();
    mMousePressed = false;
}

void VFrame::wheelEvent( QWheelEvent* event ) {
    if( event->modifiers().testFlag( Qt::ControlModifier ) ) {
        this->slotSetStrength( mStrength + 0.0001 * event->delta() );
    } else {
        this->slotSetRadius( mRadius + 0.0001 * event->delta() );
    }

    // after 1/2 sec of no more wheel usage an undo step is sent
    m_WheelTimer.start( 500 );
}
