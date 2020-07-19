#pragma once

#include <QDebug>
#include <QWidget>
#include <QVector>
#include <QPair>
#include <QUndoStack>

#include <libfoundation/app/application.hpp>
#include <libgraphics/filterpresetcollection.hpp>
#include <ui/filterundocommand.hpp>

class QComboBox;
class QPushButton;

//! \brief Base class for all filter widgets
class FilterWidget : public QWidget {
        Q_OBJECT

    protected:
        QComboBox* mComboPresets;
        QPushButton* mButtonReset;

        /// undo
        libgraphics::FilterPreset m_PreviousState;
        libgraphics::FilterPreset m_CurrentState;
        bool m_SkipUndo;

        /// filter and presets
        libfoundation::app::EFilter::t          m_AssociatedFilterType;
        libgraphics::Filter*                    m_Filter;
        libgraphics::FilterPresetCollection     m_DefaultFilterPresets;

    public:
        FilterWidget() {}
        FilterWidget( std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> filterAndPresets,
                      QWidget* parent = 0 );

        /// type identifier
        libgraphics::Filter*    filter() const;
        std::string name() const;
        libgraphics::FilterPresetCollection&    filterPresets();
        const libgraphics::FilterPresetCollection&    filterPresets() const;

        /// common interface
        virtual bool onUpdateValues() = 0;

        /// preset initializers
        void initPresets( QComboBox* cb, QPushButton* pb );
        bool selectPresetWithIndex( size_t index );

        bool setPreset( const libgraphics::FilterPreset& preset );
        void setCurrentState( const libgraphics::FilterPreset& state );

    signals:
        void signalUndoIterated( FilterUndoCommand* );

    public slots:
        virtual void reset();
        virtual void initialize();
    protected slots:
        void on_comboPresets_currentIndexChanged( int );

        // call from derived class to trigger an undo step
        void iterateUndo( const QString& what );
};
