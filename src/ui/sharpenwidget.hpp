#ifndef SHARPEN_H
#define SHARPEN_H

#include <QWidget>

#include "filterwidget.h"

namespace Ui {
class SharpenWidget;
}

//! \brief GUI class for Sharpening
class SharpenWidget : public FilterWidget {
        Q_OBJECT

    public:
        explicit SharpenWidget( std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> filterAndPresets, QWidget* parent = 0 );
        ~SharpenWidget();

        static const libfoundation::app::EFilter::t filterType;
        virtual bool onUpdateValues();
    signals:
        void signalSharpen( QVector<float>, float t );

    private slots:
        void on_slider07_valueChanged( int value );
        void on_slider14_valueChanged( int value );
        void on_slider28_valueChanged( int value );
        void on_slider56_valueChanged( int value );
        void on_sliderThreshold_valueChanged( int value );

        void setAll();

        void slotSetST( const QVector<float>& strengths, float t );

        void on_spinBoxThreshold_valueChanged( int arg1 );

    private:
        Ui::SharpenWidget* ui;
};

#endif // SHARPEN_H
