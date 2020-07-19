#pragma once

#include <libcommon/def.hpp>

namespace libcommon {

template < class _t_value >
void swap( _t_value& first, _t_value& second ) {
    _t_value   temp( first );
    first = second;
    second = temp;
}

}

