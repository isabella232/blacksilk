#ifndef GRAIN_H
#define GRAIN_H

#include <QWidget>
#include <QVector>
#include "filterwidget.h"

#include <libfoundation/app/application.hpp>

class CurveFrameEventFilter;

namespace Ui {
class GrainWidget;
}

//! \brief GUI class for artificial grain
class GrainWidget : public FilterWidget {
        Q_OBJECT

    public:
        explicit GrainWidget( std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> filterAndPresets, QWidget* parent = 0 );
        ~GrainWidget();

        static const libfoundation::app::EFilter::t filterType;
        virtual bool onUpdateValues();
    signals:
        /* send fulcrums and blur radius */
        void signalGrain( float, QVector<float> xs, QVector<float> ys );

    public slots:
        void slotSetHistogram( QVector<float> );

    private slots:
        void on_doubleSpinBoxBlur_valueChanged( double arg1 );
        void on_sliderBlur_valueChanged( int value );
        void emitAll();

    private:
        Ui::GrainWidget* ui;
        CurveFrameEventFilter* mEventFilter;
};

#endif // GRAIN_H
