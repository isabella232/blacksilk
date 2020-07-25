
#include <libfoundation/app/application.hpp>
#include <libgraphics/image.hpp>
#include <libgraphics/filterplugin.hpp>
#include <libgraphics/filterpluginloader.hpp>
#include <libgraphics/filter.hpp>
#include <libgraphics/filtercollection.hpp>
#include <libgraphics/filterstack.hpp>
#include <libgraphics/filterpreset.hpp>
#include <libgraphics/filterpresetcollection.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/backend/gl/gl_backenddevice.hpp>
#include <libgraphics/backend/cpu/cpu_backenddevice.hpp>
#include <libgraphics/backend/cpu/cpu_imageobject.hpp>
#include <libcommon/fileutils.hpp>
#include <libgraphics/fx/operations/basic.hpp>
#include <libgraphics/fx/operations/complex.hpp>
#include <log/log.hpp>

#include <QFileInfo>
#include <QFile>
#include <QRunnable>
#include <QThread>
#include <QThreadPool>
#include <QDebug>
#include <QProcess>
#include <QCoreApplication>

#include <cstdlib>

namespace libfoundation {
namespace app {

struct ActionRunnable : QRunnable {
        explicit ActionRunnable( libfoundation::app::ApplicationAction* applicationAction ) : m_Action( applicationAction ) {
            this->setAutoDelete( true );
        }
        virtual ~ActionRunnable() {}

        virtual void run() {
            const auto successfullyProcessed = m_Action->process();

            if( !successfullyProcessed ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
                qDebug() << "Failed to process action. Aborting...";
#endif
                assert( false );

                return;
            }
        }

    private:
        libfoundation::app::ApplicationAction* m_Action;
};

struct ApplicationSession::Private {
    std::shared_ptr<ApplicationBackend>             backend;
    std::shared_ptr<libgraphics::Image>             originalImage;
    std::shared_ptr<libgraphics::Image>             previewImage;

    libgraphics::fxapi::ApiBackendDevice*           previewBackend;

    std::shared_ptr<libgraphics::io::Pipeline>      pipeline;

    std::string     name;
    int             maxThreadCount;
    EImageOrigin::t imageOrigin;
    std::string     imagePath;
    std::string     sessionPath;

    struct FilterEntry {
        libgraphics::Filter*                filterObject;
        libfoundation::app::EFilter::t      filterType;
        bool                                mandatory;
        bool                                enabled;
        bool                                dirty;

        FilterEntry() : filterObject( nullptr ), filterType( libfoundation::app::EFilter::OtherFilter ),
            mandatory( false ), enabled( false ), dirty( false ) {}
    };
    std::vector<FilterEntry>        filterMetaInfo;

    libgraphics::FilterCollection   filterCollection;
    libgraphics::FilterStack        filterStack;
    libgraphics::FilterPresetCollection     filterPresetCollection;

    QThreadPool                     appThreadPool;

    std::recursive_mutex       mutex;

    Private() : maxThreadCount( 0 ), imageOrigin( EImageOrigin::Unknown ), previewBackend( nullptr ) {}

    bool isMandatoryFilter( libgraphics::Filter* filter ) const {
        if( filter == nullptr ) {
            return false;
        }

        return std::find_if(
                   filterMetaInfo.begin(),
                   filterMetaInfo.end(),
        [&]( const FilterEntry & rhs ) {
            return ( rhs.filterObject == filter ) && ( rhs.mandatory );
        }
               ) != filterMetaInfo.end();
    }
    bool isMandatoryFilter( libfoundation::app::EFilter::t filterType ) const {
        if( filterType == libfoundation::app::EFilter::OtherFilter ) {
            return false;
        }

        return std::find_if(
                   filterMetaInfo.begin(),
                   filterMetaInfo.end(),
        [&]( const FilterEntry & rhs ) {
            return ( rhs.filterType == filterType ) && ( rhs.mandatory );
        }
               ) != filterMetaInfo.end();
    }
    std::vector<FilterEntry>::iterator  filterByType( libfoundation::app::EFilter::t filterType ) {
        if( filterType == libfoundation::app::EFilter::OtherFilter ) {
            return this->filterMetaInfo.end();
        }

        return std::find_if(
                   filterMetaInfo.begin(),
                   filterMetaInfo.end(),
        [&]( const FilterEntry & rhs ) {
            return ( rhs.filterType == filterType );
        }
               );
    }
    std::vector<FilterEntry>::iterator  filterByPointer( libgraphics::Filter* filter ) {
        if( filter == nullptr ) {
            assert( false );
            return this->filterMetaInfo.end();
        }

        return std::find_if(
                   filterMetaInfo.begin(),
                   filterMetaInfo.end(),
        [&]( const FilterEntry & rhs ) {
            return ( rhs.filterObject == filter );
        }
               );
    }
    std::vector<FilterEntry>::iterator  filterByName( const std::string& name ) {
        return std::find_if(
                   filterMetaInfo.begin(),
                   filterMetaInfo.end(),
        [&]( const FilterEntry & rhs ) {
            return ( rhs.filterObject->name() == name );
        }
               );
    }
    bool containsFilter( libgraphics::Filter* filter ) const {
        if( filter == nullptr ) {
            return false;
        }

        return std::find_if(
                   filterMetaInfo.begin(),
                   filterMetaInfo.end(),
        [&]( const FilterEntry & rhs ) {
            return ( rhs.filterObject == filter );
        }
               ) != filterMetaInfo.end();
    }
    bool containsFilter( libfoundation::app::EFilter::t filterType ) const {
        if( filterType == libfoundation::app::EFilter::OtherFilter ) {
            return false;
        }

        return std::find_if(
                   filterMetaInfo.begin(),
                   filterMetaInfo.end(),
        [&]( const FilterEntry & rhs ) {
            return ( rhs.filterType == filterType );
        }
               ) != filterMetaInfo.end();
    }
    bool shouldRenderFilter( libfoundation::app::EFilter::t filterType ) const {
        if( filterType == libfoundation::app::EFilter::OtherFilter ) {
            return false;
        }

        return std::find_if(
                   filterMetaInfo.begin(),
                   filterMetaInfo.end(),
        [&]( const FilterEntry & rhs ) {
            return ( rhs.filterType == filterType ) && ( rhs.enabled || rhs.mandatory );
        }
               ) != filterMetaInfo.end();
    }
    bool shouldRenderFilter( libgraphics::Filter* filter ) const {
        if( filter == nullptr ) {
            return false;
        }

        return std::find_if(
                   filterMetaInfo.begin(),
                   filterMetaInfo.end(),
        [&]( const FilterEntry & rhs ) {
            return ( rhs.filterObject == filter ) && ( rhs.enabled || rhs.mandatory );
        }
               ) != filterMetaInfo.end();
    }
};


/// constr.
ApplicationSession::ApplicationSession( const std::string& name ) : d( new Private() ) {
    d->name = name;

    /// initialize runtime-stuff
    {
        static const size_t platformDefaultThreadCount = 4;

        this->setThreadCount( platformDefaultThreadCount );
    }

}

/// scheduling
void ApplicationSession::setThreadCount( size_t count ) {
    this->d->maxThreadCount = count;

    this->d->appThreadPool.setMaxThreadCount(
        this->d->maxThreadCount
    );
}

size_t ApplicationSession::threadCount() const {
    return d->maxThreadCount;
}

void ApplicationSession::run( QRunnable* runnable ) {
    assert( runnable );

    this->d->appThreadPool.start(
        runnable
    );
}

void ApplicationSession::waitForAll() {
    const auto result = this->d->appThreadPool.waitForDone();
    assert( result );
    LOGB_WARNING( !result, "ApplicationSession::waitForAll(): Failed to synchronize with running jobs." );
}

/// properties
const libgraphics::Image* ApplicationSession::previewImage() const {
    return d->previewImage.get();
}

const libgraphics::Image* ApplicationSession::originalImage() const {
    return d->originalImage.get();
}

const EImageOrigin::t ApplicationSession::imageOrigin() const {
    return d->imageOrigin;
}

const std::string&  ApplicationSession::imagePath() const {
    return d->imagePath;
}

const std::string&  ApplicationSession::sessionPath() const {
    return d->sessionPath;
}

const std::string& ApplicationSession::name() const {
    return d->name;
}

void ApplicationSession::setName( const std::string& name ) {
    this->d->name = name;
}

libgraphics::io::Pipeline*  ApplicationSession::pipeline() {
    return this->d->pipeline.get();
}


const libgraphics::io::Pipeline*  ApplicationSession::pipeline() const {
    return this->d->pipeline.get();
}

void ApplicationSession::setPipeline( libgraphics::io::Pipeline* pipelineObject ) {
    this->d->pipeline.reset( pipelineObject );
}

void ApplicationSession::setPipeline( const std::shared_ptr<libgraphics::io::Pipeline>& pipelineObject ) {
    this->d->pipeline = pipelineObject;
}

void ApplicationSession::setSessionPath( const std::string& path ) {
    this->d->sessionPath = path;

    this->writeSession();
}

const ApplicationBackend* ApplicationSession::backend() const {
    return d->backend.get();
}

void ApplicationSession::updateBackend( ApplicationBackend* backend ) {
    /// \todo swap images and make a clean update of all
    ///     resources. this is currently only a fast pointer
    ///     exchange.
    if( this->d->backend.get() != backend ) {
        this->d->backend.reset( backend );
    }
}

libgraphics::fxapi::ApiBackendDevice*   ApplicationSession::previewBackend() const {
    return this->d->previewBackend;
}

void ApplicationSession::setPreviewBackend( libgraphics::fxapi::ApiBackendDevice* previewBackend ) {
    this->d->previewBackend = previewBackend;
}

/// synchronization and serialization
void ApplicationSession::synchronize() {
    this->waitForAll();
}

bool ApplicationSession::trySynchronize() {
    return this->d->appThreadPool.waitForDone();
}

void ApplicationSession::writeSession( std::string _path ) {
    /** dummy **/
    assert( false );
}

ApplicationActionSerializeSession* ApplicationSession::asyncWriteSession( std::string _path ) {
    /** dummy **/
    assert( false );
    return nullptr;
}

void ApplicationSession::readSession() {
    /** dummy **/
    assert( false );
}

/// filter and preset management
bool ApplicationSession::enableFilter(
    EFilter::t filter
) {
    auto filterIt = this->d->filterByType(
                        filter
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return false;
    }

    ( *filterIt ).enabled = true;

    return true;
}

bool ApplicationSession::enableFilter(
    libgraphics::Filter* filter
) {
    auto filterIt = this->d->filterByPointer(
                        filter
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return false;
    }

    ( *filterIt ).enabled = true;

    return true;
}

bool ApplicationSession::enableFilterWithName(
    const std::string& name
) {
    auto filterIt = this->d->filterByName(
                        name
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return false;
    }

    ( *filterIt ).enabled = true;

    return true;
}

bool ApplicationSession::disableFilter(
    EFilter::t filter
) {
    auto filterIt = this->d->filterByType(
                        filter
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return false;
    }

    ( *filterIt ).enabled = false;

    return true;
}

bool ApplicationSession::disableFilter(
    libgraphics::Filter* filter
) {
    auto filterIt = this->d->filterByPointer(
                        filter
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return false;
    }

    ( *filterIt ).enabled = false;

    return true;
}

bool ApplicationSession::disableFilterWithName(
    const std::string& name
) {
    auto filterIt = this->d->filterByName(
                        name
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return false;
    }

    ( *filterIt ).enabled = false;

    return true;
}

void ApplicationSession::setMandatoryFilter(
    EFilter::t filter
) {
    auto filterIt = this->d->filterByType(
                        filter
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return;
    }

    ( *filterIt ).mandatory = true;
}

void ApplicationSession::setMandatoryFilter(
    libgraphics::Filter* filter
) {
    auto filterIt = this->d->filterByPointer(
                        filter
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return;
    }

    ( *filterIt ).mandatory = true;
}

void ApplicationSession::unsetMandatoryFilter(
    EFilter::t filter
) {
    auto filterIt = this->d->filterByType(
                        filter
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return;
    }

    ( *filterIt ).mandatory = false;
}

void ApplicationSession::unsetMandatoryFilter(
    libgraphics::Filter* filter
) {
    auto filterIt = this->d->filterByPointer(
                        filter
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return;
    }

    ( *filterIt ).mandatory = false;
}

libgraphics::FilterStack* ApplicationSession::filterStack() {
    return &this->d->filterStack;
}
const libgraphics::FilterStack* ApplicationSession::filterStack() const {
    return &this->d->filterStack;
}

libgraphics::FilterCollection* ApplicationSession::filters() {
    return &this->d->filterCollection;
}
const libgraphics::FilterCollection* ApplicationSession::filters() const {
    return &this->d->filterCollection;
}


bool ApplicationSession::addFilter(
    libgraphics::Filter* filter
) {
    if( this->d->containsFilter( filter ) ) {
        assert( false ); /** filter already part of session */
        return false;
    }

    std::shared_ptr<libgraphics::Filter>    filterPtr( filter );
    this->d->filterCollection.add(
        filterPtr
    );
    this->d->filterStack.pushBack(
        filterPtr
    );

    Private::FilterEntry entry;
    entry.enabled = false;
    entry.filterObject = filter;
    entry.filterType = libfoundation::app::EFilter::OtherFilter;
    entry.mandatory = false;

    this->d->filterMetaInfo.push_back( entry );

    return true;
}


bool ApplicationSession::isFilterSelectedForRendering( libgraphics::Filter* filter ) {
    return isFilterMandatory( filter ) || isFilterEnabled( filter );
}

bool ApplicationSession::isFilterSelectedForRendering( EFilter::t filter ) {
    return isFilterMandatory( filter ) || isFilterEnabled( filter );
}

std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> ApplicationSession::getFilterAndPresetsbyFilterType( const EFilter::t& filterType ) {

    const std::string filterName = libfoundation::app::EFilter::toName( filterType );

    libgraphics::Filter* filter = this->filters()->byName( filterName ).get();
    assert( filter != nullptr );

    libgraphics::FilterPresetCollection presets = this->presetsForFilter( filterType );

    return std::make_tuple( filter, presets );
}

bool ApplicationSession::isFilterMandatory( libgraphics::Filter* filter ) {
    auto filterIt = this->d->filterByPointer(
                        filter
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return false;
    }

    return ( *filterIt ).mandatory;
}

bool ApplicationSession::isFilterMandatory( EFilter::t filter ) {
    auto filterIt = this->d->filterByType(
                        filter
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return false;
    }

    return ( *filterIt ).mandatory;
}

bool ApplicationSession::isFilterEnabled( libgraphics::Filter* filter ) {
    auto filterIt = this->d->filterByPointer(
                        filter
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return false;
    }

    return ( *filterIt ).enabled;
}

bool ApplicationSession::isFilterEnabled( EFilter::t filter ) {
    auto filterIt = this->d->filterByType(
                        filter
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return false;
    }

    return ( *filterIt ).enabled;
}

bool ApplicationSession::associateFilterWithId(
    libgraphics::Filter* filter,
    EFilter::t type
) {
    auto filterIt = this->d->filterByPointer(
                        filter
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return false;
    }

    ( *filterIt ).filterType = type;

    return true;
}

bool ApplicationSession::addFiltersFromCollection(
    libgraphics::FilterCollection* collection
) {
    assert( collection != nullptr );

    if( !collection ) {
        return false;
    }

    for( auto it = collection->begin(); it != collection->end(); ++it ) {
        if( this->d->containsFilter( ( *it ).get() ) ) {
            assert( false ); /** filter already part of session */
            continue;
        }

        this->d->filterCollection.add(
            ( *it )
        );
        this->d->filterStack.pushBack(
            ( *it )
        );

        Private::FilterEntry entry;
        entry.enabled = false;
        entry.filterObject = ( *it ).get();
        entry.filterType = libfoundation::app::EFilter::OtherFilter;
        entry.mandatory = false;

        this->d->filterMetaInfo.push_back( entry );
    }

    return true;
}

bool ApplicationSession::addFiltersFromPlugin(
    libgraphics::FilterPlugin* filterPlugin
) {
    assert( filterPlugin != nullptr );

    if( !filterPlugin ) {
        return false;
    }

    libgraphics::FilterCollection temporaryPluginFilterCollection;

    if( filterPlugin->enumerateFilters( &temporaryPluginFilterCollection ) == 0 ) {
        assert( false ); /** failed to enumerate plugins from filter plugin **/
        return false;
    }

    return this->addFiltersFromCollection(
               &temporaryPluginFilterCollection
           );
}

bool ApplicationSession::loadFiltersFromLibrary(
    const std::string& path
) {
    auto pluginLibrary = libgraphics::loadFilterPluginFromLibrary(
                             path
                         );

    assert( pluginLibrary != nullptr );

    if( pluginLibrary == nullptr ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationSession::loadFiltersFromLibrary(): Failed to load filters from library. Aborting...";
#endif
        return false;
    }

    const auto result = this->addFiltersFromPlugin(
                            pluginLibrary
                        );
    assert( result );

    delete pluginLibrary;

    return result;
}

bool ApplicationSession::removeFilter( libgraphics::Filter* filter ) {
    assert( filter != nullptr );

    if( !filter ) {
        return false;
    }

    auto filterIt = this->d->filterByPointer(
                        filter
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return false;
    }

    this->d->filterMetaInfo.erase( filterIt );
    this->d->filterStack.remove( filter );
    this->d->filterCollection.remove( filter );

    return true;
}

bool ApplicationSession::removeFilterWithName( const std::string& name ) {
    auto filterIt = this->d->filterByName(
                        name
                    );

    if( filterIt == this->d->filterMetaInfo.end() ) {
        assert( false );
        return false;
    }

    this->d->filterMetaInfo.erase( filterIt );
    this->d->filterStack.remove( ( *filterIt ).filterObject );
    this->d->filterCollection.remove( ( *filterIt ).filterObject );

    return true;
}

void ApplicationSession::updateAllFilters() {
    for( auto it = this->d->filterMetaInfo.begin(); it != this->d->filterMetaInfo.end(); ++it ) {
        ( *it ).dirty = true;
    }
}

void ApplicationSession::disableAllFilters() {
    for( auto it = this->d->filterMetaInfo.begin(); it != this->d->filterMetaInfo.end(); ++it ) {
        ( *it ).enabled = false;
    }
}

void ApplicationSession::enableAllFilters() {
    for( auto it = this->d->filterMetaInfo.begin(); it != this->d->filterMetaInfo.end(); ++it ) {
        ( *it ).enabled = true;
    }
}

libgraphics::FilterPreset* ApplicationSession::loadPresetFromPath(
    const std::string& path
) {
    const auto successfullyLoaded = this->d->filterPresetCollection.loadPresetFromFile(
                                        path
                                    );
    assert( successfullyLoaded );

    if( !successfullyLoaded ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationSession::loadPresetFromPath(): Failed to load preset from path '" << path.c_str() << "'. Aborting...";
#endif
        return nullptr;
    }

    auto& presetRef = this->d->filterPresetCollection.back();

    return &presetRef;
}

void ApplicationSession::resetPresets() {
    for( auto it = this->d->filterPresetCollection.begin(); it != this->d->filterPresetCollection.end(); ++it ) {
        ( *it ).preset.clear();
    }
}

void ApplicationSession::clearPresets() {
    this->d->filterPresetCollection.clear();
}

libgraphics::FilterPresetCollection* ApplicationSession::presets() {
    return &d->filterPresetCollection;
}

libgraphics::FilterPreset* ApplicationSession::presetByName(
    const std::string& name
) {
    for( auto it = this->d->filterPresetCollection.begin(); it != this->d->filterPresetCollection.end(); ++it ) {
        if( ( *it ).preset.name() == name ) {
            return &( *it ).preset;
        }
    }

    return nullptr;
}

libgraphics::FilterPresetCollection ApplicationSession::presetsForFilter(
    EFilter::t filter
) {
    const auto filterName = libfoundation::app::EFilter::toName(
                                filter
                            );
    return this->d->filterPresetCollection.collectionForFilter(
               filterName
           );
}

libgraphics::FilterPresetCollection ApplicationSession::presetsForFilter(
    libgraphics::Filter* filter
) {
    assert( filter != nullptr );
    return this->d->filterPresetCollection.collectionForFilter(
               filter->name()
           );
}

libgraphics::FilterPresetCollection ApplicationSession::presetsForFilterWithName(
    const std::string& name
) {
    return this->d->filterPresetCollection.collectionForFilter(
               name
           );
}

/// rendering and io management
bool ApplicationSession::importImageFromData(
    void*                               _data,
    libgraphics::fxapi::EPixelFormat::t _format,
    size_t                              _width,
    size_t                              _height
) {
    assert( _data != nullptr );
    assert( _width * _height != 0 );
    assert( _format != libgraphics::fxapi::EPixelFormat::Empty );

    libgraphics::Image*     originalImage = new libgraphics::Image(
        this->d->backend->cpuBackend(),
        _format,
        _width,
        _height,
        _data
    );
    assert( originalImage );

    if( originalImage->empty() ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionImport::commit(): Failed to create image objects from bitmap.";
#endif
        return false;
    }

    this->resetImageState(
        nullptr,
        originalImage,
        ""
    );
    return true;
}

bool ApplicationSession::importImageFromPath(
    const std::string& path
) {

    if( !libcommon::fileutils::fileExists( path ) ) {
        LOG_WARNING( "Import file does not exist : " + path );
        return false;
    }

    std::unique_ptr<ApplicationActionImport> importAction(
        new ApplicationActionImport(
            this,
            this->d->backend->cpuBackend(),
            path
        )
    );

    bool processed = importAction->process();
    LOGB_RETURN( !processed, "ApplicationSession::importImageFromPath(): Failed to import image from path " + path, false );

    bool committed = importAction->commit();
    LOGB_RETURN( !committed, "ApplicationSession::importImageFromPath(): Failed to import image from path " + path, false );

    /// recalculate all filter states
    this->updateAllFilters();

    return true;
}

bool ApplicationSession::importImageFromPath(
    EImageFormat::t format,
    const std::string& path
) {
    ( void )format;

    std::unique_ptr<ApplicationActionImport> importAction(
        new ApplicationActionImport(
            this,
            this->d->backend->cpuBackend(),
            path
        )
    );

    const auto successfullyProcessedAction = importAction->process();

    assert( successfullyProcessedAction );

    if( !successfullyProcessedAction ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationSession::importImageFromPath(): Failed to import image from path '" << path.c_str() << "'.";
#endif
        return false;
    }

    const auto successfullyCommitedAction = importAction->commit();

    assert( successfullyCommitedAction );

    if( !successfullyCommitedAction ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationSession::importImageFromPath(): Failed to import image from path '" << path.c_str() << "'.";
#endif
        return false;
    }

    /// recalculate all filter states
    this->updateAllFilters();

    return true;
}

ApplicationActionImport* ApplicationSession::asyncImportImage(
    const std::string& path
) {
    return new ApplicationActionImport(
               this,
               this->d->backend->cpuBackend(),
               path
           );
}

ApplicationActionImport* ApplicationSession::asyncImportImage(
    EImageFormat::t format,
    const std::string& path
) {
    ( void )format;

    return new ApplicationActionImport(
               this,
               this->d->backend->cpuBackend(),
               path
           );
}

bool ApplicationSession::exportImage(
    void* buffer,
    bool fromPreview,
    libgraphics::Bitmap* seperateAlphaLayer,
    bool forceCpuRendering
) {
    assert( buffer != nullptr );

    if( fromPreview ) {
        qDebug() << "Exporting current image directly from preview to output buffer";
    } else {
        qDebug() << "Exporting current image to output buffer";
    }

    std::unique_ptr<libfoundation::app::ApplicationActionExport>   action(
        new libfoundation::app::ApplicationActionExport(
            this,
            "",
            EImageFormat::BMP, /// Needs a valid value, but will be ignored if the path is empty
            fromPreview ? this->previewImage()->bottomLayer() : this->originalImage()->layers().front().get(),
            fromPreview && !forceCpuRendering,
            !fromPreview && !forceCpuRendering,
            seperateAlphaLayer,
            buffer
        )
    );

    const auto successfullyProcessedAction = action->process();

    assert( successfullyProcessedAction );

    if( !successfullyProcessedAction ) {
        qDebug() << "Failed to export image: Failed to process image.";
        return false;
    }

    const auto successfullyCommitedAction = action->commit();

    assert( successfullyCommitedAction );

    if( !successfullyCommitedAction ) {
        qDebug() << "Failed to export image: Failed to commit rendered image.";
        return false;
    }

    qDebug() << "Successfully exported image to given path. Resetting all filters now...";

    /// recalculate all filter states
    this->updateAllFilters();

    return true;
}

bool ApplicationSession::exportImage(
    const std::string& path,
    EImageFormat::t format,
    bool fromPreview,
    libgraphics::Bitmap* seperateAlphaLayer,
    bool forceCpuRendering
) {
    if( fromPreview ) {
        qDebug() << "Exporting current image directly from preview to " << path.c_str();
    } else {
        qDebug() << "Exporting current image to " << path.c_str();
    }

    std::unique_ptr<libfoundation::app::ApplicationActionExport>   action(
        new libfoundation::app::ApplicationActionExport(
            this,
            path,
            format,
            fromPreview ? this->previewImage()->bottomLayer() : this->originalImage()->layers().front().get(),
            fromPreview && !forceCpuRendering,
            !fromPreview && !forceCpuRendering,
            seperateAlphaLayer
        )
    );

    const auto successfullyProcessedAction = action->process();

    assert( successfullyProcessedAction );

    if( !successfullyProcessedAction ) {
        qDebug() << "Failed to export image: Failed to process image.";
        return false;
    }

    const auto successfullyCommitedAction = action->commit();

    assert( successfullyCommitedAction );

    if( !successfullyCommitedAction ) {
        qDebug() << "Failed to export image: Failed to commit rendered image.";
        return false;
    }

    qDebug() << "Successfully exported image to given path. Resetting all filters now...";

    /// recalculate all filter states
    this->updateAllFilters();

    if( !libcommon::fileutils::fileExists( path ) ) {
        LOG_WARNING( "Export file does not exist : " + path );
        return false;
    }

    return true;
}

ApplicationActionExport* ApplicationSession::asyncExportImage(
    const std::string& path,
    EImageFormat::t format
) {
    return new libfoundation::app::ApplicationActionExport(
               this,
               path,
               format,
               this->originalImage()->layers().front().get(),
               false
           );
}

bool ApplicationSession::updatePreview( bool _force ) {
    if( _force ) {
        this->updateAllFilters();
    }

    std::unique_ptr<libfoundation::app::ApplicationActionRenderPreview>   action(
        new libfoundation::app::ApplicationActionRenderPreview(
            this,
            this->previewBackend(),
            this->d->previewImage->layers().front().get(),
            this->d->previewImage->layers().at( 1 ).get(),
            &this->d->filterStack
        )
    );

    const auto successfullyProcessedAction = action->process();

    assert( successfullyProcessedAction );

    if( !successfullyProcessedAction ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationSession::updatePreview(): Failed to update preview.";
#endif
        return false;
    }

    const auto successfullyCommitedAction = action->commit();

    assert( successfullyCommitedAction );

    if( !successfullyCommitedAction ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationSession::updatePreview(): Failed to update preview.";
#endif
        return false;
    }

    return true;
}

ApplicationActionRenderPreview* ApplicationSession::asyncUpdatePreview( bool _force ) {
    if( _force ) {
        this->updateAllFilters();
    }

    return new libfoundation::app::ApplicationActionRenderPreview(
               this,
               this->previewBackend(),
               this->d->previewImage->layers().front().get(),
               this->d->previewImage->layers().at( 1 ).get(),
               &this->d->filterStack
           );
}

bool ApplicationSession::renderToBitmap(
    libgraphics::Bitmap* destination,
    libgraphics::fxapi::ApiBackendDevice* backendDevice
) {
    std::unique_ptr<libgraphics::ImageLayer> imageObject(
        new libgraphics::ImageLayer(
            backendDevice
        )
    );
    assert( ( imageObject.get() != nullptr ) );

    const auto successfullyReset = imageObject->reset( this->originalImage()->format(), this->originalImage()->width(), this->originalImage()->height() );
    LOGB_WARNING( !successfullyReset, "Could not reset image" );
    assert( successfullyReset );

    if( imageObject->empty() ) {
#if LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Error: Failed to create image object. ==> Failed to render to bitmap.";
#endif
        return false;
    }

    if( !renderToLayer( imageObject.get(), backendDevice ) ) {
        assert( false );
#if LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Error: Failed to render to bitmap.";
#endif
        return false;
    }

    if( !imageObject->retrieve( destination ) ) {
#if LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Error: Failed to retrieve data from rendered image layer.";
#endif
        return false;
    }

    return true;
}

bool ApplicationSession::renderToLayer(
    libgraphics::ImageLayer* destination,
    libgraphics::fxapi::ApiBackendDevice* backendDevice
) {
    assert( destination );

    std::unique_ptr<libfoundation::app::ApplicationActionRenderPreview> preview( new libfoundation::app::ApplicationActionRenderPreview(
                this,
                backendDevice,
                destination,
                this->d->originalImage->topLayer(),
                &this->d->filterStack
            ) );
    assert( preview.get() );


    const auto successfullyRendered = preview->process();

    if( !successfullyRendered ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Failed to execute ApplicationActionRenderPreview. Aborting...";
#endif
        return false;
    }

    const auto successfullyCommited = preview->commit();
    LOGB_WARNING( !successfullyCommited, "Could not commit preview" );
    assert( successfullyCommited );

    return true;
}

ApplicationActionRenderPreview* ApplicationSession::asyncRenderToLayer(
    libgraphics::ImageLayer* destination,
    bool usingCpu
) {
    assert( destination );

    libfoundation::app::ApplicationActionRenderPreview* preview = new libfoundation::app::ApplicationActionRenderPreview(
        this,
        usingCpu ? this->backend()->cpuBackend() : this->backend()->gpuBackend(),
        destination,
        this->d->previewImage->layers().front().get(),
        &this->d->filterStack
    );
    assert( preview );

    if( usingCpu ) {
        this->run(
            new ActionRunnable(
                preview
            )
        );
    } else {
        const auto successfullyRendered = preview->process();

        if( !successfullyRendered ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
            qDebug() << "Failed to execute ApplicationActionRenderPreview. Aborting...";
#endif
        }
    }

    return preview;
}

/// state management
///
libgraphics::FilterPresetCollection ApplicationSession::currentStateToPresetCollection() {
    libgraphics::FilterPresetCollection collection;

    for( auto it = this->d->filterCollection.begin(); it != this->d->filterCollection.end(); ++it ) {
        collection.add(
            ( *it )->toPreset()
        );
    }

    return collection;
}

libgraphics::FilterPresetCollection ApplicationSession::currentActiveStateToPresetCollection() {
    libgraphics::FilterPresetCollection collection;

    for( auto it = this->d->filterCollection.begin(); it != this->d->filterCollection.end(); ++it ) {
        if( this->d->shouldRenderFilter( ( *it ).get() ) ) {
            collection.add(
                ( *it )->toPreset()
            );
        }
    }

    return collection;
}

void ApplicationSession::reset() {
    this->waitForAll();

    this->d->filterStack.clear();
    this->d->filterCollection.clear();
    this->d->filterMetaInfo.clear();

    this->d->previewImage.reset();
    this->d->originalImage.reset();

    const auto shuttingDownBackend = this->d->backend->shutdown();
    assert( shuttingDownBackend );

    if( !shuttingDownBackend ) {
        qDebug() << "Failed to shutdown application backend.";
    }

    this->d->previewBackend = nullptr;

    this->d->filterPresetCollection.clear();
    this->d->backend.reset();
}

ApplicationSession* ApplicationSession::clone() {
    ApplicationSession* clonedSession = new ApplicationSession( this->d->name );
    assert( clonedSession != nullptr );

    clonedSession->setThreadCount(
        this->d->appThreadPool.activeThreadCount()
    );
    clonedSession->d->backend                   = this->d->backend;
    clonedSession->d->filterPresetCollection    = this->d->filterPresetCollection;
    clonedSession->d->filterMetaInfo            = this->d->filterMetaInfo;

    for( auto it = this->d->filterStack.begin(); it != this->d->filterStack.end(); ++it ) {
        std::shared_ptr<libgraphics::Filter> clonedFilter( ( *it )->clone() );

        clonedSession->d->filterStack.pushBack( clonedFilter );
        clonedSession->d->filterCollection.add( clonedFilter );

        bool found( false );

        for( auto st = clonedSession->d->filterMetaInfo.begin(); st != clonedSession->d->filterMetaInfo.end(); ++st ) {
            if( ( *st ).filterObject == ( *it ).get() ) {
                ( *st ).filterObject  = clonedFilter.get();
                found               = true;
                break;
            }
        }

        LOGB_ASSERT( found != false, "did not find meta info" );
    }

    clonedSession->d->imageOrigin           = this->d->imageOrigin;
    clonedSession->d->imagePath             = this->d->imagePath;
    clonedSession->d->maxThreadCount        = this->d->maxThreadCount;
    clonedSession->d->originalImage         = this->d->originalImage;
    clonedSession->d->pipeline              = this->d->pipeline;
    clonedSession->d->previewBackend        = this->d->previewBackend;
    clonedSession->d->previewImage          = this->d->previewImage;
    clonedSession->d->sessionPath           = this->d->sessionPath;

    return clonedSession;
}

void ApplicationSession::resetImageState() {
    this->d->previewImage.reset();
    this->d->originalImage.reset();
}

void ApplicationSession::resetImageState(
    libgraphics::Image* preview,
    libgraphics::Image* original
) {
    if( preview != nullptr ) {
        this->d->previewImage.reset( preview );
    }

    if( original != nullptr ) {
        this->d->originalImage.reset( original );
    }

    this->updateAllFilters();
}

void ApplicationSession::resetImageState(
    libgraphics::Image* preview,
    libgraphics::Image* original,
    const std::string& path
) {
    resetImageState( preview, original );

    this->d->imagePath = path;
}

/** ApplicationActionRenderPreview **/
struct ApplicationActionRenderPreview::Private {
    ApplicationSession*     session;
    libgraphics::fxapi::ApiBackendDevice*   backendDevice;
    libgraphics::ImageLayer*    destination;
    libgraphics::ImageLayer*    source;
    libgraphics::FilterStack*   stack;
    const unsigned int          initialThreadId;

    Private(
        ApplicationSession* _session,
        libgraphics::fxapi::ApiBackendDevice* _backendDevice,
        libgraphics::ImageLayer* _destination,
        libgraphics::ImageLayer* _source,
        libgraphics::FilterStack* _stack ) : session( _session ), backendDevice( _backendDevice ), destination( _destination ),
        source( _source ), stack( _stack ), initialThreadId( libcommon::getCurrentThreadId() ) {}

};

ApplicationActionRenderPreview::ApplicationActionRenderPreview(
    ApplicationSession* session,
    libgraphics::fxapi::ApiBackendDevice* backendDevice,
    libgraphics::ImageLayer* destination,
    libgraphics::ImageLayer* source,
    libgraphics::FilterStack* stack
) : d( new Private( session, backendDevice, destination, source, stack ) ), SessionCommitableApplicationAction( session ) {}

libgraphics::ImageLayer* ApplicationActionRenderPreview::imageLayer() const {
    return d->destination;
}

libgraphics::fxapi::ApiBackendDevice* ApplicationActionRenderPreview::device() const {
    return d->backendDevice;
}

libgraphics::FilterStack* ApplicationActionRenderPreview::filters() const {
    return d->stack;
}

bool ApplicationActionRenderPreview::commit() {
    const unsigned int currentThreadId = libcommon::getCurrentThreadId();

    assert( currentThreadId == d->initialThreadId );

    if( currentThreadId != d->initialThreadId ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "ApplicationActionRenderPreview::commit(): Actions have to be comitted in the main thread.";
#endif
        return false;
    }

    /** no real work to do */

    return true;
}

bool ApplicationActionRenderPreview::process() {
    std::lock_guard<std::mutex> lock( this->m_FinishedMutex );

#ifdef _DEBUG
    const unsigned int currentThreadId = libcommon::getCurrentThreadId();

    if( this->d->backendDevice->backendId() == FXAPI_BACKEND_OPENGL ) {
        assert( currentThreadId == d->initialThreadId );

        if( currentThreadId != d->initialThreadId ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
            qDebug() << "ApplicationActionRenderPreview::process(): OpenGL backend has no multithreading support. Always render preview in main thread.";
#endif
            return false;
        }
    }

#endif

    libgraphics::FilterStack    renderableFilters;

    for( auto it = this->d->stack->begin(); it != this->d->stack->end(); ++it ) {
        if( this->d->session->d->shouldRenderFilter( ( *it ).get() ) ) {
            renderableFilters.pushBack(
                ( *it )
            );
        }
    }

    if( renderableFilters.count() >= 1 ) {
        libgraphics::Filter* filterObject = renderableFilters.top().get();
        assert( filterObject );

        const auto successfullyRendered = filterObject->process(
                                              this->d->backendDevice,
                                              this->d->destination,
                                              this->d->source
                                          );

        assert( successfullyRendered );

        if( !successfullyRendered ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
            qDebug() << "ApplicationActionRenderPreview::process(): Failed to render front filter. Aborting...";
#endif
            return false;
        }

        if( renderableFilters.count() == 1 ) {
            return successfullyRendered;
        }

        std::unique_ptr<libgraphics::ImageLayer>   temporaryLayer( libgraphics::makeImageLayer( this->d->backendDevice, this->d->destination ) );
        const auto successfullyInitializedTemporaryLayer = !temporaryLayer->empty();

        assert( successfullyInitializedTemporaryLayer );

        if( !successfullyInitializedTemporaryLayer ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
            qDebug() << "ApplicationActionRenderPreview::process(): Failed to create temporary layer.";
#endif
            return false;
        }

        libgraphics::ImageLayer* front( d->destination );
        libgraphics::ImageLayer* back( temporaryLayer.get() );

        for( auto it = renderableFilters.begin() + 1; it != renderableFilters.end(); ++it ) {
            libgraphics::Filter* currentFilter = ( *it ).get();
            assert( currentFilter );

            const auto successfullyRendered = currentFilter->process(
                                                  this->d->backendDevice,
                                                  back,
                                                  front
                                              );

            assert( successfullyRendered );

            if( !successfullyRendered ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
                qDebug() << "ApplicationActionRenderPreview::process(): Failed to render filter with name '" << currentFilter->name().c_str() << "'. Aborting...";
#endif
                return false;
            }

            std::swap(
                front,
                back
            );
        }

        if( ( renderableFilters.count() - 1 ) % 2 != 0 ) {
            /** number of filters is not dividable by two */
            libgraphics::fx::operations::blit(
                d->destination,
                temporaryLayer.get(),
                temporaryLayer->size(),
                temporaryLayer->size()
            );
        }

        return true;
    }

    qDebug() << "Warning: No active filters found. Aborting...";

    libgraphics::fx::operations::blit(
        d->destination,
        d->source,
        d->source->size(),
        d->source->size()
    );

    return true;
}

bool ApplicationActionRenderPreview::finished() {
    const auto locked = this->m_FinishedMutex.try_lock();

    if( locked ) {
        this->m_FinishedMutex.unlock();
        return true;
    }

    return false;
}

}
}
