#include "HatchControl.h"
#include "..\Control\RobotControl.h"

void SetEncoderPosition(WPI_TalonSRX &talon, int position);

extern RobotControl g_rc;

HatchControl::HatchControl(int slideID, int grabberID)
             : m_hatchGrab(grabberID), m_hatchSlide(slideID),
			m_hatchSliderState(eHatchSliderStateInitialize), m_hatchGrabState(eHatchGrabStateInitialize)
{
		m_hatchGrabCounter = 0;
		m_hatchGrabInitialPosition = 0;
		m_hatchGrabInitialPosition = 0;
}

void HatchControl::Initialize()
{
	m_hatchGrabCounter = 0;
	m_hatchRaiseCounter = 0;
	m_hatchGrabInitialPosition = m_hatchGrab.GetSelectedSensorPosition();

	m_hatchGrabState = eHatchGrabStateInitialize;
	
	m_hatchSliderState = eHatchSliderStateInitialize;
}

void HatchControl::Periodic()
{
	double sliderError;
	double grabError;

	//printf("%d %.6f\n\n", m_hatchGrabState, m_hatchGrab.GetOutputCurrent());

	//printf("%d %d\n", g_rc.m_cargoSwitch.Get(), g_rc.m_gamePieceSwitch.Get());

    switch(m_hatchSliderState)
	{
		case eHatchSliderStateInitialize:
			m_hatchSlide.Set(0.4); //Moves slider in

			if(m_hatchSlide.GetOutputCurrent() >= HATCH_SLIDER_INITIALIZE_CURRENT_THRESHOLD) //Checks to see if slider is hitting a current against the robot
			{
				if(++m_currentCounter == HATCH_CURRENT_ITERATIONS)
				{
					m_hatchSliderState = eHatchSliderStateIn;
					SetEncoderPosition(m_hatchSlide, 0); //Sets encoder position to zero when slider is in
				}
			}
			else
				m_currentCounter = 0;

			break;

		case eHatchSliderStateMovingIn:
			if(m_hatchSlide.GetOutputCurrent() >= HATCH_SLIDER_IN_CURRENT_THRESHOLD) //Checks to see if slider is hitting a current against the robot
				m_currentCounter++;
			else
				m_currentCounter = 0;

			sliderError = 0 - m_hatchSlide.GetSelectedSensorPosition();

			if(fabs(sliderError) <= 10 && m_currentCounter >= HATCH_CURRENT_ITERATIONS)
			{
				m_hatchSlide.Set(0.1);
				m_hatchSliderState = eHatchSliderStateIn;
			}
			else
			{
				if(fabs(sliderError) < 100)
					m_hatchSlide.Set(0.3/100 * sliderError + 0.2);
				else
					m_hatchSlide.Set(0.5);
			}

			break;
		
		case eHatchSliderStateIn:
			m_hatchSlide.Set(0.1);

			m_currentCounter = 0;

			break;

		case eHatchSliderStateMovingOut:
			//printf("%s %d %d %.6f\n", HatchSliderStateToString(m_hatchSliderState), -320 - m_hatchSlide.GetSelectedSensorPosition(), m_hatchSlide.GetSelectedSensorPosition(), m_hatchSlide.GetOutputCurrent());

			sliderError = -320 - m_hatchSlide.GetSelectedSensorPosition();

			if(m_hatchSlide.GetOutputCurrent() >= (fabs(sliderError) < 30 ? 0.6 : HATCH_SLIDER_OUT_CURRENT_THRESHOLD))
				m_currentCounter++;
			else
				m_currentCounter = 0;

			if(fabs(sliderError) <= 10 || m_currentCounter >= HATCH_CURRENT_ITERATIONS)
			{
				m_hatchSlide.Set(0);
				m_hatchSliderState = eHatchSliderStateOut;
			}
			else
			{
				if(fabs(sliderError) < 30)
					m_hatchSlide.Set(0.3/100 * sliderError - 0.2);
				else
					m_hatchSlide.Set(-0.5);
			}

			break;

		case eHatchSliderStateOut:
			m_hatchSlide.Set(0.0);

			m_currentCounter = 0;

			break;
	}

	switch(m_hatchGrabState)
	{
		case eHatchGrabStateInitialize:
			m_hatchGrab.Set(-0.4);
		
			if(m_hatchGrab.GetOutputCurrent() >= HATCH_GRAB_CURRENT_THRESHOLD)
			{
				if(++m_hatchGrabCounter == HATCH_GRAB_ITERATIONS)
				{
					if(g_rc.m_gamePieceSwitch.Get() && !g_rc.m_cargoSwitch.Get())
					{
						SetEncoderPosition(m_hatchGrab, HATCH_GRAB_HOLDING_POSITION);
						m_hatchGrabState = eHatchGrabStateAcquried;
					}
					else
					{
						SetEncoderPosition(m_hatchGrab, HATCH_GRAB_NOT_HOLDING_POSITION);
						m_hatchGrabState = eHatchGrabStateNotHolding;
					}
				}
			}

			break;

		case eHatchGrabStateAcquring:
			grabError = m_hatchGrabInitialPosition - m_hatchGrab.GetSelectedSensorPosition();
			
			if(m_hatchSliderState == eHatchSliderStateOut)
				m_hatchGrab.Set(-0.6);

			if(m_hatchGrab.GetSelectedSensorPosition() >= 310)
				m_hatchGrabState = eHatchGrabStateNotHolding;

			if(m_hatchGrab.GetOutputCurrent() >= HATCH_GRAB_CURRENT_THRESHOLD)
			{
				if(++m_hatchGrabCounter == HATCH_GRAB_ITERATIONS)
				{
					m_hatchRaiseCounter = 0;
					m_hatchGrabState = eHatchGrabStateAcquried;
					m_hatchSliderState = eHatchSliderStateMovingIn;
				}
			}
			else
				m_hatchGrabCounter = 0;

			break;

    	case eHatchGrabStateAcquried:
			m_hatchGrab.Set(-0.4);

			if(++m_hatchRaiseCounter == 50)
			{
				//g_rc.m_ladderTargetHeight = eLadderHeightReceiveHatch;
			}

			if(!g_rc.m_bCargo && g_rc.m_bAction || g_rc.m_bAbort)
			{
				m_hatchSliderState = eHatchSliderStateMovingOut;
			}

			if(m_hatchSliderState == eHatchSliderStateOut)
			{
				m_hatchGrabState = eHatchGrabStateEjecting;
			}

			break;
		
    	case eHatchGrabStateNotHolding:
			grabError = 0 - m_hatchGrab.GetSelectedSensorPosition();
			m_hatchGrab.Set(0.2);

			if(m_hatchSliderState == eHatchSliderStateOut)
				m_hatchSliderState = eHatchSliderStateMovingIn;

			if(fabs(grabError) <= 5)
				m_hatchGrabState = eHatchGrabStateWaiting;
		
			break;
		
		case eHatchGrabStateEjecting:
			grabError = -10 - m_hatchGrab.GetSelectedSensorPosition();
			m_hatchGrab.Set(0.4);

			if(fabs(grabError) <= 10)
			{
				m_hatchSliderState = eHatchSliderStateMovingIn;
				m_hatchGrabState = eHatchGrabStateWaiting;
			}
		
			break;
		
		case eHatchGrabStateWaiting:
			m_hatchGrab.Set(0);

			if(!g_rc.m_bCargo)
				g_rc.m_ladderTargetHeight = eLadderHeightGround;

			if(!g_rc.m_bCargo && g_rc.m_bAction)
				m_hatchSliderState = eHatchSliderStateMovingOut;

			if(m_hatchSliderState == eHatchSliderStateOut)
				m_hatchGrabState = eHatchGrabStateAcquring;
	
			break;
	}
}

HatchSliderState HatchControl::GetHatchMoveState()
{
	return m_hatchSliderState;
}

HatchGrabState  HatchControl::GetHatchGrabState()
{
	return m_hatchGrabState;
}

const char *HatchControl::HatchGrabStateToString(HatchGrabState hatchGrabState)
{
	switch(hatchGrabState)
	{
		case eHatchGrabStateInitialize: return "Initialize";
    	case eHatchGrabStateAcquring:	return "Acquiring";
    	case eHatchGrabStateAcquried:   return "Acquired";
    	case eHatchGrabStateNotHolding: return "Not Holding";
		case eHatchGrabStateEjecting: 	return "Ejecting";
    	case eHatchGrabStateWaiting:    return "Waiting";	
	}

	return "Unknown State";
}

const char *HatchControl::HatchSliderStateToString(HatchSliderState hatchSliderState)
{
	switch(hatchSliderState)
	{
		case eHatchSliderStateInitialize: return "Initialize";
		case eHatchSliderStateMovingIn:	  return "Moving In";
		case eHatchSliderStateIn:		  return "In";
		case eHatchSliderStateMovingOut:  return "Moving Out";
		case eHatchSliderStateOut:		  return "Out";
	}

	return "Unknown State";	
}