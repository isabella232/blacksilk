#ifndef MIXER_H
#define MIXER_H

#include <QWidget>
#include <tuple>
#include "filterwidget.h"

namespace Ui {
class MixerWidget;
}

//! \brief GUI class for Split Tone
class MixerWidget : public FilterWidget {
        Q_OBJECT

    public:
        explicit MixerWidget( std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> filterAndPresets, QWidget* parent = 0 );
        ~MixerWidget();

        static const libfoundation::app::EFilter::t filterType;
        virtual bool onUpdateValues();
    signals:
        void signalMixer( float weight, float r1, float g1, float b1, float r2, float g2, float b2 );

    private slots:
        void on_spinBoxHR_valueChanged( int arg1 );
        void on_spinBoxWeight_valueChanged( int arg1 );
        void on_spinBoxHG_valueChanged( int arg1 );
        void on_spinBoxHB_valueChanged( int arg1 );
        void on_spinBoxSR_valueChanged( int arg1 );
        void on_spinBoxSG_valueChanged( int arg1 );
        void on_spinBoxSB_valueChanged( int arg1 );
        void on_sliderWeight_valueChanged( int value );
        void on_sliderHLuma_valueChanged( int value );
        void on_buttonLocked_clicked( bool checked );
        void on_sliderSLuma_valueChanged( int value );

        void slotSetShadows( float rf, float gf, float bf );
        void slotSetHighlights( float rf, float gf, float bf );
        void emitAll();

    private:
        void setWidgetBackgroundColor( QWidget* widget, const QColor& color );
        void setRGBtoHighlights();
        void setRGBtoShadows();
        std::tuple<float, float, float> mRGBShadows;
        Ui::MixerWidget* ui;
};

#endif // MIXER_H
