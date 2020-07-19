#include <QDebug>
#include "curveswidget.hpp"
#include "ui_curveswidget.h"

#include <libgraphics/fx/filters/curves.hpp>

const libfoundation::app::EFilter::t CurvesWidget::filterType = libfoundation::app::EFilter::CurvesFilter;

CurvesWidget::CurvesWidget( std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> filterAndPresets, QWidget* parent ) :
    FilterWidget( filterAndPresets, parent ),
    ui( new Ui::CurvesWidget ),
    mEventFilter( 0 ),
    mWeightsHighlights( ColorRGB<float>( 0.333, 0.333, 0.333 ) ) {

    ui->setupUi( this );
    initPresets( ui->comboPresets, ui->buttonReset );

    connect( ui->frameCurve, SIGNAL( signalMoved( float, float ) ), SLOT( emitAll() ) );

    // undo
    connect( ui->frameCurve, &CurveFrame::signalStep, this, [ = ] { this->iterateUndo( "Curves" ); } );

    /* prepare for key press events */
    mEventFilter = new CurveFrameEventFilter( ui->frameCurve );
    qApp->installEventFilter( mEventFilter );
    ui->frameCurve->installEventFilter( mEventFilter );
}

CurvesWidget::~CurvesWidget() {
    if( mEventFilter ) {
        qApp->removeEventFilter( mEventFilter );
        ui->frameCurve->removeEventFilter( mEventFilter );
    }

    delete ui;
}

void CurvesWidget::emitAll() {
    QVector<float> histo = ui->frameCurve->getOutHistogram();
    QVector<float> xs;
    QVector<float> ys;

    ui->frameCurve->getCoordinates( xs, ys );

    emit signalCurves( xs, ys );
    emit signalOutHistogram( histo );
}

bool CurvesWidget::onUpdateValues() {
    libgraphics::fx::filters::Curves* curves = ( libgraphics::fx::filters::Curves* )this->m_Filter;
    assert( curves != nullptr );


    QVector<float>  xs;
    QVector<float>  ys;

    size_t index( 0 );
    const auto curveData( curves->curve() );

    for( auto it = curveData.begin(); it != curveData.end(); ++it ) {
        xs.push_back( ( *it ).x );
        ys.push_back( ( *it ).y );

        ++index;
    }

    ui->frameCurve->setCoordinates( xs, ys );
    emitAll();

    return true;
}

void CurvesWidget::slotSetHistogram( QVector< ColorRGB<float> > histo ) {
    if( histo.size() != HISTOGRAM_SIZE ) {
        qWarning() << "Histogramm has the wrong size";
    }

    mHistogram = histo;
}

//! \brief (Re-)Sets the weights from the mixer filter
void CurvesWidget::slotSetWeights( float weight, float r1, float g1, float b1, float r2, float g2, float b2 ) {

    mWeightsHighlights.r = r1;
    mWeightsHighlights.g = g1;
    mWeightsHighlights.b = b1;

    mWeightsShadows.r = r2;
    mWeightsShadows.g = g2;
    mWeightsShadows.b = b2;

    mWeight = weight;

    QVector< float > histoNew( mHistogram.size() );

    int size = mHistogram.size();

    int pos = 0;
    float sum = 0;
    float bright = r1 + g1 + b1;
    float dark   = r2 + g2 + b2;

    for( int i = 0; i < size; ++i ) {

        sum = 0;

        pos = i;
        float luma = weight + ( float ) pos / size;

        auto trans = [&]( float from, float c1, float c2 ) {
            float to = c1 / bright * from * luma +
                       c2 / dark * from * ( 1.0 - luma );
            return to;
        };

        if( pos < size && pos > 0 ) {
            sum += trans( mHistogram[pos].r, r1, r2 );
            sum += trans( mHistogram[pos].g, g1, g2 );
            sum += trans( mHistogram[pos].b, b1, b2 );
        }

        histoNew[i] = sum / dark ;
    }

    // qDebug() << "r: " << r << "g: " << g << "b: " << b;

    ui->frameCurve->setHistogram( histoNew );

    this->emitAll();
}
