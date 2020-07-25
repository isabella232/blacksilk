#pragma once

#include <libfoundation/app/application.hpp>

namespace blacksilk {

class OffscreenContext {
    public:
        struct Private;

        OffscreenContext();
        virtual ~OffscreenContext();

        bool    valid() const;
    protected:
        std::shared_ptr<Private>        d;
};


}
