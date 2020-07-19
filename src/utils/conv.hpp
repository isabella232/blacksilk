#pragma once

#include <libgraphics/base.hpp>
#include <libgraphics/curve.hpp>
#include <libgraphics/bitmap.hpp>

#include <QPointF>

/**
    \file utils/conv.hpp
    \brief Contains helper functions, which convert libgraphics to qt
        types.
*/

namespace blacksilk {

/**
    \fn     toGraphicsPoint32F
    \brief  Converts a QPointF typed value to a corresponding
        libgraphics::Point32F value.
    \param originalValue    The original value to be converted.
    \return A valid libgraphics::Point32F instance
*/
static inline libgraphics::Point32F toGraphicsPoint32F( const QPointF& originalValue ) {
    return libgraphics::Point32F(
               originalValue.x(),
               originalValue.y()
           );
}

/**
    \fn     toQPointF
    \brief Converts a libgraphics::Point typed value to a corresponding
        QPointF typed value.
    \param originalValue    The original value to be converted.
    \return A valid QPointF instance.
*/
static inline QPointF toQPointF( const libgraphics::Point32F& originalValue ) {
    return QPointF(
               originalValue.x,
               originalValue.y
           );
}

/**
    \fn toQPointF
    \brief Constructs a new QPointF value from the specified x and y
        components.
    \param x    x component
    \param y    y component
    \return A valid QPointF instance.
*/
static inline QPointF toQPointF( float x, float y ) {
    return QPointF(
               x,
               y
           );
}

}
