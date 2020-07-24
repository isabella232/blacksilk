/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once

#include <libcommon/sharedptr.hpp>
#include <libserialization++/SerializationInfo.hpp>

namespace spp {

namespace Apis {
enum t {
    Json,
    Binary
};
}

/**
    \class      Provider
    \since      0.4.0-0
    \brief
        The base class for all different types
        for serialization providers.
*/
class   Provider {
    public:

        /**
            \fn     ~Provider
            \since  0.4.0-0
            \brief
                Frees the current internal
                state.
        */
        virtual ~Provider() {}

        /**
            \fn         CreateSerialization
            \since      0.4.0-0
            \brief
                Constructs a new serialization info instance from the
                given existing provider.
        */
        virtual std::shared_ptr<SerializationInfo>      CreateSerialization( spp::Stream* stream ) = 0;
};

/**
    \fn     CreateSerialization
    \since  0.4.0-0
    \brief
        Constructs a new standard serialization info
        context structure.
*/
std::shared_ptr<SerializationInfo>      CreateSerialization( const spp::Apis::t& api, Stream* stream );

}
