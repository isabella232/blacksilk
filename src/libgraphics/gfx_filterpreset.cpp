#include <libgraphics/filterpreset.hpp>
#include <libgraphics/filterpreset_p.hpp>
#include <libserialization++.hpp>
#include <log/log.hpp>
#include <QDebug>

namespace libgraphics {

/// impl: FilterPresetSerializationProvider
void addPropertiesFromFilterPresetMap( const std::unordered_map<std::string, FilterPresetValueString>& map, spp::Vector<GenericFilterPresetEntry>& entries ) {
    for( auto it = map.begin(); it != map.end(); ++it ) {
        entries.PushBack(
            GenericFilterPresetEntry(
                ( *it ).first,
                ( *it ).second.value
            )
        );
    }
}
template < class _t_any >
void addPropertiesFromFilterPresetMap( const std::unordered_map<std::string, _t_any>& map, spp::Vector<GenericFilterPresetEntry>& entries ) {
    for( auto it = map.begin(); it != map.end(); ++it ) {
        entries.PushBack(
            GenericFilterPresetEntry(
                ( *it ).first,
                libgraphics::PresetValueConverter<typename _t_any::ValueType>::toString( ( *it ).second.value )
            )
        );
    }
}

void FilterPresetSerializationProvider::fillFilterPreset( FilterPreset* preset ) {
    assert( !empty() );
    assert( preset );

    if( this->m_Values.Empty() ) {
        return;
    }

    preset->filterName() = this->m_FilterName;
    preset->name() = this->m_Name;

    for( auto it = this->m_Values.Begin(); it != this->m_Values.End(); ++it ) {
        const auto numberOfElements = helpers::count(
                                          ( *it ).value,
                                          ','
                                      ) + 1;

        if( numberOfElements == 1 ) {
            const auto tokenType = helpers::tokenTypeForString(
                                       ( *it ).value
                                   );

            if( tokenType == helpers::ETokenType::None ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
                qDebug() << "FilterPresetSerializationProvider::fillFilterPreset(): Failed to fill filter preset unknown token value. Aborting serialization...";
#endif
                break;
            }

            switch( tokenType ) {
                case helpers::ETokenType::Boolean: {
                    preset->switches()[( *it ).name].value = false;

                    const auto success = PresetValueConverter<bool>::fromString( preset->switches()[( *it ).name].value, ( *it ).value );
                    LOGB_ASSERT( success, "couldn't convert to bool" );
                    break;
                }

                case helpers::ETokenType::Float: {
                    preset->floats()[( *it ).name].value = 0.0f;

                    const auto success = PresetValueConverter<float>::fromString( preset->floats()[( *it ).name].value, ( *it ).value );
                    LOGB_ASSERT( success, "couldn't convert to float" );
                    break;
                }

                case helpers::ETokenType::String: {
                    preset->strings()[( *it ).name].value = ( *it ).value;
                    break;
                }

                case helpers::ETokenType::Integer: {
                    preset->ints()[( *it ).name].value = 0;

                    const auto success = PresetValueConverter<int>::fromString( preset->ints()[( *it ).name].value, ( *it ).value );
                    LOGB_ASSERT( success, "couldn't convert to integer" );
                    break;
                }

                default: break;
            }
        } else if( numberOfElements == 2 ) {
            if( ( *it ).value.find( '.' ) != std::string::npos ) {
                preset->points()[( *it ).name].value = libgraphics::Point32F( 0.0f, 0.0f );

                const auto success = PresetValueConverter<libgraphics::Point32F>::fromString( preset->points()[( *it ).name].value, ( *it ).value );
                LOGB_ASSERT( success, "couldn't convert to Point32F" );
            } else {
                preset->positions()[( *it ).name].value = libgraphics::Point32I( 0, 0 );

                const auto success = PresetValueConverter<libgraphics::Point32I>::fromString( preset->positions()[( *it ).name].value, ( *it ).value );
                LOGB_ASSERT( success, "couldn't convert value to Point32I" );
            }
        } else if( numberOfElements == 4 ) {
            preset->rects()[( *it ).name].value = libgraphics::Rect32I();

            const auto success = PresetValueConverter<libgraphics::Rect32I>::fromString( preset->rects()[( *it ).name].value, ( *it ).value );
            LOGB_ASSERT( success, "couldn't convert value to Rect32I" );
        } else {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
            qDebug() << "FilterPresetSerializationProvider::fillFilterPreset(): Failed to fill filter preset from invalid preset value. Aborting serialization...";
#endif
            break;
        }
    }
}

void FilterPresetSerializationProvider::assign( FilterPreset* preset ) {
    assert( preset );

    clear();
    this->m_Preset = preset;
    this->m_FilterName = preset->filterName();

    /// loop through types
    addPropertiesFromFilterPresetMap(
        m_Preset->argb8s(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->argb16s(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->rgb8s(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->rgb16s(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->mono8s(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->mono16s(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->chars(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->ints(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->floats(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->switches(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->strings(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->lines(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->positions(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->rects(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->uints(),
        this->m_Values
    );
    addPropertiesFromFilterPresetMap(
        m_Preset->points(),
        this->m_Values
    );
}

bool FilterPresetSerializationProvider::empty() const {
    return this->m_Values.Empty() && ( this->m_Preset == nullptr );
}

void FilterPresetSerializationProvider::clear() {
    this->m_Values.Clear();
    this->m_Preset = nullptr;
}

spp::PropertyCollection FilterPresetSerializationProvider::GetProperties() {
    assert( !empty() );

    spp::PropertyCollection collection( m_Preset->name(), this );

    bool ret = collection.Add(
                   spp::MakeValueProperty(
                       this,
                       "FilterName",
                       &this->m_FilterName
                   )
               );
    LOGB_ASSERT( ret, "couldn't add FilterName" );

    ret = collection.Add(
              spp::MakeValueProperty(
                  this,
                  "Name",
                  &this->m_Name
              )
          );
    LOGB_ASSERT( ret, "couldn't add Name" );

    ret = collection.Add(
              spp::MakeCollectionProperty(
                  this,
                  "parameters",
                  &this->m_Values
              )
          );
    LOGB_ASSERT( ret, "couldn't add parameters" );

    return collection;
}


/// impl: GenericFilterPresetEntrySerializationProvider
void GenericFilterPresetEntrySerializationProvider::assign( GenericFilterPresetEntry* entry ) {
    this->m_Entry = entry;
}

void GenericFilterPresetEntrySerializationProvider::clear() {
    this->m_Entry = nullptr;
}

bool GenericFilterPresetEntrySerializationProvider::empty() const {
    return ( this->m_Entry == nullptr );
}

spp::PropertyCollection GenericFilterPresetEntrySerializationProvider::GetProperties() {
    spp::PropertyCollection collection( this->m_Entry->name, this->m_Entry );

    const auto ret = collection.Add(
                         spp::MakeValueProperty(
                             this->m_Entry,
                             "value",
                             &this->m_Entry->value
                         )
                     );
    LOGB_ASSERT( ret, "couldn't add value" );

    return collection;
}

/// impl: FilterPreset
FilterPreset::FilterPreset() {}

FilterPreset::FilterPreset( const std::string& name ) : m_Name( name ) {}

bool FilterPreset::writeToFile( const std::string& path ) {
    FilterPresetSerializationProvider provider( this );

    const auto ret = SppSerializeJsonToFile(
                         &provider,
                         path
                     );
    assert( ret );

    if( !ret ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "FilterPreset::writeToFile(): Failed to serialize JSON to file - " << path.c_str();
#endif
        return false;
    }

    return true;
}

bool FilterPreset::readFromFile( const std::string& path ) {
    FilterPresetSerializationProvider provider( this );

    const auto ret = SppDeserializeJsonFromFile(
                         &provider,
                         path
                     );
    assert( ret );

    if( !ret ) {
#ifdef LIBGRAPHICS_DEBUG_OUTPUT
        qDebug() << "FilterPreset::readFromFile(): Failed to serialize JSON from file - " << path.c_str();
#endif
        return false;
    }

    provider.fillFilterPreset(
        this
    );

    return true;
}

bool FilterPreset::containsValueWithName( const std::string& name ) const {
    if( m_ValARGB8.find( name ) != m_ValARGB8.end() ) {
        return true;
    }

    if( m_ValARGB16.find( name ) != m_ValARGB16.end() ) {
        return true;
    }

    if( m_ValRGB8.find( name ) != m_ValRGB8.end() ) {
        return true;
    }

    if( m_ValRGB16.find( name ) != m_ValRGB16.end() ) {
        return true;
    }

    if( m_ValMono8.find( name ) != m_ValMono8.end() ) {
        return true;
    }

    if( m_ValMono16.find( name ) != m_ValMono16.end() ) {
        return true;
    }

    if( m_ValChar.find( name ) != m_ValChar.end() ) {
        return true;
    }

    if( m_ValInt.find( name ) != m_ValInt.end() ) {
        return true;
    }

    if( m_ValUInt.find( name ) != m_ValUInt.end() ) {
        return true;
    }

    if( m_ValFloat.find( name ) != m_ValFloat.end() ) {
        return true;
    }

    if( m_ValSwitch.find( name ) != m_ValSwitch.end() ) {
        return true;
    }

    if( m_ValString.find( name ) != m_ValString.end() ) {
        return true;
    }

    if( m_ValRect.find( name ) != m_ValRect.end() ) {
        return true;
    }

    if( m_ValPosition.find( name ) != m_ValPosition.end() ) {
        return true;
    }

    if( m_ValLine.find( name ) != m_ValLine.end() ) {
        return true;
    }

    if( m_ValPoints.find( name ) != m_ValPoints.end() ) {
        return true;
    }

    return false;
}

void FilterPreset::clear() {
    positions().clear();
    lines().clear();
    rects().clear();
    floats().clear();
    ints().clear();
    chars().clear();
    uints().clear();
    switches().clear();
    strings().clear();
    rgb8s().clear();
    rgb16s().clear();
    argb8s().clear();
    argb16s().clear();
    mono8s().clear();
    mono16s().clear();
}

std::string& FilterPreset::filterName() {
    return this->m_FilterName;
}

const std::string& FilterPreset::filterName() const {
    return this->m_FilterName;
}

std::string& FilterPreset::name() {
    return this->m_Name;
}

const std::string& FilterPreset::name() const {
    return this->m_Name;
}

std::string& FilterPreset::category() {
    return this->m_Category;
}

const std::string& FilterPreset::category() const {
    return this->m_Category;
}

std::unordered_map<std::string, FilterPresetValuePosition>& FilterPreset::positions() {
    return this->m_ValPosition;
}

const std::unordered_map<std::string, FilterPresetValuePosition>& FilterPreset::positions() const {
    return this->m_ValPosition;
}

std::unordered_map<std::string, FilterPresetValuePoint>& FilterPreset::points() {
    return this->m_ValPoints;
}

const std::unordered_map<std::string, FilterPresetValuePoint>& FilterPreset::points() const {
    return this->m_ValPoints;
}

std::unordered_map<std::string, FilterPresetValueLine>& FilterPreset::lines() {
    return this->m_ValLine;
}

const std::unordered_map<std::string, FilterPresetValueLine>& FilterPreset::lines() const {
    return this->m_ValLine;
}

std::unordered_map<std::string, FilterPresetValueRect>& FilterPreset::rects() {
    return this->m_ValRect;
}

const std::unordered_map<std::string, FilterPresetValueRect>& FilterPreset::rects() const {
    return this->m_ValRect;
}

std::unordered_map<std::string, FilterPresetValueFloat>& FilterPreset::floats() {
    return this->m_ValFloat;
}

const std::unordered_map<std::string, FilterPresetValueFloat>& FilterPreset::floats() const {
    return this->m_ValFloat;
}

std::unordered_map<std::string, FilterPresetValueInteger>& FilterPreset::ints() {
    return this->m_ValInt;
}

const std::unordered_map<std::string, FilterPresetValueInteger>& FilterPreset::ints() const {
    return this->m_ValInt;
}

std::unordered_map<std::string, FilterPresetValueChar>& FilterPreset::chars() {
    return this->m_ValChar;
}

const std::unordered_map<std::string, FilterPresetValueChar>& FilterPreset::chars() const {
    return this->m_ValChar;
}

std::unordered_map<std::string, FilterPresetValueUnsignedInteger>& FilterPreset::uints() {
    return this->m_ValUInt;
}

const std::unordered_map<std::string, FilterPresetValueUnsignedInteger>& FilterPreset::uints() const {
    return this->m_ValUInt;
}

std::unordered_map<std::string, FilterPresetValueSwitch>& FilterPreset::switches() {
    return this->m_ValSwitch;
}

const std::unordered_map<std::string, FilterPresetValueSwitch>& FilterPreset::switches() const {
    return this->m_ValSwitch;
}

std::unordered_map<std::string, FilterPresetValueString>& FilterPreset::strings() {
    return this->m_ValString;
}

const std::unordered_map<std::string, FilterPresetValueString>& FilterPreset::strings() const {
    return this->m_ValString;
}

std::unordered_map<std::string, FilterPresetValueRGB8>& FilterPreset::rgb8s() {
    return this->m_ValRGB8;
}

const std::unordered_map<std::string, FilterPresetValueRGB8>& FilterPreset::rgb8s() const {
    return this->m_ValRGB8;
}

std::unordered_map<std::string, FilterPresetValueRGB16>& FilterPreset::rgb16s() {
    return this->m_ValRGB16;
}

const std::unordered_map<std::string, FilterPresetValueRGB16>& FilterPreset::rgb16s() const {
    return this->m_ValRGB16;
}

std::unordered_map<std::string, FilterPresetValueARGB8>& FilterPreset::argb8s() {
    return this->m_ValARGB8;
}

const std::unordered_map<std::string, FilterPresetValueARGB8>& FilterPreset::argb8s() const {
    return this->m_ValARGB8;
}

std::unordered_map<std::string, FilterPresetValueARGB16>& FilterPreset::argb16s() {
    return this->m_ValARGB16;
}

const std::unordered_map<std::string, FilterPresetValueARGB16>& FilterPreset::argb16s() const {
    return this->m_ValARGB16;
}

std::unordered_map<std::string, FilterPresetValueMono8>& FilterPreset::mono8s() {
    return this->m_ValMono8;
}

const std::unordered_map<std::string, FilterPresetValueMono8>& FilterPreset::mono8s() const {
    return this->m_ValMono8;
}

std::unordered_map<std::string, FilterPresetValueMono16>& FilterPreset::mono16s() {
    return this->m_ValMono16;
}

const std::unordered_map<std::string, FilterPresetValueMono16>& FilterPreset::mono16s() const {
    return this->m_ValMono16;
}

}
