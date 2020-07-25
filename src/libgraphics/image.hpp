#pragma once

#include <vector>
#include <iterator>

#include <libgraphics/base.hpp>
#include <libgraphics/bitmap.hpp>
#include <libgraphics/fxapi.hpp>
#include <libgraphics/io/pipeline.hpp>
#include <libcommon/noncopyable.hpp>

namespace libgraphics {

/// forward
class Image;
class ImageLayer;
class ImageLayerMask;
class ImageLayerMaskMode;
class ImageLayerGroup;

class ImageMetaInfo;
class ImageMetaInfoDirectory;
class ImageMetaInfoTag;

/// makers and helpers
ImageLayer* makeImageLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::fxapi::EPixelFormat::t format,
    size_t width,
    size_t height
);
ImageLayer* makeImageLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::fxapi::EPixelFormat::t format,
    size_t width,
    size_t height,
    void* data
);
ImageLayer* makeImageLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::Bitmap* bitmap
);
ImageLayer* makeImageLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I area
);
ImageLayer* makeImageLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    const libgraphics::BitmapInfo& info
);
ImageLayer* makeImageLayer(
    fxapi::ApiBackendDevice* device,
    const std::string& name
);
ImageLayer* makeImageLayer(
    fxapi::ApiBackendDevice* _device,
    ImageLayer* _templateInstance,
    bool _copyData = false
);

ImageLayer* emplaceImageLayer(
    Image* destinationImage,
    fxapi::ApiBackendDevice* device,
    const std::string& name
);
ImageLayer* emplaceImageLayer(
    Image* destinationImage,
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::fxapi::EPixelFormat::t format,
    size_t width,
    size_t height
);
ImageLayer* emplaceImageLayer(
    Image* destinationImage,
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::fxapi::EPixelFormat::t format,
    size_t width,
    size_t height,
    void* data
);
ImageLayer* emplaceImageLayer(
    Image* destinationImage,
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    const libgraphics::BitmapInfo& info
);
ImageLayer* emplaceImageLayer(
    Image* destinationImage,
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::Bitmap* bitmap
);
ImageLayer* emplaceImageLayer(
    Image* destinationImage,
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I area
);

Image* makeImage();
Image* makeImage(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::fxapi::EPixelFormat::t format,
    size_t width,
    size_t height
);
Image* makeImage(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::fxapi::EPixelFormat::t format,
    size_t width,
    size_t height,
    void* data
);
Image* makeImage(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::Bitmap* bitmap
);
Image* makeImage(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    libgraphics::Bitmap* bitmap,
    libgraphics::Rect32I area
);
Image* makeImage(
    fxapi::ApiBackendDevice* device,
    const std::string& name,
    const libgraphics::BitmapInfo& info
);

/**
  classes
 */
/// info tag
class ImageMetaInfoTag {
    public:
        ImageMetaInfoTag(
            const std::string& _name,
            const std::string& _data
        ) : m_Name( _name ), m_Data( _data ), m_Length( m_Data.size() ) {}
        ImageMetaInfoTag(
            const std::string& _name,
            char* data,
            size_t length
        ) : m_Name( _name ), m_Data( data, length ), m_Length( length ) {
            assert( data );
            assert( length > 0 );
        }

        bool operator == ( const ImageMetaInfoTag& rhs ) const;
        bool operator != ( const ImageMetaInfoTag& rhs ) const;

        const std::string& name() const;
        const std::string& data() const;
        size_t length() const;

        void assign( const std::string& _name, const std::string& _data );
        void assign( const std::string& _name, char* _data, size_t _length );
        void reset();

    protected:
        std::string     m_Name;
        std::string     m_Data;
        size_t          m_Length;
};

/// represents a group of meta
/// tags.
class ImageMetaInfoDirectory {
    public:
        typedef std::vector<ImageMetaInfoTag>   VectorType;
        typedef VectorType::iterator            Iterator;
        typedef VectorType::const_iterator      ConstIterator;

        ImageMetaInfoDirectory() {}
        explicit ImageMetaInfoDirectory( const std::string& _name ) :
            m_Name( _name ) {}

        void add( const ImageMetaInfoTag& tag );
        void add( const std::string& _name, const std::string& _data );
        void add( const std::string& _name, char* _data, size_t _length );

        bool remove( Iterator it );
        bool remove( size_t index );
        bool remove( const ImageMetaInfoTag& tag );
        bool removeByName( const std::string& _name );

        void reset();

        const std::string& name() const;
        size_t size() const;
        bool empty() const;

        void assign( Iterator first, Iterator last );
        void assign( const ImageMetaInfoDirectory& other );
        ImageMetaInfoDirectory& operator = ( const ImageMetaInfoDirectory& other );

        bool contains( const ImageMetaInfoTag& tag ) const;
        bool containsWithName( const std::string& _name ) const;

        ConstIterator findByName( const std::string& _name ) const;

        Iterator begin();
        ConstIterator begin() const;
        ConstIterator constBegin() const;

        Iterator end();
        ConstIterator end() const;
        ConstIterator constEnd() const;

        inline bool operator == ( const ImageMetaInfoDirectory& rhs ) const {
            return ( m_Name == rhs.m_Name ) && ( m_Tags == rhs.m_Tags );
        }
        inline bool operator != ( const ImageMetaInfoDirectory& rhs ) const {
            return ( m_Name != rhs.m_Name ) || ( m_Tags != rhs.m_Tags );
        }
    protected:
        std::string m_Name;
        std::vector<ImageMetaInfoTag>   m_Tags;
};

/// contains the relevant meta information
class ImageMetaInfo {
    public:
        ImageMetaInfo();
        ~ImageMetaInfo() {}

        /// directory management
        ImageMetaInfoDirectory& addDirectory(
            const std::string& _name
        );
        ImageMetaInfoDirectory& emplaceDirectory( const std::string& _name );
        ImageMetaInfoDirectory& emplaceDirectory( ImageMetaInfoDirectory::Iterator first, ImageMetaInfoDirectory::Iterator last );

        bool removeDirectory(
            const ImageMetaInfoDirectory& dir
        );
        bool removeDirectoryByName(
            const std::string& _name
        );
        bool removeDirectoryByIndex(
            size_t index
        );

        ImageMetaInfoDirectory& directoryByName( const std::string& _name );
        const ImageMetaInfoDirectory& directoryByName( const std::string& _name ) const;
        ImageMetaInfoDirectory& directoryByIndex( size_t index );
        const ImageMetaInfoDirectory& directoryByIndex( size_t index ) const;

        size_t countDirectories() const;

        bool containsDirectory( const std::string& _name ) const;
        bool containsDirectory( const ImageMetaInfoDirectory& _dir ) const;

        void clearDirectories();

        /// general state
        void reset();
        bool empty() const;
    protected:
        std::vector<ImageMetaInfoDirectory> m_Directories;
};

/// high-level image layer mask
/// mode
class ImageLayerMaskMode {
    public:
        virtual ~ImageLayerMaskMode() {}

        virtual bool apply(
            ImageLayer* destinationLayer,
            ImageLayerMask* mask
        ) = 0;
};

/// high-level image layer mask
class ImageLayerMask {
    public:
        struct Private;

        ImageLayerMask( libgraphics::fxapi::ApiBackendDevice* backendDevice );
        ImageLayerMask( const ImageLayerMask& rhs );
        ImageLayerMask( ImageLayerMask&& rhs );
        virtual ~ImageLayerMask() {}

        ImageLayerMask( libgraphics::fxapi::ApiBackendDevice* backendDevice, fxapi::EPixelFormat::t format, size_t width, size_t height );
        ImageLayerMask( libgraphics::fxapi::ApiBackendDevice* backendDevice, fxapi::EPixelFormat::t format, size_t width, size_t height, void* data );
        ImageLayerMask( libgraphics::fxapi::ApiBackendDevice* backendDevice, const libgraphics::BitmapInfo& info );
        ImageLayerMask( libgraphics::fxapi::ApiBackendDevice* backendDevice, libgraphics::Bitmap* bitmap, size_t channelIndex );
        ImageLayerMask( libgraphics::fxapi::ApiBackendDevice* backendDevice, libgraphics::Bitmap* bitmap, size_t channelIndex, libgraphics::Rect32I area );

        ImageLayerMask& operator = ( const ImageLayerMask& rhs );

        bool operator == ( const ImageLayerMask& rhs ) const;
        bool operator != ( const ImageLayerMask& rhs ) const;

        /// properties
        int width() const;
        int height() const;
        bool empty() const;

        fxapi::EPixelFormat::t format() const;

        size_t byteSize() const;
        size_t pixelSize() const;
        size_t channelSize() const; /// channelSize == pixelSize

        /// resetting the internal state
        void reset();
        bool reset( fxapi::EPixelFormat::t format, int width, int height );
        bool reset( fxapi::EPixelFormat::t format, int width, int height, void* data );
        bool reset( const libgraphics::BitmapInfo& info );
        bool reset( libgraphics::Bitmap* bitmap, size_t channelIndex );
        bool reset( libgraphics::Bitmap* bitmap, size_t channelIndex, libgraphics::Rect32I area );
        bool reset( const ImageLayerMask& other );

        /// retrieving and updating data
        bool retrieve(
            void* buffer
        );
        bool retrieve(
            void* buffer,
            libgraphics::Rect32I rect
        );
        bool retrieve(
            libgraphics::Bitmap* bitmap,
            size_t channelIndex
        );
        bool retrieve(
            libgraphics::Bitmap* bitmap,
            size_t channelIndex,
            libgraphics::Rect32I rect
        );

        bool copy( fxapi::ApiImageObject* source,
                   libgraphics::Rect32I sourceRect,
                   int destX,
                   int destY
                 );
        bool copy( ImageLayerMask* source,
                   libgraphics::Rect32I sourceRect,
                   int destX,
                   int destY
                 );
        bool copy(
            libgraphics::Bitmap* source,
            size_t channelIndex,
            libgraphics::Rect32I sourceRect,
            int destX,
            int destY
        );
        bool copy(
            void* data,
            libgraphics::Rect32I sourceRect,
            int destX,
            int destY
        );
        bool copy(
            libgraphics::Bitmap* bitmap,
            size_t channelIndex
        );
        bool copy(
            ImageLayerMask* layer
        );

        /// automatic backend mirroring
        bool containsDataForBackend(
            int backendId
        ) const;
        bool containsDataForDevice(
            libgraphics::fxapi::ApiBackendDevice* device
        ) const;
        void updateDataForBackend(
            libgraphics::fxapi::ApiBackendDevice* device,
            int backendId
        );

        /// internals
        fxapi::ApiImageObject* internalImageForBackend(
            int backendId
        ) const;
        fxapi::ApiImageObject* internalImageForDevice(
            fxapi::ApiBackendDevice* device
        ) const;
        fxapi::ApiBackendDevice* internalDeviceForBackend(
            int backendId
        ) const;
    protected:
        std::shared_ptr<Private>   d;
};

/// high-level image layer class
/// management.
class ImageLayerGroup {
    public:
        struct Private {
            std::vector<std::shared_ptr<ImageLayer> >  layers;
        };

        typedef std::vector<std::shared_ptr<ImageLayer> > VectorType;
        typedef VectorType::iterator Iterator;
        typedef VectorType::const_iterator ConstIterator;

        ImageLayerGroup();
        ImageLayerGroup( const ImageLayerGroup& rhs );
        ImageLayerGroup& operator = ( const ImageLayerGroup& rhs );
        virtual ~ImageLayerGroup() {}

        /// appends a new layer to the top of the
        /// current layer group.
        void append(
            ImageLayer* layer
        );
        void append(
            std::shared_ptr<ImageLayer> layer
        );


        /// removes the specified layer from the
        /// internal image.
        bool remove(
            ImageLayer* layer
        );
        bool removeByName(
            const std::string& name
        );
        bool removeByIndex(
            size_t index
        );
        void reset();

        /// properties
        bool empty() const;
        size_t size() const;

        ImageLayerGroup layersWithName( const std::string& name ) const;
        ImageLayerGroup layersWithSize( int width, int height ) const;
        ImageLayerGroup layersWithFormat( fxapi::EPixelFormat::t format ) const;
        ImageLayerGroup layersWithConfig( fxapi::EPixelFormat::t format, int width, int height ) const;
        ImageLayerGroup layersWithBackend( int backend ) const;

        bool contains( ImageLayer* layer ) const;
        bool contains( const std::string& name ) const;

        bool updateImageLayerDataForBackend(
            libgraphics::fxapi::ApiBackendDevice* device,
            int backendId
        );

        /// vector properties
        Iterator   begin();
        ConstIterator begin() const;
        ConstIterator constBegin() const;

        Iterator end();
        ConstIterator end() const;
        ConstIterator constEnd() const;

        std::shared_ptr<ImageLayer>    front() const;
        std::shared_ptr<ImageLayer>    back() const;

        std::shared_ptr<ImageLayer>    at( size_t index ) const;
    protected:
        std::shared_ptr<Private>   d;
};

/// high-level image class
class Image {
    public:
        struct Private;

        static const size_t npos = -1;

        /// constr. / destr.
        Image();
        Image( fxapi::ApiBackendDevice* device, fxapi::EPixelFormat::t format, size_t width, size_t height );
        Image( fxapi::ApiBackendDevice* device, fxapi::EPixelFormat::t format, size_t width, size_t height, void* data );

        /// assignments
        Image( const Image& rhs );
        Image( Image&& rhs );
        Image& operator = ( const Image& rhs );

        virtual ~Image();

        /// meta-data management
        ImageMetaInfo*  metaInfo();
        const ImageMetaInfo*  metaInfo() const;

        bool readMetaInfoFromFile(
            const std::string& filePath
        );
        bool readMetaInfoFromData(
            void* data,
            size_t length
        );
        bool writeMetaInfoToFile(
            const std::string& filePath
        );
        bool writeMetaInfoToData(
            void* data,
            size_t length
        );

        /// layer management

        /// appending a new layer to the top of the current
        /// image.
        ImageLayer* createAndAppendLayer(
            fxapi::ApiBackendDevice* device,
            const std::string& name
        );
        ImageLayer* createAndAppendLayer(
            fxapi::ApiBackendDevice* device,
            const std::string& name,
            size_t width,
            size_t height
        );
        ImageLayer* createAndAppendLayer(
            fxapi::ApiBackendDevice* device,
            const std::string& name,
            size_t width,
            size_t height,
            void* data
        );

        /// loading image layers from file
        ImageLayer* loadImageLayerFromFile(
            fxapi::ApiBackendDevice* device,
            const std::string& path,
            io::Pipeline* ioPipeline
        );
        ImageLayer* loadImageLayerFromData(
            fxapi::ApiBackendDevice* device,
            void* data,
            size_t length,
            io::Pipeline* ioPipeline
        );

        /// appends a new layer to the top of the
        /// current layer group.
        bool appendLayer(
            ImageLayer* layer
        );
        bool appendLayerWithCompatibleDevice(
            ImageLayer* layer
        );

        /// removes the specified layer from the
        /// internal image.
        bool removeLayer(
            ImageLayer* layer
        );
        bool removeLayerByName(
            const std::string& name
        );
        bool removeLayerByIndex(
            size_t index
        );
        void clearLayers();

        /// mirror to another backend
        bool updateImageLayerDataForBackend(
            libgraphics::fxapi::ApiBackendDevice* device,
            int backendId
        );

        /// properties
        size_t  layerCount() const;
        ImageLayer* layerByName( const std::string& name ) const;
        ImageLayer* layerByIndex( size_t index );
        bool empty() const;

        size_t indexForLayer( ImageLayer* layer );

        bool moveLayerUp( ImageLayer* layer );
        bool moveLayerDown( ImageLayer* layer );
        bool moveLayer( ImageLayer* layer, int steps );

        bool switchLayers( ImageLayer* first, ImageLayer* second );
        bool switchLayers( size_t firstIndex, size_t secondIndex );

        ImageLayerGroup layers() const;
        ImageLayerGroup layersWithName( const std::string& name ) const;
        ImageLayerGroup layersWithSize( int width, int height ) const;
        ImageLayerGroup layersWithFormat( fxapi::EPixelFormat::t format ) const;
        ImageLayerGroup layersWithConfig( fxapi::EPixelFormat::t format, int width, int height ) const;
        ImageLayerGroup layersWithBackend( int backend ) const;

        bool containsImageLayer( ImageLayer* layer ) const;
        bool containsImageLayerWithName( const std::string& name ) const;

        ImageLayer* topLayer() const;
        ImageLayer* bottomLayer() const;

        ImageLayer* cloneTopLayer();
        ImageLayer* cloneBottomLayer();

        /// state management
        void reset();
        bool reset( fxapi::ApiBackendDevice* device, fxapi::EPixelFormat::t format, size_t width, size_t height );
        bool reset( fxapi::ApiBackendDevice* device, fxapi::EPixelFormat::t format, size_t width, size_t height, void* data );
        bool reset( fxapi::ApiBackendDevice* device, libgraphics::Bitmap* source );
        bool reset( fxapi::ApiBackendDevice* device, libgraphics::Bitmap* source, libgraphics::Rect32I sourceArea );
        bool reset( fxapi::ApiBackendDevice* device, const libgraphics::BitmapInfo& info );

        /// serialization
        size_t readFromFile(
            libgraphics::fxapi::ApiBackendDevice* defaultDevice,
            const std::string& path
        );
        size_t readFromData(
            libgraphics::fxapi::ApiBackendDevice* defaultDevice,
            void* data,
            size_t size
        );

        size_t writeToFile(
            const std::string& path
        );
        size_t writeToData(
            void* data,
            size_t size
        );

        /// statistics
        size_t queryMemoryConsumptionForBackend( int backendId ) const;
        size_t queryMemoryConsumption() const;

        /// properties
        size_t      width() const;
        size_t      height() const;
        fxapi::EPixelFormat::t  format() const;
    protected:
        std::shared_ptr<Private>   d;
};

/// high-level image layer class implementation
class ImageLayer {
    public:
        struct Private;

        explicit ImageLayer( libgraphics::fxapi::ApiBackendDevice* backendDevice, std::string _name = "" );
        virtual ~ImageLayer();

        ImageLayer( const ImageLayer& rhs );
        ImageLayer( ImageLayer&& rhs );
        ImageLayer& operator = ( const ImageLayer& rhs );

        bool operator == ( const ImageLayer& rhs ) const;
        bool operator != ( const ImageLayer& rhs ) const;

        /** internal properties **/
        const std::string&  name() const;
        void setName( const std::string& _name );

        /** general imaging methods **/

        /// reset
        bool reset();
        bool reset(
            libgraphics::Bitmap* bitmap
        );
        bool reset(
            libgraphics::Bitmap* bitmap,
            libgraphics::Rect32I rect
        );
        bool reset(
            const libgraphics::BitmapInfo& info
        );
        bool reset( fxapi::EPixelFormat::t format,
                    int width,
                    int height,
                    void* data
                  );
        bool reset( fxapi::EPixelFormat::t format,
                    int width,
                    int height
                  );

        /// fill
        bool fill(
            libgraphics::Rect32I area,
            libgraphics::formats::RGB8::t value
        );
        bool fill(
            libgraphics::Rect32I area,
            libgraphics::formats::RGBA8::t value
        );
        bool fill(
            libgraphics::Rect32I area,
            libgraphics::formats::RGB16::t value
        );
        bool fill(
            libgraphics::Rect32I area,
            libgraphics::formats::RGBA16::t value
        );
        bool fill(
            libgraphics::Rect32I area,
            libgraphics::formats::RGB32::t value
        );
        bool fill(
            libgraphics::Rect32I area,
            libgraphics::formats::RGBA32::t value
        );
        bool fill(
            libgraphics::Rect32I area,
            libgraphics::formats::RGB32F::t value
        );
        bool fill(
            libgraphics::Rect32I area,
            libgraphics::formats::RGBA32F::t value
        );
        bool fill(
            libgraphics::Rect32I area,
            libgraphics::formats::Mono8::t value
        );
        bool fill(
            libgraphics::Rect32I area,
            libgraphics::formats::Mono16::t value
        );
        bool fill(
            libgraphics::Rect32I area,
            libgraphics::formats::Mono32::t value
        );

        bool fillChannel(
            libgraphics::Rect32I area,
            size_t channelIndex,
            unsigned char value
        );
        bool fillChannel(
            libgraphics::Rect32I area,
            size_t channelIndex,
            unsigned short value
        );
        bool fillChannel(
            libgraphics::Rect32I area,
            size_t channelIndex,
            unsigned int value
        );
        bool fillChannel(
            libgraphics::Rect32I area,
            size_t channelIndex,
            float value
        );

        /// downloading, retrieving data
        bool retrieve(
            void* buffer
        );
        bool retrieve(
            void* buffer,
            libgraphics::Rect32I rect
        );
        bool retrieve(
            libgraphics::Bitmap* bitmap
        );
        bool retrieve(
            libgraphics::Bitmap* bitmap,
            libgraphics::Rect32I rect
        );

        bool retrieveChannel(
            size_t channelIndex,
            void* buffer
        );
        bool retrieveChannel(
            size_t channelIndex,
            void* buffer,
            libgraphics::Rect32I rect
        );
        bool retrieveChannel(
            size_t channelIndex,
            libgraphics::Bitmap* bitmap
        );
        bool retrieveChannel(
            size_t channelIndex,
            libgraphics::Bitmap* bitmap,
            libgraphics::Rect32I rect
        );

        /// copy
        bool copy( fxapi::ApiImageObject* source,
                   libgraphics::Rect32I sourceRect,
                   int destX,
                   int destY
                 );
        bool copy( ImageLayer* source,
                   libgraphics::Rect32I sourceRect,
                   int destX,
                   int destY
                 );
        bool copy( libgraphics::Bitmap* bitmap,
                   libgraphics::Rect32I sourceRect,
                   int destX,
                   int destY
                 );
        bool copy(
            void* data,
            libgraphics::Rect32I sourceRect,
            int destX,
            int destY
        );
        bool copy(
            libgraphics::Bitmap* bitmap
        );
        bool copy(
            ImageLayer* layer
        );

        bool copyChannel(
            size_t channelIndex,
            fxapi::ApiImageObject* source,
            libgraphics::Rect32I sourceRect,
            size_t destX,
            size_t destY
        );
        bool copyChannel(
            size_t channelIndex,
            ImageLayer* source,
            libgraphics::Rect32I sourceRect,
            size_t destX,
            size_t destY
        );
        bool copyChannel(
            size_t channelIndex,
            libgraphics::Bitmap* bitmap,
            libgraphics::Rect32I sourceRect,
            size_t destX,
            size_t destY
        );
        bool copyChannel(
            size_t channelIndex,
            void* data,
            libgraphics::Rect32I sourceRect,
            size_t destX,
            size_t destY
        );
        bool copyChannel(
            size_t channelIndex,
            libgraphics::Bitmap* bitmap
        );
        bool copyChannel(
            size_t channelIndex,
            ImageLayer* layer
        );
        bool copyChannel( size_t sourceChannelIndex,
                          size_t destChannelIndex,
                          fxapi::ApiImageObject* source,
                          libgraphics::Rect32I sourceRect,
                          int destX,
                          int destY
                        );
        bool copyChannel( size_t sourceChannelIndex,
                          size_t destChannelIndex,
                          ImageLayer* source,
                          libgraphics::Rect32I sourceRect,
                          int destX,
                          int destY
                        );
        bool copyChannel( size_t sourceChannelIndex,
                          size_t destChannelIndex,
                          libgraphics::Bitmap* bitmap,
                          libgraphics::Rect32I sourceRect,
                          int destX,
                          int destY
                        );
        bool copyChannel( size_t sourceChannelIndex,
                          size_t destChannelIndex,
                          void* data,
                          libgraphics::Rect32I sourceRect,
                          int destX,
                          int destY
                        );
        bool copyChannel(
            size_t sourceChannelIndex,
            size_t destChannelIndex,
            libgraphics::Bitmap* bitmap
        );
        bool copyChannel(
            size_t sourceChannelIndex,
            size_t destChannelIndex,
            ImageLayer* layer
        );

        /// constructs sub textures of the current
        /// one.
        ImageLayer* duplicate();
        ImageLayer* duplicateArea(
            libgraphics::Rect32I sourceRect
        );
        ImageLayer* duplicateChannel( size_t channelIndex );
        ImageLayer* duplicateChannelArea(
            size_t channelIndex,
            libgraphics::Rect32I sourceRect
        );

        /// mask-management
        bool hasMask() const;
        bool hasMaskMode() const;

        ImageLayerMask* mask() const;
        ImageLayerMaskMode* maskMode() const;

        void setMask( ImageLayerMask* mask );
        void setMaskMode( ImageLayerMaskMode* mode );

        bool assignMask( ImageLayerMask* mask, ImageLayerMaskMode* mode );
        bool applyMask();

        /// alpha channels
        bool hasAlphaChannel() const;
        bool addAlphaChannel();
        bool removeAlphaChannel();

        ImageLayer* duplicateAlphaChannel();

        /// data management
        void synchronize();

        /// properties
        libgraphics::Rect32I size() const;
        int  width() const;
        int height() const;
        fxapi::EPixelFormat::t format() const;
        bool empty() const;

        size_t byteSize() const;
        size_t pixelSize() const;
        size_t channelSize() const; /// channelSize == pixelSize

        /// internals, automatic backend mirroring
        bool containsDataForBackend(
            int backendId
        ) const;
        bool containsDataForDevice(
            libgraphics::fxapi::ApiBackendDevice* device
        ) const;
        void updateDataForBackend(
            libgraphics::fxapi::ApiBackendDevice* device,
            int backendId
        );
        fxapi::ApiImageObject* internalImageForBackend(
            int backendId
        ) const;
        fxapi::ApiImageObject* internalImageForDevice(
            fxapi::ApiBackendDevice* device
        ) const;
        fxapi::ApiBackendDevice* internalDeviceForBackend(
            int backendId
        ) const;
        bool updateInternalState( int backendId );
        bool deleteDataForBackend( int backendId );
        bool deleteDataForDevice( libgraphics::fxapi::ApiBackendDevice* device );
    protected:
        std::shared_ptr<Private>   d;
    private:
        ImageLayer();
};

}
