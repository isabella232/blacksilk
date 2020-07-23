#pragma once

#include <libcommon/mutex.hpp>
#include <libcommon/lockable.hpp>
#include <libcommon/noncopyable.hpp>
#include <libcommon/scopedptr.hpp>
#include <string>
#include <vector>

#include <libgraphics/fxapi.hpp>
#include <libgraphics/io/pipeline.hpp>
#include <libserialization++.hpp>
#include <libgraphics/fxapi.hpp>

/// extern libgraphics-classes
namespace libgraphics {
class Image;
class ImageLayer;
class Bitmap;
namespace fxapi {
class ApiBackendDevice;
}
class Filter;
class FilterCollection;
class FilterPreset;
class FilterPresetCollection;
class FilterPlugin;
class FilterStack;

namespace io {
class Pipeline;
class PipelineExporter;
class PipelineImporter;
class PipelineInfo;
}
}
/// extern Qt classes
class QRunnable;

namespace libfoundation {
namespace app {

/// classes
struct ApplicationConfig;
class ApplicationHardwareInfo;
class ApplicationSystemLayer;
class ApplicationSession;
class ApplicationBackend;

class ApplicationAction;
class SessionCommitableApplicationAction;

class ApplicationActionRenderPreview;
class ApplicationActionExport;
class ApplicationActionImport;
class ApplicationActionSerializeSession;

/// enums
struct EFilter {
    enum t {
        BlackAndWhiteMixerFilter,
        BlackAndWhiteAdaptiveMixerFilter,
        CascadedSharpenFilter,
        ChannelCurvesFilter,
        VignetteFilter,
        OpacityFilter,
        CurvesFilter,
        UnsharpMaskFilter,
        SplitToneFilter,
        FilmGrainFilter,
        OtherFilter
    };
    static const std::string    toName( EFilter::t type ) {
        switch( type ) {
            case EFilter::BlackAndWhiteAdaptiveMixerFilter:
                return "BWAdaptiveMixer";

            case EFilter::BlackAndWhiteMixerFilter:
                return "BWMixer";

            case EFilter::CascadedSharpenFilter:
                return "CascadedSharpen";

            case EFilter::CurvesFilter:
                return "Curves";

            case EFilter::ChannelCurvesFilter:
                return "ChannelCurves";

            case EFilter::VignetteFilter:
                return "Vignette";

            case EFilter::OpacityFilter:
                return "Opacity";

            case EFilter::UnsharpMaskFilter:
                return "UnsharpMask";

            case EFilter::SplitToneFilter:
                return "SplitTone";

            case EFilter::FilmGrainFilter:
                return "FilmGrain";

            case EFilter::OtherFilter:
            default:
                return "OtherFilter";
        }
    }
    static EFilter::t   fromName( const std::string& name ) {
        if( toName( EFilter::BlackAndWhiteAdaptiveMixerFilter ) == name ) {
            return EFilter::BlackAndWhiteAdaptiveMixerFilter;
        }

        if( toName( EFilter::BlackAndWhiteMixerFilter ) == name ) {
            return EFilter::BlackAndWhiteMixerFilter;
        }

        if( toName( EFilter::CascadedSharpenFilter ) == name ) {
            return EFilter::CascadedSharpenFilter;
        }

        if( toName( EFilter::ChannelCurvesFilter ) == name ) {
            return EFilter::ChannelCurvesFilter;
        }

        if( toName( EFilter::CurvesFilter ) == name ) {
            return EFilter::CurvesFilter;
        }

        if( toName( EFilter::VignetteFilter ) == name ) {
            return EFilter::VignetteFilter;
        }

        if( toName( EFilter::OpacityFilter ) == name ) {
            return EFilter::OpacityFilter;
        }

        if( toName( EFilter::UnsharpMaskFilter ) == name ) {
            return EFilter::UnsharpMaskFilter;
        }

        if( toName( EFilter::FilmGrainFilter ) == name ) {
            return EFilter::FilmGrainFilter;
        }

        if( toName( EFilter::SplitToneFilter ) == name ) {
            return EFilter::SplitToneFilter;
        }

        return EFilter::OtherFilter;
    }
};
struct EImageOrigin {
    enum t {
        FromFile,
        FromClipboard,
        FromWeb,
        Unknown
    };
};
struct EImageFormat {
    enum t {
        FDR,
        JPEG,
        PNG,
        BMP,
        TIF,
        Unknown
    };
    static const std::string toString( const EImageFormat::t format ) {
        switch( format ) {
            case EImageFormat::BMP:
                return LIBGRAPHICS_IO_FORMAT_BMP;

            case EImageFormat::JPEG:
                return LIBGRAPHICS_IO_FORMAT_JPEG;

            case EImageFormat::TIF:
                return LIBGRAPHICS_IO_FORMAT_TIF;

            case EImageFormat::PNG:
                return LIBGRAPHICS_IO_FORMAT_PNG;

            default:
                return "";
        }
    }
    static inline bool icompare( const std::string& first, const std::string& second ) {
        if( ( first.size() != second.size() ) || first.empty() ) {
            return false;
        }

        for( size_t i = 0; first.size() > i; ++i ) {
            if( ::tolower( first[i] ) != ::tolower( second[i] ) ) {
                return false;
            }
        }

        return true;
    }

    static EImageFormat::t formatFromExtension( std::string ext ) {
        if( ext.empty() ) {
            return EImageFormat::Unknown;
        }

        if( ext.front() == '.' ) {
            ext.erase( ext.begin() );
        }

        if( icompare( ext, "jpg" ) || icompare( ext, "jpeg" ) ) {
            return EImageFormat::JPEG;
        }

        if( icompare( ext, "tif" ) || icompare( ext, "tiff" ) ) {
            return EImageFormat::TIF;
        }

        if( icompare( ext, "png" ) ) {
            return EImageFormat::PNG;
        }

        if( icompare( ext, "bmp" ) || icompare( ext, "dib" ) ) {
            return EImageFormat::BMP;
        }

        if( icompare( ext, "fdr" ) ) {
            return EImageFormat::FDR;
        }

        return EImageFormat::Unknown;
    }
};


/// base class for application actions
class ApplicationAction : public libcommon::INonCopyable {
    public:
        virtual ~ApplicationAction() {}

        virtual bool process() = 0;
        virtual bool finished() = 0;

        void waitForFinished();
    protected:
        libcommon::Mutex    m_FinishedMutex;
};

/// base class for session committable actions
class SessionCommitableApplicationAction : public ApplicationAction {
    public:
        SessionCommitableApplicationAction( ApplicationSession* session ) :
            m_Session( session ) {
            assert( m_Session );
        }

        virtual ~SessionCommitableApplicationAction() {}

        virtual bool commit() = 0;

        ApplicationSession* session() const {
            return m_Session;
        }
    private:
        ApplicationSession* m_Session;
};

/// render preview action
class ApplicationActionRenderPreview : public SessionCommitableApplicationAction {
    public:
        struct Private;

        ApplicationActionRenderPreview(
            ApplicationSession* session,
            libgraphics::fxapi::ApiBackendDevice* backendDevice,
            libgraphics::ImageLayer* destination,
            libgraphics::ImageLayer* source,
            libgraphics::FilterStack* stack
        );
        virtual ~ApplicationActionRenderPreview() {}

        libgraphics::ImageLayer* imageLayer() const;
        libgraphics::fxapi::ApiBackendDevice* device() const;
        libgraphics::FilterStack* filters() const;

        virtual bool commit();
        virtual bool process();
        virtual bool finished();
    protected:
        libcommon::PimplPtr<Private>   d;
};

/// import action
class ApplicationActionImport : public SessionCommitableApplicationAction {
    public:
        struct Private;

        ApplicationActionImport(
            ApplicationSession* session,
            libgraphics::fxapi::ApiBackendDevice* backendDevice,
            const std::string& path
        );
        virtual ~ApplicationActionImport() {}

        virtual bool commit();
        virtual bool process();
        virtual bool finished();
    protected:
        libcommon::PimplPtr<Private>   d;
};

/// export action
class ApplicationActionExport : public SessionCommitableApplicationAction {
    public:
        struct Private;

        ApplicationActionExport(
            ApplicationSession* session,
            const std::string& path, /// may be empty
            const EImageFormat::t& format, /// may be irrelevant(if path is empty)
            libgraphics::ImageLayer* imageLayer,
            bool alreadyRendered,
            bool useStreamlinedGpuRendering = false,
            libgraphics::Bitmap* seperateAlphaLayer = nullptr,
            void* outputBuffer = nullptr /// If not null, a copy of the image(in bitmap format) is copied to this buffer
        );
        virtual ~ApplicationActionExport() {}

        virtual bool commit();
        virtual bool process();
        virtual bool finished();
    protected:
        bool doStreamlinedRendering( const ApplicationBackend* currentSessionBackend, libgraphics::Bitmap& outBitmap );
        bool doCpuRendering( const ApplicationBackend* currentSessionBackend, libgraphics::Bitmap& outBitmap );
        bool applyAlphaChannel( const ApplicationBackend* currentSessionBackend, libgraphics::ImageLayer* destinationLayer, libgraphics::Bitmap* alphaLayer );

        libcommon::PimplPtr<Private>   d;
};

/// serialization action
class ApplicationActionSerializeSession : public SessionCommitableApplicationAction {
    public:
        struct Private;

        ApplicationActionSerializeSession(
            ApplicationSession* targetSession,
            const std::string& path
        );
        virtual ~ApplicationActionSerializeSession() {}

        virtual bool commit();
        virtual bool process();
        virtual bool finished();
};

/// manages the rendering backend
class ApplicationBackend : public libcommon::INonCopyable {
    public:
        struct Private;

        ApplicationBackend();
        virtual ~ApplicationBackend();

        /// setup
        bool initializeCpuBackend();
        bool initializeGpuBackend();

        bool shutdown();
        bool shutdownCpuBackend();
        bool shutdownGpuBackend();

        bool initialized() const;
        bool cpuInitialized() const;
        bool gpuInitialized() const;

        /// cleans up memory state and deletes unused memory
        /// sections.
        void    cleanUp();

        /// internal backend management
        libgraphics::fxapi::ApiBackendDevice* gpuBackend() const;
        libgraphics::fxapi::ApiBackendDevice* cpuBackend() const;

        /// allocator management
        libcommon::SharedPtr<libgraphics::StdDynamicPoolAllocator>& allocator();
        const libcommon::SharedPtr<libgraphics::StdDynamicPoolAllocator>& allocator() const;
    private:
        void initializeAllocators();

        libcommon::PimplPtr<Private>   d;
};

/// manages basic hardware info
class ApplicationHardwareInfo {
    public:
        struct Private;

    private:
};

/// basic application configuration
struct ApplicationConfig {
    bool    loadStandardIoPlugins;

    ApplicationConfig() : loadStandardIoPlugins( false ) {}
};

/// user session management
class ApplicationSession : public libcommon::INonCopyable,
    public libcommon::ILockable {
    public:
        struct Private;
        friend class ApplicationActionRenderPreview;

        /// constr.
        ApplicationSession( const std::string& name );
        virtual ~ApplicationSession() {}

        /// scheduling
        void setThreadCount( size_t count );
        size_t threadCount() const;
        void run( QRunnable* runnable );
        void waitForAll();

        /// properties
        libgraphics::io::Pipeline*          pipeline();
        const libgraphics::io::Pipeline*    pipeline() const;
        void setPipeline( libgraphics::io::Pipeline* pipelineObject );
        void setPipeline(const std::shared_ptr<libgraphics::io::Pipeline> &pipelineObject );

        const libgraphics::Image* previewImage() const;
        const libgraphics::Image* originalImage() const;

        const EImageOrigin::t imageOrigin() const;
        const std::string&  imagePath() const;
        const std::string&  sessionPath() const;

        const std::string& name() const;
        void setName( const std::string& name );

        void setSessionPath( const std::string& path );

        const ApplicationBackend* backend() const;
        void updateBackend( ApplicationBackend* backend );

        libgraphics::fxapi::ApiBackendDevice*   previewBackend() const;
        void setPreviewBackend( libgraphics::fxapi::ApiBackendDevice* previewBackend );


        /// synchronization and serialization
        void synchronize();
        bool trySynchronize();

        void writeSession( std::string _path = "" );
        ApplicationActionSerializeSession* asyncWriteSession( std::string _path = "" );

        void readSession();

        /// filter and preset management
        bool enableFilter(
            EFilter::t filter
        );
        bool enableFilter(
            libgraphics::Filter* filter
        );
        bool enableFilterWithName(
            const std::string& name
        );
        bool disableFilter(
            EFilter::t filter
        );
        bool disableFilter(
            libgraphics::Filter* filter
        );
        bool disableFilterWithName(
            const std::string& name
        );

        void setMandatoryFilter(
            EFilter::t filter
        );
        void setMandatoryFilter(
            libgraphics::Filter* filter
        );
        void unsetMandatoryFilter(
            EFilter::t filter
        );
        void unsetMandatoryFilter(
            libgraphics::Filter* filter
        );

        bool isFilterEnabled( libgraphics::Filter* filter );
        bool isFilterEnabled( EFilter::t filter );
        bool isFilterMandatory( libgraphics::Filter* filter );
        bool isFilterMandatory( EFilter::t filter );
        bool isFilterSelectedForRendering( libgraphics::Filter* filter );
        bool isFilterSelectedForRendering( EFilter::t filter );

        std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> getFilterAndPresetsbyFilterType( const libfoundation::app::EFilter::t& filterType );

        libgraphics::FilterStack* filterStack();
        const libgraphics::FilterStack* filterStack() const;
        libgraphics::FilterCollection* filters();
        const libgraphics::FilterCollection* filters() const;
        bool addFilter(
            libgraphics::Filter* filter
        );
        bool associateFilterWithId(
            libgraphics::Filter* filter,
            EFilter::t type
        );
        bool addFiltersFromCollection(
            libgraphics::FilterCollection* collection
        );
        bool addFiltersFromPlugin(
            libgraphics::FilterPlugin* filterPlugin
        );
        bool loadFiltersFromLibrary(
            const std::string& path
        );

        bool removeFilter( libgraphics::Filter* filter );
        bool removeFilterWithName( const std::string& name );

        void updateAllFilters();
        void disableAllFilters();
        void enableAllFilters();

        libgraphics::FilterPreset* loadPresetFromPath(
            const std::string& path
        );

        void resetPresets();

        void clearPresets();

        libgraphics::FilterPresetCollection* presets();

        libgraphics::FilterPreset* presetByName(
            const std::string& name
        );
        libgraphics::FilterPresetCollection presetsForFilter(
            EFilter::t filter
        );
        libgraphics::FilterPresetCollection presetsForFilter(
            libgraphics::Filter* filter
        );
        libgraphics::FilterPresetCollection presetsForFilterWithName(
            const std::string& name
        );

        /// rendering and io management
        bool importImageFromData(
            void*                               _data,
            libgraphics::fxapi::EPixelFormat::t _format,
            size_t                              _width,
            size_t                              _height
        );
        bool importImageFromPath(
            const std::string& path
        );
        bool importImageFromPath(
            EImageFormat::t format,
            const std::string& path
        );
        ApplicationActionImport* asyncImportImage(
            const std::string& path
        );
        ApplicationActionImport* asyncImportImage(
            EImageFormat::t format,
            const std::string& path
        );

        bool exportImage(
            void* buffer,
            bool fromPreview = false,
            libgraphics::Bitmap* seperateAlphaLayer = nullptr,
            bool forceCpuRendering = false
        );
        bool exportImage(
            const std::string& path,
            EImageFormat::t format,
            bool fromPreview = false,
            libgraphics::Bitmap* seperateAlphaLayer = nullptr,
            bool forceCpuRendering = false
        );
        ApplicationActionExport* asyncExportImage(
            const std::string& path,
            EImageFormat::t format
        );

        bool updatePreview( bool _force = false );
        ApplicationActionRenderPreview* asyncUpdatePreview( bool _force = false );

        ApplicationActionRenderPreview* asyncRenderToLayer(
            libgraphics::ImageLayer* destination,
            bool usingCpu
        );
        bool renderToLayer(
            libgraphics::ImageLayer* destination,
            libgraphics::fxapi::ApiBackendDevice* backendDevice
        );
        bool renderToBitmap(
            libgraphics::Bitmap* destination,
            libgraphics::fxapi::ApiBackendDevice* backendDevice
        );

        /// state management
        void reset();
        void resetImageState();
        void resetImageState(
            libgraphics::Image* preview,
            libgraphics::Image* original,
            const std::string& path
        );
        void resetImageState(
            libgraphics::Image* preview,
            libgraphics::Image* original
        );

        libgraphics::FilterPresetCollection currentStateToPresetCollection();
        libgraphics::FilterPresetCollection currentActiveStateToPresetCollection();

        ApplicationSession* clone();

        /// from ILockable
        virtual void    lock();
        virtual bool    tryLock();
        virtual void    unlock();

    private:
        libcommon::PimplPtr<Private>   d;
};

/// base system layer
class ApplicationSystemLayer : public libcommon::INonCopyable {
    public:
        virtual ~ApplicationSystemLayer() {}

        std::string getRandomFileName( size_t length = 12 ) const;
        std::string getRandomFileNameWithExtension( const std::string& ext, size_t length = 12 ) const;

        virtual std::string getTemporaryPath() = 0;
        virtual std::string getApplicationPath() = 0;
        virtual std::string getSessionPath() = 0;

        virtual bool deleteFile( const std::string& path ) = 0;
        virtual bool createFile( const std::string& path ) = 0;
};
ApplicationSystemLayer* createSystemLayerForPlatform();

/// base management class for imaging
/// applications
class Application : public libcommon::INonCopyable {
    public:
        struct Private;

        Application();
        virtual ~Application() {}

        /// initialization
        bool initialize(
            ApplicationSystemLayer* layer,
            const std::string& configurationPath
        );
        bool initialize(
            ApplicationSystemLayer* layer,
            ApplicationConfig config
        );
        bool shutdown();

        /// global configuration handling
        bool containsValidHardwareInfo() const;
        bool updateHardwareInfo();

        bool isAbleToImportFormat(
            const std::string& formatDesc
        );
        bool isAbleToExportFormat(
            const std::string& formatDesc
        );

        const ApplicationConfig& applicationConfig() const;
        const ApplicationHardwareInfo* hardwareInfo() const;

        /// import/export plugin management
        bool loadIoPluginFromPath(
            const std::string& path
        );
        bool loadIoImporterFromPath(
            const std::string& path
        );
        bool loadIoExporterFromPath(
            const std::string& path
        );
        void unloadAllIoExporters();
        void unloadAllIoImporters();

        /// session management
        ApplicationSession* currentSession();
        ApplicationSession* sessionByName( const std::string& name );
        ApplicationSession* createSession( const std::string& name );
        ApplicationSession* createAndActivateSession( const std::string& name );

        bool makeCurrentSession( const std::string& name );

    protected:
        libcommon::PimplPtr<Private> d;
};


}
}
