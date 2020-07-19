#pragma once

#include "glwidget.h"
#include <QtGui>
#include <QGLWidget>
#include <QMainWindow>
#include <QFutureWatcher>
#include <QUndoStack>
#include <QUndoView>

#include "filedealer.h"
#include "utils/colorspaces.hpp"
#include <utils/responsivebackgroundtasklistener.hpp>


#ifndef BLACKSILK_STANDALONE
#   include "plugin/plugin.h"
#endif

#include <functional>
#include <memory>
/// \end

using blacksilk::ui::GLWidget;
using blacksilk::graphics::ColorRGB;

/// \brief  Prototypes
class QToolBar;
class QStackedLayout;
class MixerWidget;
class CurvesWidget;
class SharpenWidget;
class VignetteWidget;
class GrainWidget;
class SplitToneWidget;
class FilterWidget;
class FilterUndoCommand;

/// \begin
/// \brief  local ui namespace
namespace Ui {
class MainWindow;
}
/// \end

/// \begin
/// \brief  blacksilk namespace
namespace   blacksilk {

/// \begin
/// \brief  ui namespace
namespace ui {

/// \brief  main window class
class MainWindow : public QMainWindow {
        Q_OBJECT
        typedef std::unique_ptr<int, std::function<void ( int* )>> occupy_guard;

    public:
        explicit MainWindow( libfoundation::app::ApplicationSession* session, bool* b, bool* s, QWidget* parent = 0 );
        virtual ~MainWindow();

        void setupWidgetsForPreview();

        void openSample();
#ifndef BLACKSILK_STANDALONE
        void setFinal( Plugin::Image( *final )() ) {
            mFinal = final;
        }
#endif
    signals:
        void signalFinished();
        void signalOccupied();
        void signalUnoccupied();

    private slots:
        /* actions */
        void on_lastWindowClosed();
        void on_actionMixer_triggered();
        void on_actionVignette_triggered();
        void on_actionSplitTone_triggered();
        void on_actionGrain_triggered();
        void on_actionSharpen_triggered();
        void on_actionCurves_triggered();

        /* from finish tool bar */
        void slotFinalRendering();
        void slotAbout();
        void slotResetFilters();
        void slotInitializeFilters();
        void slotResetActions();
        void slotAddUndoStep( FilterUndoCommand* command );

        /* standalone functions*/
        void openImage( const QString& filename );
        void openPreset( const QString& filename );
        void on_actionQuit_triggered();
        void on_actionOpen_triggered();
        void on_actionSave_triggered();
        void on_actionSave_As_triggered();
        void on_actionPresetsOpen_triggered();
        void on_actionPresetsSave_As_triggered();
        void on_actionResetAll_triggered();

        void on_actionWebsite_triggered();
        void on_actionAbout_triggered();

        void on_actionZoomIn_triggered();
        void on_actionZoomOut_triggered();
        void on_actionFitOnWidth_triggered();
        void on_actionFitOnHeight_triggered();
        void on_actionActualPixels_triggered();

#ifdef BLACKSILK_STANDALONE
        /* called, if image was loaded by thread */
        void slotImageImportFinished();
        void slotImageExportFinished();
        bool exportImageToPath( libfoundation::app::EImageFormat::t format, const std::string& path );
#endif
        void on_actionUndo_Stack_triggered();
        void on_actionViewLogFile_triggered();

    private:
        void updateAllocatorsForNewImage();
        void onUpdateAllValues();
        void on_actionFilter_triggered( QAction* action, FilterWidget* widget, libfoundation::app::EFilter::t filter );

        void setupDarkStyle();
        void setupHistograms();
        void setupFilterToolBar();
        void setupFinishToolBar();
        void setupPointlessBar();
        void adjustFilterWidgetWidth();
        void setupFilterMenu();
        void setupViewMenu();
        void initOperationWidgets();
        void initUndoMenu();
        void addSpacerToToolBar( QToolBar* toolbar, int width );
        void setupRecentImagesMenu();
        void setupRecentPresetsMenu();
        void saveImage( const QString& filename );
        void savePreset( const QString& filename );
        QVector< ColorRGB<float> > calculateHistogram( void* buffer, int size, bool is16Bit, bool hasAlpha );

        /* drag & drop */
        virtual void dropEvent( QDropEvent* event );
        virtual void dragEnterEvent( QDragEnterEvent* event );

        /* from PS */
        bool* run;
        bool* display;
#ifndef BLACKSILK_STANDALONE
        // function pointer, which returns the final image buffer
        Plugin::Image( *mFinal )();
#endif

        /* filter widgets */
        MixerWidget* mMixerWidget;
        VignetteWidget* mVignetteWidget;
        SplitToneWidget* mSplitToneWidget;
        GrainWidget* mGrainWidget;
        SharpenWidget* mSharpenWidget;
        CurvesWidget* mCurvesWidget;

        QStackedLayout* mStackedLayout;
        QToolBar* mToolbarFilters;
        QToolBar* mToolbarFinish;

        /* standalone */
        QFutureWatcher< bool > mImageWatcher;

        bool m_Resetting;
        QUndoStack m_UndoStack;
        QUndoView* m_UndoView;
        QAction* m_UndoAction;
        QAction* m_RedoAction;
        QString m_SampleFilename;

        /* io helper class */
        FileDealer mFileDealer;
        FileDealer mPresetsDealer;

        /* Qt User Interface */
        Ui::MainWindow* ui;
};

}
/// \end
}
/// \end
