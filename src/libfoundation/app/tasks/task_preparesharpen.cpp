
#include <libgraphics/filtercollection.hpp>

#include <libgraphics/fx/filters/cascadedsharpen.hpp>

#include <libfoundation/app/application.hpp>
#include <libfoundation/app/backgroundtasklistener.hpp>
#include <libfoundation/app/tasks/preparesharpen.hpp>

namespace libfoundation {
namespace app {
namespace tasks {

bool PrepareSharpen::onStart(
    libfoundation::app::ApplicationSession* session
) {
    assert( session != nullptr );
    this->m_Session = session;

    return ( this->m_Session != nullptr );
}

void PrepareSharpen::configure(
    const std::vector<float>&   cascades
) {
    this->m_Cascades = cascades;
}

void PrepareSharpen::run(
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

    auto cascadedSharpen = ( libgraphics::fx::filters::CascadedSharpen* )( this->m_Session->filters()->byName( "CascadedSharpen" ).get() );

    if( cascadedSharpen != nullptr ) {
        auto previewImage = this->m_Session->originalImage();

        if( !previewImage ) {
            if( this->m_Listener != nullptr ) {
                this->m_Listener->reportError(
                    this->name(),
                    "Failed to precalculate sharpen cascades for invalid preview image."
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
                    "Failed to precalculate sharpen cascades using invalid backend."
                );
            }

            this->endExecution();
            return;
        }

        cascadedSharpen->generateCascades(
            this->m_Cascades,
            previewBackend,
            previewImage->topLayer()
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
