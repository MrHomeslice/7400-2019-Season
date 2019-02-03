#include "CargoControl.h"
#include "..\Control\RobotControl.h"

extern RobotControl g_rc;

CargoControl::CargoControl(int leftID, int rightID, int intakeID, int captureID)
            : m_leftGrabberMotor(leftID, "Left_Grabber_Motor",true),
		        	m_rightGrabberMotor(rightID, "Right_Grabber_Motor", true),
							m_intakeMotor(intakeID, "Intake_Motor", true),
							m_cargoCaptureMotor(captureID, "Cargo_Capture_Motor", true),
              m_acquiredSwitch(0), m_intakingSwitch(1)
{
  m_cargoState = eCargoStateNull;
	m_lastCargoState = eCargoStateNull;

	m_captureState = eCargoCaptureStateUp;
	m_lastCaptureState = eCargoCaptureStateUp;

  m_ejectCounter    = 0;
  m_flippingCounter = 0;
	m_captureCounter  = 0;
}

void CargoControl::Periodic()
{
	ProcessCargoState();
	ProcessCaptureState();
}

bool CargoControl::MonitorCaptureMotor(int targetPosition, int maxError, double maxCurrent)
{
	if (m_cargoCaptureMotor.GetOutputCurrent() > maxCurrent)
		return true;

	if (abs(m_cargoCaptureMotor.GetSelectedSensorPosition(0) - targetPosition) < maxError)
		return true;

	return false;
}

void CargoControl::SetCapturePIDValues()
{
	double f = g_tc.GetDouble("Capture/F", kDefaultCaptureF);
	double p = g_tc.GetDouble("Capture/P", kDefaultCaptureP);
	double i = g_tc.GetDouble("Capture/I", kDefaultCaptureI);
	double d = g_tc.GetDouble("Capture/D", kDefaultCaptureD);

	printf("Cargo Capture P %.6f I %.6f D %.6f F %.6f\n", p, i, d, f);

	ctre::phoenix::ErrorCode err = m_cargoCaptureMotor.Config_kF(kPIDLoopIdx, f, kTimeoutMs);
	if (err != 0) printf("Cargo Capture, error %d Config_kF\n", err);

	err = m_cargoCaptureMotor.Config_kP(kPIDLoopIdx, p, kTimeoutMs);
	if (err != 0) printf("Cargo Capture, error %d Config_kP\n", err);

	err = m_cargoCaptureMotor.Config_kI(kPIDLoopIdx, i, kTimeoutMs);
	if (err != 0) printf("Cargo Capture, error %d Config_kI\n", err);

	err = m_cargoCaptureMotor.Config_kD(kPIDLoopIdx, d, kTimeoutMs);
	if (err != 0) printf("Cargo Capture, error %d Config_kD\n", err);
}

void CargoControl::ProcessCargoState()
{
	NewCargoStateCheck();

	switch (m_cargoState)
	{
		case eCargoStateNull :
		{
			EjectMotorsOff();
			IntakeMotorOff();
			RaiseCapture();

			if (g_rc.m_bAction)
			{
				printf("g_rc.m_bAction = true\n");
				SetNewCargoState(eCargoStateIntaking);
				IntakeMotorOn();
				LowerCapture();
			}

			break;
		}
		
		case eCargoStateIntaking :
		{
			if (MonitorCaptureMotor(CAPTURE_LOWER_POSITION, MAXIMUM_CAPTURE_ERROR, MAXIMUM_CAPTURE_CURRENT)) {
				printf("MonitorCaptureMotor returns true\n");
				CaptureMotorOff();
			}

			if (m_intakingSwitch.Get())
			{
				printf("m_intakingSwitch.Get() == true\n");
				IntakeMotorOff();
				ReadyCapture();
				SetNewCargoState(eCargoStateWaitingForReady);
			}

			else if(g_rc.m_bAbort)
				SetNewCargoState(eCargoStateNull);

			break;
		}

		case eCargoStateWaitingForReady :
		{
			if (MonitorCaptureMotor(CAPTURE_READY_POSITION, MAXIMUM_CAPTURE_ERROR, MAXIMUM_CAPTURE_CURRENT)) {
				printf("MonitorCaptureMotor returns true\n");
				CaptureMotorOff();
				GrabCargo();
				IntakeMotorOn();
				SetNewCargoState(eCargoStateWaitingForAcquired);
			}
			else if(g_rc.m_bAbort)
				SetNewCargoState(eCargoStateNull);

			break;
		}

		case eCargoStateWaitingForAcquired :
		{
			if (m_acquiredSwitch.Get())
			{
				IntakeMotorOff();
				EjectMotorsOff();
				SetNewCargoState(eCargoStateAquired);
			}
			else if(g_rc.m_bAbort)
				SetNewCargoState(eCargoStateNull);

			break;
		}

		case eCargoStateAquired  :
		{			
			if (g_rc.m_bFlipped)
			{
				SetNewCargoState(eCargoStateForwardFlip);
				m_flippingCounter = 0;
			}
			else if(g_rc.m_bAbort)
				SetNewCargoState(eCargoStateNull);
				
			break;
		}

		case eCargoStateForwardFlip :
		{
			m_pneumatics.Flip(g_rc.m_flippedStateValue);

			if (++m_flippingCounter == FLIP_CYLCE_COUNT)
				SetNewCargoState(eCargoStateFlipped);

			break;
		}

		case eCargoStateFlipped  :
		{
			if(g_rc.m_bAction && g_rc.IsLadderAtHeight())
			{
				SetNewCargoState(eCargoStateEjecting);
                m_ejectCounter = 0;
				EjectCargo();
			}

			break;
		}
		case eCargoStateEjecting :
		{
			
			if(++m_ejectCounter == EJECT_CYCLE_COUNT)
				SetNewCargoState(eCargoStateEjected);

			break;
		}

		case eCargoStateEjected  :
		{
			EjectMotorsOff();

			m_pneumatics.Flip(g_rc.m_flippedStateValue);

			SetNewCargoState(eCargoStateBackFlip);
            m_flippingCounter = 0;
			g_rc.m_ladderTargetHeight = eLadderHeightGround;

			break;
		}

        case eCargoStateBackFlip :
        {
            if(++m_flippingCounter == FLIP_CYLCE_COUNT)
            {
                g_rc.CargoEjected();
                SetNewCargoState(eCargoStateNull);
            }

            break;
        }
	}
}

void CargoControl::ProcessCaptureState()
{
	NewCaptureStateCheck();

	switch(m_captureState)
	{
		case eCargoCaptureStateUp :
		{
			CaptureMotorOff();

			break;
		}

		case eCargoCaptureStateMovingDown :
		{
			LowerCapture();
			
			break;
		}

		case eCargoCaptureStateDown :
		{
			CaptureMotorOff();
			
			break;
		}

		case eCargoCaptureStateMovingUp :
		{
			RaiseCapture();
			
			break;
		}
	}
}

void CargoControl::EjectMotorsOff()
{
	m_leftGrabberMotor.Set(0);
	m_rightGrabberMotor.Set(0);
}

void CargoControl::IntakeMotorOff()
{
	m_intakeMotor.Set(0);
}

void CargoControl::CaptureMotorOff()
{
	m_cargoCaptureMotor.Set(0);
}

void CargoControl::LowerCapture()
{
	m_cargoCaptureMotor.Set(ctre::phoenix::motorcontrol::ControlMode::Position, CAPTURE_LOWER_POSITION);
}

void CargoControl::RaiseCapture()
{
	m_cargoCaptureMotor.Set(ctre::phoenix::motorcontrol::ControlMode::Position, CAPTURE_RAISE_POSITION);
}

void CargoControl::ReadyCapture()
{
	m_cargoCaptureMotor.Set(ctre::phoenix::motorcontrol::ControlMode::Position, CAPTURE_READY_POSITION);
}

void CargoControl::IntakeMotorOn()
{
	m_intakeMotor.Set(0.2);
}

void CargoControl::GrabCargo()
{
    m_leftGrabberMotor.Set(0.2);
    m_rightGrabberMotor.Set(-0.2);
}

void CargoControl::EjectCargo()
{
    m_leftGrabberMotor.Set(-0.2);
    m_rightGrabberMotor.Set(0.2);
}

void CargoControl::NewCargoStateCheck()
{
	if(m_lastCargoState != m_cargoState)
	{
		printf("Cargo State: %d\n", m_cargoState);
		m_lastCargoState = m_cargoState;
	}
}

void CargoControl::NewCaptureStateCheck()
{
	if(m_lastCaptureState != m_captureState)
	{
		printf("Capture State: %d\n", m_captureState);
		m_lastCaptureState = m_captureState;
	}
}

void CargoControl::SetNewCargoState(CargoState state)
{
	m_cargoState = state;
	printf("New State: %s\n", CargoStateToString());
}

void CargoControl::SetNewCaptureState(CargoCaptureState state)
{
	m_captureState = state;
	printf("New State: %s\n", CaptureStateToString());
}

const char* CargoControl::CargoStateToString()
{
	switch(m_cargoState)
	{
		case eCargoStateNull				: return "Cargo State: Null";
		case eCargoStateIntaking    : return "Cargo State: Intaking";
		case eCargoStateAquiring 		: return "Cargo State: Aquiring";
		case eCargoStateWaitingForReady : return "Cargo State: Waiting For Ready";
		case eCargoStateWaitingForAcquired : return "Cargo State: Waiting For Acquired";
		case eCargoStateForwardFlip : return "Cargo State: Forward Flip";
		case eCargoStateAquired 		: return "Cargo State: Aquired";
		case eCargoStateFlipped 		: return "Cargo State: Flipped";
		case eCargoStateEjecting		: return "Cargo State: Ejecting";
		case eCargoStateEjected  		: return "Cargo State: Ejected";
		case eCargoStateBackFlip 		: return "Cargo State: Back Flip";
	}
	return "Unkown Cargo State";
}

const char* CargoControl::CaptureStateToString()
{
	switch(m_captureState)
	{
		case eCargoCaptureStateUp	      : return "Capture State: Up";
		case eCargoCaptureStateMovingDown : return "Capture State: Moving Down";
		case eCargoCaptureStateDown		  : return "Capture State: Down";
		case eCargoCaptureStateMovingUp   : return "Capture State: Moving Up";
	}
	return "Unkown Capture State";
}