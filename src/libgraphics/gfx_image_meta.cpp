#include <libgraphics/image.hpp>

namespace libgraphics {

bool ImageMetaInfoTag::operator == ( const ImageMetaInfoTag& rhs ) const {
    return ( this->m_Name == rhs.m_Name ) && ( this->m_Length == rhs.m_Length ) &&
           ( this->m_Data == rhs.m_Data );
}

bool ImageMetaInfoTag::operator != ( const ImageMetaInfoTag& rhs ) const {
    return ( this->m_Name != rhs.m_Name ) || ( this->m_Length != rhs.m_Length ) ||
           ( this->m_Data != rhs.m_Data );
}

const std::string& ImageMetaInfoTag::name() const {
    return this->m_Name;
}

const std::string& ImageMetaInfoTag::data() const {
    return this->m_Data;
}

size_t ImageMetaInfoTag::length() const {
    return this->m_Length;
}

void ImageMetaInfoTag::assign( const std::string& _name, const std::string& _data ) {
    this->m_Length      = _data.length();
    this->m_Data        = _data;
    this->m_Name        = _name;
}

void ImageMetaInfoTag::assign( const std::string& _name, char* _data, size_t _length ) {
    assert( _length > 0 );
    assert( _data != nullptr );

    this->m_Length      = _length;
    this->m_Name        = _name;
    this->m_Data.assign( _data, _length );
}

void ImageMetaInfoTag::reset() {
    this->m_Name.clear();
    this->m_Data.clear();
    this->m_Length = 0;
}

void ImageMetaInfoDirectory::add( const ImageMetaInfoTag& tag ) {
    this->m_Tags.push_back( tag );
}

void ImageMetaInfoDirectory::add( const std::string& _name, const std::string& _data ) {
    this->m_Tags.emplace_back( _name, _data );
}

void ImageMetaInfoDirectory::add( const std::string& _name, char* _data, size_t _length ) {
    this->m_Tags.emplace_back( _name, _data, _length );
}

bool ImageMetaInfoDirectory::remove( Iterator it ) {
    if( it != m_Tags.end() ) {
        m_Tags.erase( it );
        return true;
    }

    return false;
}

bool ImageMetaInfoDirectory::remove( size_t index ) {
    assert( this->m_Tags.size() > index );

    if( m_Tags.size() < index ) {
        return false;
    }

    return remove(
               m_Tags.begin() + index
           );
}

bool ImageMetaInfoDirectory::remove( const ImageMetaInfoTag& tag ) {
    for( auto it = m_Tags.begin(); it != m_Tags.end(); ++it ) {
        if( ( *it ) == tag ) {
            m_Tags.erase( it );

            return true;
        }
    }

    return false;
}

bool ImageMetaInfoDirectory::removeByName( const std::string& _name ) {
    for( auto it = m_Tags.begin(); it != m_Tags.end(); ++it ) {
        if( ( *it ).name() == _name ) {
            m_Tags.erase( it );

            return true;
        }
    }

    return false;
}

void ImageMetaInfoDirectory::reset() {
    m_Tags.clear();
    m_Name.clear();
}

const std::string& ImageMetaInfoDirectory::name() const {
    return this->m_Name;
}

size_t ImageMetaInfoDirectory::size() const {
    return this->m_Tags.size();
}

bool ImageMetaInfoDirectory::empty() const {
    return this->m_Tags.empty();
}

void ImageMetaInfoDirectory::assign( Iterator first, Iterator last ) {
    this->m_Tags.assign(
        first,
        last
    );
}

void ImageMetaInfoDirectory::assign( const ImageMetaInfoDirectory& other ) {
    this->m_Tags.assign(
        other.m_Tags.begin(),
        other.m_Tags.end()
    );
}

ImageMetaInfoDirectory& ImageMetaInfoDirectory::operator = ( const ImageMetaInfoDirectory& other ) {
    assign( other );

    return *this;
}

bool ImageMetaInfoDirectory::contains( const ImageMetaInfoTag& tag ) const {
    for( auto it = m_Tags.begin(); it != m_Tags.end(); ++it ) {
        if( ( *it ) == tag ) {
            return true;
        }
    }

    return false;
}

bool ImageMetaInfoDirectory::containsWithName( const std::string& _name ) const {
    for( auto it = m_Tags.begin(); it != m_Tags.end(); ++it ) {
        if( ( *it ).name() == _name ) {
            return true;
        }
    }

    return false;
}

ImageMetaInfoDirectory::ConstIterator ImageMetaInfoDirectory::findByName( const std::string& _name ) const {
    for( auto it = m_Tags.begin(); it != m_Tags.end(); ++it ) {
        if( ( *it ).name() == _name ) {
            return it;
        }
    }

    return m_Tags.end();
}

ImageMetaInfoDirectory::Iterator ImageMetaInfoDirectory::begin() {
    return m_Tags.begin();
}

ImageMetaInfoDirectory::ConstIterator ImageMetaInfoDirectory::begin() const {
    return m_Tags.begin();
}

ImageMetaInfoDirectory::ConstIterator ImageMetaInfoDirectory::constBegin() const {
    return m_Tags.cbegin();
}

ImageMetaInfoDirectory::Iterator ImageMetaInfoDirectory::end() {
    return m_Tags.end();
}

ImageMetaInfoDirectory::ConstIterator ImageMetaInfoDirectory::end() const {
    return m_Tags.end();
}

ImageMetaInfoDirectory::ConstIterator ImageMetaInfoDirectory::constEnd() const {
    return m_Tags.cend();
}

ImageMetaInfo::ImageMetaInfo() {}

ImageMetaInfoDirectory& ImageMetaInfo::addDirectory(
    const std::string& _name
) {
    return emplaceDirectory(
               _name
           );
}

ImageMetaInfoDirectory& ImageMetaInfo::emplaceDirectory( const std::string& _name ) {
    this->m_Directories.emplace_back( _name );
    return m_Directories.back();
}

ImageMetaInfoDirectory& ImageMetaInfo::emplaceDirectory( ImageMetaInfoDirectory::Iterator first, ImageMetaInfoDirectory::Iterator last ) {
    m_Directories.emplace_back();
    m_Directories.back().assign(
        first,
        last
    );

    return m_Directories.back();
}

bool ImageMetaInfo::removeDirectory(
    const ImageMetaInfoDirectory& dir
) {
    for( auto it = m_Directories.begin(); it != m_Directories.end(); ++it ) {
        if( dir == ( *it ) ) {
            m_Directories.erase( it );
            return true;
        }
    }

    return false;
}

bool ImageMetaInfo::removeDirectoryByName(
    const std::string& _name
) {
    for( auto it = m_Directories.begin(); it != m_Directories.end(); ++it ) {
        if( ( *it ).name() == _name ) {
            m_Directories.erase( it );
            return true;
        }
    }

    return false;
}

bool ImageMetaInfo::removeDirectoryByIndex(
    size_t index
) {
    if( m_Directories.size() > index ) {
        m_Directories.erase( m_Directories.begin() + index );
        return true;
    }

    return false;
}

ImageMetaInfoDirectory& ImageMetaInfo::directoryByName( const std::string& _name ) {
    for( auto it = m_Directories.begin(); it != m_Directories.end(); ++it ) {
        if( ( *it ).name() == _name ) {
            return ( *it );
        }
    }

    assert( false );
    return m_Directories.front();
}

const ImageMetaInfoDirectory& ImageMetaInfo::directoryByName( const std::string& _name ) const {
    for( auto it = m_Directories.begin(); it != m_Directories.end(); ++it ) {
        if( ( *it ).name() == _name ) {
            return ( *it );
        }
    }

    assert( false );
    return m_Directories.front();
}

ImageMetaInfoDirectory& ImageMetaInfo::directoryByIndex( size_t index ) {
    assert( m_Directories.size() > index );
    return *( m_Directories.begin() + index );
}

const ImageMetaInfoDirectory& ImageMetaInfo::directoryByIndex( size_t index ) const {
    assert( m_Directories.size() > index );
    return *( m_Directories.begin() + index );
}

size_t ImageMetaInfo::countDirectories() const {
    return m_Directories.size();
}

bool ImageMetaInfo::containsDirectory( const std::string& _name ) const {
    for( auto it = m_Directories.begin(); it != m_Directories.end(); ++it ) {
        if( ( *it ).name() == _name ) {
            return true;
        }
    }

    return false;
}

bool ImageMetaInfo::containsDirectory( const ImageMetaInfoDirectory& _dir ) const {
    for( auto it = m_Directories.begin(); it != m_Directories.end(); ++it ) {
        if( _dir == ( *it ) ) {
            return true;
        }
    }

    return false;
}

void ImageMetaInfo::clearDirectories() {
    m_Directories.clear();
}

void ImageMetaInfo::reset() {
    clearDirectories();
}

bool ImageMetaInfo::empty() const {
    return this->m_Directories.empty();
}

}
