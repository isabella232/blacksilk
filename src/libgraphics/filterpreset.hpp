#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/io/pipeline.hpp>
#include <libcommon/noncopyable.hpp>

#include <unordered_map>
#include <map>
#include <string>

namespace libgraphics {

template < class _t_value >
struct FilterPresetValue {
    typedef _t_value        ValueType;

    FilterPresetValue() : value( ValueType() ) {}
    explicit FilterPresetValue(
        const ValueType& _val
    ) : value( _val ) {}

    const ValueType& get() const {
        return value;
    }
    ValueType& get() {
        return value;
    }

    inline bool operator == ( const FilterPresetValue& rhs ) const {
        return ( get() == rhs.get() );
    }
    inline bool operator != ( const FilterPresetValue& rhs ) const {
        return ( get() != rhs.get() );
    }


    ValueType   value;
};
template <>
struct FilterPresetValue<float> {
    typedef float        ValueType;

    FilterPresetValue() : value( 0.0f ) {}
    explicit FilterPresetValue(
        const ValueType& _val
    ) : value( _val ) {}

    const ValueType& get() const {
        return value;
    }
    ValueType& get() {
        return value;
    }

    inline bool operator == ( const FilterPresetValue& rhs ) const {
        return ( get() == rhs.get() );
    }
    inline bool operator != ( const FilterPresetValue& rhs ) const {
        return ( get() != rhs.get() );
    }


    ValueType   value;
};

typedef FilterPresetValue <
libgraphics::Point32I
>   FilterPresetValuePosition;
typedef FilterPresetValue <
libgraphics::Line32I
>   FilterPresetValueLine;
typedef FilterPresetValue <
libgraphics::Rect32I
>   FilterPresetValueRect;
typedef FilterPresetValue <
float
>   FilterPresetValueFloat;
typedef FilterPresetValue <
int
>   FilterPresetValueInteger;
typedef FilterPresetValue <
char
>   FilterPresetValueChar;
typedef FilterPresetValue <
unsigned int
>   FilterPresetValueUnsignedInteger;
typedef FilterPresetValue <
bool
>   FilterPresetValueSwitch;
typedef FilterPresetValue <
std::string
>   FilterPresetValueString;
typedef FilterPresetValue <
libgraphics::formats::RGB8::t
>   FilterPresetValueRGB8;
typedef FilterPresetValue <
libgraphics::formats::RGB16::t
>   FilterPresetValueRGB16;
typedef FilterPresetValue <
libgraphics::formats::ARGB8::t
>   FilterPresetValueARGB8;
typedef FilterPresetValue <
libgraphics::formats::ARGB16::t
>   FilterPresetValueARGB16;
typedef FilterPresetValue <
libgraphics::formats::Mono8::t
>   FilterPresetValueMono8;
typedef FilterPresetValue <
libgraphics::formats::Mono16::t
>   FilterPresetValueMono16;
typedef FilterPresetValue <
libgraphics::Point32F
>   FilterPresetValuePoint;

class FilterPreset {
    public:
        FilterPreset();
        explicit FilterPreset( const std::string& name );

        bool writeToFile( const std::string& path );
        bool readFromFile( const std::string& path );

        bool containsValueWithName( const std::string& name ) const;

        void clear();

        std::string& category();
        const std::string& category() const;

        std::string& name();
        const std::string& name() const;

        std::string& filterName();
        const std::string& filterName() const;

        std::unordered_map<std::string, FilterPresetValuePosition>& positions();
        const std::unordered_map<std::string, FilterPresetValuePosition>& positions() const;

        std::unordered_map<std::string, FilterPresetValuePoint>& points();
        const std::unordered_map<std::string, FilterPresetValuePoint>& points() const;

        std::unordered_map<std::string, FilterPresetValueLine>& lines();
        const std::unordered_map<std::string, FilterPresetValueLine>& lines() const;

        std::unordered_map<std::string, FilterPresetValueRect>& rects();
        const std::unordered_map<std::string, FilterPresetValueRect>& rects() const;

        std::unordered_map<std::string, FilterPresetValueFloat>& floats();
        const std::unordered_map<std::string, FilterPresetValueFloat>& floats() const;

        std::unordered_map<std::string, FilterPresetValueInteger>& ints();
        const std::unordered_map<std::string, FilterPresetValueInteger>& ints() const;

        std::unordered_map<std::string, FilterPresetValueChar>& chars();
        const std::unordered_map<std::string, FilterPresetValueChar>& chars() const;

        std::unordered_map<std::string, FilterPresetValueUnsignedInteger>& uints();
        const std::unordered_map<std::string, FilterPresetValueUnsignedInteger>& uints() const;

        std::unordered_map<std::string, FilterPresetValueSwitch>& switches();
        const std::unordered_map<std::string, FilterPresetValueSwitch>& switches() const;

        std::unordered_map<std::string, FilterPresetValueString>& strings();
        const std::unordered_map<std::string, FilterPresetValueString>& strings() const;

        std::unordered_map<std::string, FilterPresetValueRGB8>& rgb8s();
        const std::unordered_map<std::string, FilterPresetValueRGB8>& rgb8s() const;

        std::unordered_map<std::string, FilterPresetValueRGB16>& rgb16s();
        const std::unordered_map<std::string, FilterPresetValueRGB16>& rgb16s() const;

        std::unordered_map<std::string, FilterPresetValueARGB8>& argb8s();
        const std::unordered_map<std::string, FilterPresetValueARGB8>& argb8s() const;

        std::unordered_map<std::string, FilterPresetValueARGB16>& argb16s();
        const std::unordered_map<std::string, FilterPresetValueARGB16>& argb16s() const;

        std::unordered_map<std::string, FilterPresetValueMono8>& mono8s();
        const std::unordered_map<std::string, FilterPresetValueMono8>& mono8s() const;

        std::unordered_map<std::string, FilterPresetValueMono16>& mono16s();
        const std::unordered_map<std::string, FilterPresetValueMono16>& mono16s() const;

        inline bool operator == ( const FilterPreset& rhs ) const {
            return ( m_Name == rhs.m_Name ) &&
                   ( m_FilterName == rhs.m_FilterName ) &&
                   ( m_Category == rhs.m_Category ) &&
                   ( m_ValFloat == rhs.m_ValFloat ) &&
                   ( m_ValPosition == rhs.m_ValPosition ) &&
                   ( m_ValLine == rhs.m_ValLine ) &&
                   ( m_ValRect == rhs.m_ValRect ) &&
                   ( m_ValInt == rhs.m_ValInt ) &&
                   ( m_ValChar == rhs.m_ValChar ) &&
                   ( m_ValUInt == rhs.m_ValUInt ) &&
                   ( m_ValSwitch == rhs.m_ValSwitch ) &&
                   ( m_ValString == rhs.m_ValString ) &&
                   ( m_ValRGB8 == rhs.m_ValRGB8 ) &&
                   ( m_ValRGB16 == rhs.m_ValRGB16 ) &&
                   ( m_ValARGB8 == rhs.m_ValARGB8 ) &&
                   ( m_ValMono8 == rhs.m_ValMono8 ) &&
                   ( m_ValPoints == rhs.m_ValPoints ) &&
                   ( m_ValARGB16 == rhs.m_ValARGB16 ) &&
                   ( m_ValMono16 == rhs.m_ValMono16 );
        }
        inline bool operator != ( const FilterPreset& rhs ) const {
            return ( m_Name != rhs.m_Name ) ||
                   ( m_FilterName != rhs.m_FilterName ) ||
                   ( m_Category != rhs.m_Category ) ||
                   ( m_ValFloat != rhs.m_ValFloat ) ||
                   ( m_ValPosition != rhs.m_ValPosition ) ||
                   ( m_ValLine != rhs.m_ValLine ) ||
                   ( m_ValRect != rhs.m_ValRect ) ||
                   ( m_ValInt != rhs.m_ValInt ) ||
                   ( m_ValChar != rhs.m_ValChar ) ||
                   ( m_ValUInt != rhs.m_ValUInt ) ||
                   ( m_ValSwitch != rhs.m_ValSwitch ) ||
                   ( m_ValString != rhs.m_ValString ) ||
                   ( m_ValRGB8 != rhs.m_ValRGB8 ) ||
                   ( m_ValRGB16 != rhs.m_ValRGB16 ) ||
                   ( m_ValARGB8 != rhs.m_ValARGB8 ) ||
                   ( m_ValMono8 != rhs.m_ValMono8 ) ||
                   ( m_ValPoints != rhs.m_ValPoints ) ||
                   ( m_ValARGB16 != rhs.m_ValARGB16 ) ||
                   ( m_ValMono16 != rhs.m_ValMono16 );
        }
    protected:
        std::string m_Category;
        std::string m_Name;
        std::string m_FilterName;
        std::unordered_map<std::string, FilterPresetValuePosition> m_ValPosition;
        std::unordered_map<std::string, FilterPresetValueLine> m_ValLine;
        std::unordered_map<std::string, FilterPresetValueRect> m_ValRect;
        std::unordered_map<std::string, FilterPresetValueFloat> m_ValFloat;
        std::unordered_map<std::string, FilterPresetValueInteger> m_ValInt;
        std::unordered_map<std::string, FilterPresetValueChar> m_ValChar;
        std::unordered_map<std::string, FilterPresetValueUnsignedInteger> m_ValUInt;
        std::unordered_map<std::string, FilterPresetValueSwitch> m_ValSwitch;
        std::unordered_map<std::string, FilterPresetValueString> m_ValString;
        std::unordered_map<std::string, FilterPresetValueRGB8> m_ValRGB8;
        std::unordered_map<std::string, FilterPresetValueRGB16> m_ValRGB16;
        std::unordered_map<std::string, FilterPresetValueARGB8> m_ValARGB8;
        std::unordered_map<std::string, FilterPresetValueARGB16> m_ValARGB16;
        std::unordered_map<std::string, FilterPresetValueMono8> m_ValMono8;
        std::unordered_map<std::string, FilterPresetValueMono16> m_ValMono16;
        std::unordered_map<std::string, FilterPresetValuePoint> m_ValPoints;
};

}
