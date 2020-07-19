#include <QString>
#include <QtTest>

#include <libgraphics/filter.hpp>
#include <utils/preset.hpp>

#include "ui/mixerwidget.hpp"
#include <iostream>

libgraphics::FilterPreset s_PreviousState = blacksilk::PresetValue::stringToPreset( "bwmixer=name(\"previous\"),highlights(255,0,0),shadows(255,0,0),weight(50)" );
libgraphics::FilterPreset s_CurrentState = blacksilk::PresetValue::stringToPreset( "bwmixer=name(\"current\"),highlights(0,0,255),shadows(255,0,0),weight(50)" );

// dummy implementation
bool FilterWidget::setPreset( const libgraphics::FilterPreset& preset ) {
    bool updated = s_CurrentState != preset;
    return updated;
}

void FilterWidget::swapStates() {
    std::swap( s_CurrentState, s_PreviousState );
}

class TestFilterUndoCommand : public QObject {
        Q_OBJECT

    public:
        TestFilterUndoCommand();

    private Q_SLOTS:
        void testUndo();
        void testSerialization();
};

TestFilterUndoCommand::TestFilterUndoCommand() {
}

void TestFilterUndoCommand::testSerialization() {

    std::vector<std::string> filters;
    filters.push_back( "bwmixer=name(\"new\"), highlights( 0, 0, 255 ), shadows( 0, 0, 255 ), weight( 50 )" );
    filters.push_back( "filmgrain=name(\"ISO 64 equiv\"), radius( 0.3 ), points( 0 : 0, 0.5 : 0.3, 1 : 0 )" );
    filters.push_back( "curves=name(\"Neutral\"), points( 0 : 0, 0.5 : 0.5, 1 : 1 )" );
    filters.push_back( "sharpen=name(\"Normal\"), cascade( 50 ), cascade( 200 ), cascade( 100 ), cascade( 0 ), threshold( 50 )" );
    filters.push_back( "splittone=name(\"Golden\"), highlights( 190, 156, 54 ), shadows( 110, 148, 198 ), weight( 50 )" );
    filters.push_back( "vignette=name(\"Black\"), x( 50 ), y( 50 ), strength( 50 ), radius( 50 )" );

    for( std::string start : filters ) {
        libgraphics::FilterPreset preset = blacksilk::PresetValue::stringToPreset( start );
        std::string end = blacksilk::PresetValue::presetToString( preset, preset.name() );
        if( start != end ) {
            std::cout << "start : " << start << std::endl;
            std::cout << "end   : " << end << std::endl;
        }
        QVERIFY( start == end );
    }
}

void TestFilterUndoCommand::testUndo() {
    QWidget widget;

    libgraphics::FilterPreset oldState = s_PreviousState;
    libgraphics::FilterPreset newState = s_CurrentState;

    std::string test = blacksilk::PresetValue::presetToString( oldState, oldState.name() );

    FilterUndoCommand* command = new FilterUndoCommand( ( FilterWidget* )( &widget ), oldState, newState );

    QUndoStack undoStack;
    undoStack.push( command );

    undoStack.undo();
    QVERIFY( oldState == s_CurrentState );
    QVERIFY( newState == s_PreviousState );
    QVERIFY( oldState != s_PreviousState );
    QVERIFY( newState != s_CurrentState );

    undoStack.redo();
    QVERIFY( oldState == s_PreviousState );
    QVERIFY( newState == s_CurrentState );
    QVERIFY( oldState != s_CurrentState );
    QVERIFY( newState != s_PreviousState );
}

QTEST_MAIN( TestFilterUndoCommand )

#include "testFilterUndoCommand.moc"
