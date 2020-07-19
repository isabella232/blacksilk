#include "filterwidget.h"

#include <QFileInfo>
#include <QComboBox>
#include <QPushButton>
#include <QFile>
#include <string>

#include <utils/app.hpp>
#include <utils/preset.hpp>
#include <libcommon/scopeguard.hpp>
#include <libgraphics/filterstack.hpp>
#include <libgraphics/filtercollection.hpp>
#include <log/log.hpp>

FilterWidget::FilterWidget( std::tuple<libgraphics::Filter*, libgraphics::FilterPresetCollection> filterAndPresets, QWidget* parent )
    : QWidget( parent ),
      mComboPresets( NULL ),
      m_SkipUndo( false ) {

    m_Filter = std::get<0>( filterAndPresets );
    m_DefaultFilterPresets = std::get<1>( filterAndPresets );
    assert( m_Filter != nullptr );
    assert( m_DefaultFilterPresets.count() > 0 );

    // to open images and presets with drag & drop
    this->setAcceptDrops( true );
}

libgraphics::Filter*    FilterWidget::filter() const {
    return this->m_Filter;
}

std::string FilterWidget::name() const {
    return m_Filter->name();
}

libgraphics::FilterPresetCollection&    FilterWidget::filterPresets() {
    return this->m_DefaultFilterPresets;
}

const libgraphics::FilterPresetCollection&    FilterWidget::filterPresets() const {
    return this->m_DefaultFilterPresets;
}



bool FilterWidget::selectPresetWithIndex( size_t index ) {

    bool ok = false;

    if( this->m_DefaultFilterPresets.count() > index ) {
        const auto selectedPresetEntry = *( m_DefaultFilterPresets.begin() + index );

        assert( m_Filter != nullptr );

        if( m_Filter == nullptr ) {
            qDebug() << "Failed to set preset for invalid filter.";
            return false;
        }

        ok = this->setPreset( selectedPresetEntry.preset );
        this->iterateUndo( QString::fromStdString( selectedPresetEntry.preset.name() ) );

    } else {
        assert( false );
        qDebug() << "Failed to set filter preset with invalid index " << index << ".";
    }

    return ok;
}

bool FilterWidget::setPreset( const libgraphics::FilterPreset& preset ) {

    if( m_CurrentState == preset ) {
        // LOG( "Won't set preset for " + preset.name() );
        // LOG( blacksilk::PresetValue::presetToString( preset, "m_CurrentState == preset" ) );
        return false;
    }

    const auto successfullySet = m_Filter->fromPreset( preset );
    assert( successfullySet );
    // LOG( blacksilk::PresetValue::presetToString( preset, "preset" ) );

    if( !successfullySet ) {
        qDebug() << "Failed to set preset for filter. Invalid preset.";
        return false;
    }

    const auto updatedValues = this->onUpdateValues();

    if( updatedValues ) {
        blacksilk::theApp()->triggerRendering();
    }

    return updatedValues;
}

void FilterWidget::setCurrentState( const libgraphics::FilterPreset& state ) {
    m_CurrentState = state;
}

void FilterWidget::initPresets( QComboBox* cb, QPushButton* pb ) {
    assert( cb != nullptr );
    assert( pb != nullptr );

    mComboPresets = cb;
    mButtonReset  = pb;

    size_t currentIndex( 0 );

    libcommon::ScopeGuard guard( [ = ]() {
        this->mComboPresets->blockSignals( true );
    }, [ = ]() {
        this->mComboPresets->blockSignals( false );
    } );

    if( this->m_DefaultFilterPresets.count() != 0 ) {
        std::string currentCategory( this->m_DefaultFilterPresets.front().category() );

        for( auto it = this->m_DefaultFilterPresets.begin(); it != this->m_DefaultFilterPresets.end(); ++it ) {
            this->mComboPresets->insertItem(
                currentIndex,
                ( *it ).preset.name().c_str(),
                QVariant::fromValue(
                    std::distance( this->m_DefaultFilterPresets.begin(), it )
                )
            );

            if( ( *it ).preset.category() != currentCategory ) {
                this->mComboPresets->insertSeparator( currentIndex );
                ++currentIndex;

                currentCategory = ( *it ).preset.category();
            }

            ++currentIndex;
        }

    }

    connect( mComboPresets, SIGNAL( currentIndexChanged( int ) ), this, SLOT( on_comboPresets_currentIndexChanged( int ) ), Qt::UniqueConnection );
    connect( mButtonReset, SIGNAL( clicked() ), this, SLOT( reset() ) );

#ifdef LIBGRAPHICS_DEBUG_OUTPUT
    qDebug() << "Added " << currentIndex << " default presets for filter '" << name().c_str() << "'...";
#endif
}

void FilterWidget::reset() {
    this->mComboPresets->setCurrentIndex( 0 );
    this->selectPresetWithIndex( 0 );
}

//! \brief resets the filters w/out triggering an undo step
void FilterWidget::initialize() {
    m_SkipUndo = true;
    this->reset();
    m_PreviousState = m_Filter->toPreset();
}

void FilterWidget::on_comboPresets_currentIndexChanged( int /*index*/ ) {
    bool successfullyConverted( false );

    const auto data = this->mComboPresets->currentData();
    const auto val  = data.toInt( &successfullyConverted );

    if( successfullyConverted ) {
        this->selectPresetWithIndex( val );
    }
}

void FilterWidget::iterateUndo( const QString& what ) {

    libgraphics::FilterPreset current = m_Filter->toPreset();

    if( m_CurrentState == current ) {
        // happens, if someone just clicks
        LOG( "States are equal, dismissing undo step" );
    } else {
        m_PreviousState = m_CurrentState;
        m_CurrentState = current;

        if( m_SkipUndo ) {
            m_SkipUndo = false;
        } else {
            // LOG( "Undo step:" );
            // LOG( blacksilk::PresetValue::presetToString( m_PreviousState, "previous" ) );
            // LOG( blacksilk::PresetValue::presetToString( m_CurrentState, "current" ) );

            FilterUndoCommand* command = new FilterUndoCommand( this, m_PreviousState, m_CurrentState );
            command->setText( what );
            emit signalUndoIterated( command );
        }
    }
}
