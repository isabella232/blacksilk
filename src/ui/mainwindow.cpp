// Qt
#include <QToolBar>
#include <QStackedLayout>
#include <QDesktopServices>
#include <QMessageBox>
#include <QToolButton>
#include <QPushButton>
#include <QtConcurrent/QtConcurrent>
#include <QElapsedTimer>
#include <QStyleFactory>

#include <libgraphics/backend/cpu/cpu_imageobject.hpp>

// local
#include "mainwindow.h"
#include "mixerwidget.hpp"
#include "curveswidget.hpp"
#include "sharpenwidget.hpp"
#include "vignettewidget.hpp"
#include "grainwidget.hpp"
#include "splittonewidget.hpp"
#include "finishgroup.h"
#include "about.h"
#include "ui/pointlessbar.hpp"
#include <utils/app.hpp>
#include <log/log.hpp>
#include <libgraphics/debug.hpp>
#include <ui/actionundocommand.hpp>
#include <ui/presetundocommand.hpp>

#ifdef WIN32
#include "ui/win/wineventfilter.hpp"
#endif // WIN32

// auto
#include "ui_mainwindow.h"

// STL
#include <algorithm>
#include <sstream>


#ifdef _WIN32
// https://gist.github.com/statico/6809850727c708f08458
// Use discrete GPU by default.
extern "C" {
    // http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
    __declspec( dllexport ) DWORD NvOptimusEnablement = 0x00000001;

    // http://developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
    __declspec( dllexport ) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif


using namespace blacksilk::ui;

//! \param  b Extern argument from DoUI(); \n
//!         Is set to true by mainwindow, if user presses OK \n
//!         Is set to false by mainwindow, if input image size is too big
//! \sa     Boolean DoUI (void)
MainWindow::MainWindow( libfoundation::app::ApplicationSession* session, bool* b, bool* s, QWidget* parent ) :
    QMainWindow( parent ),
    run( b ),
    display( s ),
    mMixerWidget( new MixerWidget( session->getFilterAndPresetsbyFilterType( MixerWidget::filterType ), this ) ),
    mVignetteWidget( new VignetteWidget( session->getFilterAndPresetsbyFilterType( VignetteWidget::filterType ), this ) ),
    mSplitToneWidget( new SplitToneWidget( session->getFilterAndPresetsbyFilterType( SplitToneWidget::filterType ), this ) ),
    mGrainWidget( new GrainWidget( session->getFilterAndPresetsbyFilterType( GrainWidget::filterType ), this ) ),
    mSharpenWidget( new SharpenWidget( session->getFilterAndPresetsbyFilterType( SharpenWidget::filterType ), this ) ),
    mCurvesWidget( new CurvesWidget( session->getFilterAndPresetsbyFilterType( CurvesWidget::filterType ), this ) ),

    mStackedLayout( new QStackedLayout( this->centralWidget() ) ),
    mToolbarFilters( new QToolBar( this ) ),

    m_Resetting( false ), // for undo
    m_UndoAction( m_UndoStack.createUndoAction( this ) ),
    m_RedoAction( m_UndoStack.createRedoAction( this ) ),

    ui( new Ui::MainWindow ) {
    if( !theApp()->initialized() ) {
        initializeApplication();
    }


    qDebug() << "Qt library paths: " << qApp->libraryPaths();

    CHECK_QT_CONNECT( connect( qApp, &QApplication::lastWindowClosed, this, &MainWindow::on_lastWindowClosed ) );

    ui->setupUi( this );

#ifdef WIN32
    qApp->installNativeEventFilter( new WinEventFilter );
#endif // WIN32

#ifndef BLACKSILK_STANDALONE
    this->setWindowModality( Qt::ApplicationModal );
    this->ui->menubar->setNativeMenuBar( false );
    this->ui->menubar->hide();
#endif
    this->setContextMenuPolicy( Qt::NoContextMenu );

    // setupDarkStyle();
    setupRecentImagesMenu();
    setupRecentPresetsMenu();
    setupFilterToolBar();
    setupFilterMenu();
    setupViewMenu();
    setupPointlessBar();
#ifndef BLACKSILK_STANDALONE
    mToolbarFinish = new QToolBar( this );
    setupFinishToolBar();
#endif
    initOperationWidgets();
    initUndoMenu();

#ifndef BLACKSILK_STANDALONE
    {
        /* init buffer */
        *display = true;
    }
#endif // BLACKSILK_STANDALONE
}

//! \brief Initializes the recent images menu
void MainWindow::setupRecentImagesMenu( ) {
    mFileDealer.setType( FileDealer::ImagesType );
    QMenu* recent = new QMenu( tr( "Recent" ), this->ui->menuFile );
    mFileDealer.setupRecentMenu( recent );

    CHECK_QT_CONNECT( connect( &mFileDealer, &FileDealer::signalOpen, this, &MainWindow::openImage ) );
    this->ui->menuFile->insertMenu( ui->actionSave, recent );
    this->ui->menuFile->insertSeparator( ui->actionSave );
}

//! \brief Initializes the recent presets menu
void MainWindow::setupRecentPresetsMenu( ) {
    mPresetsDealer.setType( FileDealer::PresetsType );
    QMenu* recent = new QMenu( tr( "Recent" ), this->ui->menuPresets );
    mPresetsDealer.setupRecentMenu( recent );

    CHECK_QT_CONNECT( connect( &mPresetsDealer, &FileDealer::signalOpen, this, &MainWindow::openPreset ) );
    this->ui->menuPresets->insertMenu( ui->actionPresetsSave_As, recent );
    this->ui->menuPresets->insertSeparator( ui->actionPresetsSave_As );
}

//! \brief Opening a QFileDialog in the last opened directory
//! \see void MainWindow::openImage( const QString& filename )
void MainWindow::on_actionOpen_triggered() {

    QString filename;
    bool ok;
    std::tie( ok, filename ) = mFileDealer.getOpenFilename();

    if( ok ) {
        openImage( filename );
    }

    /* regain focus */
    this->activateWindow();
}

void MainWindow::openSample() {
    QFile image( ":/Samples/Icons/sample.jpg" );

#ifdef __APPLE__
    QString cache = QDir::homePath() + "/Library/Caches/com.fd-imaging.blacksilk";
    QDir().mkpath( cache );
    QString filename = cache + "/Welcome.jpeg";
#else
    QString filename = QDir::tempPath() + "/Welcome.jpeg";
    m_SampleFilename = filename;
#endif

    // remove old
    QFile target( filename );

    if( target.exists() ) {
        target.setPermissions( QFileDevice::ReadOwner | QFileDevice::WriteOwner );
        target.remove();
    }

    // copy new
    bool ok = image.copy( filename );

    if( ok ) {
        openImage( filename );
    } else {
        qWarning() << "Could not copy to" << filename;
    }
}

//! \brief Opening an image
//! \param The filename
//! \see void MainWindow::on_actionOpen_triggered()
//!
void MainWindow::updateAllocatorsForNewImage() {
    static const size_t numberOfPreallocations = 12;
    const auto currentImage = theApp()->currentSession->originalImage();
    const auto defaultSize  = currentImage->topLayer()->byteSize();

    const auto unusedObjects = theApp()->appBackend->allocator()->releaseUnused();
    qDebug() << "Number of freed objects: " << unusedObjects;

    theApp()->appBackend->allocator()->reserve(
        numberOfPreallocations,
        defaultSize
    );
    qDebug() << "Preallocated" << numberOfPreallocations << " of" << ( defaultSize / ( 1024 * 1024 ) ) << "MB chunks( total: " << ( ( defaultSize * numberOfPreallocations ) / ( 1024 * 1024 ) ) << "MB).";

    qDebug() << "Finished updating allocators...";
}

void MainWindow::openImage( const QString& filename ) {
#ifdef BLACKSILK_STANDALONE

    if( !filename.isEmpty() ) {

        if( !theApp()->canImport() ) {
            QMessageBox::information(
                this,
                "Import currently not possible",
                "Import is currently not possible. Please wait for the currently running import or export process"
                "to finish.",
                QMessageBox::Ok
            );
            qDebug() << "User requested import denied: Currently not able to import image.";
            return;
        }

        theApp()->state = App::EState::Importing;

        // first wait for the old background-tasks to
        // finish.
        ui->statusbar->showMessage( "Waiting for background tasks to complete..." );
        qApp->processEvents();
        theApp()->backgroundTasks->waitForAll();

        // show pointless bar
        emit signalOccupied();

        ui->statusbar->showMessage( "Loading image" );
        qApp->processEvents();

        CHECK_QT_CONNECT( connect( &mImageWatcher, &QFutureWatcher<bool>::finished, this, &MainWindow::slotImageImportFinished, Qt::UniqueConnection ) );
        mImageWatcher.setFuture( QtConcurrent::run( [ = ]() {
            bool ok = theApp()->openImage( filename.toStdString() );
            assert( ok );
            return ok;
        } ) );
    }

#else
    Q_UNUSED( filename )
#endif
}

void MainWindow::onUpdateAllValues() {
    ( void ) this->mSplitToneWidget->onUpdateValues();

    if( theApp()->currentSession->isFilterEnabled( libfoundation::app::EFilter::SplitToneFilter ) ) {
        if( !this->ui->actionSplitTone->isChecked() ) {
            emit this->on_actionSplitTone_triggered();
        }
    }

    ( void ) this->mCurvesWidget->onUpdateValues();

    if( theApp()->currentSession->isFilterSelectedForRendering( libfoundation::app::EFilter::CurvesFilter ) ) {
        if( !this->ui->actionCurves->isChecked() ) {
            emit this->on_actionCurves_triggered();
        }
    }

    ( void ) this->mVignetteWidget->onUpdateValues();

    if( theApp()->currentSession->isFilterSelectedForRendering( libfoundation::app::EFilter::VignetteFilter ) ) {
        if( !this->ui->actionVignette->isChecked() ) {
            emit this->on_actionVignette_triggered();
        }
    }

    ( void ) this->mGrainWidget->onUpdateValues();

    if( theApp()->currentSession->isFilterSelectedForRendering( libfoundation::app::EFilter::FilmGrainFilter ) ) {
        if( !this->ui->actionGrain->isChecked() ) {
            emit this->on_actionGrain_triggered();
        }
    }

    ( void ) this->mSharpenWidget->onUpdateValues();

    if( theApp()->currentSession->isFilterSelectedForRendering( libfoundation::app::EFilter::CascadedSharpenFilter ) ) {
        if( !this->ui->actionSharpen->isChecked() ) {
            emit this->on_actionSharpen_triggered();
        }
    }

    ( void ) this->mMixerWidget->onUpdateValues();

    if( theApp()->currentSession->isFilterSelectedForRendering( libfoundation::app::EFilter::BlackAndWhiteAdaptiveMixerFilter ) ) {
        if( !this->ui->actionMixer->isChecked() ) {
            emit this->on_actionMixer_triggered();
        }
    }
}

void MainWindow::slotAddUndoStep( FilterUndoCommand* command ) {
    LOG( command->newStateAsString() );
    m_UndoStack.push( command );
}

void MainWindow::openPreset( const QString& filename ) {
    occupy_guard guard( ( int* )1, [&]( int* ) {
        emit signalUnoccupied();
    } );
    emit signalOccupied();

    if( filename.isEmpty() ) {
        qWarning() << "Empty filename";
        return;
    }

    libgraphics::FilterPresetCollection oldState = theApp()->currentSession->currentActiveStateToPresetCollection();
    {
        const auto successfullyLoadPreset = theApp()->loadPresetFromPath(
                                                filename.toStdString()
                                            );
        assert( successfullyLoadPreset );

        if( successfullyLoadPreset ) {
            this->onUpdateAllValues();

            mStackedLayout->setCurrentWidget( mMixerWidget );
            this->update();
        }
    }
    libgraphics::FilterPresetCollection newState = theApp()->currentSession->currentActiveStateToPresetCollection();

    PresetUndoCommand* undoCommand = new PresetUndoCommand( [ = ]( libgraphics::FilterPresetCollection preset ) {
        theApp()->usePresets( preset );
        this->onUpdateAllValues();
        this->update();
    }, oldState, newState );

    undoCommand->setText( tr( "Open preset" ) );
    LOG( undoCommand->newStateAsString() );
    m_UndoStack.push( undoCommand );
}

void MainWindow::on_actionSave_triggered() {
    QString filename = mFileDealer.getFilename();
    QFileInfo info( filename );

    if( info.exists() ) {
        QString question = tr( "File " ) + info.baseName() + tr( " already exists.\n Do you want to replace it?" );
        QMessageBox::StandardButton button = QMessageBox::question( 0, tr( "Overwrite?" ), question, QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok );

        if( button == QMessageBox::Ok ) {
            this->saveImage( filename );
        }
    }

    /* regain focus */
    this->activateWindow();
}

void MainWindow::on_actionSave_As_triggered() {
    auto filename_ret = mFileDealer.getSaveFilename();

    if( filename_ret.first && ( !filename_ret.second.isEmpty() ) ) {
        this->saveImage( filename_ret.second );
    }

    /* regain focus */
    this->activateWindow();
}

QVector< ColorRGB<float> > MainWindow::calculateHistogram( void* buffer, int size, bool is16Bit, bool hasAlpha ) {
    Q_ASSERT( buffer );

    int colors = 3;

    if( hasAlpha ) {
        colors = 4;
    }

    int stepwidth = size / 25600;
    stepwidth = std::max( stepwidth, 1 );
    stepwidth = std::min( stepwidth, 4096 );
    int scale = stepwidth * 60;

    QVector< ColorRGB<float> > histo( 256 );

    if( is16Bit ) {
        ushort* tmp = ( ushort* )buffer;
        QVector< ColorRGB<float> > histoBIG( 65536 );

        for( int i = 0; i < size * colors; i += stepwidth * colors ) {

            histoBIG[tmp[i + 0]].r++;
            histoBIG[tmp[i + 1]].g++;
            histoBIG[tmp[i + 2]].b++;

            if( hasAlpha ) {
                i++;
            }
        }

        for( int i = 0; i < 256; i++ ) {
            histo[i].r = histoBIG[i * 255].r / size * scale;
            histo[i].g = histoBIG[i * 255].g / size * scale;
            histo[i].b = histoBIG[i * 255].b / size * scale;
        }
    } else {
        uchar* tmp = ( uchar* )buffer;

        QVector< ColorRGB<float> > histoSMALL( 256 );

        for( int i = 0; i < size * colors; i += stepwidth * colors ) {

            histoSMALL[tmp[i + 0]].r++;
            histoSMALL[tmp[i + 1]].g++;
            histoSMALL[tmp[i + 2]].b++;

            if( hasAlpha ) {
                i++;
            }
        }

        for( int i = 0; i < 256; i++ ) {
            histo[i].r = histoSMALL[i].r / size * scale;
            histo[i].g = histoSMALL[i].g / size * scale;
            histo[i].b = histoSMALL[i].b / size * scale;
        }
    }

    return histo;
}

#ifdef BLACKSILK_STANDALONE

bool MainWindow::exportImageToPath( libfoundation::app::EImageFormat::t format, const std::string& path ) {
    QElapsedTimer t;
    t.start();

    const auto ret = theApp()->saveImage(
                         format,
                         path
                     );

    const auto elapsed = t.elapsed();
    qDebug() << "Export finished after" << ( QString::number( elapsed ) + "ms" );

    return ret;
}

void MainWindow::slotImageExportFinished() {
    occupy_guard guard( ( int* )1, [&]( int* ) {
        emit signalUnoccupied();
    } );

    /// clean up backend
    /// allocators

    theApp()->appBackend->cleanUp();
    theApp()->state = App::EState::Running;
    ui->statusbar->clearMessage();
}
#endif // BLACKSILK_STANDALONE

void MainWindow::saveImage( const QString& filename ) {
#ifdef BLACKSILK_STANDALONE

    if( !theApp()->canExport() ) {
        QMessageBox::information(
            this,
            "Export currently not possible",
            "Export is currently not possible. Please wait for the currently running import or export process"
            "to finish.",
            QMessageBox::Ok
        );
        qDebug() << "User requested export denied: Currently not able to export image.";
        return;
    }

    theApp()->state = App::EState::Exporting;

    emit signalOccupied();

    if( filename.isEmpty() ) {
        qWarning() << "MainWindow::saveImage(): Invalid filename";
        ui->statusbar->showMessage( "No filename", 2000 );
        return;
    }

    /* render */
    ui->statusbar->showMessage( "Saving image..." );
    qApp->processEvents();

    QFileInfo fi( filename );
    libfoundation::app::EImageFormat::t format = libfoundation::app::EImageFormat::formatFromExtension(
                fi.suffix().toStdString()
            );
    assert( format != libfoundation::app::EImageFormat::Unknown );

    if( format == libfoundation::app::EImageFormat::Unknown ) {
        ui->statusbar->showMessage( "Unknown image format." );
        qApp->processEvents();
        qDebug() << "Failed to save image. Unknown image format: " << filename;
        return;
    }


    /// execute asynchronously if the image was scaled down and the cpu-renderer
    /// exports the final image.
    /*if ( theApp()->preview.isScaledDown || false ) {
        QMessageBox::warning(
            this,
            "Falling back to CPU rendering.",
            "Optimized GPU rendering is currently not capable of rendering images this large: "
            "Falling back to traditional CPU rendering.",
            QMessageBox::Ok
        );

        CHECK_QT_CONNECT( connect( &mImageWatcher, SIGNAL( finished() ), SLOT( slotImageExportFinished() ) ) );
        mImageWatcher.setFuture( QtConcurrent::run( [ = ]() {
            bool _ret = exportImageToPath(
                format,
                filename.toStdString()
            );

            return _ret;
        } ) );
    } else {
        exportImageToPath( format, filename.toStdString() );
        slotImageExportFinished();
        ui->statusbar->clearMessage();
    }*/

    exportImageToPath( format, filename.toStdString() );
    slotImageExportFinished();
    ui->statusbar->clearMessage();

#else
    Q_UNUSED( filename )
#endif
}

#include <QTextStream>

void MainWindow::savePreset( const QString& filename ) {
    occupy_guard guard( ( int* )1, [&]( int* ) {
        emit signalUnoccupied();
    } );
    emit signalOccupied();

    if( filename.isEmpty() ) {
        qWarning() << "MainWindow::savePreset(): Invalid filename";
        ui->statusbar->showMessage( "No filename", 2000 );
        return;
    }

    QFileInfo fileInfo( filename );
    const std::string& presetName = fileInfo.baseName().toStdString();

    ///! \todo writing presets
    const auto successfullySaved = theApp()->savePresetToPath(
                                       theApp()->currentSession->currentActiveStateToPresetCollection(),
                                       filename.toStdString(),
                                       presetName
                                   );
    assert( successfullySaved );

    if( !successfullySaved ) {
        qDebug() << "Failed to save preset to " << filename;

        /// alert user!

    }
}

//! \brief Resets all filter GUIs and sends corresponding signals to the widget
void MainWindow::slotResetFilters() {
    Q_ASSERT( mMixerWidget );
    Q_ASSERT( mVignetteWidget );
    Q_ASSERT( mSplitToneWidget );
    Q_ASSERT( mGrainWidget );
    Q_ASSERT( mSharpenWidget );

    std::function<void( QAction* action )> uncheckAction = []( QAction * action ) {
        if( action->isChecked() ) {
            action->trigger();
        }
    };

    m_Resetting = true;
    m_UndoStack.beginMacro( tr( "Reset Filters" ) );
    {
        mMixerWidget->reset();
        mCurvesWidget->reset();
        uncheckAction( ui->actionCurves );
        mSharpenWidget->reset();
        uncheckAction( ui->actionSharpen );
        mVignetteWidget->reset();
        uncheckAction( ui->actionVignette );
        mSplitToneWidget->reset();
        uncheckAction( ui->actionSplitTone );
        mGrainWidget->reset();
        uncheckAction( ui->actionGrain );
    }
    m_UndoStack.endMacro();
    m_Resetting = false;

    theApp()->app->currentSession()->disableAllFilters();

    this->slotResetActions();
}

//! \brief Initializes all filter GUIs and sends corresponding signals to the widget
void MainWindow::slotInitializeFilters() {
    Q_ASSERT( mMixerWidget );
    Q_ASSERT( mVignetteWidget );
    Q_ASSERT( mSplitToneWidget );
    Q_ASSERT( mGrainWidget );
    Q_ASSERT( mSharpenWidget );

    mMixerWidget->initialize();
    mCurvesWidget->initialize();
    mSharpenWidget->initialize();
    mVignetteWidget->initialize();
    mSplitToneWidget->initialize();
    mGrainWidget->initialize();

    theApp()->app->currentSession()->disableAllFilters();

    this->slotResetActions();
}

void MainWindow::slotResetActions() {

    ui->actionCurves->setChecked( false );
    ui->actionSharpen->setChecked( false );
    ui->actionGrain->setChecked( false );
    ui->actionSplitTone->setChecked( false );
    ui->actionVignette->setChecked( false );

    ui->actionMixer->trigger();
    theApp()->triggerRendering();
}

//! \brief Initializes the filter GUIs and puts them into a stacked layout
void MainWindow::initOperationWidgets() {
    Q_ASSERT( mStackedLayout );
    Q_ASSERT( mMixerWidget );
    Q_ASSERT( mCurvesWidget );
    Q_ASSERT( mVignetteWidget );
    Q_ASSERT( mSplitToneWidget );
    Q_ASSERT( mGrainWidget );
    Q_ASSERT( mSharpenWidget );

    /* mixer signal: r, g, b */
    mStackedLayout->addWidget( mMixerWidget );
    CHECK_QT_CONNECT( connect( mMixerWidget, SIGNAL( signalMixer( float, float, float, float, float, float, float ) ), ui->glWidgetPreview, SLOT( slotChangeMixer( float, float, float, float, float, float, float ) ) ) );
    CHECK_QT_CONNECT( connect( mMixerWidget, SIGNAL( signalMixer( float, float, float, float, float, float, float ) ), mCurvesWidget, SLOT( slotSetWeights( float, float, float, float, float, float, float ) ) ) );
    CHECK_QT_CONNECT( connect( mMixerWidget, &MixerWidget::signalUndoIterated, this, &MainWindow::slotAddUndoStep ) );

    /* vignette signal: x, y, radius, strength */
    mStackedLayout->addWidget( mVignetteWidget );
    CHECK_QT_CONNECT( connect( mVignetteWidget, &VignetteWidget::signalVignette, ui->glWidgetPreview, &GLWidget::slotChangeVignette ) );
    CHECK_QT_CONNECT( connect( mVignetteWidget, &VignetteWidget::signalUndoIterated, this, &MainWindow::slotAddUndoStep ) );

    /* splittone signal: */
    mStackedLayout->addWidget( mSplitToneWidget );
    CHECK_QT_CONNECT( connect( mSplitToneWidget, SIGNAL( signalSplit( float, float, float, float, float, float, float ) ), ui->glWidgetPreview, SLOT( slotChangeSplit( float, float, float, float, float, float, float ) ) ) );
    CHECK_QT_CONNECT( connect( mSplitToneWidget, &SplitToneWidget::signalUndoIterated, this, &MainWindow::slotAddUndoStep ) );

    /* grain signal: */
    mStackedLayout->addWidget( mGrainWidget );
    CHECK_QT_CONNECT( connect( mGrainWidget, &GrainWidget::signalGrain, ui->glWidgetPreview, &GLWidget::slotChangeGrain ) );
    CHECK_QT_CONNECT( connect( mGrainWidget, &GrainWidget::signalUndoIterated, this, &MainWindow::slotAddUndoStep ) );

    /* sharpen signal: */
    mStackedLayout->addWidget( mSharpenWidget );
    CHECK_QT_CONNECT( connect( mSharpenWidget, &SharpenWidget::signalSharpen, ui->glWidgetPreview, &GLWidget::slotChangeSharpen ) );
    CHECK_QT_CONNECT( connect( mSharpenWidget, &SharpenWidget::signalUndoIterated, this, &MainWindow::slotAddUndoStep ) );

    /* curves signal: */
    mStackedLayout->addWidget( mCurvesWidget );
    CHECK_QT_CONNECT( connect( mCurvesWidget, &CurvesWidget::signalCurves, ui->glWidgetPreview, &GLWidget::slotChangeCurves ) );
    /* set the out histogram of the curves filter as input histogram of the grain filter */
    CHECK_QT_CONNECT( connect( mCurvesWidget, &CurvesWidget::signalOutHistogram, mGrainWidget, &GrainWidget::slotSetHistogram ) );
    CHECK_QT_CONNECT( connect( mCurvesWidget, &CurvesWidget::signalUndoIterated, this, &MainWindow::slotAddUndoStep ) );

    QBoxLayout* boxLayout = new QBoxLayout( QBoxLayout::TopToBottom, ui->container );
    boxLayout->setContentsMargins( 0, 0, 4, 0 );
    boxLayout->addLayout( mStackedLayout );

    ui->container->setContentsMargins( 0, 0, 0, 0 );
    ui->container->setLayout( boxLayout );
}

void MainWindow::initUndoMenu() {
    m_UndoAction->setShortcut( QKeySequence::Undo );
    ui->menuEdit->addAction( m_UndoAction );

    m_RedoAction->setShortcut( QKeySequence::Redo );
    ui->menuEdit->addAction( m_RedoAction );

#if _DEBUG
    this->m_UndoView = new QUndoView( &m_UndoStack );
    this->ui->splitter->addWidget( this->m_UndoView );
#endif // _DEBUG
}

void MainWindow::setupHistograms() {
    assert( theApp() );
    assert( !theApp()->currentSession->originalImage()->empty() );

    if( theApp()->currentSession->originalImage()->empty() ) {
        qDebug() << "Failed to setup histograms: original image is empty.";
        return;
    }

    const auto _topLayer = theApp()->currentSession->originalImage()->topLayer();

    if( _topLayer == nullptr ) {
        qDebug() << "Failed to setup histograms: original image not set.";
        assert( false );
        return;
    }

    libgraphics::backend::cpu::ImageObject* _backendObj = ( libgraphics::backend::cpu::ImageObject* )_topLayer->internalImageForBackend( FXAPI_BACKEND_CPU );

    if( !_backendObj ) {
        qDebug() << "Failed to setup histograms: Needs valid original image with cpu-backend.";
        assert( false );
        return;
    }

    const bool is16         = ( libgraphics::fxapi::EPixelFormat::getChannelSize( _backendObj->format() ) == 2 );
    const bool isRgba       = ( libgraphics::fxapi::EPixelFormat::getChannelCount( _backendObj->format() ) == 4 );

    QVector< ColorRGB<float> > histo = calculateHistogram( _backendObj->data(), _backendObj->width() * _backendObj->height(), is16, isRgba );
    mCurvesWidget->slotSetHistogram( histo );
}

void MainWindow::setupWidgetsForPreview() {
    ui->glWidgetPreview->makeCurrent();

    setupHistograms();

    if( !theApp()->taskListener ) {
        theApp()->initializeTaskListener(
            ui->statusbar
        );
    }

    ui->glWidgetPreview->setupPreview();
    theApp()->triggerRendering();
    this->slotInitializeFilters();
    this->m_UndoStack.clear();
}

//! \brief Helper function to insert a spacer between toolbar widgets
//! \param size Width and height of the spacer
void MainWindow::addSpacerToToolBar( QToolBar* toolbar, int size ) {
    QWidget* spacer = new QWidget( this );
    spacer->setFixedSize( size, size );
    toolbar->addWidget( spacer );
}

//! \brief  Puts zoom actions into main menu
//!         As plugin the actions are added to this
void MainWindow::setupViewMenu() {
#ifndef BLACKSILK_STANDALONE
    this->addAction( ui->actionZoomIn );
    this->addAction( ui->actionZoomOut );
    this->addAction( ui->actionFitOnWidth );
    this->addAction( ui->actionFitOnHeight );
    this->addAction( ui->actionActualPixels );
#else
    ui->menuView->addAction( ui->actionZoomIn );
    ui->menuView->addAction( ui->actionZoomOut );
    ui->menuView->addSeparator();
    ui->menuView->addAction( ui->actionFitOnWidth );
    ui->menuView->addAction( ui->actionFitOnHeight );
    ui->menuView->addSeparator();
    ui->menuView->addAction( ui->actionActualPixels );
#endif
}

//! \brief Puts filter actions into main menu: mixer, curves, sharpen, vignette, grain, split tone
void MainWindow::setupFilterMenu() {
    ui->menuFilter->addAction( ui->actionMixer );
    ui->menuFilter->addSeparator();
    ui->menuFilter->addAction( ui->actionCurves );
    ui->menuFilter->addAction( ui->actionSharpen );
    ui->menuFilter->addAction( ui->actionVignette );
    ui->menuFilter->addAction( ui->actionGrain );
    ui->menuFilter->addAction( ui->actionSplitTone );

#ifndef _DEBUG
    ui->menuHelp->removeAction( ui->actionUndo_Stack );
#endif // _DEBUG
}

//! \brief Initializes filters: mixer, curves, sharpen, vignette, grain, split tone
void MainWindow::setupFilterToolBar() {
    Q_ASSERT( mToolbarFilters );

    mToolbarFilters->setFloatable( false );
    mToolbarFilters->setMovable( false );
    mToolbarFilters->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    mToolbarFilters->setIconSize( QSize( 48, 48 ) );

    addSpacerToToolBar( mToolbarFilters, 20 );
    mToolbarFilters->addAction( ui->actionMixer );
    addSpacerToToolBar( mToolbarFilters, 20 );
    mToolbarFilters->addSeparator();

    mToolbarFilters->addAction( ui->actionCurves );
    {
        mToolbarFilters->addSeparator();
    }

    mToolbarFilters->addAction( ui->actionSharpen );
    {
        mToolbarFilters->addSeparator();
    }

    mToolbarFilters->addAction( ui->actionVignette );
    {
        mToolbarFilters->addSeparator();
    }

    mToolbarFilters->addAction( ui->actionGrain );
    {
        mToolbarFilters->addSeparator();
    }

    mToolbarFilters->addAction( ui->actionSplitTone );
    {
        mToolbarFilters->addSeparator();
    }

    QWidget* spacer = new QWidget( this );
    spacer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    mToolbarFilters->addWidget( spacer );

    addToolBar( Qt::LeftToolBarArea, mToolbarFilters );
    mToolbarFilters->setAllowedAreas( Qt::LeftToolBarArea );

    this->adjustFilterWidgetWidth();
}

void MainWindow::adjustFilterWidgetWidth() {

    QVector<QAction*> filters;
    filters << ui->actionMixer
            << ui->actionCurves
            << ui->actionSharpen
            << ui->actionVignette
            << ui->actionGrain
            << ui->actionSplitTone;

    int width = 0;

    foreach( QAction* a, filters ) {
        QWidget* w = mToolbarFilters->widgetForAction( a );
        width = qMax( width, w->width() );
    }

    foreach( QAction* a, filters ) {
        QWidget* w = mToolbarFilters->widgetForAction( a );
        w->setFixedWidth( width );
    }
}

//! \brief Initializes toolbar & buttons: ok, quit, reset, about
void MainWindow::setupFinishToolBar() {
    Q_ASSERT( mToolbarFinish );
    mToolbarFinish->setFloatable( false );
    mToolbarFinish->setMovable( false );

    QWidget* spacer = new QWidget( this );
    spacer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    FinishGroup* finishGroup = new FinishGroup( this );
    finishGroup->resize( 0, 0 );
    finishGroup->hide();
    addSpacerToToolBar( mToolbarFinish, 10 );

    // add "visible" for shortcuts
    this->addAction( m_UndoAction );
    this->addAction( m_RedoAction );
    this->addAction( ui->actionPresetsOpen );
    this->addAction( ui->actionPresetsSave_As );
    this->addAction( ui->actionResetAll );

    finishGroup->buttonEdit()->setMenu( ui->menuEdit );
    finishGroup->buttonPresets()->setMenu( ui->menuPresets );
    mToolbarFinish->addWidget( finishGroup->buttonAbout() );
    mToolbarFinish->addWidget( spacer );
    mToolbarFinish->addWidget( finishGroup->buttonEdit() );
    mToolbarFinish->addWidget( finishGroup->buttonPresets() );
    mToolbarFinish->addSeparator();
    mToolbarFinish->addWidget( finishGroup->buttonQuit() );
    addSpacerToToolBar( mToolbarFinish, 5 );
    mToolbarFinish->addWidget( finishGroup->buttonOk() );
    addSpacerToToolBar( mToolbarFinish, 5 );

    CHECK_QT_CONNECT( connect( finishGroup, &FinishGroup::signalOk, this, &MainWindow::slotFinalRendering ) );
    CHECK_QT_CONNECT( connect( finishGroup, &FinishGroup::signalAbout, this, &MainWindow::slotAbout ) );
    CHECK_QT_CONNECT( connect( finishGroup, &FinishGroup::signalQuit, this, &MainWindow::on_actionQuit_triggered ) );

    addToolBar( Qt::TopToolBarArea, mToolbarFinish );
    mToolbarFinish->setAllowedAreas( Qt::TopToolBarArea );
}

void MainWindow::setupPointlessBar() {
    PointlessBar* pointless = new PointlessBar();
    CHECK_QT_CONNECT( connect( this, &MainWindow::signalOccupied, pointless, &PointlessBar::start ) );
    CHECK_QT_CONNECT( connect( this, &MainWindow::signalUnoccupied, pointless, &PointlessBar::stop ) );
    ui->statusbar->addPermanentWidget( pointless );
}

void MainWindow::on_lastWindowClosed() {

    qDebug() << "Resetting preview and session objects...";
    theApp()->actionRenderPreview.reset();
    theApp()->currentSession->resetImageState();
    theApp()->filterFilmGrain->deleteGrainForBackend( FXAPI_BACKEND_CPU );
    theApp()->filterCascadedSharpen->deleteBlurBuffersForBackend( FXAPI_BACKEND_CPU );

    const auto undeletedMemory = theApp()->appBackend->allocator()->queryMemoryConsumption();

    if( undeletedMemory > 0 ) {
        qDebug() << "Detected undeleted backend memory:" << ( undeletedMemory / ( 1000 * 1000 ) ) << "megabytes";
    }

    blacksilk::shutdownApplication();
    resetApp();
}

void MainWindow::on_actionQuit_triggered() {
    this->close();
}

MainWindow::~MainWindow() {
    LIBGRAPHICS_MEMORY_LOG_PRINT

    qDebug() << "Bye bye...\n\n\n";
    delete ui;
}

void MainWindow::slotFinalRendering() {
    this->update();
#ifndef BLACKSILK_STANDALONE


    Plugin::Image image = this->mFinal();
    const auto successfullyWrittenImage = theApp()->saveImageToBuffer( image.mBuffer );
    assert( successfullyWrittenImage );

    if( !successfullyWrittenImage ) {
        qDebug() << "Failed to write image to destination buffer. Aborting...";
        return;
    }

    /* say PS, we were successful and leave */
    *run = true;
    on_actionQuit_triggered();
#endif
}

void MainWindow::slotAbout() {
    About about;
    about.exec();
}

void MainWindow::on_actionMixer_triggered() {
    Q_ASSERT( mMixerWidget );
    Q_ASSERT( mStackedLayout );

    mStackedLayout->setCurrentWidget( mMixerWidget );
}

void MainWindow::on_actionFilter_triggered( QAction* action, FilterWidget* widget, libfoundation::app::EFilter::t filter ) {

    bool ctrlPressed = QApplication::keyboardModifiers() & Qt::ControlModifier;
    bool oldState = ! action->isChecked(); // state before triggered

    if( ctrlPressed && mStackedLayout->currentWidget() != widget ) {
        action->setChecked( !oldState );

        if( oldState ) {
            theApp()->currentSession->disableFilter( filter );
        } else {
            theApp()->currentSession->enableFilter( filter );
        }
    } else {
        if( mStackedLayout->currentWidget() == widget ) {
            mStackedLayout->setCurrentWidget( mMixerWidget );
            theApp()->currentSession->disableFilter( filter );
            action->setChecked( false );
        } else {
            mStackedLayout->setCurrentWidget( widget );
            theApp()->currentSession->enableFilter( filter );
            action->setChecked( true );
        }
    }

    theApp()->triggerRendering();
    this->ui->glWidgetPreview->update();

    // undo
    if( m_Resetting || ( oldState != theApp()->currentSession->isFilterEnabled( filter ) ) ) {
        ActionUndoCommand* undoCommand = new ActionUndoCommand( [ = ]( bool b ) {
            if( action->isChecked() != b ) {
                action->setChecked( b );

                if( b ) {
                    theApp()->currentSession->enableFilter( filter );
                } else {
                    mStackedLayout->setCurrentWidget( mMixerWidget );
                    theApp()->currentSession->disableFilter( filter );
                }

                theApp()->triggerRendering();
                this->ui->glWidgetPreview->update();
            }

        }, oldState, !oldState );

        undoCommand->setText( action->text() );
        m_UndoStack.push( undoCommand );
    }
}

//! \sa https://gist.github.com/QuantumCD/6245215
void MainWindow::setupDarkStyle() {
    qApp->setStyle( QStyleFactory::create( "Fusion" ) );

    QPalette darkPalette;
    darkPalette.setColor( QPalette::Window, QColor( 53, 53, 53 ) );
    darkPalette.setColor( QPalette::WindowText, Qt::white );
    darkPalette.setColor( QPalette::Base, QColor( 25, 25, 25 ) );
    darkPalette.setColor( QPalette::AlternateBase, QColor( 53, 53, 53 ) );
    darkPalette.setColor( QPalette::ToolTipBase, Qt::white );
    darkPalette.setColor( QPalette::ToolTipText, Qt::white );
    darkPalette.setColor( QPalette::Text, Qt::white );
    darkPalette.setColor( QPalette::Button, QColor( 53, 53, 53 ) );
    darkPalette.setColor( QPalette::ButtonText, Qt::white );
    darkPalette.setColor( QPalette::BrightText, Qt::red );
    darkPalette.setColor( QPalette::Link, QColor( 42, 130, 218 ) );

    darkPalette.setColor( QPalette::Highlight, QColor( 42, 130, 218 ) );
    darkPalette.setColor( QPalette::HighlightedText, Qt::black );

    qApp->setPalette( darkPalette );

    qApp->setStyleSheet( "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }" );
}

void MainWindow::on_actionVignette_triggered() {
    Q_ASSERT( mMixerWidget );
    Q_ASSERT( mVignetteWidget );
    Q_ASSERT( mStackedLayout );
    Q_ASSERT( ui->glWidgetPreview );

    this->on_actionFilter_triggered( ui->actionVignette,
                                     mVignetteWidget,
                                     libfoundation::app::EFilter::VignetteFilter
                                   );
}

void MainWindow::on_actionSplitTone_triggered() {
    Q_ASSERT( mMixerWidget );
    Q_ASSERT( mSplitToneWidget );
    Q_ASSERT( mStackedLayout );

    this->on_actionFilter_triggered( ui->actionSplitTone,
                                     mSplitToneWidget,
                                     libfoundation::app::EFilter::SplitToneFilter
                                   );
}


void MainWindow::on_actionGrain_triggered() {
    Q_ASSERT( mMixerWidget );
    Q_ASSERT( mGrainWidget );
    Q_ASSERT( mStackedLayout );

    this->on_actionFilter_triggered( ui->actionGrain,
                                     mGrainWidget,
                                     libfoundation::app::EFilter::FilmGrainFilter
                                   );
}

void MainWindow::on_actionSharpen_triggered() {
    Q_ASSERT( mMixerWidget );
    Q_ASSERT( mSharpenWidget );
    Q_ASSERT( mStackedLayout );

    this->on_actionFilter_triggered( ui->actionSharpen,
                                     mSharpenWidget,
                                     libfoundation::app::EFilter::CascadedSharpenFilter
                                   );
}

void MainWindow::on_actionCurves_triggered() {
    Q_ASSERT( mMixerWidget );
    Q_ASSERT( mCurvesWidget );
    Q_ASSERT( mStackedLayout );

    this->on_actionFilter_triggered( ui->actionCurves,
                                     mCurvesWidget,
                                     libfoundation::app::EFilter::CurvesFilter
                                   );
}

void MainWindow::on_actionWebsite_triggered() {
    QDesktopServices::openUrl( QUrl( "https://github.com/elsamuko/blacksilk" ) );
}

void MainWindow::on_actionAbout_triggered() {
    slotAbout();
}

void blacksilk::ui::MainWindow::on_actionZoomIn_triggered() {
    ui->glWidgetPreview->slotZoomIn();
}

void blacksilk::ui::MainWindow::on_actionZoomOut_triggered() {
    ui->glWidgetPreview->slotZoomOut();
}

void blacksilk::ui::MainWindow::on_actionFitOnWidth_triggered() {
    ui->glWidgetPreview->slotFitTo( GLWidget::enumFitToWidth );
}

void blacksilk::ui::MainWindow::on_actionFitOnHeight_triggered() {
    ui->glWidgetPreview->slotFitTo( GLWidget::enumFitToHeight );
}

void blacksilk::ui::MainWindow::on_actionActualPixels_triggered() {
    ui->glWidgetPreview->slotFitTo( GLWidget::enumFitToActualPixels );
}

#ifdef BLACKSILK_STANDALONE
void MainWindow::slotImageImportFinished() {
    ui->glWidgetPreview->makeCurrent();

    // hide pointless bar on function exit
    occupy_guard guard( ( int* )1, [&]( int* ) {
        emit signalUnoccupied();
    } );

    QString filename = QString::fromStdString( theApp()->currentSession->imagePath() );
    const libgraphics::Image* image = theApp()->currentSession->originalImage();

    /* check */
    if( !image || image->empty() ) {
        QMessageBox::information( this, tr( "Open Image" ), QString( tr( "Cannot load %1." ) ).arg( filename ) );
        ui->statusbar->clearMessage();
        return;
    }

    if( filename != m_SampleFilename ) {
        mFileDealer.setFilename( filename );
    }

    ui->statusbar->showMessage( "Processing image..." );
    qApp->processEvents();

    /* construct window title, e.g.  */
    /* FDI Black Silk - test.jpg 300x200 */
    QFileInfo info( filename );

#if BLACKSILK_TEST_SUITE
    QString windowTitle = "FD Imaging - Black Silk Test Suite - ";
#else
    QString windowTitle = "FD Imaging - Black Silk - ";
#endif

    windowTitle += info.fileName() + " ";
    windowTitle += QString::number( theApp()->currentSession->originalImage()->width() )  + "x";
    windowTitle += QString::number( theApp()->currentSession->originalImage()->height() );

    const float megapixels = ( float )( theApp()->currentSession->originalImage()->width() * theApp()->currentSession->originalImage()->height() ) / ( float )( 1000 * 1000 );
    windowTitle += " - " + QString::number( megapixels, 'g', 3 ) + "MP";

    this->setWindowTitle( windowTitle );

    const auto _ret = theApp()->setupPreviewFromOriginalImage();

    if( !_ret ) {
        assert( false );
        return;
    }

    this->setupWidgetsForPreview();

    ui->statusbar->showMessage( "Rendering Preview" );
    qApp->processEvents();
    slotResetActions();
    update();

    theApp()->state = App::EState::Running;
    ui->statusbar->clearMessage();
}
#endif // BLACKSILK_STANDALONE

void blacksilk::ui::MainWindow::on_actionPresetsOpen_triggered() {


    QString filename;
    bool ok;
    std::tie( ok, filename ) = mPresetsDealer.getOpenFilename();

    if( ok ) {
        this->openPreset( filename );
    }

    /* regain focus */
    this->activateWindow();
}

void blacksilk::ui::MainWindow::on_actionPresetsSave_As_triggered() {

    QString filename;
    bool ok;
    std::tie( ok, filename ) = mPresetsDealer.getSaveFilename();

    if( ok ) {
        this->savePreset( filename );
    }

    /* regain focus */
    this->activateWindow();
}

void MainWindow::dragEnterEvent( QDragEnterEvent* event ) {
    event->accept();
}

//! \see mainwindow.ui: acceptDrops
void MainWindow::dropEvent( QDropEvent* event ) {
#ifdef BLACKSILK_STANDALONE
    const QMimeData* data = event->mimeData();

    if( data->hasUrls() ) {
        QList<QUrl> urls = data->urls();

        if( !urls.empty() ) {
            /* open first image */
            QString filename = urls[0].path();

#ifdef WIN32

            // avoid "/C:/image.jpg"
            if( filename.startsWith( '/' ) ) {
                filename.remove( 0, 1 );
            }

#endif // WIN32

            QFileInfo info( filename );

            if( info.exists() && mFileDealer.knows( info.suffix() ) ) {
                openImage( filename );
            } else if( info.exists() && mPresetsDealer.knows( info.suffix() ) ) {
                mPresetsDealer.setFilename( filename );
                openPreset( filename );
            } else {
                qWarning() << "Cannot open " << filename;
                ui->statusbar->showMessage( "Invalid file type", 2000 );
            }
        }
    }

#endif // BLACKSILK_STANDALONE
}

void blacksilk::ui::MainWindow::on_actionResetAll_triggered() {
    slotResetFilters();
}

void blacksilk::ui::MainWindow::on_actionUndo_Stack_triggered() {
    mStackedLayout->setCurrentWidget( m_UndoView );
}

void blacksilk::ui::MainWindow::on_actionViewLogFile_triggered() {
    QDesktopServices::openUrl( QUrl::fromLocalFile( QString::fromStdString( logging::logFilename() ) ) );
}
