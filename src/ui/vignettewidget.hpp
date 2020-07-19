#ifndef VIGNETTE_H
#define VIGNETTE_H

#include <QWidget>
#include "filterwidget.h"

namespace Ui {
class VignetteWidget;
}

//! \brief GUI class for Vignette
class VignetteWidget : public FilterWidget {
        Q_OBJECT

    public:
        explicit VignetteWidget( std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> filterAndPresets, QWidget* parent = 0 );
        ~VignetteWidget();

        static const libfoundation::app::EFilter::t filterType;
        virtual bool onUpdateValues();
    signals:
        void signalVignette( float x, float y, float radius, float strength, bool extraBlur );

    private slots:
        void on_sliderX_valueChanged( int value );
        void on_sliderY_valueChanged( int value );
        void on_sliderRadius_valueChanged( int value );
        void on_sliderStrength_valueChanged( int value );
        void on_spinBoxX_valueChanged( int arg1 );
        void on_spinBoxY_valueChanged( int arg1 );
        void on_spinBoxRadius_valueChanged( int arg1 );
        void on_spinBoxStrength_valueChanged( int arg1 );
        void on_checkBoxBlur_clicked();

        void slotSetPosition( QPointF pos );
        void slotSetRadius( float f );
        void slotSetStrength( float f );
        void emitAll();

    private:
        Ui::VignetteWidget* ui;
};

#endif // VIGNETTE_H
