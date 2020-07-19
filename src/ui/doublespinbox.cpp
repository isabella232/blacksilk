#include "doublespinbox.hpp"

DoubleSpinBox::DoubleSpinBox( QWidget* parent ) :
    QDoubleSpinBox( parent ) {

    void ( QDoubleSpinBox:: *signal )( double ) = &QDoubleSpinBox::valueChanged;
    connect( this, signal, this, &DoubleSpinBox::slotOnValueChanged );

    m_UndoTimer.setSingleShot( true );
    connect( &m_UndoTimer, &QTimer::timeout, this, [ = ] {
        if( m_OnFocusIn != this->value() ) {
            emit signalUndoStep();
        }
    } );
}

void DoubleSpinBox::slotOnValueChanged( double ) {
    if( hasFocus() ) {
        // after 1 sec of no more change an undo step is sent
        m_UndoTimer.start( 1000 );
    }
}

void DoubleSpinBox::focusInEvent( QFocusEvent* event ) {
    m_OnFocusIn = this->value();
    QDoubleSpinBox::focusInEvent( event );
}
