
#include <QFile>
#include <QCoreApplication>

#include <libfoundation/app/application.hpp>
#include <libgraphics/filtercollection.hpp>
#include <libgraphics/filterpreset.hpp>
#include <libgraphics/filterpresetcollection.hpp>
#include <libgraphics/filter.hpp>
#include <libgraphics/debug.hpp>
#include <libgraphics/backend/common/formats.hpp>

#include <libgraphics/fx/operations/samplers.hpp>
#include <libgraphics/backend/cpu/cpu_imageobject.hpp>
#include <utils/hostmachine.hpp>

#include <sstream>
#include <fstream>
#include <iostream>

#include <QFile>
#include <QFileInfo>

#include <log/log.hpp>
#include <utils/app.hpp>
#include <utils/preset.hpp>
#include <QDebug>
#include <gl/glew.h>

namespace blacksilk {

App::App() :
    state( App::EState::Running ),
    app( nullptr ),
    appBackend( nullptr ),
    currentSession( nullptr ),
    backgroundTasks( nullptr ),
    taskListener( nullptr ),
    filterBWMixer( nullptr ),
    filterCurves( nullptr ),
    filterVignette( nullptr ),
    filterSplitTone( nullptr ),
    filterFilmGrain( nullptr ),
    cachedAlphaPlane( false ),
    m_Initialized( false ),
    m_InitializedGraphicsBackend( false ),
    m_InitializedGraphicsPreview( false ),
    m_FrameTimerEnabled( true ),
    m_MaxFPS( 10 ) {

    this->view.reset( new blacksilk::GraphicsView() );

    /// initializing max fps
    const auto defaultMaxFPS = 24;
    this->enableFrameTime( defaultMaxFPS );

    /// initializing maximal megapixels
    const auto defaultMaximalImageSize = 50;

    this->preview.currentImageSize      = 0;
    this->preview.isScaledDown          = false;
    this->preview.maximalImageSize      = defaultMaximalImageSize;
    this->preview.qualityFactor         = 1.0f;

#if defined(BLACKSILK_TEST_SUITE) || defined(_DEBUG) || (LIBGRAPHICS_DEBUG_OUTPUT == 1) || ( BLACKSILK_DISPLAY_STATS == 1)
    this->preview.shouldRenderStats     = true;
#else
    this->preview.shouldRenderStats     = false;
#endif
}

App::~App() {
    ( void ) alphaPlane.reset();
}

bool App::hasImage() const {
    return preview.currentImageSize != 0;
}

void App::enableFrameTime( size_t maxFPS ) {
    this->m_FrameTimerEnabled   = true;
    this->m_MaxFPS              = maxFPS;

    this->m_FrameTimer.start();
}

void App::disableFrameTime() {
    this->m_FrameTimerEnabled   = false;
}

size_t&         App::maxFPS() {
    return this->m_MaxFPS;
}

const size_t&   App::maxFPS() const {
    return this->m_MaxFPS;
}

void App::initializeTaskListener(
    QStatusBar* statusBar
) {
    if( this->taskListener.empty() ) {
        this->taskListener.reset( new blacksilk::ResponsiveBackgroundTaskListener( statusBar ) );
    }
}

bool App::initialized() const {
    return this->m_Initialized;
}


bool loadPresetData( std::string& out, const std::string& presetName ) {
    static const std::string basePath   = ":/Presets/Presets/";
    const std::string fullPath          = basePath + presetName + ".bs";

    QFile file( fullPath.c_str() );
    const auto successfullyOpened = file.open(
                                        QFile::ReadOnly
                                    );

    if( !successfullyOpened ) {
        qDebug() << "Failed to load internal preset located at '" << fullPath.c_str() << "'.";
        return false;
    }

    assert( successfullyOpened );

    out.assign( file.readAll().data() );
    assert( !out.empty() );

    return true;
}

bool loadPresetHelper( libfoundation::app::ApplicationSession* session, const std::string& presetName ) {
    assert( session != nullptr );
    assert( !presetName.empty() );

    if( !session || presetName.empty() ) {
        return false;
    }

    /// load preset data
    std::string presetData;
    const auto successfullyLoaded = loadPresetData(
                                        presetData,
                                        presetName
                                    );
    LOGB_WARNING( !successfullyLoaded, presetName + " did not load" );
    assert( successfullyLoaded );

    /// parse preset data
    PresetValue root( blacksilk::EPresetValueType::Root );
    const auto successfullyParsed = PresetValue::parse(
                                        root,
                                        presetData
                                    );
    LOGB_WARNING( !successfullyParsed, presetName + " did not parse" );
    assert( successfullyParsed );

    if( !successfullyParsed ) {
        return false;
    }


    /// convert to filter preset collection
    libgraphics::FilterPresetCollection* presets = session->presets();
    const auto successfullyConverted = blacksilk::createFilterPresetCollection(
                                           presets,
                                           *presets,
                                           root
                                       );
    assert( successfullyConverted );

    if( !successfullyConverted ) {
        return false;
    }

    return true;
}

bool loadPreset( libfoundation::app::ApplicationSession* session, const std::string& presetName ) {
    const auto successfullyLoaded = loadPresetHelper(
                                        session,
                                        presetName
                                    );
    assert( successfullyLoaded );

    if( !successfullyLoaded ) {
        qDebug() << "Failed to load internal preset with name '" << presetName.c_str() << "'.";
        return false;
    }

    return true;
}


bool loadPresetDataFromFile( std::string& out, const std::string& fullPath ) {
    QFile file( fullPath.c_str() );
    const auto successfullyOpened = file.open(
                                        QFile::ReadOnly
                                    );

    if( !successfullyOpened ) {
        qDebug() << "Failed to load preset located at '" << fullPath.c_str() << "'.";
        return false;
    }

    assert( successfullyOpened );

    out.assign( file.readAll().data() );
    assert( !out.empty() );

    return true;
}

bool setPresetByIndex( libfoundation::app::ApplicationSession* session, const libgraphics::FilterPresetCollection& presets, libfoundation::app::EFilter::t filterType, size_t index ) {
    assert( session != nullptr );
    assert( filterType != libfoundation::app::EFilter::OtherFilter );

    if( ( session == nullptr ) || ( filterType == libfoundation::app::EFilter::OtherFilter ) ) {
        qDebug() << "Failed to set preset: Invalid filter type.";
        return false;
    }

    const std::string& filterName( libfoundation::app::EFilter::toName( filterType ) );
    libgraphics::FilterPresetCollection filterPresetCollection = presets.collectionForFilter( filterName );

    if( filterPresetCollection.count() > index ) {
        const libgraphics::FilterPreset& selectedPreset( ( *( filterPresetCollection.constBegin() + index ) ).preset );
        auto filterObj = session->filters()->byName( filterName );

        assert( filterObj.valid() );

        if( !filterObj.valid() ) {
            return false;
        }

        const auto successfullySetPreset = filterObj->fromPreset( selectedPreset );
        assert( successfullySetPreset );

        if( successfullySetPreset ) {
            qDebug() << "Successfully applied preset '" << selectedPreset.name().c_str() << "' to " << filterName.c_str();
        } else {
            qDebug() << "Failed to apply preset '" << selectedPreset.name().c_str() << "' to " << filterName.c_str();
        }

        return successfullySetPreset;
    }

    return false;
}

bool App::usePresets(
    const libgraphics::FilterPresetCollection& collection
) {
    assert( this->currentSession != nullptr );
    assert( this->initialized() );

    if( !this->initialized() || ( collection.count() == 0 ) )  {
        return false;
    }

    // do not reapply, if we already are in this state
    libgraphics::FilterPresetCollection oldState = theApp()->currentSession->currentActiveStateToPresetCollection();

    if( oldState == collection ) {
        return true;
    }

    bool appliedAPreset( false );

    static const libfoundation::app::EFilter::t    filterTypes[] = {
        libfoundation::app::EFilter::BlackAndWhiteAdaptiveMixerFilter,
        libfoundation::app::EFilter::CurvesFilter,
        libfoundation::app::EFilter::FilmGrainFilter,
        libfoundation::app::EFilter::SplitToneFilter,
        libfoundation::app::EFilter::CascadedSharpenFilter,
        libfoundation::app::EFilter::VignetteFilter
    };
    static const size_t filterTypesLen = sizeof( filterTypes ) / sizeof( libfoundation::app::EFilter::t );

    for( size_t i = 0; filterTypesLen > i; ++i ) {
        if( setPresetByIndex( currentSession, collection, filterTypes[i], 0 ) ) {
            appliedAPreset = true;
            currentSession->enableFilter(
                filterTypes[i]
            );
        } else {
            currentSession->disableFilter(
                filterTypes[i]
            );
        }
    }

    return appliedAPreset;
}

bool App::loadPresetFromPath(
    const std::string& path
) {
    assert( this->currentSession != nullptr );
    assert( this->initialized() );
    assert( !path.empty() );

    if( !this->initialized() ) {
        return false;
    }

    /// load preset data
    std::string presetData;
    const auto successfullyLoaded = loadPresetDataFromFile(
                                        presetData,
                                        path
                                    );
    LOGB_WARNING( !successfullyLoaded, path + " did not load" );
    assert( successfullyLoaded );

    /// parse preset data
    PresetValue root( blacksilk::EPresetValueType::Root );
    const auto successfullyParsed = PresetValue::parse(
                                        root,
                                        presetData
                                    );
    assert( successfullyParsed );

    if( !successfullyParsed ) {
        return false;
    }

    /// convert to filter preset collection
    libgraphics::FilterPresetCollection presets;
    const auto successfullyConverted = blacksilk::createFilterPresetCollection(
                                           currentSession->presets(),
                                           presets,
                                           root
                                       );
    assert( successfullyConverted );

    if( !successfullyConverted ) {
        return false;
    }

    return usePresets( presets );
}

bool App::savePresetToPath(
    const libgraphics::FilterPresetCollection& collection,
    const std::string& path,
    const std::string& presetName
) {
    assert( this->currentSession != nullptr );
    assert( this->initialized() );
    assert( !path.empty() );

    if( !this->initialized() ) {
        return false;
    }

    QFile fileStream( path.c_str() );

    if( !fileStream.open( QFile::WriteOnly | QFile::Text ) ) {
        assert( false );
        qDebug() << "Failed to open preset file for writing: " << path.c_str();
        return false;
    }

    const std::string presetData = collection.toString( presetName );
    assert( !presetData.empty() );

    if( presetData.empty() ) {
        qDebug() << "Failed to export empty bs-format preset.";
        return false;
    }

    const size_t numberOfBytesWritten = ( size_t )fileStream.write(
                                            ( const char* )presetData.c_str(),
                                            sizeof( char ) * presetData.size()
                                        );
    assert( numberOfBytesWritten >= presetData.size() );

    if( presetData.size() < numberOfBytesWritten ) {
        qDebug() << "Failed to write preset to path. Wrong number of bytes written( path='" <<  path.c_str() << "' ).";
        return false;
    }

    return true;
}

bool App::loadInternalPresets() {
    if( this->currentSession == nullptr ) {
        assert( false );
        /** error: needs a valid session **/
        return false;
    }


    /// bwmixer filter
    if( !loadPreset( this->currentSession, libfoundation::app::EFilter::toName( libfoundation::app::EFilter::BlackAndWhiteAdaptiveMixerFilter ) ) ) {
        return false;
    }

    /// curves filter
    if( !loadPreset( this->currentSession, libfoundation::app::EFilter::toName( libfoundation::app::EFilter::CurvesFilter ) ) ) {
        return false;
    }

    /// filmgrain filter
    if( !loadPreset( this->currentSession, libfoundation::app::EFilter::toName( libfoundation::app::EFilter::FilmGrainFilter ) ) ) {
        return false;
    }

    /// splittone filter
    if( !loadPreset( this->currentSession, libfoundation::app::EFilter::toName( libfoundation::app::EFilter::SplitToneFilter ) ) ) {
        return false;
    }

    /// sharpen filter
    if( !loadPreset( this->currentSession, libfoundation::app::EFilter::toName( libfoundation::app::EFilter::CascadedSharpenFilter ) ) ) {
        return false;
    }

    /// sharpen filter
    if( !loadPreset( this->currentSession, libfoundation::app::EFilter::toName( libfoundation::app::EFilter::VignetteFilter ) ) ) {
        return false;
    }

    return true;
}

bool App::initializeGraphicsBackend() {
    if( this->m_Initialized && !this->m_InitializedGraphicsBackend ) {
        assert( currentSession != nullptr );

        if( this->appBackend == nullptr ) {
            this->appBackend = new libfoundation::app::ApplicationBackend();
        }

        if( !this->appBackend->initializeCpuBackend() ) {
            const auto successfullyInitializedBackends = this->appBackend->initializeCpuBackend();

            assert( successfullyInitializedBackends );

            if( !successfullyInitializedBackends ) {
                qDebug() << "Failed to initialize cpu-backend. Aborting...";
                return false;
            }
        }

        currentSession->updateBackend(
            this->appBackend
        );

        this->m_InitializedGraphicsBackend = true;

        return true;
    }

    return false;
}

bool App::initializeGraphicsPreview() {
    if( this->m_Initialized && !this->m_InitializedGraphicsPreview ) {
        assert( currentSession != nullptr );

        const auto glewInitialized = glewInit();

        if( glewInitialized != GLEW_NO_ERROR ) {
            qDebug() << "Error: Failed to initialize glew.";

            return false;
        }

        if( this->appBackend == nullptr ) {
            this->appBackend = new libfoundation::app::ApplicationBackend();
        }

        if( !this->appBackend->gpuInitialized() ) {
            const auto successfullyInitializedBackends = this->appBackend->initializeGpuBackend();

            assert( successfullyInitializedBackends );

            if( !successfullyInitializedBackends ) {
                qDebug() << "Failed to initialize cpu-backend. Aborting...";
                return false;
            }
        }

        currentSession->updateBackend(
            this->appBackend
        );
        currentSession->setPreviewBackend(
            this->appBackend->gpuBackend()
        );


        blacksilk::HostMachineRequirements requirements;
        {
            requirements.minimalCpuCores        = 1;
            requirements.minimalCpuFreq         = libcommon::metrics::gigahertz<libgraphics::UInt64>( 1 );
            requirements.minimalOpenGLVersion.release = 2;
            requirements.minimalOpenGLVersion.major   = 1;
            requirements.minimalRAMSize         = libcommon::metrics::gigabytes<libgraphics::UInt64>( 2 );
            requirements.minimalVRAMSize        = libcommon::metrics::megabytes<libgraphics::UInt64>( 512 );
        }

        qDebug() << "Printing system information...";
        blacksilk::printSystemInfo();

        blacksilk::printMachineRequirements( requirements );

        qDebug() << "Printing opengl extension information...";
        blacksilk::printOpenGLExtensionInfo();

        this->m_InitializedGraphicsPreview = true;

        return true;
    }

    return this->m_InitializedGraphicsPreview;
}

bool App::initialize(
    libfoundation::app::ApplicationConfig config
) {

    if( currentSession == nullptr ) {
        this->backgroundTasks.reset( new libfoundation::app::BackgroundTaskGroup() );

        /// reinitialize session
        if( this->app.get() == nullptr ) {
            this->app.reset( new libfoundation::app::Application() );
        }

        const auto successfullyInitializedApp = this->app->initialize(
                libfoundation::app::createSystemLayerForPlatform(),
                config
                                                );
        assert( successfullyInitializedApp );

        if( !successfullyInitializedApp ) {
            qDebug() << "Failed to initialize application object. Aborting...";
            return false;
        }

        currentSession = this->app->createAndActivateSession(
                             "default"
                         );

        /// load internal presets
        const auto successfullyLoadedPresets = loadInternalPresets();

        if( !successfullyLoadedPresets ) {
            qDebug() << "Failed to load presets from internal resources.";
        }

#if LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Dumping internal preset collection...";

        for( auto it = this->currentSession->presets()->begin(); it != this->currentSession->presets()->end(); ++it ) {
            qDebug() << "*********************";
            qDebug() << "Filter Name: " << ( *it ).preset.filterName().c_str();
            qDebug() << "Preset Name: " << ( *it ).preset.name().c_str();
        }

        qDebug() << "************* END OF DUMP *************";
#endif

        /// register sharpen
        {
            this->filterCascadedSharpen = new libgraphics::fx::filters::CascadedSharpen( nullptr );

            currentSession->addFilter(
                this->filterCascadedSharpen
            );
            ( void ) currentSession->associateFilterWithId(
                this->filterCascadedSharpen,
                libfoundation::app::EFilter::CascadedSharpenFilter
            );
        }

        /// register BWMixer
        {
            this->filterBWMixer = new libgraphics::fx::filters::BWAdaptiveMixer( nullptr );

            currentSession->addFilter(
                this->filterBWMixer
            );
            ( void ) currentSession->associateFilterWithId(
                this->filterBWMixer,
                libfoundation::app::EFilter::BlackAndWhiteAdaptiveMixerFilter
            );
            currentSession->setMandatoryFilter( libfoundation::app::EFilter::BlackAndWhiteAdaptiveMixerFilter );
        }

        /// register Curves
        {
            this->filterCurves = new libgraphics::fx::filters::Curves( nullptr );

            currentSession->addFilter(
                this->filterCurves
            );
            this->filterCurves->resetCurve();

            currentSession->associateFilterWithId(
                this->filterCurves,
                libfoundation::app::EFilter::CurvesFilter
            );
        }

        /// register vignette
        {
            this->filterVignette = new libgraphics::fx::filters::Vignette( nullptr );

            currentSession->addFilter(
                this->filterVignette
            );

            currentSession->associateFilterWithId(
                this->filterVignette,
                libfoundation::app::EFilter::VignetteFilter
            );
        }

        /// register filmgrain
        {
            this->filterFilmGrain = new libgraphics::fx::filters::FilmGrain( nullptr );

            currentSession->addFilter(
                this->filterFilmGrain
            );
            currentSession->associateFilterWithId(
                this->filterFilmGrain,
                libfoundation::app::EFilter::FilmGrainFilter
            );
        }

        /// register splittone
        {
            this->filterSplitTone = new libgraphics::fx::filters::SplitTone( nullptr );

            currentSession->addFilter(
                this->filterSplitTone
            );
            currentSession->associateFilterWithId(
                this->filterSplitTone,
                libfoundation::app::EFilter::SplitToneFilter
            );
        }

        if( config.loadStandardIoPlugins ) {
            /// initialize pipeline
            std::string appDir;
            std::tie( appDir, std::ignore ) = blacksilk::applicationPath();
            qDebug() << "DirPath: " << appDir.c_str();

#if LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_MAC
            const auto successfullyLoadedPlugin = this->app->loadIoPluginFromPath(
                    appDir + "/../Frameworks/libgraphics.io.plugins.imagemagick.dylib"
                                                  );
#elif LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_WINDOWS
            const auto successfullyLoadedPlugin = this->app->loadIoPluginFromPath(
                    appDir + "/libgraphics.io.plugins.imagemagick.dll"
                                                  );
#elif LIBCOMMON_SYSTEM == LIBCOMMON_SYSTEM_LINUX
            const auto successfullyLoadedPlugin = this->app->loadIoPluginFromPath(
                    appDir + "/libBlackSilk/libgraphics.io.plugins.imagemagick.so"
                                                  );
#endif

            assert( successfullyLoadedPlugin );

            if( !successfullyLoadedPlugin ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "Failed to load plugin from specified location.";
#endif
                return false;
            }
        }

        LIBGRAPHICS_MEMORY_LOG_PRINT
    }

    this->m_Initialized = true;

    return true;
}

bool App::shutdown() {
    LIBGRAPHICS_MEMORY_LOG_PRINT

    bool success( true );

    ( void ) this->alphaPlane.reset();

    const auto successfullyShutdown = this->app->shutdown();

    if( !successfullyShutdown ) {
        qDebug() << "Failed to shutdown application state.";
        success = false;
    }

    this->backgroundTasks.reset();
    this->app.reset();

    this->currentSession = nullptr;
    this->filterBWMixer = nullptr;
    this->filterCascadedSharpen = nullptr;
    this->filterCurves = nullptr;
    this->filterFilmGrain = nullptr;
    this->filterSplitTone = nullptr;
    this->filterVignette = nullptr;

    this->m_Initialized = false;
    this->m_InitializedGraphicsBackend = false;
    this->m_InitializedGraphicsPreview = false;

    return success;
}

void App::enqueBackgroundTask(
    libfoundation::app::BackgroundTask* task,
    libfoundation::app::BackgroundTaskListener* listener
) {
    assert( currentSession != nullptr );
    assert( task != nullptr );

    listener = ( listener == nullptr ) ? ( libfoundation::app::BackgroundTaskListener* )this->taskListener.get() : listener;

    this->backgroundTasks->enqueTask(
        task,
        currentSession,
        listener
    );
}

bool App::loadIoPluginFromPath(
    const std::string& path
) {
    return this->app->loadIoPluginFromPath(
               path
           );
}

bool App::loadIoImporterFromPath(
    const std::string& path
) {
    return this->app->loadIoImporterFromPath(
               path
           );
}

bool App::loadIoExporterFromPath(
    const std::string& path
) {
    return this->app->loadIoExporterFromPath(
               path
           );
}

bool App::writeImage(
    void* destinationBuffer
) {
    libcommon::ScopedPtr<libgraphics::ImageLayer> destinationLayer( currentSession->originalImage()->topLayer()->duplicate() );
    assert( destinationLayer.get() );

    const auto successfullyRendered = currentSession->renderToLayer(
                                          destinationLayer.get(),
                                          currentSession->backend()->cpuBackend()
                                      );

    if( !successfullyRendered ) {
        qDebug() << "Failed to render to layer. Aborting...";
        return false;
    }

    const auto successfullyRetrieved = destinationLayer->retrieve(
                                           destinationBuffer
                                       );

    if( !successfullyRetrieved ) {
        qDebug() << "Failed to copy rendered ImageLayer instance buffer to destination. Aborting...";
        return false;
    }

    return true;
}

float getScalingFactor( const float maxSize, const size_t width, const size_t height, size_t formatSize ) {
    float   mp( ( float )( formatSize * width * height ) / ( 1000.0f * 1000.0f ) );
    float   fac( 1.0f );

    while( mp > maxSize ) {
        mp   = ( ( width * fac ) * ( height * fac ) * ( float )formatSize ) / ( 1000 * 1000 );
        fac -= 0.015f;
    }

    return fac;
}

void App::postProcessOriginalImage() {
    if( theApp()->currentSession->originalImage() != nullptr ) {
        libgraphics::Image* originalImage = ( libgraphics::Image* )currentSession->originalImage();
        assert( originalImage != nullptr );

        const float origMP     = ( originalImage->width() * originalImage->height() ) / ( 1000.0f * 1000.0f );
        const auto formatSize   = libgraphics::fxapi::EPixelFormat::getPixelSize( originalImage->format() );
        const float currentSize =
            ( ( float )originalImage->width() * ( float )originalImage->height()  * ( float )formatSize ) / ( 1000.0f * 1000.0f );
        const float adjustedMaxSize = ( float )preview.maximalImageSize * ( float )preview.qualityFactor;

        const size_t defaultPlaneAllocationCount = 8;
        const size_t defaultPlaneSize            = libgraphics::fxapi::EPixelFormat::getPlaneByteSize(
                    originalImage->format(),
                    originalImage->width(),
                    originalImage->height()
                );
        this->appBackend->allocator()->reserve(
            defaultPlaneAllocationCount,
            defaultPlaneSize
        );

        if( std::ceil( adjustedMaxSize ) < currentSize ) {

            /// We need to scale down this image.
            const float scalingFactor = getScalingFactor( adjustedMaxSize, originalImage->width(), originalImage->height(), formatSize );

            libgraphics::ImageLayer* previewTemplate = originalImage->createAndAppendLayer(
                        this->currentSession->backend()->cpuBackend(),
                        "PreviewTemplate"
                    );
            assert( previewTemplate != nullptr );

            libgraphics::fx::operations::areaSample2x2(
                previewTemplate,
                originalImage->topLayer(),
                scalingFactor
            );
            preview.currentImageSize =
                std::ceil( ( float )( previewTemplate->width() * previewTemplate->height() * libgraphics::fxapi::EPixelFormat::getPixelSize( previewTemplate->format() ) ) / ( 1000 * 1000 ) );

            const float mpFactor      = ( float )preview.currentImageSize / ( float )preview.maximalImageSize;
            const float currentMP     = ( previewTemplate->width() * previewTemplate->height() ) / ( 1000.0f * 1000.0f );

            qDebug() << "PostProcess: Imported image@" << origMP << "MP scaled down to @" << currentMP << "MP(" << previewTemplate->width() << "x" << previewTemplate->height() << "), " << QString::number( mpFactor * 100.0f ) << "% Quality";

            preview.isScaledDown = true;

            return;
        }

        preview.isScaledDown = false;
        preview.currentImageSize =
            std::ceil( ( float )( originalImage->width() * originalImage->height() * libgraphics::fxapi::EPixelFormat::getPixelSize( originalImage->format() ) ) / ( 1000.0f * 1000.0f ) );
    } else {
        qDebug() << "Error: Failed to postprocess original image. Original image not set.";
    }
}

bool App::setupPreviewFromOriginalImage() {
    libgraphics::ImageLayer* templateLayer = this->currentSession->originalImage()->layerByName( "PreviewTemplate" );

    if( templateLayer == nullptr ) {
        templateLayer = this->currentSession->originalImage()->topLayer();
    }

    assert( templateLayer != nullptr );

    libgraphics::backend::cpu::ImageObject* cpuLayer = ( libgraphics::backend::cpu::ImageObject* )templateLayer->internalImageForBackend( FXAPI_BACKEND_CPU );
    assert( cpuLayer != nullptr );

    bool ok = setupPreview(
                  cpuLayer->data(),
                  cpuLayer->width(),
                  cpuLayer->height(),
                  cpuLayer->format()
              );
    return ok;
}

bool App::setupPreview(
    void* data,
    size_t width,
    size_t height,
    libgraphics::fxapi::EPixelFormat::t format
) {
    assert( data != nullptr );
    assert( width * height != 0 );
    assert( format != libgraphics::fxapi::EPixelFormat::Empty );

    if( data == nullptr ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to setup new session from invalid image buffer.";
#endif
        return false;
    }

    if( width * height == 0 ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to setup new session from image buffer with invalid size.";
#endif
        return false;
    }

    if( format == libgraphics::fxapi::EPixelFormat::Empty ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to setup new session from image buffer with invalid format.";
#endif
        return false;
    }

    /// reset preview renderer
    if( this->actionRenderPreview.get() != nullptr ) {
        this->actionRenderPreview.reset();
    }

    /// initialize current image state.
    {
        libgraphics::Image* previewImage = new libgraphics::Image(
            this->appBackend->gpuBackend(),
            format,
            width,
            height,
            ( void* )data
        );

        libgraphics::ImageLayer* topLayer = previewImage->topLayer();
        libgraphics::ImageLayer* destinationLayer = topLayer->duplicate();

        previewImage->appendLayer( destinationLayer );

        this->currentSession->resetImageState(
            previewImage,
            nullptr
        );

        this->view->setLayer( destinationLayer );
        this->view->setOriginalLayer( topLayer );
        this->view->setZoom( 0.5f );
        this->view->showOriginalImage() = true;

        auto arp = new libfoundation::app::ApplicationActionRenderPreview(
            this->app->currentSession(),
            this->appBackend->gpuBackend(),
            destinationLayer,
            topLayer,
            this->app->currentSession()->filterStack()
        );
        this->actionRenderPreview.reset( arp );
    }

    {
        this->filterFilmGrain->resetCurve();
        this->filterFilmGrain->resetGrain();
        this->filterCascadedSharpen->deleteBlurBuffersForBackend( FXAPI_BACKEND_OPENGL );
        this->filterCascadedSharpen->deleteBlurBuffersForBackend( FXAPI_BACKEND_CPU );
    }


    /// reset sharpen filter
    {
        this->filterCascadedSharpen->setCascadeCount( 4 );

        this->filterCascadedSharpen->setCascadeBlurRadius( 0, 0.7f );
        this->filterCascadedSharpen->setCascadeStrength( 0, 1.0f );

        this->filterCascadedSharpen->setCascadeBlurRadius( 1, 1.4f );
        this->filterCascadedSharpen->setCascadeStrength( 1, 1.0f );

        this->filterCascadedSharpen->setCascadeBlurRadius( 2, 2.8f ); // 2.8f
        this->filterCascadedSharpen->setCascadeStrength( 2, 1.0f );

        this->filterCascadedSharpen->setCascadeBlurRadius( 3, 5.6f );
        this->filterCascadedSharpen->setCascadeStrength( 3, 1.0f );

        this->filterCascadedSharpen->updateCascades();
    }

    /// reset vignette filter
    {
        this->filterVignette->setRadius( 1.0f );
        this->filterVignette->setStrength( 1.0f );
        this->filterVignette->setCenter( libgraphics::Point32F( 0.5f, 0.5f ) );
    }

    /// reset state-driven filters
    {
        this->filterFilmGrain->deleteGrainForBackend( FXAPI_BACKEND_CPU );
        this->filterCascadedSharpen->deleteBlurBuffersForBackend( FXAPI_BACKEND_CPU );
    }


    {
        this->currentSession->updateAllFilters();
        this->currentSession->disableAllFilters();

        this->shouldRender = true;
    }

    this->appBackend->cleanUp();

    LIBGRAPHICS_MEMORY_LOG_PRINT

    return true;
}

void App::triggerRendering() {
    this->shouldRender = true;
}


bool App::updatePreview() {
    if( this->shouldRender ) {
        qint64 elapsedMilliseconds = this->m_FrameTimer.elapsed();
        qint64 minimalMilliseconds = 1000 / this->m_MaxFPS;

        if( this->m_FrameTimerEnabled ) {
            if( elapsedMilliseconds < minimalMilliseconds ) {
                return false;
            }
        }

        this->shouldRender = false;

        if( this->actionRenderPreview.empty() ) {
            assert( false );
            this->m_FrameTimer.restart();
            return false; /** no initialized preview object **/
        }

        if( !this->actionRenderPreview->finished() ) {
            assert( false );
            this->m_FrameTimer.restart();
            return false; /** preview renderer currently working **/
        }

        const auto successfullyRenderedPreview = this->actionRenderPreview->process();
        assert( successfullyRenderedPreview );

        if( !successfullyRenderedPreview ) {
            qDebug() << "Error: Failed to render preview. ";
            this->m_FrameTimer.restart();
            return false;
        }


        const auto successfullyComittedPreview = this->actionRenderPreview->commit();
        assert( successfullyComittedPreview );

        if( !successfullyComittedPreview ) {
            qDebug() << "Error: Failed to commit rendered preview. ";
            this->m_FrameTimer.restart();
            return false;
        }

        this->m_FrameTimer.restart();
        return true;
    }

    return true;
}

bool App::openImageFromData(
    void* _data,
    libgraphics::fxapi::EPixelFormat::t _format,
    size_t _width,
    size_t _height
) {
    assert( _data != nullptr );
    assert( _width * _height != 0 );
    assert( _format != libgraphics::fxapi::EPixelFormat::Empty );

    const auto successfullyLoaded = currentSession->importImageFromData(
                                        _data,
                                        _format,
                                        _width,
                                        _height
                                    );
    assert( successfullyLoaded );

    if( !successfullyLoaded ) {
        return false;
    }

    postImageLoad();

    return true;
}

bool App::openImage(
    const std::string& path
) {
    const auto successfullyLoaded = currentSession->importImageFromPath(
                                        path
                                    );
    assert( successfullyLoaded );
    ( void )successfullyLoaded;

    if( !successfullyLoaded ) {
        return false;
    }

    postImageLoad();

    return true;
}

void App::postImageLoad() {
    auto originalImage      = currentSession->originalImage();
    LOGB_RETURN( !originalImage, "Invalid image", void() );

    const auto origFormat   = originalImage->format();
    const auto pixelSize    = libgraphics::fxapi::EPixelFormat::getPixelSize( origFormat );
    const auto channelCount = libgraphics::fxapi::EPixelFormat::getChannelCount( origFormat );

    QString dimStr          = QString::number( originalImage->width() ) + "x" + QString::number( originalImage->height() ) + "@" + QString::number( pixelSize ) + "bpp,"
                              + QString::number( channelCount ) + " channels";
    QString formatStr( libgraphics::fxapi::EPixelFormat::toString( origFormat ) );
    QString mpStr = QString::number( ( float )( originalImage->width() * originalImage->height() ) / ( float )( 1000 * 1000 ) );
    QString dataStr = QString::number( ( originalImage->width() * originalImage->height() * pixelSize ) / ( 1024 * 1024 ) ) + "MB";

    qDebug() << "Imported image properties:";
    qDebug() << "\t     - format:       " << formatStr.toStdString().c_str();
    qDebug() << "\t     - dimensions:   " << dimStr.toStdString().c_str();
    qDebug() << "\t     - path:         " << currentSession->imagePath().c_str();
    qDebug() << "\t     - mp:           " << mpStr.toStdString().c_str();
    qDebug() << "\t     - buffer-size:  " << dataStr.toStdString().c_str();

    qDebug() << "Post processing imported image; preparing for preview...";
    postProcessOriginalImage();

    const auto unused = appBackend->allocator()->releaseUnused();

    qDebug() << "Cleaned up" << unused << " allocator entries.";

    LIBGRAPHICS_MEMORY_LOG_PRINT;
}

bool App::saveImageToBuffer( void* buffer ) {
    if( preview.isScaledDown ) {
        filterCascadedSharpen->updateCascades();
    }

    if( this->cachedAlphaPlane ) {
        qDebug() << "Cached alpha plane was detected. The alpha channel will be re-attached to the original image during export.";
    }

#if BLACKSILK_FORCE_STREAMLINED_RENDERING
    const auto successfullyWritten = theApp()->currentSession->exportImage(
                                         buffer,
                                         false,
                                         ( this->cachedAlphaPlane ? &this->alphaPlane : nullptr ),
                                         false
                                     );
#else
    const auto successfullyWritten = theApp()->currentSession->exportImage(
                                         buffer,
                                         !preview.isScaledDown,
                                         ( this->cachedAlphaPlane ? &this->alphaPlane : nullptr )
                                     );
#endif

    assert( successfullyWritten );

    if( !successfullyWritten ) {
        qDebug() << "Failed to write image to buffer.";
        return false;
    }

    return true;
}

bool App::saveImage(
    libfoundation::app::EImageFormat::t format,
    const std::string& path
) {
    if( preview.isScaledDown ) {
        filterCascadedSharpen->updateCascades();
    }

    if( this->cachedAlphaPlane ) {
        qDebug() << "Cached alpha plane was detected. The alpha channel will be re-attached to the original image during export.";
    }

#if BLACKSILK_FORCE_STREAMLINED_RENDERING
    const auto successfullyWritten = theApp()->currentSession->exportImage(
                                         path,
                                         format,
                                         false,
                                         ( this->cachedAlphaPlane ? &this->alphaPlane : nullptr ),
                                         false
                                     );
#else
    const auto successfullyWritten = theApp()->currentSession->exportImage(
                                         path,
                                         format,
                                         !preview.isScaledDown,
                                         ( this->cachedAlphaPlane ? &this->alphaPlane : nullptr )
                                     );
#endif

    assert( successfullyWritten );

    if( !successfullyWritten ) {
        qDebug() << "Failed to write image to path: " << path.c_str();
        return false;
    }

    return true;
}

std::string         App::retrieveCurrentLog() {
    const auto logFileName = logging::logFilename();
    assert( !logFileName.empty() );
    std::ifstream logFile( logFileName.c_str(), std::ifstream::in );
    assert( logFile.is_open() );
    return std::string( ( std::istreambuf_iterator<char>( logFile ) ), ( std::istreambuf_iterator<char>() ) );
}

static libcommon::ScopedPtr<App>    appObject;
App* theApp() {
    if( appObject.empty() ) {
        appObject.reset( new App() );
    }

    return appObject.get();
}
void resetApp() {
    if( !appObject.empty() ) {
        appObject.reset( nullptr );
    }
}

void initializeApplication() {
    assert( theApp() != nullptr );

    if( !theApp()->initialized() ) {
        libfoundation::app::ApplicationConfig   config;
#ifdef BLACKSILK_STANDALONE
        config.loadStandardIoPlugins = true;
#else
        config.loadStandardIoPlugins = false;
#endif
        const auto successfullyInitialized = theApp()->initialize(
                config
                                             );
        assert( successfullyInitialized != false );

        if( !successfullyInitialized ) {
            qDebug() << "Failed to initialize application state.";
        }

        const auto successfullyInitializedBackend = theApp()->initializeGraphicsBackend();
        assert( successfullyInitializedBackend );

        if( !successfullyInitializedBackend ) {
            qDebug() << "Failed to initialize graphics backend.";
        }
    }
}

void shutdownApplication() {
    assert( theApp() != nullptr );

    if( theApp()->initialized() ) {
        const auto successfullyShutdown = theApp()->shutdown();
        assert( successfullyShutdown );

        if( !successfullyShutdown ) {
            qDebug() << "Failed to shutdown application.";
        }
    }
}


}
