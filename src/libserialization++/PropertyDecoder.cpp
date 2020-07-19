
#include <libserialization++/PropertyDecoder.hpp>

using namespace spp;

std::string   PropertyDecoder::Decode( spp::Property& property ) {

    spp::PropertyTypeInfo  info = property.GetTypeInfo();

    if( info == spp::GetTypeInfo<std::string>() ) {

        std::string*             val;

        if( property.GetGetter().GetGetter()->operator()( info, ( void* )&val ) ) {
            return std::string( val->begin(), val->end() );
        }

    } else if( info == spp::GetTypeInfo<std::wstring>() ) {

        std::wstring*             val;

        if( property.GetGetter().GetGetter()->operator()( info, ( void* )&val ) ) {
            return std::string( val->begin(), val->end() );
        }

    } else {

        void*                   val; //  = (void*)new char[ info.TypeSize ];

        if( property.GetGetter().GetGetter()->operator()( info, ( void* )&val ) ) {
            std::string result = PropertyDecoder::Decode( info, ( void* )val );

            return result;
        }

    }

    return std::string();
}

std::string   PropertyDecoder::Decode( spp::PropertyTypeInfo& info,  void* p ) {

    static const auto UCTypeInfo = spp::GetTypeInfo<unsigned char>();
    static const auto USTypeInfo = spp::GetTypeInfo<unsigned short>();
    static const auto UITypeInfo = spp::GetTypeInfo<unsigned int>();
    static const auto ULTypeInfo = spp::GetTypeInfo<unsigned long>();
    static const auto ULLTypeInfo = spp::GetTypeInfo<unsigned long long>();
    static const auto CTypeInfo = spp::GetTypeInfo<char>();
    static const auto STypeInfo = spp::GetTypeInfo<short>();
    static const auto ITypeInfo = spp::GetTypeInfo<int>();
    static const auto LTypeInfo = spp::GetTypeInfo<long>();
    static const auto LLTypeInfo = spp::GetTypeInfo<long long>();
    static const auto FTypeInfo = spp::GetTypeInfo<float>();
    static const auto DTypeInfo = spp::GetTypeInfo<double>();
    static const auto CPCTypeInfo = spp::GetTypeInfo<char*>();

    if( info == UCTypeInfo ) {
        return PropertyDecoder::Decode( ( unsigned char* )p );
    }

    if( info == USTypeInfo ) {
        return PropertyDecoder::Decode( ( unsigned short* )p );
    }

    if( info == UITypeInfo ) {
        return PropertyDecoder::Decode( ( unsigned int* )p );
    }

    if( info == ULTypeInfo ) {
        return PropertyDecoder::Decode( ( unsigned long* )p );
    }

    if( info == ULLTypeInfo ) {
        return PropertyDecoder::Decode( ( unsigned long long* )p );
    }

    if( info == CTypeInfo ) {
        return PropertyDecoder::Decode( ( char* )p );
    }

    if( info == STypeInfo ) {
        return PropertyDecoder::Decode( ( short* )p );
    }

    if( info == ITypeInfo ) {
        return PropertyDecoder::Decode( ( int* )p );
    }

    if( info == LTypeInfo ) {
        return PropertyDecoder::Decode( ( long* )p );
    }

    if( info == LLTypeInfo ) {
        return PropertyDecoder::Decode( ( long long* )p );
    }

    if( info == FTypeInfo ) {
        return PropertyDecoder::Decode( ( float* )p );
    }

    if( info == DTypeInfo ) {
        return PropertyDecoder::Decode( ( double* )p );
    }

    if( info == CPCTypeInfo ) {
        return PropertyDecoder::Decode( ( char** )p );
    }

    return std::string();
}

std::string   PropertyDecoder::Decode( unsigned char* val ) {

    std::stringstream   ss;

    ss << *val;

    return ss.str();

}

std::string   PropertyDecoder::Decode( unsigned short* val ) {

    std::stringstream   ss;

    ss << *val;

    return ss.str();

}

std::string   PropertyDecoder::Decode( unsigned int* val ) {

    std::stringstream   ss;

    ss << *val;

    return ss.str();

}

std::string   PropertyDecoder::Decode( unsigned long* val ) {

    std::stringstream   ss;

    ss << *val;

    return ss.str();

}

std::string   PropertyDecoder::Decode( unsigned long long* val ) {

    std::stringstream   ss;

    ss << *val;

    return ss.str();

}

std::string   PropertyDecoder::Decode( char* val ) {

    std::stringstream   ss;

    ss << *val;

    return ss.str();

}

std::string   PropertyDecoder::Decode( short* val ) {

    std::stringstream   ss;

    ss << *val;

    return ss.str();

}

std::string   PropertyDecoder::Decode( int* val ) {

    std::stringstream   ss;

    ss << *val;

    return ss.str();

}

std::string   PropertyDecoder::Decode( long* val ) {

    std::stringstream   ss;

    ss << *val;

    return ss.str();

}

std::string   PropertyDecoder::Decode( long long* val ) {

    std::stringstream   ss;

    ss << *val;

    return ss.str();

}

std::string   PropertyDecoder::Decode( double* val ) {

    std::stringstream   ss;

    ss << *val;

    return ss.str();

}

std::string   PropertyDecoder::Decode( float* val ) {

    std::stringstream   ss;

    ss << *val;

    return ss.str();

}

std::string   PropertyDecoder::Decode( char** val ) {

    return std::string( *val );

}


