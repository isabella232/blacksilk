#include <libgraphics/backend/cpu/cpu_imageoperation.hpp>

namespace libgraphics {
namespace backend {
namespace cpu {

struct ImageOperation::Private {
    const std::string   name;
    Private( const char* _name ) : name( _name ) {}
};

/// constr.
ImageOperation::ImageOperation( const char* name ) : d( new Private( name ) ) {}

/// destr.
ImageOperation::~ImageOperation() {
    d.reset();
}

/// properties
const char* ImageOperation::name() {
    return d->name.c_str();
}

bool ImageOperation::working() {
    return false;
}

long ImageOperation::backendId() {
    return FXAPI_BACKEND_CPU;
}

}
}
}
