
#include <libgraphics/filtercollection.hpp>

#include <libgraphics/fx/filters/filmgrain.hpp>

#include <libfoundation/app/application.hpp>
#include <libfoundation/app/backgroundtasklistener.hpp>
#include <libfoundation/app/tasks/rendergrain.hpp>

namespace libfoundation {
namespace app {
namespace tasks {

bool RenderGrain::onStart(
    libfoundation::app::ApplicationSession* session
) {
    assert( session != nullptr );
    this->m_Session = session;

    return ( this->m_Session != nullptr );
}

void RenderGrain::run(
    BackgroundTaskListener* listener
) {

    this->beginExecution();
    this->m_Listener = listener;

    if( this->m_Listener != nullptr ) {
        this->m_Listener->reportProgress(
            this->name(),
            0
        );
    }

    auto filmGrain = ( libgraphics::fx::filters::FilmGrain* )( this->m_Session->filters()->byName( "FilmGrain" ).get() );

    if( filmGrain != nullptr ) {
        auto previewImage = this->m_Session->originalImage();

        if( !previewImage ) {
            if( this->m_Listener != nullptr ) {
                this->m_Listener->reportError(
                    this->name(),
                    "Failed to precalculate grain for invalid preview image."
                );
            }

            this->endExecution();
            return;
        }

        auto previewBackend = this->m_Session->backend()->cpuBackend();

        if( !previewBackend ) {
            if( this->m_Listener != nullptr ) {
                this->m_Listener->reportError(
                    this->name(),
                    "Failed to precalculate grain using invalid backend."
                );
            }

            this->endExecution();
            return;
        }

        filmGrain->resetGrain(
            previewBackend,
            previewImage->format(),
            previewImage->width(),
            previewImage->height()
        );

        if( this->m_Listener != nullptr ) {
            this->m_Listener->reportProgress(
                this->name(),
                100
            );
        }
    }

    this->endExecution();
}

}
}
}
