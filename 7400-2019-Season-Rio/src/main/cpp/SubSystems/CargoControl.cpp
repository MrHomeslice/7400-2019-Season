#include "CargoControl.h"
#include "..\Control\RobotControl.h"

extern RobotControl g_rc;

CargoControl::CargoControl(int leftGrabberID, int rightGrabberID, int tiltID, int intakeID)
            : m_leftGrabberMotor(leftGrabberID), m_rightGrabberMotor(rightGrabberID), 
			//m_leftGrabberMotor(4, "Left Cargo Motor", true), m_rightGrabberMotor(5, "Right Cargo Motor", true),
			m_cargoCaptureTilt(tiltID), m_cargoCaptureIntake(intakeID),
			m_cargoState(eCargoStateInitialize), m_lastCargoState(eCargoStateInitialize),
			 m_cargoCaptureState(eCargoCaptureStateInitialize)
{
		m_bCargoIntakeTestWaiting = true;
		m_bFlipped = false;
		m_bChangeHeight = false;

		m_cargoStateCounter = 0;
		m_captureCurrentCounter = 0;
		m_leftEncoderPosition = 0;
		m_rightEncoderPosition = 0;
		m_DICounter = 0;
		m_printCounter = 0;
}

void CargoControl::Initialize(bool bAuton)
{
	m_leftGrabberMotor.SetSafetyEnabled(false);
	m_rightGrabberMotor.SetSafetyEnabled(false);

	m_cargoState = eCargoStateInitialize;
	m_cargoCaptureState = eCargoCaptureStateInitialize;

	m_cargoStateCounter = 0;
	m_currentCounter = 0;

	m_bAuton = bAuton;

	if(m_bAuton)
	{
		m_pneumatics.Flip(false);
	}
	

	m_cargoCaptureTilt.Set(-0.3);
	m_cargoCaptureTilt.SetSelectedSensorPosition(0);
}

void CargoControl::Periodic() 
{
	static int oldState = -100;
	if(oldState != m_cargoCaptureState)
	{
		//printf("%d\n\n", m_cargoCaptureState);
		oldState = m_cargoCaptureState;
	}

	if(g_rc.m_bCargo && g_rc.m_driveJoystick.Flip()->Pressed() && g_rc.m_driveJoystick.Flip()->Changed())
	{
		printf("FLOPPING\n");
		m_pneumatics.Flop();
	}

	switch(m_cargoCaptureState)
	{
		case eCargoCaptureStateInitialize:
			m_cargoCaptureIntake.Set(0.0);

			if(m_cargoCaptureTilt.GetSelectedSensorPosition() >= 500) //200
			{
				m_cargoCaptureState = eCargoCaptureStateToReady;
			}

			break;

		case eCargoCaptureStateToReady:
			m_cargoCaptureTilt.Set(0.5); //0.5

			if(m_cargoCaptureTilt.GetOutputCurrent() >= CAPTURE_TILT_CURRENT_THRESHOLD)
			{
				if(++m_captureCurrentCounter == CAPTURE_TILT_CURRENT_ITERATIONS)
				{
					m_cargoCaptureTilt.SetSelectedSensorPosition(10);
					m_cargoCaptureTilt.Set(-0.5); //-0.5
					m_cargoCaptureState = eCargoCaptureStateUp;
				}
			}
			else
				m_captureCurrentCounter = 0;

			break;

		case eCargoCaptureStateUp:
			m_cargoCaptureTilt.Set(0.05);

			break;

		case eCargoCaptureStateMovingUp:
			if(m_cargoCaptureTilt.GetSelectedSensorPosition() <= 500) //500
				m_cargoCaptureTilt.Set(0.3); //0.3
			else
				m_cargoCaptureTilt.Set(1.0); //1.0

			if(fabs(m_cargoCaptureTilt.GetSelectedSensorPosition()) <= 200)
			{
				m_cargoCaptureState = eCargoCaptureStateUp;
			}

			if(m_cargoCaptureTilt.GetOutputCurrent() >= CAPTURE_TILT_CURRENT_THRESHOLD)
			{
				if(++m_captureCurrentCounter == CAPTURE_TILT_CURRENT_ITERATIONS)
					m_cargoCaptureState = eCargoCaptureStateUp;
			}

			break;

		case eCargoCaptureStateDown:
			m_cargoCaptureTilt.Set(0);

			if(g_rc.m_bCargo && g_rc.m_bAction)
			{
				m_cargoCaptureState = eCargoCaptureStateMovingUp;
				m_cargoState = eCargoStateHardPullIn;
			}

			if(g_rc.m_bAbort)
			{
				g_rc.m_ladderTargetHeight = eLadderHeightGround;
				m_cargoCaptureState = eCargoCaptureStateMovingUp;
				m_cargoCaptureIntake.Set(0.0);
			}

			break;

		case eCargoCaptureStateMovingDown:
			//printf("Moving Down\n");

			g_rc.m_ladderTargetHeight = eLadderHeightReceiveCargo;
			printf("**MOVING DOWN**\n");

			m_cargoCaptureTilt.Set(-0.5); //-0.5
			m_cargoCaptureIntake.Set(1.0);

			if(m_cargoCaptureTilt.GetSelectedSensorPosition() >= CAPTURE_TILT_DOWN_POSITION)
			{
			//	printf("***DOWN***\n");
				m_cargoCaptureState = eCargoCaptureStateDown;
			}

			if(g_rc.m_bCargo && g_rc.m_bAction)
			{
			//	printf("***ACTION***\n");
				m_cargoCaptureState = eCargoCaptureStateMovingUp;
				m_cargoState = eCargoStateHardPullIn;
			}

			if(g_rc.m_bAbort)
			{
			//	printf("***ABORTING***\n");
			g_rc.m_ladderTargetHeight = eLadderHeightGround;
				m_cargoCaptureState = eCargoCaptureStateMovingUp;
				m_cargoCaptureIntake.Set(0.0);
			}
		
			break;
	}

	switch(m_cargoState)
	{
		case eCargoStateInitialize:
			if(g_rc.m_cargoSwitch.Get() && m_bAuton)
			{
				m_cargoState = eCargoStateHardPullIn;
			}
			else
			{
				m_cargoState = eCargoStateEmpty;
			}
		
			break;

		case eCargoStateStationIntake:
			

			if(++m_flippingCounter == FLIP_TIME)
			{
				g_rc.m_ladderTargetHeight = g_rc.GetCargoShipCargoHeight();
				m_bChangeHeight = true;
			}

			if(m_bChangeHeight)
			{
				m_cargoState = eCargoStateHardPullIn;
				m_bChangeHeight = false;
			}

			break;

		case eCargoStateHardPullIn:
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

			if(m_lastCargoState == eCargoStateStationIntake && g_rc.m_bAbort)
			{
				g_rc.m_ladderTargetHeight = g_rc.GetGroundHeight();
				
				if(g_rc.GetLadderPosition() <= 10)
				{
					m_cargoState = eCargoStateEmpty;
				}
			}

			break;

		case eCargoStateSoftPullIn:
			m_leftGrabberMotor.Set(-0.1);
			m_rightGrabberMotor.Set(0.1);

			m_cargoCaptureIntake.Set(0);

			m_cargoState = eCargoStateFlipping;
			m_cargoStateCounter = 0;

			break;

		case eCargoStateFlipping:
			if(g_rc.m_hatchControl.GetHatchGrabState() == eHatchGrabStateWaiting)
			{
				m_pneumatics.Flip(true);
				m_bFlipped = true;
			}

			if(++m_cargoStateCounter >= FLIP_TIME && m_bFlipped)
			{
				m_cargoState = eCargoStateFlipped;
				g_rc.m_ladderTargetHeight = eLadderHeightGround;
				m_bFlipped = false;
			}

			break;

		case eCargoStateFlipped:
			if(g_rc.m_bCargo && g_rc.m_bAction || g_rc.m_bAbort)
			{
				m_cargoState = eCargoStateEjecting;
				m_cargoStateCounter = 0;
			}
			
			break;

    	case eCargoStateEjecting:
			m_leftGrabberMotor.Set(0.98);
			m_rightGrabberMotor.Set(-1.0);

			if(++m_cargoStateCounter == EJECT_TIME)
			{
				m_pneumatics.Flip(false);
				m_cargoState = eCargoStateEjected;
			}

			break;

		case eCargoStateEjected:
			m_leftGrabberMotor.Set(0);
			m_rightGrabberMotor.Set(0);

			g_rc.m_ladderTargetHeight = g_rc.GetGroundHeight();

			m_cargoState = eCargoStateEmpty;

			break;

		case eCargoStateEmpty:
			m_leftGrabberMotor.Set(0);
			m_rightGrabberMotor.Set(0);

			if(g_rc.m_bCargo && g_rc.m_driveJoystick.StationIntake()->Pressed() && g_rc.m_driveJoystick.StationIntake()->Changed())
			{
				m_pneumatics.Flip(true);
				m_cargoState = eCargoStateStationIntake;
				m_flippingCounter = 0;
				m_currentCounter = 0;
			}

			if(m_cargoCaptureState == eCargoCaptureStateUp)
			{
				//printf("**%d %d**\n", g_rc.m_bCargo, g_rc.m_bAction);
				if(g_rc.m_bCargo && g_rc.m_bAction)
				{
					m_cargoCaptureState = eCargoCaptureStateMovingDown;
				}
			}

			break;
	}

	

	if(m_lastCargoState != m_cargoState)
		m_lastCargoState = m_cargoState;
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

CargoCaptureState CargoControl::GetCargoCaptureState()
{
	return m_cargoCaptureState;
}

const char *CargoControl::CargoStateToString(CargoState cargoState)
{
	switch(cargoState)
	{
		case eCargoStateHardPullIn:		 return "Hard Pull In";
    	case eCargoStateSoftPullIn:		 return "Soft Pull In";
		case eCargoStateFlipping:		 return "Flipping";
		case eCargoStateFlipped:		 return "Flipped";
    	case eCargoStateEjecting: 		 return "Ejecting";
		case eCargoStateEjected:		 return "Ejected";
    	case eCargoStateEmpty: 			 return "Empty";	
	}

	return "Unknown State";
}