#pragma once

#include <QUndoCommand>
#include <libgraphics/filter.hpp>
#include <functional>

class ActionUndoCommand : public QUndoCommand {
    public:
        explicit ActionUndoCommand( std::function<void( bool )> function, bool oldState, bool newState );

        virtual void undo();
        virtual void redo();

        std::string newStateAsString() const;
        std::string oldStateAsString() const;

    private:
        std::function<void( bool )> m_Function;
        bool m_OldState;
        bool m_NewState;
};
