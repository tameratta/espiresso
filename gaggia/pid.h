#ifndef __pid_h
#define __pid_h

//-----------------------------------------------------------------------------

/// Simple PID controller class
struct PIDControl {
	/// Default constructor
	PIDControl();

	/// Set the Proportional, Integral and Derivative gains
	void setPIDGains( double pGain, double iGain, double dGain );

	/// Set the lower and upper limits for the integrator
	void setIntegratorLimits( double iMin, double iMax );

	/// Update loop
	double update( double error, double position );

private:
	double m_dState;	///< Last position input
	double m_iState;	///< Integrator state
	double m_iMax;		///< Maximum allowable integrator state
	double m_iMin;		///< Minimum allowable integrator state

	double m_iGain;		///< Integral gain
	double m_pGain;		///< Proportional gain
	double m_dGain;		///< Derivative gain
};

//-----------------------------------------------------------------------------

#endif//__pid_h
