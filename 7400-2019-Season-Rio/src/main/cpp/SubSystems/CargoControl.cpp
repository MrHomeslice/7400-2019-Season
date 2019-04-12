#include "CargoControl.h"
#include "..\Control\RobotControl.h"

void SetEncoderPosition(WPI_TalonSRX &talon, int position);

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
		m_hardPullInCounter = 0;
}

void CargoControl::Initialize(bool bFlip)
{
	m_leftGrabberMotor.SetSafetyEnabled(false);
	m_rightGrabberMotor.SetSafetyEnabled(false);

	m_cargoState = eCargoStateInitialize;
	m_cargoCaptureState = eCargoCaptureStateInitialize;

	m_cargoStateCounter = 0;
	m_currentCounter = 0;

	if(bFlip)
		m_pneumatics.Flip(false);

	m_cargoCaptureTilt.Set(-0.3); //Capture starts to go down
	SetEncoderPosition(m_cargoCaptureTilt, 0);
}

void CargoControl::Periodic() 
{
	static int oldState = -100;
	if(oldState != m_cargoCaptureState)
	{
		oldState = m_cargoCaptureState;
	}

	if(g_rc.m_bCargo && g_rc.m_driveJoystick.Flip()->Pressed() && g_rc.m_driveJoystick.Flip()->Changed()) //manual pneumatics mode, must be in cargo
	{
		printf("FLOPPING\n");
		m_pneumatics.Flop();
	}

	switch(m_cargoCaptureState)
	{
		case eCargoCaptureStateInitialize:
			m_cargoCaptureIntake.Set(0.0); 

			//m_cargoCaptureTilt is at Speed -0.3 from Initialize()
			if(m_cargoCaptureTilt.GetSelectedSensorPosition() >= 500) //moves capture down
			{
				m_cargoCaptureState = eCargoCaptureStateToReady;
			}

			break;

		case eCargoCaptureStateToReady:
			m_cargoCaptureTilt.Set(0.5); //moves capture up

			if(m_cargoCaptureTilt.GetOutputCurrent() >= CAPTURE_TILT_CURRENT_THRESHOLD) //Tests if capture has hit the block and reaches a current
			{
				if(++m_captureCurrentCounter == CAPTURE_TILT_CURRENT_ITERATIONS)
				{
					SetEncoderPosition(m_cargoCaptureTilt, 10); //Set Encoder Position of Capture to 10 (really 0 because of momentum) at the block
					m_cargoCaptureState = eCargoCaptureStateUp;
				}
			}
			else
				m_captureCurrentCounter = 0;

			break;

		case eCargoCaptureStateUp:
			m_cargoCaptureTilt.Set(0.05); //Set speed of tilt to hold it up so it does not fall

			break;

		case eCargoCaptureStateMovingUp:
			if(m_cargoCaptureTilt.GetSelectedSensorPosition() <= 500) //lowers speed as capture reaches closer to the top
				m_cargoCaptureTilt.Set(0.3);
			else
				m_cargoCaptureTilt.Set(1.0); //move capture up

			if(fabs(m_cargoCaptureTilt.GetSelectedSensorPosition()) <= 150) //Sets state to up when it gets close to up.
			{
				m_cargoCaptureState = eCargoCaptureStateUp;
			}

			if(m_cargoCaptureTilt.GetOutputCurrent() >= CAPTURE_TILT_CURRENT_THRESHOLD) //sets state to up if a current limit is hit
			{
				if(++m_captureCurrentCounter == CAPTURE_TILT_CURRENT_ITERATIONS)
					m_cargoCaptureState = eCargoCaptureStateUp;
			}

			break;

		case eCargoCaptureStateDown:
			m_cargoCaptureTilt.Set(0);

			if(g_rc.m_bCargo && g_rc.m_bAction) //must be in cargo mode, capture starts to move up and compliant wheels start pulling in when action button is pressed
			{
				m_cargoCaptureState = eCargoCaptureStateMovingUp;
				m_cargoState = eCargoStateHardPullIn;
			}

			if(g_rc.m_bAbort)
			{
				m_cargoCaptureState = eCargoCaptureStateMovingUp;
				m_cargoCaptureIntake.Set(0.0);
			}

			break;

		case eCargoCaptureStateMovingDown:
			g_rc.m_ladderTargetHeight = eLadderHeightReceiveCargo; //Raises ladder to height to recieve cargo
			printf("**MOVING DOWN**\n");

			m_cargoCaptureTilt.Set(-0.5); //Moves capture down
			m_cargoCaptureIntake.Set(1.0); //Turn on intake wheels

			if(m_cargoCaptureTilt.GetSelectedSensorPosition() >= CAPTURE_TILT_DOWN_POSITION) //Check to see if the Capure is past the Down Encoder Position 
			{
				m_cargoCaptureState = eCargoCaptureStateDown;
			}

			if(g_rc.m_bCargo && g_rc.m_bAction) //must be in cargo mode, capture starts to move up and compliant wheels start pulling in when action button is pressed
			{
				m_cargoCaptureState = eCargoCaptureStateMovingUp;
				m_cargoState = eCargoStateHardPullIn;
			}

			if(g_rc.m_bAbort) //Capture moves up intake wheels stop
			{
				m_cargoCaptureState = eCargoCaptureStateMovingUp;
				m_cargoCaptureIntake.Set(0.0);
			}
		
			break;
	}

	switch(m_cargoState)
	{
		case eCargoStateInitialize:
			if(g_rc.m_gamePieceSwitch.Get() && g_rc.m_cargoSwitch.Get()) //Pull in the ball at the start if the game piece switch is true and the cargo switch is true
			{
				m_cargoState = eCargoStateHardPullIn;
			}
			else
			{
				m_cargoState = eCargoStateEmpty;
			}
		
			break;

		case eCargoStateStationIntake:
			if(++m_flippingCounter == FLIP_TIME) //waits for pneumatics to flip
			{
				g_rc.m_ladderTargetHeight = g_rc.GetCargoShipCargoHeight(); //Cargo Ship height and loading station height are the same (still test on practive field anyway)
				m_bChangeHeight = true;
			}

			if(m_bChangeHeight) //Compliant wheels start spinning when the ladder starts moving up
			{
				m_cargoState = eCargoStateHardPullIn;
				m_bChangeHeight = false;
			}

			break;

		case eCargoStateHardPullIn:
			//Goes to soft pull in if either the left or right pair of wheels hit the current limit
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
				m_leftGrabberMotor.Set(-0.98); //Wheels spin in
				m_rightGrabberMotor.Set(1.0); //Wheels spin in
			}

			if(m_lastCargoState == eCargoStateStationIntake && g_rc.m_bAbort) //Special abort if intaking from station
			{
				g_rc.m_ladderTargetHeight = g_rc.GetGroundHeight();
				
				if(g_rc.GetLadderPosition() <= 10)
					m_cargoState = eCargoStateEmpty;
			}

			if(g_rc.m_bAbort) //Abort hard pull in
			{
				m_cargoCaptureState = eCargoCaptureStateMovingUp;
				m_cargoCaptureIntake.Set(0.0);

				m_cargoState = eCargoStateEmpty;
			}

			if(++m_hardPullInCounter == HARD_PULL_IN_TIME) //Aborts the hard pull in after set time
			{
				m_cargoCaptureState = eCargoCaptureStateMovingUp;
				m_cargoCaptureIntake.Set(0.0);

				m_cargoState = eCargoStateEmpty;
				m_hardPullInCounter = 0;
			}

			break;

		case eCargoStateSoftPullIn:
			m_leftGrabberMotor.Set(-0.1); //wheels spin to hold cargo in
			m_rightGrabberMotor.Set(0.1); //wheels spin to hold cargo in

			m_cargoCaptureIntake.Set(0);

			m_cargoState = eCargoStateFlipping;
			m_cargoStateCounter = 0;

			break;

		case eCargoStateFlipping:
			if(g_rc.m_hatchControl.GetHatchGrabState() == eHatchGrabStateWaiting) //Pneumatics may only flip if there is NO hatch acquried
			{
				m_pneumatics.Flip(true);
				m_bFlipped = true;
			}

			if(m_bFlipped)
			{
				if(++m_cargoStateCounter >= FLIP_TIME) //waits until the pneumatics has completed the flip
				{	
					m_cargoState = eCargoStateFlipped;
					g_rc.m_ladderTargetHeight = eLadderHeightGround; //moves ladder to ground height
					m_bFlipped = false;
				}
			}

			break;

		case eCargoStateFlipped:
			if(g_rc.m_bCargo && g_rc.m_bAction || g_rc.m_bAbort) //Must be in cargo mode to eject cargo. Abort will also eject cargo
			{
				m_cargoState = eCargoStateEjecting;
				m_cargoStateCounter = 0;
			}
			
			break;

    	case eCargoStateEjecting:
			m_leftGrabberMotor.Set(0.98); //Compliant wheels spin out
			m_rightGrabberMotor.Set(-1.0); //Compliant wheels spin out

			if(++m_cargoStateCounter == EJECT_TIME)
			{
				m_pneumatics.Flip(false); //Flips pneumatics back
				m_cargoState = eCargoStateEjected;
			}

			break;

		case eCargoStateEjected:
			m_leftGrabberMotor.Set(0); //Stop compliant wheels
			m_rightGrabberMotor.Set(0);

			g_rc.m_ladderTargetHeight = g_rc.GetGroundHeight();

			m_cargoState = eCargoStateEmpty;

			break;

		case eCargoStateEmpty:
			m_leftGrabberMotor.Set(0); //Stop compliant whee;s
			m_rightGrabberMotor.Set(0);

			//Checking if the Station Intake is pressed in Cargo mode
			if(g_rc.m_bCargo && g_rc.m_driveJoystick.StationIntake()->Pressed() && g_rc.m_driveJoystick.StationIntake()->Changed())
			{
				m_pneumatics.Flip(true); //Flip pneumatics out
				m_cargoState = eCargoStateStationIntake;
				m_flippingCounter = 0;
				m_currentCounter = 0;
			}

			if(m_cargoCaptureState == eCargoCaptureStateUp)
			{
				if(g_rc.m_bCargo && g_rc.m_bAction) //Checks if action button is clicked, capture must be up
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

/*
* @return Current cargo state
*/
CargoState CargoControl::GetCargoState()
{
	return m_cargoState;
}

/*
* @return Current cargo capture state
*/
CargoCaptureState CargoControl::GetCargoCaptureState()
{
	return m_cargoCaptureState;
}

/*
* @return Cargo State as a string
* @return "Unknown State" if not found
*/
const char *CargoControl::CargoStateToString(CargoState cargoState)
{
	switch(cargoState)
	{
		case eCargoStateHardPullIn:	return "Hard Pull In";
    	case eCargoStateSoftPullIn:	return "Soft Pull In";
		case eCargoStateFlipping:	return "Flipping";
		case eCargoStateFlipped:	return "Flipped";
    	case eCargoStateEjecting: 	return "Ejecting";
		case eCargoStateEjected:	return "Ejected";
    	case eCargoStateEmpty: 		return "Empty";	
	}

	return "Unknown State";
}