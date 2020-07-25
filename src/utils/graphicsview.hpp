#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/image.hpp>
#include <libgraphics/bitmap.hpp>

#include <libcommon/noncopyable.hpp>

namespace blacksilk {

class GraphicsView : public libcommon::INonCopyable {
    public:
        GraphicsView();
        GraphicsView(
            const libgraphics::Rect32UI viewport,
            libgraphics::ImageLayer* layer,
            libgraphics::ImageLayer* originalImageLayer
        );
        virtual ~GraphicsView();

        ///
        libgraphics::ImageLayer*        layer() const;
        void setLayer( libgraphics::ImageLayer* layer );

        libgraphics::ImageLayer*        originalLayer() const;
        void setOriginalLayer( libgraphics::ImageLayer* layer );

        const libgraphics::Point32F&    center() const;
        libgraphics::Point32F&          center();
        void                            move(
            const libgraphics::Point32F& delta
        );

        const libgraphics::Rect32UI&    viewport() const;
        void resetViewport(
            const libgraphics::Rect32UI& newViewport
        );

        const double&   zoom() const;
        double&         zoom();
        void            setZoom( double delta );

        void            zoomIn( float delta = 1.0f );
        void            zoomOut( float delta = 1.0f );

        const bool&     showOriginalImage() const;
        bool&           showOriginalImage();

        bool            valid() const;

        ///
        enum EDimension : unsigned short {
            Horizontal,
            Vertical,
            Pixels
        };
        void            fitImage(
            EDimension dimension
        );
        void            resetCenter();
        void            resetZoom();

        ///
        void update();
        void render();
    protected:
        libgraphics::ImageLayer*    m_ImageLayer;
        libgraphics::ImageLayer*    m_OriginalImageLayer;

        libgraphics::Rect32UI       m_Viewport;
        libgraphics::Point32F       m_Center;
        double                      m_ZoomFactor;

        bool                        m_ShowOriginalImage;
};

}
