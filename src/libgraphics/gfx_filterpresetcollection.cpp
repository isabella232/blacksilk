#include <libgraphics/filterpresetcollection.hpp>
#include <libcommon/fileutils.hpp>
#include <utils/preset.hpp>

#include <QDebug>

namespace libgraphics {

FilterPresetCollection::FilterPresetCollection() {}

std::string FilterPresetCollection::toString( const std::string& presetName ) const {

    blacksilk::PresetValue value( blacksilk::EPresetValueType::Root );

    for( auto it = this->constBegin(); it != this->constEnd(); ++it ) {
        libfoundation::app::EFilter::t type = libfoundation::app::EFilter::fromName(
                ( *it ).preset.filterName()
                                              );

        if( !blacksilk::toPresetValue( ( *it ).preset, type, presetName, value ) ) {
            assert( false );
            qDebug() << "Failed to convert libgraphics preset object to bs-format preset( presetName='" << ( *it ).preset.name().c_str() << "', filterName='" << ( *it ).preset.filterName().c_str() << "' ).";
            return std::string();
        }
    }

    const std::string presetData = value.print();
    return presetData;
}

bool FilterPresetCollection::reloadAll() {
    for( auto it = this->m_Presets.begin(); it != this->m_Presets.end(); ++it ) {
        if( ( *it ).path.empty() ) {
            continue;
        }

        if( !libcommon::fileutils::fileExists( it->path ) ) {
            return false;
        }

        if( !( *it ).preset.readFromFile( ( *it ).path ) ) {
            return false;
        }
    }

    return true;
}

bool FilterPresetCollection::reloadByPath( const std::string& path ) {
    if( path.empty() ) {
        return false;
    }

    if( !libcommon::fileutils::fileExists( path ) ) {
        return false;
    }

    for( auto it = this->m_Presets.begin(); it != this->m_Presets.end(); ++it ) {
        if( ( *it ).path == path ) {
            if( !( *it ).preset.readFromFile( path ) ) {
                return false;
            }

            return true;
        }
    }

    return false;
}

bool FilterPresetCollection::reloadByName( const std::string& name ) {
    for( auto it = this->m_Presets.begin(); it != this->m_Presets.end(); ++it ) {
        if( ( *it ).path.empty() ) {
            continue;
        }

        if( ( *it ).preset.name() == name ) {
            if( !( *it ).preset.readFromFile( ( *it ).path ) ) {
                return false;
            }

            return true;
        }
    }

    return false;
}

bool FilterPresetCollection::loadPresetFromFile( const std::string& path ) {
    if( !containsByPath( path ) ) {
        FilterPresetEntry entry;

        if( !libcommon::fileutils::fileExists( path ) ) {
            return false;
        }

        if( !entry.preset.readFromFile( path ) ) {
            return false;
        }

        entry.path = path;

        this->m_Presets.push_back( entry );

        return true;
    }

    return true;
}

bool FilterPresetCollection::containsByName( const std::string& name ) const {
    for( auto it = this->m_Presets.begin(); it != this->m_Presets.end(); ++it ) {
        if( ( *it ).preset.name() == name ) {
            return true;
        }
    }

    return false;
}

bool FilterPresetCollection::containsByPath( const std::string& path ) const {
    for( auto it = this->m_Presets.begin(); it != this->m_Presets.end(); ++it ) {
        if( ( *it ).path == path ) {
            return true;
        }
    }

    return false;
}

size_t FilterPresetCollection::count() const {
    return this->m_Presets.size();
}

void FilterPresetCollection::add( const FilterPreset& preset ) {
    FilterPresetEntry entry;

    entry.path = "";
    entry.preset = preset;

    this->m_Presets.push_back( entry );
}

FilterPreset& FilterPresetCollection::emplaceBack() {
    this->m_Presets.emplace_back();

    return this->m_Presets.back().preset;
}

bool FilterPresetCollection::remove( const FilterPreset& preset ) {
    for( auto it = this->m_Presets.begin(); it != this->m_Presets.end(); ++it ) {
        if( ( *it ).preset == preset ) {
            this->m_Presets.erase( it );

            return true;
        }
    }

    return false;
}

bool FilterPresetCollection::removeByPath( const std::string& path ) {
    for( auto it = this->m_Presets.begin(); it != this->m_Presets.end(); ++it ) {
        if( ( *it ).path == path ) {
            this->m_Presets.erase( it );

            return true;
        }
    }

    return false;
}

bool FilterPresetCollection::removeByName( const std::string& name ) {
    for( auto it = this->m_Presets.begin(); it != this->m_Presets.end(); ++it ) {
        if( ( *it ).preset.name() == name ) {
            this->m_Presets.erase( it );

            return true;
        }
    }

    return false;
}

void FilterPresetCollection::clear() {
    this->m_Presets.clear();
}

FilterPresetCollection::ContainerType::iterator FilterPresetCollection::begin() {
    return this->m_Presets.begin();
}

FilterPresetCollection::ContainerType::const_iterator FilterPresetCollection::begin() const {
    return this->m_Presets.begin();
}

FilterPresetCollection::ContainerType::const_iterator FilterPresetCollection::constBegin() const {
    return this->m_Presets.cbegin();
}

FilterPresetCollection::ContainerType::iterator FilterPresetCollection::end() {
    return this->m_Presets.end();
}

FilterPresetCollection::ContainerType::const_iterator FilterPresetCollection::end() const {
    return this->m_Presets.end();
}

FilterPresetCollection::ContainerType::const_iterator FilterPresetCollection::constEnd() const {
    return this->m_Presets.cend();
}

FilterPreset& FilterPresetCollection::front() {
    return this->m_Presets.front().preset;
}

const FilterPreset& FilterPresetCollection::front() const {
    return this->m_Presets.front().preset;
}

FilterPreset& FilterPresetCollection::back() {
    return this->m_Presets.back().preset;
}

const FilterPreset& FilterPresetCollection::back() const {
    return this->m_Presets.back().preset;
}

FilterPresetCollection FilterPresetCollection::collectionForFilter( const std::string& name ) const {
    FilterPresetCollection collection;

    for( auto it = this->m_Presets.begin(); it != this->m_Presets.end(); ++it ) {
        if( ( *it ).preset.filterName() != name ) {
            continue;
        }

        collection.add(
            ( *it ).preset
        );
    }

    return collection;
}

bool FilterPresetCollection::byName( FilterPreset& preset, const std::string& name ) {
    for( auto it = this->m_Presets.begin(); it != this->m_Presets.end(); ++it ) {
        if( ( *it ).preset.name() == name ) {
            preset = ( *it ).preset;
            return true;
        }
    }

    return false;
}
}
