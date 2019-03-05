#include "CargoControl.h"
#include "..\Control\RobotControl.h"

extern RobotControl g_rc;

CargoControl::CargoControl(int leftGrabberID, int rightGrabberID, int intakeID, int captureID)
            : m_leftGrabberMotor(leftGrabberID), m_rightGrabberMotor(rightGrabberID), 
			//m_leftGrabberMotor(4, "Left Cargo Motor", true), m_rightGrabberMotor(5, "Right Cargo Motor", true),
			m_cargoCaptureTilt(62), m_cargoCaptureIntake(61),
			m_cargoState(eCargoStateHardPullIn), m_cargoCaptureState(eCargoCaptureStateInitialize)
{
		m_bCargoIntakeTestWaiting = true;
		m_bFlipped = false;

		m_cargoStateCounter = 0;
		m_captureCurrentCounter = 0;
		m_leftEncoderPosition = 0;
		m_rightEncoderPosition = 0;
		m_DICounter = 0;
}

void CargoControl::Periodic() 
{
	switch(m_cargoState)
	{
		case eCargoStateStationIntake:
			m_pneumatics.Flip(true);

			//Goto ladder height

			m_cargoState = eCargoStateHardPullIn;

			break;

		case eCargoStateHardPullIn:
			if(g_rc.m_cargoSwitch.Get())
			{	
				if(m_leftGrabberMotor.GetOutputCurrent() >= CARGO_CURRENT_THRESHOLD || m_rightGrabberMotor.GetOutputCurrent() >= CARGO_CURRENT_THRESHOLD)
				{
					if(++m_currentCounter == CARGO_CURRENT_ITERATIONS)
					{
						m_cargoState = eCargoStateSoftPullIn;
						m_cargoStateCounter = 0;
					}
				}
				else
				{
					m_currentCounter = 0;
					m_leftGrabberMotor.Set(-0.98);
					m_rightGrabberMotor.Set(1.0);
				}
			}
			else
			{
				m_cargoState = eCargoStateEmpty;
				m_cargoStateCounter = 0;
			}

			break;

		case eCargoStateSoftPullIn:
			m_leftGrabberMotor.Set(-0.05);
			m_rightGrabberMotor.Set(0.05);

			m_cargoCaptureIntake.Set(0);

			m_cargoState = eCargoStateWaitingForFlip;

			break;

		case eCargoStateWaitingForFlip:
			m_cargoState = eCargoStateFlipping;
			m_cargoStateCounter = 0;

			break;

		case eCargoStateFlipping:
			m_pneumatics.Flip(true);

			if(++m_cargoStateCounter == FLIP_TIME)
			{
				m_cargoState = eCargoStateFlipped;
			}

			break;

		case eCargoStateFlipped:
			if(g_rc.m_driveJoystick.Action()->Pressed() && g_rc.m_driveJoystick.Action()->Changed())
			{
				m_cargoState = eCargoStateEjecting;
				m_cargoStateCounter = 0;
			}
			
			break;

    	case eCargoStateEjecting:
			m_leftGrabberMotor.Set(0.98);
			m_rightGrabberMotor.Set(-1.0);

			if(++m_cargoStateCounter == EJECT_TIME)
				m_cargoState = eCargoStateEjected;

			break;

		case eCargoStateEjected:
			m_leftGrabberMotor.Set(0);
			m_rightGrabberMotor.Set(0);

			m_cargoState = eCargoStateEmpty;

			break;

		case eCargoStateEmpty:
			m_leftGrabberMotor.Set(0);
			m_rightGrabberMotor.Set(0);

			m_pneumatics.Flip(false);

			if(g_rc.m_driveJoystick.StationIntake()->Pressed() && g_rc.m_driveJoystick.StationIntake()->Changed())
			{
				m_cargoState = eCargoStateStationIntake;
				m_currentCounter = 0;
			}

			if(m_cargoCaptureState == eCargoCaptureStateUp)
			{
				if(g_rc.m_driveJoystick.Action()->Pressed() && g_rc.m_driveJoystick.Action()->Changed())
				{
					m_cargoCaptureState = eCargoCaptureStateMovingDown;
				}
			}

			break;
	}

	switch(m_cargoCaptureState)
	{
		case eCargoCaptureStateInitialize:
			m_cargoCaptureIntake.Set(0.0);

			if(m_cargoCaptureTilt.GetSelectedSensorPosition() <= -200)
			{
				m_cargoCaptureTilt.Set(0.05);
				m_cargoCaptureState = eCargoCaptureStateToReady;
			}

			
			break;

		case eCargoCaptureStateToReady:
			m_cargoCaptureTilt.Set(-0.5);

			if(m_cargoCaptureTilt.GetOutputCurrent() >= CAPTURE_TILT_CURRENT_THRESHOLD)
			{
				if(++m_captureCurrentCounter == CAPTURE_TILT_CURRENT_ITERATIONS)
				{
					m_cargoCaptureTilt.SetSelectedSensorPosition(10);
					m_cargoCaptureTilt.Set(0.5);
					m_cargoCaptureState = eCargoCaptureStateUp;
				}
			}
			else
				m_captureCurrentCounter = 0;

			break;

		case eCargoCaptureStateUp:
			m_cargoCaptureTilt.Set(-0.05);

			break;

		case eCargoCaptureStateMovingUp:
			if(m_cargoCaptureTilt.GetSelectedSensorPosition() >= -500 - 200)
				m_cargoCaptureTilt.Set(-0.3);
			else
				m_cargoCaptureTilt.Set(-1.0);

			if(fabs(m_cargoCaptureTilt.GetSelectedSensorPosition() - (-200)) <= 30)
			{
				m_cargoCaptureState = eCargoCaptureStateUp;	
			}

			break;

		case eCargoCaptureStateDown:
			m_cargoCaptureTilt.Set(0);

			if(g_rc.m_driveJoystick.Action()->Pressed() && g_rc.m_driveJoystick.Action()->Changed())
			{
				m_cargoCaptureState = eCargoCaptureStateMovingUp;
				m_cargoState = eCargoStateHardPullIn;

			}

			break;

		case eCargoCaptureStateMovingDown:
			m_cargoCaptureTilt.Set(0.5);
			m_cargoCaptureIntake.Set(1.0);

			if(m_cargoCaptureTilt.GetSelectedSensorPosition() <= -2000)
				m_cargoCaptureState = eCargoCaptureStateDown;
		
			break;
	}
}

bool CargoControl::MonitorCaptureMotor(int targetPosition, int maxError, double maxCurrent)
{
	if (m_cargoCaptureTilt.GetOutputCurrent() > maxCurrent)
		return true;

	if (abs(m_cargoCaptureTilt.GetSelectedSensorPosition(0) - targetPosition) < maxError)
		return true;

	return false;
}

CargoState CargoControl::GetCargoState()
{
	return m_cargoState;
}

const char *CargoControl::CargoStateToString(CargoState cargoState)
{
	switch(cargoState)
	{
		case eCargoStateHardPullIn:		 return "Hard Pull In";
    	case eCargoStateSoftPullIn:		 return "Soft Pull In";
    	case eCargoStateWaitingForFlip:  return "Wating For Eject";
		case eCargoStateFlipping:		 return "Flipping";
		case eCargoStateFlipped:		 return "Flipped";
    	case eCargoStateEjecting: 		 return "Ejecting";
		case eCargoStateEjected:		 return "Ejected";
    	case eCargoStateEmpty: 			 return "Empty";	
	}

	return "Unknown State";
}