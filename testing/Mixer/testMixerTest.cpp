#include <QString>
#include <QtTest>
#include <QApplication>
#include <QTimer>

#include <utils/app.hpp>
#include <ui/mixerwidget.hpp>

class TestMixer : public QObject {
    Q_OBJECT

public:
    TestMixer();

protected slots:
    void resetMixer();

private Q_SLOTS:
    void testPresets();

private:
    MixerWidget* m_Mixer;
};

TestMixer::TestMixer() {
}

void TestMixer::resetMixer() {
    m_Mixer->reset();
}

void TestMixer::testPresets() {

    int argc = 0;
    char** argv = {};
    QApplication app( argc, argv );

    blacksilk::initializeApplication();
    auto session = blacksilk::theApp()->currentSession;
    std::tuple<libgraphics::Filter *, libgraphics::FilterPresetCollection> filterAndPresets = session->getFilterAndPresetsbyFilterType( MixerWidget::filterType );
    m_Mixer = new MixerWidget( filterAndPresets );

    int i = 0;

    connect( m_Mixer, &MixerWidget::signalMixer, [&i] { ++i; } );
    QTimer::singleShot( 500, this, SLOT( resetMixer() ) );
    QTimer::singleShot( 1000, qApp, SLOT( quit() ) );

    m_Mixer->show();
    app.exec();

    QVERIFY( i > 0 );
}

QTEST_APPLESS_MAIN( TestMixer )

#include "testMixerTest.moc"
