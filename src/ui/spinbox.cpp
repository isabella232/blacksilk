#include "spinbox.hpp"

SpinBox::SpinBox( QWidget* parent ) :
    QSpinBox( parent ) {

    void ( QSpinBox:: *signal )( int ) = &QSpinBox::valueChanged;
    connect( this, signal, this, &SpinBox::slotOnValueChanged );

    m_UndoTimer.setSingleShot( true );
    connect( &m_UndoTimer, &QTimer::timeout, this, [ = ] {
        if( m_OnFocusIn != this->value() ) {
            emit signalUndoStep();
        }
    } );
}

void SpinBox::slotOnValueChanged( int ) {
    if( hasFocus() ) {
        // after 1 sec of no more change an undo step is sent
        m_UndoTimer.start( 1000 );
    }
}

void SpinBox::focusInEvent( QFocusEvent* event ) {
    m_OnFocusIn = this->value();
    QSpinBox::focusInEvent( event );
}
