
#include <libserialization++/PropertyEncoder.hpp>

using namespace spp;

template < class _t_value >
struct  StandardConverter {
    static void Convert( Property& dst, std::string val ) {

        _t_value    dstValue = 0;

        std::stringstream       ss;

        ss << val;
        ss >> dstValue;

        dst.GetSetter().Set< _t_value >( dstValue );
    }
    static void Convert( Property& dst, std::wstring val ) {

        _t_value    dstValue = 0;

        std::wstringstream       ss;

        ss << val;
        ss >> dstValue;

        dst.GetSetter().Set< _t_value >( dstValue );
    }
};

template <>
struct  StandardConverter<std::wstring> {

    static void Convert( Property& dst, std::string val ) {
        std::wstring dstValue( val.begin(), val.end() );

        dst.GetSetter().Set< std::wstring >( dstValue );
    }
    static void Convert( Property& dst, std::wstring val ) {
        std::wstring dstValue( val.begin(), val.end() );

        dst.GetSetter().Set< std::wstring >( dstValue );
    }
};


void PropertyEncoder::Encode( Property& dst, std::string val ) {

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
    static const auto STRTypeInfo = spp::GetTypeInfo<std::string>();
    static const auto WSTRTypeInfo = spp::GetTypeInfo<std::wstring>();

    auto dstTypeInfo              = dst.GetTypeInfo();

    if( dstTypeInfo == UCTypeInfo ) {
        StandardConverter<unsigned char>::Convert( dst, val );
        return;
    }

    if( dstTypeInfo == USTypeInfo ) {
        StandardConverter<unsigned short>::Convert( dst, val );
        return;
    }

    if( dstTypeInfo == UITypeInfo ) {
        StandardConverter<unsigned int>::Convert( dst, val );
        return;
    }

    if( dstTypeInfo == ULTypeInfo ) {
        StandardConverter<unsigned long>::Convert( dst, val );
        return;
    }

    if( dstTypeInfo == ULLTypeInfo ) {
        StandardConverter<unsigned long long>::Convert( dst, val );
        return;
    }

    if( dstTypeInfo == CTypeInfo ) {
        StandardConverter<char>::Convert( dst, val );
        return;
    }

    if( dstTypeInfo == STypeInfo ) {
        StandardConverter<short>::Convert( dst, val );
        return;
    }

    if( dstTypeInfo == ITypeInfo ) {
        StandardConverter<int>::Convert( dst, val );
        return;
    }

    if( dstTypeInfo == LTypeInfo ) {
        StandardConverter<long>::Convert( dst, val );
        return;
    }

    if( dstTypeInfo == LLTypeInfo ) {
        StandardConverter<long long>::Convert( dst, val );
        return;
    }

    if( dstTypeInfo == FTypeInfo ) {
        StandardConverter<float>::Convert( dst, val );
        return;
    }

    if( dstTypeInfo == DTypeInfo ) {
        StandardConverter<double>::Convert( dst, val );
        return;
    }

    if( dstTypeInfo == CPCTypeInfo ) {
        StandardConverter<char*>::Convert( dst, val );
        return;
    }

    if( dstTypeInfo == STRTypeInfo ) {
        StandardConverter<std::string>::Convert( dst, val );
        return;
    }

    if( dstTypeInfo == WSTRTypeInfo ) {
        StandardConverter<std::wstring>::Convert( dst, val );
        return;
    }

}

