#pragma once

/// basic graphics stuff
#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>

/// filters
#include <libgraphics/fx/filters/bwmixer.hpp>
#include <libgraphics/fx/filters/bwadaptivemixer.hpp>
#include <libgraphics/fx/filters/curves.hpp>
#include <libgraphics/fx/filters/cascadedsharpen.hpp>
#include <libgraphics/fx/filters/filmgrain.hpp>
#include <libgraphics/fx/filters/splittone.hpp>
#include <libgraphics/fx/filters/unsharpmask.hpp>
#include <libgraphics/fx/filters/vignette.hpp>

/// libfoundation stuff
#include <libfoundation/app/application.hpp>
#include <libfoundation/app/backgroundtask.hpp>
#include <libfoundation/app/backgroundtaskgroup.hpp>
#include <libfoundation/app/backgroundtasklistener.hpp>

#include <libfoundation/app/tasks/preparesharpen.hpp>
#include <libfoundation/app/tasks/rendergrain.hpp>

/// core stuff
#include <utils/hostmachine.hpp>
#include <utils/graphicsview.hpp>
#include <utils/responsivebackgroundtasklistener.hpp>

#include <QElapsedTimer>


class QStatusBar;


namespace blacksilk {

struct App {
        enum EState : unsigned long {
            Running,
            Importing,
            Exporting
        };
        EState                                                                                  state;


        inline bool isRunning() const {
            return ( state == EState::Running );
        }
        inline bool canImport() const {
            return isRunning();
        }
        inline bool canExport() const {
            return isRunning();
        }

        /// core application
        /// objects
        std::unique_ptr<libfoundation::app::Application>                                   app;
        libfoundation::app::ApplicationBackend*                                                 appBackend;
        libfoundation::app::ApplicationSession*                                                 currentSession;
        std::unique_ptr<libfoundation::app::ApplicationActionRenderPreview>                actionRenderPreview;

        /// ui
        std::unique_ptr<blacksilk::GraphicsView>                       view;
        bool                                                                shouldRender;

        /// image properties
        struct PreviewInfo {
            bool        isScaledDown;

            float       qualityFactor;

            size_t      maximalImageSize;
            size_t      currentImageSize;

            bool        shouldRenderStats;

            PreviewInfo() : isScaledDown( false ),
                qualityFactor( 1.0f ), maximalImageSize( 0 ),
                currentImageSize( 0 ), shouldRenderStats( false ) {}

        } preview;

        /// background tasks and
        /// status reports
        std::unique_ptr<libfoundation::app::BackgroundTaskGroup>       backgroundTasks;
        std::unique_ptr<blacksilk::ResponsiveBackgroundTaskListener>   taskListener;

        /// filter objects
        libgraphics::fx::filters::BWAdaptiveMixer*                      filterBWMixer;
        libgraphics::fx::filters::Curves*                               filterCurves;
        libgraphics::fx::filters::CascadedSharpen*                      filterCascadedSharpen;
        libgraphics::fx::filters::Vignette*                             filterVignette;
        libgraphics::fx::filters::SplitTone*                            filterSplitTone;
        libgraphics::fx::filters::FilmGrain*                            filterFilmGrain;

        /// temporary alpha plane
        libgraphics::Bitmap                                             alphaPlane;
        bool                                                            cachedAlphaPlane;

        /// constr/destr
        App();
        ~App();

        //! \returns true, if it has an image loaded
        bool hasImage() const;

        /**
            \fn initialized
            \since 1.0
            \brief Returns true, if the current App object was successfully
                initialized.
        */
        bool initialized() const;

        /**
            \fn initializeTaskListener
            \since 1.0
            \brief Initializes the internal background task listener, which reports to the
                specified QStaturBar control.
        */
        void initializeTaskListener(
            QStatusBar* statusBar
        );

        /**
            \fn initialize
            \since 1.0
            \brief Initializes the core application state. The core application state
                consists out of the following objects:
                    - application object: backend and image management
                    - machine requirements: testing for machine requirements
        */
        bool initialize(
            libfoundation::app::ApplicationConfig config
        );


        /**
            \fn initializeGraphicsBackend
            \since 1.0
            \brief Initializes the cpu-backend rendering
                engine.
        */
        bool initializeGraphicsBackend();

        /**
            \fn initializeGraphicsPreview
            \since 1.0
            \brief Initializes the gpu-backend rendering
                engine for live-preview.
        */
        bool initializeGraphicsPreview();

        /**
            \fn initializeGraphics
            \since 1.0
            \brief Initializes the backend graphics state.
        */
        bool initializeGraphics( bool onlyOffscreen = false );

        /**
            \fn shutdown
            \since 1.0
            \brief Destroys the internal application state and shuts down all
                central subsystems(rendering,image management,plugins, etc.)
        */
        bool shutdown();

        /**
            \fn enqueBackgroundTask
            \since 1.0
            \brief Enques a new global background
                task.
        */
        void enqueBackgroundTask(
            libfoundation::app::BackgroundTask* task,
            libfoundation::app::BackgroundTaskListener* listener = nullptr
        );

        /**
            \fn loadIoPluginFromPath
            \since 1.0
            \brief Loads a new io pipeline plugin from
                the specified path.
        */
        bool loadIoPluginFromPath(
            const std::string& path
        );

        /**
            \fn loadIoImporterFromPath
            \since 1.0
            \brief Loads a new io importer plugin from the
                specified path.
        */
        bool loadIoImporterFromPath(
            const std::string& path
        );

        /**
            \fn loadIoExporterFromPath
            \since 1.0
            \brief Loads a new io exporter from the specified
                path.
        */
        bool loadIoExporterFromPath(
            const std::string& path
        );

        /**
            \fn loadPresetFromPath
            \since 1.0
            \brief Loads the a preset from the specified
                path and applies it to all filters.
        */
        bool loadPresetFromPath(
            const std::string& path
        );

        /**
            \fn savePresetToPath
            \since 1.0
            \brief Converts the specified preset collection to a bs-format
                preset object and saves it to the specified
                path.
        */
        bool savePresetToPath(
            const libgraphics::FilterPresetCollection& collection,
            const std::string& path,
            const std::string& presetName
        );

        /**
            \fn usePresets
            \since 1.0
            \brief Applies the specified preset filter collection to the currently
                activated filter objects.
            \param presets  The preset collection to apply.
            \return Returns true, if one or more filters were affected.
        */
        bool usePresets(
            const libgraphics::FilterPresetCollection& collection
        );

        /**
            \fn postProcessOriginalImage
            \since 1.0
            \brief Postprocesses the current original image
                and eventually scales the preview template image layer
                down.
        */
        void postProcessOriginalImage();

        /**
            \fn setupPreviewFromOriginalImage
            \brief Uses the processed original-image to setup
                the internal preview.
        */
        bool setupPreviewFromOriginalImage();

        /**
            \fn setupPreview
            \since 1.0
            \brief Initializes the preview for the current session.
        */
        bool setupPreview(
            void* data,
            size_t width,
            size_t height,
            libgraphics::fxapi::EPixelFormat::t format
        );

        /**
            \fn writeImage
            \since 1.0
            \brief Writes the current preview image buffer
                to the specified raw buffer.
        */
        bool writeImage(
            void* destinationBuffer
        );

        /**
            \fn triggerRendering
            \since 1.0
            \brief Triggers the preview-backend to render
                a new preview image.
        */
        void triggerRendering();

        /**
            \fn updatePreview
            \since 1.0
            \brief Executes a preview render action, if needed. You can trigger
                this by calling triggerRendering().
        */
        bool updatePreview();

        /**
            \fn enableFrameTime
            \since 1.0
            \brief Enables a minimal time-window between single
                render frames.
        */
        void enableFrameTime( size_t maxFPS );

        /**
            \fn disableFrameTime
            \since 1.0
            \brief Disables the frame-timing cap.
        */
        void disableFrameTime();

        /**
            \fn maxFPS
            \since 1.0
            \brief Returns the current maximal fps
                number.
        */
        size_t&         maxFPS();
        const size_t&   maxFPS() const;

        std::string         retrieveCurrentLog();

        /**
            \fn openImage
            \since 1.0
            \brief Imports the specified image using the currently available
        */
        bool openImage(
            const std::string& path
        );

        /**
            \fn openImageFromData
            \brief Imports the specified raw-image buffer using the specified
                format and dimensions.
        */
        bool openImageFromData(
            void* _data,
            libgraphics::fxapi::EPixelFormat::t _format,
            size_t _width,
            size_t _height
        );

        /**
            \fn saveImage
            \since 1.0
            \brief Exports the current image to the specified
                path.
        */
        bool saveImage(
            libfoundation::app::EImageFormat::t format,
            const std::string& path
        );

        /**
            \fn saveImageToBuffer
            \since 1.0
            \brief Exports the current image to the buffer(without any format).
        */
        bool saveImageToBuffer(
            void* buffer
        );

    private:
        /**
            \fn postImageLoad
            \brief Called by openImage/openImageFromData after the specified images
                were loaded.
        */
        void postImageLoad();


        /**
            \fn loadInternalPresets
            \brief Loads the internal presets into to the corresponding
                filter preset collections.
        */
        bool loadInternalPresets();

        bool                m_Initialized;
        bool                m_InitializedGraphicsBackend;
        bool                m_InitializedGraphicsPreview;

        QElapsedTimer       m_FrameTimer;
        bool                m_FrameTimerEnabled;
        size_t              m_MaxFPS;
};

App* theApp();
void resetApp();

void initializeApplication();
void shutdownApplication();


}
