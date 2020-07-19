#include "actionundocommand.hpp"

#include <QAction>
#include <log/log.hpp>

ActionUndoCommand::ActionUndoCommand( std::function<void( bool )> function, bool oldState, bool newState ) :
    QUndoCommand(),
    m_Function( function ),
    m_OldState( oldState ),
    m_NewState( newState ) {
}

void ActionUndoCommand::undo() {
    m_Function( m_OldState );
}

void ActionUndoCommand::redo() {
    m_Function( m_NewState );
}

std::string ActionUndoCommand::newStateAsString() const {
    std::string s = std::to_string( m_NewState );
    return s;
}

std::string ActionUndoCommand::oldStateAsString() const {
    std::string s = std::to_string( m_OldState );
    return s;
}
