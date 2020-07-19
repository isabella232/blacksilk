#include <QString>
#include <QtTest>

#include <iostream>

#include <utils/app.hpp>
#include <utils/offscreencontext.hpp>
#include <libcommon/fileutils.hpp>
#include <log/log.hpp>

#include "cat.h"

static std::unique_ptr<blacksilk::OffscreenContext>     offscreenCtx;
static bool                                             useGpuRendering = false;

// from main.cpp
void initializeCommandLine() {

#if __APPLE__
    // hide if run as CLI app
    hideFromDock();
#endif // __APPLE__

    if ( offscreenCtx.get() == nullptr ) {
        offscreenCtx.reset( new blacksilk::OffscreenContext() );
        if ( offscreenCtx->valid() ) {
            useGpuRendering = true;
        } else {
            useGpuRendering = false;
        }
    }

    if ( useGpuRendering ) {
        const auto successfullyInitializedGpuBackend = blacksilk::theApp()->initializeGraphicsPreview();
        if ( !successfullyInitializedGpuBackend ) {
            qDebug() << "Failed to initialize gpu graphics backend. Falling back to cpu-rendering.";
            useGpuRendering = false;
        }

        assert(successfullyInitializedGpuBackend);
    }
}

void shutdownCommandLine() {
    if ( blacksilk::theApp()->initialized() ) {
        const auto successfullyShutdown = blacksilk::theApp()->shutdown();
        if ( !successfullyShutdown ) {
            assert(false);
            qDebug() << "Error: Failed to shutdown program. Exiting.";
            exit(-1);
        }

        offscreenCtx.reset();
    }
}

class TestImageMagick : public QObject {
        Q_OBJECT

    public:
        TestImageMagick();

    private Q_SLOTS:
        void testIO();
};

TestImageMagick::TestImageMagick() {
}

void TestImageMagick::testIO() {
    qInstallMessageHandler( logging::customMessageHandler );

    LOG( "Start" );

    std::string filename( "cat.jpg" );
    std::string filename2( "cat2.jpg" );

    std::string data( ( const char* ) cat_jpg, sizeof( cat_jpg ) );
    libcommon::fileutils::toFile( filename, data );

    QVERIFY( blacksilk::theApp() != nullptr );
    QVERIFY( blacksilk::theApp()->initialize( libfoundation::app::ApplicationConfig() ) );
    QVERIFY( blacksilk::theApp()->initializeGraphicsBackend() );
    initializeCommandLine();

    QVERIFY( blacksilk::theApp()->openImage( filename ) );

    QElapsedTimer t;
    t.start();

    QVERIFY( blacksilk::theApp()->currentSession->exportImage(
                 filename2,
                 libfoundation::app::EImageFormat::JPEG,
                 false,
                 nullptr,
                 !useGpuRendering
                 )
             );
    // QVERIFY( blacksilk::theApp()->saveImage( libfoundation::app::EImageFormat::JPEG, filename2 ) );

    const auto elapsed = t.elapsed();
    qDebug() << "Export finished after" << ( QString::number(elapsed) + "ms" );
    shutdownCommandLine();
}


QTEST_MAIN( TestImageMagick )

#include "testImageMagick.moc"
