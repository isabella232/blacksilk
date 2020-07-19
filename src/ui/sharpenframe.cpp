#include "SharpenFrame.h"


#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

using namespace blacksilk::graphics;

SharpenFrame::SharpenFrame( QWidget* parent ) :
    QFrame( parent ),
    mousePressed( false ),
    mColorSpace( EnumColorSpaceRGB ) {
    /* make frame sensible for all events */
    setFocusPolicy( Qt::StrongFocus );

    /* init */
    mYUV.y = 255;
    mYUV.u = 0;
    mYUV.v = 0;

    setColorSpace( EnumColorSpaceSuperRGB );

    /* paint background */
    renderBackground();
    emitAll();
}

QString SharpenFrame::toString() {
    ColorRGB<float> rgb;
    YUV2RGB16U( mYUV, rgb );

    QString s;
    QTextStream ts( &s );
    ts << "\n";
    ts << " ********* SharpenFrame ********* \n";
    ts << "    Y: " << mYUV.y <<  "\n";
    ts << "    U: " << mYUV.u <<  "\n";
    ts << "    V: " << mYUV.v <<  "\n";
    ts << "\n";
    ts << "    R: " << rgb.r <<  "\n";
    ts << "    G: " << rgb.g <<  "\n";
    ts << "    B: " << rgb.b <<  "\n";
    ts << "    mouseX: " << mousePos.x() <<  "\n";
    ts << "    mouseY: " << mousePos.y() <<  "\n";
    ts << " **************************** \n";

    return s;
}

//! \brief Adjust the boundaries of the input color space
//! \param a colorspace, e.g. SharpenFrame::EnumColorSpaceSuperRGB
void SharpenFrame::setColorSpace( EnumColorSpace space ) {
    mColorSpace = space;

    switch( space ) {
        case EnumColorSpaceRGB:
            Umin = -170;
            Umax = 170;
            Vmin = -170;
            Vmax = 170;
            break;

        case EnumColorSpaceSuperRGB:
            Umin = -341;
            Umax = 341;
            Vmin = -341;
            Vmax = 341;
            break;

        case EnumColorSpaceUltraRGB:
            Umin = -907;
            Umax = 907;
            Vmin = -718;
            Vmax = 718;
            break;

        default:
            qWarning() << "Could not identify color space";
    }
}

void SharpenFrame::slotSetRGB( ColorRGB<float> rgb ) {
    /* check */
    if( areEqual( rgb, mYUV ) ) {
        return;
    }

    RGB2YUV( rgb, mYUV );
    // qDebug() << "SET:" << qPrintable( this->toString() );

    mousePos.setX( ( mYUV.u - ( float )Umin ) / ( ( float )( Umax - Umin ) ) );
    mousePos.setY( 1.0 - ( mYUV.v - ( float )Vmin ) / ( ( float )( Vmax - Vmin ) ) );

    renderBackground();
    update();
}

void SharpenFrame::slotSetLuma( float luma ) {
    mYUV.y = blacksilk::graphics::CLAMP < float, -512, 512 > ( luma );
    renderBackground();
    update();
}

void SharpenFrame::resizeEvent( QResizeEvent* ) {
    update();
}

void SharpenFrame::wheelEvent( QWheelEvent* event ) {
    this->slotSetLuma( mYUV.y + 0.05 * ( event->delta() / 5 ) );
    emitAll();
}

void SharpenFrame::renderBackground() {
    background = QImage( Umax - Umin, Vmax - Vmin, QImage::Format_ARGB32 );
    uchar* bits = background.bits();
    YUV yuv = mYUV;
    BGR<uchar> bgr;

    for( int j = Vmin; j < Vmax; j++ ) {
        for( int i = Umin; i < Umax; i++ ) {
            yuv.u = ( float )( i );
            yuv.v = ( float )( j );

            YUV2BGR8( yuv, bgr );

            memcpy( bits, &bgr, 3 * sizeof( char ) );
            bits += 3;
            *bits = 255;
            bits++;
        }
    }

    /* switch from top down to down top axis */
    background = background.mirrored( false, true );
}

void SharpenFrame::paintEvent( QPaintEvent* ) {
    /* init painter */
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setClipRect( contentsRect() );

    drawContents( &painter );
}

void SharpenFrame::emitAll( ) {
    mYUV.u = mousePos.x() * ( Umax - Umin ) + Umin;
    /* y is negative for natural y-axis */
    mYUV.v = -( mousePos.y() * ( Vmax - Vmin ) + Vmin );

    ColorRGB<float> rgb;
    YUV2RGB16U( mYUV, rgb );

    emit signalWeights( rgb.r, rgb.g, rgb.b );
    // qDebug() << "EMIT: " << qPrintable( this->toString() );

    update();
}

void SharpenFrame::mousePressEvent( QMouseEvent* event ) {
    mousePressed = true;

    if( !event ) {
        qWarning() << "Invalid mouse event";
        return;
    }

    /* u=x v=y */
    mousePos.setX( event->posF().x() / rect().width() );
    mousePos.setY( event->posF().y() / rect().height() );
    emitAll();
}

void SharpenFrame::mouseReleaseEvent( QMouseEvent* event ) {
    Q_UNUSED( event );
    mousePressed = false;
}

void SharpenFrame::mouseMoveEvent( QMouseEvent* event ) {
    if( !mousePressed ) {
        return;
    }

    if( !event ) {
        qWarning() << "Invalid mouse event";
        return;
    }

    /* u=x v=y */
    mousePos = event->posF();
    mousePos.setX( mousePos.x() / rect().width() );
    mousePos.setY( mousePos.y() / rect().height() );

    emitAll();
}

void SharpenFrame::drawContents( QPainter* painter ) {
    /* check */
    if( background.isNull() ) {
        return;
    }

    if( !painter ) {
        qDebug() << "ERROR: Invalid painter";
        return;
    }

    /* scale and draw background image */
    QRect r = contentsRect();
    scale.setWidth( ( qreal )r.width() / ( Umax - Umin ) );
    scale.setHeight( ( qreal )r.height() / ( Vmax - Vmin ) );
    painter->drawImage( 0, 0, background.scaled( contentsRect().size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) );

    /* create hexagons */
    int i = 255;
    YUV yuv;
    QPolygonF innerHexagon;
    {
        // c
        RGB2YUV( ColorRGB<int> ( 0, i, i ), yuv );
        innerHexagon << QPointF( ( yuv.u - Umin )*scale.width(), ( Vmax - yuv.v )*scale.height() );
        // g
        RGB2YUV( ColorRGB<int> ( 0, i, 0 ), yuv );
        innerHexagon << QPointF( ( yuv.u - Umin )*scale.width(), ( Vmax -  yuv.v )*scale.height() );
        // y
        RGB2YUV( ColorRGB<int> ( i, i, 0 ), yuv );
        innerHexagon << QPointF( ( yuv.u - Umin )*scale.width(), ( Vmax -  yuv.v )*scale.height() );
        // r
        RGB2YUV( ColorRGB<int> ( i, 0, 0 ), yuv );
        innerHexagon << QPointF( ( yuv.u - Umin )*scale.width(), ( Vmax -  yuv.v )*scale.height() );
        // m
        RGB2YUV( ColorRGB<int> ( i, 0, i ), yuv );
        innerHexagon << QPointF( ( yuv.u - Umin )*scale.width(), ( Vmax -  yuv.v )*scale.height() );
        // b
        RGB2YUV( ColorRGB<int> ( 0, 0, i ), yuv );
        innerHexagon << QPointF( ( yuv.u - Umin )*scale.width(), ( Vmax -  yuv.v )*scale.height() );
    }
    // qDebug() << "u: " << yuv.u << "v: " << yuv.v ;
    // qDebug() << QPointF( ( yuv.u - Umin )*scale.width(), ( Vmax -  yuv.v )*scale.height() );

    QPolygonF outerHexagon;
    {
        // c
        RGB2YUV( ColorRGB<int> ( -i, i, i ), yuv );
        outerHexagon << QPointF( ( yuv.u - Umin )*scale.width(), ( Vmax -  yuv.v )*scale.height() );
        // g
        RGB2YUV( ColorRGB<int> ( -i, i, -i ), yuv );
        outerHexagon << QPointF( ( yuv.u - Umin )*scale.width(), ( Vmax -  yuv.v )*scale.height() );
        // y
        RGB2YUV( ColorRGB<int> ( i, i, -i ), yuv );
        outerHexagon << QPointF( ( yuv.u - Umin )*scale.width(), ( Vmax -  yuv.v )*scale.height() );
        // r
        RGB2YUV( ColorRGB<int> ( i, -i, -i ), yuv );
        outerHexagon << QPointF( ( yuv.u - Umin )*scale.width(), ( Vmax -  yuv.v )*scale.height() );
        // m
        RGB2YUV( ColorRGB<int> ( i, -i, i ), yuv );
        outerHexagon << QPointF( ( yuv.u - Umin )*scale.width(), ( Vmax -  yuv.v )*scale.height() );
        // b
        RGB2YUV( ColorRGB<int> ( -i, -i, i ), yuv );
        outerHexagon << QPointF( ( yuv.u - Umin )*scale.width(), ( Vmax -  yuv.v )*scale.height() );
    }


    /* if luma is lower than 75, set painter color to white */
    int composite = 255;
    int averageLuma = 84;

    if( mYUV.y > averageLuma ) {
        composite = 0;
    }

    /* draw hexagons */
    painter->setPen( QPen( QColor( composite, composite, composite ), 1 ) );
    painter->drawPolygon( innerHexagon );
    painter->drawPolygon( outerHexagon );

    /* draw center lines */
    painter->setPen( QPen( QColor( composite, composite, composite ), 0.5 ) );
    painter->drawLine( r.width() / 2, 0, r.width() / 2, r.height() );
    painter->drawLine( 0, r.height() / 2, r.width(), r.height() / 2 );

    /* draw cursor */
    {
        painter->setPen( QPen( QColor( composite, composite, composite ), 2 ) );
        int size = 5;
        int x = mousePos.x() * r.width();
        int y = mousePos.y() * r.height();

        painter->drawLine( x, y - size, x, y + size );
        painter->drawLine( x - size, y, x + size, y );
    }
}

