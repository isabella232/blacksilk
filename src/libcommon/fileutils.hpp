#include <string>

namespace libcommon {
namespace fileutils {
bool fileExists( const std::string& filename );
std::string fromFile( const std::string& filename );
bool toFile( const std::string& filename, const std::string& data );
}
}
