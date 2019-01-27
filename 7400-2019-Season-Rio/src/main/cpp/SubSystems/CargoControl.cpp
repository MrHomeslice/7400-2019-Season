#include "CargoControl.h"
#include "..\Control\RobotControl.h"

extern RobotControl g_rc;

CargoControl::CargoControl(int leftID, int rightID)
              : m_left(leftID),
		        m_right(rightID),
                m_acquiredSwitch(0)
{
    m_cargoState = eCargoStateNull;
	m_lastCargoState = eCargoStateNull;

    m_ejectCounter = 0;
    m_flippingCounter = 0;
}

void CargoControl::Aquire()
{
    m_left.Set(-0.2);
    m_right.Set(0.2);
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
			if(g_rc.m_bAction)
			{
				m_cargoState = eCargoStateAquiring;
			}

			break;
		}
		case eCargoStateAquiring :
		{			
			if(m_acquiredSwitch.Get()) 
			{
				m_cargoState = eCargoStateAquired;
			}

			if(g_rc.m_bAction)
			{
				m_cargoState = eCargoStateNull;
			}

			break;
		}
		case eCargoStateAquired  :
		{
			if(g_rc.m_bFlipped)
			{
				m_cargoState = eCargoStateFlipped;
			}

			break;
		}
		case eCargoStateFlipped  :
		{
            m_pneumatics.Flip(g_rc.m_driveJoyStick.ElevatorFlip()->Value());

			if(g_rc.m_bAction)
			{
				m_cargoState = eCargoStateEjecting;
                m_ejectCounter = 0;
			}

			break;
		}
		case eCargoStateEjecting :
		{
			m_ejectCounter++;
			if(m_ejectCounter == EJECT_CYCLE_COUNT)
				m_cargoState = eCargoStateEjected;
			
			break;
		}

		case eCargoStateEjected  :
		{
			m_pneumatics.Flip(g_rc.m_driveJoyStick.ElevatorFlip()->Value());

			m_cargoState = eCargoStateFlipping;
            m_flippingCounter = 0;

			break;
		}

        case eCargoStateFlipping :
        {
            m_flippingCounter++;
            if(m_flippingCounter == FLIP_CYLCE_COUNT)
            {
                g_rc.CargoEjected();
                m_cargoState = eCargoStateNull;
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