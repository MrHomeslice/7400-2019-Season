#include "HatchControl.h"
#include "..\Control\RobotControl.h"

void SetEncoderPosition(WPI_TalonSRX &talon, int position);

extern RobotControl g_rc;

HatchControl::HatchControl(int slideID, int grabberID)
            : m_hatchGrab(grabberID), m_hatchSlide(slideID),
							m_hatchSliderState(eHatchSliderStateInitialize), m_hatchGrabState(eHatchGrabStateInitialize)
{
		m_hatchGrabCurrentCounter = 0;
		m_hatchGrabInitialPosition = 0;
}

void HatchControl::Initialize()
{
	m_hatchGrab.SetSafetyEnabled(false);
	m_hatchSlide.SetSafetyEnabled(false);

	m_hatchGrabCurrentCounter = 0;
	m_hatchRaiseCounter = 0;
	m_hatchGrabInitialPosition = m_hatchGrab.GetSelectedSensorPosition(); //Set hatch grabber position on initialize

	m_hatchGrabState = eHatchGrabStateInitialize;
	
	m_hatchSliderState = eHatchSliderStateInitialize;
	m_hatchSlideCurrentCounter   = 0;
}

/**
* Controls states for Hatch slider and grabbers
*/
void HatchControl::Periodic()
{
	double sliderError;
	double grabError;

	//printf("%d %.6f\n\n", m_hatchGrabState, m_hatchGrab.GetOutputCurrent());

	//printf("%d %d\n", g_rc.m_cargoSwitch.Get(), g_rc.m_gamePieceSwitch.Get());

	switch(m_hatchSliderState) //to get out of the In or Out states, that is controlled by the switch(m_hatchGrabState) {}
	{
		case eHatchSliderStateInitialize:
			m_hatchSlide.Set(0.4); //Moves slider in

			if (m_hatchSlide.GetOutputCurrent() >= HATCH_SLIDER_INITIALIZE_CURRENT_THRESHOLD) //Checks if hatch slide is against the robot
			{
				if (++m_hatchSlideCurrentCounter >= HATCH_CURRENT_ITERATIONS)
				{
					m_hatchSliderState = eHatchSliderStateIn;
					SetEncoderPosition(m_hatchSlide, 0); //Sets encoder position to zero when slider is in
				}
			}
			else
				m_hatchSlideCurrentCounter = 0;

			break;

		case eHatchSliderStateMovingIn:
			if (m_hatchSlide.GetOutputCurrent() >= HATCH_SLIDER_IN_CURRENT_THRESHOLD) //Checks if hatch slide is against the robot
				m_hatchSlideCurrentCounter++;
			else
				m_hatchSlideCurrentCounter = 0;

			sliderError = 0 - m_hatchSlide.GetSelectedSensorPosition(); //finds the error between 0 and the current position

			if (fabs(sliderError) <= 10 && m_hatchSlideCurrentCounter >= HATCH_CURRENT_ITERATIONS) //checks if slider is close to 0
			{
				m_hatchSlide.Set(0.1); //set slider to low speed to keep it from falling during pneumatics flip
				m_hatchSliderState = eHatchSliderStateIn;
			}
			else
			{
				if (fabs(sliderError) < 100) //Checks if slider is within 100 encoder ticks of 0 (getting closer to in)
					m_hatchSlide.Set(0.3/100 * sliderError + 0.2); //Set speed to slow down as slider gets closer to zero
				else //If slider is far from in
					m_hatchSlide.Set(0.5); //sets speed to a constant
			}

			break;
		
		case eHatchSliderStateIn:
			m_hatchSlide.Set(0.1); //set slider to low speed to keep it from falling during pneumatics flip
			m_hatchSlideCurrentCounter = 0;

			break;

		case eHatchSliderStateMovingOut:
			//printf("%s %d %d %.6f\n", HatchSliderStateToString(m_hatchSliderState), -320 - m_hatchSlide.GetSelectedSensorPosition(), m_hatchSlide.GetSelectedSensorPosition(), m_hatchSlide.GetOutputCurrent());

			sliderError = -320 - m_hatchSlide.GetSelectedSensorPosition(); //finds error between out position and current position

			/*if the error  is less than 30 (close to out) current limit is 0.6 amps
			if error is greater than 30, current limit is the constant, 1.8 amps*/
			if (m_hatchSlide.GetOutputCurrent() >= (fabs(sliderError) < 30 ? 0.6 : HATCH_SLIDER_OUT_CURRENT_THRESHOLD))
				m_hatchSlideCurrentCounter++;
			else
				m_hatchSlideCurrentCounter = 0;

			if (fabs(sliderError) <= 10 || m_hatchSlideCurrentCounter >= HATCH_CURRENT_ITERATIONS) //check to see if the slider is close to out OR hits a current limit
			{
				m_hatchSlide.Set(0);
				m_hatchSliderState = eHatchSliderStateOut;
			}
			else
			{
				if(fabs(sliderError) < 30) //Checks if slider is within 30 encoder ticks of 320 (getting closer to out)
					m_hatchSlide.Set(0.3/100 * sliderError - 0.2); //Set speed to slow down as slider gets closer to 320
				else //If slider is far from out
					m_hatchSlide.Set(-0.5); //Sets speed to a constant
			}

			break;

		case eHatchSliderStateOut:
			m_hatchSlide.Set(0.0); //waiting for grabbers
			m_hatchSlideCurrentCounter = 0;

			break;
	}

	switch(m_hatchGrabState)
	{
		case eHatchGrabStateInitialize:
			m_hatchGrab.Set(-0.4); //Move grabbers out
		
			if (m_hatchGrab.GetOutputCurrent() >= HATCH_GRAB_CURRENT_THRESHOLD) //Check if grabbers hit a current limit
			{
				if (++m_hatchGrabCurrentCounter >= HATCH_GRAB_ITERATIONS)
				{
					if (g_rc.m_gamePieceSwitch.Get() && !g_rc.m_cargoSwitch.Get()) //Pull in the hatch at the start if the game piece switch is true and the cargo switch is false
					{
						SetEncoderPosition(m_hatchGrab, HATCH_GRAB_HOLDING_POSITION); //if there is a hatch at the start, when a current limit is hit, set the encoder position to holding
						m_hatchGrabState = eHatchGrabStateAcquried;
					}
					else
					{
						SetEncoderPosition(m_hatchGrab, HATCH_GRAB_NOT_HOLDING_POSITION); //if there is not hatch a hatch at the start, when a current limit is hit, set the encoder to not holding
						m_hatchGrabState = eHatchGrabStateNotHolding;
					}
				}
			}

			break;

		case eHatchGrabStateAcquring:
			grabError = m_hatchGrabInitialPosition - m_hatchGrab.GetSelectedSensorPosition(); //Find error from the initial position (value from initialize state) and current position
			
			if(m_hatchSliderState == eHatchSliderStateOut) //check if the slider is out to move the grabbers out
				m_hatchGrab.Set(-0.6); //grabbers go out to start acquiring the hatch

			if(m_hatchGrab.GetSelectedSensorPosition() >= HATCH_GRAB_HOLDING_POSITION + 40) //if grabbers go past the grab position while trying to grab a hatch
				m_hatchGrabState = eHatchGrabStateNotHolding; //go back to start (auto abort)

			if(m_hatchGrab.GetOutputCurrent() >= HATCH_GRAB_CURRENT_THRESHOLD)//check if hatch grabbers hit a current limit (hit the hatch)
			{
				if(++m_hatchGrabCurrentCounter >= HATCH_GRAB_ITERATIONS)
				{
					m_hatchRaiseCounter = 0;
					m_hatchGrabState = eHatchGrabStateAcquried;
					m_hatchSlideCurrentCounter = 0;
					m_hatchSliderState = eHatchSliderStateMovingIn; //when hatch is acquired, slider moves in
				}
			}
			else
				m_hatchGrabCurrentCounter = 0;

			break;

    case eHatchGrabStateAcquried:
			m_hatchGrab.Set(-0.4); //set grab speed in order to hold the hatch (not let go)

			if(++m_hatchRaiseCounter == 50) 
			{
				//g_rc.m_ladderTargetHeight = eLadderHeightReceiveHatch; //raise hatch height in order to see through camera, not used.
			}

			if(!g_rc.m_bCargo && g_rc.m_bAction || g_rc.m_bAbort) //in hatch mode and either the action or abort button is pressed, eject the hatch
			{
				m_hatchSlideCurrentCounter = 0;
				m_hatchSliderState = eHatchSliderStateMovingOut;
			}

			if(m_hatchSliderState == eHatchSliderStateOut) //wait until slider is out from above, then start ejecting
			{
				m_hatchGrabState = eHatchGrabStateEjecting;
			}

			break;
		
    case eHatchGrabStateNotHolding:
			grabError = 0 - m_hatchGrab.GetSelectedSensorPosition(); //find error between 0 and current position
			m_hatchGrab.Set(0.2); //move hatch grabbers in, toward 0

			if(m_hatchSliderState == eHatchSliderStateOut) //wait until the slider is fully out to start moving it back in
			{
				m_hatchSlideCurrentCounter = 0;				
				m_hatchSliderState = eHatchSliderStateMovingIn;
			}

			if(fabs(grabError) <= 5) //check if hatch grabbers are close to 0 (close to in)
				m_hatchGrabState = eHatchGrabStateWaiting;
		
			break;
		
		case eHatchGrabStateEjecting:
			grabError = -10 - m_hatchGrab.GetSelectedSensorPosition(); //find error between -10 (close to in) and current position
			m_hatchGrab.Set(0.4); //move grabbers in

			if(fabs(grabError) <= 10) //check if grabbers are close to in (ejected)
			{
				m_hatchSlideCurrentCounter = 0;
				m_hatchSliderState = eHatchSliderStateMovingIn; //once hatch ejected, start moving slider in
				m_hatchGrabState = eHatchGrabStateWaiting; //hatch ejected, set grabbers to waiting
			}
		
			break;
		
		case eHatchGrabStateWaiting: //this is the empty state, the state it is when driving around without a hatch, waiting to acquire a hatch
			m_hatchGrab.Set(0);

			if (!g_rc.m_bCargo) //checks if in hatch mode
				g_rc.m_ladderTargetHeight = eLadderHeightGround; //moves to ground height

			if (!g_rc.m_bCargo && g_rc.m_bAction) { //if in hatch mode, checks for action button
				m_hatchSlideCurrentCounter = 0;
				m_hatchSliderState         = eHatchSliderStateMovingOut;
			}

			if (m_hatchSliderState == eHatchSliderStateOut) { //waits for slider to be out
				m_hatchGrabCurrentCounter = 0;
				m_hatchGrabState          = eHatchGrabStateAcquring;
			}
	
			break;
	}
}

/**
* @return hatch slider state
*/
HatchSliderState HatchControl::GetHatchMoveState()
{
	return m_hatchSliderState;
}

/**
* @return hatch grab state
*/
HatchGrabState  HatchControl::GetHatchGrabState()
{
	return m_hatchGrabState;
}

/**
* @param hatchGrabState state of grabbers
* @return Grabbers state as a string
* @return "Unknown Grabber State" if state is not found
*/
const char *HatchControl::HatchGrabStateToString(HatchGrabState hatchGrabState)
{
	switch(hatchGrabState)
	{
		case eHatchGrabStateInitialize: return "Initialize";
    case eHatchGrabStateAcquring:	  return "Acquiring";
    case eHatchGrabStateAcquried:   return "Acquired";
    case eHatchGrabStateNotHolding: return "Not Holding";
		case eHatchGrabStateEjecting: 	return "Ejecting";
    case eHatchGrabStateWaiting:    return "Waiting";	
	}

	return "Unknown Grabber State";
}

/**
* @param hatchSliderState state of Slider
* @return Slider state as a string
* @return "Unknown Slider State" if state is not found
*/
const char *HatchControl::HatchSliderStateToString(HatchSliderState hatchSliderState)
{
	switch(hatchSliderState)
	{
		case eHatchSliderStateInitialize: return "Initialize";
		case eHatchSliderStateMovingIn:	  return "Moving In";
		case eHatchSliderStateIn:		      return "In";
		case eHatchSliderStateMovingOut:  return "Moving Out";
		case eHatchSliderStateOut:		    return "Out";
	}

	return "Unknown Slider State";	
}
