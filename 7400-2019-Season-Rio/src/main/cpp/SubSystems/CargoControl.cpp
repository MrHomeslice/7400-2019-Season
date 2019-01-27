#include "CargoControl.h"
#include "..\Control\RobotControl.h"

extern RobotControl g_rc;

CargoControl::CargoControl(int leftID, int rightID, int intakeID)
              : m_left(leftID),
		        m_right(rightID),
				m_intakeMotor(intakeID),
                m_acquiredSwitch(0)
{
    m_cargoState = eCargoStateNull;
	m_lastCargoState = eCargoStateNull;

    m_ejectCounter = 0;
    m_flippingCounter = 0;
}

void CargoControl::MotorsOff()
{
	m_left.Set(0);
	m_right.Set(0);
	m_intakeMotor.Set(0);
}

void CargoControl::Aquire()
{
	m_intakeMotor.Set(0.2);
}

void CargoControl::Eject()
{
    m_left.Set(0.2);
    m_right.Set(-0.2);
}

void CargoControl::ProcessCargoState()
{
	NewStateCheck();

	switch (m_cargoState)
	{
		case eCargoStateNull     :
		{
			MotorsOff();

			if(g_rc.m_bAction)
				SetNewState(eCargoStateAquiring);

			break;
		}
		case eCargoStateAquiring :
		{
			Aquire();

			if(m_acquiredSwitch.Get())
			{
				SetNewState(eCargoStateAquired);
				m_flippingCounter = 0;
			}

			else if(g_rc.m_bAction) //Abort
				SetNewState(eCargoStateNull);

			break;
		}
		case eCargoStateAquired  :
		{
			MotorsOff();
			
			if(g_rc.m_bFlipped)
			{
				SetNewState(eCargoStateForwardFlip);
				m_flippingCounter = 0;
			}

			break;
		}

		case eCargoStateForwardFlip :
		{
			m_pneumatics.Flip(g_rc.m_flippedStateValue);

			if(++m_flippingCounter == FLIP_CYLCE_COUNT)
				

			break;
		}

		case eCargoStateFlipped  :
		{
			if(g_rc.m_bAction)
			{
				SetNewState(eCargoStateEjecting);
                m_ejectCounter = 0;
				Eject();
			}

			break;
		}
		case eCargoStateEjecting :
		{
			
			if(++m_ejectCounter == EJECT_CYCLE_COUNT)
				SetNewState(eCargoStateEjected);

			break;
		}

		case eCargoStateEjected  :
		{
			MotorsOff();

			m_pneumatics.Flip(g_rc.m_flippedStateValue);

			SetNewState(eCargoStateBackFlip);
            m_flippingCounter = 0;

			break;
		}

        case eCargoStateBackFlip :
        {
            if(++m_flippingCounter == FLIP_CYLCE_COUNT)
            {
                g_rc.CargoEjected();
                SetNewState(eCargoStateNull);
            }

            break;
        }
	}
}

void CargoControl::NewStateCheck()
{
	if(m_lastCargoState != m_cargoState)
	{
		printf("State: %d\n", m_cargoState);
		m_lastCargoState = m_cargoState;
	}
}

void CargoControl::SetNewState(CargoState state)
{
	m_cargoState = state;
	printf("New State: %s\n", StateToString());
}

const char* CargoControl::StateToString()
{
	switch(m_cargoState)
	{
		case eCargoStateNull		: return "Cargo State: Null";
		case eCargoStateAquiring 	: return "Cargo State: Aquiring";
		case eCargoStateForwardFlip : return "Cargo State: Forward Flip";
		case eCargoStateAquired 	: return "Cargo State: Aquired";
		case eCargoStateFlipped 	: return "Cargo State: Flipped";
		case eCargoStateEjecting	: return "Cargo State: Ejecting";
		case eCargoStateEjected  	: return "Cargo State: Ejected";
		case eCargoStateBackFlip 	: return "Cargo State: Back Flip";
	}
	return "Unkown State";
}