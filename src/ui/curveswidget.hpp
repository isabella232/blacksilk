#ifndef CURVES_H
#define CURVES_H

#include <QWidget>
#include <QButtonGroup>

#include "filterwidget.h"
#include "utils/colorspaces.hpp"

using blacksilk::graphics::ColorRGB;

class CurveFrameEventFilter;

namespace Ui {
class CurvesWidget;
}

//! \brief GUI class for curves and histogram
class CurvesWidget : public FilterWidget {
        Q_OBJECT

    public:
        explicit CurvesWidget( std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> filterAndPresets, QWidget* parent = 0 );
        ~CurvesWidget();

        static const libfoundation::app::EFilter::t filterType;
        virtual bool onUpdateValues();
    public slots:
        void slotSetHistogram( QVector<ColorRGB<float> > );
        void slotSetWeights( float weight, float r1, float g1, float b1, float r2, float g2, float b2 );

    signals:
        /* send fulcrums */
        void signalCurves( QVector<float>, QVector<float> );
        void signalOutHistogram( QVector<float> );

    private slots:
        void emitAll();

    private:
        Ui::CurvesWidget* ui;
        CurveFrameEventFilter* mEventFilter;

        /* weights from mixer */
        ColorRGB<float> mWeightsHighlights;
        ColorRGB<float> mWeightsShadows;
        float mWeight;
        QVector< ColorRGB<float> > mHistogram;
};

#endif // CURVES_H
