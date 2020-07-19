#include "presetundocommand.hpp"

#include <QAction>
#include <log/log.hpp>

PresetUndoCommand::PresetUndoCommand( std::function<void( libgraphics::FilterPresetCollection )> function, libgraphics::FilterPresetCollection oldState, libgraphics::FilterPresetCollection newState ) :
    QUndoCommand(),
    m_Function( function ),
    m_OldState( oldState ),
    m_NewState( newState ) {
}

void PresetUndoCommand::undo() {
    m_Function( m_OldState );
}

void PresetUndoCommand::redo() {
    m_Function( m_NewState );
}

std::string PresetUndoCommand::newStateAsString() const {
    std::string s = m_NewState.toString( "new state" );
    return s;
}

std::string PresetUndoCommand::oldStateAsString() const {
    std::string s = m_OldState.toString( "old state" );
    return s;
}
