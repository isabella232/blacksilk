#include <QtCore/QString>
#include <QtTest/QtTest>

#include "utils/colorspaces.hpp"
#include "helpers.h"

using namespace blacksilk::graphics;

class TestColorSpaces : public QObject
{
    Q_OBJECT

public:
    TestColorSpaces(){}

private Q_SLOTS:
    void testYUV2RGB8();
    void testYUV2BGR8();
    void testYUV2RGB16();
    void testYUV2RGB32();
    void testYUV2RGB16U();
    void testColors();
};


void TestColorSpaces::testYUV2RGB8()
{
    ColorRGB<char> rgb(10,10,8);
    ColorRGB<char> rgb2(3,3,3);
    YUV yuv;
    RGB2YUV(rgb,yuv);
    YUV2RGB8(yuv,rgb2);
    QVERIFY2( areEqual<char>( rgb, rgb2 ), "Error: YUV2RGB8() is broken!" );
}

void TestColorSpaces::testYUV2BGR8()
{
    BGR<char> bgr(10,10,10);
    BGR<char> bgr2(3,3,3);
    YUV yuv;
    BGR2YUV(bgr,yuv);
    YUV2BGR8(yuv,bgr2);
    QVERIFY2( areEqual<char>( bgr, bgr2 ), "Error: YUV2RGB8() is broken!" );
}

void TestColorSpaces::testYUV2RGB16()
{
    ColorRGB<ushort> rgb(10,10,10);
    ColorRGB<ushort> rgb2(3,3,3);
    YUV yuv;
    RGB2YUV(rgb,yuv);
    YUV2RGB16(yuv,rgb2);
    QVERIFY2( areEqual<ushort>( rgb, rgb2 ), "Error: YUV2RGB16() is broken!" );
}

void TestColorSpaces::testYUV2RGB32()
{
    ColorRGB<float> rgb(0.5,0.5,0.5);
    ColorRGB<float> rgb2(0.1,0.1,0.1);
    YUV yuv;
    RGB2YUV(rgb,yuv);
    YUV2RGB32(yuv,rgb2);
    QVERIFY2( areEqual<float>( rgb, rgb2 ), "Error: YUV2RGB32() is broken!" );
}

void TestColorSpaces::testColors()
{
    ColorRGB<int> rgb(255,0.0,0.0);
    ColorRGB<int> rgb2(0,0,0);
    YUV yuv;
    RGB2YUV(rgb,yuv);
    YUV2RGB8(yuv,rgb2);
    QVERIFY2( areEqual<int>( rgb, rgb2 ), "Error: Red is broken!" );
}

void TestColorSpaces::testYUV2RGB16U()
{
    ColorRGB<float> rgb(10,5,-5);
    ColorRGB<float> rgb2(1,4,3);
    YUV yuv;
    RGB2YUV(rgb,yuv);
    YUV2RGB16U<float>(yuv,rgb2);
    QVERIFY2( areEqual<float>( rgb, rgb2 ), "Error: YUV2RGB16U() is broken!" );
}

QTEST_MAIN( TestColorSpaces )

#include "testColorSpaces.moc"
