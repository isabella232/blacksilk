#include <libserialization++/SerializationStream.hpp>

#ifdef WIN32
#   include <Windows.h>
#endif

#include <fstream>
#include <cstring>

using namespace spp;

Stream::Stream() {

    this->m_Cursor      = 0;
    this->m_Length      = 0;

}

size_t  Stream::Length() const {

    return this->m_Length;

}

bool    Stream::SetPosition( size_t pos ) {

    if( pos <= m_Length ) {

        m_Cursor    = pos;

        return true;

    }

    return false;

}

size_t  Stream::GetPosition() const {

    return this->m_Cursor;

}

bool Stream::Move( size_t offset ) {

    return SetPosition( m_Cursor + offset );

}

void Stream::MoveBegin() {

    SetPosition( 0 );

}

void Stream::MoveEnd() {

    SetPosition( m_Length - 1 );

}

MemoryStream::MemoryStream() : Stream() {
    this->m_Buffer      = 0;
}

MemoryStream::MemoryStream( void* p, MemoryStreamMode::t mode ) : Stream() {

    this->m_Buffer      = 0;
    this->m_Mode        = spp::MemoryStreamMode::Buffer;

    Attach( p, mode );

}

MemoryStream::~MemoryStream() {

    if( this->m_Buffer ) {

        if( this->m_Mode == spp::MemoryStreamMode::Buffer ) {
            delete [] this->m_Buffer;
        }

        this->m_Buffer  = 0;
    }

}

void    MemoryStream::Attach( void* p, MemoryStreamMode::t mode, size_t length ) {

    if( this->m_Buffer ) {

        if( this->m_Mode == spp::MemoryStreamMode::Buffer ) {
            delete [] this->m_Buffer;
        }

        this->m_Buffer  = 0;
    }

    this->m_Mode        = mode;
    this->m_Buffer      = ( char* )p;
    this->m_Length      = length;

}


MemoryStreamMode::t         MemoryStream::GetMode() const {

    return this->m_Mode;

}

void*   MemoryStream::GetPointer() const {

    return this->m_Buffer;

}

size_t MemoryStream::Write( void* buffer, size_t length ) {

    if( this->m_Mode != spp::MemoryStreamMode::ReadWritable && this->m_Mode != spp::MemoryStreamMode::Writable &&
            this->m_Mode != spp::MemoryStreamMode::Buffer ) {

        return  0;

    }

    if( this->m_Mode == spp::MemoryStreamMode::Buffer ) {
        bool hasToResize( m_Length < m_Cursor + length );

        if( hasToResize ) {

            char*       newBuffer = new char[ m_Cursor + length ];
            memcpy( ( void* )newBuffer, ( const void* )m_Buffer, sizeof( char ) * m_Length );

            delete [] m_Buffer;

            m_Buffer = newBuffer;
            m_Length = m_Cursor + length;
        }
    }

    size_t  bytesWritten( 0 );
    size_t  cursor( 0 );
    char*   dataCursor( m_Buffer + m_Cursor );

    while( cursor + m_Cursor < m_Length && length != 0 ) {

        dataCursor[ cursor ] = ( ( char* )buffer )[ cursor ];

        --length;
        ++bytesWritten;
        ++cursor;
    }

    return bytesWritten;

}

size_t MemoryStream::Read( void* dst, size_t length ) {

    if( this->m_Mode != spp::MemoryStreamMode::ReadWritable && this->m_Mode != spp::MemoryStreamMode::Readable &&
            this->m_Mode != spp::MemoryStreamMode::Buffer ) {

        return  0;

    }

    size_t      bytesRead( 0 );
    char*       dataCursor( m_Buffer + m_Cursor );
    size_t      cursor( 0 );

    while( ( cursor + m_Cursor < m_Length ) && ( length != 0 ) ) {

        ( ( char* )dst )[cursor] = dataCursor[ cursor ];

        --length;
        ++bytesRead;
        ++cursor;
    }

    return bytesRead;

}

void MemoryStream::Flush() {

    /// Dummy
}

void MemoryStream::Close() {

    if( this->m_Mode == spp::MemoryStreamMode::Buffer ) {
        delete [] this->m_Buffer;
    }

    this->m_Buffer      = 0;
    this->m_Cursor      = 0;
    this->m_Length      = 0;
}


FileStream::FileStream() : Stream() {
    this->m_OpenMode        = spp::FileOpenMode::AlwaysOpen;
    this->m_FileMode        = spp::FileStreamMode::ReadWritable;
}

FileStream::FileStream( const std::string& path, FileOpenMode::t openMode, FileStreamMode::t fileMode ) : Stream() {

    this->m_OpenMode        = spp::FileOpenMode::AlwaysOpen;
    this->m_FileMode        = spp::FileStreamMode::ReadWritable;

    this->Open( path, openMode, fileMode );
}

FileStream::~FileStream() {
    /// Dummy
    Close();
}

bool FileStream::Open( const std::string& path, FileOpenMode::t openMode, FileStreamMode::t fileMode ) {

    Close();

    std::ifstream       inputFile;

    if( openMode == spp::FileOpenMode::BinaryAlwaysOpen || openMode == spp::FileOpenMode::BinaryOpen ) {
        inputFile.open( path, std::ios_base::binary );
    } else {
        inputFile.open( path );
    }

    if( openMode == spp::FileOpenMode::AlwaysOpen || openMode == spp::FileOpenMode::Open || openMode == spp::FileOpenMode::BinaryAlwaysOpen || openMode == spp::FileOpenMode::BinaryOpen ) {
        if( inputFile.is_open() ) {

            char*           buffer( 0 );
            size_t          length( 0 );

            size_t          begin( 0 );
            size_t          end( 0 );

            begin = inputFile.tellg();

            inputFile.seekg( 0, std::ios_base::end );

            end = inputFile.tellg();

            inputFile.seekg( 0, std::ios_base::beg );

            length = end - begin;

            if( length != 0 ) {

                buffer = new char[ length * sizeof( char ) ];

                inputFile.read( buffer, length * sizeof( char ) );

                this->m_BufferStream.Attach( ( void* )buffer, spp::MemoryStreamMode::Buffer, length );

                this->m_Path            = path;
                this->m_FileMode        = fileMode;
                this->m_OpenMode        = openMode;
                this->m_Length          = length;

                return true;
            } else {

                this->m_BufferStream.Attach( ( void* )0, spp::MemoryStreamMode::Buffer );

                this->m_Path            = path;
                this->m_FileMode        = fileMode;
                this->m_OpenMode        = openMode;
                this->m_Length          = length;

                return true;
            }

            inputFile.close();
        } else if( openMode == spp::FileOpenMode::AlwaysOpen || openMode == spp::FileOpenMode::BinaryAlwaysOpen ) {

            std::ofstream       out( path.c_str() );

            if( out.is_open() ) {

                out.flush();
                out.close();

                return Open( path, openMode, fileMode );

            }

            return false;

        }

        return false;

    } else if( openMode == spp::FileOpenMode::AlwaysCreate || openMode == spp::FileOpenMode::BinaryAlwaysCreate ) {

#ifdef WIN32
        DeleteFileA( m_Path.c_str() );
#endif

        this->m_BufferStream.Attach( ( void* )0, spp::MemoryStreamMode::Buffer );

        this->m_Path            = path;
        this->m_FileMode        = fileMode;
        this->m_OpenMode        = openMode;

        return true;

    }

    return false;

}

std::string FileStream::Path() const {

    return this->m_Path;

}

FileOpenMode::t     FileStream::OpenMode() const {

    return this->m_OpenMode;

}

FileStreamMode::t   FileStream::Mode() const {

    return this->m_FileMode;

}

size_t FileStream::Write( void* buffer, size_t length ) {

    if( this->m_FileMode == spp::FileStreamMode::ReadWritable ||
            this->m_FileMode == spp::FileStreamMode::Writable ) {

        size_t written = this->m_BufferStream.Write( buffer, length );

        m_Length = this->m_BufferStream.Length();

        return written;

    }

    return 0;

}

size_t FileStream::Read( void* dst, size_t length ) {

    if( this->m_FileMode == spp::FileStreamMode::ReadWritable ||
            this->m_FileMode == spp::FileStreamMode::Readable ) {

        return this->m_BufferStream.Read( dst, length );

    }

    return 0;

}

void FileStream::Flush() {

    if( this->m_FileMode != spp::FileStreamMode::Readable ) {

        if( this->m_OpenMode == spp::FileOpenMode::AlwaysCreate || this->m_OpenMode == spp::FileOpenMode::BinaryAlwaysCreate ) {
#ifdef WIN32
            DeleteFileA( m_Path.c_str() );
#endif

            std::ofstream   out;

            if( this->m_OpenMode == spp::FileOpenMode::BinaryAlwaysCreate ) {
                out.open( this->m_Path, std::ios_base::binary );
            } else {
                out.open( this->m_Path );
            }

            out.write( ( char* )this->m_BufferStream.GetPointer(), m_BufferStream.Length() * sizeof( char ) );
            out.flush();
            out.close();

        } else {

            std::ofstream   out;

            if( this->m_OpenMode == spp::FileOpenMode::BinaryAlwaysOpen || this->m_OpenMode == spp::FileOpenMode::BinaryOpen ) {
                out.open( this->m_Path, std::ios_base::binary );
            } else {
                out.open( this->m_Path );
            }

            out.write( ( char* )this->m_BufferStream.GetPointer(), m_BufferStream.Length() * sizeof( char ) );
            out.flush();
            out.close();

        }

    }

    return;

}

void FileStream::Close() {
    this->m_Path.clear();

    this->m_BufferStream.Close();
}


size_t  FileStream::Length() const {

    return m_BufferStream.Length();

}

bool    FileStream::SetPosition( size_t pos ) {

    return m_BufferStream.SetPosition( pos );

}

size_t  FileStream::GetPosition() const {

    return m_BufferStream.GetPosition();

}

bool FileStream::Move( size_t offset ) {

    return m_BufferStream.Move( offset );

}

void FileStream::MoveBegin() {

    m_BufferStream.SetPosition( 0 );

}

void FileStream::MoveEnd() {

    m_BufferStream.SetPosition( m_Length - 1 );

}
