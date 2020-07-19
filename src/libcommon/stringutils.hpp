#include <string>

namespace libcommon {
namespace stringutils {
bool iequals( const std::string& first, const std::string& second );
std::istream& safeGetline( std::istream& is, std::string& t );
}
}

