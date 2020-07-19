#pragma once

/// common stuff
#include <libcommon/sharedptr.hpp>
#include <libcommon/scopedptr.hpp>

#include <vector>
#include <string>
#include <map>

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>

#include <libgraphics/filter.hpp>
#include <libgraphics/image.hpp>

#include <libfoundation/app/application.hpp>

namespace blacksilk {

/**
    \enum EPresetValueType
    \since 1.0
    \brief Represents the type of a PresetValue
        instance. Each type is associated with a specific
        c++ type.
*/
enum class EPresetValueType : unsigned long {
    /// Top-level container type
    Root,
    Parameter,

    /// General preset values
    Identifier, /// (string) preset-name or value-identifier(color), bwmixer=red,green,factor(0.5)
    Factor, /// (float) a floating-point number indicating a specific factor, bwmixer=blue,red,factor(1.2)
    Color, /// (RGB8) a rgb-color value, bwmixer=color(120,120,120),red,factor(1.0)
    Decimal, /// (int) non-fp numbers
    FactorList, /// (float) a list of floating-point numbers
    TupleList, /// (float) a list of floating-point tuples
    Tuple, /// (float) a key-value pair

    /// Specific preset values
    Cascade, /// (float) specifies a cascaded-sharpen filter cascade
    Threshold, /// (float) specifies the cascaded-sharpen filter's threshold value
    Radius, /// (float) specifies a vignette filter's radius
    Center, /// (float) specifies a vignette filter's center point
    Highlights, /// (RGB8) a rgb-color value, bwmixer=highlights(red)
    Shadows, /// (RGB8) a rgb-color value
    Name, /// for preset names
    Category,
    X,
    Y,
    Strength,

    /// Error Types
    Invalid
};
bool getPresetValueTypeKeyword( const EPresetValueType& type, std::string& identifier );
bool checkPresetValueTypeKeyword( const EPresetValueType& type, const std::string& identifier );

/**
    \class  PresetValue
    \since  1.0
    \brief Represents a simple string-based representation of a
        preset value.
*/
struct PresetValue {
        PresetValue() : m_Type( EPresetValueType::Invalid ) {}
        PresetValue(
            EPresetValueType _type,
            const std::string& _keyword = "",
            const std::string& _value = ""
        ) : m_Type( _type ), m_Keyword( _keyword ), m_Value( _value ) {}

        const EPresetValueType&  type() const { return m_Type; }
        const std::string&  keyword() const { return m_Keyword; }
        const std::string&  value() const { return m_Value; }
        std::string&  keyword() { return m_Keyword; }
        std::string&  value() { return m_Value; }

        inline bool operator == ( const PresetValue& rhs ) const {
            return ( m_Type == rhs.type() ) && ( m_Keyword == rhs.keyword() ) && ( m_Value == rhs.value() );
        }
        inline bool operator != ( const PresetValue& rhs ) const {
            return ( m_Type != rhs.type() ) || ( m_Keyword != rhs.keyword() ) || ( m_Value != rhs.value() );
        }
        inline bool isRoot() const { return ( m_Value.empty() && m_Keyword.empty() ) && ( m_Type == EPresetValueType::Root ); }

        std::vector<PresetValue>& children() { return m_Children; }
        const std::vector<PresetValue>& children() const { return m_Children; }

        std::string print() const;


        /// converter functions
        static bool toRGB8(
            const PresetValue&  value,
            libgraphics::formats::RGB8::t&  color
        );
        static bool toRGB16(
            const PresetValue&  value,
            libgraphics::formats::RGB16::t&  color
        );
        static bool toRGB16S(
            const PresetValue&  value,
            libgraphics::formats::RGB16S::t&  color
        );
        static bool toRGB32(
            const PresetValue&  value,
            libgraphics::formats::RGB32::t&  color
        );
        static bool toRGB32S(
            const PresetValue&  value,
            libgraphics::formats::RGB32S::t&  color
        );


        static bool toFloat(
            const PresetValue& value,
            float& fvalue
        );
        static bool toString(
            const PresetValue& value,
            std::string& identifierName
        );

        // easy helper functions
        static std::string presetToString(
            const libgraphics::FilterPreset& preset,
            const std::string name
        );
        static libgraphics::FilterPreset stringToPreset( const std::string str );

        static bool toDecimal(
            const PresetValue& value,
            int& decimal
        );
        static bool toFactorList(
            const PresetValue& value,
            std::vector<float>& factorList
        );
        static bool toTupleList(
            const PresetValue& value,
            std::vector< std::pair<float, float> >& factorList
        );

        /// parser functions
        static bool parse(
            PresetValue& root,
            std::string source
        );
    private:
        const EPresetValueType      m_Type;
        std::string                 m_Keyword;
        std::string                 m_Value;
        std::vector<PresetValue>    m_Children;
};

/**
    \fn toPresetValue
    \since 1.0
    \brief Creates a PresetValue from an existing preset
        filter object.
*/
bool toPresetValue(
    const libgraphics::FilterPreset& preset,
    const libfoundation::app::EFilter::t type,
    const std::string& presetName,
    PresetValue& valueOut
);

/**
    \fn parseFilterPresetCollection
    \since 1.0
    \brief Creates a filter-preset collection from the specified
        PresetValue structure.
*/
bool createFilterPresetCollection(
    libgraphics::FilterPresetCollection* defaultPresets,
    libgraphics::FilterPresetCollection& collection,
    const PresetValue& value
);
bool createFilterPresetCollection(
    libgraphics::FilterPresetCollection* defaultPresets,
    libgraphics::FilterPresetCollection& collection,
    const std::string& source
);


/**
    \fn parseFilterPreset
    \since 1.0
    \brief Parses the specified string and creates a
        filter preset.
*/
bool   createFilterPreset(
    libgraphics::FilterPresetCollection* defaultPresets,
    libgraphics::FilterPreset& preset,
    const PresetValue& value
);



}
