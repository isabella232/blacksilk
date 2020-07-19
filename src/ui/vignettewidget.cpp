#include "vignettewidget.hpp"
#include "ui_vignettewidget.h"

#include <libgraphics/fx/filters/vignette.hpp>

const libfoundation::app::EFilter::t VignetteWidget::filterType = libfoundation::app::EFilter::VignetteFilter;

VignetteWidget::VignetteWidget( std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> filterAndPresets, QWidget* parent ) :
    FilterWidget( filterAndPresets, parent ),
    ui( new Ui::VignetteWidget ) {

    ui->setupUi( this );
    initPresets( ui->comboPresets, ui->buttonReset );
    ui->checkBoxBlur->hide();

    connect( ui->frame, SIGNAL( signalPosition( QPointF ) ), SLOT( slotSetPosition( QPointF ) ) );
    connect( ui->frame, SIGNAL( signalRadius( float ) ), SLOT( slotSetRadius( float ) ) );
    connect( ui->frame, SIGNAL( signalStrength( float ) ), SLOT( slotSetStrength( float ) ) );

    // undo
    connect( ui->frame, &VFrame::signalMouseReleased, this, [ = ] { this->iterateUndo( tr( "Vignette: Change" ) ); } );
    connect( ui->sliderX, &Slider::signalMouseReleased, this, [ = ] { this->iterateUndo( tr( "Vignette: X" ) ); } );
    connect( ui->sliderY, &Slider::signalMouseReleased, this, [ = ] { this->iterateUndo( tr( "Vignette: Y" ) ); } );
    connect( ui->sliderRadius, &Slider::signalMouseReleased, this, [ = ] { this->iterateUndo( tr( "Vignette: Radius" ) ); } );
    connect( ui->sliderStrength, &Slider::signalMouseReleased, this, [ = ] { this->iterateUndo( tr( "Vignette: Strength" ) ); } );

    connect( ui->spinBoxX, &SpinBox::signalUndoStep, this, [ = ] { this->iterateUndo( tr( "Vignette: X" ) ); } );
    connect( ui->spinBoxY, &SpinBox::signalUndoStep, this, [ = ] { this->iterateUndo( tr( "Vignette: Y" ) ); } );
    connect( ui->spinBoxRadius, &SpinBox::signalUndoStep, this, [ = ] { this->iterateUndo( tr( "Vignette: Radius" ) ); } );
    connect( ui->spinBoxStrength, &SpinBox::signalUndoStep, this, [ = ] { this->iterateUndo( tr( "Vignette: Strength" ) ); } );

    emitAll();
}

VignetteWidget::~VignetteWidget() {
    delete ui;
}

bool VignetteWidget::onUpdateValues() {
    libgraphics::fx::filters::Vignette* vignetteFilter( ( libgraphics::fx::filters::Vignette* )this->filter() );
    assert( vignetteFilter );

    /* avoid signal flooding */
    this->blockSignals( true );
    {
        ui->spinBoxRadius->setValue( vignetteFilter->radius() );
        ui->spinBoxStrength->setValue( vignetteFilter->strength() );
        ui->spinBoxX->setValue( vignetteFilter->center().x );
        ui->spinBoxY->setValue( vignetteFilter->center().y );
    }
    this->blockSignals( false );

    /* send one now */
    emitAll();

    return true;
}

/// \brief Send relative coordinates, relative radius and slope
void VignetteWidget::emitAll() {
    float x = ui->spinBoxX->value();
    float y = ui->spinBoxY->value();
    float r = ui->spinBoxRadius->value();
    float s = ui->spinBoxStrength->value();
    bool e = ui->checkBoxBlur->isChecked();

    emit signalVignette( x, y, r, s, e );
}

void VignetteWidget::on_sliderX_valueChanged( int value ) {
    if( ui->spinBoxX->value() != value ) {
        ui->spinBoxX->setValue( value );
    }

    ui->frame->slotSetX( ( float )value / 100 );
    emitAll();
}

void VignetteWidget::on_sliderY_valueChanged( int value ) {
    if( ui->spinBoxY->value() != value ) {
        ui->spinBoxY->setValue( value );
    }

    ui->frame->slotSetY( ( float )value / 100 );
    emitAll();
}

void VignetteWidget::on_sliderRadius_valueChanged( int value ) {
    if( ui->spinBoxStrength->value() != value ) {
        ui->spinBoxRadius->setValue( value );
    }

    ui->frame->slotSetRadius( ( float )value / 100 );
    emitAll();
}

void VignetteWidget::on_sliderStrength_valueChanged( int value ) {
    if( ui->spinBoxStrength->value() != value ) {
        ui->spinBoxStrength->setValue( value );
    }

    ui->frame->slotSetStrength( ( float )value / 100 );
    emitAll();
}

void VignetteWidget::on_spinBoxX_valueChanged( int arg1 ) {
    ui->sliderX->setValue( arg1 );
}

void VignetteWidget::on_spinBoxY_valueChanged( int arg1 ) {
    ui->sliderY->setValue( arg1 );
}

void VignetteWidget::on_spinBoxRadius_valueChanged( int arg1 ) {
    ui->sliderRadius->setValue( arg1 );
}

void VignetteWidget::on_spinBoxStrength_valueChanged( int arg1 ) {
    ui->sliderStrength->setValue( arg1 );
}

void VignetteWidget::slotSetPosition( QPointF pos ) {
    ui->frame->blockSignals( true );
    ui->spinBoxX->setValue( pos.x() * 100 );
    ui->spinBoxY->setValue( pos.y() * 100 );
    ui->frame->blockSignals( false );
}

void VignetteWidget::slotSetRadius( float f ) {
    ui->frame->blockSignals( true );
    ui->spinBoxRadius->setValue( f * 100.0 + 0.1 );
    ui->frame->blockSignals( false );
}

void VignetteWidget::slotSetStrength( float f ) {

    // rounding
    if( f < 0 ) {
        f = f * 100.0 - 0.1;
    } else {
        f = f * 100.0 + 0.1;
    }

    ui->frame->blockSignals( true );
    ui->spinBoxStrength->setValue( f );
    ui->frame->blockSignals( false );
}

void VignetteWidget::on_checkBoxBlur_clicked() {
    emitAll();
}
