#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/filterpreset.hpp>
#include <libgraphics/io/pipeline.hpp>
#include <libcommon/noncopyable.hpp>

#include <map>
#include <string>

namespace libgraphics {

class FilterPresetCollection {
    public:
        struct FilterPresetEntry {
            FilterPreset preset;
            std::string path;

            FilterPresetEntry() {}
            explicit FilterPresetEntry( const std::string& _path ) : path( _path ) {
                preset.readFromFile( path );
            }
            inline bool operator == ( const FilterPresetEntry& rhs ) const {
                return rhs.preset == preset;
            }
        };
        typedef std::vector<FilterPresetEntry>  ContainerType;

        FilterPresetCollection();

        inline bool operator == ( const FilterPresetCollection& rhs ) const {
            return rhs.m_Presets == m_Presets;
        }

        std::string toString( const std::string& presetName = std::string() ) const;

        bool reloadAll();
        bool reloadByPath( const std::string& path );
        bool reloadByName( const std::string& name );

        bool loadPresetFromFile( const std::string& path );

        bool containsByName( const std::string& name ) const;
        bool containsByPath( const std::string& path ) const;

        size_t count() const;

        void add( const FilterPreset& preset );
        FilterPreset& emplaceBack();

        bool remove( const FilterPreset& preset );
        bool removeByPath( const std::string& path );
        bool removeByName( const std::string& name );

        void clear();

        ContainerType::iterator begin();
        ContainerType::const_iterator begin() const;
        ContainerType::const_iterator constBegin() const;

        ContainerType::iterator end();
        ContainerType::const_iterator end() const;
        ContainerType::const_iterator constEnd() const;

        FilterPreset& front();
        const FilterPreset& front() const;

        FilterPreset& back();
        const FilterPreset& back() const;

        FilterPresetCollection collectionForFilter( const std::string& name ) const;

        bool byName( FilterPreset& preset, const std::string& name );
        template < class _Pred >
        bool get( FilterPreset& preset, const _Pred& pred ) {
            for( auto it = m_Presets.begin(); it != m_Presets.end(); ++it ) {
                if( pred( *it ) ) {
                    preset = ( *it ).preset;
                    return true;
                }
            }

            return false;
        }

    protected:
        ContainerType m_Presets;
};

}
