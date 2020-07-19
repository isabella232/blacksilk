#include <QString>
#include <QtTest>
#include <QMetaType>

#include "ui/yuvframe.h"
#include "helpers.h"

class TestYUVFrame : public QObject {
    Q_OBJECT

    typedef blacksilk::graphics::ColorRGB<float> ColorRGBf;

private:
    ColorRGBf mRGB;

public:
    TestYUVFrame();

public slots:
    void slotSetRGB( float r, float g, float b );

private Q_SLOTS:
    void testClick();
    void testClick_data();
};


TestYUVFrame::TestYUVFrame() {
}

void TestYUVFrame::slotSetRGB( float r, float g, float b ) {
    mRGB = ColorRGBf( r, g, b );
}

void TestYUVFrame::testClick_data() {
    QTest::addColumn<float>( "x" );
    QTest::addColumn<float>( "y" );
    QTest::addColumn<float>( "r" );
    QTest::addColumn<float>( "g" );
    QTest::addColumn<float>( "b" );

    QTest::newRow( "center" ) << 0.5f << 0.5f << 85.f << 85.f << 85.f;
    QTest::newRow( "superred" ) << 0.25f << 0.0f << 425.f << -85.f << -85.f;
    QTest::newRow( "red" ) << 0.375f << 0.25f << 255.f << 0.f << 0.f;
}

void TestYUVFrame::testClick() {

    QFETCH( float, x );
    QFETCH( float, y );
    QFETCH( float, r );
    QFETCH( float, g );
    QFETCH( float, b );
    ColorRGBf expected( r, g, b );

    YUVFrame frame;
    frame.resize( 1024, 1024 );
    connect( &frame, SIGNAL( signalWeights( float, float, float ) ), this, SLOT( slotSetRGB( float, float, float ) ) );

    mRGB = ColorRGBf( -1, -1, -1 );
    ColorRGBf rgb( 1, 1, 1 );
    frame.slotSetRGB( rgb );
    frame.slotSetLuma( 85 );

    QSize size = frame.contentsRect().size();
    QPoint center( x * size.width(), y * size.height() );
    QTest::mouseClick( &frame, Qt::LeftButton, 0, center );

    QVERIFY( areEqual( expected , mRGB ) );
}

QTEST_MAIN( TestYUVFrame )

#include "testYUVFrameTest.moc"
