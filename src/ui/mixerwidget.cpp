#include "mixerwidget.hpp"
#include "ui_mixerwidget.h"
#include <QColorDialog>
#include <QDebug>

#include <libgraphics/fx/filters/bwadaptivemixer.hpp>

const libfoundation::app::EFilter::t MixerWidget::filterType = libfoundation::app::EFilter::BlackAndWhiteAdaptiveMixerFilter;

MixerWidget::MixerWidget( std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> filterAndPresets, QWidget* parent ) :
    FilterWidget( filterAndPresets, parent ),
    ui( new Ui::MixerWidget ) {

    ui->setupUi( this );
    initPresets( ui->comboPresets, ui->buttonReset );

    ui->frameShadows->setColorSpace( YUVFrame::EnumColorSpaceSuperRGB );
    ui->frameHighlights->setColorSpace( YUVFrame::EnumColorSpaceSuperRGB );

    connect( ui->frameHighlights, SIGNAL( signalWeights( float, float, float ) ), SLOT( slotSetHighlights( float, float, float ) ) );
    connect( ui->frameShadows,    SIGNAL( signalWeights( float, float, float ) ), SLOT( slotSetShadows( float, float, float ) ) );

    // undo
    connect( ui->frameHighlights, &YUVFrame::signalMouseReleased, this, [ = ] { this->iterateUndo( "Mixer: Highlights" ); } );
    connect( ui->frameShadows,    &YUVFrame::signalMouseReleased, this, [ = ] { this->iterateUndo( "Mixer: Shadows" ); } );
    connect( ui->sliderWeight,    &Slider::signalMouseReleased,   this, [ = ] { this->iterateUndo( "Mixer: Weight" ); } );
    connect( ui->sliderHLuma,     &Slider::signalMouseReleased,   this, [ = ] { this->iterateUndo( "Mixer: Highlights" ); } );
    connect( ui->sliderSLuma,     &Slider::signalMouseReleased,   this, [ = ] { this->iterateUndo( "Mixer: Shadows" ); } );

    connect( ui->spinBoxWeight,   &SpinBox::signalUndoStep,       this, [ = ] { this->iterateUndo( "Mixer: Weight" ); } );

    connect( ui->spinBoxHR,       &SpinBox::signalUndoStep,       this, [ = ] { this->iterateUndo( "Mixer: Highlights Red" ); } );
    connect( ui->spinBoxHG,       &SpinBox::signalUndoStep,       this, [ = ] { this->iterateUndo( "Mixer: Highlights Green" ); } );
    connect( ui->spinBoxHB,       &SpinBox::signalUndoStep,       this, [ = ] { this->iterateUndo( "Mixer: Highlights Blue" ); } );

    connect( ui->spinBoxSR,       &SpinBox::signalUndoStep,       this, [ = ] { this->iterateUndo( "Mixer: Shadows Red" ); } );
    connect( ui->spinBoxSG,       &SpinBox::signalUndoStep,       this, [ = ] { this->iterateUndo( "Mixer: Shadows Green" ); } );
    connect( ui->spinBoxSB,       &SpinBox::signalUndoStep,       this, [ = ] { this->iterateUndo( "Mixer: Shadows Blue" ); } );
}

MixerWidget::~MixerWidget() {
    delete ui;
}

bool MixerWidget::onUpdateValues() {
    libgraphics::fx::filters::BWAdaptiveMixer*  mixerFilter( ( libgraphics::fx::filters::BWAdaptiveMixer* )this->filter() );

    ui->spinBoxSR->blockSignals( true );
    ui->spinBoxSG->blockSignals( true );
    ui->spinBoxSB->blockSignals( true );
    ui->spinBoxHR->blockSignals( true );
    ui->spinBoxHG->blockSignals( true );
    ui->spinBoxHB->blockSignals( true );
    ui->spinBoxWeight->blockSignals( true );
    ui->sliderWeight->blockSignals( true );
    ui->buttonLocked->blockSignals( true );

    ui->spinBoxSR->setValue( std::ceil( mixerFilter->shadowWeights().Values[0] * 255.0f ) );
    ui->spinBoxSG->setValue( std::ceil( mixerFilter->shadowWeights().Values[1] * 255.0f ) );
    ui->spinBoxSB->setValue( std::ceil( mixerFilter->shadowWeights().Values[2] * 255.0f ) );

    mRGBShadows = std::make_tuple( ui->spinBoxSR->value(), ui->spinBoxSG->value(), ui->spinBoxSB->value() );

    ui->spinBoxHR->setValue( std::ceil( mixerFilter->highlightWeights().Values[0] * 255.0f ) );
    ui->spinBoxHG->setValue( std::ceil( mixerFilter->highlightWeights().Values[1] * 255.0f ) );
    ui->spinBoxHB->setValue( std::ceil( mixerFilter->highlightWeights().Values[2] * 255.0f ) );

    const int balance = std::ceil( ( mixerFilter->balance() + 0.5f ) * 100.0f );
    ui->spinBoxWeight->setValue( balance );
    ui->sliderWeight->setValue( balance );

    bool locked( mixerFilter->highlightWeights() == mixerFilter->shadowWeights() );

    ui->buttonLocked->setChecked( locked );
    ui->groupBoxShadows->setDisabled( locked );
    ui->labelWeight->setDisabled( locked );
    ui->sliderWeight->setDisabled( locked );
    ui->spinBoxWeight->setDisabled( locked );

    ui->spinBoxSR->blockSignals( false );
    ui->spinBoxSG->blockSignals( false );
    ui->spinBoxSB->blockSignals( false );
    ui->spinBoxHR->blockSignals( false );
    ui->spinBoxHG->blockSignals( false );
    ui->spinBoxHB->blockSignals( false );
    ui->spinBoxWeight->blockSignals( false );
    ui->sliderWeight->blockSignals( false );
    ui->buttonLocked->blockSignals( false );

    setRGBtoShadows();
    setRGBtoHighlights();

    this->emitAll();

    return true;
}


void MixerWidget::setWidgetBackgroundColor( QWidget* widget, const QColor& color ) {
    Q_ASSERT( widget );

    if( color.isValid() ) {
        widget->setStyleSheet( "background-color:" + color.name() + ";border:1px solid gray;border-radius:4px;" );
    }
}

void MixerWidget::setRGBtoShadows() {

    float r = ui->spinBoxSR->value();
    float g = ui->spinBoxSG->value();
    float b = ui->spinBoxSB->value();
    // qDebug() << "LO2" << r << " " << g << " " << b;

    blacksilk::graphics::ColorRGB<float> rgb( r, g, b );
    float luma = rgb.lumaFromRGB();

    ui->sliderSLuma->blockSignals( true );
    ui->sliderSLuma->setValue( ( int )luma );
    ui->sliderSLuma->blockSignals( false );

    ui->frameShadows->blockSignals( true );
    ui->frameShadows->slotSetRGB( blacksilk::graphics::ColorRGB<float>( r, g, b ) );
    ui->frameShadows->blockSignals( false );

    emitAll();
}

void MixerWidget::setRGBtoHighlights() {

    float r = ui->spinBoxHR->value();
    float g = ui->spinBoxHG->value();
    float b = ui->spinBoxHB->value();
    // qDebug() << "HI2" << r << " " << g << " " << b;

    blacksilk::graphics::ColorRGB<float> rgb( r, g, b );
    float luma = rgb.lumaFromRGB();

    ui->sliderHLuma->blockSignals( true );
    ui->sliderHLuma->setValue( ( int )luma );
    ui->sliderHLuma->blockSignals( false );

    ui->frameHighlights->blockSignals( true );
    ui->frameHighlights->slotSetRGB( blacksilk::graphics::ColorRGB<float>( r, g, b ) );
    ui->frameHighlights->blockSignals( false );

    emitAll();
}

void MixerWidget::emitAll() {
    float rh = ui->spinBoxHR->value();
    float gh = ui->spinBoxHG->value();
    float bh = ui->spinBoxHB->value();
    float rs = ui->spinBoxSR->value();
    float gs = ui->spinBoxSG->value();
    float bs = ui->spinBoxSB->value();

    rh /= 255.0;
    gh /= 255.0;
    bh /= 255.0;
    rs /= 255.0;
    gs /= 255.0;
    bs /= 255.0;

    /* clip range from -50 to 50 */
    float weight = ui->sliderWeight->value();
    weight = weight - 50.0;

    emit signalMixer( weight / 100.0, rh, gh, bh, rs, gs, bs );
    //    qDebug() << "Mixer w:" <<  weight
    //             << " H: " << ui->frameHighlights->toString()
    //             << " W: " << ui->frameShadows->toString();
}

void MixerWidget::on_spinBoxWeight_valueChanged( int arg1 ) {
    ui->sliderWeight->setValue( arg1 );
    emitAll();
}

void MixerWidget::on_spinBoxHR_valueChanged( int arg1 ) {
    if( ui->buttonLocked->isChecked() ) {
        ui->spinBoxSR->blockSignals( true );
        ui->spinBoxSR->setValue( arg1 );
        ui->spinBoxSR->blockSignals( false );
        setRGBtoShadows();
    }

    setRGBtoHighlights();
}

void MixerWidget::on_spinBoxHG_valueChanged( int arg1 ) {
    if( ui->buttonLocked->isChecked() ) {
        ui->spinBoxSG->blockSignals( true );
        ui->spinBoxSG->setValue( arg1 );
        ui->spinBoxSG->blockSignals( false );
        setRGBtoShadows();
    }

    setRGBtoHighlights();
}

void MixerWidget::on_spinBoxHB_valueChanged( int arg1 ) {
    if( ui->buttonLocked->isChecked() ) {
        ui->spinBoxSB->blockSignals( true );
        ui->spinBoxSB->setValue( arg1 );
        ui->spinBoxSB->blockSignals( false );
        setRGBtoShadows();
    }

    setRGBtoHighlights();
}

void MixerWidget::on_spinBoxSR_valueChanged( int arg1 ) {
    Q_UNUSED( arg1 );
    setRGBtoShadows();
}

void MixerWidget::on_spinBoxSG_valueChanged( int arg1 ) {
    Q_UNUSED( arg1 );
    setRGBtoShadows();
}

void MixerWidget::on_spinBoxSB_valueChanged( int arg1 ) {
    Q_UNUSED( arg1 );
    setRGBtoShadows();
}

void MixerWidget::on_sliderWeight_valueChanged( int value ) {
    ui->spinBoxWeight->setValue( value );
}

void MixerWidget::on_sliderHLuma_valueChanged( int value ) {
    float r = ui->spinBoxHR->value();
    float g = ui->spinBoxHG->value();
    float b = ui->spinBoxHB->value();

    /* set y to value/3  */
    blacksilk::graphics::ColorRGB<float> rgb( r, g, b );
    rgb.lumaToRGB( value );

    /* calc new y value */
    ui->frameHighlights->blockSignals( true );
    ui->frameHighlights->slotSetLuma( value );
    ui->frameHighlights->blockSignals( false );

    /* set new RGB values */
    slotSetHighlights( rgb.r, rgb.g, rgb.b );
}

void MixerWidget::on_sliderSLuma_valueChanged( int value ) {
    float r = ui->spinBoxSR->value();
    float g = ui->spinBoxSG->value();
    float b = ui->spinBoxSB->value();

    /* set y to value/3  */
    blacksilk::graphics::ColorRGB<float> rgb( r, g, b );
    rgb.lumaToRGB( value );

    /* calc new y value */
    ui->frameShadows->blockSignals( true );
    ui->frameShadows->slotSetLuma( value );
    ui->frameShadows->blockSignals( false );

    /* set new RGB values */
    slotSetShadows( rgb.r, rgb.g, rgb.b );
}

void MixerWidget::slotSetHighlights( float rf, float gf, float bf ) {
    int r = ( int )( rf );
    int g = ( int )( gf );
    int b = ( int )( bf );

    /* get luma from rgb  */
    blacksilk::graphics::ColorRGB<float> rgb( r, g, b );
    int luma = rgb.lumaFromRGB();

    /* set luma to slider */
    ui->sliderHLuma->blockSignals( true );
    ui->sliderHLuma->setValue( luma );
    ui->sliderHLuma->blockSignals( false );

    ui->spinBoxHR->blockSignals( true );
    ui->spinBoxHG->blockSignals( true );
    ui->spinBoxHB->blockSignals( true );

    ui->spinBoxHR->setValue( r );
    ui->spinBoxHG->setValue( g );
    ui->spinBoxHB->setValue( b );

    ui->spinBoxHR->blockSignals( false );
    ui->spinBoxHG->blockSignals( false );
    ui->spinBoxHB->blockSignals( false );

    if( ui->buttonLocked->isChecked() ) {
        slotSetShadows( rf, gf, bf );
        setRGBtoShadows();
    }

    emitAll();
}

void MixerWidget::slotSetShadows( float rf, float gf, float bf ) {
    int r = ( int )( rf );
    int g = ( int )( gf );
    int b = ( int )( bf );

    /* get luma from rgb  */
    blacksilk::graphics::ColorRGB<float> rgb( r, g, b );
    int luma = rgb.lumaFromRGB();

    /* set luma to slider */
    ui->sliderSLuma->blockSignals( true );
    ui->sliderSLuma->setValue( luma );
    ui->sliderSLuma->blockSignals( false );

    ui->spinBoxSR->blockSignals( true );
    ui->spinBoxSG->blockSignals( true );
    ui->spinBoxSB->blockSignals( true );

    ui->spinBoxSR->setValue( r );
    ui->spinBoxSG->setValue( g );
    ui->spinBoxSB->setValue( b );

    ui->spinBoxSR->blockSignals( false );
    ui->spinBoxSG->blockSignals( false );
    ui->spinBoxSB->blockSignals( false );

    emitAll();
}

void MixerWidget::on_buttonLocked_clicked( bool checked ) {

    ui->groupBoxShadows->setDisabled( checked );
    ui->labelWeight->setDisabled( checked );
    ui->sliderWeight->setDisabled( checked );
    ui->spinBoxWeight->setDisabled( checked );

    // chache shadow rgb value
    if( checked ) {
        {
            float r = ui->spinBoxSR->value();
            float g = ui->spinBoxSG->value();
            float b = ui->spinBoxSB->value();
            mRGBShadows = std::make_tuple( r, g, b );
        }
        {
            float r = ui->spinBoxHR->value();
            float g = ui->spinBoxHG->value();
            float b = ui->spinBoxHB->value();

            slotSetShadows( r, g, b );
            setRGBtoShadows();
        }
    } else {
        float r = std::get<0>( mRGBShadows );
        float g = std::get<1>( mRGBShadows );
        float b = std::get<2>( mRGBShadows );
        slotSetShadows( r, g, b );
        setRGBtoShadows();
    }
}
