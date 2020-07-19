
#include <QFile>
#include <QDebug>
#include <sstream>
#include <functional>

#include <utils/conv.hpp>
#include <utils/curve.hpp>
#include <utils/preset.hpp>

#include <libcommon/stringutils.hpp>

#include <libgraphics/curve.hpp>
#include <libgraphics/filterpreset.hpp>
#include <libgraphics/filterpresetcollection.hpp>

#include <log/log.hpp>

namespace blacksilk {
namespace helpers {

static const char validChars[]          = "abcdefghijklmnopqrstuvwxyz0123456789-=";
static const size_t validCharsLength    = sizeof( validChars ) / sizeof( char );

static const char numberChars[]         = "0123456789";
static const size_t numberCharsLength   = sizeof( numberChars ) / sizeof( char );

static const char signChars[]           = "-=";
static const size_t signCharsLength     = sizeof( signChars ) / sizeof( char );

static const char seperatorChars[]          = "\t, \n";
static const size_t seperatorCharsLength    = sizeof( seperatorChars ) / sizeof( char );

static std::string remove_trailing_whitespaces( const std::string& inp ) {
    if( inp.empty() ) {
        return inp;
    }

    const auto begin_it = std::find_if_not(
                              inp.begin(),
                              inp.end(),
    []( const char& val ) { return ( val == ' ' ); }
                          );
    const auto end_it = std::find_if_not(
                            inp.rbegin(),
                            inp.rend(),
    []( const char& val ) { return ( val == ' ' ); }
                        );
    std::string detrailed = std::string( begin_it, end_it.base() );

    return detrailed;
}

static std::string preprocess_string( const std::string& inp ) {

    std::stringstream is( inp );
    std::string line;
    std::stringstream os;

    while( libcommon::stringutils::safeGetline( is, line ) ) {
        os << line << " ";
    }

    return os.str();
}

template < class _It >
static _It    get_first_valid_char( _It first, _It last ) {
    bool in_string( false );

    while( first != last ) {
        if( ( *first ) == '"' ) {
            in_string = !in_string;
        }

        if( !in_string ) {
            const auto lower_case_value( ::tolower( *first ) );

            for( size_t i = 0; validCharsLength > i; ++i ) {
                if( validChars[i] == lower_case_value ) {
                    return first;
                }
            }
        }

        ++first;
    }

    return last;
}
template < class _It, class _Val >
static _It  get_first( _It first, _It last, const _Val& value ) {
    return std::find(
               first,
               last,
               value
           );
}
template < class _It >
static _It get_corresponding_parenthesis( _It first, _It last, int parenthesis_count ) {
    bool in_string( false );

    while( first != last ) {
        if( parenthesis_count == 0 ) {
            return first;
        }

        if( ( *first ) == '"' ) {
            in_string = !in_string;
        }

        if( !in_string ) {
            if( ( *first ) == '(' ) {
                ++parenthesis_count;
            } else if( ( *first ) == ')' ) {
                --parenthesis_count;
            }
        }

        ++first;
    }

    return last;
}
template < class _It, class _Val >
static _It  get_first_non_string( _It first, _It last, const _Val& value ) {
    bool in_string( false );

    while( first != last ) {
        if( ( *first ) == '"' ) {
            in_string = !in_string;
        } else if( ( *first ) == value ) {
            if( !in_string ) {
                return first;
            }
        }

        ++first;
    }

    return last;
}
template < class _It, class _Val >
static _It get_first_top( _It first, _It last, const _Val& value ) {
    bool in_string( false );

    const _Val lower_inp( ::tolower( value ) );
    int level_count( 0 );

    while( first != last ) {
        if( ( *first ) == '"' ) {
            in_string = !in_string;
        }

        if( !in_string ) {
            const _Val lower_case_value( ::tolower( *first ) );

            if( lower_case_value == '(' ) {
                ++level_count;
                ++first;
                continue;
            } else if( lower_case_value == ')' ) {
                --level_count;
                ++first;
                continue;
            }

            if( ( lower_case_value == lower_inp ) && ( level_count == 0 ) ) {
                return first;
            }
        }

        ++first;
    }

    return last;
}
template < class _It >
static _It get_first_block_seperator( _It first, _It last ) {
    bool in_string( false );

    while( first != last ) {
        if( ( *first ) == '"' ) {
            in_string = !in_string;
        }

        if( !in_string ) {
            const auto lower_case_value( ::tolower( *first ) );

            for( size_t i = 0; seperatorCharsLength > i; ++i ) {
                if( seperatorChars[i] == lower_case_value ) {
                    return first;
                }
            }
        }

        ++first;
    }

    return last;
}
template < class _It >
static _It get_first_seperator( _It first, _It last ) {
    bool in_string( false );

    while( first != last ) {
        if( ( *first ) == '"' ) {
            in_string = !in_string;
        }

        if( !in_string ) {
            const auto lower_case_value( ::tolower( *first ) );

            for( size_t i = 0; seperatorCharsLength > i; ++i ) {
                if( seperatorChars[i] == lower_case_value ) {
                    return first;
                }
            }
        }

        ++first;
    }

    return last;
}
template < class _It >
static _It get_first_top_seperator( _It first, _It last ) {
    bool in_string( false );

    int level_count( 0 );

    while( first != last ) {
        const auto lower_case_value( ::tolower( *first ) );

        if( ( *first ) == '"' ) {
            in_string = !in_string;
        }

        if( !in_string ) {
            if( lower_case_value == '(' ) {
                ++level_count;
                ++first;
                continue;
            } else if( lower_case_value == ')' ) {
                --level_count;
                ++first;
                continue;
            }

            for( size_t i = 0; seperatorCharsLength > i; ++i ) {
                if( ( seperatorChars[i] == lower_case_value ) && ( level_count == 0 ) )  {
                    return first;
                }
            }
        }

        ++first;
    }

    return last;
}

template < class _It >
static _It    rget_first_valid_char( _It first, _It last ) {
    bool in_string( false );

    while( first != last ) {
        if( ( *first ) == '"' ) {
            in_string = !in_string;
        }

        if( !in_string ) {
            const auto lower_case_value( ::tolower( *first ) );

            for( size_t i = 0; validCharsLength > i; ++i ) {
                if( validChars[i] == lower_case_value ) {
                    return first;
                }
            }
        }

        --first;
    }

    return last;
}
template < class _It, class _Val >
static _It rget_first( _It first, _It last, const _Val& value ) {
    bool in_string( false );

    while( first != last ) {
        if( ( *first ) == '"' ) {
            in_string = !in_string;
        }

        if( !in_string ) {
            if( ( *first ) == value ) {
                return first;
            }
        }

        --first;
    }

    return last;
}
template < class _It >
static _It rget_first_seperator( _It first, _It last ) {
    bool in_string( false );

    while( first != last ) {
        if( ( *first ) == '"' ) {
            in_string = !in_string;
        }

        if( !in_string ) {
            const auto lower_case_value( ::tolower( *first ) );

            for( size_t i = 0; seperatorCharsLength > i; ++i ) {
                if( seperatorChars[i] == lower_case_value ) {
                    return first;
                }
            }
        }

        --first;
    }

    return last;
}

template < class _It >
static bool is_number( const _It& value ) {
    for( size_t i = 0; numberCharsLength > i; ++i ) {
        if( numberChars[i] == ( *value ) ) {
            return true;
        }
    }

    return false;
}
template < class _It >
static bool is_character( const _It& value ) {
    for( size_t i = 0; validCharsLength > i; ++i ) {
        if( validChars[i] == ( *value ) ) {
            return true;
        }
    }

    return false;
}
template < class _It >
static bool is_sign( const _It& value ) {
    for( size_t i = 0; signCharsLength > i; ++i ) {
        if( signChars[i] == ( *value ) ) {
            return true;
        }
    }

    return false;
}
template < class _It >
static bool is_letter( const _It& value ) {
    return is_character( value ) && !is_number( value ) && !is_sign( value );
}

static std::string remove_whitespaces( const std::string& value ) {
    if( value.empty() ) {
        return value;
    }

    auto front_whitespace = helpers::get_first_valid_char(
                                value.begin(),
                                value.end()
                            );

    auto back_whitespace = helpers::rget_first_seperator(
                               value.begin() + ( value.size() - 1 ),
                               front_whitespace
                           );

    if( back_whitespace == front_whitespace ) {
        back_whitespace = value.begin() + ( value.size() - 1 );
    }

    return std::string(
               front_whitespace,
               back_whitespace + 1
           );
}

}

typedef std::pair <
blacksilk::EPresetValueType,
          std::string
          > PresetValueTypeIdentifierPair;
static PresetValueTypeIdentifierPair makePresetValueTypeIdentifierPair( const blacksilk::EPresetValueType& type, const std::string& id ) {
    return PresetValueTypeIdentifierPair(
               type,
               id
           );
}
static const PresetValueTypeIdentifierPair presetIdentifiers[] = {
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::X, "x" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::Y, "y" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::Strength, "strength" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::Cascade, "cascade" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::Center, "center" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::Color, "color" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::Factor, "factor" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::Highlights, "highlights" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::Radius, "radius" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::Shadows, "shadows" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::Threshold, "threshold" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::TupleList, "points" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::FactorList, "factors" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::FactorList, "values" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::Name, "name" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::Category, "category" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::Identifier, "id" ),
    makePresetValueTypeIdentifierPair( blacksilk::EPresetValueType::Identifier, "identifier" )
};
static const size_t presetIdentifiersLength = sizeof( presetIdentifiers ) / sizeof( PresetValueTypeIdentifierPair );

bool getPresetValueTypeKeyword( const EPresetValueType& type, std::string& identifier ) {
    for( size_t i = 0; presetIdentifiersLength > i; ++i ) {
        if( presetIdentifiers[i].first == type ) {
            identifier = presetIdentifiers[i].second;
            return true;
        }
    }

    return false;
}
bool getPresetValueName( const std::string& identifier, EPresetValueType& type ) {
    for( size_t i = 0; presetIdentifiersLength > i; ++i ) {
        if( libcommon::stringutils::iequals( presetIdentifiers[i].second, identifier ) ) {
            type = presetIdentifiers[i].first;
            return true;
        }
    }

    return false;
}

bool checkPresetValueTypeKeyword( const EPresetValueType& type, const std::string& identifier ) {
    std::string localIdentifier;
    const auto returnedKeyword = getPresetValueTypeKeyword( type, localIdentifier );

    if( returnedKeyword ) {
        return libcommon::stringutils::iequals(
                   localIdentifier,
                   identifier
               );
    }

    return false;
}


typedef std::pair <
std::string,
    libgraphics::formats::RGB8::t
    > ColorPresetNamePair;
ColorPresetNamePair makeColorPresetNamePair( const std::string& name, const libgraphics::formats::RGB8::t& value ) {
    return std::make_pair( name, value );
}
static const ColorPresetNamePair colorPresetNames[] = {
    makeColorPresetNamePair( "red", libgraphics::makeRGB8( 255, 0, 0 ) ),
    makeColorPresetNamePair( "green", libgraphics::makeRGB8( 0, 255, 0 ) ),
    makeColorPresetNamePair( "blue", libgraphics::makeRGB8( 0, 0, 255 ) ),
    makeColorPresetNamePair( "yellow", libgraphics::makeRGB8( 255, 255, 0 ) ),
    makeColorPresetNamePair( "white", libgraphics::makeRGB8( 255, 255, 255 ) ),
    makeColorPresetNamePair( "grey", libgraphics::makeRGB8( 128, 128, 128 ) ),
    makeColorPresetNamePair( "black", libgraphics::makeRGB8( 0, 0, 0 ) )
};
static const size_t colorPresetNamesSize = sizeof( colorPresetNames ) / sizeof( ColorPresetNamePair );



template < class _t_color_type >
bool toColor(
    const PresetValue&  value,
    typename _t_color_type::t&  color
) {
    if( ( value.type() == blacksilk::EPresetValueType::Color ) && ( value.children().size() == _t_color_type::Channels ) ) {
        typename _t_color_type::t localColor;

        size_t colorValueIndex  = 0;
        const auto children     = value.children();

        for( auto it = children.begin(); it != children.end(); ++it ) {
            if( ( *it ).type() == blacksilk::EPresetValueType::Decimal ) {
                int decimalValue( 0 );
                const auto successfullyQueriedDecimal = PresetValue::toDecimal(
                        ( *it ),
                        decimalValue
                                                        );
                assert( successfullyQueriedDecimal );

                if( successfullyQueriedDecimal ) {
                    localColor.Values[colorValueIndex] = decimalValue;
                } else {
                    return false;
                }
            }

            ++colorValueIndex;
        }

        color = localColor;

        return true;
    }

    return false;
}

bool PresetValue::toRGB8(
    const PresetValue&  value,
    libgraphics::formats::RGB8::t&  color
) {
    return toColor<libgraphics::formats::RGB8>(
               value,
               color
           );
}

bool PresetValue::toRGB16(
    const PresetValue&  value,
    libgraphics::formats::RGB16::t&  color
) {
    return toColor<libgraphics::formats::RGB16>(
               value,
               color
           );
}

bool PresetValue::toRGB16S(
    const PresetValue&  value,
    libgraphics::formats::RGB16S::t&  color
) {
    return toColor<libgraphics::formats::RGB16S>(
               value,
               color
           );
}

bool PresetValue::toRGB32(
    const PresetValue&  value,
    libgraphics::formats::RGB32::t&  color
) {
    return toColor<libgraphics::formats::RGB32>(
               value,
               color
           );
}

bool PresetValue::toRGB32S(
    const PresetValue&  value,
    libgraphics::formats::RGB32S::t&  color
) {
    return toColor<libgraphics::formats::RGB32S>(
               value,
               color
           );
}


bool PresetValue::toFactorList(
    const PresetValue& value,
    std::vector<float>& factorList
) {

    switch( value.type() ) {
        case blacksilk::EPresetValueType::FactorList: {
            float localFloat( 0.0f );

            for( auto it = value.children().begin(); it != value.children().end(); ++it ) {
                std::stringstream ss;
                ss << ( *it ).value();
                ss >> localFloat;

                factorList.push_back( localFloat );
            }

            return true;
        }

        default:
            break;
    }

    return false;
}

bool PresetValue::toTupleList(
    const PresetValue& value,
    std::vector< std::pair<float, float> >& factorList
) {
    switch( value.type() ) {
        case blacksilk::EPresetValueType::TupleList: {
            float f_first( 0.0f );
            float f_second( 0.0f );

            for( auto it = value.children().begin(); it != value.children().end(); ++it ) {
                std::stringstream ss;
                ss << ( *it ).value();
                ss >> f_second;

                ss.clear();
                ss << ( *it ).keyword();
                ss >> f_first;

                factorList.push_back( std::make_pair( f_first, f_second ) );
            }

            return true;
        }

        default:
            break;
    }

    return false;
}

bool PresetValue::toFloat(
    const PresetValue& value,
    float& fvalue
) {
    switch( value.type() ) {
        case blacksilk::EPresetValueType::Cascade:
        case blacksilk::EPresetValueType::Factor:
        case blacksilk::EPresetValueType::Threshold:
        case blacksilk::EPresetValueType::Radius: {
            float localValue            = 0.0f;
            const std::string& strValue = value.value();

            std::stringstream ss;

            try {
                ss << strValue;
                ss >> localValue;
            } catch( ... ) { return false; }

            if( ss.fail() ) {
                return false;
            }

            fvalue = localValue;
            return true;
        }

        default:
            return false;
    }
}

bool PresetValue::toString(
    const PresetValue& value,
    std::string& identifierName
) {
    if( value.type() == blacksilk::EPresetValueType::Identifier ) {
        identifierName = value.value();
        return true;
    }

    return false;
}

std::string PresetValue::presetToString( const libgraphics::FilterPreset& preset, const std::string name ) {
    blacksilk::PresetValue value( blacksilk::EPresetValueType::Root );
    libfoundation::app::EFilter::t type = libfoundation::app::EFilter::fromName( preset.filterName() );

    if( !blacksilk::toPresetValue( preset, type, name, value ) ) {
        assert( false );
        qWarning() << "Failed to convert libgraphics preset object to bs-format preset( presetName='" << preset.name().c_str() << "', filterName='" << preset.filterName().c_str() << "' ).";
        return std::string();
    }

    return value.print();
}

libgraphics::FilterPreset PresetValue::stringToPreset( const std::string str ) {

    PresetValue root( blacksilk::EPresetValueType::Root );
    const auto successfullyParsed = PresetValue::parse( root, str );
    LOGB_WARNING( !successfullyParsed, "Could not parse " + str );
    assert( successfullyParsed );

    // blacksilk::createFilterPreset( 0, preset, root );
    libgraphics::FilterPresetCollection presets;
    const auto successfullyConverted = blacksilk::createFilterPresetCollection(
                                           &presets,
                                           presets,
                                           root
                                       );
    LOGB_WARNING( !successfullyConverted, "Could not convert " + str );
    assert( successfullyConverted );
    assert( presets.count() > 0 );

    return presets.front();
}

bool PresetValue::toDecimal(
    const PresetValue& value,
    int& decimal
) {
    switch( value.type() ) {
        case blacksilk::EPresetValueType::Decimal: {
            int localValue              = 0;
            const std::string& strValue = value.value();

            std::stringstream ss;

            try {
                ss << strValue;
                ss >> localValue;
            } catch( ... ) { return false; }

            if( ss.fail() ) {
                return false;
            }

            decimal = localValue;
            return true;
        }

        default:
            return false;
    }
}

std::string iterateChildren( const PresetValue& value, std::string seperator = ", " );
namespace printers {
std::string type_Default( const PresetValue& value ) {
    ( void )value;

    return "";
}
std::string type_Parameter( const PresetValue& value ) {
    assert( !value.children().empty() );
    std::string str = value.keyword() + "=";
    str += iterateChildren( value );
    return str;
}
std::string type_Name( const PresetValue& value ) {
    assert( value.children().empty() );
    return "name(" + value.value() + ")";
}
std::string type_Category( const PresetValue& value ) {
    assert( value.children().empty() );
    return "category(" + value.value() + ")";
}
std::string type_TupleList( const PresetValue& value ) {
    std::string str = value.keyword() + "( ";
    str += iterateChildren( value ) + " )";
    return str;
}
std::string type_Tuple( const PresetValue& value ) {
    std::string str = value.keyword() + " : " + value.value();
    return str;
}
std::string type_FactorList( const PresetValue& value ) {
    std::string str = value.keyword() + "( ";
    str += iterateChildren( value ) + " )";
    return str;
}
std::string type_Factor( const PresetValue& value ) {
    if( !value.keyword().empty() ) {
        return value.keyword() + "( " + value.value() + " )";
    }

    return value.value();
}
std::string type_Decimal( const PresetValue& value ) {
    return value.value();
}
std::string type_Root( const PresetValue& value ) {
    return iterateChildren( value, "\n" );
}
}

typedef std::function<std::string( const PresetValue& )>      FnPresetValueToString;
typedef std::pair<EPresetValueType, FnPresetValueToString>   MapPresetTypeToStringEntry;

static const MapPresetTypeToStringEntry mapPresetTypeToString[] = {
    std::make_pair( EPresetValueType::Root, printers::type_Root ),

    std::make_pair( EPresetValueType::Invalid, printers::type_Default ),
    std::make_pair( EPresetValueType::Parameter, printers::type_Parameter ),
    std::make_pair( EPresetValueType::Name, printers::type_Name ),
    std::make_pair( EPresetValueType::Category, printers::type_Name ),
    std::make_pair( EPresetValueType::TupleList, printers::type_TupleList ),
    std::make_pair( EPresetValueType::Tuple, printers::type_Tuple ),

    std::make_pair( EPresetValueType::FactorList, printers::type_FactorList ),
    std::make_pair( EPresetValueType::Shadows, printers::type_FactorList ),
    std::make_pair( EPresetValueType::Highlights, printers::type_FactorList ),
    std::make_pair( EPresetValueType::Color, printers::type_FactorList ),
    std::make_pair( EPresetValueType::Center, printers::type_FactorList ),

    std::make_pair( EPresetValueType::Factor, printers::type_Factor ),
    std::make_pair( EPresetValueType::Cascade, printers::type_Factor ),
    std::make_pair( EPresetValueType::X, printers::type_Factor ),
    std::make_pair( EPresetValueType::Y, printers::type_Factor ),
    std::make_pair( EPresetValueType::Radius, printers::type_Factor ),
    std::make_pair( EPresetValueType::Strength, printers::type_Factor ),
    std::make_pair( EPresetValueType::Threshold, printers::type_Factor ),
    std::make_pair( EPresetValueType::Cascade, printers::type_Factor ),

    std::make_pair( EPresetValueType::Decimal, printers::type_Decimal )
};
static const size_t mapPresetTypeToStringLen = sizeof( mapPresetTypeToString ) / sizeof( MapPresetTypeToStringEntry );
std::string iterateChildren( const PresetValue& value, std::string seperator ) {
    std::string data;
    data.reserve( 1024 );

    for( auto it = value.children().begin(); it != value.children().end(); ++it ) {
        const std::string childBuf = ( *it ).print();
        assert( !childBuf.empty() );

        if( !childBuf.empty() ) {
            data += childBuf;

            if( ( it + 1 ) != value.children().end() ) {
                data += seperator;
            }
        }
    }

    return data;
}

bool getPrinterByType( FnPresetValueToString& printer, EPresetValueType type ) {
    for( size_t i = 0; mapPresetTypeToStringLen > i; ++i ) {
        if( mapPresetTypeToString[i].first == type ) {
            printer = mapPresetTypeToString[i].second;
            return true;
        }
    }

    return false;
}

std::string PresetValue::print() const {
    std::string data;
    data.reserve( 1024 );

    FnPresetValueToString   printer;
    const auto successfullyFoundPrinter = getPrinterByType(
            printer,
            this->type()
                                          );
    assert( successfullyFoundPrinter );

    if( !successfullyFoundPrinter ) {
        qDebug() << "Failed to find printer for PresetValue with keyword='" << this->keyword().c_str() << "' and value='" << this->value().c_str() << "'.";
        return "";
    }

    const std::string buf = printer(
                                *this
                            );
    data += buf;

    return data;
}


/// parser functions
namespace helpers {

PresetValue parsePresetValue_TupleList( bool& success, const std::string& param_name,  std::string::iterator& begin, std::string::iterator& end ) {
    auto valid_char_it = helpers::get_first_valid_char( begin, end );

    if( valid_char_it == end ) {
        success = false;
        return PresetValue();
    }

    auto first_seperator = helpers::get_first_non_string(
                               valid_char_it,
                               end,
                               ','
                           );

    if( first_seperator == end ) {
        first_seperator = helpers::get_first_non_string( valid_char_it, end, ')' );
    }

    PresetValue value(
        blacksilk::EPresetValueType::TupleList,
        param_name
    );
    value.keyword() = helpers::remove_trailing_whitespaces( value.keyword() );
    value.value()   = helpers::remove_trailing_whitespaces( value.value() );

    auto begin_range( begin );
    auto end_range( first_seperator );


    do {
        begin_range = helpers::get_first_valid_char(
                          begin_range,
                          end_range
                      );

        if( begin_range == end_range ) {
            break;
        }

        auto dot_seperator = helpers::get_first_non_string(
                                 begin_range,
                                 end_range,
                                 ':'
                             );

        if( dot_seperator == end_range ) {
            break;
        }

        auto keyword_range_begin( begin_range );
        auto keyword_range_end( dot_seperator ); ///( helpers::rget_first_valid_char( dot_seperator, begin_range ) );
        auto value_range_begin( helpers::get_first_valid_char( dot_seperator + 1, end_range ) );
        auto value_range_end( end_range );

        value.children().emplace_back(
            blacksilk::EPresetValueType::Tuple,
            std::string( keyword_range_begin, keyword_range_end ),
            std::string( value_range_begin, value_range_end )
        );
        value.children().back().keyword() = helpers::remove_trailing_whitespaces( value.children().back().keyword() );
        value.children().back().value()   = helpers::remove_trailing_whitespaces( value.children().back().value() );
        /// \todo should check if begin_range-end_range is a number.

        if( end_range == end ) {
            break;
        }

        begin_range = end_range + 1;
        end_range   = helpers::get_first_non_string(
                          begin_range,
                          end,
                          ','
                      );
    } while( true );

    success = true;
    return value;
}

PresetValue parsePresetValue_FactorList( bool& success, const std::string& param_name, std::string::iterator& begin, std::string::iterator& end ) {
    auto valid_char_it = helpers::get_first_valid_char( begin, end );

    if( valid_char_it == end ) {
        success = false;
        return PresetValue();
    }

    auto first_seperator = helpers::get_first_seperator(
                               valid_char_it,
                               end
                           );

    PresetValue value(
        blacksilk::EPresetValueType::FactorList,
        param_name
    );

    auto begin_range( begin );
    auto end_range( first_seperator );

    size_t color_name_index( 0 );

    do {
        std::stringstream ss;
        ss << color_name_index;

        begin_range = helpers::get_first_valid_char(
                          begin_range,
                          end_range
                      );

        if( begin_range == end_range ) {
            break;
        }

        value.children().emplace_back(
            blacksilk::EPresetValueType::Factor,
            ss.str(),
            std::string( begin_range, end_range )
        );
        value.children().back().keyword() = helpers::remove_trailing_whitespaces( value.children().back().keyword() );
        value.children().back().value()   = helpers::remove_trailing_whitespaces( value.children().back().value() );

        /// \todo should check if begin_range-end_range is a number.

        if( end_range == end ) {
            break;
        }

        begin_range = end_range + 1;
        end_range   = helpers::get_first_non_string(
                          begin_range,
                          end,
                          ','
                      );

        ++color_name_index;
    } while( true );

    success = true;
    return value;
}

PresetValue parsePresetValue_Float( bool& success, const std::string& param_name, std::string::iterator& begin, std::string::iterator& end ) {
    /// \todo check if the specified range is a floating point number.
    const std::string value( begin, end );

    success = true;
    return PresetValue(
               blacksilk::EPresetValueType::Factor,
               param_name,
               value
           );
}

PresetValue parsePresetValue_Category( bool& success, const std::string& param_name, std::string::iterator& begin, std::string::iterator& end ) {
    /// \todo check if the specified range is a floating point number.
    const std::string value( begin, end );

    success = true;
    return PresetValue(
               blacksilk::EPresetValueType::Category,
               param_name,
               value
           );
}
PresetValue parsePresetValue_Name( bool& success, const std::string& param_name, std::string::iterator& begin, std::string::iterator& end ) {
    /// \todo check if the specified range is a floating point number.
    const std::string value( begin, end );

    success = true;
    return PresetValue(
               blacksilk::EPresetValueType::Name,
               param_name,
               value
           );
}
PresetValue parsePresetValue_Identifier( bool& success, const std::string& param_name, std::string::iterator& begin, std::string::iterator& end ) {
    /// \todo check if the specified range is a floating point number.
    const std::string value( begin, end );

    success = true;
    return PresetValue(
               blacksilk::EPresetValueType::Identifier,
               param_name,
               value
           );
}
PresetValue parsePresetValue_Color( bool& success, const std::string& param_name, std::string::iterator& begin, std::string::iterator& end ) {
    auto valid_char_it = helpers::get_first_valid_char( begin, end );

    if( valid_char_it == end ) {
        success = false;
        return PresetValue();
    }

    auto first_seperator = helpers::get_first_seperator(
                               valid_char_it,
                               end
                           );

    static const std::string color_name = "RGBA";

    if( first_seperator == end ) {
        const std::string colorName( begin, end );

        for( size_t i = 0; colorPresetNamesSize > i; ++i ) {
            if( libcommon::stringutils::iequals( colorName, colorPresetNames[i].first ) ) {

                PresetValue value(
                    blacksilk::EPresetValueType::Color,
                    param_name,
                    colorName
                );
                success = true;

                assert( 4 >= colorPresetNames[i].second.Count );

                for( size_t j = 0; colorPresetNames[i].second.Count > j; ++j ) {
                    std::stringstream ss;
                    ss << ( int )colorPresetNames[i].second.Values[j];
                    value.children().push_back(
                        PresetValue(
                            EPresetValueType::Decimal,
                            std::string() + color_name[j],
                            ss.str()
                        )
                    );
                }


                return value;
            }
        }

        success = false;
        qDebug() << "Failed to find color preset for '" << colorName.c_str() << "'.";
        return PresetValue();

    } else {
        PresetValue value(
            blacksilk::EPresetValueType::Color,
            param_name
        );

        auto begin_range( begin );
        auto end_range( first_seperator );

        size_t color_name_index( 0 );

        do {
            begin_range = helpers::get_first_valid_char(
                              begin_range,
                              end_range
                          );

            if( begin_range == end_range ) {
                break;
            }

            value.children().emplace_back(
                blacksilk::EPresetValueType::Decimal,
                std::string() + color_name[color_name_index],
                std::string( begin_range, end_range )
            );
            /// \todo should check if begin_range-end_range is a number.

            if( end_range == end ) {
                break;
            }

            begin_range = end_range + 1;
            end_range   = helpers::get_first_non_string(
                              begin_range,
                              end,
                              ','
                          );

            ++color_name_index;
        } while( true );

        success = true;
        return value;
    }

    success = false;
    return PresetValue();
}

typedef std::function<PresetValue( bool&, const std::string&, std::string::iterator&, std::string::iterator& )> ParserFn;
typedef std::pair<std::string, ParserFn> PresetParserPair;
PresetParserPair makePresetParserPair( const std::string& name, const ParserFn& fn ) {
    return std::make_pair( name, fn );
}
static PresetParserPair presetParserFunctions[] = {
    /// floating-point values
    makePresetParserPair( "cascade", &parsePresetValue_Float ),
    makePresetParserPair( "factor", &parsePresetValue_Float ),
    makePresetParserPair( "balance", &parsePresetValue_Float ),
    makePresetParserPair( "weight", &parsePresetValue_Float ),
    makePresetParserPair( "radius", &parsePresetValue_Float ),
    makePresetParserPair( "strength", &parsePresetValue_Float ),
    makePresetParserPair( "threshold", &parsePresetValue_Float ),
    makePresetParserPair( "x", &parsePresetValue_Float ),
    makePresetParserPair( "y", &parsePresetValue_Float ),

    /// color values
    makePresetParserPair( "highlights", &parsePresetValue_Color ),
    makePresetParserPair( "shadows", &parsePresetValue_Color ),
    makePresetParserPair( "color", &parsePresetValue_Color ),

    /// factor lists
    makePresetParserPair( "factors", &parsePresetValue_FactorList ),
    makePresetParserPair( "values", &parsePresetValue_FactorList ),
    makePresetParserPair( "center", &parsePresetValue_FactorList ),

    /// tuple lists
    makePresetParserPair( "points", &parsePresetValue_TupleList ),

    /// IDs
    makePresetParserPair( "name", &parsePresetValue_Name ),
    makePresetParserPair( "category", &parsePresetValue_Category ),
    makePresetParserPair( "identifier", &parsePresetValue_Identifier ),
    makePresetParserPair( "id", &parsePresetValue_Identifier )
};
static const size_t presetParserFunctionsSize = sizeof( presetParserFunctions ) / sizeof( PresetParserPair );

PresetValue parsePresetParameterElement( bool& success, std::string::iterator& begin, std::string::iterator& end ) {
    /// skip to the first valid char
    auto valid_char_it = helpers::get_first_valid_char( begin, end );

    if( valid_char_it == end ) {
        success = false;
        return PresetValue();
    }

    /// assign a new first iterator
    begin = valid_char_it;

    /// parse till first seperator
    auto seperator_char_it = helpers::get_first_seperator( valid_char_it, end );

#ifdef PRESET_PARSER_DEBUG_OUTPUT
    {
        std::string str( valid_char_it, end );
        qDebug() << "(PresetParser) Current selected range:" << str.c_str();
    }
#endif

    /// find first open parenthesis
    auto open_parenthesis = helpers::get_first_non_string(
                                begin,
                                end,
                                '('
                            );

    if( open_parenthesis != end ) {
        auto close_parenthesis = helpers::get_first_non_string(
                                     open_parenthesis,
                                     end,
                                     ')'
                                 );

        if( close_parenthesis == seperator_char_it ) {
            qDebug() << "PresetParser: Failed to parse preset parameter element - closed parenthesis ')' not found - could not parse preset.";
            success = false;
            return PresetValue();
        }

        std::string raw_param_name( valid_char_it, open_parenthesis );
        assert( !raw_param_name.empty() );

        std::string param_name( helpers::remove_whitespaces( raw_param_name ) );
        assert( !param_name.empty() );

#ifdef PRESET_PARSER_DEBUG_OUTPUT
        {
            qDebug() << "Element-Name:" << param_name.c_str();
        }
#endif

        for( size_t i = 0; presetParserFunctionsSize > i; ++i ) {
            if( libcommon::stringutils::iequals( presetParserFunctions[i].first, param_name ) ) {
                auto& fn = presetParserFunctions[i].second;

                ++open_parenthesis;

                bool successfullyParsedElementValue( false );
                PresetValue presetValue = fn(
                                              successfullyParsedElementValue,
                                              param_name,
                                              open_parenthesis,
                                              close_parenthesis
                                          );
                presetValue.keyword() = helpers::remove_trailing_whitespaces( presetValue.keyword() );
                presetValue.value()   = helpers::remove_trailing_whitespaces( presetValue.value() );

                assert( successfullyParsedElementValue );

                success = successfullyParsedElementValue;

                return presetValue;
            }
        }

        qDebug() << "PresetParser: " << param_name.c_str() << " is an unknown identifier - check your spelling.";
        success = false;
        return PresetValue();
    }

    qDebug() << "PresetParser: Failed to parse preset parameter element - open parenthesis ')' not found - could not parse preset.";
    success = false;
    return PresetValue();

}


PresetValue parsePresetParameter( bool& success, std::string::iterator& begin, std::string::iterator& end ) {
    auto current_equals_it = helpers::get_first_non_string(
                                 begin,
                                 end,
                                 '='
                             );

    if( current_equals_it == end ) {
        qDebug() << "Failed to parse preset parameter - expected '='.";
        success = false;
        return PresetValue();
    }

    std::string param_name(
        begin,
        current_equals_it
    );
#ifdef PRESET_PARSER_DEBUG_OUTPUT
    {
        qDebug() << "Parameter-Name:" << param_name.c_str();
    }
#endif


    auto comma_seperator = helpers::get_first_top(
                               current_equals_it,
                               end,
                               ','
                           );

    if( comma_seperator == end ) {
        PresetValue retval(
            EPresetValueType::Parameter,
            param_name
        );
        retval.keyword() = helpers::remove_trailing_whitespaces( retval.keyword() );
        retval.value()   = helpers::remove_trailing_whitespaces( retval.value() );

        success = true;

        retval.children().push_back(
            PresetValue(
                EPresetValueType::Identifier,
                "",
                std::string( current_equals_it + 1, end )
            )
        );
        retval.children().back().keyword() = helpers::remove_trailing_whitespaces( retval.children().back().keyword() );
        retval.children().back().value()   = helpers::remove_trailing_whitespaces( retval.children().back().value() );


        return retval;
    } else {
        PresetValue retval(
            EPresetValueType::Parameter,
            param_name
        );
        retval.keyword() = helpers::remove_trailing_whitespaces( retval.keyword() );
        retval.value()   = helpers::remove_trailing_whitespaces( retval.value() );

        success = true;

        std::string::iterator   begin_range( current_equals_it + 1 );
        std::string::iterator   end_range( comma_seperator );

        do {
            begin_range = helpers::get_first_valid_char(
                              begin_range,
                              end_range
                          );

            if( begin_range == end_range ) {
                break;
            }

            bool successfullyParsedElement( false );
            PresetValue element = parsePresetParameterElement(
                                      successfullyParsedElement,
                                      begin_range,
                                      end_range
                                  );

            if( !successfullyParsedElement ) {
                return PresetValue();
            }

            element.keyword() = helpers::remove_trailing_whitespaces( element.keyword() );
            element.value()   = helpers::remove_trailing_whitespaces( element.value() );


            retval.children().push_back( element );

            if( end_range == end ) {
                break;
            }

            begin_range = end_range + 1;
            end_range   = helpers::get_first_top(
                              begin_range,
                              end,
                              ','
                          );
        } while( true );

        return retval;
    }

    return PresetValue();
}

PresetValue parsePresetValue( bool& success, std::string::iterator& begin, std::string::iterator& end ) {
    /// skip to the first valid char
    auto valid_char_it = helpers::get_first_valid_char( begin, end );

    if( valid_char_it == end ) {
        success = false;
        return PresetValue();
    }

    /// assign a new first iterator
    begin = valid_char_it;

    /// get the range to next parameter( x=y... )
    std::string::iterator       range_begin( end );
    std::string::iterator       range_end( end );

    auto current_equals_it = helpers::get_first_non_string(
                                 begin,
                                 end,
                                 '='
                             );

    if( current_equals_it != end ) {
        auto next_equals_it = helpers::get_first_non_string(
                                  current_equals_it + 1,
                                  end,
                                  '='
                              );

        if( next_equals_it != end ) {
            auto previous_seperator = helpers::rget_first_seperator(
                                          next_equals_it,
                                          begin
                                      );

            if( ( ( *previous_seperator ) == ';' ) || ( ( *previous_seperator ) == ',' ) || ( ( *previous_seperator ) == ' ' )
                    || ( ( *previous_seperator ) == '\t' ) ) {
                range_begin = begin;
                range_end   = previous_seperator;
            } else {
                assert( false );
            }
        } else {
            range_begin = begin;
            range_end   = end;
        }
    } else {
        range_begin = begin;
        range_end = end;
    }

#ifdef PRESET_PARSER_DEBUG_OUTPUT
    {
        std::string val( range_begin, range_end );
        qDebug() << "Parsed Range: " << val.c_str();
    }
#endif

    /// parse the current value
    bool successfullyParsedParameter( false );
    PresetValue value = helpers::parsePresetParameter(
                            successfullyParsedParameter,
                            range_begin,
                            range_end
                        );
    value.keyword() = helpers::remove_trailing_whitespaces( value.keyword() );
    value.value()   = helpers::remove_trailing_whitespaces( value.value() );

    if( successfullyParsedParameter ) {
        success = true;
        begin   = range_end;

        return value;
    }

    success = false;

    return PresetValue();
}
}

bool PresetValue::parse(
    PresetValue& root,
    std::string source
) {
    source = helpers::preprocess_string(
                 source
             );

    auto first = source.begin();
    auto last  = source.end();


    bool successfullyParsed( true );

    do {
        auto parsedPresetValue = helpers::parsePresetValue(
                                     successfullyParsed,
                                     first,
                                     last
                                 );

        if( !successfullyParsed ) {
            break;
        }

        root.children().push_back( parsedPresetValue );
    } while( true );

    return ( !root.children().empty() );
}


namespace converters {
std::string removeStringFromName( const std::string& inp ) {
    if( ( inp.front() == '"' ) && ( inp.back() == '"' ) ) {
        return std::string( inp.begin() + 1, inp.begin() + inp.size() - 1 );
    }

    return inp;
}

bool   bwmixer(
    libgraphics::FilterPresetCollection* defaultPresets,
    libgraphics::FilterPreset& preset,
    const PresetValue& value
) {
    static const libgraphics::formats::RGB8::t defaultColorValue =
        libgraphics::makeRGB8( 85, 85, 85 );
    static const float defaultFloatValue = 50.0f;

    if( ( value.children().size() == 1 ) && ( value.children().front().type() == blacksilk::EPresetValueType::Identifier ) ) {
        if( defaultPresets != nullptr ) {
            const PresetValue& currentPresetValue = value.children().front();
            const auto successfullyAcquired = defaultPresets->get(
                                                  preset,
            [&]( const libgraphics::FilterPresetCollection::FilterPresetEntry & entry ) {
                return ( libcommon::stringutils::iequals( currentPresetValue.value(), entry.preset.name() ) );
            }
                                              );
            assert( successfullyAcquired );

            if( !successfullyAcquired ) {
                qDebug() << "Failed to find filter preset with name '" << currentPresetValue.value().c_str() << "'.";
                return false;
            }

            return true;
        } else {
            qDebug() << "No preset collection available. Failed to load filter preset.";
            return false;
        }
    }

    /// set defaults
    {
        preset.floats()["HighlightWeights.R"].value = ( float )defaultColorValue.Values[0] / 255.0f;
        preset.floats()["HighlightWeights.G"].value = ( float )defaultColorValue.Values[1] / 255.0f;
        preset.floats()["HighlightWeights.B"].value = ( float )defaultColorValue.Values[2] / 255.0f;

        preset.floats()["ShadowWeights.R"].value = ( float )defaultColorValue.Values[0] / 255.0f;
        preset.floats()["ShadowWeights.G"].value = ( float )defaultColorValue.Values[1] / 255.0f;
        preset.floats()["ShadowWeights.B"].value = ( float )defaultColorValue.Values[2] / 255.0f;

        preset.floats()["Balance"].value = defaultFloatValue;
    }

    preset.filterName() = libfoundation::app::EFilter::toName(
                              libfoundation::app::EFilter::BlackAndWhiteAdaptiveMixerFilter
                          );


    /// set values from PresetValue instance.
    for( auto it = value.children().begin(); it != value.children().end(); ++it ) {
        const PresetValue& currentPresetValue = ( *it );

        switch( currentPresetValue.type() ) {
            case blacksilk::EPresetValueType::Name: {
                preset.name() = removeStringFromName( currentPresetValue.value() );
                break;
            }

            case blacksilk::EPresetValueType::Category: {
                preset.category() = removeStringFromName( currentPresetValue.value() );
                break;
            }

            case blacksilk::EPresetValueType::Color: {
                libgraphics::formats::RGB16S::t colorValue;
                const auto successfullyConvertedToColor = PresetValue::toRGB16S(
                            currentPresetValue,
                            colorValue
                        );

                EPresetValueType type;

                if( !getPresetValueName( currentPresetValue.keyword(), type ) ) {
                    break;
                }

                assert( successfullyConvertedToColor );

                if( !successfullyConvertedToColor ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                    qDebug() << "Failed to convert PresetValue type to RGB8 color.";
#endif
                    return false;
                }

                if( type == blacksilk::EPresetValueType::Shadows ) {
                    preset.floats()["ShadowWeights.R"].value = ( float )colorValue.Values[0] / 255.0f;
                    preset.floats()["ShadowWeights.G"].value = ( float )colorValue.Values[1] / 255.0f;
                    preset.floats()["ShadowWeights.B"].value = ( float )colorValue.Values[2] / 255.0f;
                } else if( type == blacksilk::EPresetValueType::Highlights ) {
                    preset.floats()["HighlightWeights.R"].value = ( float )colorValue.Values[0] / 255.0f;
                    preset.floats()["HighlightWeights.G"].value = ( float )colorValue.Values[1] / 255.0f;
                    preset.floats()["HighlightWeights.B"].value = ( float )colorValue.Values[2] / 255.0f;
                } else {
                    /** err **/
                    break;
                }


                break;
            }

            case blacksilk::EPresetValueType::Factor: {
                float floatValue( 0.0f );
                const auto successfullyConvertedToFloat = PresetValue::toFloat(
                            currentPresetValue,
                            floatValue
                        );

                assert( successfullyConvertedToFloat );

                if( !successfullyConvertedToFloat ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                    qDebug() << "Failed to convert PresetValue type to float value.";
#endif
                    return false;
                }

                preset.floats()["Balance"].value = ( floatValue / 100.0f ) - 0.5f;
                break;
            }

            default: break;
        }
    }

    return true;
}

bool   sharpen(
    libgraphics::FilterPresetCollection* defaultPresets,
    libgraphics::FilterPreset& preset,
    const PresetValue& value
) {
    static const size_t defaultThreshold = 50;
    static const size_t defaultCascadeSize = 4;
    static const float defaultCascadeValue = 100.0f;
    static const float defaultBlurRadius[defaultCascadeSize] = {
        0.7f, 1.4f, 2.8f, 5.6f
    };

    if( ( value.children().size() == 1 ) && ( value.children().front().type() == blacksilk::EPresetValueType::Identifier ) ) {
        if( defaultPresets != nullptr ) {
            const PresetValue& currentPresetValue = value.children().front();

            const auto successfullyAcquired = defaultPresets->get(
                                                  preset,
            [&]( const libgraphics::FilterPresetCollection::FilterPresetEntry & entry ) {
                return ( libcommon::stringutils::iequals( currentPresetValue.value(), entry.preset.name() ) );
            }
                                              );
            assert( successfullyAcquired );

            if( !successfullyAcquired ) {
                qDebug() << "Failed to find filter preset with name '" << currentPresetValue.value().c_str() << "'.";
                return false;
            }

            return true;
        } else {
            qDebug() << "No preset collection available. Failed to load filter preset.";
            return false;
        }
    }

    /// set default values
    {
        preset.floats()["Threshold"].value = ( float )( defaultThreshold );
        preset.ints()["NumberOfCascades"].value = defaultCascadeSize;

        for( size_t i = 0; defaultCascadeSize > i; ++i ) {
            std::stringstream ss;
            ss << i;

            const std::string valStrength( "Strength" + ss.str() );
            preset.floats()[valStrength].value = defaultCascadeValue;

            const std::string valRadius( "BlurRadius" + ss.str() );
            preset.floats()[valRadius].value = defaultBlurRadius[i];
        }
    }

    preset.filterName() = libfoundation::app::EFilter::toName(
                              libfoundation::app::EFilter::CascadedSharpenFilter
                          );

    size_t cascadeCounter( 0 );

    for( auto it = value.children().begin(); it != value.children().end(); ++it ) {

        const PresetValue& currentPresetValue = ( *it );

        switch( currentPresetValue.type() ) {
            case blacksilk::EPresetValueType::Name: {
                preset.name() = removeStringFromName( currentPresetValue.value() );
                break;
            }

            case blacksilk::EPresetValueType::Category: {
                preset.category() = removeStringFromName( currentPresetValue.value() );
                break;
            }

            case blacksilk::EPresetValueType::Factor: {
                blacksilk::EPresetValueType convertedType( blacksilk::EPresetValueType::Invalid );

                if( !getPresetValueName( currentPresetValue.keyword(), convertedType ) ) {
                    break;
                }

                if( convertedType == blacksilk::EPresetValueType::Cascade ) {
                    if( defaultCascadeSize <= cascadeCounter ) {
                        continue;
                    }

                    float floatValue( 0.0f );
                    const auto successfullyConvertedToFloat = PresetValue::toFloat(
                                currentPresetValue,
                                floatValue
                            );

                    assert( successfullyConvertedToFloat );

                    if( !successfullyConvertedToFloat ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                        qDebug() << "Failed to convert PresetValue type to float value.";
#endif
                        return false;
                    }

                    std::stringstream ss;
                    ss << cascadeCounter;

                    preset.floats()["Strength" + ss.str()].value = floatValue;

                    ++cascadeCounter;

                    break;
                } else if( convertedType == blacksilk::EPresetValueType::Threshold ) {
                    float floatValue( 0.0f );
                    const auto successfullyConvertedToFloat = PresetValue::toFloat(
                                currentPresetValue,
                                floatValue
                            );

                    assert( successfullyConvertedToFloat );

                    if( !successfullyConvertedToFloat ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                        qDebug() << "Failed to convert PresetValue type to float value.";
#endif
                        return false;
                    }

                    preset.floats()["Threshold"].value = floatValue;
                    break;
                }
            }

            default: break;
        }
    }

    return true;
}
bool   curves(
    libgraphics::FilterPresetCollection* defaultPresets,
    libgraphics::FilterPreset& preset,
    const PresetValue& value
) {
    static const int defaultCurveLength = 256; /** for 8bit per channel images **/

    if( ( value.children().size() == 1 ) && ( value.children().front().type() == blacksilk::EPresetValueType::Identifier ) ) {
        if( defaultPresets != nullptr ) {
            const PresetValue& currentPresetValue = value.children().front();

            const auto successfullyAcquired = defaultPresets->get(
                                                  preset,
            [&]( const libgraphics::FilterPresetCollection::FilterPresetEntry & entry ) {
                return ( libcommon::stringutils::iequals( currentPresetValue.value(), entry.preset.name() ) );
            }
                                              );
            assert( successfullyAcquired );

            if( !successfullyAcquired ) {
                qDebug() << "Failed to find filter preset with name '" << currentPresetValue.value().c_str() << "'.";
                return false;
            }

            return true;
        } else {
            qDebug() << "No preset collection available. Failed to load filter preset.";
            return false;
        }
    }

    preset.filterName() = libfoundation::app::EFilter::toName(
                              libfoundation::app::EFilter::CurvesFilter
                          );


    /// go through list of children
    std::vector< std::pair<float, float> >    floatingPairs;

    for( auto it = value.children().begin(); it != value.children().end(); ++it ) {
        const PresetValue& currentPresetValue = ( *it );

        switch( currentPresetValue.type() ) {

            case blacksilk::EPresetValueType::Name: {
                preset.name() = removeStringFromName( currentPresetValue.value() );
                break;
            }

            case blacksilk::EPresetValueType::Category: {
                preset.category() = removeStringFromName( currentPresetValue.value() );
                break;
            }

            /// specified points
            case blacksilk::EPresetValueType::TupleList: {
                const auto successfullyConverted = PresetValue::toTupleList(
                                                       currentPresetValue,
                                                       floatingPairs
                                                   );
                assert( successfullyConverted );

                if( !successfullyConverted ) {
                    qDebug() << "Failed to create FilterPreset from preset string. Invalid format for filter.";
                    return false;
                }

                break;
            }

            default: break;
        }
    }


    std::vector< libgraphics::Point32F >    points;
    points.resize( floatingPairs.size() );
    std::transform(
        floatingPairs.begin(),
        floatingPairs.end(),
        points.begin(),
    []( const std::pair<float, float>& p ) {
        return libgraphics::Point32F(
                   p.first, p.second
               );
    }
    );

    preset.ints()["Length"].value = defaultCurveLength;

    size_t index( 0 );

    for( auto it = points.begin(); it != points.end(); ++it ) {
        std::stringstream ss;
        ss << "Point";
        ss << index;

        preset.points()[ss.str()].value = ( *it );
        ++index;
    }

    return true;
}
bool   grain(
    libgraphics::FilterPresetCollection* defaultPresets,
    libgraphics::FilterPreset& preset,
    const PresetValue& value
) {
    static const int defaultCurveLength = 256; /** for 8bit per channel images **/

    if( ( value.children().size() == 1 ) && ( value.children().front().type() == blacksilk::EPresetValueType::Identifier ) ) {
        if( defaultPresets != nullptr ) {
            const PresetValue& currentPresetValue = value.children().front();

            const auto successfullyAcquired = defaultPresets->get(
                                                  preset,
            [&]( const libgraphics::FilterPresetCollection::FilterPresetEntry & entry ) {
                return ( libcommon::stringutils::iequals( currentPresetValue.value(), entry.preset.name() ) );
            }
                                              );
            assert( successfullyAcquired );

            if( !successfullyAcquired ) {
                qDebug() << "Failed to find filter preset with name '" << currentPresetValue.value().c_str() << "'.";
                return false;
            }

            return true;
        } else {
            qDebug() << "No preset collection available. Failed to load filter preset.";
            return false;
        }
    }

    preset.filterName() = libfoundation::app::EFilter::toName(
                              libfoundation::app::EFilter::FilmGrainFilter
                          );
    preset.ints()["MonoGrain"].value = 1; /** only mono grain is possible **/


    /// go through list of children
    std::vector< std::pair<float, float> >    floatingPairs;

    for( auto it = value.children().begin(); it != value.children().end(); ++it ) {
        const PresetValue& currentPresetValue = ( *it );

        switch( currentPresetValue.type() ) {

            /// name
            case blacksilk::EPresetValueType::Name: {
                preset.name() = removeStringFromName( currentPresetValue.value() );
                break;
            }

            case blacksilk::EPresetValueType::Category: {
                preset.category() = removeStringFromName( currentPresetValue.value() );
                break;
            }

            case blacksilk::EPresetValueType::Factor: {
                blacksilk::EPresetValueType type( blacksilk::EPresetValueType::Invalid );

                if( !getPresetValueName( currentPresetValue.keyword(), type ) ) {
                    break;
                }

                if( type != blacksilk::EPresetValueType::Radius ) {
                    break;
                }


                float radius( 0.0f );
                const auto successfullyConverted = PresetValue::toFloat(
                                                       currentPresetValue,
                                                       radius
                                                   );
                assert( successfullyConverted );

                if( !successfullyConverted ) {
                    qDebug() << "Failed to create FilterPreset from preset string. Invalid format for filter.";
                    return false;
                }

                preset.floats()["GrainBlurRadius"].value = radius;

                break;
            }

            /// specified points
            case blacksilk::EPresetValueType::TupleList: {
                const auto successfullyConverted = PresetValue::toTupleList(
                                                       currentPresetValue,
                                                       floatingPairs
                                                   );
                assert( successfullyConverted );

                if( !successfullyConverted ) {
                    qDebug() << "Failed to create FilterPreset from preset string. Invalid format for filter.";
                    return false;
                }

                break;
            }

            default: break;
        }
    }


    std::vector< libgraphics::Point32F >    points;
    points.resize( floatingPairs.size() );
    std::transform(
        floatingPairs.begin(),
        floatingPairs.end(),
        points.begin(),
    []( const std::pair<float, float>& p ) {
        return libgraphics::Point32F(
                   p.first, p.second
               );
    }
    );

    preset.ints()["Length"].value = defaultCurveLength;

    size_t index( 0 );

    for( auto it = points.begin(); it != points.end(); ++it ) {
        std::stringstream ss;
        ss << "Point";
        ss << index;

        preset.points()[ss.str()].value = ( *it );
        ++index;
    }

    return true;
}
bool   splittone(
    libgraphics::FilterPresetCollection* defaultPresets,
    libgraphics::FilterPreset& preset,
    const PresetValue& value
) {
    static const libgraphics::formats::RGB8::t defaultColorValue = libgraphics::makeRGB8( 85, 85, 85 );

    if( ( value.children().size() == 1 ) && ( value.children().front().type() == blacksilk::EPresetValueType::Identifier ) ) {
        if( defaultPresets != nullptr ) {
            const PresetValue& currentPresetValue = value.children().front();

            const auto successfullyAcquired = defaultPresets->get(
                                                  preset,
            [&]( const libgraphics::FilterPresetCollection::FilterPresetEntry & entry ) {
                return ( libcommon::stringutils::iequals( currentPresetValue.value(), entry.preset.name() ) );
            }
                                              );
            assert( successfullyAcquired );

            if( !successfullyAcquired ) {
                qDebug() << "Failed to find filter preset with name '" << currentPresetValue.value().c_str() << "'.";
                return false;
            }

            return true;
        } else {
            qDebug() << "No preset collection available. Failed to load filter preset.";
            return false;
        }
    }

    /// set defaults
    {
        preset.floats()["HighlightsFactor.R"].value = ( float )defaultColorValue.Values[0] / 255.0f;
        preset.floats()["HighlightsFactor.G"].value = ( float )defaultColorValue.Values[1] / 255.0f;
        preset.floats()["HighlightsFactor.B"].value = ( float )defaultColorValue.Values[2] / 255.0f;

        preset.floats()["ShadowsFactor.R"].value = ( float )defaultColorValue.Values[0] / 255.0f;
        preset.floats()["ShadowsFactor.G"].value = ( float )defaultColorValue.Values[1] / 255.0f;
        preset.floats()["ShadowsFactor.B"].value = ( float )defaultColorValue.Values[2] / 255.0f;

        preset.floats()["Balance"].value = 1.0f;
    }

    preset.filterName() = libfoundation::app::EFilter::toName(
                              libfoundation::app::EFilter::SplitToneFilter
                          );


    /// set values from PresetValue instance.
    for( auto it = value.children().begin(); it != value.children().end(); ++it ) {
        const PresetValue& currentPresetValue = ( *it );

        switch( currentPresetValue.type() ) {
            case blacksilk::EPresetValueType::Name: {
                preset.name() = removeStringFromName( currentPresetValue.value() );
                break;
            }

            case blacksilk::EPresetValueType::Category: {
                preset.category() = removeStringFromName( currentPresetValue.value() );
                break;
            }

            case blacksilk::EPresetValueType::Color: {
                libgraphics::formats::RGB8::t colorValue;
                const auto successfullyConvertedToColor = PresetValue::toRGB8(
                            currentPresetValue,
                            colorValue
                        );

                EPresetValueType type;

                if( !getPresetValueName( currentPresetValue.keyword(), type ) ) {
                    break;
                }

                assert( successfullyConvertedToColor );

                if( !successfullyConvertedToColor ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                    qDebug() << "Failed to convert PresetValue type to RGB8 color.";
#endif
                    return false;
                }



                if( type == blacksilk::EPresetValueType::Shadows ) {
                    preset.floats()["ShadowsFactor.R"].value = ( float )colorValue.Values[0] / 255.0f;
                    preset.floats()["ShadowsFactor.G"].value = ( float )colorValue.Values[1] / 255.0f;
                    preset.floats()["ShadowsFactor.B"].value = ( float )colorValue.Values[2] / 255.0f;
                } else if( type == blacksilk::EPresetValueType::Highlights ) {
                    preset.floats()["HighlightsFactor.R"].value = ( float )colorValue.Values[0] / 255.0f;
                    preset.floats()["HighlightsFactor.G"].value = ( float )colorValue.Values[1] / 255.0f;
                    preset.floats()["HighlightsFactor.B"].value = ( float )colorValue.Values[2] / 255.0f;
                } else {
                    /** err **/
                    break;
                }

                break;
            }

            case blacksilk::EPresetValueType::Factor: {
                float floatValue( 0.0f );
                const auto successfullyConvertedToFloat = PresetValue::toFloat(
                            currentPresetValue,
                            floatValue
                        );

                assert( successfullyConvertedToFloat );

                if( !successfullyConvertedToFloat ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                    qDebug() << "Failed to convert PresetValue type to float value.";
#endif
                    return false;
                }

                preset.floats()["Balance"].value = 0.75f + ( floatValue / 200.0f );
                break;
            }

            default: break;
        }
    }

    return true;
}
bool   vignette(
    libgraphics::FilterPresetCollection* defaultPresets,
    libgraphics::FilterPreset& preset,
    const PresetValue& value
) {

    if( ( value.children().size() == 1 ) && ( value.children().front().type() == blacksilk::EPresetValueType::Identifier ) ) {
        if( defaultPresets != nullptr ) {
            const PresetValue& currentPresetValue = value.children().front();

            const auto successfullyAcquired = defaultPresets->get(
                                                  preset,
            [&]( const libgraphics::FilterPresetCollection::FilterPresetEntry & entry ) {
                return ( libcommon::stringutils::iequals( currentPresetValue.value(), entry.preset.name() ) );
            }
                                              );
            assert( successfullyAcquired );

            if( !successfullyAcquired ) {
                qDebug() << "Failed to find filter preset with name '" << currentPresetValue.value().c_str() << "'.";
                return false;
            }

            return true;
        } else {
            qDebug() << "No preset collection available. Failed to load filter preset.";
            return false;
        }
    }

    /// set defaults
    {
        preset.floats()["Radius"].value = 50.0f;
        preset.floats()["Strength"].value = 50.0f;
        preset.floats()["X"].value = 50.0f;
        preset.floats()["Y"].value = 50.0f;
    }

    preset.filterName() = libfoundation::app::EFilter::toName(
                              libfoundation::app::EFilter::VignetteFilter
                          );


    /// set values from PresetValue instance.
    for( auto it = value.children().begin(); it != value.children().end(); ++it ) {
        const PresetValue& currentPresetValue = ( *it );

        switch( currentPresetValue.type() ) {
            case blacksilk::EPresetValueType::Name: {
                preset.name() = removeStringFromName( currentPresetValue.value() );
                break;
            }

            case blacksilk::EPresetValueType::Category: {
                preset.category() = removeStringFromName( currentPresetValue.value() );
                break;
            }

            case blacksilk::EPresetValueType::Factor: {
                blacksilk::EPresetValueType type( blacksilk::EPresetValueType::Invalid );

                if( !getPresetValueName( currentPresetValue.keyword(), type ) ) {
                    break;
                }

                static const std::pair<blacksilk::EPresetValueType, std::string> presetNameValuePairs[] = {
                    std::make_pair( blacksilk::EPresetValueType::X, "X" ),
                    std::make_pair( blacksilk::EPresetValueType::Y, "Y" ),
                    std::make_pair( blacksilk::EPresetValueType::Strength, "Strength" ),
                    std::make_pair( blacksilk::EPresetValueType::Radius, "Radius" ),
                };
                static const size_t presetNameValuePairsLen = sizeof( presetNameValuePairs ) / sizeof( std::pair<blacksilk::EPresetValueType, std::string> );

                bool                foundName( false );
                std::string         name;

                for( size_t i = 0; presetNameValuePairsLen > i; ++i ) {
                    if( type == presetNameValuePairs[i].first ) {
                        name        = presetNameValuePairs[i].second;
                        foundName   = true;
                        break;
                    }
                }

                assert( foundName );

                if( !foundName ) {
                    break;
                }


                float floatValue( 0.0f );
                const auto successfullyConvertedToFloat = PresetValue::toFloat(
                            currentPresetValue,
                            floatValue
                        );

                assert( successfullyConvertedToFloat );

                if( !successfullyConvertedToFloat ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                    qDebug() << "Failed to convert PresetValue type to float value.";
#endif
                    return false;
                }

                preset.floats()[name].value = floatValue;
                break;
            }

            default: break;
        }
    }

    return true;
}
}

typedef std::pair <
libfoundation::app::EFilter::t, std::string
>   FilterTypeNamePair;
static const FilterTypeNamePair filterTypeNameList[] = {
    std::make_pair( libfoundation::app::EFilter::BlackAndWhiteAdaptiveMixerFilter, "bwmixer" ),
    std::make_pair( libfoundation::app::EFilter::CascadedSharpenFilter, "sharpen" ),
    std::make_pair( libfoundation::app::EFilter::CurvesFilter, "curves" ),
    std::make_pair( libfoundation::app::EFilter::FilmGrainFilter, "filmgrain" ),
    std::make_pair( libfoundation::app::EFilter::SplitToneFilter, "splittone" ),
    std::make_pair( libfoundation::app::EFilter::VignetteFilter, "vignette" )
};
static const size_t filterTypeNameListLength = sizeof( filterTypeNameList ) / sizeof( FilterTypeNamePair );

typedef std::pair <
libfoundation::app::EFilter::t, std::function<bool( libgraphics::FilterPresetCollection*, libgraphics::FilterPreset&, const PresetValue& )>
>   FilterTypeConverterFn;
static const FilterTypeConverterFn filterTypeConverterFunctions[] = {
    std::make_pair( libfoundation::app::EFilter::BlackAndWhiteAdaptiveMixerFilter, &converters::bwmixer ),
    std::make_pair( libfoundation::app::EFilter::CascadedSharpenFilter, &converters::sharpen ),
    std::make_pair( libfoundation::app::EFilter::CurvesFilter, &converters::curves ),
    std::make_pair( libfoundation::app::EFilter::FilmGrainFilter, &converters::grain ),
    std::make_pair( libfoundation::app::EFilter::SplitToneFilter, &converters::splittone ),
    std::make_pair( libfoundation::app::EFilter::VignetteFilter, &converters::vignette )
};
static const size_t filterTypeConverterFunctionsLength = sizeof( filterTypeConverterFunctions ) / sizeof( FilterTypeConverterFn );

bool   createFilterPreset(
    libgraphics::FilterPresetCollection* defaultPresets,
    libgraphics::FilterPreset& preset,
    const PresetValue& value
) {
    if( value.type() != blacksilk::EPresetValueType::Parameter ) {
        assert( false );
        return false;
    }

    libfoundation::app::EFilter::t filterType( libfoundation::app::EFilter::OtherFilter );

    for( size_t index = 0; filterTypeNameListLength > index; ++index ) {
        if( libcommon::stringutils::iequals( filterTypeNameList[index].second, value.keyword() ) ) {
            filterType = filterTypeNameList[index].first;
            break;
        }
    }

    assert( filterType != libfoundation::app::EFilter::OtherFilter );

    if( filterType == libfoundation::app::EFilter::OtherFilter ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to create FilterPreset from invalid PresetValue instance.";
#endif
        return false;
    }

    bool foundFn( false );
    std::function<bool( libgraphics::FilterPresetCollection*, libgraphics::FilterPreset&, const PresetValue& )>  fn;

    for( size_t index = 0; filterTypeConverterFunctionsLength > index; ++index ) {
        if( filterTypeConverterFunctions[index].first == filterType ) {
            fn = filterTypeConverterFunctions[index].second;
            foundFn = true;
            break;
        }
    }

    assert( foundFn );

    if( !foundFn ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "Failed to create FilterPreset from invalid PresetValue instance - Couldn't find associated converter functions for filter type.";
#endif
        return false;
    }

    return fn( defaultPresets, preset, value );
}

bool createFilterPresetCollection(
    libgraphics::FilterPresetCollection* defaultPresets,
    libgraphics::FilterPresetCollection& collection,
    const PresetValue& value
) {
    if( value.type() != EPresetValueType::Root ) {
        return false;
    }

    for( auto it = value.children().begin(); it != value.children().end(); ++it ) {
        libgraphics::FilterPreset preset;

        if( !createFilterPreset( defaultPresets, preset, ( *it ) ) ) {
            return false;
        }

        collection.add( preset );
    }

    return true;
}

bool createFilterPresetCollection(
    libgraphics::FilterPresetCollection* defaultPresets,
    libgraphics::FilterPresetCollection& collection,
    const std::string& source
) {
    PresetValue presetValue( EPresetValueType::Root );

    const auto successfullyParsed = PresetValue::parse(
                                        presetValue,
                                        source
                                    );

    if( !successfullyParsed ) {
        return false;
    }

    return createFilterPresetCollection(
               defaultPresets,
               collection,
               presetValue
           );
}

namespace tree {
PresetValue makeName( const std::string& name ) {
    std::string c_name;
    c_name += "\"" + name + "\"";

    return PresetValue(
               EPresetValueType::Name,
               "name",
               c_name
           );
}
PresetValue makeCategory( const std::string& name ) {
    std::string c_name;
    c_name += "\"" + name + "\"";

    return PresetValue(
               EPresetValueType::Category,
               "category",
               c_name
           );
}
PresetValue makeFactor_Internal( const std::string& identifier, float value, EPresetValueType _type = EPresetValueType::Factor ) {
    std::string valueStr;
    std::stringstream ss;
    ss << value;
    ss >> valueStr;

    return PresetValue(
               _type,
               identifier,
               valueStr
           );
}
PresetValue makeFactor( const std::string& identifier, float value ) { return makeFactor_Internal( identifier, value, EPresetValueType::Factor ); }
PresetValue makeThreshold( const std::string& identifier, float value ) { return makeFactor_Internal( identifier, value, EPresetValueType::Threshold ); }
PresetValue makeX( const std::string& identifier, float value ) { return makeFactor_Internal( identifier, value, EPresetValueType::Y ); }
PresetValue makeY( const std::string& identifier, float value ) { return makeFactor_Internal( identifier, value, EPresetValueType::X ); }
PresetValue makeRadius( const std::string& identifier, float value ) { return makeFactor_Internal( identifier, value, EPresetValueType::Radius ); }
PresetValue makeCascade( const std::string& identifier, float value ) { return makeFactor_Internal( identifier, value, EPresetValueType::Cascade ); }

PresetValue makeDecimal( int value ) {
    std::string valueStr;
    std::stringstream ss;
    ss << value;
    ss >> valueStr;

    return PresetValue(
               EPresetValueType::Decimal,
               "",
               valueStr
           );
}

PresetValue makeFactorList_Internal( EPresetValueType type, const std::string& identifier, const std::vector<float>& factors ) {
    PresetValue value(
        type,
        identifier,
        ""
    );

    for( auto it = factors.begin(); it != factors.end(); ++it ) {
        value.children().push_back(
            makeFactor(
                "",
                ( *it )
            )
        );
    }

    return value;
}
PresetValue makeFactorList( const std::string& identifier, const std::vector<float>& factors ) {
    return makeFactorList_Internal( EPresetValueType::FactorList, identifier, factors );
}
PresetValue makeHighlights( const std::string& identifier, const std::vector<float>& factors ) {
    return makeFactorList_Internal( EPresetValueType::Highlights, identifier, factors );
}
PresetValue makeShadows( const std::string& identifier, const std::vector<float>& factors ) {
    return makeFactorList_Internal( EPresetValueType::Shadows, identifier, factors );
}

PresetValue makeTuple_Internal( EPresetValueType type, const std::pair<float, float> tuple ) {
    std::string keyStr;
    std::string valStr;
    {
        std::stringstream ss;
        ss << tuple.first;
        ss >> keyStr;
    }
    {
        std::stringstream ss;
        ss << tuple.second;
        ss >> valStr;
    }
    return PresetValue(
               type,
               keyStr,
               valStr
           );
}
PresetValue makeTuple( const std::pair<float, float> tuple ) {
    return makeTuple_Internal( EPresetValueType::Tuple, tuple );
}

PresetValue makeTupleList_Internal( EPresetValueType type, const std::string& identifier, const std::vector<std::pair<float, float> >& tupleList ) {
    PresetValue value( type, identifier, "" );

    for( auto it = tupleList.begin(); it != tupleList.end(); ++it ) {
        value.children().push_back(
            makeTuple(
                *it
            )
        );
    }

    return value;
}

PresetValue makeParameter( std::string identifier ) {
    return PresetValue(
               EPresetValueType::Parameter,
               identifier,
               identifier
           );
}

}

bool getFilterTypeName( libfoundation::app::EFilter::t filterType, std::string& out ) {
    for( size_t i = 0; filterTypeNameListLength > i; ++i ) {
        if( filterTypeNameList[i].first == filterType ) {
            out = filterTypeNameList[i].second;
            return true;
        }
    }

    return false;
}


typedef std::function<bool( const libgraphics::FilterPreset&, const std::string&, PresetValue& )>  FnFilterPresetExporter;
typedef std::pair<libfoundation::app::EFilter::t, FnFilterPresetExporter>    FilterPresetExporterPair;
namespace exporters {
namespace getters {
template < class _t_map >
bool getEntryWithName( const _t_map& presetMap, const std::string& name, typename _t_map::mapped_type::ValueType& value ) {
    for( auto it = presetMap.begin(); it != presetMap.end(); ++it ) {
        if( ( *it ).first == name ) {
            value = ( *it ).second.value;
            return true;
        }
    }

    return false;
}
}

bool bwmixer( const libgraphics::FilterPreset& preset, const std::string& presetName, PresetValue& value ) {
    std::string filterName;

    if( !getFilterTypeName( libfoundation::app::EFilter::BlackAndWhiteAdaptiveMixerFilter, filterName ) ) {
        return false;
    }

    PresetValue param = tree::makeParameter(
                            filterName
                        );

    if( presetName.empty() ) {
        if( !preset.name().empty() ) {
            param.children().push_back(
                tree::makeName(
                    preset.name()
                )
            );
        }
    } else {
        param.children().push_back(
            tree::makeName(
                presetName
            )
        );
    }

    {
        std::vector<float> factors;
        factors.resize( 3 );

        if( !getters::getEntryWithName( preset.floats(), "HighlightWeights.R", factors[0] ) ) {
            assert( false );
            return false;
        }

        if( !getters::getEntryWithName( preset.floats(), "HighlightWeights.G", factors[1] ) ) {
            assert( false );
            return false;
        }

        if( !getters::getEntryWithName( preset.floats(), "HighlightWeights.B", factors[2] ) ) {
            assert( false );
            return false;
        }

        factors[0] = factors[0] * 255.0f;
        factors[1] = factors[1] * 255.0f;
        factors[2] = factors[2] * 255.0f;

        param.children().push_back(
            tree::makeFactorList(
                "highlights",
                factors
            )
        );
    }

    {
        std::vector<float> factors;
        factors.resize( 3 );

        if( !getters::getEntryWithName( preset.floats(), "ShadowWeights.R", factors[0] ) ) {
            assert( false );
            return false;
        }

        if( !getters::getEntryWithName( preset.floats(), "ShadowWeights.G", factors[1] ) ) {
            assert( false );
            return false;
        }

        if( !getters::getEntryWithName( preset.floats(), "ShadowWeights.B", factors[2] ) ) {
            assert( false );
            return false;
        }

        factors[0] = factors[0] * 255.0f;
        factors[1] = factors[1] * 255.0f;
        factors[2] = factors[2] * 255.0f;

        param.children().push_back(
            tree::makeFactorList(
                "shadows",
                factors
            )
        );
    }

    {
        float factor( 0.0f );

        if( !getters::getEntryWithName( preset.floats(), "Balance", factor ) ) {
            assert( false );
            return false;
        }

        factor *= 100.0f;
        factor += 50.0f;

        param.children().push_back(
            tree::makeFactor(
                "weight",
                factor
            )
        );
    }

    value.children().push_back( param );

    return true;
}
bool curves( const libgraphics::FilterPreset& preset, const std::string& presetName, PresetValue& value ) {
    std::string filterName;

    if( !getFilterTypeName( libfoundation::app::EFilter::CurvesFilter, filterName ) ) {
        return false;
    }

    PresetValue param = tree::makeParameter(
                            filterName
                        );

    if( presetName.empty() ) {
        if( !preset.name().empty() ) {
            param.children().push_back(
                tree::makeName(
                    preset.name()
                )
            );
        }
    } else {
        param.children().push_back(
            tree::makeName(
                presetName
            )
        );
    }

    {
        std::vector<std::pair<float, float> >        curvePoints;

        for( auto it = preset.points().begin(); it != preset.points().end(); ++it ) {
            curvePoints.push_back(
                std::make_pair( ( *it ).second.value.x, ( *it ).second.value.y )
            );
        }

        std::sort( curvePoints.begin(), curvePoints.end() );

        param.children().push_back(
            tree::makeTupleList_Internal(
                EPresetValueType::TupleList,
                "points",
                curvePoints
            )
        );
    }

    value.children().push_back( param );

    return true;
}
bool splittone( const libgraphics::FilterPreset& preset, const std::string& presetName, PresetValue& value ) {
    std::string filterName;

    if( !getFilterTypeName( libfoundation::app::EFilter::SplitToneFilter, filterName ) ) {
        return false;
    }

    PresetValue param = tree::makeParameter(
                            filterName
                        );

    if( presetName.empty() ) {
        if( !preset.name().empty() ) {
            param.children().push_back(
                tree::makeName(
                    preset.name()
                )
            );
        }
    } else {
        param.children().push_back(
            tree::makeName(
                presetName
            )
        );
    }

    {
        std::vector<float> factors;
        factors.resize( 3 );

        if( !getters::getEntryWithName( preset.floats(), "HighlightsFactor.R", factors[0] ) ) {
            assert( false );
            return false;
        }

        if( !getters::getEntryWithName( preset.floats(), "HighlightsFactor.G", factors[1] ) ) {
            assert( false );
            return false;
        }

        if( !getters::getEntryWithName( preset.floats(), "HighlightsFactor.B", factors[2] ) ) {
            assert( false );
            return false;
        }

        factors[0] = factors[0] * 255.0f;
        factors[1] = factors[1] * 255.0f;
        factors[2] = factors[2] * 255.0f;

        param.children().push_back(
            tree::makeFactorList(
                "highlights",
                factors
            )
        );
    }

    {
        std::vector<float> factors;
        factors.resize( 3 );

        if( !getters::getEntryWithName( preset.floats(), "ShadowsFactor.R", factors[0] ) ) {
            assert( false );
            return false;
        }

        if( !getters::getEntryWithName( preset.floats(), "ShadowsFactor.G", factors[1] ) ) {
            assert( false );
            return false;
        }

        if( !getters::getEntryWithName( preset.floats(), "ShadowsFactor.B", factors[2] ) ) {
            assert( false );
            return false;
        }

        factors[0] = factors[0] * 255.0f;
        factors[1] = factors[1] * 255.0f;
        factors[2] = factors[2] * 255.0f;

        param.children().push_back(
            tree::makeFactorList(
                "shadows",
                factors
            )
        );
    }

    {
        float factor( 0.0f );

        if( !getters::getEntryWithName( preset.floats(), "Balance", factor ) ) {
            assert( false );
            return false;
        }

        float weight = ( factor - 0.75f ) * 2 * 100.0f;

        param.children().push_back(
            tree::makeFactor(
                "weight",
                weight
            )
        );
    }

    value.children().push_back( param );

    return true;
}
bool sharpen( const libgraphics::FilterPreset& preset, const std::string& presetName, PresetValue& value ) {
    std::string filterName;

    if( !getFilterTypeName( libfoundation::app::EFilter::CascadedSharpenFilter, filterName ) ) {
        return false;
    }

    PresetValue param = tree::makeParameter(
                            filterName
                        );

    if( presetName.empty() ) {
        if( !preset.name().empty() ) {
            param.children().push_back(
                tree::makeName(
                    preset.name()
                )
            );
        }
    } else {
        param.children().push_back(
            tree::makeName(
                presetName
            )
        );
    }

    int cascadeCount( 0 );
    {
        if( !getters::getEntryWithName( preset.ints(), "NumberOfCascades", cascadeCount ) ) {
            assert( false );
            return false;
        }
    }
    assert( cascadeCount > 0 );

    if( cascadeCount <= 0 ) {
        return false;
    }

    for( int i = 0; cascadeCount > i; ++i ) {
        std::stringstream ss;
        ss << i;

        float factor( 0.0f );

        if( !getters::getEntryWithName( preset.floats(), "Strength" + ss.str(), factor ) ) {
            assert( false );
            return false;
        }

        param.children().push_back(
            tree::makeCascade(
                "cascade",
                factor
            )
        );
    }

    {
        float factor( 0.0f );

        if( !getters::getEntryWithName( preset.floats(), "Threshold", factor ) ) {
            assert( false );
            return false;
        }

        param.children().push_back(
            tree::makeThreshold(
                "threshold",
                factor
            )
        );
    }

    value.children().push_back( param );

    return true;
}

bool filmgrain( const libgraphics::FilterPreset& preset, const std::string& presetName, PresetValue& value ) {
    std::string filterName;

    if( !getFilterTypeName( libfoundation::app::EFilter::FilmGrainFilter, filterName ) ) {
        return false;
    }

    PresetValue param = tree::makeParameter(
                            filterName
                        );

    if( presetName.empty() ) {
        if( !preset.name().empty() ) {
            param.children().push_back(
                tree::makeName(
                    preset.name()
                )
            );
        }
    } else {
        param.children().push_back(
            tree::makeName(
                presetName
            )
        );
    }

    {
        float factor( 0.0f );

        if( !getters::getEntryWithName( preset.floats(), "GrainBlurRadius", factor ) ) {
            assert( false );
            return false;
        }

        param.children().push_back(
            tree::makeRadius(
                "radius",
                factor
            )
        );
    }


    {
        std::vector<std::pair<float, float> >        curvePoints;

        for( auto it = preset.points().begin(); it != preset.points().end(); ++it ) {
            curvePoints.push_back(
                std::make_pair( ( *it ).second.value.x, ( *it ).second.value.y )
            );
        }

        std::sort( curvePoints.begin(), curvePoints.end() );

        param.children().push_back(
            tree::makeTupleList_Internal(
                EPresetValueType::TupleList,
                "points",
                curvePoints
            )
        );
    }

    value.children().push_back( param );

    return true;
}
bool vignette( const libgraphics::FilterPreset& preset, const std::string& presetName, PresetValue& value ) {
    std::string filterName;

    if( !getFilterTypeName( libfoundation::app::EFilter::VignetteFilter, filterName ) ) {
        return false;
    }

    PresetValue param = tree::makeParameter(
                            filterName
                        );

    if( presetName.empty() ) {
        if( !preset.name().empty() ) {
            param.children().push_back(
                tree::makeName(
                    preset.name()
                )
            );
        }
    } else {
        param.children().push_back(
            tree::makeName(
                presetName
            )
        );
    }

    {
        float factor( 0.0f );

        if( !getters::getEntryWithName( preset.floats(), "X", factor ) ) {
            assert( false );
            return false;
        }

        param.children().push_back(
            tree::makeX(
                "x",
                factor
            )
        );
    }

    {
        float factor( 0.0f );

        if( !getters::getEntryWithName( preset.floats(), "Y", factor ) ) {
            assert( false );
            return false;
        }

        param.children().push_back(
            tree::makeY(
                "y",
                factor
            )
        );
    }

    {
        float factor( 0.0f );

        if( !getters::getEntryWithName( preset.floats(), "Strength", factor ) ) {
            assert( false );
            return false;
        }

        param.children().push_back(
            tree::makeFactor(
                "strength",
                factor
            )
        );
    }

    {
        float factor( 0.0f );

        if( !getters::getEntryWithName( preset.floats(), "Radius", factor ) ) {
            assert( false );
            return false;
        }

        param.children().push_back(
            tree::makeRadius(
                "radius",
                factor
            )
        );
    }


    value.children().push_back( param );

    return true;

}
}
static const FilterPresetExporterPair exporterFnMap[] = {
    std::make_pair( libfoundation::app::EFilter::BlackAndWhiteAdaptiveMixerFilter, exporters::bwmixer ),
    std::make_pair( libfoundation::app::EFilter::CurvesFilter, exporters::curves ),
    std::make_pair( libfoundation::app::EFilter::SplitToneFilter, exporters::splittone ),
    std::make_pair( libfoundation::app::EFilter::CascadedSharpenFilter, exporters::sharpen ),
    std::make_pair( libfoundation::app::EFilter::FilmGrainFilter, exporters::filmgrain ),
    std::make_pair( libfoundation::app::EFilter::VignetteFilter, exporters::vignette )
};
static const size_t exporterFnMapLen = sizeof( exporterFnMap ) / sizeof( FilterPresetExporterPair );

bool toPresetValue(
    const libgraphics::FilterPreset& preset,
    const libfoundation::app::EFilter::t type,
    const std::string& presetName,
    PresetValue& valueOut
) {

    FnFilterPresetExporter  exporterFn;
    bool                    foundExporterFn( false );

    for( size_t i = 0; exporterFnMapLen > i; ++i ) {
        if( exporterFnMap[i].first == type ) {
            exporterFn = exporterFnMap[i].second;
            foundExporterFn = true;
            break;
        }
    }

    assert( foundExporterFn );

    if( !foundExporterFn ) {
        qDebug() << "Failed to find exporter function for filter preset type='" << libfoundation::app::EFilter::toName( type ).c_str() << "'.";
        return false;
    }

    return exporterFn(
               preset,
               presetName,
               valueOut
           );
}

}
