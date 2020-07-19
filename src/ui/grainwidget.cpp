#include "grainwidget.hpp"
#include "ui_grainwidget.h"

#include <libgraphics/fx/filters/filmgrain.hpp>

const libfoundation::app::EFilter::t GrainWidget::filterType =  libfoundation::app::EFilter::FilmGrainFilter;

GrainWidget::GrainWidget( std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> filterAndPresets, QWidget* parent ) :
    FilterWidget( filterAndPresets, parent ),
    ui( new Ui::GrainWidget ),
    mEventFilter( 0 ) {

    ui->setupUi( this );
    initPresets( ui->comboPresets, ui->buttonReset );

    ui->frameCurve->slotSetInterpolation( CurveFrame::interpolationTypeBezier );
    connect( ui->frameCurve, SIGNAL( signalMoved( float, float ) ), SLOT( emitAll() ) );

    /* prepare for key press events */
    mEventFilter = new CurveFrameEventFilter( ui->frameCurve );
    qApp->installEventFilter( mEventFilter );
    ui->frameCurve->installEventFilter( mEventFilter );

    // undo
    connect( ui->frameCurve, &CurveFrame::signalStep, this, [ = ] { this->iterateUndo( tr( "Film Grain: Curve" ) ); } );
    connect( ui->sliderBlur, &Slider::signalMouseReleased, this, [ = ] { this->iterateUndo( tr( "Film Grain: Blur" ) ); } );

    connect( ui->doubleSpinBoxBlur, &DoubleSpinBox::signalUndoStep, this, [ = ] { this->iterateUndo( tr( "Film Grain: Blur" ) ); } );

    emitAll();
}

GrainWidget::~GrainWidget() {
    if( mEventFilter ) {
        qApp->removeEventFilter( mEventFilter );
        ui->frameCurve->removeEventFilter( mEventFilter );
    }

    delete ui;
}

bool GrainWidget::onUpdateValues() {
    libgraphics::fx::filters::FilmGrain* filmGrain( ( libgraphics::fx::filters::FilmGrain* )this->filter() );
    assert( filmGrain != nullptr );

    QVector<float>  xs;
    QVector<float>  ys;

    size_t index( 0 );
    const auto curveData( filmGrain->curve() );

    for( auto it = curveData.begin(); it != curveData.end(); ++it ) {
        xs.push_back( ( *it ).x );
        ys.push_back( ( *it ).y );

        ++index;
    }

    ui->frameCurve->setCoordinates( xs, ys );
    ui->doubleSpinBoxBlur->setValue( filmGrain->grainBlurRadius() );

    emitAll();

    return true;
}

void GrainWidget::on_doubleSpinBoxBlur_valueChanged( double arg1 ) {
    ui->sliderBlur->setValue( ( int )( arg1 * 10.0 ) );
    emitAll();
}

void GrainWidget::on_sliderBlur_valueChanged( int value ) {
    ui->doubleSpinBoxBlur->setValue( ( float )value / 10.0 );
}

void GrainWidget::slotSetHistogram( QVector<float> histogram ) {
    QVector< float > smoothed( histogram.size() );

    /* borders */
    smoothed[0] = histogram[0];
    smoothed[histogram.size() - 1] = histogram[histogram.size() - 1];

    /* average over 3 */
    for( int i = 1; i < histogram.size() - 1; ++i ) {
        float sum = histogram[i - 1] + histogram[i] + histogram[i + 1];
        smoothed[i] = sum / 3.0;
    }

    ui->frameCurve->setHistogram( smoothed );
}

void GrainWidget::emitAll() {
    QVector<float> fulcrums = ui->frameCurve->getSpline();
    float radius = ui->doubleSpinBoxBlur->value();

    QVector<float> xs;
    QVector<float> ys;

    ui->frameCurve->getCoordinates( xs, ys );

    emit signalGrain( radius, xs, ys );

    /* update preview frame */
    ui->framePreview->slotAdjustWeights( fulcrums );
    ui->framePreview->slotSetRadius( radius );
}
