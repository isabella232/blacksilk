#include <string>

namespace libcommon {
namespace fileutils {
std::string fromFile( const std::string& filename );
bool toFile( const std::string& filename, const std::string& data );
}
}
