#pragma once

#include <QUndoCommand>
#include <libgraphics/filter.hpp>
#include <functional>

class PresetUndoCommand : public QUndoCommand {
    public:
        explicit PresetUndoCommand( std::function<void( libgraphics::FilterPresetCollection )> function, libgraphics::FilterPresetCollection oldState, libgraphics::FilterPresetCollection newState );

        virtual void undo();
        virtual void redo();

        std::string newStateAsString() const;
        std::string oldStateAsString() const;

    private:
        std::function<void( libgraphics::FilterPresetCollection )> m_Function;
        libgraphics::FilterPresetCollection m_OldState;
        libgraphics::FilterPresetCollection m_NewState;
};
