/**
    Copyright 2013 by FD Imaging

    http://fd-imaging.com

    All rights reserved.
*/
#pragma once

#include <string>

namespace spp {

/**
    \class      Stream
    \since      0.4.0-0
    \brief
        The base class for every
        serialization stream.
*/
class   Stream {
    public:
        /**
            \fn     Stream
            \since  0.4.0-0
            \brief
                Constructs a new empty stream
                instance.
        */
        Stream();

        /**
            \fn     ~Stream
            \since  0.4.0-0
            \brief
                Frees the internal data.
        */
        virtual ~Stream() {}

        /**
            \fn     Length
            \since  0.4.0-0
            \brief
                Returns the length of the current
                internal stream.
        */
        virtual size_t  Length() const;

        /**
            \fn     SetPosition
            \since  0.4.0-0
            \brief
                Sets the accessor cursor position.
        */
        virtual bool    SetPosition( size_t pos );

        /**
            \fn     GetPosition
            \since  0.4.0-0
            \brief
                Returns the current accessor cursor position.
        */
        virtual size_t  GetPosition() const;

        /**
            \fn     Move
            \since  0.4.0-0
            \brief
                Moves the current cursor position by offset.
        */
        virtual bool Move( size_t offset );

        /**
            \fn     MoveBegin
            \since  0.4.0-0
            \brief
                Moves the current cursor to the first
                byte( always pos = 0 ).
        */
        virtual void MoveBegin();

        /**
            \fn     MoveEnd
            \since  0.4.0-0
            \brief
                Moves the current cursor to the last
                byte( always pos = Length ).
        */
        virtual void MoveEnd();

        /**
            \fn     Write
            \since  0.4.0-0
            \brief
                Writes a specific blob of data to the current cursor
                position.
        */
        virtual size_t Write( void* buffer, size_t length ) = 0;

        /**
            \fn     Read
            \since  0.4.0-0
            \brief
                Reads a specific range of data from the current cursor
                to the specified target buffer.
        */
        virtual size_t Read( void* dst, size_t length ) = 0;

        /**
            \fn     Flush
            \since  0.4.0-0
            \brief
                Flushes the internal buffer to the specified
                target.
        */
        virtual void Flush() = 0;

        /**
            \fn     Close
            \since  0.4.0-0
            \brief
                Resets the internal state and closes
                the stream.
        */
        virtual void Close() = 0;
    protected:
        size_t          m_Cursor;
        size_t          m_Length;
};

/**
    \class      MemoryStreamMode
    \since      0.4.0-0
    \brief
        The MemoryStreamMode influences the memory-management
        behavior of a MemoryStream.
*/
namespace MemoryStreamMode  {
enum t {

    /**
        Only read from the attached buffer.
    */
    Readable,

    /**
        Only write to the attached buffer.
    */
    Writable,

    /**
        Read and write to the attached buffer.
    */
    ReadWritable,

    /**
        Create a new buffer and own it.
    */
    Buffer

};
}

/**
    \class      MemoryStream
    \since      0.4.0-0
    \brief
        The MemoryStream is attaches a Stream interface
        to a specific or unspecific buffer object.
*/
class   MemoryStream : public Stream {
    public:

        /**
            \fn     MemoryStream
            \since  0.4.0-0
            \brief
                Constructs a new empty MemoryStream. Sets the internal
                mode to "Buffer".
        */
        MemoryStream();

        /**
            \fn     MemoryStream
            \since  0.4.0-0
            \brief
                Constructs a new MemoryStream and attaches the new instance
                to the specified buffer pointer.
        */
        MemoryStream( void* p, MemoryStreamMode::t mode );

        /**
            \fn     ~MemoryStream
            \since  0.4.0-0
            \brief
                Frees the current blob and disposes
                the internal state.
        */
        ~MemoryStream();

        /**
            \fn     Attach
            \since  0.4.0-0
            \brief
                Attaches a new buffer to the current state. Disposes the
                old state.
        */
        void    Attach( void* p, MemoryStreamMode::t mode, size_t length = 0 );

        /**
            \fn     GetMode
            \since  0.4.0-0
            \brief
                Returns the current memory stream
                mode.
        */
        MemoryStreamMode::t         GetMode() const;

        /**
            \fn         GetPointer
            \since      0.4.0-0
            \brief
                Returns the pointer to the internal
                buffer.
        */
        void*   GetPointer() const;

        /**
            \fn     Write
            \since  0.4.0-0
            \brief
                Writes a specific blob of data to the current cursor
                position.
        */
        virtual size_t Write( void* buffer, size_t length );

        /**
            \fn     Read
            \since  0.4.0-0
            \brief
                Reads a specific range of data from the current cursor
                to the specified target buffer.
        */
        virtual size_t Read( void* dst, size_t length );

        /**
            \fn     Flush
            \since  0.4.0-0
            \brief
                Flushes the internal buffer to the specified
                target.
        */
        virtual void Flush();

        /**
            \fn     Close
            \since  0.4.0-0
            \brief
                Resets the internal state and closes
                the stream.
        */
        virtual void Close();
    protected:
        MemoryStreamMode::t         m_Mode;
        char*                       m_Buffer;
};

/**
    \enum   FileStreamMode
    \since  0.4.0-0
    \brief
        The FileStreamMode enumeration influences the
        read/write behavior of the FileStream.
*/
namespace   FileStreamMode {
enum t {

    /**
        The stream only reads from the internal
        file object.
    */
    Readable,

    /**
        The stream only writes to the internal
        file object.
    */
    Writable,

    /**
        The stream reads and writes to the internal
        file object
    */
    ReadWritable
};
}

/**
    \enum   FileOpenMode
    \since  0.4.0-0
    \brief
        The FileOpenMode enumeration influences the
        open/create behavior of the FileStream.
*/
namespace   FileOpenMode {
enum t {

    /**
        Always creates a new file.
    */
    AlwaysCreate,

    /**
        Opens only existing file. Creates
        a new file, if not existing.
    */
    AlwaysOpen,

    /**
        Opens only existing files.
    */
    Open,


    ///
    ///     Binary Stuff
    ///

    /**
        Always creates a new file.
    */
    BinaryAlwaysCreate,

    /**
        Opens only existing file. Creates
        a new file, if not existing.
    */
    BinaryAlwaysOpen,

    /**
        Opens only existing files.
    */
    BinaryOpen
};
}

/**
    \class      FileStream
    \since      0.4.0-0
    \brief
        The FileStream class implements the Stream interface
        for filesystem-based streaming.
*/
class FileStream : public Stream {
    public:

        /**
            \fn         FileStream
            \since      0.4.0-0
            \brief
                Constructs a new empty FileStream instance.
        */
        FileStream();

        /**
            \fn         FileStream
            \since      0.4.0-0
            \brief
                Constructs a new FileStream and opens the specified
                file path.
        */
        FileStream( const std::string& path, FileOpenMode::t openMode, FileStreamMode::t fileMode );

        /**
            \fn     ~MemoryStream
            \since  0.4.0-0
            \brief
                Frees the current blob and disposes
                the internal state.
        */
        ~FileStream();

        /**
            \fn         Open
            \since      0.4.0-0
            \brief
                Opens or creates a new file. Disposes the current internal
                state.
        */
        bool Open( const std::string& path, FileOpenMode::t openMode, FileStreamMode::t fileMode );

        /**
            \fn         Path
            \since      0.4.0-0
            \brief
                Returns the current file
                path.
        */
        std::string Path() const;

        /**
            \fn         OpenMode
            \since      0.4.0-0
            \brief
                Returns the current open
                mode.
        */
        FileOpenMode::t     OpenMode() const;

        /**
            \fn         Mode
            \since      0.4.0-0
            \brief
                Returns the current file stream
                mode.
        */
        FileStreamMode::t   Mode() const;

        /**
            \fn     Write
            \since  0.4.0-0
            \brief
                Writes a specific blob of data to the current cursor
                position.
        */
        virtual size_t Write( void* buffer, size_t length );

        /**
            \fn     Read
            \since  0.4.0-0
            \brief
                Reads a specific range of data from the current cursor
                to the specified target buffer.
        */
        virtual size_t Read( void* dst, size_t length );

        /**
            \fn     Flush
            \since  0.4.0-0
            \brief
                Flushes the internal buffer to the specified
                target.
        */
        virtual void Flush();

        /**
            \fn     Close
            \since  0.4.0-0
            \brief
                Resets the internal state and closes
                the stream.
        */
        virtual void Close();

        /**
            \fn     Length
            \since  0.4.0-0
            \brief
                Returns the length of the current
                internal stream.
        */
        virtual size_t  Length() const;

        /**
            \fn     SetPosition
            \since  0.4.0-0
            \brief
                Sets the accessor cursor position.
        */
        virtual bool    SetPosition( size_t pos );

        /**
            \fn     GetPosition
            \since  0.4.0-0
            \brief
                Returns the current accessor cursor position.
        */
        virtual size_t  GetPosition() const;

        /**
            \fn     Move
            \since  0.4.0-0
            \brief
                Moves the current cursor position by offset.
        */
        virtual bool Move( size_t offset );

        /**
            \fn     MoveBegin
            \since  0.4.0-0
            \brief
                Moves the current cursor to the first
                byte( always pos = 0 ).
        */
        virtual void MoveBegin();

        /**
            \fn     MoveEnd
            \since  0.4.0-0
            \brief
                Moves the current cursor to the last
                byte( always pos = Length ).
        */
        virtual void MoveEnd();

    protected:
        FileOpenMode::t     m_OpenMode;
        FileStreamMode::t   m_FileMode;
        std::string         m_Path;
        MemoryStream        m_BufferStream;
};

}
