#pragma once

#include <libserialization++.hpp>
#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>

#include <algorithm>
#include <sstream>
#include <vector>
#include <string>

namespace libgraphics {

struct GenericFilterPresetEntry {
    std::string name;
    std::string value;

    GenericFilterPresetEntry() {}
    GenericFilterPresetEntry( const std::string& _name, const std::string& _value ) :
        name( _name ), value( _value ) {}
    inline bool operator == ( const GenericFilterPresetEntry& rhs ) const {
        return ( rhs.name == name ) && ( rhs.value == value );
    }
    inline bool operator != ( const GenericFilterPresetEntry& rhs ) const {
        return ( rhs.name != name ) || ( rhs.value != value );
    }
};
typedef std::vector<GenericFilterPresetEntry>   GenericFilterPresetEntryVector;

struct GenericFilterPresetEntrySerializationProvider : public spp::AutoSerializable {
    public:
        explicit GenericFilterPresetEntrySerializationProvider(
            GenericFilterPresetEntry* entry
        ) : m_Entry( entry ) { assert( entry ); }
        virtual ~GenericFilterPresetEntrySerializationProvider() {}

        void assign( GenericFilterPresetEntry* entry );
        void clear();
        bool empty() const;

        virtual spp::PropertyCollection GetProperties();
    protected:
        GenericFilterPresetEntry* m_Entry;
};

template < class _t_any >
struct PresetValueConverter;

template < class _t_any >
struct PresetValueConverter {
    static std::string toString( const _t_any& value ) {
        static spp::PropertyDecoder decoder;

        return decoder.Decode(
                   ( _t_any* )&value
               );
    }
    static bool fromString( _t_any& value, const std::string& str ) {
        std::stringstream ss;
        ss << str;
        ss >> value;

        return !ss.bad();
    }
};
template <>
struct PresetValueConverter<bool> {
    static std::string toString( const bool& value ) {
        return value ? "true" : "false";
    }
    static bool fromString( bool& value, std::string str ) {
        std::transform(
            str.begin(),
            str.end(),
            str.begin(),
            ::tolower
        );
        static const std::string const_true = "true";
        static const std::string const_false = "false";

        if( str == const_true ) {
            value = true;
            return true;
        }

        if( str == const_false ) {
            value = false;
            return true;
        }

        return false;
    }
};

namespace helpers {
static std::string removeWhitespaces( const std::string& val ) {
    std::string ret;

    for( auto it = val.begin(); it != val.end(); ++it ) {
        if( ( *it ) != ' ' ) {
            ret += ( *it );
        }
    }

    return ret;
}
static std::vector<std::string> split( const std::string& val, char split ) {
    std::vector<std::string> values;
    std::string current;

    for( auto it = val.begin(); it != val.end(); ++it ) {
        if( ( *it ) == split ) {
            if( !current.empty() ) {
                values.push_back( current );
            }

            current.clear();

            continue;
        }

        current += ( *it );
    }

    return values;
}
static size_t count( const std::string& val, char chr ) {
    size_t c( 0 );

    for( auto it = val.begin(); it != val.end(); ++it ) {
        if( ( *it ) == chr ) {
            ++c;
        }
    }

    return c;
}
static bool lowerStringEqual( const std::string& first, const std::string& second ) {
    if( first.size() != second.size() ) { return false; }

    for( size_t index = 0; first.size() > index; ++index ) {
        if( ::tolower( first[index] ) != ::tolower( second[index] ) ) {
            return false;
        }
    }

    return true;
}
static bool isSubset( const std::string& set, const std::string& value ) {
    for( auto it = value.begin(); it != value.end(); ++it ) {
        if( set.find( *it ) == std::string::npos ) {
            return false;
        }
    }

    return true;
}
static bool isNumber( const std::string& value ) {
    static const std::string numberSet = "0123456789";
    return isSubset(
               numberSet,
               value
           );
}

struct ETokenType {
    enum t {
        String,
        Integer,
        Float,
        Boolean,
        None
    };
};
static ETokenType::t tokenTypeForString( const std::string& val ) {
    assert( !val.empty() );

    if( val.empty() ) {
        return ETokenType::None;
    }

    /// check for string
    if( ( ( val.front() == '"' ) || ( val.front() == '"' ) ) &&
            ( ( val.back() == '"' ) || ( val.back() == '"' ) ) ) {
        return ETokenType::String;
    }

    /// check for boolean
    {
        static const std::string string_true = "true";
        static const std::string string_false = "false";

        if( lowerStringEqual( val, string_true ) || lowerStringEqual( val, string_false ) ) {
            return ETokenType::Boolean;
        }
    }

    /// check for number
    if( isNumber( val ) ) {
        return ETokenType::Integer;
    }

    /// check for floating point number
    const auto countCommas = count( val, ',' );
    const auto countDots = count( val, '.' );

    if( ( countCommas == 1 ) || ( countDots == 1 ) ) {
        const auto parts = split(
                               val,
                               ( countDots == 1 ) ? '.' : ','
                           );
        assert( !parts.empty() );

        if( isNumber( parts[0] ) && isNumber( parts[1] ) ) {
            return ETokenType::Float;
        }
    }

    return ETokenType::None;
}

}
template < class _t_value, size_t count >
struct PresetValuePixelHelper {
    static std::string toString( const libgraphics::BasicPixel<_t_value, count>& value ) {
        std::stringstream ss;

        for( size_t i = 0; count - 1 > i; ++i ) {
            ss << value.Values[i];
            ss << ",";
        }

        ss << value.Values[count - 1];

        return ss.str();
    }
    static bool fromString( libgraphics::BasicPixel<_t_value, count>& value, const std::string& str ) {
        const auto values = helpers::split(
                                helpers::removeWhitespaces(
                                    str
                                ),
                                ','
                            );
        assert( !values.empty() );

        if( values.empty() ) {
            return false;
        }

        bool success( true );
        size_t index( 0 );

        for( auto it = values.begin(); it != values.end(); ++it ) {
            std::stringstream ss;
            ss << ( *it );

            if( !ss.good() ) {
                success = false;
                break;
            }

            try {
                ss >> value.Values[index];
            } catch( ... ) {
                success = false;
                break;
            }

            if( !ss.good() ) {
                success = false;
                break;
            }

            ++index;
        }

        return success;
    }
};
template <>
struct PresetValueConverter<libgraphics::formats::ARGB8::t> {
    typedef libgraphics::formats::ARGB8::t  ValueType;

    static std::string toString( const ValueType& value ) {
        return PresetValuePixelHelper <
               ValueType::PixelType,
               ValueType::Count
               >::toString(
                   value
               );
    }
    static bool fromString( ValueType& value, const std::string& str ) {
        return PresetValuePixelHelper <
               ValueType::PixelType,
               ValueType::Count
               >::fromString(
                   value,
                   str
               );
    }
};
template <>
struct PresetValueConverter<libgraphics::formats::ARGB16::t> {
    typedef libgraphics::formats::ARGB16::t  ValueType;

    static std::string toString( const ValueType& value ) {
        return PresetValuePixelHelper <
               ValueType::PixelType,
               ValueType::Count
               >::toString(
                   value
               );
    }
    static bool fromString( ValueType& value, const std::string& str ) {
        return PresetValuePixelHelper <
               ValueType::PixelType,
               ValueType::Count
               >::fromString(
                   value,
                   str
               );
    }
};
template <>
struct PresetValueConverter<libgraphics::formats::RGB8::t> {
    typedef libgraphics::formats::RGB8::t  ValueType;

    static std::string toString( const ValueType& value ) {
        return PresetValuePixelHelper <
               ValueType::PixelType,
               ValueType::Count
               >::toString(
                   value
               );
    }
    static bool fromString( ValueType& value, const std::string& str ) {
        return PresetValuePixelHelper <
               ValueType::PixelType,
               ValueType::Count
               >::fromString(
                   value,
                   str
               );
    }
};
template <>
struct PresetValueConverter<libgraphics::formats::RGB16::t> {
    typedef libgraphics::formats::RGB16::t  ValueType;

    static std::string toString( const ValueType& value ) {
        return PresetValuePixelHelper <
               ValueType::PixelType,
               ValueType::Count
               >::toString(
                   value
               );
    }
    static bool fromString( ValueType& value, const std::string& str ) {
        return PresetValuePixelHelper <
               ValueType::PixelType,
               ValueType::Count
               >::fromString(
                   value,
                   str
               );
    }
};
template <>
struct PresetValueConverter<libgraphics::formats::Mono8::t> {
    typedef libgraphics::formats::Mono8::t  ValueType;

    static std::string toString( const ValueType& value ) {
        return PresetValuePixelHelper <
               ValueType::PixelType,
               ValueType::Count
               >::toString(
                   value
               );
    }
    static bool fromString( ValueType& value, const std::string& str ) {
        return PresetValuePixelHelper <
               ValueType::PixelType,
               ValueType::Count
               >::fromString(
                   value,
                   str
               );
    }
};
template <>
struct PresetValueConverter<libgraphics::formats::Mono16::t> {
    typedef libgraphics::formats::Mono16::t  ValueType;

    static std::string toString( const ValueType& value ) {
        return PresetValuePixelHelper <
               ValueType::PixelType,
               ValueType::Count
               >::toString(
                   value
               );
    }
    static bool fromString( ValueType& value, const std::string& str ) {
        return PresetValuePixelHelper <
               ValueType::PixelType,
               ValueType::Count
               >::fromString(
                   value,
                   str
               );
    }
};
template <>
struct PresetValueConverter<libgraphics::Rect32I> {
    static std::string toString( const libgraphics::Rect32I& value ) {
        std::stringstream ss;

        ss << value.x;
        ss << ",";
        ss << value.y;
        ss << ",";
        ss << value.width;
        ss << ",";
        ss << value.height;

        return ss.str();
    }
    static bool fromString( libgraphics::Rect32I& value, const std::string& str ) {
        const auto values = helpers::split(
                                helpers::removeWhitespaces(
                                    str
                                ),
                                ','
                            );
        assert( !values.empty() );

        if( values.empty() ) {
            return false;
        }

        if( values.size() == 4 ) {
            try {
                std::stringstream ss;
                ss << values[0];
                ss >> value.x;
            } catch( ... ) { return false; }

            try {
                std::stringstream ss;
                ss << values[1];
                ss >> value.y;
            } catch( ... ) { return false; }

            try {
                std::stringstream ss;
                ss << values[2];
                ss >> value.width;
            } catch( ... ) { return false; }

            try {
                std::stringstream ss;
                ss << values[3];
                ss >> value.height;
            } catch( ... ) { return false; }

            return true;
        }

        return false;
    }
};
template <>
struct PresetValueConverter<libgraphics::Point32F> {
    static std::string toString( const libgraphics::Point32F& value ) {
        std::stringstream ss;

        ss << value.x;
        ss << ",";
        ss << value.y;

        return ss.str();
    }
    static bool fromString( libgraphics::Point32F& value, const std::string& str ) {
        const auto values = helpers::split(
                                helpers::removeWhitespaces(
                                    str
                                ),
                                ','
                            );
        assert( !values.empty() );

        if( values.empty() ) {
            return false;
        }

        if( values.size() == 2 ) {
            try {
                std::stringstream ss;
                ss << values[0];
                ss >> value.x;
            } catch( ... ) { return false; }

            try {
                std::stringstream ss;
                ss << values[1];
                ss >> value.y;
            } catch( ... ) { return false; }

            return true;
        }

        return false;
    }
};
template <>
struct PresetValueConverter<libgraphics::Point32I> {
    static std::string toString( const libgraphics::Point32I& value ) {
        std::stringstream ss;

        ss << value.x;
        ss << ",";
        ss << value.y;

        return ss.str();
    }
    static bool fromString( libgraphics::Point32I& value, const std::string& str ) {
        const auto values = helpers::split(
                                helpers::removeWhitespaces(
                                    str
                                ),
                                ','
                            );
        assert( !values.empty() );

        if( values.empty() ) {
            return false;
        }

        if( values.size() == 2 ) {
            try {
                std::stringstream ss;
                ss << values[0];
                ss >> value.x;
            } catch( ... ) { return false; }

            try {
                std::stringstream ss;
                ss << values[1];
                ss >> value.y;
            } catch( ... ) { return false; }

            return true;
        }

        return false;
    }
};
template <>
struct PresetValueConverter<libgraphics::Line32I> {
    static std::string toString( const libgraphics::Line32I& value ) {
        std::stringstream ss;

        ss << value.begin.x;
        ss << ",";
        ss << value.begin.y;
        ss << ",";
        ss << value.end.x;
        ss << ",";
        ss << value.end.y;

        return ss.str();
    }
    static bool fromString( libgraphics::Line32I& value, const std::string& str ) {
        const auto values = helpers::split(
                                helpers::removeWhitespaces(
                                    str
                                ),
                                ','
                            );
        assert( !values.empty() );

        if( values.empty() ) {
            return false;
        }

        if( values.size() == 4 ) {
            try {
                std::stringstream ss;
                ss << values[0];
                ss >> value.begin.x;
            } catch( ... ) { return false; }

            try {
                std::stringstream ss;
                ss << values[1];
                ss >> value.begin.y;
            } catch( ... ) { return false; }

            try {
                std::stringstream ss;
                ss << values[2];
                ss >> value.end.x;
            } catch( ... ) { return false; }

            try {
                std::stringstream ss;
                ss << values[3];
                ss >> value.end.y;
            } catch( ... ) { return false; }

            return true;
        }

        return false;
    }
};

/// external
class FilterPreset;

struct FilterPresetSerializationProvider : public spp::AutoSerializable {
        explicit FilterPresetSerializationProvider( FilterPreset* preset ) : m_Preset( preset ) {}
        virtual ~FilterPresetSerializationProvider() {}

        void fillFilterPreset( FilterPreset* preset );
        void assign( FilterPreset* preset );
        bool empty() const;
        void clear();

        virtual spp::PropertyCollection GetProperties();
    protected:
        FilterPreset* m_Preset;
        spp::Vector<GenericFilterPresetEntry>   m_Values;
        std::string m_FilterName;
        std::string m_Name;
};

}
