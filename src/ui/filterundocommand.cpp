#include "filterundocommand.hpp"

#include <ui/filterwidget.h>
#include <utils/preset.hpp>
#include <log/log.hpp>

FilterUndoCommand::FilterUndoCommand( FilterWidget* filterWidget, const libgraphics::FilterPreset& oldState, const libgraphics::FilterPreset& newState ) :
    QUndoCommand(),
    m_FilterWidget( filterWidget ),
    m_OldState( oldState ),
    m_NewState( newState ) {
}

void FilterUndoCommand::undo() {
    if( m_FilterWidget->setPreset( m_OldState ) ) {
        m_FilterWidget->setCurrentState( m_OldState );
    }
}

void FilterUndoCommand::redo() {
    if( m_FilterWidget->setPreset( m_NewState ) ) {
        m_FilterWidget->setCurrentState( m_NewState );
    }
}

std::string FilterUndoCommand::newStateAsString() const {
    std::string s = blacksilk::PresetValue::presetToString( m_NewState, m_NewState.name() );
    return s;
}

std::string FilterUndoCommand::oldStateAsString() const {
    std::string s = blacksilk::PresetValue::presetToString( m_OldState, m_OldState.name() );
    return s;
}
