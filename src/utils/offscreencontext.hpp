#pragma once

#include <libcommon/pimpl.hpp>
#include <libfoundation/app/application.hpp>

namespace blacksilk {

class OffscreenContext {
    public:
        struct Private;

        OffscreenContext();
        virtual ~OffscreenContext();

        bool    valid() const;
    protected:
        libcommon::PimplPtr<Private>        d;
};


}
