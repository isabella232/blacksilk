#include "sharpenwidget.hpp"
#include "ui_sharpenwidget.h"
#include <libgraphics/fx/filters/cascadedsharpen.hpp>

const libfoundation::app::EFilter::t SharpenWidget::filterType = libfoundation::app::EFilter::CascadedSharpenFilter;

SharpenWidget::SharpenWidget( std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> filterAndPresets, QWidget* parent ) :
    FilterWidget( filterAndPresets, parent ),
    ui( new Ui::SharpenWidget ) {

    ui->setupUi( this );
    initPresets( ui->comboPresets, ui->buttonReset );

    //undo
    connect( ui->slider07, &Slider::signalMouseReleased, this, [ = ] { this->iterateUndo( "Sharpen: 0.7" ); } );
    connect( ui->slider14, &Slider::signalMouseReleased, this, [ = ] { this->iterateUndo( "Sharpen: 1.4" ); } );
    connect( ui->slider28, &Slider::signalMouseReleased, this, [ = ] { this->iterateUndo( "Sharpen: 2.8" ); } );
    connect( ui->slider56, &Slider::signalMouseReleased, this, [ = ] { this->iterateUndo( "Sharpen: 5.6" ); } );

    connect( ui->sliderThreshold,  &Slider::signalMouseReleased, this, [ = ] { this->iterateUndo( "Sharpen: Threshold" ); } );
    connect( ui->spinBoxThreshold, &SpinBox::signalUndoStep,     this, [ = ] { this->iterateUndo( "Sharpen: Threshold" ); } );
}

SharpenWidget::~SharpenWidget() {
    delete ui;
}

bool SharpenWidget::onUpdateValues() {
    libgraphics::fx::filters::CascadedSharpen* sharpenFilter( ( libgraphics::fx::filters::CascadedSharpen* )this->filter() );

    QVector<float> strengths;

    for( size_t i = 0; sharpenFilter->cascadeCount() > i; ++i ) {
        strengths.push_back( sharpenFilter->cascadeStrength( i ) );
    }

    slotSetST( strengths, sharpenFilter->threshold() );

    return true;
}

void SharpenWidget::slotSetST( const QVector<float>& strengths, float t ) {
    ui->slider07->blockSignals( true );
    ui->slider14->blockSignals( true );
    ui->slider28->blockSignals( true );
    ui->slider56->blockSignals( true );
    ui->sliderThreshold->blockSignals( true );
    ui->spinBoxThreshold->blockSignals( true );

    ui->slider07->setValue( strengths[0] );
    ui->slider14->setValue( strengths[1] );
    ui->slider28->setValue( strengths[2] );
    ui->slider56->setValue( strengths[3] );
    ui->sliderThreshold->setValue( t );
    ui->spinBoxThreshold->setValue( t );

    ui->slider07->blockSignals( false );
    ui->slider14->blockSignals( false );
    ui->slider28->blockSignals( false );
    ui->slider56->blockSignals( false );
    ui->sliderThreshold->blockSignals( false );
    ui->spinBoxThreshold->blockSignals( false );

    emit signalSharpen( strengths, t );
}

void SharpenWidget::on_sliderThreshold_valueChanged( int value ) {
    Q_UNUSED( value )

    /* set new RGB values */
    setAll();
}

void SharpenWidget::on_slider07_valueChanged( int value ) {
    Q_UNUSED( value )

    /* set new RGB values */
    setAll();
}

void SharpenWidget::on_slider14_valueChanged( int value ) {
    Q_UNUSED( value )

    /* set new RGB values */
    setAll();
}

void SharpenWidget::on_slider28_valueChanged( int value ) {
    Q_UNUSED( value )

    /* set new RGB values */
    setAll();
}

void SharpenWidget::on_slider56_valueChanged( int value ) {
    Q_UNUSED( value )

    /* set new RGB values */
    setAll();
}

void SharpenWidget::setAll() {
    QVector<float> strengths;
    strengths.push_back( ui->slider07->value() );
    strengths.push_back( ui->slider14->value() );
    strengths.push_back( ui->slider28->value() );
    strengths.push_back( ui->slider56->value() );

    float t = ui->sliderThreshold->value();
    ui->spinBoxThreshold->setValue( ui->sliderThreshold->value() );

    slotSetST( strengths, t );
}

void SharpenWidget::on_spinBoxThreshold_valueChanged( int arg1 ) {
    if( ui->sliderThreshold->value() != arg1 ) {
        ui->sliderThreshold->setValue( arg1 );
    }
}
