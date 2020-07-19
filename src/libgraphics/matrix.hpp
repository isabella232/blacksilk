#pragma once

#include <cstdint>
#include <cmath>
#include <limits>
#include <utility>

namespace libgraphics {

template < class _t_any, size_t _v_size >
struct Matrix {
    typedef _t_any      ValueType;

    static const size_t size = _v_size;
    static const size_t rows = _v_size;
    static const size_t columns = _v_size;

    Matrix() { reset(); }
    explicit Matrix( const ValueType& value ) {
        reset( value );
    }
    Matrix( const Matrix& rhs ) {
        assign( rhs );
    }

    void assign( float* data ) {
        memcpy( ( void* )Values, ( const void* )data, sizeof( ValueType ) * rows * columns );
    }
    void assign( const Matrix& rhs ) {
        memcpy( ( void* )Values, ( const void* )rhs.Values, sizeof( ValueType ) * rows * columns );
    }

    Matrix operator + ( const Matrix& rhs ) const {
        Matrix tmp( Values );

        tmp += rhs;

        return tmp;
    }
    Matrix& operator += ( const Matrix& rhs ) {
        for( size_t x = 0; rows > x; ++x ) {
            for( size_t y = 0; columns > y; ++y ) {
                Values[x][y] += rhs.Values[x][y];
            }
        }

        return *this;
    }

    Matrix operator * ( const Matrix& rhs ) const {
        Matrix tmp( Values );

        tmp *= rhs;

        return tmp;
    }
    Matrix& operator *= ( const Matrix& rhs ) {
        for( size_t x = 0; rows > x; ++x ) {
            for( size_t y = 0; columns > y; ++y ) {
                ValueType value( 0 );

                for( size_t i = 0; size > i; ++i ) {
                    value += Values[x][i] * rhs.Values[i][y];
                }

                Values[x][y] = value;
            }
        }

        return *this;
    }

    float* operator []( size_t index ) {
        assert( index < rows );

        return ( float* )Values[index];
    }

    ValueType& operator []( const std::pair<size_t, size_t>& position ) {
        assert( position.x < rows );
        assert( position.y < columns );

        return Values[position.first][position.second];
    }
    const ValueType& operator []( const std::pair<size_t, size_t>& position ) const {
        assert( position.x < rows );
        assert( position.y < columns );

        return Values[position.first][position.second];
    }

    float* data() const { return ( float* )Values; }
    float* row( size_t index ) const {
        return ( ( float* )Values + ( index * columns ) );
    }

    bool operator == ( const Matrix& rhs ) const {
        for( size_t x = 0; rows > x; ++x ) {
            for( size_t y = 0; columns > y; ++y ) {
                if( Values[x][y] != rhs.Values[x][y] ) {
                    return false;
                }
            }
        }

        return true;
    }
    bool operator != ( const Matrix& rhs ) const {
        for( size_t x = 0; rows > x; ++x ) {
            for( size_t y = 0; columns > y; ++y ) {
                if( Values[x][y] == rhs.Values[x][y] ) {
                    return false;
                }
            }
        }

        return true;
    }

    void reset() { reset( static_cast<ValueType>( 0 ) ); }
    void reset( const ValueType& value ) {
        for( size_t x = 0; rows > x; ++x ) {
            for( size_t y = 0; columns > y; ++y ) {
                Values[x][y] = value;
            }
        }
    }

    ValueType   Values[_v_size][_v_size];
};

typedef Matrix<float, 4>     Matrix16F;
typedef Matrix<double, 4>    Matrix16D;
typedef Matrix<int, 4>       Matrix16I;

typedef Matrix<float, 3>     Matrix9F;
typedef Matrix<double, 3>    Matrix9D;
typedef Matrix<int, 3>       Matrix9I;


}
