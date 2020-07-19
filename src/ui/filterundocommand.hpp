#ifndef FILTERUNDOCOMMAND_HPP
#define FILTERUNDOCOMMAND_HPP

#include <QUndoCommand>

#include <libgraphics/filter.hpp>

class FilterWidget;

class FilterUndoCommand : public QUndoCommand {
    public:
        explicit FilterUndoCommand( FilterWidget* filterWidget, const libgraphics::FilterPreset& oldState, const libgraphics::FilterPreset& newState );

        virtual void undo();
        virtual void redo();

        std::string newStateAsString() const;
        std::string oldStateAsString() const;

    private:
        FilterWidget* m_FilterWidget;
        libgraphics::FilterPreset m_OldState;
        libgraphics::FilterPreset m_NewState;
};

#endif // FILTERUNDOCOMMAND_HPP
