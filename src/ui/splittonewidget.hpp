#ifndef SPLITTONE_H
#define SPLITTONE_H

#include <QWidget>
#include <tuple>
#include "filterwidget.h"

namespace Ui {
class SplitToneWidget;
}

//! \brief GUI class for Split Tone
class SplitToneWidget : public FilterWidget {
        Q_OBJECT

    public:
        explicit SplitToneWidget( std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> filterAndPresets, QWidget* parent = 0 );
        ~SplitToneWidget();

        static const libfoundation::app::EFilter::t filterType;
        virtual bool onUpdateValues();
    signals:
        void signalSplit( float weight, float r1, float g1, float b1, float r2, float g2, float b2 );

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
        Ui::SplitToneWidget* ui;
};

#endif // SPLITTONE_H
