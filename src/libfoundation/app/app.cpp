#include <libgraphics/io/pipelinepluginloader.hpp>
#include <libfoundation/app/application.hpp>
#include <libcommon/lockable.hpp>
#include <QDebug>

#include <memory>

#include <libcommon/fileutils.hpp>

namespace libfoundation {
namespace app {

void ApplicationAction::waitForFinished() {
    std::lock_guard<std::mutex> lock( this->m_FinishedMutex );
}

struct Application::Private : libcommon::PimplPrivate {
    ApplicationSession*                                     currentSession;
    std::vector<std::shared_ptr<ApplicationSession> >       sessions;
    ApplicationConfig                                       config;
    libcommon::ScopedPtr<ApplicationSystemLayer>            systemLayer;
    libcommon::ScopedPtr<ApplicationHardwareInfo>           hardwareInfo;
    std::shared_ptr<libgraphics::io::Pipeline>              pipeline;

    bool initialized;

    Private() : currentSession( nullptr ), hardwareInfo( new ApplicationHardwareInfo() ), pipeline( new libgraphics::io::StdPipeline() ), initialized( false ) {}
};

Application::Application() : d( new Private() ) {}

bool Application::initialize(
    ApplicationSystemLayer* layer,
    const std::string& configurationPath
) {
    assert( layer );
    ( void )configurationPath;

    if( d->initialized ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Application::initialize(): Failed to initialize application. Already initialized.";
#endif
        return false;
    }

    d->systemLayer.reset( layer );

    d->initialized = true;

#ifdef LIBFOUNDATION_DEBUG_OUTPUT
    qDebug() << "Application::initialize(): Successfully initialized application.";
#endif

    return true;
}

bool Application::initialize(
    ApplicationSystemLayer* layer,
    ApplicationConfig config
) {
    assert( layer );

    if( d->initialized ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Application::initialize(): Failed to initialize application. Already initialized.";
#endif
        return false;
    }

    d->config = config;
    d->systemLayer.reset( layer );

    d->initialized = true;

#ifdef LIBFOUNDATION_DEBUG_OUTPUT
    qDebug() << "Application::initialize(): Successfully initialized application.";
#endif

    return true;
}

bool Application::shutdown() {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
    qDebug() << "Application::shutdown(): Shutting down application.";
#endif

    if( !d->initialized ) {
        return false;
    }

    for( auto it = this->d->sessions.begin(); it != this->d->sessions.end(); ++it ) {
        ( *it )->reset();
    }

    d->sessions.clear();
    d->hardwareInfo.reset();
    d->pipeline.reset();

    d->initialized = false;

    return true;
}

bool Application::containsValidHardwareInfo() const {
    return !( d->hardwareInfo.empty() );
}

bool Application::updateHardwareInfo() {
    return true;
}

bool Application::isAbleToImportFormat(
    const std::string& formatDesc
) {
    return d->pipeline->containsImporterForExtension( formatDesc.c_str() );
}

bool Application::isAbleToExportFormat(
    const std::string& formatDesc
) {
    return d->pipeline->containsExporterForExtension( formatDesc.c_str() );
}

const ApplicationConfig& Application::applicationConfig() const {
    return d->config;
}

const ApplicationHardwareInfo* Application::hardwareInfo() const {
    return d->hardwareInfo.get();
}

bool Application::loadIoPluginFromPath(
    const std::string& path
) {

    if( !libcommon::fileutils::fileExists( path ) ) {
        qWarning() << "Plugin file does not exist '" << path.c_str() << "'.";
        return false;
    }

    libgraphics::io::PipelinePluginLoader loader;

    if( !loader.load( path.c_str() ) ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Application::loadIoPluginFromPath(): Failed to load plugin from path '" << path.c_str() << "'.";
#endif
        return false;
    }

    libcommon::ScopedPtr<libgraphics::io::PipelinePlugin> plugin( loader.instantiatePlugin() );

    if( plugin.empty() ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Application::loadIoPluginFromPath(): Can't load import object from invalid plugin object.";
#endif
        return false;
    }

    if( !d->pipeline->addImportersFromPlugin( plugin.get() ) ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Application::loadIoPluginFromPath(): Failed to add importers to pipeline.";
#endif
        return false;
    }

    if( !d->pipeline->addExportersFromPlugin( plugin.get() ) ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Application::loadIoExporterFromPath(): Failed to add exporters to pipeline.";
#endif
        return false;
    }

    return true;
}

bool Application::loadIoImporterFromPath(
    const std::string& path
) {
    libgraphics::io::PipelinePluginLoader loader;

    if( !loader.load( path.c_str() ) ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Application::loadIoImporterFromPath(): Failed to load plugin from path '" << path.c_str() << "'.";
#endif
        return false;
    }

    libcommon::ScopedPtr<libgraphics::io::PipelinePlugin> plugin( loader.instantiatePlugin() );

    if( plugin.empty() ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Application::loadIoImporterFromPath(): Can't load import object from invalid plugin object.";
#endif
        return false;
    }

    if( !d->pipeline->addImportersFromPlugin( plugin.get() ) ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Application::loadIoImportersFromPath(): Failed to add importers to pipeline.";
#endif
        return false;
    }

    return true;
}

bool Application::loadIoExporterFromPath(
    const std::string& path
) {
    libgraphics::io::PipelinePluginLoader loader;

    if( !loader.load( path.c_str() ) ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Application::loadIoExporterFromPath(): Failed to load plugin from path '" << path.c_str() << "'.";
#endif
        return false;
    }

    libcommon::ScopedPtr<libgraphics::io::PipelinePlugin> plugin( loader.instantiatePlugin() );

    if( plugin.empty() ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Application::loadIoExporterFromPath(): Can't load import object from invalid plugin object.";
#endif
        return false;
    }

    if( !d->pipeline->addExportersFromPlugin( plugin.get() ) ) {
#ifdef LIBFOUNDATION_DEBUG_OUTPUT
        qDebug() << "Application::loadIoExporterFromPath(): Failed to add exporters to pipeline.";
#endif
        return false;
    }

    return true;
}

void Application::unloadAllIoExporters() {
    d->pipeline->clearExporters();
}

void Application::unloadAllIoImporters() {
    d->pipeline->clearImporters();
}

ApplicationSession* Application::currentSession() {
    return d->currentSession;
}

ApplicationSession* Application::sessionByName( const std::string& name ) {
    for( auto it = this->d->sessions.begin(); it != this->d->sessions.end(); ++it ) {
        if( ( *it )->name() == name ) {
            return ( *it ).get();
        }
    }

    return nullptr;
}

ApplicationSession* Application::createSession( const std::string& name ) {
    ApplicationSession* session = new ApplicationSession(
        name
    );
    assert( session );

    this->d->sessions.push_back(
        std::shared_ptr<ApplicationSession>( session )
    );

    session->setPipeline(
        this->d->pipeline
    );
    session->setThreadCount( 8 );

    return session;
}

ApplicationSession* Application::createAndActivateSession( const std::string& name ) {
    ApplicationSession* session = createSession(
                                      name
                                  );
    assert( session );

    d->currentSession = session;

    return session;
}

bool Application::makeCurrentSession( const std::string& name ) {
    for( auto it = this->d->sessions.begin(); it != this->d->sessions.end(); ++it ) {
        if( ( *it )->name() == name ) {
            this->d->currentSession = ( *it ).get();

            return true;
        }
    }

    return false;
}

}
}
