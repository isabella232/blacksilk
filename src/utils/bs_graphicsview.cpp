#include <QDebug>
#include <utils/graphicsview.hpp>
#include <libgraphics/utils/gl.hpp>
#include <libgraphics/backend/gl/gl_backenddevice.hpp>
#include <libgraphics/backend/gl/gl_imageobject.hpp>

#include <gl/glew.h>

#ifdef _WIN32
#   include <Windows.h>
#   include <gl/GL.h>
#endif


namespace blacksilk {

GraphicsView::GraphicsView() :
    m_ImageLayer( nullptr ),
    m_OriginalImageLayer( nullptr ),
    m_ZoomFactor( 1.0 ),
    m_ShowOriginalImage( false ) {}

GraphicsView::GraphicsView(
    const libgraphics::Rect32UI viewport,
    libgraphics::ImageLayer* layer,
    libgraphics::ImageLayer* originalImageLayer ) :
    m_ImageLayer( layer ),
    m_OriginalImageLayer( originalImageLayer ),
    m_Viewport( viewport ),
    m_ZoomFactor( 1.0 ),
    m_ShowOriginalImage( false ) {

    assert( layer != nullptr );
    assert( originalImageLayer != nullptr );

    this->resetCenter();
}

GraphicsView::~GraphicsView() {
    this->m_OriginalImageLayer  = nullptr;
    this->m_ImageLayer          = nullptr;
}

///
libgraphics::ImageLayer*        GraphicsView::layer() const {
    return this->m_ImageLayer;
}

void GraphicsView::setLayer( libgraphics::ImageLayer* layer ) {
    assert( layer );
    this->m_ImageLayer = layer;
}

libgraphics::ImageLayer*        GraphicsView::originalLayer() const {
    return this->m_OriginalImageLayer;
}

void GraphicsView::setOriginalLayer( libgraphics::ImageLayer* layer ) {
    assert( layer );
    this->m_OriginalImageLayer = layer;
}

const libgraphics::Point32F&    GraphicsView::center() const {
    return this->m_Center;
}

libgraphics::Point32F&          GraphicsView::center() {
    return this->m_Center;
}

void                            GraphicsView::move(
    const libgraphics::Point32F& delta
) {
    this->m_Center.x += delta.x;
    this->m_Center.y += delta.y;
}

const libgraphics::Rect32UI&    GraphicsView::viewport() const {
    return this->m_Viewport;
}

void GraphicsView::resetViewport(
    const libgraphics::Rect32UI& newViewport
) {
    this->m_Viewport = newViewport;
}

const double&   GraphicsView::zoom() const {
    return this->m_ZoomFactor;
}

double&         GraphicsView::zoom() {
    return this->m_ZoomFactor;
}

void            GraphicsView::setZoom( double delta ) {
    if( !valid() ) {
        return;
    }

    const float ratioWidth      = ( float )this->m_Viewport.width / ( float )this->m_ImageLayer->width();
    const float ratioHeight     = ( float )this->m_Viewport.height / ( float )this->m_ImageLayer->height();

    const float lower           = 0.5f * std::min<float>( ratioWidth, ratioHeight );

    if( ( ( delta > 0.0f ) && ( this->m_ZoomFactor <= 20.0f ) ) || ( this->m_ZoomFactor > lower ) ) {
        this->m_ZoomFactor = this->m_ZoomFactor + ( delta / 25.0f );
    }
}

void            GraphicsView::zoomIn( float delta ) {
    this->setZoom( 1.0f * delta );
}

void            GraphicsView::zoomOut( float delta ) {
    this->setZoom( -1.0f * delta );
}

const bool&     GraphicsView::showOriginalImage() const {
    return this->m_ShowOriginalImage;
}

bool&           GraphicsView::showOriginalImage() {
    return this->m_ShowOriginalImage;
}

bool            GraphicsView::valid() const {
    return ( this->m_ImageLayer != nullptr ) && ( this->m_OriginalImageLayer != nullptr );
}

///
void            GraphicsView::fitImage(
    GraphicsView::EDimension dimension
) {
    if( !valid() ) {
        return;
    }

    const float ratioWidth    = ( float )this->m_Viewport.width / ( float )this->m_ImageLayer->width();
    const float ratioHeight   = ( float )this->m_Viewport.height / ( float )this->m_ImageLayer->height();

    float ratio( 1.0f );

    switch( dimension ) {
        case GraphicsView::EDimension::Pixels:
            ratio = 1.0f;
            break;

        case GraphicsView::EDimension::Horizontal:
            ratio = 0.99 * std::min( ratioWidth, ratioHeight );
            break;

        case GraphicsView::EDimension::Vertical:
            ratio = 0.99 * std::max( ratioWidth, ratioHeight );
            break;

        default:
            assert( false );
    }

    this->m_ZoomFactor = ratio;
    this->resetCenter();
}

void            GraphicsView::resetCenter() {
    this->m_Center.x = this->m_Viewport.width / 2;
    this->m_Center.y = this->m_Viewport.height / 2;
}

void            GraphicsView::resetZoom() {
    this->m_ZoomFactor = 1.0f;
}

///
void GraphicsView::update() {
    if( !valid() ) {
        return;
    }

    auto glctx = libgraphics::backend::gl::globalCtx();
    assert( glctx != nullptr );

    if( glctx == nullptr ) {
        qDebug() << "Failed to update view: invalid opengl context.";
        return;
    }

    const float relativeWidth = this->m_Viewport.width / this->m_ZoomFactor;
    const float relativeHeight = this->m_Viewport.height / this->m_ZoomFactor;

    glViewport( 0.0f, 0.0f, this->m_Viewport.width, this->m_Viewport.height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(
        -( relativeWidth / 2.0f ),
        ( relativeWidth / 2.0f ),
        ( relativeHeight / 2.0f ),
        -( relativeHeight / 2.0f ),
        -1.0f,
        1.0f
    );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glTranslatef( -( this->m_ImageLayer->width() / 2.0f ), -( this->m_ImageLayer->height() / 2.0f ), 0.0f );
    glTranslatef( ( m_Center.x - ( this->m_Viewport.width / 2.0f ) ), ( m_Center.y - ( this->m_Viewport.height / 2.0f ) ), 0.0f );
}

void GraphicsView::render() {
    if( !valid() ) {
        return;
    }

    auto glctx = libgraphics::backend::gl::globalCtx();
    assert( glctx != nullptr );

    if( glctx == nullptr ) {
        qDebug() << "Failed to update view: invalid opengl context.";
        return;
    }

    libgraphics::ImageLayer* imageLayerToShow = ( this->m_ShowOriginalImage ) ? this->m_OriginalImageLayer : this->m_ImageLayer;
    assert( imageLayerToShow != nullptr );

    if( imageLayerToShow == nullptr ) {
        qDebug() << "Failed to render preview: GraphicsView contains no associated image layer.";
        return;
    }

    libgraphics::backend::gl::ImageObject* glBaseImage = ( libgraphics::backend::gl::ImageObject* )imageLayerToShow->internalImageForBackend( FXAPI_BACKEND_OPENGL );
    assert( glBaseImage != nullptr );

    if( glBaseImage == nullptr ) {
        qDebug() << "Failed to render preview: Associated ImageLayer contains no valid data for GL-backend.";
        return;
    }

    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT );


    const auto horizontalTileCount = glBaseImage->countHorizontalTiles();
    const auto verticalTileCount   = glBaseImage->countVerticalTiles();

    for( size_t y = 0; verticalTileCount > y; ++y ) {
        for( size_t x = 0; horizontalTileCount > x; ++x ) {
            libgraphics::backend::gl::Texture* tileTextureData = glBaseImage->tileTexture( x, y );
            libgraphics::Rect32I               tileTextureArea = glBaseImage->logicalTileArea( x, y );

            assert( tileTextureData != nullptr );
            assert( ( tileTextureData->width() * tileTextureData->height() != 0 ) );
            assert( ( tileTextureArea.width * tileTextureArea.height ) != 0 );


            libgraphics::utils::GLTexture* texObject = tileTextureData->internalTexture();
            assert( texObject != nullptr );
            ( void ) libgraphics::utils::GL::bindTexture(
                glctx,
                texObject
            );

            libgraphics::Rect32I            realTextureArea = glBaseImage->tileArea( x, y );
            const float ratioWidth    = ( float )tileTextureArea.width / ( float )realTextureArea.width;
            const float ratioHeight   = ( float )tileTextureArea.height / ( float )realTextureArea.height;

            glBegin( GL_QUADS );

            glTexCoord2f( 0.0f, 0.0f );
            glVertex2f( tileTextureArea.x, tileTextureArea.y );

            glTexCoord2f( 0.0f, ratioHeight );
            glVertex2f( tileTextureArea.x, tileTextureArea.y + tileTextureArea.height );

            glTexCoord2f( ratioWidth, ratioHeight );
            glVertex2f( tileTextureArea.x + tileTextureArea.width, tileTextureArea.y + tileTextureArea.height );

            glTexCoord2f( ratioWidth, 0.0f );
            glVertex2f( tileTextureArea.x + tileTextureArea.width, tileTextureArea.y );

            glEnd();
        }
    }
}


}
