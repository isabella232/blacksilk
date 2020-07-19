#pragma once

#include <vector>
#include <limits>
#include <algorithm>
#include <cmath>
#include <climits>

namespace libgraphics {

/**
    \class  CurveData
    \since  0.5.0
    \brief
        The CurveData template class is a container class for curve data used in filters
        such as FilmGrain und Curves. A CurveData instance is able to handle a fixed curve length
        and provides several data-handling helper methods such as copy(to copy data) and reset(to reset the current state).
*/
template < class _t_any, size_t _v_length >
struct CurveData {
    typedef _t_any          ValueType;

    static const size_t length = _v_length;

    static ValueType max() { return std::numeric_limits<ValueType>::max(); }
    static ValueType min() { return std::numeric_limits<ValueType>::min(); }


    CurveData() {
        reset( _v_length, 0 );
    }

    /// getters/setters
    ValueType& at( const size_t pos ) {
        assert( _v_length > pos );
        return data[pos];
    }
    const ValueType& at( const size_t pos ) const {
        assert( _v_length > pos );
        return data[pos];
    }
    void set( const size_t pos, const ValueType& value ) {
        assert( _v_length > pos );
        data[pos] = value;
    }

    /// properties
    void*   buffer() const { return ( void* )&data; }
    size_t  byteSize() const { return sizeof( ValueType ) * _v_length; }

    /// copy
    template < class _t_conv, class _t_other >
    void copy( const _t_conv& conv, const std::vector<_t_other>& vec ) {
        const size_t the_smaller = std::min( vec.size(), _v_length );

        for( size_t i = 0; the_smaller > i; ++i ) {
            data[i] = conv( vec[i] );
        }
    }
    template < class _t_conv, class _t_other >
    void copy( const _t_conv& conv, const std::vector<_t_other>& vec, size_t _to_copy ) {
        copy<_t_conv, _t_other>( conv, vec, _to_copy );
    }
    template < class _t_conv, class _t_other >
    void copy( const _t_conv& conv, const std::vector<_t_other>& vec, size_t _to_copy, size_t _src_offset ) {
        copy<_t_conv, _t_other>( conv, vec, _to_copy, _src_offset );
    }
    template < class _t_conv, class _t_other >
    void copy( const _t_conv& conv, const std::vector<_t_other>& vec, size_t _to_copy, size_t _src_offset, size_t _dst_offset ) {
        assert( ( _v_length ) > ( _to_copy + _src_offset ) );
        assert( ( vec.size() ) > ( _to_copy + _dst_offset ) );

        for( size_t i = 0; _to_copy > i; ++i ) {
            data[i + _src_offset] = conv( vec[i + _dst_offset] );
        }
    }

    void copy( const std::vector<ValueType>& vec ) {
        const size_t the_smaller = std::min( vec.size(), _v_length );

        for( size_t i = 0; the_smaller > i; ++i ) {
            data[i] = vec[i];
        }
    }
    void copy( const std::vector<ValueType>& vec, size_t _to_copy ) {
        copy( vec, _to_copy, 0, 0 );
    }
    void copy( const std::vector<ValueType>& vec, size_t _to_copy, size_t _src_offset ) {
        copy( vec, _to_copy, _src_offset, 0 );
    }
    void copy( const std::vector<ValueType>& vec, size_t _to_copy, size_t _src_offset, size_t _dst_offset ) {
        assert( ( _v_length ) >= ( _to_copy + _src_offset ) );
        assert( ( vec.size() ) >= ( _to_copy + _dst_offset ) );

        for( size_t i = 0; _to_copy > i; ++i ) {
            data[i + _src_offset] = vec[i + _dst_offset];
        }
    }

    void copy( const CurveData<ValueType, _v_length>& other ) {
        for( size_t i = 0; _v_length > i; ++i ) {
            data[i] = other.data[i];
        }
    }
    template < size_t _v_any_length >
    void copy( const CurveData<ValueType, _v_any_length>& other ) {
        const size_t the_smaller = std::min( _v_any_length, _v_length );

        for( size_t i = 0; the_smaller > i; ++i ) {
            data[i] = other.data[i];
        }
    }
    template < size_t _v_any_length >
    void copy( const CurveData<ValueType, _v_any_length>& other, size_t _to_copy ) {
        return copy<_v_any_length>( other, _to_copy, 0, 0 );
    }
    template < size_t _v_any_length >
    void copy( const CurveData<ValueType, _v_any_length>& other, size_t _to_copy, size_t _src_offset ) {
        return copy<_v_any_length>( other, _to_copy, _src_offset, 0 );
    }
    template < size_t _v_any_length >
    void copy( const CurveData<ValueType, _v_any_length>& other, size_t _to_copy, size_t _src_offset, size_t _dst_offset ) {
        assert( ( _v_length ) >= ( _to_copy + _src_offset ) );
        assert( ( _v_any_length ) >= ( _to_copy + _dst_offset ) );

        for( size_t i = 0; _to_copy > i; ++i ) {
            data[i + _src_offset] = other.data[i + _dst_offset];
        }
    }


    template < class _t_conv, class _t_other >
    void copy( const _t_conv& conv, const CurveData<_t_other, _v_length>& other ) {
        for( size_t i = 0; _v_length > i; ++i ) {
            data[i] = other.data[i];
        }
    }
    template < class _t_conv, class _t_other, size_t _v_any_length >
    void copy( const _t_conv& conv, const CurveData<_t_other, _v_any_length>& other ) {
        const size_t the_smaller = std::min( _v_any_length, _v_length );

        for( size_t i = 0; the_smaller > i; ++i ) {
            data[i] = conv( other.data[i] );
        }
    }
    template < class _t_conv, class _t_other, size_t _v_any_length >
    void copy( const _t_conv& conv, const CurveData<_t_other, _v_any_length>& other, size_t _to_copy ) {
        return copy<_t_conv, _t_other, _v_any_length>( conv, other, _to_copy, 0, 0 );
    }
    template < class _t_conv, class _t_other, size_t _v_any_length >
    void copy( const _t_conv& conv, const CurveData<_t_other, _v_any_length>& other, size_t _to_copy, size_t _src_offset ) {
        return copy<_t_conv, _t_other, _v_any_length>( conv, other, _to_copy, _src_offset, 0 );
    }
    template < class _t_conv, class _t_other, size_t _v_any_length >
    void copy( const _t_conv& conv, const CurveData<_t_other, _v_any_length>& other, size_t _to_copy, size_t _src_offset, size_t _dst_offset ) {
        assert( ( _v_length ) >= ( _to_copy + _src_offset ) );
        assert( ( _v_any_length ) >= ( _to_copy + _dst_offset ) );

        for( size_t i = 0; _to_copy > i; ++i ) {
            data[i + _src_offset] = conv( other.data[i + _dst_offset] );
        }
    }

    /// reset
    void reset( size_t len, const ValueType& value ) {
        assert( _v_length >= len );

        for( size_t i = 0; len > i; ++i ) {
            data[i] = value;
        }
    }
    void reset() {
        reset( _v_length );
    }
    void reset( size_t len ) {
        reset( len, 0 );
    }
    void reset( const std::vector<ValueType>& vec ) {
        reset( vec, vec.size() );
    }
    void reset( const std::vector<ValueType>& vec, size_t len ) {
        assert( _v_length >= len );

        for( size_t i = 0; len > i; ++i ) {
            data[i] = vec[i];
        }
    }
    void reset( const float* vec, size_t len ) {
        assert( _v_length >= len );
        assert( vec != nullptr );

        for( size_t i = 0; len > i; ++i ) {
            data[i] = vec[i];
        }
    }

    ValueType   data[_v_length];
};

typedef CurveData<unsigned char, UCHAR_MAX>      CurveData8U;
typedef CurveData<signed char, SCHAR_MAX>        CurveData8S;

typedef CurveData<unsigned short, USHRT_MAX>     CurveData16U;
typedef CurveData<signed short, SHRT_MAX>        CurveData16S;

typedef CurveData<unsigned int, UINT_MAX>        CurveData32U;
typedef CurveData<signed int, INT_MAX>           CurveData32S;

typedef CurveData<float, UCHAR_MAX>              CurveData8F;
typedef CurveData<float, USHRT_MAX>              CurveData16F;
typedef CurveData<float, UINT_MAX>               CurveData32F;


/**
    \class CurveRange
    \since 0.5.0
    \brief The CurveRange class defines a value range for a
            specific type of curve.

            _v_end_range > _v_begin_range should always be true.
*/


/**
    \class CurveSampler
    \since 0.5.0
    \brief The base class for CurveSampler classes.
*/
template < class _t_base_range >
struct CurveDataSampler {
    typedef _t_base_range                   BaseRange;
    typedef typename BaseRange::ValueType   ValueType;

    template < class _t_dst_range >
    typename _t_dst_range::ValueType sampleAtIndex( const CurveData<ValueType, BaseRange::range>& data, size_t index ) const {
        const float value = BaseRange::mapValueToFloat(
                                data.at( index )
                            );
        return _t_dst_range::mapFloatToValue(
                   value
               );
    }
};

/**
    \class CurvePoint
    \since 0.5.0
    \brief The base class for CurvePoint classes. duh.
*/
template < class _t_index, class _t_value >
struct CurvePoint {
    typedef _t_index        IndexType;
    typedef _t_value        ValueType;

    /// constr. / assign
    explicit CurvePoint(
        IndexType _index = 0,
        ValueType _value = 0
    ) : value( _value ), index( _index ) {}
    CurvePoint(
        const CurvePoint& rhs
    ) : value( rhs.value ), index( rhs.index ) {}
    CurvePoint& operator = ( const CurvePoint& rhs ) {
        value = rhs.value;
        index = rhs.index;
        return *this;
    }

    /// comparison operators
    inline bool operator == ( const CurvePoint& rhs ) const { return ( index == rhs.index ) && ( value == rhs.value ); }
    inline bool operator != ( const CurvePoint& rhs ) const { return ( index != rhs.index ) || ( value != rhs.value ); }
    inline bool operator > ( const CurvePoint& rhs ) const { return ( index > rhs.index ); }
    inline bool operator < ( const CurvePoint& rhs ) const { return ( index < rhs.index ); }
    inline bool operator >= ( const CurvePoint& rhs ) const { return ( index >= rhs.index ); }
    inline bool operator <= ( const CurvePoint& rhs ) const { return ( index <= rhs.index ); }

    /// distance
    inline IndexType distance( const IndexType& _other_index ) { return std::max( index, _other_index ) - std::min( index, _other_index ); }
    inline IndexType distance( const CurvePoint& rhs ) { return distance( rhs.index ); }

    /// difference
    inline ValueType difference( const ValueType& _other_value ) { return ( value - _other_value ); }
    inline ValueType difference( const CurvePoint& rhs ) { return difference( rhs.value ); }

    ValueType       value;
    IndexType       index;
};

/**
    \class Curve
    \since 0.5.0
    \brief The high-level curve class.
*/
template < class _t_range, class _t_index >
struct Curve {
        typedef _t_range                            ValueRange;
        typedef _t_index                            IndexType;
        typedef typename ValueRange::ValueType      ValueType;
        typedef CurvePoint<IndexType, ValueType>     CurvePointType;
        typedef typename ValueRange::RangeType      RangeType;

        static const RangeType min = ValueRange::min;
        static const RangeType max = ValueRange::max;
        static const RangeType range = ValueRange::range;

        /// constr. / assign.
        Curve() {}
        Curve( const Curve& rhs ) {
            assign( rhs );
        }
        void assign( const Curve& rhs ) {
            m_Points = rhs.m_Points;
        }
        Curve& operator = ( const Curve& rhs ) {
            assign( rhs );
            return *this;
        }

        void clear() { m_Points.clear(); }
        bool empty() const { return m_Points.empty(); }
        size_t size() const { return m_Points.size(); }

        /// modifiers
        void merge( const Curve& other ) {
            for( auto it = other.m_Points.begin(); it != other.m_Points.end(); ++it ) {
                bool updatedValue( false );
                byIndex( ( *it ).index, ( *it ).value, &updatedValue );

                if( !updatedValue ) {
                    m_Points.push_back(
                        ( *it )
                    );
                }
            }

            sort();
        }
        void set( const IndexType& index, const ValueType& value ) {
            for( auto it = m_Points.begin(); it != m_Points.end(); ++it ) {
                if( ( *it ).index == index ) {
                    ( *it ).value = value;
                    return;
                }
            }

            /// append point
            CurvePointType  point(
                index,
                value
            );
            m_Points.push_back( point );

            /// sort the shit
            sort();
        }
        const ValueType& operator []( const IndexType& index ) const {
            for( auto it = m_Points.begin(); it != m_Points.end(); ++it ) {
                if( ( *it ).index == index ) {
                    return ( *it ).value;
                }
            }

            assert( false );
            return m_Points.front().value;
        }
        ValueType& operator []( const IndexType& index ) {
            for( auto it = m_Points.begin(); it != m_Points.end(); ++it ) {
                if( ( *it ).index == index ) {
                    return ( *it ).value;
                }
            }

            assert( false );
            return m_Points.front().value;
        }

        /// accessor
        bool containsIndex( const IndexType& index ) const {
            bool _containsIndex( false );
            ( void ) byIndex(
                index,
                &_containsIndex
            );
            return _containsIndex;
        }
        CurvePointType byIndex( const IndexType& index, const ValueType& newValue, bool* found = nullptr ) {
            for( auto it = m_Points.begin(); it != m_Points.end(); ++it ) {
                if( ( *it ).index == index ) {
                    CurvePointType current( *it );

                    if( found != nullptr ) {
                        *found = true;
                    }

                    ( *it ).value = newValue;

                    return current;
                }
            }

            if( found != nullptr ) {
                *found = false;
            }

            return m_Points.front();
        }
        CurvePointType byIndex( const IndexType& index, bool* found = nullptr ) const {
            /// since m_Points is always ordered in a specific way, a binary search is many times
            /// faster than a simple linear search algorithm.
            /// \todo implement binary search for curve objects.

            for( auto it = m_Points.begin(); it != m_Points.end(); ++it ) {
                if( ( *it ).index == index ) {
                    if( found != nullptr ) {
                        *found = true;
                    }

                    return ( *it );
                }
            }

            if( found != nullptr ) {
                *found = false;
            }

            return m_Points.front();
        }
        template < class _t_sampler >
        CurveData<ValueType, range>  sample( const _t_sampler& sampler, const IndexType _begin = min, const IndexType _max = max ) const {
            CurveData<ValueType, range>  data;

            for( IndexType _it = _begin; _max > _it; ++_it ) {
                bool found( false );
                data.data[_it] = sample(
                                     sampler,
                                     _it,
                                     &found
                                 ).value;
                ( void )found;

                assert( found == true );
            }

            return data;
        }
        template < class _t_sampler >
        CurvePointType sample( const _t_sampler& sampler, const IndexType& index, bool* found = nullptr ) const {
            static const CurvePointType emptyRet;

            /// check if the specified value is an indexed
            /// curve value.
            bool isValueIndexed( false );
            const auto indexedValue = byIndex(
                                          index,
                                          &isValueIndexed
                                      );

            if( isValueIndexed ) {
                if( found != nullptr ) {
                    *found = true;
                }

                return indexedValue;
            }

            /// range-check
            if( empty() || ( index > m_Points.back().index ) || ( index < m_Points.front().index ) ) {
                /// error: Specified index is out of
                ///     range
                if( found != nullptr ) {
                    *found = false;
                }

                return emptyRet;
            }

            /// find the nearest curve-point with a lower index
            /// than the current one
            const auto reverseFirstLowerPoint = std::find_if(
                                                    m_Points.begin(),
                                                    m_Points.end(),
            [ = ]( const CurvePointType & value ) -> bool {
                return ( value.index < index );
            }
                                                );
            assert( reverseFirstLowerPoint != m_Points.end() );

            const auto firstLowerPoint = reverseFirstLowerPoint; //.base();
            assert( firstLowerPoint != m_Points.end() );

            /// find the first curve-point with a higher index
            /// than the current one.
            const auto firstHighPoint = std::find_if(
                                            firstLowerPoint,
                                            m_Points.end(),
            [ = ]( const CurvePointType & value ) -> bool {
                return ( value.index > index );
            }
                                        );
            assert( firstHighPoint != m_Points.end() );

            if( found != nullptr ) {
                *found = true;
            }

            /// now call the sampler object using the two specified
            /// points.
            return sampler.sample(
                       *firstLowerPoint,
                       *firstHighPoint,
                       index
                   );
        }

    private:
        void sort() {
            std::sort(
                m_Points.begin(),
                m_Points.end(),
            []( const CurvePointType & first, CurvePointType & second ) -> bool {
                return ( first < second );
            }
            );
        }

        std::vector<CurvePointType>     m_Points;
};


/**
    \class BaseCurvePointSampler
    \since 1.0
    \brief Default interface for curve point
        sampler classes.
*/
template < class _t_range, class _t_index >
struct BaseCurvePointSampler {
    typedef _t_range                            ValueRange;
    typedef _t_index                            IndexType;
    typedef typename ValueRange::ValueType      ValueType;
    typedef CurvePoint<IndexType, ValueType>     CurvePointType;

    CurvePointType  sample( const CurvePointType& first, const CurvePointType& second, const IndexType& index ) const {
        assert( index > first.index );
        assert( index < second.index );

        return CurvePointType();
    }
};


/**
    \class LinearCurvePointSampler
    \since 1.0
    \brief Default interface for curve point
        sampler classes.
*/
template < class _t_range, class _t_index >
struct LinearCurvePointSampler {
    typedef _t_range                            ValueRange;
    typedef _t_index                            IndexType;
    typedef typename ValueRange::ValueType      ValueType;
    typedef CurvePoint<IndexType, ValueType>     CurvePointType;

    CurvePointType  sample( const CurvePointType& first, const CurvePointType& second, const IndexType& index ) const {
        assert( index > first.index );
        assert( index < second.index );

        const CurvePointType _empty_ret( 0, 0 );

        if( ( index <= first.index ) || ( index >= second.index ) ) {
            return _empty_ret;
        }

        const ValueType diff = std::max( first.value, second.value ) - std::min( first.value, second.value );

        if( diff != 0 ) {
            const IndexType len = second.index - first.index;
            const ValueType val = ( ( diff / len ) * ( index - first.index ) ) + first.value;

            return CurvePointType(
                       index,
                       val
                   );
        } else {
            /// two points equal a straight
            /// line
            return first;
        }
    }
};


}
