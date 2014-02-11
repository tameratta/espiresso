#include "ranger.h"
#include <math.h>
#include "settings.h"
#include "timing.h"

//-----------------------------------------------------------------------------

Ranger::Ranger() :
	m_trigger( RANGER_TRIGGER_OUT ),
	m_echo( RANGER_ECHO_IN ),
	m_range( 0.0 ),
	m_count( 0 ),
	m_run( true )
{
	// make trigger an output and set it low
	m_trigger.setOutput( true ).setState( false );

	// make echo an input and enable edge triggered interrupts on both
	// rising and falling edges
	m_echo.setOutput( false ).setEdgeTrigger( GPIOPin::Both );

	// record current time as last run time, in case the initialisation
	// above has triggered the ranger
	m_timeLastRun = getClock();

	// start the worker thread
	m_thread = std::thread( &Ranger::worker, this );
}

//-----------------------------------------------------------------------------

Ranger::~Ranger()
{
	// gracefully terminate the thread
	m_run = false;

	// wait for the thread to terminate
	m_thread.join();
}

//-----------------------------------------------------------------------------

double Ranger::getRange() const
{
	// return the range value
	std::lock_guard<std::mutex> lock( m_mutex );
	return m_range;
}

//-----------------------------------------------------------------------------

unsigned Ranger::getCount() const
{
	// return the counter value
	std::lock_guard<std::mutex> lock( m_mutex );
	return m_count;
}

//-----------------------------------------------------------------------------

bool Ranger::initialise()
{
	// wait up to 0.5s for the first range measurement
	for ( int i=0; (i<10) && !ready(); i++ )
		delayms( 50 );

	// did we get one?
	return ready();
}

//-----------------------------------------------------------------------------

bool Ranger::ready() const
{
	// return true if at least one range measurement has been taken
	std::lock_guard<std::mutex> lock( m_mutex );
	return (m_count > 0);
}

//-----------------------------------------------------------------------------

void Ranger::worker()
{
	double range = 0.0;
	double oldRange = 0.0;
	bool firstTime = true;

	while (m_run) {
		oldRange = range;

		// take a range measurement (will block)
		double range = measureRange();

		// does this measurement look dubious?
		bool outlier =
			( !firstTime && (fabs(range - oldRange) > 0.01) ) ||
			( range < 0.001 );

		// have another attempt
		if ( outlier )
			range = measureRange();

		{
			// lock the mutex
			std::lock_guard<std::mutex> lock( m_mutex );

			// initialisation of filter
			if ( firstTime ) {
				m_range = range;
				firstTime = false;
			}

			// store filtered value
			const double k = 0.5;
			m_range = m_range + k * (range - m_range);
			m_count++;
		}
	}
}

//-----------------------------------------------------------------------------

double Ranger::measureRange()
{
    // minimum time (in seconds) between successive calls
    // this is to prevent the ranger from being triggered too frequently
    const double minimumInterval = 0.2;

    // timeout in milliseconds when waiting for echo
    const unsigned timeout = 60;

    double elapsed = 0.0;   // elapsed time in seconds
    bool response = false;  // have we received a reply?

    // calculate interval since we were last run
    double interval = getClock() - m_timeLastRun;
    if ( interval < minimumInterval ) {
        // delay if needed to avoid sending a trigger when echoes from the
        // previous trigger are still incoming
        delayms( 1000.0 * (minimumInterval - interval) );
    }

    // remember time of last run
    m_timeLastRun = getClock();

    // transmit 10us high pulse to trigger the ranger
	m_trigger.usPulse( true, 10 );

    // wait for rising edge
    if ( m_echo.poll( timeout ) ) {
        // record time of rising edge
        double rise = getClock();

		// wait for falling edge
        if ( m_echo.poll( timeout ) ) {
            // record time of falling edge
            double fall = getClock();

            // calculate elapsed time
            elapsed = fall - rise;

            // set flag to indicate that a response was received
            response = true;
        }
    }

    // calculate distance in m
    double speedOfSound = 340.27;   // m/s
    double distance = elapsed * speedOfSound / 2.0;

    // return distance (or zero if echo not received)
    if ( response )
        return distance;
    else
        return 0.0;
}//getRange

//-----------------------------------------------------------------------------
