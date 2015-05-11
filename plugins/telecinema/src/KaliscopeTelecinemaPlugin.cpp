#include "KaliscopeTelecinemaPlugin.hpp"

#include <kali-core/settingsTools.hpp>

#include <tuttle/host/Node.hpp>
#include <tuttle/host/Graph.hpp>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QApplication>
#include <boost/filesystem/path.hpp>

namespace kaliscope
{
namespace plugins
{
using namespace mvpplayer::logic;

void KaliscopeTelecinemaPlugin::setup( mvpplayer::MVPPlayerEngine & model, mvpplayer::gui::IMVPPlayerDialog & view, mvpplayer::logic::MVPPlayerPresenter & presenter )
{
    // Call base class' setup
    IMVPPlugin::setup( model, view, presenter );
    // Intercept record button click
    view.signalViewHitButton.connect( [this, &presenter]( const std::string & commandName, const bool record ) { if ( commandName == "Record" ) this->recordClicked( record ); } );
    presenter.askStoppedStateExternalTransition.connect( boost::bind( &KaliscopeTelecinemaPlugin::recordTransition, this, _1, _2 ) );

    presenter.registerPluginPresenter( kMVPPlayerPluginName, _plugPresenter );
    _plugPresenter.signalRecord.connect( boost::bind( &KaliscopeTelecinemaPlugin::record, this, _1 ) );
    _plugPresenter.signalStopRecord.connect( boost::bind( &MVPPlayerPresenter::processCommandActive, &presenter, std::string( "Record" ), false ) );
    _plugPresenter.signalNextFrame.connect( boost::bind( &KaliscopeTelecinemaPlugin::captureNextFrame, this ) );
}

/**
 * Triggered when the user click on the record button
 */
void KaliscopeTelecinemaPlugin::recordClicked( const bool activated )
{
    if ( activated )
    {
        // Stop current action
        _presenter->processStop();

        // Ask for recording settings
        mvpplayer::Settings recordingSettings;
        const auto settingsSet = _presenter->signalAskSettingsFor( "Record", recordingSettings );
        // Cancelled ?
        if ( settingsSet == boost::none || settingsSet == false )
        {
            return;
        }

        // Queue custom event to enter the right plugin recording state
        {
            using EventT = EvCustomState;
            EventT *event = new EventT( kRecordAction );
            _presenter->signalEvent( *event );
            _presenter->processEvent( *event );
        }
        // Queue record event
        {
            using EventT = logic::plugin::EvRecord;
            EventT *event = new EventT( recordingSettings );
            _presenter->signalEvent( *event );
            _presenter->processEvent( *event );
        }
    }
    else
    {
        // Queue stop event
        _presenter->processStop();
    }
}

/**
 * @brief Needed to enter into the Recording state
 */
boost::statechart::detail::reaction_result KaliscopeTelecinemaPlugin::recordTransition( const std::string & action, Stopped & state )
{
    if ( action == kRecordAction )
    {
        const auto result = state.transit<logic::plugin::Recording>();
        // The following is needed to silent assert in boost
        sc::detail::result_utility::get_result( result );
        return boost::statechart::detail::consumed;
    }
    else
    {
        return boost::statechart::detail::no_reaction;
    }
}

/**
 * Triggered when we want to start the recording using given settings
 * @param settings recording settings
 */
void KaliscopeTelecinemaPlugin::record( const mvpplayer::Settings & settings )
{
    try
    {
        // Recording goes here:
        _kaliscopeEngine = dynamic_cast<KaliscopeEngine*>( _model );
        assert( _kaliscopeEngine != nullptr );
        _kaliscopeEngine->stop();
        _kaliscopeEngine->setFrameStepping( true );

        std::shared_ptr<tuttle::host::Graph> graph( new tuttle::host::Graph() );
        setupGraphWithSettings( *graph, settings );
        _kaliscopeEngine->setProcessingGraph( graph );
        _kaliscopeEngine->start();
    }
    catch( ... )
    {
        std::cerr << "Unable to record!" << std::endl;
    }
}

/**
 * @brief capture next frame
 */
void KaliscopeTelecinemaPlugin::captureNextFrame()
{
    if ( _kaliscopeEngine )
    {
        std::cout << "processing next frame" << std::endl;
        _kaliscopeEngine->processNextFrame();
    }
}


void KaliscopeTelecinemaPlugin::playTrack()
{
    // Queue stop event
    _presenter->processStop();
    _presenter->processPlay( boost::none );
}


}
}

#if (QT_VERSION < 0x050000)
Q_EXPORT_PLUGIN2( kaliscope::plugins::KaliscopeTelecinemaPlugin, KaliscopeTelecinemaPlugin )
#endif // (QT_VERSION >= 0x050000)